// The program of a window as a whole (+Edit.s): Ed_New / Ed_New2 /
// Edt_New, the size of its text buffer (Ed_SetBuffer, Ed_SB,
// Ed_GetPlace), saving it when it changed (Ed_Saved), overwriting a file
// (Ed_SaveOver), the file selector (Ed_File_Selector), Ed_OpenLoad.

#include "runtime/static_symbols.h"
#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EcEdit = 9;
constexpr uint16_t EdDSaved = 11, EdDSetBuf = 36, EdDTooSmall = 37, EdDAExist = 47;

void run(host::RoutineArg routine)
{
    Regs r;
    call(routine, r);
}

[[noreturn]] void alert(host::RoutineArg routine)
{
    run(routine);
    __builtin_unreachable();
}

be_u32 *dialogue_variables() { return ptr<be_u32>(idata().Ed_VDialogues); }

// Prg_ChgTTexte: a text buffer of size bytes for the program (0: none);
// false if out of memory
bool text_buffer(uint32_t size)
{
    Regs r;
    r.d[0] = size;
    lib_call(L_Prg_ChgTTexte, r);
    return r.d[0];
}

// Edt_New: the window's editing state for a new program
void window_new()
{
    if (!idata().Ed_Zappeuse) {
        Regs l;
        l.d[1] = 0;
        ec_call(ec::Last, l);
        Regs v;
        sy_call(sy::WaitVbl, v);
        Regs c;
        ec_call(ec::CopForce, c);
    }
    Regs a;
    a.d[1] = EcEdit;
    ec_call(ec::Active, a);
    EditWindow &w = window();
    uint8_t *p = reinterpret_cast<uint8_t *>(&w);
    for (int i = EditWindow::Edt_SInit; i < EditWindow::Edt_EInit; i++)
        p[i] = 0;
    w.Edt_YBloc = 0xFFFF;
    w.Edt_YOldBloc = 0xFFFF;
    run(AMOS_ROUTINE(ed_undo_create_entry));
    run(AMOS_ROUTINE(ed_marks_clear_entry));
    idata().MathFlags = 0;
}

// Ed_New2: the program erased (a program driving the editor keeps the
// banks)
void program_new()
{
    run(AMOS_ROUTINE(ed_menu_program));
    Regs n;
    n.d[0] = idata().Ed_Zappeuse ? 0 : 1;
    lib_call(L_Prg_New, n);
    window_new();
    new_buffer_all();
}

// Ed_SaveOver: Name1 exists already: overwrite it?
void save_over()
{
    IData &d = idata();
    Regs e;
    lib_call(L_RExist, e);
    if (!e.d[3])
        return;
    dialogue_variables()[0] = d.Name1;
    if (int16_t(dialogue(EdDAExist)) != 1)
        alert(AMOS_ROUTINE(ed_not_done));
}

// Ed_Saved: the program saved first if it was changed (or not, if asked)
void saved()
{
    IData &d = idata();
    if (!program().Prg_Change)
        return;
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    for (int i = 0; i < 32; i++)
        buffer[i] = ' ';
    buffer[32] = 0;
    *name_into(buffer, program()) = 0;
    dialogue_variables()[0] = address(dialogue_string(buffer));
    int16_t answer = int16_t(dialogue(EdDSaved));
    if (answer == 2)
        return;
    if (answer != 1)
        alert(AMOS_ROUTINE(ed_not_done2));
    run(AMOS_ROUTINE(ed_save_it_entry));
}

// Ed_New: the program erased, saved first if it was changed
void new_program()
{
    tokenise_current_line();
    saved();
    program_new();
}

// Ed_SB: the size of the text buffer asked for, from size
void set_buffer(uint32_t size)
{
    Program &p = program();
    dialogue_variables()[2] = size;
    if (int16_t(dialogue(EdDSetBuf)) != 1)
        alert(AMOS_ROUTINE(ed_not_done));
    Regs v;
    v.d[0] = 1;
    v.d[1] = 3;
    v.d[2] = 0xFFFFFFFF;
    lib_call(L_Dia_GetValue, v);
    uint32_t wanted = v.d[1] & ~1u;
    uint32_t had = p.Prg_StTTexte;
    if (wanted < 1024 || wanted == had)
        alert(AMOS_ROUTINE(ed_not_done));
    bool kept = false;
    if (wanted > had) {
        // a new buffer, the program copied into it
        uint32_t size_was = p.Prg_StTTexte, mini = p.Prg_StMini, top = p.Prg_StHaut, bottom = p.Prg_StBas;
        p.Prg_StTTexte = 0;
        if (text_buffer(wanted)) {
            store_block(ptr<uint8_t>(bottom), top - bottom, 0);
            free_memory(mini, size_was);
            kept = true;
        } else {
            // (the assembler went on with the old buffer forgotten and the
            // stack unbalanced)
            p.Prg_StTTexte = size_was;
            p.Prg_StMini = mini;
            p.Prg_StHaut = top;
            p.Prg_StBas = bottom;
        }
    }
    if (!kept) {
        // the buffer changed, the program erased
        saved();
        program_new();
        if (!text_buffer(wanted)) {
            if (!text_buffer(had))
                text_buffer(1024);
            new_buffer_all();
            alert(AMOS_ROUTINE(ed_out_of_memory));
        }
    }
    Regs b;
    lib_call(L_Prg_SetBanks, b);
    new_buffer_all();
}

void set_buffer_command()
{
    tokenise_current_line();
    run(AMOS_ROUTINE(ed_clear_var));
    set_buffer(program().Prg_StTTexte);
}

// Ed_GetPlace: a program of size bytes does not fit: a larger buffer?
void get_place(uint32_t size)
{
    Program &p = program();
    if (int16_t(dialogue(EdDTooSmall)) != 1) {
        p.Prg_Change = 0;
        return set_buffer(size);
    }
    uint32_t had = p.Prg_StTTexte;
    if (text_buffer(size))
        return;
    if (!text_buffer(had))
        text_buffer(1024);
    new_buffer_all();
    alert(AMOS_ROUTINE(ed_out_of_memory));
}

// Ed_File_Selector: the file selector with the editor's messages n to n+3
// (title, ...); 0 if cancelled. A program driving the editor gives Name1.
uint16_t file_selector(uint16_t n)
{
    IData &d = idata();
    if (d.Ed_Zappeuse)
        return 1;
    // the parameters as on the interpreter's stack: the last one first,
    // each a string (a length word, the text)
    be_u32 parameters[4];
    for (int i = 0; i < 4; i++)
        parameters[3 - i] = address(message(uint16_t(n + i))) - 2;
    d.Ed_FSel = 1;
    set_banks();
    AllRegs r = {};
    Ctx &c = ctx();
    r.a[3] = address(parameters);
    r.a[4] = encode_context_cursor(c.a4);
    r.a[5] = encode_data_zone(c.a5);
    r.a[6] = encode_context_cursor(c.a6);
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    amos_call_all(&r, lib_entry(L_Dsk_FileSelector));
    d.Ed_FSel = 0;
    return uint16_t(r.d[0]);
}

// Ed_OpenLoad: a new window, a program loaded into it
void open_load()
{
    AllRegs r = {};
    Ctx &c = ctx();
    r.a[4] = encode_context_cursor(c.a4);
    r.a[5] = encode_data_zone(c.a5);
    r.a[6] = encode_context_cursor(c.a6);
    amos_call_all(&r, AMOS_ROUTINE(ed_open_shown_window_entry));
    c.a4 = decode_context_cursor(r.a[4]);
    c.a6 = decode_context_cursor(r.a[6]);
    run(AMOS_ROUTINE(ed_load_entry));
}

} // namespace

extern "C" {
void ed_new_fn(AllRegs *) { new_program(); }
void ed_new2_fn(AllRegs *) { program_new(); }
void ed_window_new(AllRegs *) { window_new(); }
void ed_save_over_fn(AllRegs *) { save_over(); }
void ed_saved_fn(AllRegs *) { saved(); }
void ed_set_buffer(AllRegs *) { set_buffer_command(); }
void ed_set_buffer_from(AllRegs *r) { set_buffer(r->d[0]); }
void ed_get_place_fn(AllRegs *r) { get_place(r->d[0]); }
void ed_file_selector_fn(AllRegs *r) { r->d[0] = (r->d[0] & 0xFFFF0000) | file_selector(uint16_t(r->d[0])); }
void ed_open_load(AllRegs *r)
{
    open_load();
    r->a[4] = encode_context_cursor(ctx().a4);
    r->a[6] = encode_context_cursor(ctx().a6);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_program.inc"
#endif
