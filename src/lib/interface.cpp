// The Interface instructions of AMOSPro.Lib (IO.s): Dialog Open/Run/Box,
// Vdialog, Rdialog, Resource Bank/Screen Open/Unpack. The channels and
// their interpreter are in lib/dialog.cpp.

#include "lib/banks.h"
#include "lib/dialog.h"
#include "lib/strings.h"

using namespace amos;

// unpack.cpp
extern "C" uint32_t lib_unpack_bitmap(RegFrame &r);

namespace {

constexpr uint32_t EntNul = 0x80000000;

// Dia_GoError: an Interface error (AMOS errors 120-131)
[[noreturn]] void dia_error(uint32_t error) { lib_jump(L_Error, error + IDiaErrors); }

// The resource the Interface uses: graphics, messages, programs
struct Resource {
    const uint8_t *programs;
    const uint8_t *graphics;
    const uint8_t *messages;
    uint16_t count;             // programs of a Resource Bank
};

// Dia_GetDefault: the default resource (Sys_Resource)
Resource default_resource()
{
    uint32_t r = idata().Sys_Resource;
    if (!r)
        lib_jump(L_BkNoRes, 0);
    const uint8_t *b = ptr<uint8_t>(r);
    return {b + peek_l(b + 10), b + peek_l(b + 2), b + peek_l(b + 6), 0};
}

// Dia_GetPuzzle: the bank of Resource Bank, over the default resource
Resource resource()
{
    uint32_t n = idata().IDia_BankPuzzle;
    if (!n)
        return default_resource();
    uint8_t *bank = bank_find(n);
    if (!bank || peek_l(bank - 8) != 0x5265736F)       // "Reso"
        lib_jump(L_BkNoRes, 0);
    Resource r = default_resource();
    uint16_t parts = peek_w(bank);
    if (parts >= 1 && peek_l(bank + 2))
        r.graphics = bank + peek_l(bank + 2);
    if (parts >= 2 && peek_l(bank + 6))
        r.messages = bank + peek_l(bank + 6);
    if (parts >= 3 && peek_l(bank + 10)) {
        r.programs = bank + peek_l(bank + 10);
        r.count = peek_w(r.programs);
    }
    return r;
}

// A program: a string (copied), or a number in the resource bank
const uint8_t *program(const Resource &r, uint32_t p, bool &copy)
{
    if (p >= 1024) {
        copy = true;
        return ptr<uint8_t>(p);
    }
    if (static_cast<uint16_t>(p) > r.count || !static_cast<uint16_t>(p))
        fon_call();
    copy = false;
    return r.programs + peek_l(r.programs + 2 + (static_cast<uint16_t>(p - 1) << 2));
}

int32_t positive(int32_t n)
{
    if (n <= 0)
        fon_call();
    return n;
}

} // namespace

// Dia_WarmInit
extern "C" uint32_t lib_dia_warm_init(RegFrame &r)
{
    idata().IDia_BankPuzzle = 0;
    idata().IDia_Error = 0;
    return r.d[0];
}

// Dia_GetDefault / Dia_GetPuzzle: Out: a0 = programs, a1 = graphics,
// a2 = messages, d0 = number of programs (of a Resource Bank)
static uint32_t resource_regs(RegFrame &f, const Resource &r)
{
    f.a[0] = address(r.programs);
    f.a[1] = address(r.graphics);
    f.a[2] = address(r.messages);
    return f.d[0] = r.count;
}
extern "C" uint32_t lib_dia_get_default(RegFrame &f) { return resource_regs(f, default_resource()); }
extern "C" uint32_t lib_dia_get_puzzle(RegFrame &f) { return resource_regs(f, resource()); }

// DIALOG OPEN channel,program[,variables[,buffer]]
extern "C" void lib_in_dialog_open4(int32_t channel, uint32_t prog, int32_t variables, uint32_t buffer)
{
    Resource r = resource();
    if (buffer <= 256 || variables < 0)
        fon_call();
    bool copy;
    const uint8_t *p = program(r, prog, copy);
    positive(channel);
    idata().IDia_Error = 0;
    DiaOpen o = dia_open(static_cast<uint32_t>(channel), p, r.graphics, r.messages, buffer,
                         static_cast<uint32_t>(variables), copy);
    if (o.error) {
        idata().IDia_Error = o.position;
        dia_error(o.error);
    }
}
extern "C" void lib_in_dialog_open3(int32_t c, uint32_t p, int32_t v) { lib_in_dialog_open4(c, p, v, 1024); }
extern "C" void lib_in_dialog_open2(int32_t c, uint32_t p) { lib_in_dialog_open4(c, p, 16, 1024); }

// =EDIALOG: the position of the last error
extern "C" uint32_t lib_fn_edialog() { return idata().IDia_Error; }

// DIALOG CLOSE [channel]
extern "C" void lib_in_dialog_close0() { dia_close_all(); }
extern "C" void lib_in_dialog_close1(int32_t c)
{
    if (uint16_t e = dia_close(static_cast<uint32_t>(positive(c))))
        dia_error(e);
}

// DIALOG CLR channel
extern "C" void lib_in_dialog_clr(int32_t c)
{
    if (uint16_t e = dia_erase(static_cast<uint32_t>(positive(c))))
        dia_error(e);
}

// DIALOG FREEZE / UNFREEZE [channel]
extern "C" void lib_in_dialog_freeze0() { dia_freeze_all(true); }
extern "C" void lib_in_dialog_unfreeze0() { dia_freeze_all(false); }
extern "C" void lib_in_dialog_freeze1(int32_t c)
{
    if (uint16_t e = dia_freeze(static_cast<uint32_t>(positive(c)), true))
        dia_error(e);
}
extern "C" void lib_in_dialog_unfreeze1(int32_t c)
{
    if (uint16_t e = dia_freeze(static_cast<uint32_t>(positive(c)), false))
        dia_error(e);
}

// DIALOG UPDATE channel,zone[,a[,b[,c]]]
extern "C" void lib_in_dialog_update5(int32_t c, uint32_t z, uint32_t a, uint32_t b, uint32_t v)
{
    DiaResult x = dia_update(static_cast<uint32_t>(positive(c)), z, a, b, v);
    if (x.error)
        dia_error(x.error);
}
extern "C" void lib_in_dialog_update4(int32_t c, uint32_t z, uint32_t a, uint32_t b)
{
    lib_in_dialog_update5(c, z, a, b, EntNul);
}
extern "C" void lib_in_dialog_update3(int32_t c, uint32_t z, uint32_t a) { lib_in_dialog_update5(c, z, a, EntNul, EntNul); }
extern "C" void lib_in_dialog_update2(int32_t c, uint32_t z) { lib_in_dialog_update5(c, z, EntNul, EntNul, EntNul); }

// =DIALOG RUN(channel[,label][,x,y])
extern "C" uint32_t lib_fn_dialog_run4(int32_t c, int32_t label, uint32_t x, uint32_t y)
{
    if (label >= 65536)
        fon_call();
    positive(c);
    // Not with the monitor
    if (idata().Patch_Errors)
        fon_call();
    idata().IDia_Error = 0;
    DiaResult r = dia_run(static_cast<uint32_t>(c), label, x, y);
    if (r.error) {
        idata().IDia_Error = static_cast<uint32_t>(r.value);
        dia_error(r.error);
    }
    return static_cast<uint32_t>(r.value);
}
extern "C" uint32_t lib_fn_dialog_run2(int32_t c, int32_t l) { return lib_fn_dialog_run4(c, l, EntNul, EntNul); }
extern "C" uint32_t lib_fn_dialog_run1(int32_t c) { return lib_fn_dialog_run4(c, -1, EntNul, EntNul); }

// =DIALOG(channel): the last zone used, -1 when the dialog is closed
extern "C" uint32_t lib_fn_dialog(int32_t c)
{
    DiaResult r = dia_return(static_cast<uint32_t>(positive(c)));
    if (r.error)
        dia_error(r.error);
    return static_cast<uint32_t>(r.value);
}

// VDIALOG(channel,n)=v, =VDIALOG(channel,n), and the same with $
static int32_t *variable(uint32_t c, uint32_t n)
{
    uint16_t e;
    int32_t *v = dia_variable(c, n, &e);
    if (e)
        dia_error(e);
    return v;
}
extern "C" void lib_in_vdialog(uint32_t c, uint32_t n, uint32_t v) { *variable(c, n) = static_cast<int32_t>(v); }
extern "C" uint32_t lib_fn_vdialog(uint32_t c, uint32_t n) { return static_cast<uint32_t>(*variable(c, n)); }

// =RDIALOG(channel,zone[,which]), =RDIALOG$(...)
static DiaValue value(uint32_t c, uint32_t z, uint32_t which)
{
    DiaValue v = dia_value(c, z, which);
    if (v.error)
        dia_error(v.error);
    return v;
}
extern "C" uint32_t lib_fn_rdialog3(uint32_t c, uint32_t z, uint32_t w)
{
    DiaValue v = value(c, z, w);
    return v.type ? 0 : v.value;
}
extern "C" uint32_t lib_fn_rdialog2(uint32_t c, uint32_t z) { return lib_fn_rdialog3(c, z, 1); }
extern "C" uint32_t lib_fn_rdialogd3(uint32_t c, uint32_t z, uint32_t w)
{
    DiaValue v = value(c, z, w);
    return v.type ? str_from_cstr(ptr<char>(v.value)) : str_empty();
}
extern "C" uint32_t lib_fn_rdialogd2(uint32_t c, uint32_t z) { return lib_fn_rdialogd3(c, z, 1); }

// =ZDIALOG(channel,x,y): the zone at x,y
extern "C" uint32_t lib_fn_zdialog(uint32_t c, uint32_t x, uint32_t y)
{
    DiaResult r = dia_zone_at(c, x, y);
    if (r.error)
        dia_error(r.error);
    return static_cast<uint32_t>(r.value);
}

// =DIALOG BOX(program[,v[,v$]][,x,y]): open, run and close a dialog
extern "C" uint32_t lib_fn_dialog_box5(uint32_t prog, uint32_t v, uint32_t vs, uint32_t x, uint32_t y)
{
    if (idata().Patch_Errors)
        fon_call();
    Regs s = {};
    lib_call(L_SaveRegs, s);
    Resource r = resource();
    bool copy;
    const uint8_t *p = program(r, prog, copy);
    DiaResult d = dia_run_quick(p, r.graphics, r.messages, x, y, v, vs, 0, copy, 1024);
    if (d.error) {
        idata().IDia_Error = static_cast<uint32_t>(d.value);
        dia_error(d.error);
    }
    Regs l = {};
    lib_call(L_LoadRegs, l);
    return static_cast<uint32_t>(d.value);
}
extern "C" uint32_t lib_fn_dialog_box3(uint32_t p, uint32_t v, uint32_t vs)
{
    return lib_fn_dialog_box5(p, v, vs, EntNul, EntNul);
}
extern "C" uint32_t lib_fn_dialog_box2(uint32_t p, uint32_t v)
{
    return lib_fn_dialog_box5(p, v, idata().ChVide, EntNul, EntNul);
}
extern "C" uint32_t lib_fn_dialog_box1(uint32_t p) { return lib_fn_dialog_box5(p, EntNul, idata().ChVide, EntNul, EntNul); }

// RESOURCE SCREEN OPEN screen,width,height,flash
extern "C" void lib_in_resource_screen_open(uint32_t n, uint32_t w, uint32_t h, uint32_t flash)
{
    Resource r = resource();
    if (n >= 8)
        lib_jump(L_IllScN, 0);
    uint32_t error;
    uint8_t *s = dia_resource_screen(r.graphics, n, w, h, flash, -1, &error);
    if (static_cast<uint16_t>(error))
        lib_jump(L_EcWiErr, error);
    idata().ScOnAd = address(s);
    idata().ScOn = static_cast<uint16_t>(reinterpret_cast<Screen *>(s)->EcNumber + 1);
}

// RESOURCE BANK n (0: the default resource)
extern "C" void lib_in_resource_bank(int32_t n)
{
    if (n < 0)
        fon_call();
    idata().IDia_BankPuzzle = static_cast<uint32_t>(n);
}

// RESOURCE UNPACK image,x,y: into the current screen
extern "C" void lib_in_resource_unpack(int32_t n, uint32_t x, uint32_t y)
{
    const uint8_t *g = resource().graphics;
    if (n <= 0 || static_cast<uint16_t>(n) > peek_w(g))
        fon_call();
    g += peek_l(g + 2 + (static_cast<uint16_t>(n - 1) << 2));
    if (!idata().ScOnAd)
        lib_jump(L_ScNOp, 0);
    RegFrame f = {};
    f.a[0] = address(g);
    f.a[1] = idata().ScOnAd;
    f.d[1] = x;
    f.d[2] = y;
    if (!static_cast<uint16_t>(lib_unpack_bitmap(f)))
        fon_call();
}
