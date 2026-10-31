// The monitor started and stopped (+Monitor.s Tt_ColdStart, Tt_Free,
// Tt_MonFree): its data and buffers, its two screens, the dialogue of the
// text screen's background, the patches on the interpreter and
// AMOS.library, the view's interrupt.

#include "monitor/monitor.h"
#include "core/amiga.h"

#include <exec/memory.h>
#include "hal/system.h"

using namespace amos;
using namespace amos::monitor;

// +Monitor.s
extern "C" void mon_view_inter();


namespace {

constexpr uint16_t E1Sx = 640, E1Sy = 116, E2Sx = 640, E2MinY = 128;
constexpr uint16_t NButtons = 16;
constexpr uint32_t InfSize = 88 * 100;
constexpr uint32_t TokenBuffer = 1024 + 512;     // Tt_BufT, then Tt_Ligne
constexpr uint16_t BitStep = 1 << 1;            // T_Actualise / ActuMask: the tests' call

uint32_t memory(uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[0] = w::reserve(r.d[0], w::FastClear);
    return r.a[0];
}

void free_memory(uint32_t address, uint32_t size)
{
    Regs r;
    r.a[1] = address;
    r.d[0] = size;
    w::release(r.a[1], r.d[0]);
}

uint32_t edit_buffer_size() { return uint32_t(uint16_t(mon().Ttt_Sy >> 3)) * 256; }

// Dia_RScOpen: screen n of the pictures of resource, the current one
// placed at y; false if it failed
bool open_screen(uint16_t n, uint16_t width, uint32_t height, uint32_t interlace, uint32_t resource,
                 uint16_t y)
{
    Regs r;
    r.d[0] = n;
    r.d[1] = width;
    r.d[2] = height;
    r.d[3] = 0;
    r.d[4] = interlace;
    r.a[0] = resource + peek_l(ptr<uint8_t>(resource + 2));
    lib_call(L_Dia_RScOpen, r);
    if (uint16_t(r.d[0]))
        return false;
    ptr<Screen>(wdata().EcCourant)->EcAWY = y;
    return true;
}

bool reserve_zones(uint16_t n)
{
    Regs r;
    r.d[1] = n;
    sy_call(sy::ResZone, r);
    return !r.d[0];
}

// Tt_Free: all but the data; the program's banks back
void free_all()
{
    IData &d = idata();
    // (the assembler went on when the data had not been reserved, the
    // program's banks then taken from wherever a4 pointed)
    if (!d.Mon_Base)
        return;
    MonData &m = mon();
    patches(false);
    Regs c;
    c.d[0] = 0;
    lib_call(L_SetChrPatch, c);
    d.ExtTests[1] = 0;
    wdata().Actualise &= ~BitStep;
    d.ActuMask &= ~BitStep;
    d.Patch_Menage = 0;
    d.Patch_Errors = 0;
    d.Patch_ScFront = 0;
    d.Patch_ScCopy = 0;
    // (the assembler also did this when it had failed to start, before
    // keeping the error routine: the program's errors then jumped to 0)
    if (m.Tt_JError)
        d.Prg_JError = m.Tt_JError;
    for (be_u32 &v : d.VblRout)
        if (v == address(mon_view_inter)) {
            v = 0;
            break;
        }
    banks_monitor();
    Regs ch;
    ch.d[0] = ChannelN;
    lib_call(L_Dia_CloseChannel, ch);
    Regs e1;
    e1.d[1] = Ecran1;
    ec_call(ec::Del, e1);
    Regs e2;
    e2.d[1] = Ecran2;
    ec_call(ec::Del, e2);
    help_free();
    if (m.Ttt_BufE)
        free_memory(m.Ttt_BufE, edit_buffer_size());
    if (m.Tt_BufT)
        free_memory(m.Tt_BufT, TokenBuffer);
    if (m.Inf_Buf)
        free_memory(m.Inf_Buf, InfSize);
    banks_program();
}

// Tt_MonFree
void free_data()
{
    IData &d = idata();
    if (uint32_t a = d.Mon_Base) {
        // (the assembler left Mon_Base pointing at the freed data: the next
        // call of the monitor from a program took it as there)
        d.Mon_Base = 0;
        free_memory(a, MonData::Tt_DzLong);
    }
}

// Tt_ColdStart: the monitor for program; false if out of memory
bool cold_start(uint32_t program)
{
    IData &d = idata();
    uint32_t base = memory(MonData::Tt_DzLong);
    if (!base)
        return false;
    d.Mon_Base = base;
    MonData &m = mon();
    m.Tt_Program = program;
    m.Tt_CurBanks = d.Cur_Banks;
    m.Tt_CurDialogs = d.Cur_Dialogs;
    m.Tt_Ecran = 0xFFFF;
    m.Tt_View = 0xFFFF;
    m.Tt_YMark = 0xFFFF;
    m.Tt_YNext = 0xFFFF;
    m.Tt_NotHere = 0;
    uint16_t sy = uint16_t(d.Ed_Sy - E1Sy);
    if (d.Ed_Inter)
        sy = uint16_t(sy - E1Sy);
    sy &= 0xFFF0;
    if (sy < E2MinY)
        sy = E2MinY;
    m.Ttt_Sy = sy;

    if (!(m.Ttt_BufE = memory(edit_buffer_size())))
        return false;
    if (!(m.Tt_BufT = memory(TokenBuffer)))
        return false;
    m.Tt_Ligne = m.Tt_BufT + 512;
    if (!(m.Inf_Buf = memory(InfSize)))
        return false;

    // The resource bank, the top screen and its buttons
    banks_monitor();
    Regs b;
    b.d[0] = 16;
    lib_call(L_Bnk_GetAdr, b);
    if (!b.a[0])
        return false;
    m.Tt_Resource = b.a[0];
    // (not interlaced)
    if (!open_screen(Ecran1, E1Sx, E1Sy, 0, m.Tt_Resource, d.Ed_Wy) || !reserve_zones(NButtons))
        return false;
    picture(1, 0, 0);
    buttons_draw();
    mode_set(2);
    view_erase();
    screens_refresh();

    // The text screen, the dialogue drawing its background
    if (!open_screen(Ecran2, E2Sx, uint32_t(int32_t(int16_t(m.Ttt_Sy))), d.Ed_Inter, d.Ed_Resource,
                     uint16_t(d.Ed_Wy + E1Sy)) ||
        !reserve_zones(4))
        return false;
    Regs p;
    p.a[1] = address(mon_window_init);
    wi_call(wi::Print, p);
    Regs o;
    o.d[0] = ChannelN;
    o.d[1] = 512;
    o.d[2] = 12;
    o.d[3] = 0;                                 // not copied
    o.a[0] = address(mon_dbl);
    o.a[1] = d.Ed_Resource + peek_l(ptr<uint8_t>(d.Ed_Resource + 2));
    o.a[2] = 0;
    lib_call(L_Dia_OpenChannel, o);
    if (o.d[0])
        return false;
    m.Tt_Channel = o.a[0];
    m.Tt_Variables = o.a[1];
    listing_size(uint16_t((m.Ttt_Sy >> 1) + 8));
    if (!listing_screen())
        return false;
    listing_new();
    m.Inf_Pos = m.Inf_Buf;
    const uint8_t *separation = message(5);
    info_begin(2);
    info_print(separation);
    info_end();
    info_begin(4);
    info_print(separation);
    info_end();

    // The patches
    d.ExtTests[1] = address(mon_tests_patch_entry);
    d.Patch_ScFront = address(mon_screen_front);
    d.Patch_ScCopy = address(mon_screen_copy_entry);
    Regs c;
    c.d[0] = L_Mon_MonitorChr;
    lib_call(L_SetChrPatch, c);
    m.Tt_JError = d.Prg_JError;
    patches(true);
    for (be_u32 *v = d.VblRout;; v++)
        if (!*v) {
            view_data_zone = address(ctx().a5);
            *v = address(mon_view_inter);
            break;
        }
    view_reset();
    activate(Ecran1);
    view_update();
    mouse_free();
    // 4K of chip memory at least
    if (hal::available_memory(hal::MemoryPublic | hal::MemoryChip) < 4 * 1024)
        return false;
    screens_refresh();
    return true;
}

} // namespace

extern "C" {
// Tt_ColdStart: a3 the program; d0 0, or 1 if out of memory (all freed)
void mon_cold_start(AllRegs *r)
{
    if (cold_start(r->a[3])) {
        r->d[0] = 0;
        return;
    }
    free_all();
    free_data();
    r->d[0] = 1;
}

void mon_free(AllRegs *) { free_all(); }
}
