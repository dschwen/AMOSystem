// The editor's lists and memory blocks, its dialogue boxes, its macros
// (+Edit.s Ed_MemReserve, Ed_ListeNew..., Ed_Dialogue, Ed_DChaine...,
// EdMa_*).
//
// A list: a long in the data zone pointing to its first element; an
// element: the next one, the length of its data, the data.
// A macro: an element whose data are the key long, then the keys (see
// editor/keys.cpp), -1 at the end. A file of macros: "ApMa", then each
// element as it is in memory (the next one too), then 8 zeros.
// While a macro is recorded, the first element of EdMa_List is a buffer
// of 1024 bytes, EdMa_Tape one more than the bytes recorded.

#include "editor/editor.h"
#include "core/amiga.h"

#include <dos/dos.h>
#include <exec/memory.h>

#include <dos/dos.h>

#include "hal/files.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint32_t MacroHead = 0x41704D61;      // "ApMa"
constexpr uint16_t DiaImages = 66;              // Ed_DiaImages
constexpr uint16_t EdDMacro1 = 13, EdDMacro2 = 14, EdDMacroD = 18, EdDMacroNA = 19, EdDMacroAll = 20,
                   EdDMacroEra = 21, EdDMacroNo = 22, EdDMacroPas = 23;

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

[[noreturn]] void not_done()
{
    run(ed_not_done);
    __builtin_unreachable();
}

be_u32 *macros() { return &idata().EdMa_List; }

uint32_t last_key() { return ptr<Dialog>(idata().Ed_ADialogues)->Dia_LastKey; }

// Ed_Open / Ed_Read / Ed_Write / Ed_Close on Handle(a5)
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

// Sys_AddPath: the system's message n with the system's path into Name1
void system_file(uint16_t n)
{
    Regs m;
    m.d[0] = n;
    call(ed_get_systeme, m);
    Regs p;
    p.a[0] = m.a[0];
    boot::sys_add_path(ptr<char>(p.a[0]));
}

} // namespace

namespace amos::ed {

// Ed_MemReserve: size cleared bytes (their size in the long before them)
// into *at; 0 if out of memory
uint8_t *memory_reserve(be_u32 *at, uint32_t size)
{
    uint32_t m = memory(size + 4, true);
    if (!m)
        return nullptr;
    poke_l(ptr<uint8_t>(m), size);
    *at = m + 4;
    return ptr<uint8_t>(m + 4);
}

// Ed_MemFree
void memory_release(be_u32 *at)
{
    if (uint32_t m = *at) {
        *at = 0;
        free_memory(m - 4, peek_l(ptr<uint8_t>(m - 4)) + 4);
    }
}

// Ed_ListeNew: a new element of size bytes of data at the head of the
// list; 0 if out of memory
uint8_t *list_new(be_u32 *list, uint32_t size)
{
    uint32_t m = memory(size + 8, true);
    if (!m)
        return nullptr;
    uint8_t *e = ptr<uint8_t>(m);
    poke_l(e, *list);
    *list = m;
    poke_l(e + 4, size);
    return e;
}

// Ed_ListeDel: element e out of the list (nothing if it is not in it)
void list_delete(be_u32 *list, uint8_t *e)
{
    uint8_t *before = reinterpret_cast<uint8_t *>(list);
    for (uint32_t n = peek_l(before); n; n = peek_l(before)) {
        if (n == address(e)) {
            poke_l(before, peek_l(e));
            free_memory(n, peek_l(e + 4) + 8);
            return;
        }
        before = ptr<uint8_t>(n);
    }
}

// Ed_ListeDelAll
void list_delete_all(be_u32 *list)
{
    while (uint32_t e = *list)
        list_delete(list, ptr<uint8_t>(e));
}

// Ed_IChaine / Ed_DChaine / Ed_AChaine: strings for the dialogues in
// Ed_BufT, each a zero, its length, the text (with a zero after it);
// Ed_DChaine starts one, Ed_AChaine adds to it. Both return the string.
void dialogue_strings_reset()
{
    IData &d = idata();
    d.Ed_DiaCopyC = d.Ed_BufT;
}

uint8_t *dialogue_string_add(const uint8_t *text, const uint8_t **after)
{
    IData &d = idata();
    uint8_t *a1 = ptr<uint8_t>(d.Ed_DiaCopyC);
    while ((*a1++ = *text++))
        ;
    if (after)
        *after = text;
    a1--;
    d.Ed_DiaCopyC = address(a1);
    uint8_t *s = ptr<uint8_t>(d.Ed_DiaCopyD);
    s[0] = 0;
    s[1] = uint8_t(a1 - s - 2);
    return s;
}

uint8_t *dialogue_string(const uint8_t *text, const uint8_t **after)
{
    IData &d = idata();
    d.Ed_DiaCopyD = d.Ed_DiaCopyC;
    d.Ed_DiaCopyC += 2;
    return dialogue_string_add(text, after);
}

// Ed_DoDialog: dialogue n of the editor's resource; its answer
int32_t run_dialogue(uint16_t n)
{
    IData &d = idata();
    Regs b;
    call(ed_set_banks, b);
    Regs r;
    r.d[0] = 1;
    r.d[1] = n;
    r.d[2] = r.d[3] = EntNul;
    lib_call(L_Dia_RunProgram, r);
    d.Dia_Magic = 0;
    dialogue_strings_reset();
    return int32_t(r.d[1]);
}

// Ed_Dialogue: the same, but a program driving the editor gives the
// answer instead
int32_t dialogue(uint16_t n)
{
    IData &d = idata();
    if (d.Ed_Zappeuse)
        return int32_t(d.Ed_ZapParam);
    d.Dia_Magic = 0;
    if (hal::available_memory(hal::MemoryPublic | hal::MemoryChip) >= 48 * 1024)
        return run_dialogue(n);
    // under 48k of chip memory, the dialogue does not keep what was under it
    d.Dia_Magic = 0x4E6F624C;                   // "NobL"
    int32_t answer = run_dialogue(n);
    draw_windows();
    return answer;
}

} // namespace amos::ed

namespace {

// Ed_InitDialogues: the editor's channel of dialogues (1); its error
uint32_t dialogues_open()
{
    IData &d = idata();
    Regs b;
    call(ed_set_banks, b);
    const uint8_t *resource = ptr<uint8_t>(d.Ed_Resource);
    const uint8_t *programs = resource + peek_l(resource + 2 + 8);
    Regs r;
    r.d[0] = 1;
    r.d[1] = 1024;
    r.d[2] = 16;
    r.d[3] = 0;                 // the program not copied
    r.a[0] = address(programs + peek_l(programs + 2));      // its first program
    r.a[1] = address(resource + peek_l(resource + 2));      // the graphics
    r.a[2] = d.Ed_Messages;
    lib_call(L_Dia_OpenChannel, r);
    if (r.d[0])
        return r.d[0];
    d.Ed_ADialogues = r.a[0];
    d.Ed_VDialogues = r.a[1];
    ptr<Dialog>(r.a[0])->Dia_PuzzleI = DiaImages;
    dialogue_strings_reset();
    return 0;
}

// Ed_EndDialogues
void dialogues_close()
{
    Regs b;
    call(ed_set_banks, b);
    Regs r;
    r.d[0] = 1;
    lib_call(L_Dia_CloseChannel, r);
}

// EdMa_Load: the macros of the file Name1 instead of the ones there; 0,
// 1 a disk error, 2 not a file of macros, -1 out of memory
int32_t load()
{
    IData &d = idata();
    list_delete_all(macros());
    d.EdMa_Change = 0;
    int32_t result = 1;
    if (open(MODE_OLDFILE)) {
        uint8_t *buffer = ptr<uint8_t>(d.Buffer);
        if (read(buffer, 4)) {
            result = 2;
            if (peek_l(buffer) == MacroHead) {
                for (;;) {
                    if (!read(buffer, 8)) {
                        result = 1;
                        break;
                    }
                    uint32_t size = peek_l(buffer + 4);
                    if (!size) {
                        result = 0;
                        break;
                    }
                    uint8_t *e = list_new(macros(), size);
                    if (!e) {
                        result = -1;
                        break;
                    }
                    if (!read(e + 8, int32_t(size))) {
                        result = 1;
                        break;
                    }
                }
            }
        }
    }
    close();
    return result;
}

// EdMa_Save: the macros into the file Name1; false if a disk error
bool save()
{
    IData &d = idata();
    if (!open(MODE_NEWFILE))
        run(ed_d_error);
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    poke_l(buffer, MacroHead);
    bool ok = write(buffer, 4);
    for (uint32_t e = *macros(); ok && e; e = peek_l(ptr<uint8_t>(e)))
        ok = write(ptr<uint8_t>(e), int32_t(peek_l(ptr<uint8_t>(e) + 4) + 8));
    if (ok) {
        poke_l(buffer, 0);
        poke_l(buffer + 4, 0);
        ok = write(buffer, 8);
    }
    close();
    return ok;
}

// EdMa_LoadIt: with a warning, after asking if there are macros
void load_it()
{
    if (*macros() && int16_t(dialogue(EdDMacroEra)) != 1)
        not_done();
    run_d0(ed_warn, 62);
    int32_t result = load();
    run(ed_aver_fin);
    if (!result)
        return;
    if (result < 0)
        run(ed_out_of_memory);
    if (result == 1)
        run(ed_d_error);
    dialogue(EdDMacroPas);
    place_cursor();
}

// EdMa_SaveIt
void save_it()
{
    run_d0(ed_warn, 61);
    bool ok = save();
    run(ed_aver_fin);
    if (!ok)
        run(ed_d_error);
    idata().EdMa_Changed = 0;
}

bool select_file(uint16_t message)
{
    Regs r;
    r.d[0] = message;
    if (!uint16_t(call(ed_file_selector, r)))
        return false;
    path_it();
    return true;
}

// EdMa_LoadDefault / EdMa_LoadAs / EdMa_SaveDef / EdMa_SaveAs
void load_default()
{
    system_file(46);
    load_it();
    idata().EdMa_Changed = 0;
}

void load_as()
{
    if (!select_file(55))
        not_done();
    load_it();
    idata().EdMa_Changed = 1;
}

void save_default()
{
    system_file(46);
    save_it();
}

void save_as()
{
    if (!select_file(51))
        not_done();
    run(ed_save_over);
    save_it();
}

// EdMa_No
void no_macros()
{
    dialogue(EdDMacroNo);
    place_cursor();
}

// EdMa_Del: the macro of the key pressed erased
void erase()
{
    tokenise_current_line();
    if (!*macros())
        return no_macros();
    if (int16_t(dialogue(EdDMacroD)))
        not_done();
    uint32_t key = last_key();
    if (!key)
        not_done();
    if (uint8_t *m = macro_of(key)) {
        list_delete(macros(), m);
        idata().EdMa_Change = 1;
    } else {
        dialogue(EdDMacroNA);
    }
    place_cursor();
}

// EdMa_DelAll
void erase_all()
{
    tokenise_current_line();
    if (!*macros())
        return no_macros();
    if (int16_t(dialogue(EdDMacroAll)) != 1)
        not_done();
    list_delete_all(macros());
    idata().EdMa_Change = 1;
    place_cursor();
}

// EdMa_New: a macro recorded for the key pressed (that key's one replaced)
void record()
{
    IData &d = idata();
    tokenise_current_line();
    if (int16_t(dialogue(EdDMacro1)))
        not_done();
    uint32_t key = last_key();
    if (!key)
        not_done();
    if (uint8_t *m = macro_of(key)) {
        if (int16_t(dialogue(EdDMacro2)) != 1)
            not_done();
        list_delete(macros(), m);
    }
    uint8_t *buffer = list_new(macros(), 1024);
    // (the assembler went on with no memory, and wrote at address 8)
    if (!buffer)
        run(ed_out_of_memory);
    d.EdMa_Tape = 1;
    poke_l(buffer + 8, key);
    poke_l(buffer + 8 + 1024 - 4, 0xFFFFFFFF);
    run(ed_aver_fin);
    place_cursor();
}

// EdMa_Stop: the macro recorded kept (a mouse button ends it)
void stop()
{
    IData &d = idata();
    Regs n;
    lib_call(L_Dia_NoMKey, n);
    d.EdMa_Change = 1;
    uint8_t *buffer = ptr<uint8_t>(*macros());
    uint16_t recorded = d.EdMa_Tape - 1;
    d.EdMa_Tape = 0;
    if (!recorded) {
        list_delete(macros(), buffer);
        not_done();
    }
    uint8_t *m = list_new(macros(), 8 + recorded);
    // (the assembler went on with no memory, and wrote at address 8)
    if (!m) {
        list_delete(macros(), buffer);
        run(ed_out_of_memory);
    }
    poke_l(m + 8, peek_l(buffer + 8));
    for (uint16_t i = 0; i < recorded; i++)
        m[12 + i] = buffer[12 + i];
    m[12 + recorded] = 0xFF;
    list_delete(macros(), buffer);
    run_d0(ed_alert_100, 45);
}

} // namespace

namespace amos::ed {

// EdMa_Adr: the macro of key, 0 if none
uint8_t *macro_of(uint32_t key)
{
    for (uint32_t m = *macros(); m; m = peek_l(ptr<uint8_t>(m)))
        if (peek_l(ptr<uint8_t>(m) + 8) == key)
            return ptr<uint8_t>(m);
    return nullptr;
}

} // namespace amos::ed

extern "C" {
// a0 the long of the block; a0 the block, 0 if out of memory; d0 + 4
uint32_t ed_mem_reserve(AllRegs *r)
{
    uint32_t size = r->d[0];
    uint8_t *m = memory_reserve(ptr<be_u32>(r->a[0]), size);
    r->a[0] = address(m);
    if (m)
        r->d[0] = size + 4;
    return address(m);
}

void ed_mem_free(AllRegs *r) { memory_release(ptr<be_u32>(r->a[0])); }

// a0 the list, d0 the size; a1 the element, the condition codes on it
uint32_t ed_list_new(AllRegs *r)
{
    uint8_t *e = list_new(ptr<be_u32>(r->a[0]), r->d[0]);
    r->a[1] = address(e);
    return address(e);
}

void ed_list_delete(AllRegs *r) { list_delete(ptr<be_u32>(r->a[0]), ptr<uint8_t>(r->a[1])); }

void ed_list_delete_all(AllRegs *r)
{
    list_delete_all(ptr<be_u32>(r->a[0]));
    r->d[0] = 0;
}

void ed_dialogues_open(AllRegs *r) { r->d[0] = dialogues_open(); }
void ed_dialogues_close(AllRegs *) { dialogues_close(); }
void ed_strings_reset(AllRegs *) { dialogue_strings_reset(); }

// a0 the text; a1 the string, d0 its length, a0 after the text
static void string_regs(AllRegs *r, uint8_t *(*f)(const uint8_t *, const uint8_t **))
{
    const uint8_t *after;
    uint8_t *s = f(ptr<uint8_t>(r->a[0]), &after);
    r->a[0] = address(after);
    r->a[1] = address(s);
    r->d[0] = (r->d[0] & 0xFFFFFF00) | s[1];
}

void ed_string_new(AllRegs *r) { string_regs(r, dialogue_string); }
void ed_string_add(AllRegs *r) { string_regs(r, dialogue_string_add); }

// d0 the dialogue; d0 (and d1) its answer
void ed_dialogue_fn(AllRegs *r)
{
    int32_t answer = dialogue(uint16_t(r->d[0]));
    r->d[0] = r->d[1] = uint32_t(answer);
}

void ed_do_dialogue(AllRegs *r)
{
    int32_t answer = run_dialogue(uint16_t(r->d[0]));
    r->d[0] = r->d[1] = uint32_t(answer);
}

void ed_macro_load_default(AllRegs *) { load_default(); }
void ed_macro_load_as(AllRegs *) { load_as(); }
void ed_macro_save_default(AllRegs *) { save_default(); }
void ed_macro_save_as(AllRegs *) { save_as(); }
void ed_macro_load(AllRegs *r) { r->d[0] = uint32_t(load()); }
void ed_macro_erase(AllRegs *) { erase(); }
void ed_macro_erase_all(AllRegs *) { erase_all(); }
void ed_macro_record(AllRegs *) { record(); }
void ed_macro_stop_fn(AllRegs *) { stop(); }
void ed_macros_free(AllRegs *) { list_delete_all(macros()); }
}
