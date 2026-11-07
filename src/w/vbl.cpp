// The VBL interrupt server of AMOS.library: the interlaced screens' and
// screen swaps' bitplane pointers changed in the copper list, the sprite
// buffers shown, the counters, the routines of the extensions, the mouse,
// shift/flash/fade of colours, and AMAL.
//
// It runs in the interrupt: no task context (no wdata()), the data zone
// comes as the argument. The colour functions (w/colours.cpp) change the
// tables it reads with the interrupt held off.

#include "core/amiga.h"
#include "w/w.h"

#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include "hal/system.h"

using namespace amos;
using namespace amos::w;

// w/vbl_glue.s: call fn with a5 the data zone and a6 the custom chips
extern "C" void w_int_call(const void *fn, uint8_t *data);
// +W.s

namespace {

constexpr uint16_t PalMax = 16;
constexpr uint16_t BitVBL = 1 << 15;
constexpr uint32_t SwapL = 32;
constexpr uint32_t FlMax = 16, FlashSize = 2 + 2 + 4 + 2 + 16 * 4 + 2;

// The copper moves of a list of marks (copper address, offset pairs, ending
// with 0) get the bitplanes planes (count) plus the offset
void poke_planes(const uint8_t *marks, const be_u32 *planes, uint16_t count, uint32_t extra)
{
    for (;;) {
        uint32_t moves = peek_l(marks);
        if (!moves)
            return;
        uint32_t offset = peek_l(marks + 4) + extra;
        marks += 8;
        uint8_t *m = ptr<uint8_t>(moves);
        for (uint16_t p = 0; p <= count; p++, m += 8) {
            uint32_t at = planes[p] + offset;
            poke_w(m + 6, uint16_t(at));
            poke_w(m + 2, uint16_t(at >> 16));
        }
    }
}

// The colour n * 2 + 2 (d0 of the assembler) in the copper lists of screen s
void poke_colour(WData &d, const Screen *s, uint16_t n4, uint16_t colour)
{
    const uint8_t *marks = d.CopMark + (s->EcNumber << 7);
    if (n4 >= PalMax * 4 + 4)
        marks += 64;
    for (uint32_t list; (list = peek_l(marks)); marks += 4)
        poke_w(ptr<uint8_t>(list) + n4 - 2, colour);
}

// MousInt: the mouse within its limits, its sprite's control words
void mouse(WData &d)
{
    if (!(d.AMOSHere >> 8))
        return;
    int16_t x = d.MouseX, y = d.MouseY;
    if (x < int16_t(d.MouXMin))
        x = d.MouXMin;
    if (x > int16_t(d.MouXMax))
        x = d.MouXMax;
    if (y < int16_t(d.MouYMin))
        y = d.MouYMin;
    if (y > int16_t(d.MouYMax))
        y = d.MouYMax;
    d.MouseX = x;
    d.MouseY = y;
    uint16_t hx = uint16_t(x) >> 1, hy = uint16_t(y) >> 1;
    d.XMouse = hx;
    d.YMouse = hy;
    if (int16_t(d.MouShow) < 0)
        return;
    hx -= d.MouHotX;
    hy -= d.MouHotY;
    uint16_t rx = uint16_t(hx >> 1 | hx << 15);
    uint16_t first = uint16_t((hy & 0xFF) << 8) | (rx & 0xFF);
    uint16_t second = hy & 0x100 ? 1 << 2 : 0;
    uint16_t end = hy + d.MouTy;
    second |= uint16_t((end & 0xFF) << 8);
    if (end & 0x100)
        second |= 1 << 1;
    if (rx & 0x8000)
        second |= 1;
    const uint32_t buffers[3] = {d.HsPhysic, d.HsLogic, d.HsInter};
    for (uint32_t b : buffers) {
        poke_w(ptr<uint8_t>(b), first);
        poke_w(ptr<uint8_t>(b) + 2, second);
    }
}

// Shifter: the colours shifted one step every speed VBLs
void shifter(WData &d)
{
    uint8_t *t = d.TShift;
    if (!peek_w(t))
        return;
    poke_w(t, peek_w(t) - 1);
    if (peek_w(t))
        return;
    poke_w(t, peek_w(t + 2));
    Screen *s = ptr<Screen>(peek_l(t + 4));
    uint16_t first = peek_w(t + 8), last = peek_w(t + 10);
    auto *pal = s->EcPal;
    uint16_t a = first, b = last, kept;
    if (!t[12]) {
        // Up
        kept = pal[b / 2];
        for (; b != a; b -= 2)
            pal[b / 2] = pal[b / 2 - 1];
    } else {
        kept = pal[a / 2];
        for (; a != b; a += 2)
            pal[a / 2] = pal[a / 2 + 1];
    }
    if (t[13])
        pal[a / 2] = kept;
    const uint8_t *marks0 = d.CopMark + (s->EcNumber << 7);
    for (uint16_t c = first, off = uint16_t(first << 1); c <= last; c += 2, off += 4) {
        const uint8_t *marks = c >= PalMax * 2 ? marks0 + 64 : marks0;
        for (uint32_t list; (list = peek_l(marks)); marks += 4)
            poke_w(ptr<uint8_t>(list) + 2 + off, pal[c / 2]);
    }
}

// FlInt: the flashing colours
void flash(WData &d)
{
    uint8_t *counts = reinterpret_cast<uint8_t *>(&d.NbFlash);
    if (counts[1])
        return;
    uint8_t n = counts[0];
    if (!n)
        return;
    counts[1]++;
    uint8_t *entry = d.TFlash;
    for (;; entry += FlashSize) {
        uint16_t colour = peek_w(entry);
        if (!colour)
            continue;
        uint16_t count = peek_w(entry + 2) - 1;
        poke_w(entry + 2, count);
        if (!count) {
            Screen *s = ptr<Screen>(peek_l(entry + 4));
            uint8_t *step = entry + 10 + int16_t(peek_w(entry + 8));
            uint16_t speed = peek_w(step);
            poke_w(entry + 2, speed);
            step += 2;
            if (!speed) {
                poke_w(entry + 8, 0);
                step = entry + 10;
                poke_w(entry + 2, peek_w(step));
                step += 2;
            }
            poke_w(entry + 8, peek_w(entry + 8) + 4);
            uint16_t value = peek_w(step);
            s->EcPal[colour / 2 - 1] = value;
            poke_colour(d, s, uint16_t(colour << 1), value);
        }
        if (!--n)
            break;
    }
    counts[1]--;
}

// FadeI: the fade one step nearer, every speed VBLs
void fade(WData &d)
{
    if (!d.FadeFlag)
        return;
    if (--d.FadeCpt)
        return;
    d.FadeCpt = d.FadeVit;
    be_u16 *pal = ptr<be_u16>(d.FadePal);
    const uint8_t *marks0 = ptr<uint8_t>(d.FadeCop);
    int16_t n = d.FadeNb;
    uint8_t *e = d.FadeCol;
    uint16_t changing = 0;
    do {
        int16_t offset = int16_t(peek_w(e));
        e += 2;
        if (offset < 0) {
            e += 6;
            continue;
        }
        uint16_t moved = 0, rgb[3];
        for (int i = 0; i < 3; i++, e += 2) {
            uint8_t now = e[0], to = e[1];
            if (now != to) {
                now += now > to ? -1 : 1;
                e[0] = now;
                moved++;
            }
            rgb[i] = now;
        }
        if (!moved) {
            poke_w(e - 8, 0xFFFF);
            continue;
        }
        changing++;
        uint16_t colour = uint16_t(rgb[0] << 8 | rgb[1] << 4 | rgb[2]);
        pal[offset / 2] = colour;
        uint16_t off4 = uint16_t(offset << 1);
        const uint8_t *marks = off4 >= PalMax * 4 ? marks0 + 64 : marks0;
        for (uint32_t list; (list = peek_l(marks)); marks += 4)
            poke_w(ptr<uint8_t>(list) + 2 + off4, colour);
    } while (--n != -1);
    d.FadeFlag = changing;
}

} // namespace

// VblIn: data is the data zone (a5)
extern "C" void w_vbl(uint8_t *data)
{
    WData &d = *reinterpret_cast<WData *>(data - sizeof(WData));
    IData &i = *reinterpret_cast<IData *>(data);
    // The interlaced screens: the odd frames one line further
    if (d.InterBit) {
        uint16_t frame = chips().vposr;
        for (const be_u32 *e = d.InterList; *e; e += 2) {
            const Screen *s = ptr<Screen>(e[0]);
            uint32_t extra = int16_t(frame) < 0 ? 0 : uint16_t(s->EcTx >> 3);
            poke_planes(ptr<uint8_t>(e[1]), s->EcPhysic, s->EcNPlan - 1, extra);
        }
    }
    // The screen swaps
    if (uint32_t marks = d.SwapList[0]) {
        uint8_t *e = reinterpret_cast<uint8_t *>(d.SwapList);
        poke_l(e, 0);
        for (;;) {
            uint16_t count = peek_w(e + 4);
            poke_planes(ptr<uint8_t>(marks), reinterpret_cast<const be_u32 *>(e + 6), count, 0);
            e += SwapL;
            marks = peek_l(e);
            if (!marks)
                break;
        }
    }
    // The sprite buffer to show (HsPCop)
    if (uint32_t buffer = d.HsChange) {
        d.HsChange = 0;
        uint8_t *l = ptr<uint8_t>(d.CopLogic) + 4, *p = ptr<uint8_t>(d.CopPhysic) + 4;
        uint32_t column = int16_t(d.HsTCol);
        for (int s = 0; s < 8; s++, l += 8, p += 8, buffer += column) {
            poke_w(l + 2, uint16_t(buffer >> 16));
            poke_w(p + 2, uint16_t(buffer >> 16));
            poke_w(l + 6, uint16_t(buffer));
            poke_w(p + 6, uint16_t(buffer));
        }
    }
    d.VblCount++;
    d.VblTimer++;
    d.EveCpt--;
    d.Actualise |= BitVBL;
    for (const be_u32 *r = i.VblRout; *r; r++)
        w_int_call(ptr<void>(*r), data);
    mouse(d);
    shifter(d);
    flash(d);
    fade(d);
    if (!d.SyncOff)
        w_int_call(AMOS_DATA(w_animator), data);
}

namespace amos::w {

// Add_VBL: the server into the VBL chain (+W.s VblIn calls w_vbl)
void add_vbl()
{
    WData &d = wdata();
    auto is = vbl_interrupt();
    is->is_Node.ln_Type = NT_INTERRUPT;
    is->is_Node.ln_Pri = 100;
    is->is_Node.ln_Name = nullptr;
    is->is_Data = reinterpret_cast<uint8_t *>(&d) + sizeof(WData);
    is->is_Code = reinterpret_cast<void (*)()>(AMOS_ENTRY(w_vbl_in));
    hal::add_vbl_server(is);
}

// Rem_VBL
void rem_vbl()
{
    auto is = vbl_interrupt();
    if (is->is_Code)
        hal::remove_vbl_server(is);
}

// VblInit: no flash, no shift, the mouse from its bank (hidden), the VBL
void vbl_init()
{
    WData &d = wdata();
    d.NbFlash = 0;
    for (uint8_t &b : d.TFlash)
        b = 0;
    for (uint8_t &b : d.TShift)
        b = 0;
    d.MouDes = d.MouBank;
    d.MouTy = peek_w(ptr<uint8_t>(d.MouBank) + 2);
    d.MouXOld = 0;
    d.MouYOld = 0;
    d.MouShow = 0xFFFF;
    add_vbl();
}

} // namespace amos::w

extern "C" {
void w_sy_add_vbl(AllRegs *) { add_vbl(); }
void w_sy_rem_vbl(AllRegs *) { rem_vbl(); }
void w_sy_wait_vbl(AllRegs *r)
{
    wait_vbl();
    r->d[0] = 0;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/w_vbl.inc"
#endif
