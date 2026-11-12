// AMCAF: the drawing commands (R15a-R15d: Turbo Plot, Turbo Point, Turbo Draw). They go to the planes of the
// logic screen without AMOS's own clipping, and the lines are the blitter's (hal/chips.h).

#include "ext/amcaf.h"
#include "hal/chips.h"
#include "lib/banks.h"

using namespace amos;

namespace {

Screen *current()
{
    return ptr<Screen>(idata().ScOnAd);
}

// the bit of pixel x in its byte, the byte's place in the plane for the pixel (x, y); false outside the
// screen (the compares are signed words, a negative coordinate is out)
struct Spot {
    uint32_t offset;
    uint8_t mask;
};
bool spot(const Screen *s, int32_t x, int32_t y, Spot &p)
{
    if (y < 0 || x < 0)
        return false;
    if (int16_t(y) >= int16_t(s->EcTy) || int16_t(x) >= int16_t(s->EcTx))
        return false;
    uint32_t row = uint16_t(s->EcTx) >> 3;
    uint32_t product = uint32_t(uint16_t(y)) * row;
    p.offset = product + uint16_t(uint16_t(x) >> 3);
    p.mask = uint8_t(0x80 >> (uint16_t(x) & 7));
    return true;
}

} // namespace

extern "C" {

// TURBO PLOT x,y,colour: a pixel in the planes of the logic screen, the colour's bits in them. Out of the
// screen nothing happens (a negative colour has all its bits set)
void amcaf_turbo_plot(int32_t x, int32_t y, int32_t colour)
{
    if (y < 0 || x < 0)
        return;
    Screen *s = current();
    if (!uint16_t(address(s)))
        error(47);
    Spot p;
    if (!spot(s, x, y, p))
        return;
    uint16_t planes = s->EcNPlan;
    for (uint16_t i = 0; i < planes; i++) {
        ContextCursor at = decode_context_cursor(s->EcLogic[i] + p.offset);
        uint8_t v = host::detail::read_cursor_byte(at);
        v = colour >> i & 1 ? uint8_t(v | p.mask) : uint8_t(v & ~p.mask);
        host::detail::write_cursor_byte(at, v);
    }
}

// =TURBO POINT(x,y): the colour of the pixel in the logic screen, -1 outside it
int32_t amcaf_turbo_point(int32_t x, int32_t y)
{
    if (y < 0 || x < 0)
        return -1;
    const Screen *s = current();
    Spot p;
    if (!s || !spot(s, x, y, p))
        return -1;
    int32_t colour = 0;
    uint16_t planes = s->EcNPlan;
    for (uint16_t i = 0; i < planes; i++)
        if (host::detail::read_cursor_byte(decode_context_cursor(s->EcLogic[i] + p.offset)) & p.mask)
            colour |= 1 << i;
    return colour;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_draw.inc"
#endif
