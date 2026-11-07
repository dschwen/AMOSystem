// Bobs and blocks of AMOS.library: the bob list and its drawing with the
// blitter (background save, erase, draw by priority), images (Get Bob,
// masks, hot spots, flipping), Paste Bob, and the blocks of the screen
// table: plain ones (Get/Put Block) and compressed ones (Get/Put
// CBlock).
//
// An image (sprite bank entry) is two longs: the image (width in words,
// height, planes, hot spot x with the flip flags in bits 14-15, hot spot
// y, then the planes) and its mask (0: to compute, negative: none, else
// its size long then one plane).
//
// The collisions (+W.s BbColl...) and AMAL, which run in the VBL
// interrupt, stay in the assembler and read the same fields: AMAL sets
// BbAct and the coordinates, which update() takes. BbAct is a byte in
// the high half of its word, as the assembler writes it.

#include "runtime/static_symbols.h"
#include "core/amiga.h"
#include "w/w.h"

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::w;

// +W.s


namespace {

constexpr uint16_t BitBobs = 1 << 13;          // T_Actualise
constexpr uint16_t DoubleBuffer = 1 << (8 + 5);
constexpr uint32_t DecorSize = 20;
constexpr uint16_t ErrBlock = 19;               // BlE

// What BbADraw holds: which drawing routine (the assembler kept its
// address; only this file uses it)
enum : uint32_t { DrawShifted = 1, DrawShiftedLong, Draw16 };

// A background save (BbDABuf, and Decor bytes further for the second
// buffer). APlan and NPlan are longs written as words.
struct Decor {
    be_u32 ABuf;
    be_u16 LBuf, AEc, APlan, unused1, NPlan, unused2, Mod, ASize;
};
static_assert(sizeof(Decor) == DecorSize);

Decor *decor(amos::Bob *b, uint16_t at)
{
    return reinterpret_cast<Decor *>(reinterpret_cast<uint8_t *>(b) + int16_t(at) + offsetof(amos::Bob, BbDABuf));
}

uint8_t &act(amos::Bob *b) { return reinterpret_cast<uint8_t *>(&b->BbAct)[0]; }

// BbAAEc is a long written as a word: the screen offset / 2, signed (a
// bob cut on the left starts a word early: at the top of the screen,
// before the plane)
uint16_t screen_offset(const amos::Bob *b) { return peek_w(&b->BbAAEc); }
void set_screen_offset(amos::Bob *b, uint16_t halved) { poke_w(&b->BbAAEc, halved); }
uint32_t bytes(uint16_t halved) { return uint32_t(int32_t(int16_t(halved)) * 2); }

// MCls / MCls2
uint16_t mask_left(int n) { return 0xFFFF >> n; }
uint16_t mask_left2(int n) { return uint16_t((1 << n) - 1); }

void own()
{
    hal::own_blitter();
    hal::wait_blit();
}

void call_a0(host::RoutineArg routine, uint32_t a0)
{
    Regs r;
    r.a[0] = a0;
    amos_call(&r, routine);
}

// ---- images ----------------------------------------------------------------------

// Image header
struct Image {
    be_u16 width, height, planes, hot_x, hot_y;
    uint8_t *data() { return reinterpret_cast<uint8_t *>(this) + 10; }
};

// The bytes reversed (TRetour)
uint8_t reversed(uint8_t b)
{
    uint8_t r = 0;
    for (int i = 0; i < 8; i++, b >>= 1)
        r = uint8_t(r << 1 | (b & 1));
    return r;
}

// RBbX: lines of width words, reversed byte by byte from the middle
void flip_lines_x(uint8_t *p, uint16_t width, uint16_t lines)
{
    for (uint16_t y = 0; y < lines; y++) {
        uint8_t *right = p + width, *left = right;
        for (uint16_t n = 0; n < width; n++) {
            uint8_t a = *--left, b = *right;
            *left = reversed(b);
            *right++ = reversed(a);
        }
        p += uint32_t(width) << 1;
    }
}

// RBbY: each plane upside down
void flip_lines_y(uint8_t *p, uint16_t width, uint16_t height, uint16_t planes)
{
    uint32_t line = uint32_t(width) << 1;
    uint32_t half = uint32_t(height >> 1) * line, back = line;
    if (height & 1) {
        back += line;
        half += line;
    }
    for (uint16_t n = 0; n < planes; n++) {
        p += int16_t(half);
        auto *a = reinterpret_cast<be_u16 *>(p), *b = reinterpret_cast<be_u16 *>(p - int16_t(back));
        for (uint16_t y = 0; y < (height >> 1); y++) {
            for (uint16_t x = 0; x < width; x++) {
                uint16_t v = *a;
                *a++ = *b;
                *b++ = v;
            }
            b -= 2 * width;
        }
        p = reinterpret_cast<uint8_t *>(a);
    }
}

// Retourne: the image of entry flipped to flags (bit 15 in x, 14 in y)
void flip(be_u32 *entry, uint16_t flags)
{
    Image *image = ptr<Image>(entry[0]);
    if (!image)
        return;
    uint16_t change = (image->hot_x & 0xC000) ^ flags;
    if (!change)
        return;
    if (change & 0x8000) {
        int16_t hot = int16_t(image->hot_x << 2) >> 2;
        image->hot_x = uint16_t((image->width << 4) - hot);
        flip_lines_x(image->data(), image->width, image->height * image->planes);
        if (int32_t(entry[1]) > 0)
            flip_lines_x(ptr<uint8_t>(entry[1]) + 4, image->width, image->height);
    }
    if (change & 0x4000) {
        image->hot_y = image->height - image->hot_y;
        flip_lines_y(image->data(), image->width, image->height, image->planes);
        if (int32_t(entry[1]) > 0)
            flip_lines_y(ptr<uint8_t>(entry[1]) + 4, image->width, image->height, 1);
    }
    image->hot_x = (image->hot_x & 0x3FFF) | flags;
}

// Masque: the mask of an image, the planes or'ed together
uint32_t make_mask(be_u32 *entry)
{
    Image *image = ptr<Image>(entry[0]);
    uint32_t plane = uint32_t(uint16_t(image->width << 1)) * image->height;
    uint32_t size = plane + 4;
    if (int32_t(entry[1]) <= 0) {
        uint32_t m = chip_mm2(size);
        if (!m)
            return 0xFFFFFFFF;
        entry[1] = m;
    }
    uint8_t *mask = ptr<uint8_t>(entry[1]);
    poke_l(mask, size);
    auto *to = reinterpret_cast<be_u16 *>(mask + 4);
    const uint8_t *data = image->data();
    uint16_t words = uint16_t(plane) >> 1;
    for (uint16_t i = 0; i < words; i++, data += 2) {
        uint16_t v = peek_w(data);
        for (uint16_t p = 1; p < image->planes; p++)
            v |= peek_w(data + p * plane);
        *to++ = v;
    }
    return 0;
}

// SpotH: the hot spot of an image: mode 0 at dx,dy, else the position
// (mode - 1: bits 4-5 in x, 0-1 in y: 0 left, 1 middle, 2 right)
uint32_t hot_spot(be_u32 *entry, uint16_t mode, uint16_t dx, uint16_t dy)
{
    Image *image = ptr<Image>(entry[0]);
    if (!image)
        return 0xFFFFFFFF;
    if (mode) {
        dx = image->width << 4;
        dy = image->height;
        mode--;
        auto place = [](uint16_t where, uint16_t size) -> uint16_t {
            if (where >= 2)
                return size;
            return where ? size >> 1 : 0;
        };
        dx = place((mode >> 4) & 3, dx);
        dy = place(mode & 3, dy);
    }
    image->hot_x = (image->hot_x & 0xC000) | (dx & 0x3FFF);
    image->hot_y = dy;
    return 0;
}

// GetBob: entry gets the tx x ty pixels at x,y of screen s (all its
// planes), with no mask
uint32_t get_image(Screen *s, be_u32 *entry, uint16_t x, uint16_t y, uint16_t tx, uint16_t ty)
{
    uint16_t right = tx & 15;
    uint16_t planes = s->EcNPlan;
    uint16_t words = (tx + 15) >> 4;
    uint16_t line = words << 1;
    uint32_t size = uint32_t(uint16_t(uint32_t(line) * ty)) * planes;
    if (Image *old = ptr<Image>(entry[0])) {
        uint32_t old_size = uint32_t(uint16_t(uint32_t(uint16_t(old->width << 1)) * old->height)) * old->planes;
        if (old_size != size) {
            free_mm(entry[0], old_size + 10);
            entry[0] = 0;
        }
    }
    if (int32_t(entry[1]) > 0)
        free_mm(entry[1], peek_l(ptr<uint8_t>(entry[1])));
    entry[1] = 0;
    if (!entry[0]) {
        uint32_t m = chip_mm2(size + 10);
        if (!m)
            return 0xFFFFFFFF;
        entry[0] = m;
    }
    Image *image = ptr<Image>(entry[0]);
    image->width = words;
    image->height = ty;
    image->planes = planes;
    image->hot_x = 0;
    image->hot_y = 0;
    uint8_t *data = image->data();
    auto &c = chips();
    own();
    uint32_t offset = uint32_t(y) * s->EcTLigne + uint32_t(int16_t((x >> 4) << 1));
    if (!(x & 15)) {
        c.bltafwm = 0xFFFF;
        c.bltalwm = right ? uint16_t(~mask_left(right)) : 0xFFFF;
        c.bltamod = s->EcTLigne - line;
        c.bltdpt = data;
        c.bltdmod = 0;
        c.bltcon0 = 0x09F0;
        c.bltcon1 = 0;
        for (uint16_t p = 0; p < planes; p++) {
            c.bltapt = ptr<uint8_t>(s->EcCurrent[p]) + offset;
            hal::blit(uint16_t(ty << 6) | words);
            hal::wait_blit();
        }
    } else {
        // A line at a time, shifted, one word early: the word before is
        // put back, the last one masked
        c.bltcon0 = 0x05CC;
        c.bltcon1 = uint16_t((16 - (x & 15)) << 12);
        uint16_t size = (words + 1) | 0x40;
        uint16_t last = mask_left(right);
        if (int16_t(last) >= 0)
            last = ~last;
        uint8_t *to = data - 2;
        for (uint16_t p = 0; p < planes; p++) {
            uint8_t *from = ptr<uint8_t>(s->EcCurrent[p]) + offset;
            for (uint16_t row = 0; row < ty; row++) {
                c.bltbpt = from;
                uint16_t before = peek_w(to);
                c.bltdpt = to;
                hal::blit(size);
                uint8_t *at = to;
                from += int16_t(s->EcTLigne);
                to += line;
                hal::wait_blit();
                poke_w(at, before);
                poke_w(to, peek_w(to) & last);
            }
        }
    }
    hal::disown_blitter();
    return 0;
}

// ---- drawing ----------------------------------------------------------------------

// BobCalc: the blitter parameters of image entry at x,y on screen s,
// clipped to the limits of b. flags: the flip flags; bit 31 set: no hot
// spot. False if nothing shows.
bool calc(Screen *s, be_u32 *entry, amos::Bob *b, int16_t x, int16_t y, uint32_t flags)
{
    Image *image = ptr<Image>(entry[0]);
    if (!entry[1] && make_mask(entry))
        return false;
    if (int32_t(flags) >= 0) {
        uint16_t hx = image->hot_x;
        int16_t dx = int16_t(hx << 2) >> 2, dy = image->hot_y;
        uint16_t f = uint16_t(flags) ^ hx;
        if (f & 0x4000)
            dy = image->height - dy;
        if (f & 0x8000)
            dx = int16_t((image->width << 4) - dx);
        y -= dy;
        x -= dx;
    }
    uint16_t planes = image->planes;
    if (planes > s->EcNPlan)
        planes = s->EcNPlan;
    b->BbNPlan = planes - 1;
    b->BbESize = 0;
    bool no_mask = int32_t(entry[1]) < 0;
    if (!b->BbACon) {
        b->BbACon = no_mask ? 0x07CA : 0x0FCA;
    } else if (int16_t(b->BbACon) < 0) {
        uint16_t con = (b->BbACon & 0x7FFF) | 0x0F00;
        if (no_mask)
            con &= 0x07FF;
        b->BbACon = con;
    }

    uint16_t shift = x & 15;
    if (shift) {
        b->BbACon1 = uint16_t(shift << 12);
        b->BbACon0 = b->BbACon1 | b->BbACon;
        uint16_t line = image->width << 1;
        uint16_t source_mod = line;
        int16_t height = image->height;
        uint16_t plane = uint16_t(uint32_t(line) * uint16_t(height));
        b->BbTPlan = plane;
        b->BbETPlan = plane + 2 * height;
        int16_t bottom = height + y;
        if (bottom > int16_t(b->BbLimB)) {
            uint16_t over = bottom - b->BbLimB;
            if (uint16_t(height) <= over)
                return false;
            height -= over;
        }
        uint32_t skip = 0;
        bool left_clip = false;
        if (y < int16_t(b->BbLimH)) {
            uint16_t over = b->BbLimH - y;
            if (uint16_t(height) <= over)
                return false;
            height -= over;
            skip = uint32_t(over) * line;
            y = b->BbLimH;
        }
        uint16_t screen_line = s->EcTLigne;
        uint32_t offset = uint32_t(screen_line) * uint16_t(y);
        uint16_t width = line << 3;
        b->BbAMaskD = 0;
        if (int16_t(width + x) > int16_t(b->BbLimD)) {
            uint16_t over = ((width + x - b->BbLimD) & 0xFFF0) + 16;
            width -= over;
            if (int16_t(width) < 0)
                return false;
            b->BbAMaskD = uint16_t(~mask_left2(x & 15));
        }
        uint16_t first = 0xFFFF;
        int16_t at = x;
        if (x < int16_t(b->BbLimG)) {
            uint16_t over = b->BbLimG - x;
            if (width <= over)
                return false;
            width -= over;
            width += 16;
            skip = (skip & 0xFFFF0000) | uint16_t(skip + ((over >> 4) << 1));
            left_clip = true;
            offset -= 2;
            first = mask_left2(x & 15);
            at = b->BbLimG;
        }
        b->BbAMaskG = first;
        width += 16;
        offset += uint32_t(uint16_t(uint16_t(at) >> 4) << 1);
        uint16_t halved = uint16_t(offset >> 1);
        set_screen_offset(b, halved);
        uint16_t words = width >> 4;
        b->BbAModO = source_mod - (words << 1);
        uint16_t screen_mod = screen_line - (words << 1);
        b->BbAModD = screen_mod;
        uint16_t erase_words = words;
        uint32_t draw = DrawShifted;
        if (left_clip) {
            halved++;
            erase_words--;
            screen_mod += 2;
            if (!screen_mod)
                draw = DrawShiftedLong;
        }
        uint16_t h = uint16_t(height << 6);
        b->BbESize = h | erase_words;
        b->BbASize = h | words;
        b->BbEMod = screen_mod;
        b->BbEAEc = halved;
        b->BbADraw = draw;
        b->BbAMask = entry[1] + 4 + int16_t(skip);
        b->BbAData = entry[0] + 10 + int16_t(skip);
        return true;
    }

    // On a word: the limits at the top and the left cut whole words
    int16_t right = x, bottom = y;
    uint16_t skip_words = 0, skip_lines = 0;
    if (x < int16_t(b->BbLimG)) {
        skip_words = uint16_t(b->BbLimG - x) >> 4;
        x = b->BbLimG;
    }
    if (y < int16_t(b->BbLimH)) {
        skip_lines = b->BbLimH - y;
        y = b->BbLimH;
    }
    uint32_t offset = uint32_t(uint16_t(y)) * s->EcTLigne + uint32_t(int16_t((uint16_t(x) >> 4) << 1));
    uint16_t halved = uint16_t(offset >> 1);
    set_screen_offset(b, halved);
    b->BbEAEc = halved;
    uint16_t w = image->width, h = image->height;
    uint16_t line = w << 1;
    uint16_t plane = uint16_t(uint32_t(line) * h);
    b->BbTPlan = plane;
    b->BbETPlan = plane;
    uint32_t skip = uint32_t(line) * skip_lines;
    skip = (skip & 0xFFFF0000) | uint16_t(skip + skip_words + skip_words);
    b->BbAMask = entry[1] + 4 + int16_t(skip);
    b->BbAData = entry[0] + 10 + int16_t(skip);
    b->BbACon0 = b->BbACon;
    b->BbACon1 = 0;
    right += int16_t(w << 4);
    bottom += h;
    int16_t words = w, lines = h;
    if (right > int16_t(b->BbLimD))
        words -= uint16_t(right - b->BbLimD) >> 4;
    if (bottom > int16_t(b->BbLimB))
        lines -= bottom - b->BbLimB;
    words -= skip_words;
    if (words <= 0)
        return false;
    lines -= skip_lines;
    if (lines <= 0)
        return false;
    b->BbAModO = uint16_t((w - words) << 1);
    uint16_t mod = s->EcTLigne - 2 * words;
    b->BbAModD = mod;
    b->BbEMod = mod;
    uint16_t size = uint16_t(lines << 6) | uint16_t(words);
    b->BbASize = size;
    b->BbESize = size;
    b->BbADraw = Draw16;
    return true;
}

// The drawing routines (BbAp, BbAL, BbA16, BMA16, BMAp): data planes
// step apart, into planes (EcLogic or EcCurrent), mask (negative: none)
void draw(amos::Bob *b, const be_u32 *planes, uint32_t data, uint32_t step, uint32_t mask)
{
    auto &c = chips();
    uint32_t offset = bytes(screen_offset(b));
    uint16_t size = b->BbASize;
    uint16_t which = b->BbAPlan;
    bool masked = int32_t(mask) >= 0;
    uint32_t routine = b->BbADraw;
    auto each = [&](auto blit) {
        for (uint16_t p = 0; p <= b->BbNPlan; p++, data += step, which >>= 1) {
            if (!(which & 1))
                continue;
            uint8_t *to = ptr<uint8_t>(uint32_t(planes[p]) + offset);   // (the offset wraps: it may be negative)
            hal::wait_blit();
            blit(to);
        }
    };
    if (routine == Draw16 && masked) {
        c.bltamod = b->BbAModO;
        c.bltbmod = b->BbAModO;
        c.bltcon0 = b->BbACon0;
        c.bltcon1 = 0;
        c.bltafwm = 0xFFFF;
        c.bltalwm = 0xFFFF;
        each([&](uint8_t *to) {
            c.bltapt = ptr<uint8_t>(mask);
            c.bltbpt = ptr<uint8_t>(data);
            c.bltcpt = to;
            c.bltdpt = to;
            hal::blit(size);
        });
    } else if (routine == Draw16) {
        c.bltbmod = b->BbAModO;
        if (uint8_t(b->BbACon0) == 0xCA) {
            // A plain copy: A to D only, twice as fast
            c.bltamod = b->BbAModO;
            c.bltcon0 = 0x09F0;
            c.bltcon1 = 0;
            c.bltafwm = 0xFFFF;
            c.bltalwm = 0xFFFF;
            c.bltbdat = 0xFFFF;
            c.bltcdat = 0xFFFF;
            each([&](uint8_t *to) {
                c.bltapt = ptr<uint8_t>(data);
                c.bltdpt = to;
                hal::blit(size);
            });
        } else {
            c.bltcon0 = b->BbACon0;
            c.bltcon1 = 0;
            c.bltafwm = 0xFFFF;
            c.bltalwm = 0xFFFF;
            c.bltadat = 0xFFFF;
            each([&](uint8_t *to) {
                c.bltbpt = ptr<uint8_t>(data);
                c.bltcpt = to;
                c.bltdpt = to;
                hal::blit(size);
            });
        }
    } else if (!masked) {
        // (Also for DrawShiftedLong, as the assembler)
        c.bltbmod = b->BbAModO;
        c.bltcon0 = b->BbACon0;
        c.bltcon1 = b->BbACon1;
        c.bltafwm = b->BbAMaskG;
        c.bltalwm = b->BbAMaskD;
        c.bltadat = 0xFFFF;
        each([&](uint8_t *to) {
            c.bltbpt = ptr<uint8_t>(data);
            c.bltcpt = to;
            c.bltdpt = to;
            hal::blit(size);
        });
    } else {
        c.bltcon0 = b->BbACon0;
        c.bltcon1 = b->BbACon1;
        c.bltamod = b->BbAModO;
        c.bltbmod = b->BbAModO;
        c.bltafwm = b->BbAMaskG;
        c.bltalwm = b->BbAMaskD;
        c.bltadat = 0;
        if (routine == DrawShifted) {
            each([&](uint8_t *to) {
                c.bltapt = ptr<uint8_t>(mask);
                c.bltbpt = ptr<uint8_t>(data);
                c.bltcpt = to;
                c.bltdpt = to;
                hal::blit(size);
            });
        } else {
            // A line at a time
            uint16_t lines = size >> 6, line = (size & 0x3F) | 0x40;
            each([&](uint8_t *to) {
                c.bltapt = ptr<uint8_t>(mask);
                c.bltbpt = ptr<uint8_t>(data);
                c.bltcpt = to;
                c.bltdpt = to;
                uint16_t n = lines;
                do {
                    hal::wait_blit();
                    c.bltadat = 0;
                    hal::blit(line);
                } while (--n);
            });
        }
    }
}

// PBobA and the like: draw b at once in the current planes
void draw_now(amos::Bob *b)
{
    auto &c = chips();
    own();
    Screen *s = ptr<Screen>(b->BbEc);
    c.bltcmod = b->BbAModD;
    c.bltdmod = b->BbAModD;
    draw(b, s->EcCurrent, b->BbAData, uint32_t(int32_t(int16_t(b->BbTPlan))), b->BbAMask);
    hal::wait_blit();
    hal::disown_blitter();
}

// The limits of b from the clip rectangle of the current screen
void clip_limits(amos::Bob *b, Screen *s)
{
    b->BbLimG = s->EcClipX0 & 0xFFF0;
    b->BbLimH = s->EcClipY0;
    b->BbLimD = (s->EcClipX1 + 15) & 0xFFF0;
    b->BbLimB = s->EcClipY1;
}

// ---- the bobs -----------------------------------------------------------------------

amos::Bob *first_bob() { return ptr<amos::Bob>(wdata().BbDeb); }

// BobAd
amos::Bob *find_bob(uint16_t n)
{
    for (amos::Bob *b = first_bob(); b; b = ptr<amos::Bob>(b->BbNext)) {
        if (b->BbNb == n)
            return b;
        if (n < b->BbNb)
            break;
    }
    return nullptr;
}

// DelBob: b out of the list and freed
void delete_bob(amos::Bob *b)
{
    for (uint32_t at = 0; at <= DecorSize; at += DecorSize) {
        Decor *d = decor(b, at);
        if (d->LBuf)
            free_mm(d->ABuf, uint32_t(d->LBuf) << 1);
    }
    call_a0(AMOS_ROUTINE(w_dad_amal), address(&b->BbAct));
    amos::Bob *next = ptr<amos::Bob>(b->BbNext), *prev = ptr<amos::Bob>(b->BbPrev);
    if (prev)
        prev->BbNext = address(next);
    else
        wdata().BbDeb = address(next);
    if (next)
        next->BbPrev = address(prev);
    free_mm(address(b), amos::Bob::BbLong);
}

// ResBOB: a new bob n on the current screen; erase mode (negative: no
// background), planes, minterms
amos::Bob *new_bob(uint16_t n, uint16_t erase, uint16_t planes, uint16_t minterms)
{
    uint32_t m = fast_mm(amos::Bob::BbLong);
    if (!m)
        return nullptr;
    amos::Bob *b = ptr<amos::Bob>(m);
    Screen *s = current_screen();
    b->BbNb = n;
    b->BbEc = address(s);
    b->BbLimD = s->EcTx;
    b->BbLimB = s->EcTy;
    b->BbAPlan = planes;
    minterms &= 0xFF;
    b->BbACon = minterms ? minterms | 0x8000 : 0;
    b->BbDecor = 1;
    if (s->EcFlags & DoubleBuffer) {
        b->BbDecor++;
        b->BbDCur2 = DecorSize;
    }
    if (int16_t(erase) < 0)
        b->BbDecor = 0;
    b->BbEff = erase;
    return b;
}

// BobSet: Bob n,x,y,image (EntNul: unchanged); d5 erase mode, d6 planes,
// d7 minterms for a new one
uint32_t set_bob(const AllRegs *r)
{
    WData &d = wdata();
    uint16_t n = r->d[1];
    if (n >= d.BbMax)
        return 1;
    amos::Bob *b = first_bob(), *last = nullptr;
    while (b && b->BbNb < n) {
        last = b;
        b = ptr<amos::Bob>(b->BbNext);
    }
    if (!b || b->BbNb != n) {
        amos::Bob *nb = new_bob(n, r->d[5], r->d[6], r->d[7]);
        if (!nb)
            return 0xFFFFFFFF;
        if (b) {
            // before b
            nb->BbPrev = b->BbPrev;
            b->BbPrev = address(nb);
            if (nb->BbPrev)
                ptr<amos::Bob>(nb->BbPrev)->BbNext = address(nb);
            else
                d.BbDeb = address(nb);
            nb->BbNext = address(b);
        } else if (last) {
            nb->BbPrev = address(last);
            last->BbNext = address(nb);
        } else {
            d.BbDeb = address(nb);
        }
        b = nb;
    }
    uint8_t a = act(b);
    if (int8_t(a) >= 0) {
        if (r->d[2] != EntNul) {
            b->BbX = r->d[2];
            a |= 2;
        }
        if (r->d[3] != EntNul) {
            b->BbY = r->d[3];
            a |= 4;
        }
        if (r->d[4] != EntNul) {
            b->BbI = r->d[4];
            a |= 1;
        }
        act(b) = a;
    }
    d.Actualise |= BitBobs;
    return 0;
}

// BobLim: Limit Bob n (or all, n < 0) of the current screen. (As the
// assembler: a coordinate of 0 counts as omitted, and the bottom is
// compared with the left.)
uint32_t limit_bobs(const AllRegs *r)
{
    amos::Bob *b = first_bob();
    if (!b)
        return 0;
    Screen *s = current_screen();
    uint16_t x1 = r->d[2], y1 = r->d[3], x2 = r->d[4], y2 = r->d[5];
    if (!x2)
        x2 = s->EcTx;
    if (!y2)
        y2 = s->EcTy;
    x1 &= 0xFFF0;
    x2 &= 0xFFF0;
    if (x2 <= x1 || y2 <= x1 || x2 > s->EcTx || y2 > s->EcTy)
        return 0xFFFFFFFF;
    int16_t n = r->d[1];
    for (; b; b = ptr<amos::Bob>(b->BbNext)) {
        if (int8_t(act(b)) < 0 || b->BbEc != address(s))
            continue;
        if (n >= 0) {
            if (uint16_t(n) > b->BbNb)
                continue;
            if (uint16_t(n) < b->BbNb)
                break;
        }
        b->BbLimG = x1;
        b->BbLimH = y1;
        b->BbLimD = x2;
        b->BbLimB = y2;
        act(b) |= 1;
        wdata().Actualise |= BitBobs;
    }
    return 0;
}

// BobAct: the bobs that changed get their new parameters, their
// backgrounds a buffer, and T_BbPrio the list to draw, those of the
// priority screen sorted by y then x
void update()
{
    WData &d = wdata();
    be_u32 *list = ptr<be_u32>(d.BbPrio);
    const uint8_t *bank = ptr<uint8_t>(d.SprBank);
    amos::Bob *b = first_bob();
    if (bank && b) {
        uint16_t sorted = 0;
        be_u32 *second = ptr<be_u32>(d.BbPrio2);
        uint32_t priority = d.Priorite;
        while (b) {
            amos::Bob *next = ptr<amos::Bob>(b->BbNext);
            uint16_t swap = b->BbDCur2;
            b->BbDCur2 = b->BbDCur1;
            b->BbDCur1 = swap;
            bool visible = true;
            if (!b->BbECpt && act(b)) {
                if (int8_t(act(b)) < 0) {
                    // Off: deleted once its backgrounds are put back
                    if (b->BbDecor-- <= 1)
                        delete_bob(b);
                    b = next;
                    continue;
                }
                act(b) = 0;
                uint16_t image = b->BbI;
                uint32_t flags = image & 0xC000;
                b->BbRetour = flags;
                image &= 0x3FFF;
                visible = false;
                if (image && image <= peek_w(bank)) {
                    be_u32 *entry = ptr<be_u32>(address(bank) + 2 + (image - 1) * 8);
                    if (entry[0]) {
                        b->BbARetour = address(entry);
                        visible = calc(ptr<Screen>(b->BbEc), entry, b, b->BbX, b->BbY, flags);
                    }
                }
            }
            if (visible && b->BbDecor) {
                // The background to save
                if (!b->BbESize) {
                    visible = false;
                } else {
                    b->BbDCpt = b->BbDecor;
                    Decor *dc = decor(b, b->BbDCur1);
                    dc->ASize = b->BbESize;
                    dc->Mod = b->BbEMod;
                    dc->APlan = b->BbAPlan;
                    dc->AEc = b->BbEAEc;
                    dc->NPlan = b->BbNPlan;
                    if (!b->BbEff) {
                        uint32_t need = uint32_t(uint16_t(b->BbNPlan + 1)) * b->BbETPlan;
                        uint32_t have = uint32_t(dc->LBuf) << 1;
                        if (!have || need > have) {
                            if (have) {
                                free_mm(dc->ABuf, have);
                                dc->ABuf = 0;
                                dc->LBuf = 0;
                            }
                            uint32_t m = chip_mm(need);
                            if (m) {
                                dc->ABuf = m;
                                dc->LBuf = uint16_t(need >> 1);
                            }
                        }
                    }
                }
            }
            if (visible) {
                if (b->BbEc == priority) {
                    *second++ = address(b);
                    sorted++;
                } else {
                    *list++ = address(b);
                }
            }
            b = next;
        }
        if (sorted) {
            be_u32 *start = list;
            const be_u32 *from = ptr<be_u32>(d.BbPrio2);
            for (uint16_t i = 0; i < sorted; i++)
                *list++ = from[i];
            // Bubble sort, by y then x
            if (sorted > 1) {
                bool swapped;
                do {
                    swapped = false;
                    for (uint16_t i = 0; i + 1 < sorted; i++) {
                        amos::Bob *a = ptr<amos::Bob>(start[i]), *c = ptr<amos::Bob>(start[i + 1]);
                        int16_t ya = a->BbY, yc = c->BbY;
                        if (ya < yc || (ya == yc && int16_t(a->BbX) <= int16_t(c->BbX)))
                            continue;
                        start[i] = address(c);
                        start[i + 1] = address(a);
                        swapped = true;
                    }
                } while (swapped);
            }
        }
    }
    *list = 0;
    if (d.PriRev) {
        be_u32 *a = ptr<be_u32>(d.BbPrio);
        while (a < list) {
            uint32_t v = *a;
            *a++ = *--list;
            *list = v;
        }
    }
}

// BobEff: the backgrounds back (or the erase colour)
void erase()
{
    amos::Bob *b = first_bob();
    if (!b)
        return;
    auto &c = chips();
    own();
    c.bltamod = 0;
    c.bltcon1 = 0;
    c.bltafwm = 0xFFFF;
    c.bltalwm = 0xFFFF;
    for (; b; b = ptr<amos::Bob>(b->BbNext)) {
        if (b->BbECpt) {
            b->BbECpt--;
            continue;
        }
        const auto *planes = ptr<Screen>(b->BbEc)->EcLogic;
        Decor *dc = decor(b, b->BbDCur2);
        uint16_t size = dc->ASize;
        if (!size)
            continue;
        uint32_t offset = bytes(dc->AEc);
        uint16_t which = dc->APlan;
        if (uint16_t colour = b->BbEff) {
            colour--;
            hal::wait_blit();
            c.bltdmod = dc->Mod;
            c.bltcon0 = 0x01F0;
            for (uint16_t p = 0; p <= dc->NPlan; p++, colour >>= 1, which >>= 1) {
                if (!(which & 1))
                    continue;
                hal::wait_blit();
                c.bltadat = colour & 1 ? 0xFFFF : 0;
                c.bltdpt = ptr<uint8_t>(planes[p]) + offset;
                hal::blit(size);
            }
            continue;
        }
        if (!dc->ABuf)
            continue;
        hal::wait_blit();
        c.bltdmod = dc->Mod;
        c.bltapt = ptr<uint8_t>(dc->ABuf);
        c.bltcon0 = 0x09F0;
        for (uint16_t p = 0; p <= dc->NPlan; p++, which >>= 1) {
            if (!(which & 1))
                continue;
            hal::wait_blit();
            c.bltdpt = ptr<uint8_t>(planes[p]) + offset;
            hal::blit(size);
        }
    }
    hal::wait_blit();
    hal::disown_blitter();
}

// BobAff: save the backgrounds, then draw the bobs of T_BbPrio
void show()
{
    auto &c = chips();
    own();
    if (amos::Bob *b = first_bob()) {
        c.bltdmod = 0;
        c.bltcon0 = 0x09F0;
        c.bltcon1 = 0;
        c.bltafwm = 0xFFFF;
        c.bltalwm = 0xFFFF;
        for (; b; b = ptr<amos::Bob>(b->BbNext)) {
            if (!b->BbDCpt || b->BbEff)
                continue;
            const auto *planes = ptr<Screen>(b->BbEc)->EcLogic;
            Decor *dc = decor(b, b->BbDCur1);
            if (!dc->ABuf || !dc->ASize)
                continue;
            b->BbDCpt--;
            uint32_t offset = bytes(dc->AEc);
            uint16_t which = dc->APlan;
            hal::wait_blit();
            c.bltdpt = ptr<uint8_t>(dc->ABuf);
            c.bltamod = dc->Mod;
            for (uint16_t p = 0; p <= dc->NPlan; p++, which >>= 1) {
                if (!(which & 1))
                    continue;
                hal::wait_blit();
                c.bltapt = ptr<uint8_t>(planes[p]) + offset;
                hal::blit(dc->ASize);
            }
        }
    }
    for (const be_u32 *list = ptr<be_u32>(wdata().BbPrio); *list; list++) {
        amos::Bob *b = ptr<amos::Bob>(*list);
        if (!b->BbASize)
            continue;
        flip(ptr<be_u32>(b->BbARetour), b->BbRetour);
        hal::wait_blit();
        c.bltcmod = b->BbAModD;
        c.bltdmod = b->BbAModD;
        draw(b, ptr<Screen>(b->BbEc)->EcLogic, b->BbAData, b->BbTPlan, b->BbAMask);
    }
    hal::wait_blit();
    hal::disown_blitter();
}

// TPatch: Paste Bob/Icon: entry (a2) at d2,d3 flipped as d1, with
// minterms d4 and planes d5, using the calculation buffer a1
void paste(const AllRegs *r)
{
    amos::Bob *b = ptr<amos::Bob>(r->a[1]);
    be_u32 *entry = ptr<be_u32>(r->a[2]);
    flip(entry, r->d[1] & 0xC000);
    Screen *s = current_screen();
    clip_limits(b, s);
    uint16_t minterms = r->d[4];
    if (minterms)
        minterms = (minterms & 0xFF) | 0x8000;
    b->BbACon = minterms;
    b->BbAPlan = r->d[5];
    b->BbEc = address(s);
    if (!calc(s, entry, b, r->d[2], r->d[3], r->d[1] | 0x80000000))
        return;
    if (!s->EcAuto) {
        draw_now(b);
        return;
    }
    call(AMOS_ROUTINE(w_tabk1));
    draw_now(b);
    call(AMOS_ROUTINE(w_tabk2));
    draw_now(b);
    call(AMOS_ROUTINE(w_tabk3));
}

// ---- blocks ------------------------------------------------------------------------

struct Block {
    be_u32 BlPrev, BlNext;
    be_u16 BlNb, BlX, BlY, BlMask, BlCon, BlAPlan;
    be_u32 BlDesc[2];
};
static_assert(sizeof(Block) == 28);

Block *find_block(uint16_t n)
{
    for (Block *b = ptr<Block>(wdata().AdBlocs); b; b = ptr<Block>(b->BlNext))
        if (b->BlNb == n)
            return b;
    return nullptr;
}

// FrBloc
void free_block(Block *b)
{
    if (Image *image = ptr<Image>(b->BlDesc[0]))
        free_mm(b->BlDesc[0], (uint32_t(uint16_t(uint32_t(image->width) * image->height)) * image->planes + 5) << 1);
    if (int32_t(b->BlDesc[1]) > 0)
        free_mm(b->BlDesc[1], peek_l(ptr<uint8_t>(b->BlDesc[1])));
    WData &d = wdata();
    if (d.AdBlocs == address(b))
        d.AdBlocs = b->BlNext;
    if (b->BlPrev)
        ptr<Block>(b->BlPrev)->BlNext = b->BlNext;
    if (b->BlNext)
        ptr<Block>(b->BlNext)->BlPrev = b->BlPrev;
    free_mm(address(b), sizeof(Block));
}

// MakeBloc: Get Block n (d1),x,y,tx,ty (d2-d5), with a mask if d6
uint32_t get_block(const AllRegs *r)
{
    Screen *s = current_screen();
    int16_t x = r->d[2], y = r->d[3], tx = r->d[4], ty = r->d[5];
    if (x < 0 || y < 0 || tx <= 0 || ty <= 0 || uint16_t(ty + y) > s->EcTy || uint16_t(tx + x) > s->EcTx)
        return ErrBlock + 3;
    uint16_t n = r->d[1];
    Block *b = find_block(n);
    bool fresh = !b;
    if (fresh) {
        uint32_t m = fast_mm(sizeof(Block));
        if (!m)
            return 1;
        b = ptr<Block>(m);
    }
    b->BlNb = n;
    b->BlCon = 0;
    b->BlAPlan = 0xFFFF;
    b->BlX = x;
    b->BlY = y;
    if (get_image(s, b->BlDesc, x, y, tx, ty)) {
        free_block(b);
        return 1;
    }
    b->BlDesc[1] = 0xC0000000;
    if (uint16_t(r->d[6]))
        make_mask(b->BlDesc);
    if (fresh) {
        WData &d = wdata();
        if (d.AdBlocs)
            ptr<Block>(d.AdBlocs)->BlPrev = address(b);
        b->BlNext = d.AdBlocs;
        d.AdBlocs = address(b);
    }
    return 0;
}

// DrawBloc: Put Block n (d1) at d2,d3 (EntNul: where it was taken), planes
// d4, minterms d5, with the calculation buffer a1
uint32_t put_block(const AllRegs *r)
{
    amos::Bob *calc_buffer = ptr<amos::Bob>(r->a[1]);
    Block *bl = find_block(r->d[1]);
    if (!bl)
        return 2;
    Screen *s = current_screen();
    clip_limits(calc_buffer, s);
    int16_t x = r->d[2] == EntNul ? bl->BlX : r->d[2];
    int16_t y = r->d[3] == EntNul ? bl->BlY : r->d[3];
    calc_buffer->BbAPlan = r->d[4] == EntNul ? bl->BlAPlan : r->d[4];
    calc_buffer->BbACon = r->d[5] == EntNul ? bl->BlCon : (r->d[5] & 0xFF) | 0x8000;
    calc_buffer->BbEc = address(s);
    if (calc(s, bl->BlDesc, calc_buffer, x, y, 0xFFFFFFFF))
        draw_now(calc_buffer);
    return 0;
}

// ---- compressed blocks ----------------------------------------------------------

// The header: prev, next, allocated length, number, x (pixels), y, width
// in bytes, height, planes; then for each line and plane, bytes or runs
// (%11nnnnnn, then the byte, n+1 times)
struct CBlock {
    be_u32 prev, next, length;
    be_u16 number, x, y, width, height, planes;
};
static_assert(sizeof(CBlock) == 24);

// The bytes of one plane line, packed (to null: only counted). (The
// assembler stopped a run after 65 bytes but recorded 64, shifting the
// rest of the line.)
uint32_t pack_line(const uint8_t *from, uint16_t n, uint8_t *to)
{
    uint32_t length = 0;
    auto put = [&](uint8_t v) {
        if (to)
            to[length] = v;
        length++;
    };
    while (n) {
        uint8_t v = *from++;
        n--;
        uint16_t run = 0;
        while (n && *from == v && run < 63) {
            from++;
            n--;
            run++;
        }
        if (run) {
            put(uint8_t(run | 0xC0));
            put(v);
        } else {
            if (v >= 0xC0)
                put(0xC0);
            put(v);
        }
    }
    return length;
}

// CBloc: the compressed block of x,y,tx,ty (pixels; x and tx / 8) of the
// current screen. (Offsets are 32 bits; the assembler added in 16.)
uint32_t compress(uint16_t x, uint16_t y, uint16_t tx, uint16_t ty, CBlock **result)
{
    Screen *s = current_screen();
    uint16_t bx = x >> 3, bw = tx >> 3;
    if (uint16_t(bx + bw) > s->EcTLigne || uint32_t(uint16_t(y + ty)) * s->EcTLigne > s->EcTPlan)
        return ErrBlock + 3;
    uint32_t start = uint32_t(y) * s->EcTLigne + bx;
    uint32_t total = 0;
    for (int pass = 0; pass < 2; pass++) {
        uint8_t *to = nullptr;
        if (pass) {
            uint32_t length = (total + 20 + 16) & 0xFFFFFFF8;
            uint32_t m = fast_mm(length);
            if (!m)
                return 1;
            CBlock *c = ptr<CBlock>(m);
            c->prev = 0;
            c->next = 0;
            c->length = length;
            c->number = 0;
            c->x = x;
            c->y = y;
            c->width = bw;
            c->height = ty;
            c->planes = s->EcNPlan;
            to = reinterpret_cast<uint8_t *>(c + 1);
            *result = c;
        }
        uint32_t offset = start;
        for (uint16_t row = 0; row < ty; row++, offset += s->EcTLigne) {
            for (uint16_t p = 0; p < s->EcNPlan; p++) {
                uint32_t n = pack_line(ptr<uint8_t>(s->EcCurrent[p]) + offset, bw, to);
                if (to)
                    to += n;
                else
                    total += n;
            }
        }
    }
    return 0;
}

// PBloc: c back at x,y (negative: where it was taken). (Planes the screen
// does not have are skipped; the assembler lost its place in the data.)
uint32_t decompress(const CBlock *c, int16_t x, int16_t y)
{
    Screen *s = current_screen();
    if (x < 0)
        x = c->x;
    if (y < 0)
        y = c->y;
    uint16_t bx = uint16_t(x) >> 3;
    if (uint16_t(bx + c->width) > s->EcTLigne || uint32_t(uint16_t(y + c->height)) * s->EcTLigne > s->EcTPlan)
        return ErrBlock + 3;
    uint32_t offset = uint32_t(uint16_t(y)) * s->EcTLigne + bx;
    uint16_t planes = c->planes < s->EcNPlan ? c->planes : s->EcNPlan;
    const uint8_t *from = reinterpret_cast<const uint8_t *>(c + 1);
    for (uint16_t row = 0; row < c->height; row++, offset += s->EcTLigne) {
        for (uint16_t p = 0; p < c->planes; p++) {
            uint8_t *to = p < planes ? ptr<uint8_t>(s->EcCurrent[p]) + offset : nullptr;
            int16_t left = c->width;
            while (left > 0) {
                uint8_t v = *from++;
                uint16_t n = 1;
                if (v >= 0xC0) {
                    n = (v & 0x3F) + 1;
                    v = *from++;
                }
                for (uint16_t i = 0; i < n; i++)
                    if (to)
                        *to++ = v;
                left -= n;
            }
        }
    }
    return 0;
}

CBlock *find_cblock(uint16_t n)
{
    for (CBlock *c = ptr<CBlock>(wdata().AdCBlocs); c; c = ptr<CBlock>(c->next))
        if (c->number == n)
            return c;
    return nullptr;
}

// FrCBloc
void free_cblock(CBlock *c)
{
    WData &d = wdata();
    if (d.AdCBlocs == address(c))
        d.AdCBlocs = c->next;
    if (c->prev)
        ptr<CBlock>(c->prev)->next = c->next;
    if (c->next)
        ptr<CBlock>(c->next)->prev = c->prev;
    free_mm(address(c), c->length);
}

} // namespace

namespace amos::w {

// BbInit: no bobs, the priority tables for n of them
bool bobs_init(uint16_t n)
{
    WData &d = wdata();
    d.BbDeb = 0;
    bobs_end();
    d.BbMax = n;
    uint32_t size = uint32_t(n) * 4;
    d.BbPrio = fast_mm(size);
    if (!d.BbPrio)
        return false;
    d.BbPrio2 = fast_mm(size);
    return d.BbPrio2 != 0;
}

// BbEnd
void bobs_end()
{
    WData &d = wdata();
    uint32_t size = uint32_t(d.BbMax) * 4;
    if (d.BbPrio)
        free_mm(d.BbPrio, size);
    if (d.BbPrio2)
        free_mm(d.BbPrio2, size);
}

} // namespace amos::w

extern "C" {
// SyCall functions
void w_sy_bob_set(AllRegs *r) { r->d[0] = set_bob(r); }

void w_sy_bob_off(AllRegs *r)
{
    amos::Bob *b = find_bob(r->d[1]);
    if (!b) {
        r->d[0] = 1;
        return;
    }
    act(b) = 0xFF;
    wdata().Actualise |= BitBobs;
    r->d[0] = 0;
}

void w_sy_bobs_off(AllRegs *r)
{
    for (amos::Bob *b = first_bob(); b; b = ptr<amos::Bob>(b->BbNext))
        act(b) = 0xFF;
    wdata().Actualise |= BitBobs;
    r->d[0] = 0;
}

void w_sy_bob_update(AllRegs *) { update(); }
void w_sy_bob_show(AllRegs *) { show(); }
void w_sy_bob_erase(AllRegs *) { erase(); }
void w_sy_bob_limit(AllRegs *r) { r->d[0] = limit_bobs(r); }

void w_sy_get_bob(AllRegs *r)
{
    r->d[0] = get_image(ptr<Screen>(r->a[1]), ptr<be_u32>(r->a[2]), r->d[2], r->d[3], r->d[4], r->d[5]);
}

void w_sy_mask(AllRegs *r) { r->d[0] = make_mask(ptr<be_u32>(r->a[2])); }
void w_sy_hot_spot(AllRegs *r) { r->d[0] = hot_spot(ptr<be_u32>(r->a[2]), r->d[1], r->d[2], r->d[3]); }

// BobXY: d1-d3 the x, y and image of bob d1 (low words)
void w_sy_bob_xy(AllRegs *r)
{
    amos::Bob *b = find_bob(r->d[1]);
    if (!b) {
        r->d[0] = 1;
        return;
    }
    r->d[1] = (r->d[1] & 0xFFFF0000) | b->BbX;
    r->d[2] = (r->d[2] & 0xFFFF0000) | b->BbY;
    r->d[3] = (r->d[3] & 0xFFFF0000) | b->BbI;
    r->d[0] = 0;
}

// BobPut: Put Bob: no erase for the next draws
void w_sy_bob_put(AllRegs *r)
{
    amos::Bob *b = find_bob(r->d[1]);
    if (!b) {
        r->d[0] = 1;
        return;
    }
    b->BbECpt = b->BbDecor;
    r->d[0] = 0;
}

void w_sy_paste(AllRegs *r)
{
    paste(r);
    r->d[0] = 0;
}

// TPrio: Priority On/Off (d1: >0 the current screen, 0 none, <0
// unchanged), Priority Reverse (d2, <0 unchanged)
void w_sy_priority(AllRegs *r)
{
    WData &d = wdata();
    if (int32_t(r->d[1]) >= 0)
        d.Priorite = r->d[1] ? d.EcCourant : 0;
    if (int32_t(r->d[2]) >= 0)
        d.PriRev = r->d[2];
    r->d[0] = 0;
}

// BbEcOff: the bobs of screen a0 deleted
void w_sy_bobs_screen_off(AllRegs *r)
{
    for (amos::Bob *b = first_bob(), *next; b; b = next) {
        next = ptr<amos::Bob>(b->BbNext);
        if (b->BbEc == r->a[0])
            delete_bob(b);
    }
    r->d[0] = 0;
}

// EcCall block functions
void w_ec_block_get(AllRegs *r) { r->d[0] = get_block(r); }

void w_ec_block_delete(AllRegs *r)
{
    Block *b = find_block(r->d[1]);
    if (!b) {
        r->d[0] = ErrBlock + 2;
        return;
    }
    free_block(b);
    r->d[0] = 0;
}

void w_ec_blocks_delete(AllRegs *r)
{
    while (wdata().AdBlocs)
        free_block(ptr<Block>(wdata().AdBlocs));
    r->d[0] = 0;
}

void w_ec_block_put(AllRegs *r) { r->d[0] = put_block(r); }

// RevBloc: flip block d1 (d2: the flip flags)
void w_ec_block_flip(AllRegs *r)
{
    Block *b = find_block(r->d[1]);
    if (!b) {
        r->d[0] = 2;
        return;
    }
    ptr<Image>(b->BlDesc[0])->hot_x &= 0x3FFF;
    flip(b->BlDesc, r->d[2]);
    r->d[0] = 0;
}

// RevTrap: flip the image entry a1 to the flags d1
void w_ec_flip(AllRegs *r) { flip(ptr<be_u32>(r->a[1]), r->d[1]); }

// MakeCBloc: Get CBlock d5, d1-d4
void w_ec_cblock_get(AllRegs *r)
{
    if (CBlock *old = find_cblock(r->d[5]))
        free_cblock(old);
    CBlock *c = nullptr;
    uint32_t error = compress(r->d[1], r->d[2], r->d[3], r->d[4], &c);
    r->d[0] = error;
    if (error)
        return;
    r->a[1] = address(c);
    c->number = r->d[5];
    WData &d = wdata();
    if (d.AdCBlocs)
        ptr<CBlock>(d.AdCBlocs)->prev = address(c);
    c->next = d.AdCBlocs;
    d.AdCBlocs = address(c);
}

// DrawCBloc: Put CBlock d5 at d1,d2
void w_ec_cblock_put(AllRegs *r)
{
    CBlock *c = find_cblock(r->d[5]);
    r->d[0] = c ? decompress(c, r->d[1], r->d[2]) : ErrBlock + 2;
}

void w_ec_cblock_delete(AllRegs *r)
{
    CBlock *c = find_cblock(r->d[5]);
    if (!c) {
        r->d[0] = ErrBlock + 2;
        return;
    }
    free_cblock(c);
    r->d[0] = 0;
}

void w_ec_cblocks_delete(AllRegs *r)
{
    while (wdata().AdCBlocs)
        free_cblock(ptr<CBlock>(wdata().AdCBlocs));
    r->d[0] = 0;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/w_bobs.inc"
#endif
