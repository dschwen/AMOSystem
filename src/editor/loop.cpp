// The editor's loop and its functions (+Edit.s Ed_Loop, Ed_FCall), and
// programs driving the editor (Ed_ZapIn, Ed_ZapOut, Ed_ZapFonction,
// EdZ_*: the instructions Call Editor and Ask Editor of accessories).
//
// Ed_Loop resets the stack (+Edit.s) and comes here: editor_loop() does
// not return. Neither do the alerts, which go back to Ed_Loop.
// The functions are the JFonc table of +Edit.s (their addresses, from 0), with
// the flags of FlagFonc: bit 0 the buffer to be shown again, 1 the line,
// 2 not in a closed procedure, 5 allowed in a macro, 6 with a command line
// (a program driving the editor gives it), 7 allowed to a program.

#include "runtime/static_symbols.h"
#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EcEdit = 9;
constexpr uint16_t HiddenCommands = 184, HiddenCall = 111;
constexpr uint8_t EtABAll = 0x7F;
constexpr uint16_t XScrollG = 20, XScrollD = 10, YScrollH = 4, YScrollB = 4;
constexpr uint16_t EdDMacro3 = 15;

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

// The current window and its program as a4/a6
void use(EditWindow *w)
{
    ctx().a4 = host::detail::capture_cursor(reinterpret_cast<uint8_t *>(w));
    ctx().a6 = w ? ptr<uint8_t>(w->Edt_Prg) : nullptr;
}

EditWindow *current() { return ptr<EditWindow>(idata().Edt_Current); }

void ec_fn(int function, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    ec_call(function, r);
}

} // namespace

namespace amos::ed {

// Ed_FCall: function n (from 0) of the editor, or of the menu: an
// option with a program, a command of the hidden programs
void function_call(uint16_t n)
{
    IData &d = idata();
    EditWindow &w = window();
    run(AMOS_ROUTINE(ed_clear_error_proc_entry));
    run(AMOS_ROUTINE(ed_menu_program));
    run(AMOS_ROUTINE(ed_cursor_off_entry));
    uint8_t flags = ed_function_flags[n];
    if (d.EdMa_Tape && !(flags & 1 << 5)) {
        // not allowed in a macro
        d.EdMa_Tape -= 3;
        dialogue(EdDMacro3);
        jump(AMOS_ROUTINE(ed_loop));
    }
    d.Ed_SCallFlags = flags;
    if (flags & 1 << 2) {
        uint8_t *line = ptr<uint8_t>(w.Edt_BufE) + int16_t(uint16_t(w.Edt_YCu << 8));
        if (line[255])
            jump(AMOS_ROUTINE(ed_not_edit));
    }
    w.Edt_YOldBloc = uint16_t(w.Edt_YCu + w.Edt_YPos);
    uint32_t d1 = n;
    if (n >= HiddenCommands - 1) {
        // the commands of the hidden programs: Run, Edit or New of one
        uint16_t i = uint16_t(n - (HiddenCommands - 1));
        n = uint16_t(HiddenCall - 1 + i % 3);
        d1 = uint32_t(i / 3) + d.EdM_PosHidden;
    } else if (d.Ed_AutoLoad[3 * n] && !d.Ed_Zappeuse) {
        // an option with a program
        if (d.EdMa_Tape) {
            d.EdMa_Tape -= 3;
            dialogue(EdDMacro3);
            jump(AMOS_ROUTINE(ed_loop));
        }
        Regs p;
        p.a[0] = address(d.Ed_AutoLoad + 3 * n);
        call(AMOS_ROUTINE(ed_program_command_entry), p);
        return;
    }
    wdata().Actualise = 0;
    Regs r;
    r.d[0] = 0;
    r.d[1] = d1;
    r.d[2] = uint32_t(n) << 2;
    call(host::decode_routine(peek_l(ed_function_table + 4 * n)), r);
}

} // namespace amos::ed

namespace {

// Ed_Loop: one turn, then the next
[[noreturn]] void editor_loop()
{
    IData &d = idata();
    for (;;) {
        ec_fn(ec::Active, EcEdit);
        d.Ed_Ok = 1;
        wdata().AMOState = 1;
        // a window to delete (after a merge, an option's program)
        if (EditWindow *w = ptr<EditWindow>(d.Ed_WindowToDel)) {
            d.Ed_WindowToDel = 0;
            use(w);
            if (w->Edt_Hidden) {
                run(AMOS_ROUTINE(ed_delete_window_entry));
                run(AMOS_ROUTINE(ed_branch_amos));
            } else {
                run(AMOS_ROUTINE(ed_close_window_entry));
            }
            continue;
        }
        all_warnings_end();
        // the program before an option's program, back
        if (d.Ed_Prg2ReLoad)
            jump(AMOS_ROUTINE(ed_program_reload_entry));
        EditWindow *w = current();
        use(w);
        d.MathFlags = program().Prg_MathFlags;
        d.Ed_FUndo = 0;
        d.Ed_RunnedHidden = 0;
        // the linked windows, the cursor and the block
        update_split(w, d.Ed_SCallFlags);
        d.Ed_SCallFlags = 0;
        run(AMOS_ROUTINE(ed_follow_scroll_entry));
        run(AMOS_ROUTINE(ed_block_show_entry));
        // the status lines and sliders of the windows shown
        for (EditWindow *e = ptr<EditWindow>(d.Edt_List); e; e = ptr<EditWindow>(e->Edt_Next)) {
            if (e->Edt_Hidden)
                continue;
            use(e);
            e->Edt_EtatAff |= d.Ed_EtatAff;
            if (e->Edt_EtMess && !--e->Edt_EtMess)
                e->Edt_EtatAff = EtABAll;
            run(AMOS_ROUTINE(ed_status_line_d0_entry));
            if (e->Edt_ASlY && !--e->Edt_ASlY)
                show_slider();
        }
        d.Ed_EtatAff = 0;
        // the menus, unless memory is short
        if (!d.Ed_Zappeuse) {
            bool make = d.Ed_MemCurrent >= 72 * 1024;
            if (!make && d.Ed_NewAppear) {
                if (!--d.Ed_NewAppear) {
                    // some room made
                    Regs m;
                    lib_call(L_Mon_Free, m);
                    Regs c;
                    lib_call(L_Math_Close, c);
                    run(AMOS_ROUTINE(ed_undo_reset_all_entry));
                    Regs f;
                    sy_call(sy::MemFlush, f);
                }
                Regs a;
                a.d[0] = 125;
                a.a[0] = address(message(186));
                call(AMOS_ROUTINE(ed_alert_rts_entry), a);
                d.Ed_Sounds = 0;
                run(AMOS_ROUTINE(ed_sounds_changed_entry));
                run(AMOS_ROUTINE(ed_menu_program));
                use(current());
                Regs b;
                lib_call(L_Prg_SetBanks, b);
                Regs x;
                lib_call(L_MemMaximum, x);
                make = true;
            }
            if (make && !d.EdM_Table) {
                menu_init();
                continue;
            }
        }
        // the current window, its cursor
        w = current();
        use(w);
        Regs q;
        q.d[1] = w->Edt_Window;
        wi_call(wi::QWindow, q);
        if (!d.Ed_CuFlag) {
            d.Ed_CuFlag = 1;
            run(AMOS_ROUTINE(ed_cursor_on_entry));
        }
        // scrolled to keep the cursor away from the sides
        uint16_t x = w->Edt_XPos, column = uint16_t(w->Edt_XCu - x);
        if (column < XScrollG && x) {
            run(AMOS_ROUTINE(ed_cursor_off_entry));
            run(AMOS_ROUTINE(ed_cgo_entry));
            continue;
        }
        if (uint16_t(w->Edt_WindTx - w->Edt_XCu + x) < XScrollD && x < 200) {
            run(AMOS_ROUTINE(ed_cursor_off_entry));
            run(AMOS_ROUTINE(ed_cdr_entry));
            continue;
        }
        uint16_t lines = w->Edt_WindTy;
        if (lines > 2) {
            uint16_t top = 1, bottom = 1;
            if (lines >= 7) {
                top = YScrollH / 2;
                bottom = YScrollB / 2;
                if (lines >= (YScrollH + YScrollB) * 2) {
                    top = uint16_t(uint32_t(lines) * YScrollH / 25);
                    bottom = uint16_t(uint32_t(lines) * YScrollB / 25);
                }
            }
            if (top > w->Edt_YCu && w->Edt_YPos) {
                run(AMOS_ROUTINE(ed_cursor_off_entry));
                run(AMOS_ROUTINE(ed_shaut2_entry));
                w->Edt_YCu++;
                place_cursor();
                continue;
            }
            if (bottom >= uint16_t(w->Edt_WindTy - w->Edt_YCu) &&
                uint16_t(w->Edt_YPos + w->Edt_YCu) < program().Prg_NLigne) {
                run(AMOS_ROUTINE(ed_cursor_off_entry));
                run(AMOS_ROUTINE(ed_sbas2_entry));
                w->Edt_YCu--;
                place_cursor();
                continue;
            }
        }
        // a program driving the editor: back to it when settled
        if (d.Ed_Zappeuse) {
            if (++d.Ed_ZapCounter < 5)
                continue;
            d.Ed_ZapCounter = 0;
            jump(AMOS_ROUTINE(ed_zap_out));
        }
        // the keys, the mouse
        Regs k;
        if (uint32_t(call(AMOS_ROUTINE(ed_key_entry), k)))
            continue;
        Regs m;
        if (uint32_t(call(AMOS_ROUTINE(ed_mouse_entry), m)))
            continue;
        // the memory gauges, now and then
        if (!(wdata().VblCount & 63)) {
            memory_show();
            continue;
        }
        // autosave
        if (uint32_t every = d.Ed_AutoSave) {
            uint32_t now = wdata().VblCount;
            if (int32_t(d.Ed_AutoSaveRef) < 0)
                d.Ed_AutoSaveRef = now;
            if (now - d.Ed_AutoSaveRef >= every && !current()->Edt_LEdited) {
                d.Ed_AutoSaveRef = 0xFFFFFFFF;
                run(AMOS_ROUTINE(ed_save_all_entry));
                continue;
            }
        }
        boot::wait_vbl_multi();
    }
}

// Ed_ZapIn: function n with param, for a program (an accessory, not the
// edited one). 0: run it (the function in *function), 1: an error, the
// program reconnected (Ed_ZapOut), 2: an error before it was
int32_t zap_in(uint32_t n, uint32_t param, uint16_t *function)
{
    IData &d = idata();
    EditWindow *runned = ptr<EditWindow>(d.Edt_Runned);
    if (d.Edt_Runned == d.Edt_Current || !d.Prg_Accessory) {
        d.Ed_ZapError = uint16_t(-6);
        d.Ed_ZapMessage = address(message(15));
        return 2;
    }
    d.Ed_ZapParam = param;
    Regs s;
    s.a[0] = address(&ptr<Program>(runned->Edt_Prg)->Prg_ZapData);
    lib_call(L_Prg_DataSave, s);
    Regs n2;
    lib_call(L_Prg_DataNew, n2);
    d.Ed_ZapError = 0;
    uint16_t f = uint16_t(n - 1);
    uint8_t flags = f < HiddenCommands ? ed_function_flags[f] : 0;
    if (!(flags & 0x80)) {
        d.Ed_ZapError = uint16_t(-4);
        d.Ed_ZapMessage = address(message(13));
        return 1;
    }
    if (flags & 0x40 && !peek_w(ptr<uint8_t>(d.Name2))) {
        d.Ed_ZapError = uint16_t(-5);
        d.Ed_ZapMessage = address(message(14));
        return 1;
    }
    d.Ed_Zappeuse = flags & 0xF0;
    ec_fn(ec::Active, EcEdit);
    *function = f;
    return 0;
}

// The editor ready for the function (after the stack is reset)
void zap_prepare()
{
    IData &d = idata();
    use(current());
    Regs b;
    lib_call(L_Prg_SetBanks, b);
    // (the assembler cleared this flag through the window's pointer, a
    // byte far past the window structure)
    d.Ed_YaUTest = 0;
    d.MathFlags = program().Prg_MathFlags;
    d.Ed_ZapCounter = 0;
}

// Ed_ZapOut: the calling program's data and screen back
void zap_out()
{
    IData &d = idata();
    run(AMOS_ROUTINE(ed_menu_program));
    use(ptr<EditWindow>(d.Edt_Runned));
    Regs l;
    l.a[0] = address(&program().Prg_ZapData);
    lib_call(L_Prg_DataLoad, l);
    ec_fn(ec::Last, EcEdit);
    Regs v;
    sy_call(sy::WaitVbl, v);
    ec_fn(ec::CopForce);
    if (d.ScOn)
        ec_fn(ec::Active, uint32_t(d.ScOn - 1));
}

// Ed_ZapFonction: Ask Editor function n with param: d0 the value, or a0
// a string with d2 = 2
struct Answer {
    uint32_t value;
    uint32_t text;
    uint32_t type;
};

Answer zap_function(uint32_t n, uint32_t param)
{
    IData &d = idata();
    if (!d.Prg_Accessory)
        return {uint32_t(-6), address(message(15)), 2};
    // (the table has 13 functions, but only 11 are let through)
    if (n - 1 >= 11)
        return {uint32_t(-7), address(message(16)), 2};
    EditWindow *w = current();
    Program *p = ptr<Program>(w->Edt_Prg);
    auto coordinate = [](uint16_t v) { return uint32_t(int32_t(int16_t(v + 1))); };
    auto block = [&](int which) -> uint32_t {
        uint16_t y1 = w->Edt_YBloc;
        if (int16_t(y1) < 0)
            return 0xFFFFFFFF;
        uint16_t y2 = uint16_t(w->Edt_YCu + w->Edt_YPos), x1 = w->Edt_XBloc, x2 = w->Edt_XCu;
        if (!(y2 > y1 || (y2 == y1 && x2 >= x1))) {
            uint16_t t = y1;
            y1 = y2;
            y2 = t;
            t = x1;
            x1 = x2;
            x2 = t;
        }
        const uint16_t values[] = {x1, y1, x2, y2};
        return coordinate(values[which]);
    };
    switch (n) {
    case 1: {
        // a line of the program (the cursor's if param is not above 0)
        uint8_t *buffer = ptr<uint8_t>(d.Ed_BufT);
        poke_w(buffer, 0);
        Ctx &c = ctx();
        ContextCursor a4 = c.a4;
        uint8_t *a6 = c.a6;
        use(w);
        int32_t line = int32_t(param) > 0 ? int32_t(param) - 1 : int32_t(w->Edt_YCu + w->Edt_YPos);
        if (!(int32_t(param) > 0 && uint16_t(param) > p->Prg_NLigne)) {
            uint8_t *l = find_line(uint16_t(line));
            if (peek_w(l)) {
                Regs t;
                t.a[0] = address(l);
                t.a[1] = address(buffer);
                t.d[0] = 0;
                call(AMOS_ROUTINE(ed_detok_entry), t);
            }
        }
        c.a4 = a4;
        c.a6 = a6;
        buffer[2 + peek_w(buffer)] = 0;
        return {0, address(buffer + 2), 2};
    }
    case 2:
        return {0, address(p->Prg_NamePrg), 2};
    case 3:
        return {coordinate(w->Edt_XCu), 0, 0};
    case 4:
        return {coordinate(uint16_t(w->Edt_YPos + w->Edt_YCu)), 0, 0};
    case 5:
        return {uint32_t(int32_t(int16_t(p->Prg_NLigne))), 0, 0};
    case 6:
        return {block(0), 0, 0};
    case 7:
        return {block(1), 0, 0};
    case 8:
        return {block(2), 0, 0};
    case 9:
        return {block(3), 0, 0};
    case 10:
        return {p->Prg_StBas - p->Prg_StMini, 0, 0};
    default:
        return {address(w), 0, 0};
    }
}

// EdZ_NewConfig: text n (the long at Name1) of a list replaced by the
// string after it (list ZapParam: 1-5 the editor's, 0 the system's;
// above 5 only the flags)
void zap_new_config()
{
    IData &d = idata();
    uint32_t list = d.Ed_ZapParam;
    if (list <= 5) {
        be_u32 *texts = list ? &d.Ed_Systeme + (list - 1) : &d.Sys_Messages;
        uint32_t n = peek_l(ptr<uint8_t>(d.Name1));
        if (n <= text_count(ptr<uint8_t>(*texts)) && !text_change(texts, ptr<uint8_t>(d.Name1 + 4), uint16_t(n)))
            jump(AMOS_ROUTINE(ed_out_of_memory));
    }
    d.EdC_Modified = 1;
    d.EdC_Changed = 1;
}

// EdZ_NewLine / EdZ_NewLineTok: the cursor's line replaced by Name1
void zap_new_line()
{
    IData &d = idata();
    EditWindow &w = window();
    uint8_t *line = ptr<uint8_t>(w.Edt_BufE) + int16_t(uint16_t(w.Edt_YCu << 8)) + 2;
    uint16_t length = 0;
    for (const uint8_t *s = ptr<uint8_t>(d.Name1); (line[length] = s[length]); length++)
        ;
    poke_w(line - 2, length);
    w.Edt_LEdited++;
    show_current_line();
}

} // namespace

extern "C" {
void ed_loop_fn(AllRegs *) { editor_loop(); }
void ed_function_call_fn(AllRegs *r) { function_call(uint16_t(r->d[2])); }
// Ed_GoHelp: the help (F5), function 26
void ed_go_help(AllRegs *r)
{
    r->d[2] = 26;
    ed_function_call_fn(r);
}

// d0 the function, d1 the parameter; d0 0 (d2 the function), 1 or 2
void ed_zap_in(AllRegs *r)
{
    uint16_t function = 0;
    r->d[0] = uint32_t(zap_in(r->d[0], r->d[1], &function));
    r->d[2] = function;
}

void ed_zap_prepare(AllRegs *r)
{
    zap_prepare();
    r->a[4] = encode_context_cursor(ctx().a4);
    r->a[6] = address(ctx().a6);
}

void ed_zap_out_fn(AllRegs *) { zap_out(); }

// Ed_ZapX: d0 the error (0), a0 its message
void ed_zap_result(AllRegs *r)
{
    IData &d = idata();
    r->d[0] = uint32_t(int32_t(int16_t(d.Ed_ZapError)));
    r->a[0] = d.Ed_ZapMessage;
    d.Ed_ZapError = 0;
}

void ed_zap_function(AllRegs *r)
{
    Answer a = zap_function(r->d[0], r->d[1]);
    r->d[0] = a.value;
    r->d[2] = a.type;
    if (a.type)
        r->a[0] = a.text;
}

void ed_zap_new_config(AllRegs *) { zap_new_config(); }
void ed_zap_new_line(AllRegs *) { zap_new_line(); }
void ed_zap_new_line_tok(AllRegs *)
{
    zap_new_line();
    tokenise_current_line();
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_loop.inc"
#endif
