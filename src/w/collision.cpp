// The collisions of bobs and sprites (SyCall Bob Col, Sprite Col, Col):
// the rectangles of two images first, then the blitter ANDs their masks
// where they cross and says whether anything is left. The results are
// bits in T_TColl, one per bob or sprite number.
//
// AMAL tests the collisions too: Bob Col and Sprite Col only while it runs
// from Synchro (in the program's task), Col also in the VBL, so Col takes
// the data zone as an argument.

#include "w/w.h"

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::w;

namespace {

constexpr uint16_t EcYBase = 0x1000;
constexpr uint32_t ToOther = 0x80000000;        // d2: bobs to sprites or sprites to bobs
constexpr uint16_t BlitZero = 1 << 13;          // DMACONR BZERO

// An image where it is: its mask, its descriptor, its rectangle
struct Placed {
    const uint8_t *mask, *image;
    int16_t x1, y1, x2, y2;
};

// The image n of the bank placed with its hot spot at x,y. False if there
// is no such image or it has no mask.
bool place(const uint8_t *bank, uint16_t n, int16_t x, int16_t y, Placed &p)
{
    n &= 0x3FFF;
    if (!n || n > peek_w(bank))
        return false;
    const uint8_t *entry = bank + 2 + (n - 1) * 8;
    if (int32_t(peek_l(entry + 4)) <= 0)
        return false;
    p.mask = ptr<uint8_t>(peek_l(entry + 4));
    p.image = ptr<uint8_t>(peek_l(entry));
    if (!p.image)
        return false;
    // (the hot spot's x is 14 bits, signed)
    p.x1 = int16_t(x - (int16_t(peek_w(p.image + 6) << 2) >> 2));
    p.y1 = int16_t(y - peek_w(p.image + 8));
    p.x2 = int16_t(p.x1 + peek_w(p.image) * 16);
    p.y2 = int16_t(p.y1 + peek_w(p.image + 2));
    return true;
}

// ColRout: does image n at x,y touch the image of source?
bool collide(const uint8_t *bank, const Placed &source, int16_t x, int16_t y, uint16_t n)
{
    Placed other;
    if (!place(bank, n, x, y, other))
        return false;
    if (other.x2 <= source.x1 || other.x1 >= source.x2 || other.y2 <= source.y1 || other.y1 >= source.y2)
        return false;
    // a the one more on the left, b the other
    const Placed *a = &other, *b = &source;
    if (b->x1 < a->x1) {
        a = &source;
        b = &other;
    }
    int16_t skip_a = 0, skip_b = 0;
    if (a->y1 < b->y1)
        skip_a = b->y1 - a->y1;
    else
        skip_b = a->y1 - b->y1;
    int16_t height = int16_t((a->y2 < b->y2 ? a->y2 : b->y2) - (a->y1 > b->y1 ? a->y1 : b->y1));
    if (height <= 0)
        return false;
    uint16_t dx = b->x1 - a->x1;
    uint16_t shift = dx & 15;
    uint16_t words = uint16_t((a->x2 < b->x2 ? a->x2 : b->x2) - b->x1) >> 4;
    if (shift)
        words++;
    uint16_t bytes = words * 2;
    uint16_t row_a = peek_w(a->image) * 2, row_b = peek_w(b->image) * 2;
    // a's mask from the column where b starts, b's shifted onto it (32-bit
    // offsets: the original's were 16-bit, wrong for tall images)
    volatile struct Custom &c = chips();
    c.bltcon1 = uint16_t(shift << 12);
    c.bltcon0 = 0x0CC0;
    c.bltamod = row_a - bytes;
    c.bltbmod = row_b - bytes;
    c.bltbpt = const_cast<uint8_t *>(b->mask + 4 + uint32_t(row_b) * uint16_t(skip_b));
    c.bltapt = const_cast<uint8_t *>(a->mask + 4 + uint32_t(row_a) * uint16_t(skip_a) + (dx >> 4) * 2);
    c.bltalwm = 0xFFFF;
    c.bltafwm = 0xFFFF >> shift;
    c.bltsize = uint16_t(height << 6 | words);
    hal::wait_blit();
    return !(c.dmaconr & BlitZero);
}

// CXyS: screen to hardware coordinates
void to_hardware(const Screen *s, int16_t &x, int16_t &y)
{
    if (s->EcCon0 & 0x8000)
        x >>= 1;
    x += s->EcWX;
    if (s->EcCon0 & 4)
        y >>= 1;
    y = int16_t(y + s->EcWY - EcYBase);
}

void mark(WData &d, uint16_t n)
{
    reinterpret_cast<uint8_t *>(d.TColl)[(n & 0xFF) >> 3] |= 1 << (n & 7);
}

// The sprites first to last but except against source; true if any touches
bool against_sprites(WData &d, const uint8_t *bank, const Placed &source, uint16_t except, uint16_t first, uint16_t last)
{
    bool hit = false;
    if (last >= 64 || last < first)
        return false;
    for (uint16_t n = first; n <= last; n++) {
        const uint16_t *s = d.HsTAct + n * 4;
        if (n == except || int16_t(s[0]) < 0 || int16_t(s[3]) <= 0)
            continue;
        if (collide(bank, source, s[1], s[2], s[3])) {
            mark(d, n);
            hit = true;
        }
    }
    return hit;
}

// BbColl: bob d1 against the bobs (of its screen) d2 to d3, against the
// sprites if bit 31 of d2 is set. -1 if any touches.
int32_t bob_collisions(WData &d, uint16_t number, uint32_t first, uint16_t last)
{
    const uint8_t *bank = ptr<uint8_t>(d.SprBank);
    amos::Bob *bob = nullptr;
    for (uint32_t b = d.BbDeb; b; b = ptr<amos::Bob>(b)->BbNext) {
        if (ptr<amos::Bob>(b)->BbNb >= number) {
            if (ptr<amos::Bob>(b)->BbNb == number)
                bob = ptr<amos::Bob>(b);
            break;
        }
    }
    Placed source;
    if (!bank || !bob || int8_t(bob->BbAct >> 8) < 0 || !place(bank, bob->BbI, bob->BbX, bob->BbY, source))
        return 0;
    if (first & ToOther) {
        int16_t x = source.x1, y = source.y1;
        to_hardware(ptr<Screen>(bob->BbEc), x, y);
        source.x2 += x - source.x1;
        source.y2 += y - source.y1;
        source.x1 = x;
        source.y1 = y;
        return against_sprites(d, bank, source, 0xFFFF, uint16_t(first), last) ? -1 : 0;
    }
    bool hit = false;
    for (uint32_t a = d.BbDeb; a; a = ptr<amos::Bob>(a)->BbNext) {
        const amos::Bob *b = ptr<amos::Bob>(a);
        if (b->BbNb < uint16_t(first))
            continue;
        if (b->BbNb > last)
            break;
        if (b->BbNb == number || b->BbEc != bob->BbEc || int8_t(b->BbAct >> 8) < 0)
            continue;
        if (collide(bank, source, b->BbX, b->BbY, b->BbI)) {
            mark(d, b->BbNb);
            hit = true;
        }
    }
    return hit ? -1 : 0;
}

// SpColl: sprite d1 against the sprites d2 to d3, against the bobs if bit
// 31 of d2 is set. -1 if any touches.
int32_t sprite_collisions(WData &d, uint16_t number, uint32_t first, uint16_t last)
{
    const uint8_t *bank = ptr<uint8_t>(d.SprBank);
    if (!bank || number >= 64)
        return 0;
    const uint16_t *s = d.HsTAct + number * 4;
    Placed source;
    if (!place(bank, s[3], s[1], s[2], source))
        return 0;
    if (!(first & ToOther))
        return against_sprites(d, bank, source, number, uint16_t(first), last) ? -1 : 0;
    bool hit = false;
    // (the original walked the list from address 0 if there were no bobs)
    for (uint32_t a = d.BbDeb; a; a = ptr<amos::Bob>(a)->BbNext) {
        const amos::Bob *b = ptr<amos::Bob>(a);
        if (b->BbNb < uint16_t(first))
            continue;
        if (b->BbNb > last)
            break;
        if (int8_t(b->BbAct >> 8) < 0)
            continue;
        int16_t x = b->BbX, y = b->BbY;
        to_hardware(ptr<Screen>(b->BbEc), x, y);
        if (collide(bank, source, x, y, b->BbI)) {
            mark(d, b->BbNb);
            hit = true;
        }
    }
    return hit ? -1 : 0;
}

// The collisions with the blitter owned, T_TColl cleared first
int32_t collisions(AllRegs *r, int32_t (*test)(WData &, uint16_t, uint32_t, uint16_t))
{
    WData &d = wdata();
    hal::own_blitter();
    hal::wait_blit();
    for (uint32_t &t : d.TColl)
        t = 0;
    int32_t hit = test(d, uint16_t(r->d[1]), r->d[2], uint16_t(r->d[3]));
    hal::disown_blitter();
    return hit;
}

} // namespace

extern "C" {
void w_sy_bob_collisions(AllRegs *r)
{
    r->d[0] = uint32_t(collisions(r, bob_collisions));
}

void w_sy_sprite_collisions(AllRegs *r)
{
    r->d[0] = uint32_t(collisions(r, sprite_collisions));
}

// GetCol: Col(n): -1 if bob or sprite n collided in the last test, 0 if
// not; Col(-n): the first number from n on that did, 0 if none
int32_t w_collision_get(uint8_t *data, int32_t n)
{
    const uint8_t *bits = reinterpret_cast<const uint8_t *>(reinterpret_cast<WData *>(data - sizeof(WData))->TColl);
    if (n >= 0) {
        n &= 0xFF;
        return bits[n >> 3] >> (n & 7) & 1 ? -1 : 0;
    }
    n = -n;
    if (n >= 255)
        return 0;
    for (; n < 256; n++)
        if (bits[n >> 3] >> (n & 7) & 1)
            return n;
    return 0;
}
}
