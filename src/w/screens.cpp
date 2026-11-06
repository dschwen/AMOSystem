// Screen management of AMOS.library's screen table (EcCall): opening and
// closing screens, clones, double buffering, dual playfield, display
// position and order. The copper list builder (+W.s EcCopper) turns what
// these set into the display: T_EcYAct asks it to recompute the screens,
// T_Actualise (BitEcrans) to rebuild.
//
// Screens are in T_EcAdr by number; T_EcPri lists them from the front,
// ending with -1 (zeros are skipped). EcFlags and the EcAW/EcAWT/EcAV
// change flags are set with byte operations in the assembler, so their
// bits are in the high byte.

#include "runtime/static_symbols.h"
#include "core/amiga.h"
#include "w/w.h"

#include <exec/execbase.h>
#include <graphics/gfx.h>
#include <graphics/layers.h>
#include <graphics/rastport.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"
#include "hal/system.h"
#include <graphics/clip.h>
#include <graphics/layers.h>

using namespace amos;
using namespace amos::w;

// +W.s, the glue of w/


namespace {

constexpr uint32_t EcMax = 12;
constexpr uint16_t EcMaxPlans = 6;
constexpr uint16_t EcYBase = 0x1000;
constexpr uint16_t Hide = 1 << (8 + 7), Clone = 1 << (8 + 6);
constexpr uint16_t Interlace = 1 << 2;          // EcCon0
constexpr uint16_t BitEcrans = 1 << 12;         // T_Actualise
constexpr uint32_t SwapL = 32;
constexpr uint32_t CopL1 = 16 * 4 * 2, CopL2 = 16 * 4, CopML = EcMax * CopL1 + 10 * CopL2;
constexpr uint32_t EcTCop = 1024;

enum : uint32_t {
    ErrMemory = 1, ErrOpen = 2, ErrNotOpen = 3, ErrParam = 4, ErrDouble = 25, ErrDual = 26, ErrNotDual = 27,
};

inline void barrier() { asm volatile("" ::: "memory"); }

uint8_t &high(be_u16 &w) { return reinterpret_cast<uint8_t *>(&w)[0]; }

// EcGet: the screen number n (no check of n)
be_u32 *slot(uint16_t n) { return &wdata().EcAdr[0] + int16_t(n << 2) / 4; }
Screen *screen(uint16_t n) { return ptr<Screen>(*slot(n)); }

void call_a0(host::RoutineArg routine, uint32_t a0)
{
    Regs r;
    r.a[0] = a0;
    amos_call(&r, routine);
}

// Ec_Active: s the current screen, its RastPort the one drawn in
void activate(Screen *s)
{
    WData &d = wdata();
    d.EcCourant = address(s);
    if (s)
        d.RastPort = s->Ec_RastPort;
}

// EcTout / EcOtoV: the screens are to be recomputed, or only rebuilt
void recompute() { wdata().EcYAct++; }
void rebuild() { wdata().Actualise |= BitEcrans; }

// The copper marks of a screen for its swaps
uint32_t swap_marks(const Screen *s)
{
    return address(wdata().CopMark) + uint16_t(uint16_t(s->EcNumber << 6) + CopL1 * EcMax);
}

// InterPlus: an interlaced screen goes into T_InterList (pairs of the
// screen and its copper marks, ending with 0)
void interlace_add(Screen *s)
{
    if (!(s->EcCon0 & Interlace))
        return;
    WData &d = wdata();
    d.InterBit = 0;
    be_u32 *e = d.InterList;
    bool more;
    do {
        more = *e;
        e += 2;
    } while (more);
    *e = 0;
    barrier();
    e[-2] = address(s);
    e[-1] = swap_marks(s);
}

// ---- order --------------------------------------------------------------------

// EcFirst: screen n to the front
uint32_t to_front(uint16_t n)
{
    Screen *s = screen(n);
    if (!s)
        return ErrNotOpen;
    auto *to = wdata().EcPri, *from = to;
    uint32_t next = *from;
    *to = address(s);
    for (;;) {
        to++;
        uint32_t v;
        do {
            from++;
            v = next;
            next = *from;
            *to = v;
            if (int32_t(v) < 0) {
                recompute();
                rebuild();
                return 0;
            }
        } while (!v || v == address(s));
    }
}

// EcLast: screen n to the back
uint32_t to_back(uint16_t n)
{
    Screen *s = screen(n);
    if (!s)
        return ErrNotOpen;
    auto *to = wdata().EcPri, *from = to;
    for (;;) {
        uint32_t v = *from++;
        *to = v;
        if (int32_t(v) < 0)
            break;
        if (v && v != address(s))
            to++;
    }
    *to++ = address(s);
    *to = 0xFFFFFFFF;
    recompute();
    rebuild();
    return 0;
}

// ---- display ------------------------------------------------------------------

// EcView: screen n displayed at x,y, size w,h (EntNul: unchanged)
uint32_t view(uint16_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    Screen *s = screen(n);
    if (!s)
        return ErrNotOpen;
    if (x != EntNul) {
        s->EcAWX = x;
        high(s->EcAW) |= 2;
    }
    if (w != EntNul) {
        s->EcAWTX = w;
        high(s->EcAWT) |= 2;
    }
    if (y != EntNul) {
        s->EcAWY = y;
        high(s->EcAW) |= 4;
    }
    if (h != EntNul) {
        s->EcAWTY = h;
        high(s->EcAWT) |= 4;
    }
    rebuild();
    return 0;
}

// EcOffs: the part of screen n shown starts at x,y
uint32_t offset(uint16_t n, uint32_t x, uint32_t y)
{
    Screen *s = screen(n);
    if (!s)
        return ErrNotOpen;
    if (x != EntNul) {
        s->EcAVX = x;
        high(s->EcAV) |= 2;
    }
    if (y != EntNul) {
        s->EcAVY = y;
        high(s->EcAV) |= 4;
    }
    rebuild();
    return 0;
}

// EcHide: hide (hide != 0) or show screen n; not the second playfield of
// a dual playfield
uint32_t hide(uint16_t n, uint16_t hide)
{
    Screen *s = screen(n);
    if (!s)
        return ErrNotOpen;
    if (int16_t(s->EcDual) >= 0) {
        s->EcFlags &= ~Hide;
        if (hide)
            s->EcFlags |= Hide;
    }
    recompute();
    rebuild();
    return 0;
}

// ---- closing ------------------------------------------------------------------

// EcDD: free screen s, out of the tables already. Another screen becomes
// the current one if it was.
void free_screen(Screen *s)
{
    WData &d = wdata();
    if (s->EcAZones)
        free_mm(s->EcAZones, uint32_t(s->EcNZones) * 8);
    call_a0(AMOS_ROUTINE(w_dad_amal), address(&s->EcAW));
    call_a0(AMOS_ROUTINE(w_dad_amal), address(&s->EcAWT));
    call_a0(AMOS_ROUTINE(w_dad_amal), address(&s->EcAV));
    call_a0(AMOS_ROUTINE(w_bb_ec_off), address(s));
    Screen *current = ptr<Screen>(d.EcCourant);
    activate(s);
    call(AMOS_ROUTINE(w_widela));
    call(AMOS_ROUTINE(w_flash_off));
    call(AMOS_ROUTINE(w_shift_off));
    call(AMOS_ROUTINE(w_fade_stop));
    // (The assembler cleared the pattern through a null RastPort when the
    // screen failed to open, writing to address 8)
    if (d.RastPort)
        clear_pattern();
    call(AMOS_ROUTINE(w_close_font));

    // The current screen: the first not cloned one of number < 8, else
    // the first not cloned one
    if (current == s) {
        current = nullptr;
        for (int pass = 0; pass < 2 && !current; pass++) {
            for (const be_u32 *p = d.EcPri; int32_t(*p) >= 0; p++) {
                Screen *o = ptr<Screen>(*p);
                if (o->EcFlags & Clone)
                    continue;
                if (pass == 0 && o->EcNumber >= 8)
                    continue;
                current = o;
                break;
            }
        }
    }
    activate(current);

    if (!(s->EcFlags & Clone)) {
        if (s->Ec_Region) {
            hal::install_clip_region(ptr<struct Layer>(s->Ec_Layer), nullptr);
            hal::dispose_region(ptr<struct Region>(s->Ec_Region));
        }
        if (s->Ec_Layer)
            hal::delete_layer(ptr<struct Layer>(s->Ec_Layer));
        hal::wait_blit();
        wait_vbl();
        hal::wait_blit();
        if (s->Ec_LayerInfo)
            hal::dispose_layer_info(ptr<struct Layer_Info>(s->Ec_LayerInfo));
        // (The layers library needs the blitter done and a VBL)
        hal::wait_blit();
        wait_vbl();
        hal::wait_blit();
        for (int p = 0; p < EcMaxPlans; p++) {
            uint32_t plane = s->EcLogic[p];
            if (plane)
                free_mm(plane, s->EcTPlan);
            s->EcLogic[p] = 0;
            if (s->EcPhysic[p] == plane)
                s->EcPhysic[p] = 0;
        }
        for (int p = 0; p < EcMaxPlans; p++) {
            if (s->EcPhysic[p])
                free_mm(s->EcPhysic[p], s->EcTPlan);
            s->EcPhysic[p] = 0;
        }
        if (s->Ec_BitMap)
            free_mm(s->Ec_BitMap, sizeof(struct BitMap));
    }
    free_mm(address(s), Screen::EcLong);
}

// EcDel: close screen n. a0: the current screen afterwards.
uint32_t close(uint16_t n, uint32_t *current)
{
    WData &d = wdata();
    be_u32 *at = slot(n);
    Screen *s = ptr<Screen>(*at);
    *current = address(at);
    if (!s)
        return ErrNotOpen;
    *at = 0;
    // Out of the order
    be_u32 *to = d.EcPri, *from = to;
    for (;;) {
        uint32_t v = *from++;
        *to = v;
        if (int32_t(v) < 0)
            break;
        if (v && v != address(s))
            to++;
    }
    // Out of the interlaced screens
    if (s->EcCon0 & Interlace) {
        d.InterBit = 0;
        be_u32 *to = d.InterList, *from = to;
        while (*from) {
            if (*from == address(s)) {
                from += 2;
                continue;
            }
            *to++ = *from++;
            *to++ = *from++;
        }
        *to = 0;
    }
    // No screen swap now
    d.SwapList[0] = 0;
    // The copper list without it
    wait_vbl();
    s->EcFlags |= Hide;
    call(AMOS_ROUTINE(w_ec_force_cop));
    wait_vbl();
    free_screen(s);
    *current = d.EcCourant;
    return 0;
}

// ---- opening ------------------------------------------------------------------

// EcCree: screen n (d1), tx x ty (d2/d3), planes (d4), mode d5 (BPLCON0
// bits: hires, interlace), colours d6, palette a1. a0: the screen.
uint32_t open(const AllRegs *r, uint32_t *result)
{
    WData &d = wdata();
    uint32_t n = r->d[1];
    uint32_t tx = r->d[2] & 0xFFFFFFF0, ty = r->d[3], planes = r->d[4];
    if (!tx || tx >= 1024 || !ty || ty >= 1024 || !planes || planes > EcMaxPlans)
        return ErrParam;
    uint32_t dummy;
    while (screen(n))
        close(n, &dummy);
    uint32_t memory = fast_mm(Screen::EcLong);
    if (!memory)
        return ErrMemory;
    Screen *s = ptr<Screen>(memory);
    auto fail = [&] {
        free_screen(s);
        return ErrMemory;
    };
    s->EcNbCol = r->d[6];
    copy(s->EcPal, ptr<uint8_t>(r->a[1]), sizeof s->EcPal);
    s->EcTx = tx;
    s->EcTxM = tx - 1;
    uint16_t line = uint16_t(tx) >> 3;
    s->EcTLigne = line;
    s->EcTy = ty;
    s->EcTyM = ty - 1;
    s->EcTPlan = uint32_t(line) * uint16_t(ty);
    s->EcNPlan = planes;
    uint16_t system = hal::system_bplcon0() & 0x03FB;
    s->EcCon0 = uint16_t(r->d[5]) | uint16_t(planes << 12) | system;
    s->EcCon2 = 0x24;

    uint32_t bitmap = fast_mm(sizeof(struct BitMap));
    if (!bitmap)
        return fail();
    s->Ec_BitMap = bitmap;
    struct BitMap *bm = ptr<struct BitMap>(bitmap);
    hal::init_bitmap(bm, int16_t(planes), int16_t(tx), int16_t(ty));
    for (uint16_t p = 0; p < planes; p++) {
        uint32_t plane = chip_mm(s->EcTPlan);
        if (!plane)
            return fail();
        bm->Planes[p] = ptr<UBYTE>(plane);
        s->EcCurrent[p] = plane;
        s->EcLogic[p] = plane;
        s->EcPhysic[p] = plane;
    }
    // A real layer, for the RastPort
    struct Layer_Info *info = hal::new_layer_info();
    s->Ec_LayerInfo = address(info);
    if (!info)
        return fail();
    struct Layer *layer
        = hal::create_upfront_layer(info, bm, 0, 0, int16_t(s->EcTx - 1), int16_t(s->EcTy - 1), LAYERSIMPLE, nullptr);
    s->Ec_Layer = address(layer);
    if (!layer)
        return fail();
    s->Ec_RastPort = address(layer->rp);
    hal::wait_blit();
    wait_vbl();
    hal::wait_blit();
    s->EcAZones = 0;
    s->EcNZones = 0;

    *slot(n) = memory;
    s->EcNumber = n;
    activate(s);
    to_front(n);
    interlace_add(s);

    // Where it is displayed by default
    uint32_t x = int16_t(d.DefWX), y = int16_t(d.DefWY);
    uint16_t w = s->EcTx, h = s->EcTy;
    if (s->EcCon0 & 0x8000)
        w >>= 1;
    if (w >= 320 + 16)
        x = int16_t(d.DefWX2);
    if (h >= 256 && (!(s->EcCon0 & Interlace) || h >= 256 * 2))
        y = int16_t(d.DefWY2);
    view(n, x, y, int16_t(w), int16_t(h));

    // The text window of the whole screen
    s->EcWindow = 0;
    Regs wo;
    wo.d[1] = 0;
    wo.d[2] = 0;
    wo.d[3] = 0;
    wo.d[4] = uint16_t(s->EcTx >> 4) << 1;
    wo.d[5] = s->EcTy >> 3;
    wo.d[6] = 1;
    wo.d[7] = 0;
    wo.a[1] = 0;
    amos_call(&wo, AMOS_ROUTINE(w_wopen));
    if (wo.d[0]) {
        close(n, &dummy);
        return ErrMemory;
    }

    // The graphic state, in the colours of the window
    Window *window = ptr<Window>(s->EcWindow);
    uint8_t pen = window->WiPen, paper = window->WiPaper;
    s->EcInkA = pen;
    s->EcInkB = paper;
    s->EcFInkC = pen;
    s->EcIInkC = pen;
    s->EcFInkA = paper;
    s->EcFInkB = paper;
    s->EcIInkA = paper;
    s->EcIInkB = paper;
    s->EcIPat = 1;
    s->EcFPat = 2;
    s->EcMode = 1;
    s->EcLine = 0xFFFF;
    struct RastPort *rp = ptr<struct RastPort>(s->Ec_RastPort);
    hal::set_a_pen(rp, s->EcInkA);
    hal::set_b_pen(rp, s->EcInkB);
    hal::set_draw_mode(rp, s->EcMode);
    rp->LinePtrn = s->EcLine;
    rp->cp_x = 0;
    rp->cp_y = 0;
    hal::set_font(rp, ptr<struct TextFont>(d.DefaultFont));
    s->EcClipX0 = 0;
    s->EcClipY0 = 0;
    s->EcClipX1 = s->EcTx;
    s->EcClipY1 = s->EcTy;

    *result = d.EcCourant;
    recompute();
    rebuild();
    return 0;
}

// EcCClo: screen n, a copy of the current one showing the same bitmaps
uint32_t clone(uint16_t n)
{
    if (screen(n))
        return ErrOpen;
    uint32_t memory = fast_mm(Screen::EcLong);
    if (!memory)
        return ErrMemory;
    Screen *s = ptr<Screen>(memory);
    copy(s, ptr<Screen>(wdata().EcCourant), Screen::EcLong);
    s->EcAZones = 0;
    s->EcNZones = 0;
    s->EcWindow = 0;
    s->EcPat = 0;
    s->EcFontFlag = 0;
    s->EcFlags |= Clone;
    s->EcNumber = n;
    interlace_add(s);
    *slot(n) = memory;
    to_front(n);
    recompute();
    rebuild();
    return 0;
}

// EcDouble: the current screen gets a second set of bitmaps. (The
// assembler counted the copy in 16 bits, overrunning the new planes of
// more than 64 KB, and left the end of planes whose size is not a
// multiple of 16 bytes uncopied.)
uint32_t double_buffer()
{
    Screen *s = current_screen();
    if (s->EcFlags & DoubleBuffer)
        return ErrDouble;
    for (uint16_t p = 0; p < s->EcNPlan; p++) {
        uint32_t plane = chip_mm(s->EcTPlan);
        if (!plane) {
            uint32_t dummy;
            close(s->EcNumber, &dummy);
            return ErrMemory;
        }
        s->EcLogic[p] = plane;
        s->EcCurrent[p] = plane;
        copy(ptr<uint8_t>(plane), ptr<uint8_t>(s->EcPhysic[p]), s->EcTPlan);
    }
    s->EcFlags |= DoubleBuffer;
    s->EcAuto = 2;
    call(AMOS_ROUTINE(w_tabk1));
    call(AMOS_ROUTINE(w_tabk2));
    call(AMOS_ROUTINE(w_tabk3));
    return 0;
}

// The swap of the bitmaps of s, in the swap list entry e, which the VBL
// interrupt shows when active (e[0] the copper marks). Returns whether
// it may be.
bool swap_bitmaps(Screen *s, uint8_t *e)
{
    uint16_t n = s->EcNPlan - 1;
    poke_w(e + 4, n);
    uint8_t *planes = e + 6;
    struct BitMap *bm = ptr<struct BitMap>(s->Ec_BitMap);
    for (int p = 0; p <= n; p++, planes += 4) {
        uint32_t logic = s->EcLogic[p];
        s->EcLogic[p] = s->EcPhysic[p];
        s->EcPhysic[p] = logic;
        poke_l(planes, logic + s->EcDEcran);
    }
    for (int p = 0; p <= n; p++) {
        s->EcCurrent[p] = s->EcLogic[p];
        bm->Planes[p] = ptr<UBYTE>(s->EcLogic[p]);
    }
    return wdata().CopON && !(s->EcCon0 & Interlace);
}

// ScSwap: Screen Swap n
uint32_t swap(uint16_t n)
{
    Screen *s = screen(n);
    if (!s)
        return ErrNotOpen;
    if (!(s->EcFlags & DoubleBuffer))
        return 0;
    uint8_t *e = reinterpret_cast<uint8_t *>(wdata().SwapList);
    if (peek_l(e)) {
        do {
            if (peek_w(e + SwapL - 2) == s->EcNumber)
                return 0;               // swapped already
            e += SwapL;
        } while (peek_l(e));
    }
    poke_w(e + SwapL - 2, s->EcNumber);
    if (swap_bitmaps(s, e)) {
        poke_l(e + SwapL, 0);
        barrier();
        poke_l(e, swap_marks(s));
    }
    return 0;
}

// ScSwapS: Screen Swap of all double buffered screens 0-7
void swap_all()
{
    uint8_t *e = reinterpret_cast<uint8_t *>(wdata().SwapList);
    poke_l(e, 0);
    for (int n = 0; n < 8; n++) {
        Screen *s = screen(n);
        if (!s || !(s->EcFlags & DoubleBuffer))
            continue;
        poke_w(e + SwapL - 2, s->EcNumber);
        uint8_t *entry = e;
        e += SwapL;
        if (swap_bitmaps(s, entry)) {
            poke_l(entry + SwapL, 0);
            barrier();
            poke_l(entry, swap_marks(s));
        }
    }
}

// Duale: Dual Playfield a,b: b becomes the second playfield of a, of the
// same mode, with as many planes or one less (up to 3 each, 2 in hires)
uint32_t dual_playfield(uint16_t a, uint16_t b)
{
    if (a == b)
        return ErrDual;
    Screen *second = screen(b);
    if (!second)
        return ErrNotOpen;
    Screen *first = screen(a);
    if (!first)
        return ErrNotOpen;
    if (first->EcDual || second->EcDual)
        return ErrDual;
    uint16_t most = first->EcCon0 & 0x8000 ? 2 : 3;
    uint16_t con0 = first->EcCon0 & 0x8FFF;
    if (con0 != (second->EcCon0 & 0x8FFF))
        return ErrDual;
    uint16_t p1 = first->EcNPlan, p2 = second->EcNPlan;
    if (p1 > most || p2 > most)
        return ErrDual;
    if (p1 != p2 && p2 + 1 != p1)
        return ErrDual;
    first->EcCon0 = con0 | uint16_t((p1 + p2) << 12) | 1 << 10;
    first->EcCon2 = (first->EcCon2 | (first->EcCon2 & 7) << 3) & 0x3F;
    second->EcFlags |= Hide;
    first->EcDual = b + 1;
    second->EcDual = -(b + 1);
    recompute();
    rebuild();
    return 0;
}

// DualP: Dual Priority a,b: which playfield is in front
uint32_t dual_priority(uint16_t a, uint16_t b)
{
    if (a == b)
        return ErrNotDual;
    Screen *sb = screen(b);
    if (!sb)
        return ErrNotOpen;
    Screen *sa = screen(a);
    if (!sa)
        return ErrNotOpen;
    if (!sa->EcDual || !sb->EcDual)
        return ErrNotDual;
    Screen *first = sa;
    bool second_front = false;
    if (int16_t(sb->EcDual) >= 0) {
        first = sb;
        second_front = true;
    }
    first->EcCon2 &= ~(1 << 6);
    if (second_front)
        first->EcCon2 |= 1 << 6;
    rebuild();
    return 0;
}

// EcRaz: no screens, no copper marks
void screens_reset()
{
    WData &d = wdata();
    for (be_u32 &a : d.EcAdr)
        a = 0;
    d.EcPri[0] = 0xFFFFFFFF;
    for (uint32_t i = 0; i < CopML; i += 4)
        poke_l(d.CopMark + i, 0);
    d.EcYAct = 1;
}

} // namespace

namespace amos::w {

// EcInit: the tables of the copper list builder, the first (empty) list,
// the screen functions' vector
bool screens_init()
{
    WData &d = wdata();
    d.EcCop = fast_mm(EcTCop);
    if (!d.EcCop)
        return false;
    d.ChipBuf = chip_mm(256);
    if (!d.ChipBuf)
        return false;
    d.EcYMax = 311 + EcYBase;
    if (hal::vblank_frequency() != 50)
        d.EcYMax = 261 + EcYBase;               // NTSC
    screens_reset();
    call(AMOS_ROUTINE(w_copper_make));
    if (d.AMOSHere >> 8) {
        chips().copjmp1 = 0;
        chips().dmacon = 0x82A0;
    }
    d.EcVect = address(w_ec_in);
    return true;
}

// EcEnd: the screens closed, the blocks and rainbows deleted
void screens_end()
{
    WData &d = wdata();
    Regs r;
    r.d[1] = 0;
    r.d[2] = EcMax - 1;
    amos_call(&r, AMOS_ROUTINE(w_sc_close_range));
    call(AMOS_ROUTINE(w_cblocks_delete));
    call(AMOS_ROUTINE(w_blocks_delete));
    Regs t;
    t.d[1] = 0xFFFFFFFF;
    amos_call(&t, AMOS_ROUTINE(w_rain_del));
    if (d.ChipBuf)
        free_mm(d.ChipBuf, 256);
    if (d.EcCop)
        free_mm(d.EcCop, EcTCop);
}

} // namespace amos::w

extern "C" {
// EcRaz, EcEnd (screen table entries Raz and Visible)
void w_ec_reset(AllRegs *r)
{
    screens_reset();
    r->d[0] = 0;
}

void w_ec_end(AllRegs *r)
{
    screens_end();
    r->d[0] = 0;
}

void w_ec_open(AllRegs *r)
{
    uint32_t result = r->a[0];
    r->d[0] = open(r, &result);
    r->a[0] = result;
}

void w_ec_close(AllRegs *r)
{
    uint32_t current;
    r->d[0] = close(r->d[1], &current);
    r->a[0] = current;
}

// EcDAll: screens d1 to d2
void w_ec_close_range(AllRegs *r)
{
    uint32_t current;
    uint32_t n = r->d[1];
    do
        close(n++, &current);
    while (n <= r->d[2]);
    r->d[0] = 0;
}

void w_ec_to_front(AllRegs *r) { r->d[0] = to_front(r->d[1]); }
void w_ec_to_back(AllRegs *r) { r->d[0] = to_back(r->d[1]); }
void w_ec_view(AllRegs *r) { r->d[0] = view(r->d[1], r->d[2], r->d[3], r->d[4], r->d[5]); }
void w_ec_offset(AllRegs *r) { r->d[0] = offset(r->d[1], r->d[2], r->d[3]); }
void w_ec_hide(AllRegs *r) { r->d[0] = hide(r->d[1], r->d[2]); }
void w_ec_clone(AllRegs *r) { r->d[0] = clone(r->d[1]); }
void w_ec_double(AllRegs *r) { r->d[0] = double_buffer(); }
void w_ec_swap(AllRegs *r) { r->d[0] = swap(r->d[1]); }

void w_ec_swap_all(AllRegs *r)
{
    swap_all();
    r->d[0] = 0;
}

void w_ec_dual(AllRegs *r) { r->d[0] = dual_playfield(r->d[1], r->d[2]); }
void w_ec_dual_priority(AllRegs *r) { r->d[0] = dual_priority(r->d[1], r->d[2]); }

// EcMarch: screen d1 becomes the current one; 3 not open, 4 a clone
void w_ec_activate(AllRegs *r)
{
    Screen *s = screen(r->d[1]);
    if (!s)
        r->d[0] = ErrNotOpen;
    else if (s->EcFlags & Clone)
        r->d[0] = ErrParam;
    else {
        activate(s);
        r->d[0] = 0;
    }
    r->a[0] = s ? address(s) : address(slot(r->d[1]));
}

// EcCrnt: the number of the current screen, -1 if not a user one (8+)
void w_ec_current(AllRegs *r)
{
    Screen *s = current_screen();
    r->a[0] = address(s);
    r->d[0] = s->EcNumber < 8 ? (r->d[0] & 0xFFFF0000) | s->EcNumber : 0xFFFFFFFF;
}

// EcAdres: the address of screen d1 (0-7), or 0
void w_ec_address(AllRegs *r)
{
    if (uint16_t(r->d[1]) < 8) {
        r->a[0] = address(slot(r->d[1]));
        r->d[0] = *slot(r->d[1]);
    } else {
        r->d[0] = 0;
    }
}

// EcLibre: d1 the first free screen number, d0 -1 if none. (As the
// assembler, the long after the table counts as screen 12.)
void w_ec_free_number(AllRegs *r)
{
    const uint8_t *table = reinterpret_cast<const uint8_t *>(wdata().EcAdr);
    for (uint32_t n = 0; n <= EcMax; n++) {
        if (!peek_l(table + 4 * n)) {
            r->d[1] = n;
            r->a[0] = address(table + 4 * (n + 1));
            r->d[0] = 0;
            return;
        }
    }
    r->d[1] = EcMax;
    r->a[0] = address(table + 4 * (EcMax + 1));
    r->d[0] = 0xFFFFFFFF;
}

// TMaxRaw: the last line of the display
void w_ec_max_raw(AllRegs *r)
{
    r->d[1] = int16_t(wdata().EcYMax - EcYBase);
    r->d[0] = 0;
}

// TNTSC: d1 -1 on an NTSC machine (VBlankFrequency not 50)
void w_ec_ntsc(AllRegs *r)
{
    r->d[0] = 0;
    r->d[1] = hal::vblank_frequency() == 50 ? 0 : 0xFFFFFFFF;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/w_screens.inc"
#endif
