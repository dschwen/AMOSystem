// Running and testing programs from the editor (+Edit.s): Ed_Run,
// Ed_RunHidden / Ed_RunHide, Ed_RunDirect, the programs of the menu
// options (Ed_PrgCommand, Ed_PrgReLoad), Ed_Test, Ed_Check1.3,
// Ed_Indent, Ed_VaTester, Ed_GoMonitor, Ed_ProcML; and the returns from a
// program: Ed_ErrRun, Ed_ErrTest, Ed_ErrRunHidden, Ed_ErrEdit, Ed_Ligne.
//
// Prg_RunIt and Prg_TestIt take the address the program comes back to
// (+Edit.s Ed_ErrRun... jump here) with the error in d0 (negative: one of
// the test) and its message in a0 (0: to be found); and a table of three
// routines the tester calls (Ed_TestMessage). They do not come back,
// unless the program could not be started.

#include "editor/editor.h"
#include "core/source.h"

#include <dos/dos.h>

#define DOS_BASE_NAME (amos::ptr<struct DosLibrary>(amos::idata().DosBase))
#include <proto/dos.h>

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_loop[], ed_system[], ed_err_direct[], ed_err_run[], ed_err_run_hidden[], ed_err_test[],
    ed_err_direct_run[], ed_test_message[], ed_direct_test_message[], ed_not_done[], ed_out_of_memory[],
    ed_alert_100[], ed_already_runned[], ed_prg_load_err[], ed_fo_e[], ed_d_error[], ed_recop[], ed_hide[],
    ed_appear[], ed_open_editor[], ed_close_editor[], ed_escape_hide_entry[], ed_redraw_entry[],
    ed_config_redraw_entry[], ed_clear_var[], ed_branch_amos[], ed_menus_end_entry[], ed_menu_program[],
    ed_block_free_entry[], ed_undo_reset_all_entry[], ed_recall_alert_entry[], ed_name_part_entry[],
    ed_detok_entry[], ed_proc_open_entry[], ed_escape_max_mouse_entry[], ed_file_selector[], ed_window_new_entry[],
    ed_new2[], ed_saved[], ed_alert_entry[], ed_delete_window_entry[], ed_av_name[];

namespace {

constexpr uint16_t EcEdit = 9;
constexpr uint32_t EntNul = 0x80000000;
constexpr uint16_t TBuffer = 1024;
constexpr uint16_t EdDLigne = 59;

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

void run_d0(uint8_t *routine, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    call(routine, r);
}

[[noreturn]] void jump(uint8_t *routine)
{
    run(routine);
    __builtin_unreachable();
}

[[noreturn]] void jump_d0(uint8_t *routine, uint32_t d0)
{
    run_d0(routine, d0);
    __builtin_unreachable();
}

// A return routine of a program, with its registers
[[noreturn]] void jump_error(uint8_t *routine, uint32_t d0, uint32_t a0)
{
    Regs r;
    r.d[0] = d0;
    r.a[0] = a0;
    call(routine, r);
    __builtin_unreachable();
}

[[noreturn]] void alert(const uint8_t *text, uint16_t time)
{
    Regs r;
    r.a[0] = address(text);
    r.d[0] = time;
    call(ed_alert_entry, r);
    __builtin_unreachable();
}

void ec_fn(int function, uint32_t d1 = 0, uint32_t d2 = 0, uint32_t d3 = 0, uint32_t d4 = 0, uint32_t d5 = 0)
{
    Regs r;
    r.d[1] = d1;
    r.d[2] = d2;
    r.d[3] = d3;
    r.d[4] = d4;
    r.d[5] = d5;
    ec_call(function, r);
}

void use(EditWindow *w)
{
    ctx().a4 = reinterpret_cast<uint8_t *>(w);
    ctx().a6 = ptr<uint8_t>(w->Edt_Prg);
}

void set_banks_program()
{
    Regs r;
    lib_call(L_Prg_SetBanks, r);
}

// The command line of the program run: "CmdL", its length, the text
uint8_t *command_line() { return ptr<uint8_t>(idata().Buffer) + TBuffer - 256 - 6; }

// Detok: the position in line of the address at
uint16_t position_in(const uint8_t *line, uint32_t at)
{
    Regs r;
    r.a[0] = address(line);
    r.a[1] = idata().Buffer;
    r.d[0] = at;
    call(ed_detok_entry, r);
    return uint16_t(r.d[0]);
}

// Get the program ready for running: room, no undo, the program's menus
void prepare()
{
    run(ed_block_free_entry);
    run(ed_undo_reset_all_entry);
    run(ed_menus_end_entry);
    run(ed_menu_program);
}

// Prg_RunIt: mode 0 a program, 1 an accessory; 0 out of memory, -1
// running already
int32_t run_it(uint8_t *back, uint8_t *messages, uint32_t mode)
{
    Regs r;
    r.d[0] = mode;
    r.a[1] = address(back);
    r.a[2] = address(messages);
    lib_call(L_Prg_RunIt, r);
    return int32_t(r.d[0]);
}

// Ed_Run: the current program
void run_program()
{
    IData &d = idata();
    tokenise_current_line();
    prepare();
    poke_l(command_line(), 0);
    d.Edt_Runned = address(&window());
    run_it(ed_err_run, ed_test_message, 0);
    d.Edt_Runned = 0;
    jump(ed_out_of_memory);
}

// Ed_RunHide: the program of window w, as an accessory
[[noreturn]] void run_hidden_window(EditWindow *w)
{
    IData &d = idata();
    d.Edt_Runned = address(w);
    ctx().a6 = ptr<uint8_t>(w->Edt_Prg);
    prepare();
    d.Ed_RunnedHidden = 1;
    int32_t result = run_it(ed_err_run_hidden, ed_test_message, 1);
    d.Edt_Runned = 0;
    // (the assembler tested the flags of clearing Edt_Runned, and always
    // said "out of memory")
    if (!result)
        jump(ed_out_of_memory);
    jump(ed_already_runned);
}

// Ed_RunHidden: hidden program n
void run_hidden(uint16_t n)
{
    tokenise_current_line();
    EditWindow *w = hidden(n);
    if (!w)
        jump(ed_not_done);
    poke_l(command_line(), 0);
    run_hidden_window(w);
}

// Ed_RunDirect: the current program, from direct mode
void run_direct()
{
    IData &d = idata();
    use(ptr<EditWindow>(d.Edt_Current));
    prepare();
    poke_l(command_line(), 0);
    d.Edt_Runned = d.Edt_Current;
    run_it(ed_err_direct_run, ed_direct_test_message, 0);
    d.Edt_Runned = 0;
    run(ed_escape_hide_entry);
    run(ed_appear);
    jump(ed_out_of_memory);
}

// Edt_AccAdr: the window of the program of Name1 (its name without the
// path, in any case), 0 if none
uint8_t upper(uint8_t c) { return c >= 'a' && c <= 'z' ? uint8_t(c - 0x20) : c; }

const uint8_t *name_part(const uint8_t *name)
{
    Regs r;
    r.a[0] = address(name);
    call(ed_name_part_entry, r);
    return ptr<uint8_t>(r.a[0]);
}

EditWindow *window_of_name1()
{
    IData &d = idata();
    const uint8_t *wanted = name_part(ptr<uint8_t>(d.Name1));
    for (EditWindow *w = ptr<EditWindow>(d.Edt_List); w; w = ptr<EditWindow>(w->Edt_Next)) {
        const uint8_t *a = name_part(ptr<Program>(w->Edt_Prg)->Prg_NamePrg), *b = wanted;
        for (;;) {
            uint8_t x = upper(*a++), y = upper(*b++);
            if (x != y)
                break;
            if (!(x | y))
                return w;
        }
    }
    return nullptr;
}

// Ed_PrgCommand: the program of a menu option (def: its flags, the texts
// of its name and command line in Ed_MnPrograms) run, loaded in a hidden
// window or in the current one
void program_command(const uint8_t *def)
{
    IData &d = idata();
    // the command line: the option's, or the line from the cursor
    const uint8_t *from;
    uint16_t length;
    if (def[2]) {
        from = message_of(ptr<uint8_t>(d.Ed_MnPrograms), def[2], nullptr);
        length = from[-1];
    } else {
        EditWindow &w = window();
        uint8_t *line = ptr<uint8_t>(w.Edt_BufE) + int16_t(uint16_t(w.Edt_YCu << 8)) + 2;
        from = line + w.Edt_XCu;
        length = uint16_t(peek_w(line - 2) - w.Edt_XCu);
    }
    uint8_t *to = command_line();
    poke_l(to, 0x436D644C);                     // "CmdL"
    poke_w(to + 4, length);
    for (uint16_t i = 0; i < length; i++)
        to[6 + i] = from[i];
    tokenise_current_line();
    const uint8_t *name = message_of(ptr<uint8_t>(d.Ed_MnPrograms), def[1], nullptr);
    if (def[0] & 1) {
        // in a hidden window (the one it is in already, if loaded)
        uint8_t *name1 = ptr<uint8_t>(d.Name1);
        while ((*name1++ = *name++))
            ;
        EditWindow *w = window_of_name1();
        if (!w) {
            Regs p;
            lib_call(L_Dsk_PathIt, p);
            load_hidden();
            d.Ed_WindowToDel = 0;
            w = &window();
            if (def[0] & 4) {
                run(ed_branch_amos);
                run(ed_menu_program);
            } else {
                w->Edt_PrgDelete = 0xFF;
            }
        }
        run_hidden_window(w);
    }
    // in the current window
    run(ed_saved);
    uint8_t *back = ed_err_run;
    if (!(def[0] & 4)) {
        // the program kept, to be loaded back after
        EditWindow &w = window();
        Program &p = program();
        uint32_t size = (EditWindow::Edt_EReload - EditWindow::Edt_SReload) + 4 + 4 * 10 + 128;
        uint8_t *k = memory_reserve(&d.Ed_Prg2ReLoad, size);
        if (!k)
            jump(ed_out_of_memory);
        poke_l(k, p.Prg_StTTexte);
        k += 4;
        for (int i = 0; i < 10; i++, k += 4)
            poke_l(k, p.Prg_Marks[i]);
        const uint8_t *s = reinterpret_cast<uint8_t *>(&w) + EditWindow::Edt_SReload;
        for (int i = 0; i < EditWindow::Edt_EReload - EditWindow::Edt_SReload; i++)
            *k++ = s[i];
        for (const uint8_t *n = p.Prg_NamePrg; (*k++ = *n++);)
            ;
        back = ed_err_run_hidden;
        d.Ed_RunnedHidden = 1;
    }
    // new, load, run
    uint8_t *name1 = ptr<uint8_t>(d.Name1);
    while ((*name1++ = *name++))
        ;
    run(ed_new2);
    Regs w;
    w.d[0] = 155;
    w.a[0] = d.Name1;
    call(ed_av_name, w);
    Regs l;
    l.d[0] = 0xFFFFFFFF;                        // the buffer made to fit
    lib_call(L_Prg_Load, l);
    if (l.d[0])
        jump_d0(ed_prg_load_err, l.d[0]);
    warning_end();
    poke_l(ptr<uint8_t>(d.Ed_BufT) + 256, 0);   // no Ed_RAlert
    prepare();
    d.Edt_Runned = address(&window());
    run_it(back, ed_test_message, 0);
    d.Edt_Runned = 0;
    jump(ed_out_of_memory);
}

// Ed_PrgReLoad: the program kept before running a menu option's back
void program_reload()
{
    IData &d = idata();
    EditWindow &w = window();
    Program &p = program();
    run(ed_new2);
    auto free_kept = [&] {
        memory_release(&d.Ed_Prg2ReLoad);
        update_split(&w, 2);
    };
    const uint8_t *k = ptr<uint8_t>(d.Ed_Prg2ReLoad);
    Regs t;
    t.d[0] = peek_l(k);
    lib_call(L_Prg_ChgTTexte, t);
    if (!t.d[0]) {
        Regs s;
        s.d[0] = 1024;
        lib_call(L_Prg_ChgTTexte, s);
        run(ed_window_new_entry);
        free_kept();
        jump(ed_out_of_memory);
    }
    k += 4;
    for (int i = 0; i < 10; i++, k += 4)
        p.Prg_Marks[i] = peek_l(k);
    uint8_t *s = reinterpret_cast<uint8_t *>(&w) + EditWindow::Edt_SReload;
    for (int i = 0; i < EditWindow::Edt_EReload - EditWindow::Edt_SReload; i++)
        s[i] = *k++;
    if (*k) {
        for (uint8_t *n = ptr<uint8_t>(d.Name1); (*n++ = *k++);)
            ;
        Regs a;
        a.d[0] = 155;
        a.a[0] = d.Name1;
            call(ed_av_name, a);
        Regs l;
        l.d[0] = 1;                             // back if too small
        lib_call(L_Prg_Load, l);
        if (!l.d[0]) {
            p.Prg_StModif = 1;
            warning_end();
            new_buffer_all();
        }
    }
    warning_end();
    free_kept();
    run(ed_recall_alert_entry);
    jump(ed_loop);
}

// Ed_VaTester: the program tested if it changed (an error goes to
// Ed_ErrTest)
void test_program()
{
    IData &d = idata();
    if (!program().Prg_StModif)
        return;
    d.Ed_YaUTest = 1;
    Regs r;
    r.a[1] = address(ed_err_test);
    r.a[2] = address(ed_test_message);
    lib_call(L_Prg_TestIt, r);
    if (r.d[0])
        jump(ed_already_runned);
}

// Ed_Test: tested; a warning if single and double precision constants
// are mixed
void test()
{
    IData &d = idata();
    tokenise_current_line();
    test_program();
    uint8_t mixed = d.MathFlags & 0x80 ? d.Ver_SPConst : d.Ver_DPConst;
    jump_d0(ed_alert_100, mixed ? 221 : 197);
}

// Ed_Check1.3
void check_13()
{
    IData &d = idata();
    tokenise_current_line();
    d.VerCheck1_3 = 1;
    program().Prg_StModif = 1;
    test_program();
    alert(message_of(ptr<uint8_t>(d.Ed_TstMessages), d.VerNot1_3 ? 48 : 49, nullptr), 127);
}

// Ed_Indent
void indent_program()
{
    tokenise_current_line();
    test_program();
    indent(ptr<uint8_t>(program().Prg_StBas), idata().Ed_Tabs);
    new_buffer();
}

// Ed_Test1 / Ed_Test2 / Ed_Test3: called by the tester: a message for a
// long program, taken away, the editor away before running
void test_shown()
{
    IData &d = idata();
    Program &p = program();
    d.Ed_TstMesOn = 0;
    if (p.Prg_StHaut - p.Prg_StBas < 4096)
        return;
    d.Ed_TstMesOn = 1;
    warn_message(198);
    set_banks_program();
}

void test_end()
{
    IData &d = idata();
    if (!d.Ed_TstMesOn)
        return;
    d.Ed_TstMesOn = 0;
    warning_end();
    set_banks_program();
}

void test_run()
{
    run(ed_menus_end_entry);
    run(ed_hide);
}

// Ed_GoMonitor: the program run in the monitor
void go_monitor()
{
    IData &d = idata();
    tokenise_current_line();
    run(ed_clear_var);
    run(ed_hide);
    run(ed_close_editor);
    set_banks_program();
    Regs m;
    lib_call(L_Mon_Load, m);
    uint16_t message = 204;
    if (uint16_t(m.d[0])) {
        if (int16_t(m.d[0]) != -1)
            message = 222;
    } else {
        d.Edt_Runned = address(&window());
        d.Prg_JError = address(ed_err_run);
        Regs i;
        lib_call(L_Mon_In_Editor, i);
        d.Edt_Runned = 0;
    }
    run(ed_open_editor);
    run(ed_appear);
    jump_d0(ed_alert_100, message);
}

// Ed_Ligne: the error's line in the window below the program, Editor or
// Direct mode: 1 for the direct mode
uint16_t error_line(uint32_t code, const uint8_t *message)
{
    IData &d = idata();
    Error e = get_error(int16_t(code), message);
    run(ed_open_editor);
    uint32_t *v = ptr<uint32_t>(d.Ed_VDialogues);
    v[0] = address(dialogue_string(e.message));
    source::Number n = source::find_address(ptr<uint8_t>(program().Prg_StBas), ptr<uint8_t>(d.VerPos));
    v[1] = n.number + 1;
    uint16_t x = position_in(n.line, d.VerPos);
    uint8_t *text = ptr<uint8_t>(d.Buffer) + 2;
    text[peek_w(text - 2)] = 0;
    text[x + 60] = 0;
    int16_t start = int16_t(x + 60 - 73);
    if (start < 0)
        start = 0;
    v[3] = address(dialogue_string(text + x));
    text[x] = 0;
    v[2] = address(dialogue_string(text + start));
    v[4] = address(ed_recop);
    // the window
    ec_fn(ec::EHide, EcEdit, 0);
    ec_fn(ec::First, EcEdit);
    ec_fn(ec::AView, EcEdit, d.Ed_Wx, d.Es_Y1, EntNul, 56);
    ec_fn(ec::OffSet, EcEdit, 0, 0);
    Regs s;
    s.d[1] = 0xFFFFFFFF;
    sy_call(sy::Show, s);
    run(ed_escape_max_mouse_entry);
    Regs m;
    m.d[1] = uint16_t(d.Ed_Wx + 1);
    m.d[2] = uint16_t(d.Es_Y1 + 1);
    sy_call(sy::SetM, m);
    uint16_t answer = uint16_t(run_dialogue(EdDLigne));
    ec_fn(ec::EHide, EcEdit, 0xFFFFFFFF);
    run(ed_recop);
    return answer;
}

// Ed_ErrEdit: back to the editor, the cursor on the error
[[noreturn]] void error_edit(uint32_t code, const uint8_t *message)
{
    IData &d = idata();
    bool test = int16_t(code) < 0;
    Error e = get_error(int16_t(code), message);
    run(ed_open_editor);
    run(ed_escape_hide_entry);
    run(ed_appear);
    if (!test && (e.code == 10 || e.code == 1000))
        jump(ed_loop);
    Program &p = program();
    source::Number n = source::find_address(ptr<uint8_t>(p.Prg_StBas), ptr<uint8_t>(d.VerPos));
    const uint8_t *line = n.line;
    p.Prg_AdEProc = 0;
    if (n.procedure) {
        // in a closed procedure: kept for showing it
        p.Prg_AdEProc = address(n.line);
        p.Prg_XEProc = position_in(n.line, d.VerPos);
        d.VerPos = 0;
        line = n.procedure;
    }
    set_y(uint16_t(n.number));
    set_x(position_in(line, d.VerPos));
    new_buffer_all();
    // the message, with a "."
    uint8_t *to = ptr<uint8_t>(d.Ed_BufT);
    for (const uint8_t *m = e.message; (*to++ = *m++);)
        ;
    to[-1] = '.';
    to[0] = 0;
    alert(ptr<uint8_t>(d.Ed_BufT), 200);
}

// Ed_Errr: an error of a program run or tested from the editor
[[noreturn]] void error_dispatch(uint32_t code, uint32_t message)
{
    if (int32_t(code) < 0 || uint16_t(code) == 1000)
        error_edit(code, ptr<uint8_t>(message));
    if (uint16_t(code) == 1001)
        jump_error(ed_err_direct, code, message);
    if (uint16_t(code) == 1002)
        jump(ed_system);
    if (error_line(code, ptr<uint8_t>(message)) == 1)
        jump_error(ed_err_direct, code, message);
    error_edit(code, ptr<uint8_t>(message));
}

// Ed_ErrRunHidden: back from an accessory (or a menu option's program)
[[noreturn]] void error_run_hidden(uint32_t code, const uint8_t *message)
{
    IData &d = idata();
    use(ptr<EditWindow>(d.Edt_Runned));
    d.Edt_Runned = 0;
    d.Ed_RunnedHidden = 0;
    bool test = int16_t(code) < 0;
    Error e = get_error(int16_t(code), message);
    if (d.EdC_Modified) {
        run(ed_config_redraw_entry);
    } else if (d.Prg_Accessory) {
        run(ed_redraw_entry);
    } else {
        run(ed_open_editor);
        run(ed_escape_hide_entry);
        run(ed_appear);
    }
    run(ed_clear_var);
    if (window().Edt_PrgDelete) {
        run(ed_delete_window_entry);
        run(ed_branch_amos);
    }
    use(ptr<EditWindow>(d.Edt_Current));
    set_banks_program();
    Regs b;
    lib_call(L_Bnk_Change, b);
    set_banks();
    if (!test) {
        if (e.code == 10 || e.code == 1000)
            jump(ed_loop);
        if (e.code == 1002)
            jump(ed_system);
    }
    // "Error in hidden program: ..."
    uint8_t *to = ptr<uint8_t>(d.Ed_BufT);
    for (const uint8_t *m = message_of(ptr<uint8_t>(d.Ed_Messages), 9, nullptr); (*to++ = *m++);)
        ;
    to--;
    for (const uint8_t *m = e.message; (*to++ = *m++);)
        ;
    to[-1] = '.';
    to[0] = 0;
    alert(ptr<uint8_t>(d.Ed_BufT), 200);
}

// Ed_ErrRun: back from a program run
[[noreturn]] void error_run(uint32_t code, uint32_t message)
{
    IData &d = idata();
    if (d.Ed_RunnedHidden)
        error_run_hidden(code, ptr<uint8_t>(message));
    use(ptr<EditWindow>(d.Edt_Runned));
    d.Edt_Runned = 0;
    Program &p = program();
    if (p.Prg_Reloaded) {
        p.Prg_Reloaded = 0;
        window().Edt_YBloc = 0xFFFF;
    }
    error_dispatch(code, message);
}

// Ed_ErrTest: an error found by the tester
[[noreturn]] void error_test(uint32_t code, uint32_t message)
{
    use(ptr<EditWindow>(idata().Edt_Current));
    error_dispatch(code, message);
}

// Ed_DErrRun: back from a program run in direct mode
[[noreturn]] void error_direct_run(uint32_t code, uint32_t message)
{
    if (idata().Direct)
        jump_error(ed_err_direct, code, message);
    error_run(code, message);
}

// Ed_ProcML: the machine code of an executable file as the procedure at
// the cursor
void procedure_machine_code()
{
    IData &d = idata();
    EditWindow &w = window();
    tokenise_current_line();
    Regs f;
    f.d[0] = 178;
    if (!uint16_t(call(ed_file_selector, f)))
        jump(ed_not_done);
    Regs pi;
    lib_call(L_Dsk_PathIt, pi);
    uint8_t *procedure;
    for (;;) {
        find_line(uint16_t(w.Edt_YPos + w.Edt_YCu));
        procedure = ptr<uint8_t>(w.Edt_DebProc);
        if (!procedure)
            jump(ed_fo_e);
        if (procedure[10] & 0x80)
            break;
        run(ed_proc_open_entry);
    }
    // the code hunk of the file
    BPTR file = Open(ptr<char>(d.Name1), MODE_OLDFILE);
    d.Handle = uint32_t(file);
    if (!file)
        jump(ed_d_error);
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    auto get = [&]() -> uint32_t {
        if (Read(file, buffer, 4) != 4)
            jump(ed_d_error);
        return peek_l(buffer);
    };
    if (get() != 0x3F3)
        alert(message(182), 250);
    while (get() != 0x3E9)
        ;
    uint32_t size = get() << 2;
    Regs t;
    t.d[0] = size + 512;
    lib_call(L_ResTempBuffer, t);
    uint8_t *proc = ptr<uint8_t>(t.a[0]);
    // the procedure's line, marked machine code, then the code
    uint8_t *a1 = proc;
    for (uint16_t i = 0; i < uint16_t(procedure[0] * 2); i++)
        *a1++ = procedure[i];
    poke_w(proc + 10, uint16_t(peek_w(proc + 10) | 0x5000));
    poke_w(a1, 0x0301);
    poke_w(a1 + 2, tk::ML);
    a1 += 4;
    const uint8_t *parameters = proc + 10 + 6;
    parameters += 4 + parameters[0];
    int32_t offset = int32_t(parameters - a1);
    if (peek_w(parameters - 2) != tk::Bra1)
        offset = 0;
    poke_w(a1, uint16_t(offset));
    a1 += 2;
    if (Read(file, a1, int32_t(size)) != int32_t(size))
        jump(ed_d_error);
    d.Handle = 0;
    Close(file);
    a1 += size;
    poke_w(a1, 0x0301);
    poke_w(a1 + 2, tk::EndP);
    poke_w(a1 + 4, 0);
    a1 += 6;
    poke_l(proc + 4, uint32_t(a1 - proc - 14));
    uint32_t length = uint32_t(a1 - proc);
    // in place of the old procedure
    uint16_t y = uint16_t(w.Edt_YCu + w.Edt_YPos);
    uint8_t *from = find_line(y);
    delete_chunk(from, next_line());
    store_block(proc, length, y);
    Regs c;
    c.d[0] = 0;
    lib_call(L_ResTempBuffer, c);
    Regs n;
    lib_call(L_Prg_CptLines, n);
    warning_end();
}

} // namespace

extern "C" {
void ed_run(AllRegs *) { run_program(); }
void ed_run_hidden(AllRegs *r) { run_hidden(uint16_t(r->d[1])); }
void ed_run_hide(AllRegs *r) { run_hidden_window(ptr<EditWindow>(r->a[0])); }
void ed_run_direct(AllRegs *) { run_direct(); }
void ed_program_command(AllRegs *r) { program_command(ptr<uint8_t>(r->a[0])); }
void ed_program_reload(AllRegs *) { program_reload(); }
void ed_window_of_name(AllRegs *r)
{
    EditWindow *w = window_of_name1();
    r->d[0] = r->a[0] = address(w);
}
void ed_test(AllRegs *) { test(); }
void ed_check_13(AllRegs *) { check_13(); }
void ed_indent_program(AllRegs *) { indent_program(); }
void ed_test_program_fn(AllRegs *) { test_program(); }
void ed_test_shown(AllRegs *) { test_shown(); }
void ed_test_end(AllRegs *) { test_end(); }
void ed_test_run(AllRegs *) { test_run(); }
void ed_go_monitor(AllRegs *) { go_monitor(); }
void ed_error_line(AllRegs *r) { r->d[1] = (r->d[1] & 0xFFFF0000) | error_line(r->d[0], ptr<uint8_t>(r->a[0])); }
void ed_error_edit(AllRegs *r) { error_edit(r->d[0], ptr<uint8_t>(r->a[0])); }
void ed_error_run(AllRegs *r) { error_run(r->d[0], r->a[0]); }
void ed_error_test(AllRegs *r) { error_test(r->d[0], r->a[0]); }
void ed_error_run_hidden(AllRegs *r) { error_run_hidden(r->d[0], ptr<uint8_t>(r->a[0])); }
void ed_error_direct_run(AllRegs *r) { error_direct_run(r->d[0], r->a[0]); }
void ed_procedure_machine_code(AllRegs *) { procedure_machine_code(); }
}
