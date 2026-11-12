// AMCAF: the blitter commands (R03c-R04b: the limits of Blitter Copy, the busy flag, the clear and the
// area fill). They drive the blitter's registers (hal/chips.h: the chip on the Amiga, its emulation on
// another host) as the library does.

#include "ext/amcaf.h"
#include "hal/chips.h"
#include "hal/gfx.h"
#include "lib/banks.h"

using namespace amos;

extern "C" uint32_t lib_get_ec(RegFrame &r);

namespace {

// GetEc: the screen of a number (or a Logic() / Physic() value), error 47 if there is none
Screen *get_ec(int32_t n)
{
    Regs r;
    r.d[1] = uint32_t(n);
    lib_get_ec(reinterpret_cast<RegFrame &>(r));
    return ptr<Screen>(r.a[0]);
}

// The plane of a screen's logic bitmap, for a plane number below the screen's count (the original's compare
// is a signed one that lets a negative plane through: that is an illegal function call here)
uint32_t logic_plane(const Screen *s, int32_t plane)
{
    int16_t p = int16_t(plane);
    if (p < 0 || p >= int16_t(s->EcNPlan))
        fon_call();
    return s->EcLogic[p];
}

// R047's / R04b's rectangle: the first word, its count of words, its height. False when there is
// nothing in it
struct Words {
    uint16_t y, first, count, height;
};
bool word_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Words &w)
{
    uint16_t first = uint16_t(uint16_t(x1) >> 4);
    int16_t count = int16_t(uint16_t(uint16_t(uint16_t(x2) + 15) >> 4) - first);
    if (count <= 0)
        return false;
    int16_t height = int16_t(uint16_t(y2) - uint16_t(y1));
    if (height <= 0)
        return false;
    w = {uint16_t(y1), first, uint16_t(count), uint16_t(height)};
    return true;
}

// The blitter taken as the library takes it: the priority bit set until it is idle, then cleared
void take_blitter()
{
    volatile struct Custom &c = hal::custom();
    c.dmacon = 0x8400;
    hal::wait_blit();
    c.dmacon = 0x0400;
    hal::own_blitter();
}

// R047: Blitter Clear. The rectangle is that of the screen's planes; the plane of the screen
void clear(int32_t screen, int32_t plane, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    Words w;
    if (!word_rect(x1, y1, x2, y2, w))
        return;
    const Screen *s = get_ec(screen);
    uint32_t base = logic_plane(s, plane);
    uint16_t row = uint16_t(s->EcTx >> 3);
    uint32_t offset = uint32_t(w.y) * row + 2u * w.first;
    volatile struct Custom &c = hal::custom();
    take_blitter();
    c.bltcon0 = 0x01F0;
    c.bltcon1 = 0;
    c.bltadat = 0;
    c.bltafwm = 0xFFFF;
    c.bltalwm = 0xFFFF;
    c.bltdpt = ptr<uint8_t>(base + offset);
    c.bltdmod = uint16_t(row - 2 * w.count);
    hal::blit(uint16_t(w.height << 6) + w.count);
    hal::disown_blitter();
}

// R04b: Blitter Fill. The area between the set bits (exclusive fill, from the right), from the rectangle of
// the source plane into the same rectangle of the destination plane; both are done from their last word
void fill(int32_t screen, int32_t plane, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t to_screen,
          int32_t to_plane)
{
    const Screen *d = get_ec(to_screen);
    uint32_t dest = logic_plane(d, to_plane);
    uint16_t drow = uint16_t(d->EcTx >> 3);
    Words w;
    if (!word_rect(x1, y1, x2, y2, w))
        return;
    const Screen *a = get_ec(screen);
    uint32_t source = logic_plane(a, plane);
    uint16_t arow = uint16_t(a->EcTx >> 3);
    auto last_word = [&](uint16_t row) {
        uint16_t lines = uint16_t(w.y + w.height - 1);
        uint16_t words = uint16_t(w.first + w.count);
        return uint32_t(lines) * row + 2u * words - 2;
    };
    dest += last_word(drow);
    source += last_word(arow);
    volatile struct Custom &c = hal::custom();
    take_blitter();
    c.bltcon0 = 0x09F0;
    c.bltcon1 = 0x0012;
    c.bltafwm = 0xFFFF;
    c.bltalwm = 0xFFFF;
    c.bltapt = ptr<uint8_t>(source);
    c.bltdpt = ptr<uint8_t>(dest);
    c.bltamod = uint16_t(arow - 2 * w.count);
    c.bltdmod = uint16_t(drow - 2 * w.count);
    hal::blit(uint16_t(w.height << 6) + w.count);
    hal::disown_blitter();
}

void copy_limits(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    uint16_t first = uint16_t(uint16_t(x1) >> 4);
    int16_t count = int16_t(uint16_t(uint16_t(uint16_t(x2) + 15) >> 4) - first);
    if (count <= 0)
        fon_call();
    int16_t height = int16_t(uint16_t(y2) - uint16_t(y1));
    if (height <= 0)
        fon_call();
    poke_w(amcaf::state_at(amcaf::CopyY), uint16_t(y1));
    poke_w(amcaf::state_at(amcaf::CopyWords), uint16_t(count));
    poke_w(amcaf::state_at(amcaf::CopyHeight), uint16_t(height));
    poke_w(amcaf::state_at(amcaf::CopyX), first);
}

} // namespace

extern "C" {

// =BLITTER BUSY: -1 while the blitter works
int32_t amcaf_blitter_busy() { return hal::custom().dmaconr & 0x4000 ? -1 : 0; }

// BLITTER WAIT: until the blitter is idle (its priority bit set meanwhile, then cleared)
void amcaf_blitter_wait()
{
    volatile struct Custom &c = hal::custom();
    c.dmacon = 0x8400;
    hal::wait_blit();
    c.dmacon = 0x0400;
}

// BLITTER CLEAR screen,plane [,x1,y1 TO x2,y2]: the bits of the rectangle (all the screen) cleared
void amcaf_blitter_clear_2(int32_t screen, int32_t plane)
{
    const Screen *s = get_ec(screen);
    clear(screen, plane, 0, 0, s->EcTx, s->EcTy);
}
void amcaf_blitter_clear_6(int32_t screen, int32_t plane, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    clear(screen, plane, x1, y1, x2, y2);
}

// BLITTER FILL screen,plane [,x1,y1,x2,y2] [TO screen,plane]
void amcaf_blitter_fill_2(int32_t screen, int32_t plane)
{
    const Screen *s = get_ec(screen);
    fill(screen, plane, 0, 0, s->EcTx, s->EcTy, screen, plane);
}
void amcaf_blitter_fill_4(int32_t screen, int32_t plane, int32_t to_screen, int32_t to_plane)
{
    const Screen *s = get_ec(screen);
    fill(screen, plane, 0, 0, s->EcTx, s->EcTy, to_screen, to_plane);
}
void amcaf_blitter_fill_6(int32_t screen, int32_t plane, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    fill(screen, plane, x1, y1, x2, y2, screen, plane);
}
void amcaf_blitter_fill_8(int32_t screen, int32_t plane, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                          int32_t to_screen, int32_t to_plane)
{
    fill(screen, plane, x1, y1, x2, y2, to_screen, to_plane);
}

// BLITTER COPY LIMIT screen; BLITTER COPY LIMIT x1,y1 TO x2,y2
void amcaf_blitter_copy_limit_1(int32_t screen)
{
    const Screen *s = get_ec(screen);
    poke_l(amcaf::state_at(amcaf::CopyX), 0);
    poke_w(amcaf::state_at(amcaf::CopyWords), uint16_t(s->EcTx >> 4));
    poke_w(amcaf::state_at(amcaf::CopyHeight), s->EcTy);
}
void amcaf_blitter_copy_limit_4(int32_t x1, int32_t y1, int32_t x2, int32_t y2) { copy_limits(x1, y1, x2, y2); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_blitter.inc"
#endif
