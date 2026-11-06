// The rest of the editor (+Edit.s): the tab value (Ed_STab), the
// windows' slider routines (Edt_SlUpdateY, Edt_SlDrawY), closing the
// windows of a program for a program driving the editor (Ed_CloseName),
// the bottom of the direct mode's window (Ed_Enlarge), the buttons drawn
// (Bt_EdDraw, Bt_EsCDraw), the disk errors (Ed_DError, Ed_PrgLoadErr,
// Ed_PErr), Workbench (Ed_Wb), the system file closed (Ed_Close).

#include "runtime/static_symbols.h"
#include "editor/editor.h"

#include <dos/dos.h>

#include <dos/dos.h>

#include "hal/files.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EcEdit = 9;
constexpr uint16_t EdDSetTab = 38, EdDDiskErr = 58;
constexpr uint16_t DEBase = 45 + 35 - 1;         // the disk errors' messages

[[noreturn]] void jump(host::RoutineArg routine)
{
    Regs r;
    call(routine, r);
    __builtin_unreachable();
}

// Ed_STab
void set_tab()
{
    IData &d = idata();
    tokenise_current_line();
    ptr<be_u32>(d.Ed_VDialogues)[2] = d.Ed_Tabs;
    dialogue(EdDSetTab);
    Regs v;
    v.d[0] = 1;
    v.d[1] = 3;
    v.d[2] = 0xFFFFFFFF;
    lib_call(L_Dia_GetValue, v);
    d.Ed_Tabs = uint16_t(v.d[1]);
    d.EdC_Changed = 1;
    place_cursor();
}

// Edt_SlUpdateY / Edt_SlDrawY: the window's slider from the window, the
// window from the slider
void slider_update()
{
    EditWindow &w = window();
    auto s = reinterpret_cast<Slider *>(w.Edt_SlV);
    s->Sl_Window = w.Edt_WindTy;
    s->Sl_Position = w.Edt_YPos;
    s->Sl_Global = program().Prg_NLigne;
}

void slider_draw()
{
    EditWindow &w = window();
    w.Edt_YPos = reinterpret_cast<Slider *>(w.Edt_SlV)->Sl_Position;
    new_buffer();
}

// Ed_CloseName: the windows of the program of Name1 closed, unless it is
// running
void close_name()
{
    for (;;) {
        Regs a;
        call(AMOS_ROUTINE(ed_window_of_name_entry), a);
        if (!a.d[0])
            return;
        EditWindow *w = ptr<EditWindow>(a.d[0]);
        ctx().a4 = reinterpret_cast<uint8_t *>(w);
        ctx().a6 = ptr<uint8_t>(w->Edt_Prg);
        Regs r;
        lib_call(L_Prg_DejaRunned, r);
        if (r.d[0])
            return;
        Regs c;
        call(AMOS_ROUTINE(ed_close_window_entry), c);
    }
}

// Ed_Enlarge: lines y1 to y2 of the editor's screen, the middle of a
// picture stretched to the width
void enlarge(int16_t y1, int16_t y2)
{
    IData &d = idata();
    Regs r;
    r.a[0] = r.a[1] = wdata().EcAdr[EcEdit];
    r.d[0] = 160;
    r.d[1] = r.d[3] = uint32_t(int32_t(y1));
    r.d[2] = uint32_t(int32_t(int16_t(d.Ed_Sx - (480 - 160))));
    r.d[4] = 480;
    r.d[5] = uint32_t(int32_t(y2));
    lib_call(L_ScCopy, r);
}

// Bt_EdDraw / Bt_EsCDraw: a button's picture
void button_draw(const Button *b, bool title)
{
    unpack(uint16_t(b->Bt_Image + b->Bt_Pos), int16_t(b->Bt_X), int16_t(b->Bt_Y), title);
}

// Ed_DBis: run-time message n in a box (an alert for a program driving
// the editor), then the editor's loop
[[noreturn]] void disk_message(uint16_t n)
{
    IData &d = idata();
    uint8_t length;
    const uint8_t *text = message_of(ptr<uint8_t>(d.Ed_RunMessages), n, &length);
    if (d.Ed_Zappeuse) {
        Regs z;
        z.a[0] = address(text);
        z.d[0] = length;
        call(AMOS_ROUTINE(ed_zap_alert_entry), z);
    }
    ptr<be_u32>(d.Ed_VDialogues)[0] = address(text);
    Regs c;
    call(AMOS_ROUTINE(ed_alert_clean_entry), c);
    sound_play('G');
    dialogue(EdDDiskErr);
    jump(AMOS_ROUTINE(ed_loop));
}

// Ed_DError: the message of the last DOS error
[[noreturn]] void editor_disk_error()
{
    static const uint8_t known[] = {203, 204, 205, 210, 213, 214, 216, 218, 220, 221, 222, 223, 224, 225, 226};
    int32_t error = hal::io_error();
    uint16_t n = DEBase + 22 + 1;
    for (uint16_t i = 0; i < sizeof(known); i++)
        if (uint8_t(error) == known[i]) {
            n = uint16_t(DEBase + i + 1);
            break;
        }
    disk_message(n);
}

// Ed_PrgLoadErr: an error of Prg_Load
[[noreturn]] void load_error(int16_t error)
{
    if (error == -2)
        jump(AMOS_ROUTINE(ed_out_of_memory));
    if (error == -3)
        jump(AMOS_ROUTINE(ed_not_amos));
    editor_disk_error();
}

} // namespace

extern "C" {
void ed_set_tab(AllRegs *) { set_tab(); }
void ed_slider_update(AllRegs *) { slider_update(); }
void ed_slider_draw(AllRegs *) { slider_draw(); }
void ed_close_name(AllRegs *) { close_name(); }
void ed_enlarge_fn(AllRegs *r) { enlarge(int16_t(r->d[0]), int16_t(r->d[1])); }
void ed_button_draw(AllRegs *r) { button_draw(ptr<Button>(r->a[2]), false); }
void ed_button_draw_title(AllRegs *r) { button_draw(ptr<Button>(r->a[2]), true); }
void ed_disk_error(AllRegs *) { editor_disk_error(); }
void ed_load_error(AllRegs *r) { load_error(int16_t(r->d[0])); }
void ed_print_error_fn(AllRegs *) { disk_message(216); }

// Ed_Close: the file of Handle(a5) closed
void ed_close_fn(AllRegs *)
{
    IData &d = idata();
    if (BPTR f = BPTR(d.Handle)) {
        d.Handle = 0;
        hal::close(f);
    }
}

// Ed_Wb: back to Workbench
void ed_workbench(AllRegs *)
{
    Regs r;
    r.d[1] = 0;
    ec_call(ec::AMOS_WB, r);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_misc.inc"
#endif
