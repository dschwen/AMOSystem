// Loading and saving in the editor (+Edit.s): programs loaded, loaded into
// a hidden window, merged, ASCII files merged; programs saved (with a .bak
// of the old file and an icon if set so). Name1 holds the file's name
// after the file selector.

#include "editor/editor.h"

#include <dos/dos.h>

#define DOS_BASE_NAME (amos::ptr<struct DosLibrary>(amos::idata().DosBase))
#include <proto/dos.h>
#define ICON_BASE_NAME (amos::ptr<struct Library>(amos::idata().IconBase))
#include <proto/icon.h>

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_file_selector[], ed_not_done[], ed_av_name[], ed_aver_fin[], ed_prg_load_err[], ed_get_place[],
    ed_saved[], ed_new2[], ed_clear_var[], ed_out_of_memory[], ed_d_error[], ed_l_to_long[], ed_oof_buf[],
    ed_branch_amos[], ed_get_systeme[], ed_block_free_entry[];

namespace {

constexpr uint16_t BitControl = 1 << 8;
constexpr uint8_t EtANom = 1 << 4;

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

uint8_t *name1() { return ptr<uint8_t>(idata().Name1); }

// Ed_File_Selector: false if cancelled; then L_Dsk.PathIt on the name
bool select_file(uint16_t message)
{
    Regs r;
    r.d[0] = message;
    call(ed_file_selector, r);
    if (!uint16_t(r.d[0]))
        return false;
    Regs p;
    lib_call(L_Dsk_PathIt, p);
    return true;
}

void select_or_not_done(uint16_t message)
{
    if (!select_file(message))
        run(ed_not_done);
}

// Ed_AvName: message n followed by a name, as a warning
void warn_name(uint16_t message, const uint8_t *name)
{
    Regs r;
    r.d[0] = message;
    r.a[0] = address(name);
    call(ed_av_name, r);
}

void count_lines()
{
    Regs r;
    lib_call(L_Prg_CptLines, r);
}

// L_Prg_Load: d0 the mode; d0 0, an error (negative), or a size needed
// (positive, in d1)
Regs load(int32_t mode)
{
    Regs r;
    r.d[0] = uint32_t(mode);
    lib_call(L_Prg_Load, r);
    return r;
}

void copy_name(uint8_t *to, const uint8_t *from)
{
    while ((*to++ = *from++))
        ;
}

} // namespace

namespace amos::ed {

// Ed_RLoadHidden: the program of Name1 loaded into a new hidden window,
// which becomes a4/a6 (Ed_WindowToDel deletes it after a merge)
void load_hidden()
{
    warn_name(155, name1());
    if (open_window(false, 0))
        run(ed_out_of_memory);
    idata().Ed_WindowToDel = address(&window());
    Regs r = load(-1);
    if (r.d[0])
        run_d0(ed_prg_load_err, r.d[0]);
    program().Prg_StModif = 1;
    run(ed_aver_fin);
}

} // namespace amos::ed

namespace {

// Ed_ReLoad: the program of Name1 loaded into the current window, the
// buffer made larger if asked
void reload()
{
    for (;;) {
        warn_name(155, name1());
        Regs r = load(1);
        if (!r.d[0])
            break;
        if (int32_t(r.d[0]) < 0)
            run_d0(ed_prg_load_err, r.d[0]);
        run_d0(ed_get_place, r.d[1]);
        run(ed_aver_fin);
    }
    program().Prg_StModif = 1;
    run(ed_aver_fin);
    new_buffer_all();
}

// Ed_Merge: a program merged at the cursor
void merge()
{
    IData &d = idata();
    tokenise_current_line();
    select_or_not_done(78);
    run(ed_clear_var);
    run(ed_block_free_entry);
    undo_reset();
    load_hidden();
    Program &hidden = program();
    uint8_t *from = ptr<uint8_t>(hidden.Prg_StBas);
    int32_t length = int32_t(hidden.Prg_StHaut - hidden.Prg_StBas - 2);
    if (length > 0) {
        uint16_t lines = hidden.Prg_NLigne;
        EditWindow *c = ptr<EditWindow>(d.Edt_Current);
        ctx().a4 = reinterpret_cast<uint8_t *>(c);
        ctx().a6 = ptr<uint8_t>(c->Edt_Prg);
        uint16_t at = c->Edt_YPos + c->Edt_YCu;
        if (store_block(from, uint32_t(length), at).error)
            run(ed_oof_buf);
        marks_change(at, int16_t(lines));
    }
    // (the hidden window is deleted on the way back to the loop)
    count_lines();
    new_buffer_all();
}

// Ed_LoadA: an ASCII file merged at the cursor, line by line
void merge_ascii()
{
    IData &d = idata();
    tokenise_current_line();
    undo_reset();
    select_or_not_done(86);
    warn_name(158, name1());
    BPTR file = Open(reinterpret_cast<CONST_STRPTR>(name1()), MODE_OLDFILE);
    d.Handle = uint32_t(file);
    if (!file)
        run(ed_d_error);
    Seek(file, 0, OFFSET_END);
    int32_t length = Seek(file, 0, OFFSET_BEGINNING);
    Regs t;
    t.d[0] = uint32_t(length + 4);
    lib_call(L_ResTempBuffer, t);
    if (!t.a[0])
        run(ed_out_of_memory);
    if (Read(file, ptr<void>(t.a[0]), length) != length)
        run(ed_d_error);
    d.Handle = 0;
    Close(file);
    EditWindow &w = window();
    uint16_t at = w.Edt_YPos + w.Edt_YCu;
    uint8_t *a3 = ptr<uint8_t>(d.TempBuffer);
    uint8_t *buffer = ptr<uint8_t>(d.Ed_BufT);
    // (on an error, the temporary buffer and the screen first)
    auto finish = [&] {
        Regs f;
        f.d[0] = 0;
        lib_call(L_ResTempBuffer, f);
        run(ed_aver_fin);
        count_lines();
        new_buffer_all();
        Regs k;
        sy_call(sy::ClearKey, k);
    };
    for (;;) {
        uint16_t &actualise = wdata().Actualise;
        if (actualise & BitControl) {
            actualise &= ~BitControl;
            break;
        }
        // the end of the line; tabs as spaces, other control codes refused
        uint8_t *a2 = a3;
        for (;; a2++) {
            uint8_t c = *a2;
            if (!c)
                break;
            if (c >= 32)
                continue;
            if (c == 10 || c == 13) {
                *a2 = 0;
                if (a2[1] < 32)
                    a2++;
                break;
            }
            if (c != 9) {
                finish();
                run(ed_l_to_long);
            }
            *a2 = ' ';
        }
        if (a2 - a3 >= 250) {
            finish();
            run(ed_l_to_long);
        }
        uint8_t *line = a3;
        a3 = a2 + 1;
        if (tokenise_line(line, buffer) < 0) {
            finish();
            run(ed_l_to_long);
        }
        if (store(buffer, at, true).error) {
            finish();
            run(ed_oof_buf);
        }
        at++;
        program().Prg_NLigne++;
        show_slider();
        if (!*a3)
            break;
    }
    finish();
}

// Ed_MakeBak: Name1 renamed to its .bak (Name2); false on an error
bool make_bak()
{
    IData &d = idata();
    uint8_t *name2 = ptr<uint8_t>(d.Name2);
    copy_name(name2, name1());
    uint8_t *a0 = name2;
    while (*a0)
        a0++;
    uint8_t *dot = a0;
    for (uint8_t *p = a0 + 1; p > name2;)
        if (*--p == '.') {
            dot = p;
            break;
        }
    Regs r;
    r.d[0] = 21;
    call(ed_get_systeme, r);
    copy_name(dot, ptr<uint8_t>(r.a[0]));
    for (;;) {
        if (Rename(reinterpret_cast<CONST_STRPTR>(name1()), reinterpret_cast<CONST_STRPTR>(name2)))
            return true;
        int32_t error = IoErr();
        if (error == ERROR_RENAME_ACROSS_DEVICES || error == ERROR_OBJECT_NOT_FOUND)
            return true;
        if (error != ERROR_OBJECT_EXISTS)
            return false;
        if (!DeleteFile(reinterpret_cast<CONST_STRPTR>(name2)))
            return false;
    }
}

// Ed_SaveIcon
void save_icon()
{
    IData &d = idata();
    if (d.PI_Icons && d.IconBase && d.AdrIcon)
        PutDiskObject(reinterpret_cast<CONST_STRPTR>(name1()), ptr<struct DiskObject>(d.AdrIcon));
}

// Ed_SavePrg2: the program saved as Name1; false on an error
bool save_program2()
{
    run(ed_clear_var);
    count_lines();
    Regs r;
    lib_call(L_Prg_Save, r);
    if (r.d[0])
        return false;
    program().Prg_StModif = 1;
    save_icon();
    return true;
}

// Ed_SavePrg: the same with a .bak first if set so
bool save_program()
{
    if (idata().Ed_SvBak && !make_bak())
        return false;
    return save_program2();
}

// Ed_Sv / Ed_SvAs / Ed_SaveIt
void save_as_name1()
{
    warn_name(154, name1());
    if (!save_program())
        run(ed_d_error);
    run(ed_aver_fin);
    window().Edt_EtatAff |= EtANom;
}

void save_as()
{
    select_or_not_done(74);
    save_as_name1();
}

void save()
{
    Program &p = program();
    if (!p.Prg_NamePrg[0]) {
        save_as();
        return;
    }
    copy_name(name1(), p.Prg_NamePrg);
    Regs r;
    lib_call(L_Dsk_PathIt, r);
    save_as_name1();
}

} // namespace

extern "C" {
void ed_load_hidden_fn(AllRegs *r)
{
    load_hidden();
    r->a[4] = address(ctx().a4);
    r->a[6] = address(ctx().a6);
}

// Ed_LoadHidden: a program into a hidden window, kept
void ed_load_hidden_key(AllRegs *)
{
    tokenise_current_line();
    select_or_not_done(66);
    load_hidden();
    idata().Ed_WindowToDel = 0;
    run(ed_branch_amos);
}

// Ed_Load: a program into the current window
void ed_load(AllRegs *)
{
    tokenise_current_line();
    run(ed_saved);
    select_or_not_done(70);
    run(ed_new2);
    reload();
}

void ed_reload(AllRegs *) { reload(); }

// Ed_DNom: a0 the name without its path
void ed_name_part(AllRegs *r)
{
    const uint8_t *start = ptr<uint8_t>(r->a[0]), *p = start;
    while (*p)
        p++;
    // (from the last character back to the start, a byte before it at worst)
    for (;;) {
        uint8_t c = *--p;
        if (c == '/' || c == ':' || p < start)
            break;
    }
    r->a[0] = address(p + 1);
    r->a[1] = address(start);
}

void ed_merge(AllRegs *r)
{
    merge();
    r->a[4] = address(ctx().a4);
    r->a[6] = address(ctx().a6);
}

void ed_merge_ascii(AllRegs *) { merge_ascii(); }

// Ed_Rename: the program named Name1
void ed_rename(AllRegs *)
{
    copy_name(program().Prg_NamePrg, name1());
    program().Prg_Change = 1;
    window().Edt_EtatAff |= EtANom;
}

// Ed_SaveAsName: saved as Name1 without changing the program's name
void ed_save_as_name(AllRegs *)
{
    uint8_t kept[128];
    copy_name(kept, program().Prg_NamePrg);
    bool ok = save_program2();
    copy_name(program().Prg_NamePrg, kept);
    if (!ok)
        run(ed_d_error);
}

void ed_save_as(AllRegs *)
{
    tokenise_current_line();
    save_as();
}

void ed_save(AllRegs *)
{
    tokenise_current_line();
    save();
}

void ed_save_it(AllRegs *) { save(); }

// Ed_SavePrg / Ed_SavePrg2 / Ed_MakeBak: d0 0 or -1 (the condition codes)
void ed_save_program(AllRegs *r) { r->d[0] = save_program() ? 0 : 0xFFFFFFFF; }
void ed_save_program2(AllRegs *r) { r->d[0] = save_program2() ? 0 : 0xFFFFFFFF; }
void ed_make_bak(AllRegs *r) { r->d[0] = make_bak() ? 0 : 0xFFFFFFFF; }
void ed_save_icon(AllRegs *) { save_icon(); }
}
