// The copper list builder of AMOS.library (EcCall CopMake, CopForce): the
// pending changes of the screens' display (EcAW/EcAWT/EcAV) are taken,
// the screens cut into horizontal slices by priority (T_EcBuf, then the
// list of the slices shown in T_EcCop), and the logic copper list written
// from it: per screen its palette, bitplanes, display window and control
// registers, the rainbows mixed in line by line. The lists are then
// swapped.
//
// The places of the palettes and bitplane pointers in the list go into
// T_CopMark, for the colour functions and the VBL interrupt (screen swaps,
// interlace), which change the list where it is.
//
// Written with the labels and register roles of the assembler (+W.s
// EcCopper), which it follows step by step.

#include "w/w.h"

using namespace amos;
using namespace amos::w;

namespace {

constexpr uint16_t EcYBase = 0x1000, EcYStrt = EcYBase + 26;
constexpr uint16_t PalMax = 16;
constexpr uint32_t EcMax = 12, CopL1 = 16 * 4 * 2, CopL2 = 16 * 4;
constexpr uint32_t NbRain = 4;
constexpr uint16_t Hide = 1 << (8 + 7);
constexpr uint16_t Interlace = 4;

// Custom registers
enum : uint16_t {
    DmaCon = 0x96, Color00 = 0x180, BplCon0 = 0x100, BplCon1 = 0x102, BplCon2 = 0x104, Bpl1PtH = 0x0E0,
    Bpl1Mod = 0x108, Bpl2Mod = 0x10A, DiwStrt = 0x08E, DiwStop = 0x090, DdfStrt = 0x092, DdfStop = 0x094,
};

struct Rainbow {
    uint16_t RnDY, RnFY, RnTY, RnBase, RnColor, RnLong;
    uint32_t RnBuf;
    uint16_t RnAct, RnX, RnY, RnI;
};
static_assert(sizeof(Rainbow) == 24);

uint8_t &high(uint16_t &w) { return reinterpret_cast<uint8_t *>(&w)[0]; }

Rainbow *rainbows() { return reinterpret_cast<Rainbow *>(wdata().RainTable); }

struct Builder {
    WData &d;
    uint16_t *a1;               // the copper list being written
    const uint16_t *a2;         // T_EcCop being read
    uint8_t *a4;                // the palette mark of the last screen
    // The rainbows: the colour, its register, the current one's end, its
    // colours
    uint32_t d3;
    uint16_t d4, d5, d7;
    const uint16_t *a3, *a6;
    uint32_t d6;

    void emit(uint16_t v) { *a1++ = v; }
    void emit(uint16_t reg, uint16_t v)
    {
        *a1++ = reg;
        *a1++ = v;
    }

    // WaitD2: wait for line y (past 255 once)
    void wait(uint16_t y)
    {
        if (y >= 256 && !d.Cop255) {
            emit(0xFFDF, 0xFFFE);
            d.Cop255++;
        }
        emit(uint16_t(y << 8) | 3, 0xFFFE);
    }

    // EcCopBa: the end of a screen at line y
    void screen_end(uint16_t y)
    {
        wait(y - EcYBase);
        emit(DmaCon, 0x0100);
        emit(Color00, d.EcFond);
    }

    // A pair of the marks list at list (pairs of the bitplane moves and
    // the offset, ending with 0)
    static void mark_swap(uint8_t *list, uint32_t moves, uint32_t offset)
    {
        for (;;) {
            uint32_t v = peek_l(list);
            list += 8;
            if (!v)
                break;
        }
        poke_l(list, 0);
        poke_l(list - 8, moves);
        poke_l(list - 4, offset);
    }

    // A mark into a zero-terminated list of longs
    static uint8_t *mark(uint8_t *list, uint32_t value)
    {
        while (peek_l(list))
            list += 4;
        list += 4;
        poke_l(list, 0);
        list -= 4;
        poke_l(list, value);
        return list;
    }

    uint32_t plane_moves(const Screen *s, uint16_t first_register, uint16_t step, uint32_t offset)
    {
        uint32_t moves = address(a1);
        uint16_t reg = first_register;
        for (uint16_t p = 0; p < s->EcNPlan; p++, reg += step) {
            uint32_t at = s->EcPhysic[p] + offset;
            emit(reg, uint16_t(at >> 16));
            emit(reg + 2, uint16_t(at));
        }
        return moves;
    }

    // The offset of the first line shown: line in the window, then the
    // display offset (EcVY/EcVX)
    static uint32_t display_offset(const Screen *s, uint16_t line)
    {
        uint32_t offset = uint32_t(uint16_t(line + s->EcVY)) * s->EcTLigne;
        return (offset & 0xFFFF0000) | uint16_t(offset + uint16_t(uint16_t(s->EcVX >> 4) << 1));
    }

    // MkC1: the display window within what the hardware can show
    void window(Screen *s)
    {
        uint16_t limit = int16_t(s->EcCon0) < 0 ? 465 : 465 + 16;
        uint16_t x = s->EcWX + 1, w = s->EcWTx;
        uint16_t end = x + w;
        if (end >= limit) {
            end = end - limit + 16;
            w -= end;
        } else if (end <= 176) {
            end -= 176;
            x -= end;
        }
        s->EcWXr = x;
        s->EcWTxr = w;
        emit(DiwStrt, x | 0x0100);
        emit(DiwStop, ((x + w) & 0xFF) | 0x3700);
    }

    static uint16_t modulo(const Screen *s, const Screen *mode)
    {
        uint16_t shown = s->EcWTxr >> 3;
        if (!(mode->EcCon0 & 0x8000))
            shown >>= 1;
        int16_t m = int16_t(s->EcTLigne - (shown << 1));
        return m < 0 ? 0 : m;
    }

    // EcCopHo: the start of screen s at line y
    void screen_start(uint16_t y, Screen *s)
    {
        uint16_t line = y - s->EcWY + 1;
        if (s->EcCon0 & Interlace)
            line <<= 1;
        wait(y - EcYBase);
        emit(DmaCon, 0x0100);
        uint32_t palette = address(a1);
        for (uint16_t c = 0; c < PalMax; c++)
            emit(Color00 + 2 * c, s->EcPal[c]);

        if (uint16_t n = s->EcDual) {
            if (dual(line, s, n))
                goto FiniCop;
        }
        {
            uint32_t offset = display_offset(s, line);
            uint32_t moves = plane_moves(s, Bpl1PtH, 4, offset);
            if (s->EcNumber < 10)
                mark_swap(d.CopMark + CopL1 * EcMax + (s->EcNumber << 6), moves, offset);
            window(s);
            uint16_t mod = modulo(s, s);
            uint16_t x = s->EcWXr, w = s->EcWTxr, scroll = s->EcVX;
            if (!(s->EcCon0 & 0x8000)) {
                x = uint16_t(uint16_t(x - 17) >> 1) & 0xFFF8;
                w = uint16_t((w >> 1) - 8 + x);
                scroll &= 15;
                if (scroll) {
                    x -= 8;
                    mod -= 2;
                    scroll = 16 - scroll;
                }
            } else {
                x = uint16_t(uint16_t(x - 9) >> 1) & 0xFFFC;
                w = uint16_t((w >> 1) - 8 + x);
                scroll = (scroll & 15) >> 1;
                if (scroll) {
                    x -= 4;
                    mod -= 4;
                    scroll = 8 - scroll;
                }
            }
            emit(DdfStrt, x);
            emit(DdfStop, w);
            uint16_t con0 = s->EcCon0;
            if (con0 & Interlace)
                mod += s->EcTx >> 3;
            emit(Bpl1Mod, mod);
            emit(Bpl2Mod, mod);
            emit(BplCon0, con0 | d.InterInter);
            emit(BplCon1, uint16_t(scroll << 4) | scroll);
            emit(BplCon2, s->EcCon2);
        }
    FiniCop:
        // The DMA back on, and the second half of the palette, from the
        // next line (unless another screen starts there)
        uint16_t next = y + 1;
        int16_t following = int16_t(*a2);
        if (following < 0)
            following = -following;
        if (uint16_t(following) != next) {
            wait(next - EcYBase);
            emit(DmaCon, 0x8300);
            uint32_t second = address(a1);
            for (uint16_t c = PalMax; c < 32; c++)
                emit(Color00 + 2 * c, s->EcPal[c]);
            mark(d.CopMark + 64 + (s->EcNumber << 7), second - 4 * PalMax);
        }
        a4 = mark(d.CopMark + (s->EcNumber << 7), palette);
    }

    // CreeDual: screen s (first playfield) with its second, number n-1.
    // False if that one is gone: s becomes a plain screen.
    bool dual(uint16_t line, Screen *s, uint16_t n)
    {
        Screen *t = ptr<Screen>(peek_l(reinterpret_cast<const uint8_t *>(d.EcAdr) - 4 + int16_t(n << 2)));
        if (!t) {
            s->EcDual = 0;
            s->EcCon0 = (s->EcCon0 & 0x8BFF) | uint16_t(s->EcNPlan << 12);
            return false;
        }
        uint32_t offset = display_offset(s, line);
        uint32_t moves = plane_moves(s, Bpl1PtH, 8, offset);
        if (s->EcNumber < 8)
            mark_swap(d.CopMark + CopL1 * EcMax + (s->EcNumber << 6), moves, offset);
        offset = display_offset(t, line);
        moves = plane_moves(t, Bpl1PtH + 4, 8, offset);
        if (t->EcNumber < 8)
            mark_swap(d.CopMark + CopL1 * EcMax + (t->EcNumber << 6), moves, offset);
        window(s);
        uint16_t mod1 = modulo(s, s), mod2 = modulo(t, s);
        uint16_t v1 = s->EcVX, v2 = t->EcVX;
        if (!(v1 & 15))
            v2 &= 0xFFF0;
        if (!(v2 & 15))
            v1 &= 0xFFF0;
        uint16_t x = s->EcWXr, w = s->EcWTxr;
        if (!(s->EcCon0 & 0x8000)) {
            x = uint16_t(uint16_t(x - 17) >> 1) & 0xFFF8;
            w = uint16_t((w >> 1) - 8 + x);
            v1 &= 15;
            v2 &= 15;
            if (v2) {
                x -= 8;
                mod1 -= 2;
                mod2 -= 2;
                v1 = 16 - v1;
                v2 = 16 - v2;
            }
        } else {
            x = uint16_t(uint16_t(x - 9) >> 1) & 0xFFFC;
            w = uint16_t((w >> 1) - 8 + x);
            v1 = (v1 & 15) >> 1;
            v2 = (v2 & 15) >> 1;
            if (v2) {
                x -= 4;
                mod1 -= 4;
                mod2 -= 4;
                v1 = 8 - v1;
                v2 = 8 - v2;
            }
        }
        emit(DdfStrt, x);
        emit(DdfStop, w);
        emit(Bpl1Mod, mod1);
        emit(Bpl2Mod, mod2);
        emit(BplCon0, s->EcCon0 | d.InterInter);
        emit(BplCon1, uint16_t(v2 << 4) | v1);
        emit(BplCon2, s->EcCon2);
        return true;
    }

    // ---- rainbows ----------------------------------------------------------

    void rainbow_next()
    {
        if (++a3 >= a6)
            a3 = ptr<uint16_t>(d6);
    }

    // Rain: rainbow lines from line y0 up to y1 (d0 to d1); returns y1
    uint16_t rain(uint16_t y0, uint16_t y1)
    {
        uint16_t y = y0;
        for (;;) {
            // Rain
            if (y >= d7) {
                // RainNX: the colour back after a rainbow, then the next
                if (int32_t(d3) >= 0) {
                    if (int16_t(d3) < 0)
                        emit(Color00, d.EcFond);
                    else
                        emit(d5, uint16_t(d3));
                    d3 |= 0x80000000;
                }
                Rainbow *r = rainbows();
                Rainbow *in = nullptr;
                for (uint32_t i = 0; i < NbRain; i++, r++)
                    if (y >= r->RnDY && y < r->RnFY) {
                        in = r;
                        break;
                    }
                if (!in) {
                    uint16_t top = y1;
                    for (uint32_t i = 0; i < NbRain; i++) {
                        Rainbow *q = rainbows() + i;
                        if (y >= q->RnFY || y1 < q->RnDY || top < q->RnDY)
                            continue;
                        top = q->RnDY;
                        in = q;
                    }
                    d7 = 0;
                    if (!in)
                        return y1;
                    y = in->RnDY;
                }
                // RainN5: the colours from the line
                uint16_t at = uint16_t((y - in->RnDY + in->RnBase) << 1);
                d6 = in->RnBuf;
                a3 = ptr<uint16_t>(in->RnBuf);
                a6 = reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(a3) + int16_t(in->RnLong));
                a3 = reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(a3) + int16_t(at));
                while (a3 >= a6)
                    a3 = reinterpret_cast<const uint16_t *>(reinterpret_cast<const uint8_t *>(a3)
                                                            - int16_t(in->RnLong));
                d7 = in->RnFY;
                uint16_t old = d4;
                d4 = in->RnColor << 2;
                d5 = Color00 + (in->RnColor << 1);
                if (int16_t(d3) >= 0 && d4 != old) {
                    const uint8_t *pal = ptr<uint8_t>(peek_l(a4));
                    d3 = (d3 & 0xFFFF0000) | peek_w(pal + 2 + int16_t(d4));
                }
                d3 &= 0x7FFFFFFF;
            }
            // RainD0: a line of the rainbow
            if (y >= y1)
                return y1;
            uint16_t line = y - EcYBase;
            if (line >= 256 && !d.Cop255) {
                emit(0xFFE1, 0xFFFE);
                d.Cop255++;
            }
            emit(uint16_t(line << 8) | 3, 0xFFFE);
            emit(d5, *a3);
            rainbow_next();
            y++;
        }
    }

    // The list with the rainbows
    void with_rainbows()
    {
        a2 = ptr<uint16_t>(d.EcCop);
        uint16_t y = EcYBase;
        d3 = 0xFFFFFFFF;
        d4 = 0xFFFF;
        d7 = 0;
        for (;;) {
            int16_t e = int16_t(*a2++);
            if (!e)
                break;
            if (e > 0) {
                // Rain1: a screen starts
                y = rain(y, e);
                Screen *s = ptr<Screen>(peek_l(a2));
                a2 += 2;
                uint32_t keep3 = d3;
                uint16_t keep4 = d4, keep5 = d5, keep7 = d7;
                uint32_t keep6 = d6;
                screen_start(y, s);
                d3 = keep3;
                d4 = keep4;
                d5 = keep5;
                d6 = keep6;
                d7 = keep7;
                d3 &= 0xFFFF0000;
                uint8_t *pal = nullptr;
                if (int16_t(d4) >= 0) {
                    if (d4 >= PalMax * 4)
                        a4 += 64;
                    pal = ptr<uint8_t>(peek_l(a4));
                    d3 = (d3 & 0xFFFF0000) | peek_w(pal + 2 + int16_t(d4));
                    d3 &= 0x7FFFFFFF;
                }
                // Rain1e
                if (y < d7) {
                    poke_w(pal + 2 + int16_t(d4), *a3);
                    rainbow_next();
                }
                y++;
                // Rain1a
                int16_t f = int16_t(*a2);
                if (f < 0)
                    f = -f;
                if (uint16_t(f) == y)
                    continue;
                if (y >= d7) {
                    y++;
                    continue;
                }
                emit(d5, *a3);
                rainbow_next();
                y++;
                continue;
            }
            // Rain2: a screen ends
            uint16_t end = -e;
            y = rain(y, end);
            screen_end(y);
            // (Rain1b then compares the line with itself: on to the next)
            if (d4)
                continue;
            d3 = (d3 & 0xFFFF0000) | d.EcFond;
            if (y < d7) {
                uint16_t *c = a1;
                while (*--c != Color00)
                    ;
                c[1] = *a3;
                rainbow_next();
            }
            y++;
        }
        // Rain3
        a2--;
        if (a2 == ptr<uint16_t>(d.EcCop)) {
            y = rain(y, d.EcYMax - 1);
            screen_end(y);
        }
    }
};

// RainA1: the changes of the rainbows (Rainbow), and how many are shown
uint16_t update_rainbows(WData &d)
{
    uint16_t shown = 0, exist = 0;
    Rainbow *r = rainbows();
    for (uint32_t i = 0; i < NbRain; i++, r++) {
        if (!r->RnLong)
            continue;
        exist++;
        if (int16_t(r->RnI) < 0)
            continue;
        shown++;
        uint8_t act = high(r->RnAct);
        if (!act)
            continue;
        high(r->RnAct) = 0;
        if (act & 1) {
            r->RnTY = r->RnI;
            act |= 4;
        }
        if (act & 4) {
            r->RnDY = 0;
            r->RnFY = 0;
            uint16_t y = r->RnY;
            if (y < 28)
                y = 28;
            y += EcYBase;
            r->RnDY = y;
            r->RnFY = y + r->RnTY;
        }
        if (act & 2) {
            if (uint16_t(r->RnX << 1) < r->RnLong)
                r->RnBase = r->RnX;
        }
    }
    d.RainBow = exist;
    return shown;
}

// The display changes of the screens (EcAct). Returns whether the slices
// must be computed again.
uint16_t take_changes(WData &d)
{
    uint16_t again = d.EcYAct;
    for (const uint32_t *p = d.EcPri;; p++) {
        uint32_t v = *p;
        if (!v)
            continue;
        if (int32_t(v) < 0)
            break;
        Screen *s = ptr<Screen>(v);
        if (uint8_t c = high(s->EcAW)) {
            if (c & 4) {
                int16_t y = int16_t(s->EcAWY + EcYBase);
                s->EcWY = y < 0 ? 0 : y;
                again++;
            }
            if (c & 2)
                s->EcWX = s->EcAWX & 0xFFF0;
            s->EcAW = 0;
        }
        if (uint8_t c = high(s->EcAWT)) {
            if (c & 4) {
                if (uint16_t h = s->EcAWTY) {
                    if (h >= s->EcTy)
                        h = s->EcTy;
                    if (s->EcCon0 & Interlace)
                        h >>= 1;
                    s->EcWTy = h;
                    again++;
                }
            }
            if (c & 2) {
                if (uint16_t w = s->EcAWTX & 0xFFF0) {
                    uint16_t max = s->EcTx;
                    if (int16_t(s->EcCon0) < 0)
                        max >>= 1;
                    if (w >= max)
                        w = max;
                    s->EcWTx = w;
                }
            }
            s->EcAWT = 0;
        }
        if (uint8_t c = high(s->EcAV)) {
            if (c & 4)
                s->EcVY = s->EcAVY;
            if (c & 2)
                s->EcVX = s->EcAVX;
            s->EcAV = 0;
        }
    }
    return again;
}

// MkD: the screens cut into slices: (line, end, number * 4 in T_EcPri,
// bit 15 for an end), the top of the display marked, -1 at the end
void cut(WData &d)
{
    uint16_t *out = d.EcBuf;
    auto emit = [&](uint16_t a, uint16_t b, uint16_t c) {
        *out++ = a;
        *out++ = b;
        *out++ = c;
    };
    uint16_t top = 0;
    uint16_t end_of = 0;
    for (;;) {
        uint16_t next = 10000, which = 0xFFFF;
        uint16_t index = 0;
        for (const uint32_t *p = d.EcPri;; p++) {
            index += 4;
            uint32_t v = *p;
            if (int32_t(v) < 0)
                break;
            if (!v)
                continue;
            const Screen *s = ptr<Screen>(v);
            if (s->EcFlags & Hide)
                continue;
            uint16_t line = s->EcWY - 1;
            if (line > top && line < next) {
                next = line;
                end_of = next + s->EcWTy + 1;
                which = index;
                continue;
            }
            line += s->EcWTy + 1;
            if (line <= top || line >= next)
                continue;
            next = line;
            which = index | 0x8000;
        }
        if (which == 0xFFFF)
            break;
        if (top < EcYStrt - 1 && next >= EcYStrt - 1)
            emit(EcYStrt - 1, 0, 0x8000);
        emit(next, end_of, which);
        // Other screens starting on that line
        which &= 0x7FFF;
        index = 0;
        for (const uint32_t *p = d.EcPri;; p++) {
            index += 4;
            uint32_t v = *p;
            if (int32_t(v) < 0)
                break;
            if (!v)
                continue;
            const Screen *s = ptr<Screen>(v);
            if (s->EcFlags & Hide)
                continue;
            if (uint16_t(s->EcWY - 1) != next || index == which)
                continue;
            emit(next, uint16_t(next + 1 + s->EcWTy), index);
        }
        top = next;
    }
    *out = 0xFFFF;
}

// MkA: the slices shown, into T_EcCop: (line, screen) for a start, -line
// for an end, 0 at the end
void slices(WData &d)
{
    d.InterInter = 0;
    uint16_t *out = ptr<uint16_t>(d.EcCop);
    const uint16_t *const first = d.EcBuf;
    const uint16_t *a3 = first;
    uint16_t y, which;
    int16_t w;
    const uint16_t *a0;
    uint16_t limit;
MkA1:
    y = a3[0];
    if (int16_t(y) < 0)
        goto MkAFin;
    w = int16_t(a3[2]);
    if (w < 0)
        goto MkA4;
    which = w;
    // A start: is it behind a screen shown already?
    for (a0 = first; a0 < a3; a0 += 3) {
        if (int16_t(a0[2]) < 0)
            continue;
        if (y < a0[0] || y >= a0[1])
            continue;
        if (which >= a0[2])
            goto MkA10;
    }
    goto MkA8;
MkA4:
    // An end: another start on that line first, else what shows again
    which = uint16_t(w) & 0x7FFF;
    if (which == 0x100)
        goto MkA9a;
    for (uint16_t i = 3;; i += 3) {
        if (a3[i] != y)
            break;
        if (int16_t(a3[i + 2]) >= 0) {
            a3 += i;
            goto MkA1;
        }
    }
    {
        uint16_t best = 1000;
        for (a0 = first; a0 < a3; a0 += 3) {
            if (int16_t(a0[2]) < 0)
                continue;
            if (y < a0[0] || y >= a0[1])
                continue;
            if (best < a0[2])
                continue;
            best = a0[2];
        }
        if (best == 1000)
            goto MkA9;
        if (best <= which)
            goto MkA10;
        which = best;
    }
MkA8:
    {
        Screen *s = ptr<Screen>(peek_l(reinterpret_cast<const uint8_t *>(d.EcPri) - 4 + int16_t(which)));
        y = a3[0];
        if (y < EcYStrt - 1 || y >= uint16_t(d.EcYMax - 2))
            goto MkA10;
        *out++ = y;
        poke_l(out, address(s));
        out += 2;
        if (s->EcCon0 & Interlace)
            d.InterInter = 4;
        goto MkA10;
    }
MkA9:
    if (!which)
        goto MkA10;
    y = a3[0];
MkA9a:
    if (y < EcYStrt - 1)
        goto MkA10;
    limit = d.EcYMax - 1;
    if (y >= limit) {
        *out++ = -limit;
        goto MkAFin;
    }
    *out++ = -y;
MkA10:
    a3 += 3;
    goto MkA1;
MkAFin:
    *out = 0;
}

} // namespace

// EcCopper / EcForceCop
extern "C" void w_ec_copper(AllRegs *r, bool force)
{
    WData &d = wdata();
    if (force)
        d.EcYAct++;
    if (take_changes(d)) {
        d.EcYAct = 0;
        cut(d);
    }
    slices(d);
    d.Cop255 = 0;
    d.InterBit = 0;
    d.SwapList[0] = 0;
    for (uint32_t i = 0; i < EcMax; i++) {
        poke_l(d.CopMark + CopL1 * i, 0);
        poke_l(d.CopMark + CopL1 * i + 64, 0);
    }
    for (uint32_t i = 0; i < 10; i++)
        poke_l(d.CopMark + CopL1 * EcMax + CopL2 * i, 0);
    r->d[0] = 0;
    if (!d.CopON)
        return;
    Builder b = {d, ptr<uint16_t>(d.CopLogic + 64 + 4), nullptr, ptr<uint8_t>(r->a[4]), 0, 0, 0, 0, nullptr, nullptr,
                 0};
    if (d.RainBow && update_rainbows(d)) {
        b.with_rainbows();
    } else {
        // MCop0
        b.a2 = ptr<uint16_t>(d.EcCop);
        for (;;) {
            int16_t e = int16_t(*b.a2++);
            if (!e)
                break;
            if (e < 0) {
                b.screen_end(-e);
                continue;
            }
            Screen *s = ptr<Screen>(peek_l(b.a2));
            b.a2 += 2;
            b.screen_start(e, s);
        }
        b.a2--;
        if (b.a2 == ptr<uint16_t>(d.EcCop))
            b.screen_end(d.EcYMax - 1);
    }
    poke_l(b.a1, 0xFFFFFFFE);
    // MCopSw: the new list shown
    uint32_t logic = d.CopLogic;
    d.CopLogic = d.CopPhysic;
    d.CopPhysic = logic;
    if (d.AMOSHere >> 8) {
        chips().cop1lc = logic;
        d.InterBit = d.InterInter;
    }
}

extern "C" {
void w_ec_copper_make(AllRegs *r) { w_ec_copper(r, false); }
void w_ec_copper_force(AllRegs *r) { w_ec_copper(r, true); }
}
