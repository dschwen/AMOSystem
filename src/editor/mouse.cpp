// The mouse in the editor (+Edit.s Ed_Mouse): the buttons pressed (Ed_MKey
// the new presses, Ed_MkFl those held, Ed_MkCpt how long the same ones
// have been), the menu on the right button, the zones of the editor's
// screen: the general buttons, the separations and buttons of the
// windows, the text, the slider.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_macro_stop[], ed_menu[], ed_menu_error[], ed_function_call[],
    ed_separation_top[], ed_separation_bottom[], ed_get_systeme[];

namespace {

constexpr uint16_t EcEdit = 9;
constexpr uint16_t ButtonsZones = 128;      // Ed_BoutonsZones
constexpr uint8_t EtABXY = 0x0C;

// Edt_GetAd: the edit window of zone/window number n, 0 if none
EditWindow *window_of(uint16_t n)
{
    for (uint32_t w = idata().Edt_List; w; w = ptr<EditWindow>(w)->Edt_Next)
        if (ptr<EditWindow>(w)->Edt_Window == n)
            return ptr<EditWindow>(w);
    return nullptr;
}

void function_call(uint16_t function, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    r.d[2] = function;
    call(ed_function_call, r);
}

void call_a0(uint8_t *routine, uint32_t a0)
{
    Regs r;
    r.a[0] = a0;
    call(routine, r);
}

// Ed_MEnd / Ed_MQuit
bool done()
{
    idata().Ed_CuFlag = 0;
    return true;
}

bool mouse()
{
    IData &d = idata();
    EditWindow &w = window();
    Regs k;
    sy_call(sy::MouseKey, k);
    uint8_t keys = uint8_t(k.d[1]);
    d.Ed_MkIns = keys;
    if (keys == d.Ed_OMKey) {
        d.Ed_MkCpt++;
    } else {
        d.Ed_MkCpt = 0;
        d.Ed_OMKey = keys;
    }
    for (int b = 1; b >= 0; b--) {
        uint8_t bit = 1 << b;
        if (d.Ed_MkFl & bit) {
            d.Ed_MKey &= ~bit;
            if (!(keys & bit))
                d.Ed_MkFl &= ~bit;
        } else if (keys & bit) {
            d.Ed_MkFl |= bit;
            d.Ed_MKey |= bit;
        }
    }
    uint8_t pressed = d.Ed_MkIns & 3;
    if (!pressed)
        return false;
    if (d.EdMa_Tape) {
        Regs r;
        call(ed_macro_stop, r);
    }
    d.EdMa_Play = 0;
    // The menu
    if (pressed & 2) {
        Regs m;
        int32_t chosen = int32_t(call(ed_menu, m));
        if (chosen < 0) {
            Regs e;
            call(ed_menu_error, e);
        } else if (chosen) {
            function_call(uint16_t(m.d[2]));
        }
        return done();
    }
    // A zone of the editor's screen
    Regs z;
    sy_call(sy::GetZone, z);
    if (uint16_t(z.d[1]) != EcEdit)
        return false;
    uint16_t zone = uint16_t(z.d[1] >> 16);
    if (zone >= ButtonsZones) {
        // a general button
        Regs b;
        b.a[0] = address(d.Ed_Boutons);
        lib_call(L_Bt_Gere, b);
        if (b.d[0]) {
            uint16_t n = peek_w(ptr<uint8_t>(b.d[0]));
            Regs s;
            s.d[0] = 13;
            call(ed_get_systeme, s);
            function_call(uint16_t(ptr<uint8_t>(s.a[0])[n - 1] - 1));
        }
        return done();
    }
    uint16_t window_number = zone & 0xFFF8, part = zone & 7;
    // a separation of the windows moved
    if (part == 1 || part == 2) {
        tokenise_current_line();
        call_a0(part == 1 ? ed_separation_top : ed_separation_bottom, address(window_of(window_number)));
        return done();
    }
    // a button of a window
    if (part) {
        if (EditWindow *e = window_of(window_number)) {
            Regs b;
            b.a[0] = address(e->Edt_Bt1);
            lib_call(L_Bt_Gere, b);
            if (b.d[0])
                return done();
        }
    }
    // another window
    if (window_number != w.Edt_Window) {
        tokenise_current_line();
        if (EditWindow *e = window_of(window_number)) {
            activate(e);
            return done();
        }
    }
    // In the text: the cursor there; on it, the block
    Regs c;
    sy_call(sy::XyMou, c);
    c.d[3] = 0;
    sy_call(sy::XyScr, c);
    sy_call(sy::XyWin, c);
    if (!(int32_t(c.d[1] | c.d[2]) < 0) && pressed & 1) {
        if (d.Ed_MkCpt && d.Ed_MkCpt < 20)
            return false;
        uint16_t x = uint16_t(c.d[1]) + w.Edt_XPos;
        if (x >= 250)
            return false;
        uint16_t y = uint16_t(c.d[2]) + w.Edt_YPos;
        if (y > program().Prg_NLigne)
            return false;
        y -= w.Edt_YPos;
        cursor_off();
        if (y != w.Edt_YCu)
            tokenise_current_line();
        w.Edt_YOldBloc = w.Edt_YCu + w.Edt_YPos;
        if (y == w.Edt_YCu && x == w.Edt_XCu) {
            if (!d.Ed_MkCpt)
                block_on();
            return done();
        }
        w.Edt_YCu = y;
        w.Edt_XCu = x;
        w.Edt_EtatAff |= EtABXY;
        place_cursor();
        return done();
    }
    // The slider
    auto slider = reinterpret_cast<Slider *>(w.Edt_SlV);
    if (slider->Sl_Zone == zone) {
        cursor_off();
        tokenise_current_line();
        Regs s;
        s.a[0] = address(slider);
        lib_call(L_Sl_Clic, s);
    }
    return false;
}

} // namespace

extern "C" {
// Ed_Mouse: d0 -1 if the mouse did something, 0 if not
void ed_mouse(AllRegs *r) { r->d[0] = mouse() ? 0xFFFFFFFF : 0; }
}
