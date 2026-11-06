// Direct mode (+Edit.s Ed_Escape, Esc_*, Es_Display, Ed_ErrDirect,
// Ed_GetError): a window below a title screen of buttons, a line editor;
// each line entered is tokenised and run by the interpreter, which comes
// back through Esc_Loop (or Ed_ErrDirect on an error).
//
// The lines entered are kept (Esc_KMem, the last first): the next one,
// the size of the element (a word), the text. Esc_KMemPos is the one
// recalled.
//
// Esc_Loop resets the stack (+Edit.s) and comes here: escape_loop() does
// not return, it ends in the interpreter or the editor's loop.

#include "runtime/static_symbols.h"
#include "editor/editor.h"
#include "hal/clipboard.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EcFonc = 8, EcEdit = 9;
constexpr uint16_t Hide = 1 << (8 + 7), AWChange = 1 << (8 + 2);
constexpr uint16_t BitActualise4 = 1 << (8 + 4);
constexpr uint8_t Shf = 0x03, Ctr = 0x08;
constexpr uint16_t EsPics = 38, EsButtonPics = 41;
constexpr uint16_t ButtonSx = 32, ButtonSy = 16, ButtonZones = 1, TitleSx = ButtonSx * 4, TitleSy = 16;
constexpr uint16_t ButtonsX = ButtonSx + TitleSx, ButtonsY = 0, MiniSy = TitleSy + 8;
constexpr uint8_t BtFlagOnOf = 1 << 2;
// LEd_FMulti, FTests, FKeys, FMouse, FCursor, FMouCur
constexpr uint32_t LineFlags = 1 << 6 | 1 << 5 | 1 << 0 | 1 << 4 | 1 << 2 | 1 << 7;

void run(host::RoutineArg routine)
{
    Regs r;
    call(routine, r);
}

[[noreturn]] void jump(host::RoutineArg routine)
{
    run(routine);
    __builtin_unreachable();
}

uint32_t sy_fn(int function, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    sy_call(function, r);
    return r.d[1];
}

void ec_fn(int function, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    ec_call(function, r);
}

void print(const uint8_t *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

Screen *screen(uint16_t n) { return ptr<Screen>(wdata().EcAdr[n]); }

void recop()
{
    sy_fn(sy::WaitVbl);
    ec_fn(ec::CopForce);
}

int16_t mouse_y()
{
    Regs r;
    sy_call(sy::XyMou, r);
    return int16_t(r.d[2]);
}

bool left_button() { return sy_fn(sy::MouseKey) & 1; }

void set_zone(uint16_t n, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    Regs r;
    r.d[1] = n;
    r.d[2] = x1;
    r.d[3] = y1;
    r.d[4] = x2;
    r.d[5] = y2;
    sy_call(sy::SetZone, r);
}

// Es_DisplayD0: the title screen at y1, the window below it to y2
void display(int16_t y1, int16_t y2)
{
    IData &d = idata();
    Screen *title = screen(EcFonc), *text = screen(EcEdit);
    title->EcAWY = uint16_t(y1);
    title->EcAW |= AWChange;
    y1 = int16_t(y1 + TitleSy);
    text->EcAWY = uint16_t(y1);
    text->EcAW |= AWChange;
    int16_t height = int16_t(y2 - y1);
    text->EcAWTY = uint16_t(height);
    text->EcAWT |= AWChange;
    text->EcAVY = uint16_t(d.Ed_Sy - height);
    text->EcAV |= AWChange;
    title->EcFlags &= ~Hide;
    text->EcFlags &= ~Hide;
    ec_fn(ec::First, EcFonc);
    ec_fn(ec::First, EcEdit);
}

void display() { display(int16_t(idata().Es_Y1), int16_t(idata().Es_Y2)); }

// Esc_MaxMouse
void max_mouse()
{
    Regs r;
    r.d[1] = 128 - 16;
    r.d[2] = 35;
    r.d[3] = 500;
    r.d[4] = 312;
    sy_call(sy::LimitM, r);
}

// Esc_Appear: the title screen and the window, sliding down
void appear()
{
    IData &d = idata();
    if (d.Direct)
        return;
    d.Direct = 1;
    int16_t left = int16_t(d.Ed_Sx);
    ec_fn(ec::Active, EcFonc);
    ec_fn(ec::First, EcEdit);
    // the AMOS logo, then the 13 buttons
    unpack(EsPics, ButtonSx, ButtonsY, true);
    left = int16_t(left - TitleSx);
    Button *b = reinterpret_cast<Button *>(d.Ed_Boutons);
    uint16_t x = ButtonsX, picture = EsButtonPics, zone = ButtonZones;
    for (uint16_t n = 1; n <= 13; n++, b++) {
        b->Bt_Number = n;
        if (n == 1) {
            b->Bt_X = 0;                        // DIRECT
        } else if (n == 2) {
            b->Bt_X = uint16_t(d.Ed_Sx - 32);   // WB
        } else {
            b->Bt_X = x;
            x += 32;
        }
        b->Bt_Y = ButtonsY;
        b->Bt_Image = picture;
        b->Bt_Zone = zone;
        b->Bt_Routines.store(AMOS_ROUTINE(ed_button_routines));
        b->Bt_RDraw = 4;
        b->Bt_RPos = 0;
        b->Bt_RChange = 0;
        b->Bt_Flags = 0;
        b->Bt_Pos = 0;
        b->Bt_Sx = ButtonSx;
        b->Bt_Sy = ButtonSy;
        picture += 2;
        zone++;
        left = int16_t(left - ButtonSx);
    }
    b->Bt_Number = 0;
    // the memory gauges
    memory_draw(int16_t(x), ButtonsY, uint16_t(left), true);
    memory_show();
    // the OUT button
    b = reinterpret_cast<Button *>(d.Ed_Boutons);
    b[2].Bt_RPos = 5;
    b[2].Bt_Flags |= BtFlagOnOf;
    Regs i;
    i.a[0] = address(d.Ed_Boutons);
    i.d[0] = 1;
    lib_call(L_Bt_InitList, i);
    set_zone(14, 0, 0, d.Ed_Sx, TitleSy);
    // the window's right border and bottom
    ec_fn(ec::Active, EcEdit);
    int16_t y = 0;
    for (uint16_t n = uint16_t(d.Ed_Ty + 1); n; n--, y = int16_t(y + 8))
        unpack(EsPics + 1, int16_t(d.Ed_Sx - 16), y, false);
    unpack(EsPics + 2, 0, y, false);
    Regs e;
    e.d[0] = uint32_t(int32_t(y));
    e.d[1] = uint32_t(int32_t(y + 8));
    call(AMOS_ROUTINE(ed_enlarge), e);
    Regs w;
    w.d[1] = 1;
    w.d[2] = 0;
    w.d[3] = 0;
    w.d[4] = uint32_t((d.Ed_Sx >> 3) - 2);
    w.d[5] = uint32_t(d.Ed_Ty + 1);
    w.d[6] = 0;
    w.d[7] = 0;
    w.a[1] = 0;
    wi_call(wi::WindOp, w);
    print(system_message(16));
    sy_fn(sy::RazZone, 0);
    set_zone(2, 0, 0, d.Ed_Sx, d.Ed_Sy);
    set_zone(1, 0, 0, uint16_t(d.Ed_Sx - 16), uint16_t(d.Ed_Sy - 8));
    // down it comes
    int16_t y1 = int16_t(d.Es_Y1), y2 = int16_t(y1 + MiniSy);
    for (;;) {
        int16_t was = y2;
        y2 = int16_t(y2 + d.Ed_VScrol);
        if (y2 > int16_t(d.Es_Y2))
            y2 = int16_t(d.Es_Y2);
        display(y1, y2);
        recop();
        if (y2 == was)
            break;
    }
    max_mouse();
}

// Esc_Hide: up it goes, the screens hidden
void hide()
{
    IData &d = idata();
    if (!d.Direct)
        return;
    d.Direct = 0;
    int16_t y1 = int16_t(d.Es_Y1), y2 = int16_t(d.Es_Y2);
    for (;;) {
        int16_t was = y2;
        y2 = int16_t(y2 - d.Ed_VScrol);
        if (y2 < int16_t(y1 + MiniSy))
            y2 = int16_t(y1 + MiniSy);
        display(y1, y2);
        recop();
        if (y2 == was)
            break;
    }
    screen(EcEdit)->EcFlags |= Hide;
    screen(EcFonc)->EcFlags |= Hide;
    recop();
    ec_fn(ec::Active, EcEdit);
    Regs w;
    wi_call(wi::WinDel, w);
    sy_fn(sy::RazZone, 0);
}

// Esc_KMemEffD0: the oldest line kept taken away if there are max or more
void forget_oldest(uint16_t max)
{
    IData &d = idata();
    be_u32 *before = nullptr;
    uint8_t *last = nullptr;
    uint16_t n = 0;
    for (uint32_t e = d.Esc_KMem; e; e = peek_l(ptr<uint8_t>(e))) {
        before = last ? reinterpret_cast<be_u32 *>(last) : nullptr;
        last = ptr<uint8_t>(e);
        n++;
    }
    if (!last || n < max)
        return;
    if (before)
        *before = 0;
    else
        d.Esc_KMem = 0;
    free_memory(address(last), peek_w(last + 4));
}

// Esc_KMemEnd
void forget_all()
{
    while (idata().Esc_KMem)
        forget_oldest(0);
}

uint8_t *line_editor() { return idata().Es_LEd; }

// LEd_New: the line being edited replaced by text
void new_line(const uint8_t *text)
{
    Regs r;
    r.a[0] = address(line_editor());
    r.a[1] = address(text);
    r.d[0] = 0xFFFFFFFF;
    lib_call(L_LEd_New, r);
}

} // namespace

namespace amos::ed {

// Ed_GetError: the message of error code (when message is 0): a test-time
// one for a code below 0, a run-time one above; the code as a word
Error get_error(int16_t code, const uint8_t *message)
{
    IData &d = idata();
    if (message || code >= 256)
        return {message, code};
    if (code <= 0)
        return {message_of(ptr<uint8_t>(d.Ed_TstMessages), uint16_t(-code), nullptr), int16_t(-code)};
    return {message_of(ptr<uint8_t>(d.Ed_RunMessages), uint16_t(code + 1), nullptr), code};
}

} // namespace amos::ed

namespace {

[[noreturn]] void escape_loop();

// Esc_Esc: back to the editor
[[noreturn]] void leave()
{
    sound_play('E');
    hide();
    window().Edt_EtMess = 0;
    run(AMOS_ROUTINE(ed_appear));
    jump(AMOS_ROUTINE(ed_loop));
}

// Esc_R: the line run (an empty one, nothing)
[[noreturn]] void run_line()
{
    IData &d = idata();
    uint8_t *text = ptr<uint8_t>(d.Buffer);
    if (!*text)
        jump(AMOS_ROUTINE(ed_esc_loop));
    forget_oldest(d.Esc_KMemMax);
    uint32_t length = 0;
    while (text[length])
        length++;
    uint32_t size = length + 1 + 6;
    if (uint8_t *e = ptr<uint8_t>(memory(size, true))) {
        poke_l(e, d.Esc_KMem);
        d.Esc_KMem = address(e);
        d.Esc_KMemPos = 0;
        poke_w(e + 4, uint16_t(size));
        for (uint32_t i = 0; i <= length; i++)
            e[6 + i] = text[i];
    }
    static const uint8_t cr[] = {13, 10, 0};
    print(cr);
    if (uint16_t(size) > uint16_t((d.Ed_Sx >> 3) - 9))
        print(cr);
    Regs t;
    t.a[0] = d.Buffer;
    t.a[1] = d.Ed_BufT;
    call(AMOS_ROUTINE(ed_tokenise_entry), t);
    if (!uint16_t(t.d[0]))
        jump(AMOS_ROUTINE(ed_esc_loop));
    // the interpreter, on the current program's banks
    EditWindow *current = ptr<EditWindow>(d.Edt_Current);
    ctx().a4 = host::detail::capture_cursor(reinterpret_cast<uint8_t *>(current));
    ctx().a6 = ptr<uint8_t>(current->Edt_Prg);
    Regs b;
    lib_call(L_Prg_SetBanks, b);
    d.Edt_Runned = 0;
    d.Prg_Runned = 0;
    d.Prg_JError.store(AMOS_ROUTINE(ed_err_direct));
    Regs v;
    lib_call(L_VerDirect, v);
    if (d.ScOn)
        ec_fn(ec::Active, uint32_t(d.ScOn - 1));
    amos_goto(lib_entry(L_New_ChrGet), 0);
}

// Esc_Recall
void recall(uint8_t *e)
{
    idata().Esc_KMemPos = address(e);
    new_line(e + 6);
}

// Esc_BtFonc: function key n (system text 24 + n, a ` at the end runs it)
void function_key(uint16_t n)
{
    IData &d = idata();
    const uint8_t *from = system_message(uint16_t(24 + n));
    uint8_t *to = ptr<uint8_t>(d.Buffer);
    uint8_t c;
    for (;;) {
        c = *from++;
        *to++ = c;
        if (!c)
            break;
        if (c == '`') {
            to[-1] = 0;
            break;
        }
    }
    new_line(ptr<uint8_t>(d.Buffer));
    if (c)
        run_line();
}

// Up / Down: the lines kept; with Control the window moved, with Shift its
// bottom
bool key_up(uint8_t shifts)
{
    IData &d = idata();
    if (shifts & Ctr) {
        if (int16_t(d.Es_Y2) < 0)
            return false;
        d.Es_Y1 -= 8;
        d.Es_Y2 -= 8;
        return true;
    }
    if (shifts & Shf) {
        int16_t min = int16_t(d.Es_Y1 + MiniSy);
        d.Es_Y2 -= 8;
        if (min > int16_t(d.Es_Y2))
            d.Es_Y2 = uint16_t(min);
        return true;
    }
    uint8_t *at = d.Esc_KMemPos ? ptr<uint8_t>(d.Esc_KMemPos) : reinterpret_cast<uint8_t *>(&d.Esc_KMem);
    if (uint32_t e = peek_l(at))
        recall(ptr<uint8_t>(e));
    return false;
}

// false when nothing is to be shown again
bool key_down(uint8_t shifts)
{
    IData &d = idata();
    if (shifts & Ctr) {
        if (int16_t(d.Es_Y1) >= 312)
            return false;
        d.Es_Y1 += 8;
        d.Es_Y2 += 8;
        return true;
    }
    if (shifts & Shf) {
        int16_t max = int16_t(d.Es_Y1 + d.Ed_Sy);
        d.Es_Y2 += 8;
        if (max < int16_t(d.Es_Y2))
            d.Es_Y2 = uint16_t(max);
        return true;
    }
    uint32_t at = d.Esc_KMemPos, e = d.Esc_KMem;
    if (!at || !e)
        return false;
    if (at == e) {
        d.Esc_KMemPos = 0;
        uint8_t *buffer = ptr<uint8_t>(d.Buffer);
        poke_w(buffer, 0);
        new_line(buffer);
        return false;
    }
    for (;;) {
        uint32_t next = peek_l(ptr<uint8_t>(e));
        if (!next)
            return false;
        if (next == at) {
            recall(ptr<uint8_t>(e));
            return false;
        }
        e = next;
    }
}

// Esc_MEcran / Esc_MMove: the window moved, or its bottom
void drag(bool bottom)
{
    IData &d = idata();
    if (bottom) {
        int16_t offset = int16_t(mouse_y() - d.Es_Y2);
        do {
            int16_t y = int16_t(mouse_y() - offset);
            int16_t min = int16_t(d.Es_Y1 + MiniSy), max = int16_t(d.Es_Y1 + d.Ed_Sy);
            if (y < min)
                y = min;
            if (y > max)
                y = max;
            if (y != int16_t(d.Es_Y2)) {
                d.Es_Y2 = uint16_t(y);
                display();
                recop();
            }
        } while (left_button());
        return;
    }
    int16_t offset = int16_t(mouse_y() - d.Es_Y1);
    do {
        int16_t y = int16_t(mouse_y() - offset);
        if (y != int16_t(d.Es_Y1)) {
            uint16_t height = uint16_t(d.Es_Y2 - d.Es_Y1);
            d.Es_Y1 = uint16_t(y);
            d.Es_Y2 = uint16_t(y + height);
            display();
            recop();
        }
    } while (left_button());
}

// Esc_Bouton: button b clicked with mouse buttons keys
void button(Button *b, uint16_t keys)
{
    IData &d = idata();
    switch (b->Bt_Number) {
    case 1:
        leave();
    case 2:
        run(AMOS_ROUTINE(ed_wb));
        return;
    case 3:
        d.Esc_Output = uint8_t(b->Bt_Pos);
        return;
    default:
        function_key(uint16_t(b->Bt_Number - 4 + (keys & 2 ? 10 : 0)));
    }
}

// Esc_MKey: a click on the buttons or in the window
void click()
{
    IData &d = idata();
    Regs z;
    sy_call(sy::GetZone, z);
    uint16_t on = uint16_t(z.d[1]);
    if (on == EcFonc) {
        Regs g;
        g.a[0] = address(d.Ed_Boutons);
        lib_call(L_Bt_Gere, g);
        if (g.d[0])
            return button(ptr<Button>(g.d[0]), uint16_t(g.d[1]));
        return drag(false);
    }
    if (on == EcEdit)
        drag(uint16_t(z.d[1] >> 16) != 1);
}

// Esc_Aff: everything shown again
void show()
{
    display();
    wdata().Actualise |= BitActualise4;
    idata().ActuMask |= BitActualise4;
    sy_fn(sy::WaitVbl);
}

[[noreturn]] void escape_loop()
{
    IData &d = idata();
    wdata().AMOState = 1;
    d.Prg_JError.store(AMOS_ROUTINE(ed_err_direct));
    ec_fn(ec::Active, EcFonc);
    memory_show();
    ec_fn(ec::Active, EcEdit);
    print(system_message(14));
    // the line editor
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    buffer[0] = 0;
    Regs i;
    i.a[0] = address(line_editor());
    i.a[1] = address(buffer);
    i.a[2] = address(buffer);
    i.d[0] = LineFlags;
    i.d[1] = 0;
    i.d[2] = 160;
    i.d[3] = 0xFFFFFFFF;
    i.d[4] = '*';
    lib_call(L_LEd_Init, i);
    show();
    for (;;) {
        ec_fn(ec::Active, EcEdit);
    again:
        boot::wait_vbl_multi();
        Regs l;
        l.a[0] = address(line_editor());
        hal::clipboard_direct_active(true);
        lib_call(L_LEd_Loop, l);
        hal::clipboard_direct_active(false);
        int16_t code = int16_t(l.d[2]);
        if (code < 0)
            goto again;             // Control-C
        if (code == 1)
            run_line();
        if (code == 2) {
            click();
            continue;
        }
        uint8_t scan = uint8_t(l.d[1] >> 16), shifts = uint8_t(l.d[1] >> 24);
        switch (scan) {
        case 0x45:
            leave();
        case 0x4C:
            if (key_up(shifts))
                show();
            continue;
        case 0x4D:
            if (key_down(shifts))
                show();
            continue;
        case 0x5F:
            // Help: the mouse back into the window
            sy_fn(sy::Show, 0xFFFFFFFF);
            max_mouse();
            {
                Regs m;
                m.d[1] = d.Ed_Wx;
                m.d[2] = uint16_t(d.Es_Y1 + 16);
                sy_call(sy::SetM, m);
            }
            show();
            continue;
        }
        if (scan >= 0x50 && scan <= 0x59)
            function_key(uint16_t(scan - 0x50 + (shifts & Shf ? 10 : 0)));
    }
}

// Ed_ErrDirect: an error of the line run (or its end): the editor's
// screens back, the message
[[noreturn]] void error_direct(int16_t code, const uint8_t *message)
{
    IData &d = idata();
    EditWindow *current = ptr<EditWindow>(d.Edt_Current);
    ctx().a4 = host::detail::capture_cursor(reinterpret_cast<uint8_t *>(current));
    ctx().a6 = ptr<uint8_t>(current->Edt_Prg);
    Error e = get_error(code, message);
    run(AMOS_ROUTINE(ed_open_editor));
    d.Edt_Runned = 0;
    d.ActuMask |= 0x1100;
    ec_fn(ec::CopOnOff, 0xFFFFFFFF);
    ec_fn(ec::First, EcEdit);
    ec_fn(ec::First, EcFonc);
    ec_fn(ec::Active, EcEdit);
    recop();
    run(AMOS_ROUTINE(ed_hide));
    appear();
    if (e.code == 1002)
        jump(AMOS_ROUTINE(ed_system));
    if (e.code == 1000)
        leave();
    if (uint16_t(e.code) < 1000 && e.code != 10)
        print(e.message);
    jump(AMOS_ROUTINE(ed_esc_loop));
}

// Ed_Escape: to direct mode
[[noreturn]] void escape()
{
    sound_play('E');
    tokenise_current_line();
    run(AMOS_ROUTINE(ed_hide));
    appear();
    jump(AMOS_ROUTINE(ed_esc_loop));
}

} // namespace

extern "C" {
// Esc_BtGetOutput: the output button (a2) at Esc_Output
void ed_escape_output(AllRegs *r)
{
    be_u16 &pos = ptr<Button>(r->a[2])->Bt_Pos;
    pos = uint16_t((pos & 0xFF00) | idata().Esc_Output);
}
void ed_escape(AllRegs *) { escape(); }
void ed_escape_loop(AllRegs *) { escape_loop(); }
void ed_escape_leave(AllRegs *) { leave(); }
void ed_error_direct(AllRegs *r) { error_direct(int16_t(r->d[0]), ptr<uint8_t>(r->a[0])); }
void ed_escape_appear(AllRegs *) { appear(); }
void ed_escape_hide(AllRegs *) { hide(); }
void ed_escape_display(AllRegs *) { display(); }
void ed_escape_display_d0(AllRegs *r) { display(int16_t(r->d[0]), int16_t(r->d[1])); }
void ed_escape_max_mouse(AllRegs *) { max_mouse(); }
void ed_escape_forget(AllRegs *r) { forget_oldest(uint16_t(r->d[0])); }
void ed_escape_forget_all(AllRegs *) { forget_all(); }

// Ed_GetError: d0 the code, a0 the message or 0; a0 the message, d0 the
// code (low word)
void ed_get_error(AllRegs *r)
{
    Error e = get_error(int16_t(r->d[0]), ptr<uint8_t>(r->a[0]));
    r->a[0] = address(e.message);
    r->d[0] = (uint32_t(int32_t(int16_t(r->d[0]))) & 0xFFFF0000) | uint16_t(e.code);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_direct.inc"
#endif
