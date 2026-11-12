// AMCAF: the drawing commands (R15a-R15d: Turbo Plot, Turbo Point, Turbo Draw). They go to the planes of the
// logic screen without AMOS's own clipping, and the lines are the blitter's (hal/chips.h).

#include "ext/amcaf.h"
#include "hal/chips.h"
#include "hal/gfx.h"
#include "lib/banks.h"

#include <graphics/rastport.h>

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

// 16-bit register moves: the low word of a long register replaced, the high word kept
inline void set_w(uint32_t &reg, int32_t v) { reg = (reg & 0xFFFF0000u) | uint16_t(v); }
inline int16_t w(uint32_t reg) { return int16_t(reg); }

// DIVS.W: the 32-bit dividend by a word; the quotient in the low word and the remainder above it, or the
// register as it was when the quotient does not fit
void divs_w(uint32_t &reg, int16_t divisor)
{
    if (!divisor)
        error(23);
    int32_t dividend = int32_t(reg);
    int64_t q = int64_t(dividend) / divisor;
    if (q > 32767 || q < -32768)
        return;
    int32_t r = int32_t(dividend - q * divisor);
    reg = uint32_t(uint16_t(r)) << 16 | uint16_t(int16_t(q));
}

// (a * b * 2 / c) rounded, as the library's clipping does it: MULS.W, the product doubled, DIVS.W, the
// quotient halved with its low bit added back (ASR.W then ADDX.W)
int16_t scaled(int16_t a, int16_t b, int16_t c)
{
    uint32_t d4 = uint32_t(int32_t(a) * int32_t(b));
    d4 <<= 1;
    divs_w(d4, c);
    int16_t q = int16_t(d4);
    int x = q & 1;
    return int16_t((q >> 1) + x);
}

// R15b: Turbo Draw's lines, drawn by the blitter's line mode in each plane the mask names. The ends are
// clipped to the screen (x from 0, y from 0 to the size less one) as the library does, in 16-bit words.
// A negative mask: only the first pixel of each row, by XOR (the edges of a polygon to fill); it also keeps
// its line along the right border when it is cut by it
void draw(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour, int32_t mask)
{
    if (mask == 0)
        return;
    Screen *s = current();
    uint32_t d0 = uint32_t(x1), d1 = uint32_t(y1), d2 = uint32_t(x2), d3 = uint32_t(y2);
    uint32_t d6 = uint32_t(mask);
    auto exchange = [](uint32_t &a, uint32_t &b) { uint32_t t = a; a = b; b = t; };
    auto swap01 = [&] { exchange(d0, d2); exchange(d1, d3); };
    int16_t d4, d7;
    // x from the left
    if (int32_t(d0) < 0)
        goto x_left;
    goto x_right_ok;
x_left:
    if (w(d2) < 0)
        return;
    if (w(d0) == w(d2)) {
        d0 = 0;
    } else {
        int16_t dy = int16_t(w(d3) - w(d1)), dx = int16_t(w(d2) - w(d0));
        int16_t q = scaled(dy, w(d2), dx);
        set_w(d1, w(d3) - q);
        d0 = 0;
    }
x_right_ok:
    if (w(d2) < 0) {
        swap01();
        goto x_left;
    }
    // y from the top
    if (w(d1) < 0)
        goto y_top;
    goto y_top_ok;
y_top:
    if (w(d3) < 0)
        return;
    if (w(d1) == w(d3)) {
        d1 = 0;
    } else {
        int16_t dx = int16_t(w(d2) - w(d0)), dy = int16_t(w(d3) - w(d1));
        int16_t q = scaled(dx, w(d3), dy);
        set_w(d0, w(d2) - q);
        d1 = 0;
    }
y_top_ok:
    if (w(d3) < 0) {
        swap01();
        goto y_top;
    }
    // x to the right
    d4 = int16_t(s->EcTx - 1);
    if (uint16_t(w(d0)) <= uint16_t(d4))
        goto x_end;
x_over:
    if (w(d2) > d4) {
        if (int32_t(d6) >= 0)
            return;
        set_w(d0, d4);
        set_w(d2, d4);
        goto x_end;
    }
    if (w(d1) == w(d3)) {
        set_w(d0, d4);
        goto x_end;
    }
    d7 = int16_t(w(d3) - w(d1));
    d7 = scaled(d7, int16_t(d4 - w(d2)), int16_t(w(d2) - w(d0)));
    d4 = int16_t(s->EcTx - 1);
    set_w(d0, d4);
    if (int32_t(d6) < 0) {
        uint32_t keep[4] = {d0, d1, d2, d3};
        draw(w(d0), w(d1), w(d0), w(d3) + d7, colour, int32_t(d6));
        d0 = keep[0];
        d1 = keep[1];
        d2 = keep[2];
        d3 = keep[3];
    }
    set_w(d1, w(d3) + d7);
x_end:
    if (uint16_t(w(d2)) > uint16_t(d4)) {
        swap01();
        goto x_over;
    }
    // y to the bottom
    d4 = int16_t(s->EcTy - 1);
    if (uint16_t(w(d1)) <= uint16_t(d4))
        goto y_end;
y_over:
    if (w(d3) > d4)
        return;
    if (w(d0) == w(d2)) {
        set_w(d1, d4);
        goto y_end;
    }
    d7 = int16_t(w(d2) - w(d0));
    d7 = scaled(d7, int16_t(d4 - w(d3)), int16_t(w(d3) - w(d1)));
    set_w(d0, w(d2) + d7);
    d4 = int16_t(s->EcTy - 1);
    set_w(d1, d4);
y_end:
    if (uint16_t(w(d3)) > uint16_t(d4)) {
        swap01();
        goto y_over;
    }
    {
        uint32_t mode = 0x0BCA0001;
        if (int32_t(d6) < 0) {
            if (w(d1) == w(d3))
                return;
            mode = 0x0B480003;
            d6 = uint32_t(-int32_t(d6));
        }
        if (!(w(d3) > w(d1)))
            swap01();
        uint16_t row = uint16_t(s->EcTx) >> 3;
        int16_t start_x = w(d0);
        uint32_t d5 = uint16_t(w(d1)) * row;
        int32_t offset = int16_t(uint16_t(d5));
        uint16_t xw = uint16_t(uint16_t(start_x) >> 4);
        offset += int16_t(uint16_t(xw + xw));
        set_w(d2, w(d2) - w(d0));
        set_w(d3, w(d3) - w(d1));
        uint8_t bit = uint8_t(uint32_t(d0) & 15);
        uint32_t con = uint32_t(uint32_t(d0) & 15) << 28;
        set_w(con, 4);
        if (w(d2) < 0) {
            set_w(con, w(con) + 1);
            set_w(d2, -w(d2));
        }
        if (!(w(d3) <= w(d2))) {
            exchange(d2, d3);
            set_w(con, w(con) - 4);
            set_w(con, w(con) + w(con));
        }
        int16_t d4w = int16_t(w(d3) - w(d2));
        d4w = int16_t(d4w << 2);
        set_w(d3, w(d3) + w(d3));
        int16_t error = int16_t(w(d3) - w(d2));
        if (error < 0)
            con |= 16;
        set_w(d3, w(d3) + w(d3));
        set_w(con, int16_t(w(con) << 2));
        uint16_t size = uint16_t((uint16_t(w(d2) + 1) << 6) + 2);
        uint32_t modulo = uint32_t(uint16_t(w(d3))) << 16 | uint16_t(d4w);
        uint32_t flags = con | mode;
        uint16_t special = uint16_t(mode);
        volatile struct Custom &c = hal::custom();
        hal::own_blitter();
        for (int plane = 5; plane >= 0; plane--) {
            if (!(d6 >> plane & 1))
                continue;
            c.dmacon = 0x8400;
            hal::wait_blit();
            c.dmacon = 0x0400;
            uint32_t base = s->EcLogic[plane];
            if (!base)
                continue;
            base += uint32_t(offset);
            if (special == 3) {
                ContextCursor at = decode_context_cursor(base + (bit & 8 ? 1 : 0));
                uint8_t v = host::detail::read_cursor_byte(at);
                host::detail::write_cursor_byte(at, uint8_t(v ^ (1 << (~bit & 7))));
            }
            const RastPort *rp = ptr<RastPort>(s->Ec_RastPort);
            c.bltbdat = (uint32_t(colour) >> plane & 1) ? uint16_t(rp->LinePtrn) : uint16_t(0);
            c.bltafwm = 0xFFFF;
            c.bltalwm = 0xFFFF;
            c.bltcmod = row;
            c.bltdmod = row;
            c.bltbmod = uint16_t(modulo >> 16);
            c.bltamod = uint16_t(modulo);
            c.bltcpt = ptr<uint8_t>(base);
            c.bltdpt = ptr<uint8_t>(base);
            c.bltcon0 = uint16_t(flags >> 16);
            c.bltcon1 = uint16_t(flags);
            hal::custom_word(0x52) = uint16_t(error);
            c.bltadat = 0x8000;
            hal::blit(size);
        }
        hal::disown_blitter();
    }
}

} // namespace

extern "C" {

// TURBO DRAW x1,y1 TO x2,y2,colour [,planes]: a line in the planes of the logic screen named by the mask
// (all of them without it), each with the colour's bit: the line pattern, or nothing at all (clearing)
void amcaf_turbo_draw_5(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour)
{
    static const uint8_t all[6] = {1, 3, 7, 15, 31, 63};
    int32_t planes = current()->EcNPlan;
    draw(x1, y1, x2, y2, colour, all[planes < 1 ? 0 : planes > 6 ? 5 : planes - 1]);
}
void amcaf_turbo_draw_6(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t colour, int32_t mask)
{
    draw(x1, y1, x2, y2, colour, mask);
}

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
