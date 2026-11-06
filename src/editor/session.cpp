// Quitting the editor, and the session kept for the next start (+Edit.s
// Ed_Quit, Ed_DoQuit, Ed_WarmStart).
//
// The session file (system file 47): "ApLC", the length of the rest, the
// program structures, then the window structures, each as its address,
// its length, a long for the new address, then the structure itself; a
// zero long ends each list. The windows' links to programs and other
// windows are old addresses, made new ones from the lists. The programs
// are saved first (a program without a name as New_Project_<n>.AMOS,
// loaded back and deleted).

#include "runtime/static_symbols.h"
#include "editor/editor.h"

#include <dos/dos.h>

#include <dos/dos.h>

#include "hal/files.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint32_t QuitHead = 0x41704C43;       // "ApLC"
constexpr uint16_t EdDQuit = 2, EdDNoWarm = 51, EdDWarmErr = 52, EdDWarmDel = 53;

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

// a4/a6 of the assembler: the window and program worked on
void use(EditWindow *w, Program *p)
{
    ctx().a4 = reinterpret_cast<uint8_t *>(w);
    ctx().a6 = reinterpret_cast<uint8_t *>(p);
}

uint8_t *system_text(uint16_t n)
{
    Regs m;
    m.d[0] = n;
    call(AMOS_ROUTINE(ed_get_systeme), m);
    return ptr<uint8_t>(m.a[0]);
}

// Sys_AddPath: system file n into Name1
void system_file(uint16_t n)
{
    Regs p;
    p.a[0] = address(system_text(n));
    boot::sys_add_path(ptr<char>(p.a[0]));
}

// EdCocop: from copied to, the end past its zero
uint8_t *copy(uint8_t *to, const uint8_t *from)
{
    while ((*to++ = *from++))
        ;
    return to;
}

bool open(int32_t mode)
{
    BPTR f = hal::open(ptr<char>(idata().Name1), mode);
    idata().Handle = uint32_t(f);
    return f;
}

bool read(void *to, int32_t length) { return hal::read(BPTR(idata().Handle), to, length) == length; }

bool write(const void *from, int32_t length)
{
    return hal::write(BPTR(idata().Handle), const_cast<void *>(from), length) == length;
}

void close()
{
    IData &d = idata();
    if (BPTR f = BPTR(d.Handle)) {
        d.Handle = 0;
        hal::close(f);
    }
}

void temp_buffer(uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[0] = boot::temp_buffer(r.d[0]);
}

// .StructureSave: a structure with its address first (0: the end)
bool save_structure(uint8_t *header, const void *s, uint32_t length)
{
    poke_l(header, address(s));
    if (!s)
        return write(header, 4);
    poke_l(header + 4, length);
    poke_l(header + 8, 0);
    return write(header, 12) && write(s, int32_t(length));
}

// .SavAll: the programs, then the session; false on an error
bool save_session()
{
    IData &d = idata();
    warn_message(149);
    uint32_t n = 1;
    for (EditWindow *w = ptr<EditWindow>(d.Edt_List); w; w = ptr<EditWindow>(w->Edt_Next), n++) {
        Program *p = ptr<Program>(w->Edt_Prg);
        use(w, p);
        if (w->Edt_LinkPrev)
            continue;
        uint8_t *name = ptr<uint8_t>(d.Name1);
        if (p->Prg_NamePrg[0]) {
            if (!p->Prg_Change)
                continue;
            p->Prg_NoNamed = 0;
            copy(name, p->Prg_NamePrg);
        } else {
            // New_Project_<n>.AMOS
            p->Prg_NoNamed++;
            uint8_t *end = copy(name, system_text(49)) - 1;
            Regs r;
            r.d[0] = n;
            r.a[0] = address(end);
            lib_call(L_LongToDec, r);
            copy(ptr<uint8_t>(r.a[0]), system_text(22));
        }
        path_it();
        Regs s;
        lib_call(L_Prg_Save, s);
        if (s.d[0])
            return false;
    }
    system_file(47);
    if (!open(MODE_NEWFILE))
        return false;
    uint8_t *header = ptr<uint8_t>(d.Buffer);
    poke_l(header, 0);
    poke_l(header + 4, 0);
    if (!write(header, 8))
        return false;
    for (Program *p = ptr<Program>(d.Prg_List); p; p = ptr<Program>(p->Prg_Next))
        if (!save_structure(header, p, sizeof(Program)))
            return false;
    if (!save_structure(header, nullptr, 0))
        return false;
    for (EditWindow *w = ptr<EditWindow>(d.Edt_List); w; w = ptr<EditWindow>(w->Edt_Next)) {
        // the current window has an order, the others none
        w->Edt_Order = d.Edt_Current == address(w) ? 1 : 0;
        if (!save_structure(header, w, sizeof(EditWindow)))
            return false;
    }
    if (!save_structure(header, nullptr, 0))
        return false;
    int32_t length = hal::seek(BPTR(d.Handle), 0, OFFSET_BEGINNING);
    poke_l(header, QuitHead);
    poke_l(header + 4, uint32_t(length - 8));
    if (!write(header, 8))
        return false;
    close();
    all_warnings_end();
    return true;
}

// Ed_DoQuit: what is to be saved, then the end
void do_quit()
{
    IData &d = idata();
    tokenise_current_line();
    run(AMOS_ROUTINE(ed_clear_var));
    sound_play('B');
    if (d.Ed_QuitFlags & 2 && d.EdC_Changed)
        run(AMOS_ROUTINE(ed_config_save_default_quiet_entry));
    if (d.Ed_QuitFlags & 4 && d.EdMa_Changed)
        run(AMOS_ROUTINE(ed_macro_save_default_entry));
    if (!(d.Ed_QuitFlags & 8)) {
        // the programs not saved, each one asked about
        for (EditWindow *w = ptr<EditWindow>(d.Edt_List); w; w = ptr<EditWindow>(w->Edt_Next)) {
            use(w, ptr<Program>(w->Edt_Prg));
            run(AMOS_ROUTINE(ed_saved));
        }
        jump(AMOS_ROUTINE(ed_system));
    }
    if (save_session())
        jump(AMOS_ROUTINE(ed_system));
    close();
    all_warnings_end();
    dialogue(EdDNoWarm);
    draw_windows();
}

// Ed_Quit
void quit()
{
    tokenise_current_line();
    if (idata().Ed_QuitFlags & 1 && int16_t(dialogue(EdDQuit)) != 1)
        jump(AMOS_ROUTINE(ed_loop));
    do_quit();
}

// .Linke: an old address in *at made the new one, from the list of
// structures at list
void relink(be_u32 *at, const uint8_t *list)
{
    uint32_t old = *at;
    if (!old)
        return;
    while (peek_l(list) != old) {
        list += 12 + int16_t(peek_w(list + 6));
        if (!peek_l(list))
            break;
    }
    *at = peek_l(list + 8);
}

void warm_error();

// .DelFichier: the session file deleted (or asked again; the end if not)
void delete_file()
{
    close();
    system_file(47);
    while (!hal::delete_file(ptr<char>(idata().Name1))) {
        if (int16_t(dialogue(EdDWarmDel)) != 1) {
            warm_error();
            jump(AMOS_ROUTINE(ed_system));
        }
        close();
        system_file(47);
    }
}

// .Err0: the editor with one empty window again
void restart_empty()
{
    IData &d = idata();
    close();
    temp_buffer(0);
    d.Edt_Current = 0;
    ctx().a4 = nullptr;
    open_window(true, int32_t(d.PI_DefSize));
    run(AMOS_ROUTINE(ed_branch_amos));
    draw_windows();
    dialogue(EdDWarmErr);
}

// .Err: the programs and windows made so far freed
void warm_error()
{
    IData &d = idata();
    for (uint32_t next = d.Prg_List; next;) {
        Program *p = ptr<Program>(next);
        next = p->Prg_Next;
        ctx().a6 = reinterpret_cast<uint8_t *>(p);
        Regs t;
        t.d[0] = 0;
        lib_call(L_Prg_ChgTTexte, t);
        d.Cur_Banks = p->Prg_Banks;
        bank_erase_all();
        free_memory(address(p), sizeof(Program));
    }
    d.Prg_List = 0;
    for (uint32_t next = d.Edt_List; next;) {
        EditWindow *w = ptr<EditWindow>(next);
        next = w->Edt_Next;
        free_memory(address(w), sizeof(EditWindow));
    }
    d.Edt_List = 0;
    delete_file();
    restart_empty();
}

// Ed_WarmStart: the session of the last time, if kept
void warm_start()
{
    IData &d = idata();
    system_file(47);
    Regs e;
    lib_call(L_RExist, e);
    if (!e.d[3])
        return;
    warn_message(159);
    // the first window, the system's, away
    if (EditWindow *first = ptr<EditWindow>(d.Edt_List)) {
        ctx().a4 = reinterpret_cast<uint8_t *>(first);
        delete_window();
    }
    // the file into the temporary buffer
    if (!open(MODE_OLDFILE)) {
        delete_file();
        return restart_empty();
    }
    uint8_t *header = ptr<uint8_t>(d.Buffer);
    if (!read(header, 8) || peek_l(header) != QuitHead)
        return restart_empty();
    uint32_t size = peek_l(header + 4);
    temp_buffer(size);
    if (!d.TempBuffer || !read(ptr<uint8_t>(d.TempBuffer), int32_t(size)))
        return restart_empty();
    close();
    uint8_t *programs = ptr<uint8_t>(d.TempBuffer);
    uint8_t *windows = programs;
    while (peek_l(windows))
        windows += 12 + peek_l(windows + 4);
    windows += 4;
    // the programs
    be_u32 *link = &d.Prg_List;
    for (uint8_t *s = programs; peek_l(s); s += 12 + peek_l(s + 4)) {
        Program *p = ptr<Program>(memory(sizeof(Program), true));
        if (!p)
            return warm_error();
        *link = address(p);
        link = &p->Prg_Next;
        poke_l(s + 8, address(p));
        uint8_t *to = reinterpret_cast<uint8_t *>(p);
        for (uint32_t i = 0; i < sizeof(Program); i++)
            to[i] = s[12 + i];
        // (the assembler kept the old link until the next one was made:
        // freeing the list after running out of memory followed it)
        p->Prg_Next = 0;
        p->Prg_StBas = p->Prg_StTTexte;
        p->Prg_StTTexte = 0;
        p->Prg_Dialogs = 0;
        p->Prg_Banks = 0;
        p->Prg_Undo = 0;
        p->Prg_AdEProc = 0;
        p->Prg_ZapData = 0;
        p->Prg_RunData = 0;
        p->Prg_Previous = 0;
    }
    // the windows
    link = &d.Edt_List;
    for (uint8_t *s = windows; peek_l(s); s += 12 + peek_l(s + 4)) {
        EditWindow *w = ptr<EditWindow>(memory(sizeof(EditWindow), true));
        if (!w)
            return warm_error();
        *link = address(w);
        link = &w->Edt_Next;
        poke_l(s + 8, address(w));
        uint8_t *to = reinterpret_cast<uint8_t *>(w);
        for (uint32_t i = 0; i < sizeof(EditWindow); i++)
            to[i] = s[12 + i];
        w->Edt_Next = 0;
    }
    for (EditWindow *w = ptr<EditWindow>(d.Edt_List); w; w = ptr<EditWindow>(w->Edt_Next)) {
        relink(&w->Edt_Prg, programs);
        relink(&w->Edt_LinkPrev, windows);
        relink(&w->Edt_LinkNext, windows);
        relink(&w->Edt_LinkScroll, windows);
        if (w->Edt_Order)
            d.Edt_Current = address(w);
    }
    temp_buffer(0);
    // the programs loaded again
    for (Program *p = ptr<Program>(d.Prg_List); p; p = ptr<Program>(p->Prg_Next)) {
        ctx().a6 = reinterpret_cast<uint8_t *>(p);
        copy(ptr<uint8_t>(d.Name1), p->Prg_NamePrg);
        Regs t;
        t.d[0] = p->Prg_StBas;
        lib_call(L_Prg_ChgTTexte, t);
        if (!t.d[0])
            return warm_error();
        Regs l;
        l.d[0] = 1;                 // loaded as it is
        lib_call(L_Prg_Load, l);
        if (l.d[0])
            return warm_error();
        if (p->Prg_NoNamed) {
            // a New_Project_<n>: no name again, and the file deleted
            p->Prg_NamePrg[0] = 0;
            p->Prg_NoNamed = 0;
            p->Prg_Change = 1;
            hal::delete_file(ptr<char>(d.Name1));
        }
    }
    delete_file();
    run(AMOS_ROUTINE(ed_undo_create_all_entry));
    run(AMOS_ROUTINE(ed_branch_amos));
    // (the assembler went on with a4 on a window record of the freed
    // buffer)
    EditWindow *current = ptr<EditWindow>(d.Edt_Current);
    use(current, current ? ptr<Program>(current->Edt_Prg) : nullptr);
    draw_windows();
}

} // namespace

extern "C" {
void ed_quit(AllRegs *) { quit(); }
void ed_do_quit_fn(AllRegs *) { do_quit(); }
void ed_warm_start(AllRegs *r)
{
    warm_start();
    r->a[4] = address(ctx().a4);
    r->a[6] = address(ctx().a6);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_session.inc"
#endif
