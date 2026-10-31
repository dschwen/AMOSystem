// The programs in memory (+Verif.s Prg_*): their structures and text
// buffers, loading and saving, and the stack of running programs (a
// program can run another, an accessory or Prun: the data of the one below
// is kept in its structure, Prg_RunData). Prg_RunIt and Prg_TestIt verify a
// program before it runs.
//
// The assembler hands the program's structure in a6 (WEntry: all the
// registers in an AllRegs).

#include "verif/verif.h"

#include "core/amiga.h"
#include "core/source.h"

#include <dos/dos.h>

#include "hal/files.h"

using namespace amos;

// +Verif.s


namespace amos::verif {

// A5_Reserve: size bytes with the flags at *slot (after a long, the size);
// false if out of memory
bool a5_reserve(be_u32 *slot, uint32_t size, uint32_t flags)
{
    Regs r;
    r.d[0] = size + 4;
    r.d[1] = flags;
    r.a[0] = w::reserve(r.d[0], r.d[1]);
    if (!r.a[0])
        return false;
    poke_l(ptr<void>(r.a[0]), size);
    *slot = r.a[0] + 4;
    return true;
}

// A5_Free: the memory at *slot, if any
void a5_free(be_u32 *slot)
{
    if (uint32_t m = *slot) {
        *slot = 0;
        Regs r;
        r.a[1] = m - 4;
        r.d[0] = peek_l(ptr<void>(r.a[1])) + 4;
        w::release(r.a[1], r.d[0]);
    }
}

// D_Open, D_Read, D_Write, D_Close of +B.s: the current file (Handle)
bool file_open(uint32_t mode)
{
    IData &d = idata();
    d.Handle = static_cast<uint32_t>(hal::open(ptr<char>(d.Name1), mode));
    return d.Handle;
}

bool file_read(void *to, uint32_t length) { return uint32_t(hal::read(BPTR(idata().Handle), to, length)) == length; }
bool file_write(const void *from, uint32_t length) { return uint32_t(hal::write(BPTR(idata().Handle), const_cast<void *>(from), length)) == length; }

void file_close()
{
    IData &d = idata();
    if (BPTR handle = BPTR(d.Handle)) {
        d.Handle = 0;
        hal::close(handle);
    }
}

int32_t file_seek(int32_t offset, int32_t mode) { return hal::seek(BPTR(idata().Handle), offset, mode); }

namespace {

// The headers of a program file: AMOS 1.3 and AMOS Pro. Byte 11 is 'V'
// for a tested program, 'v' if not; byte 15 of the Pro one the maths flags.
const char Header13[] = "AMOS Basic v134 ";
const char HeaderPro[] = "AMOS Pro101v\0\0\0";

bool same(const char *a, const char *b, int n)
{
    for (int i = 0; i < n; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

} // namespace

int program_kind(const char *header)
{
    if (same(header, Header13, 10))
        return 0;
    return same(header, HeaderPro, 8) ? 1 : -1;
}

} // namespace amos::verif

using namespace amos::verif;

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint32_t ModeOldFile = 1005;
constexpr uint32_t ModeNewFile = 1006;
constexpr int NFiche = 10, TFiche = 10;  // the files' table
constexpr int DevMax = 7, LibMax = 7;    // devices (3 longs each), libraries


Program &program_of(AllRegs *r) { return *ptr<Program>(r->a[6]); }

void lib(int routine)
{
    Regs r;
    lib_call(routine, r);
}

// Prg_SetBanks: the banks, dialogs and source of program p become the current
void set_banks(Program &p)
{
    IData &d = idata();
    d.Cur_Banks = address(&p.Prg_Banks);
    d.Cur_Dialogs = address(&p.Prg_Dialogs);
    d.Prg_Source = p.Prg_StBas;
}

// Prg_ChgTTexte: a text buffer of size bytes for p (0: none). Returns
// StBas, 0 if out of memory. With size 0 the original returns its a0.
uint32_t change_text(Program &p, uint32_t size, uint32_t a0)
{
    size &= ~1u;
    if (p.Prg_StTTexte) {
        Regs f;
        f.d[0] = p.Prg_StTTexte;
        f.a[1] = p.Prg_StMini;
        w::release(f.a[1], f.d[0]);
        p.Prg_StTTexte = 0;
    }
    if (!size)
        return a0;
    Regs r;
    r.d[0] = size;
    r.a[0] = w::reserve(r.d[0], w::FastClear);
    if (!r.a[0])
        return 0;
    p.Prg_StTTexte = size;
    p.Prg_StMini = r.a[0];
    p.Prg_StHaut = r.a[0] + size;
    p.Prg_StBas = p.Prg_StHaut - 2;
    poke_w(ptr<void>(p.Prg_StBas), 0);
    return p.Prg_StBas;
}

// Prg_DelStructure: p, its text and banks, out of the list of programs
void delete_structure(Program &p)
{
    IData &d = idata();
    if (p.Prg_StTTexte) {
        set_banks(p);
        clear_variables();
        bank_erase_all();
        change_text(p, 0, 0);
    }
    be_u32 *link = &d.Prg_List;
    while (*link != address(&p))
        link = &ptr<Program>(*link)->Prg_Next;
    *link = p.Prg_Next;
    Regs r;
    r.d[0] = Program::Prg_Long;
    r.a[1] = address(&p);
    sy_call(sy::SyFree, r);
}

// Prg_NewStructure: a new program with a text buffer of size bytes, first
// in the list; 0 if out of memory
uint32_t new_structure(uint32_t size)
{
    IData &d = idata();
    Regs r;
    r.d[0] = Program::Prg_Long;
    sy_call(sy::SyFast, r);
    if (!r.d[0])
        return 0;
    Program &p = *ptr<Program>(r.d[0]);
    p.Prg_Next = d.Prg_List;
    d.Prg_List = address(&p);
    // with no buffer, the address of the structure (a0) tells success
    if (!change_text(p, size, address(&p))) {
        delete_structure(p);
        return 0;
    }
    return address(&p);
}

// Prg_DejaRunned: p is running (itself or below the program running)
bool running(const Program &p)
{
    for (uint32_t q = idata().Prg_Runned; q; q = ptr<Program>(q)->Prg_Previous)
        if (q == address(&p))
            return true;
    return false;
}

// Prg_DataSave: the running program's data into *slot, with the list of
// routines to call at ClearVar (their addresses, then 0); false if out of
// memory
bool data_save(be_u32 *slot)
{
    IData &d = idata();
    constexpr uint32_t Size = IData::FinSave - IData::DebSave;
    if (!a5_reserve(slot, Size + 4 + 4 + 4 * 32, MemPublic | MemClear))
        return false;
    uint8_t *to = ptr<uint8_t>(*slot);
    const uint8_t *from = reinterpret_cast<const uint8_t *>(&d) + IData::DebSave;
    for (uint32_t i = 0; i < Size; i++)
        *to++ = from[i];
    uint32_t routine = d.Sys_ClearRoutines;
    for (;;) {
        poke_l(to, routine);
        to += 4;
        if (!routine)
            break;
        routine = peek_l(ptr<void>(routine)) << 1;
    }
    return true;
}

// Prg_DataLoad: the data saved at *slot back (and freed)
void data_load(be_u32 *slot)
{
    IData &d = idata();
    constexpr uint32_t Size = IData::FinSave - IData::DebSave;
    const uint8_t *from = ptr<uint8_t>(*slot);
    uint8_t *to = reinterpret_cast<uint8_t *>(&d) + IData::DebSave;
    for (uint32_t i = 0; i < Size; i++)
        to[i] = *from++;
    // the routines' links: the next one / 2, bit 31 set (in the list)
    uint32_t routine = peek_l(from);
    from += 4;
    d.Sys_ClearRoutines = routine;
    while (routine) {
        uint32_t next = peek_l(from);
        from += 4;
        poke_l(ptr<void>(routine), (next >> 1) | 0x80000000u);
        routine = next;
    }
    Regs c;
    c.d[0] = d.Cur_ChrJump >> 16;   // a word in the long's first half
    lib_call(L_SetChrPatch, c);
    bank_changed();
    a5_free(slot);
}

// Prg_DataNew: a clean interpreter for the program pushed
void data_new()
{
    IData &d = idata();
    d.VarBuf = 0;
    d.VNmMini = 0;
    d.Stack_Size = 0;
    d.Stack_CSize = 0;
    d.MnBase = 0;
    d.OMnNb = 0;
    d.OMnBase = 0;
    d.Prg_Includes = 0;
    d.Prg_FullSource = 0;
    d.Patch_Errors = 0;
    d.Patch_Menage = 0;
    d.Patch_ScFront = 0;
    d.Patch_ScCopy = 0;
    d.ActuMask &= ~2;
    Regs c;
    c.d[0] = 0;
    lib_call(L_SetChrPatch, c);
    d.Ed_Zappeuse = 0;
    d.Prg_Accessory = 0;
    for (int i = 0; i < NFiche; i++)
        poke_l(d.Fichiers + i * TFiche, 0);
    for (int i = 0; i < 3 * DevMax; i++)
        poke_l(d.Dev_List + 4 * i, 0);
    for (int i = 0; i < LibMax; i++)
        d.Lib_List[i] = 0;
}

// Prg_Push: p becomes the running program, above the one running (whose
// data is saved); false if out of memory
bool push(Program &p)
{
    IData &d = idata();
    p.Prg_Previous = d.Prg_Runned;
    d.Prg_Runned = address(&p);
    if (!p.Prg_Previous)
        return true;
    if (!data_save(&ptr<Program>(p.Prg_Previous)->Prg_RunData)) {
        d.Prg_Runned = p.Prg_Previous;
        p.Prg_Previous = 0;
        return false;
    }
    data_new();
    return true;
}

// Prg_Pull: back to the program below the running one (the last one
// keeps everything)
void pull()
{
    IData &d = idata();
    if (!d.Prg_Runned)
        return;
    Program &p = *ptr<Program>(d.Prg_Runned);
    d.Prg_Runned = p.Prg_Previous;
    p.Prg_Previous = 0;
    if (!d.Prg_Runned)
        return;
    clear_variables();
    includes_clear();
    if (!p.Prg_Edited)
        delete_structure(p);
    data_load(&ptr<Program>(d.Prg_Runned)->Prg_RunData);
}

// The display routines of a test: hooks + 0 before, + 4 after, + 8 before
// the screens of a normal program are made (a2 = hooks, as they were)
void hook(uint32_t hooks, int which)
{
    if (!hooks)
        return;
    Regs r;
    r.a[2] = hooks;
    amos_call(&r, ptr<uint8_t>(hooks + which));
}

// PTest on program p (pass 1 and 2; an error goes to Prg_JError)
void test(Program &p, uint32_t hooks)
{
    IData &d = idata();
    p.Prg_Not1_3 = 0;
    hook(hooks, 0);
    clear_variables();
    AllRegs r = {};
    r.a[2] = hooks;
    r.a[3] = address(ctx().a3);
    r.a[4] = address(ctx().a4);
    r.a[5] = address(ctx().a5);
    r.a[6] = address(&p);
    r.d[6] = ctx().d6;
    r.d[7] = ctx().d7;
    amos_call_all(&r, ver_program_test_entry);
    p.Prg_StModif = 0;
    p.Prg_Not1_3 = d.VerNot1_3;
    p.Prg_MathFlags = d.MathFlags;
    hook(hooks, 4);
}

// Prg_CptLines: the number of lines of p (a closed procedure is one)
void count_lines(Program &p)
{
    const uint8_t *line = ptr<uint8_t>(p.Prg_StBas);
    uint16_t n = 0;
    while (*line) {
        n++;
        if (peek_w(line + 2) == tk::Proc && int8_t(line[10]) < 0)
            line += peek_l(line + 4) + 10 + 2 + 2;
        else
            line += 2 * *line;
    }
    p.Prg_NLigne = n;
}

void copy_name(Program &p)
{
    const char *from = ptr<char>(idata().Name1);
    char *to = reinterpret_cast<char *>(p.Prg_NamePrg);
    while ((*to++ = *from++))
        ;
}

// Prg_Load: the program file Name1 into p. mode < 0: always a new buffer
// (fitting the program), 0: a new one if too small, > 0: return 1 if too
// small (and *needed the size to ask for). Returns 0, -1 disk error, -2
// out of memory, -3 not an AMOS program.
int32_t load_file(Program &p, int16_t mode, uint32_t *needed)
{
    IData &d = idata();
    const char *header = ptr<char>(d.Buffer);
    if (!file_read(ptr<void>(d.Buffer), 16 + 4))
        return -1;
    p.Prg_MathFlags = 0;
    p.Prg_Not1_3 = 0;
    uint8_t maths = 0;
    int kind = program_kind(header);
    if (kind) {
        p.Prg_Not1_3 = 1;
        if (kind < 0)
            return -3;
        maths = header[15];
    }
    uint32_t size = peek_l(header + 16);
    if (mode >= 0 && int32_t(size) < int32_t(p.Prg_StTTexte)) {
        // fits
    } else if (mode > 0) {
        *needed = size + 256;
        return 1;
    } else if (!change_text(p, size + 256, 0)) {
        return -2;
    }
    uint8_t *end = ptr<uint8_t>(p.Prg_StHaut) - 2;
    poke_w(end, 0);
    uint8_t *start = end - size;
    if (!file_read(start, size))
        return -1;
    p.Prg_StBas = address(start);
    set_banks(p);
    Regs b;
    b.d[0] = EntNul;
    lib_call(L_Bnk_Load, b);
    if (int16_t(b.d[0]))
        return int16_t(b.d[0]);
    copy_name(p);
    count_lines(p);
    p.Prg_Change = 0;
    p.Prg_MathFlags = maths;
    return 0;
}

int32_t load(Program &p, int16_t mode, uint32_t *needed)
{
    int32_t result = file_open(ModeOldFile) ? load_file(p, mode, needed) : -1;
    file_close();
    return result;
}

// Prg_Save: p into the file Name1; false if writing failed
bool save(Program &p)
{
    IData &d = idata();
    if (!file_open(ModeNewFile))
        return false;
    // the Pro header, or 1.3's if the program is compatible
    char header[16];
    const char *from = p.Prg_Not1_3 ? "AMOS Pro101v\0\0\0" : "AMOS Basic v134 ";
    for (int i = 0; i < 16; i++)
        header[i] = from[i];
    if (p.Prg_Not1_3)
        header[15] = p.Prg_MathFlags;
    header[11] = p.Prg_StModif ? 'v' : 'V';
    if (!file_write(header, 16))
        return false;
    // the end of the program: its first zero line, not the zeros after
    uint8_t *start = ptr<uint8_t>(p.Prg_StBas);
    uint8_t *end = start;
    for (;;) {
        source::Found f = source::find_line(end, 1);
        end = f.line;
        if (!peek_w(end))
            break;
    }
    uint32_t size = end - start;
    poke_l(ptr<void>(d.Buffer), size);
    if (!file_write(ptr<void>(d.Buffer), 4) || !file_write(start, size))
        return false;
    set_banks(p);
    Regs b;
    lib_call(L_Bnk_SaveAll, b);
    if (b.d[0])
        return false;
    file_close();
    copy_name(p);
    p.Prg_Change = 0;
    return true;
}

// Prg_New: p empty (no lines, name or banks). show 1: DefRun (screen
// opened), -1 or 2: DefRun without the screen, 0: no DefRun.
void new_program(Program &p, uint16_t show)
{
    p.Prg_NLigne = 0;
    if (p.Prg_StTTexte) {
        p.Prg_StBas = p.Prg_StHaut - 2;
        poke_w(ptr<void>(p.Prg_StBas), 0);
        p.Prg_NamePrg[0] = 0;
        p.Prg_Change = 0;
        p.Prg_StModif = 1;
        p.Prg_MathFlags = 0;
    }
    wdata().Actualise &= 0x00FF;
    set_banks(p);
    bank_erase_all();
    clear_variables();
    if (show) {
        idata().DefFlag = show;
        lib(L_DefRun1);
        lib(L_DefRun2);
    }
}

char upper(char c) { return c >= 'a' && c <= 'z' ? c - 0x20 : c; }

// Dsk.DNom: the name after the path
const char *file_part(const char *name)
{
    Regs r;
    r.a[0] = address(name);
    lib_call(L_Dsk_DNom, r);
    return ptr<char>(r.a[0]);
}

// Prg_AccAdr: the program in memory with the file name of Name1 (the
// paths ignored, any case), or 0
uint32_t find_program()
{
    IData &d = idata();
    const char *name = file_part(ptr<char>(d.Name1));
    for (uint32_t q = d.Prg_List; q; q = ptr<Program>(q)->Prg_Next) {
        const char *a = file_part(reinterpret_cast<char *>(ptr<Program>(q)->Prg_NamePrg));
        const char *b = name;
        for (;;) {
            char ca = upper(*a++), cb = upper(*b++);
            if (ca != cb)
                break;
            if (!ca)
                return q;
        }
    }
    return 0;
}

} // namespace

// Prg_RunIt / Prg_TestIt: d0 = 0 a program, 1 an accessory, -1 Prun;
// a1 = where an error goes; a2 = the display hooks (or 0). Out: d0 = -1
// running already, 0 out of memory; 1: RunIt jumps to the program.
extern "C" {

void ver_prg_run_it(AllRegs *r)
{
    Program &p = program_of(r);
    IData &d = idata();
    int16_t mode = int16_t(r->d[0]);
    uint32_t hooks = r->a[2];
    if (running(p)) {
        r->d[0] = uint32_t(-1);
        return;
    }
    if (!push(p)) {
        r->d[0] = 0;
        return;
    }
    d.Prg_JError = r->a[1];
    set_banks(p);
    bank_changed();
    test(p, hooks);
    if (mode && (mode < 0 || d.Prg_Accessory)) {
        // an accessory or Prun
        lib(L_DefRunAcc);
        boot::recop();
    } else {
        d.DefFlag = 0xFFFF;
        lib(L_DefRun1);
        hook(hooks, 8);
        lib(L_DefRun2);
        boot::recop();
        Regs a;
        a.d[1] = 0;
        ec_call(ec::Active, a);
        d.Prg_Accessory = 0;
    }
    lib(L_WOption);
    r->d[0] = 1;
}

void ver_prg_test_it(AllRegs *r)
{
    Program &p = program_of(r);
    if (running(p)) {
        r->d[0] = uint32_t(-1);
        return;
    }
    if (!push(p)) {
        r->d[0] = 0;
        return;
    }
    idata().Prg_JError = r->a[1];
    set_banks(p);
    test(p, r->a[2]);
    pull();
    r->d[0] = 0;
}

// Flags only, d0 kept (WEntryF)
uint32_t ver_prg_deja_runned(AllRegs *r) { return running(program_of(r)) ? uint32_t(-1) : 0; }

void ver_prg_push(AllRegs *r) { r->d[0] = push(program_of(r)) ? 0 : uint32_t(-1); }
void ver_prg_pull(AllRegs *) { pull(); }

// a0 = the place of the data: d0 = -1, or 0 if out of memory
void ver_prg_data_save(AllRegs *r) { r->d[0] = data_save(ptr<be_u32>(r->a[0])) ? uint32_t(-1) : 0; }
void ver_prg_data_load(AllRegs *r)
{
    data_load(ptr<be_u32>(r->a[0]));
    r->d[0] = 0;
}
void ver_prg_data_new(AllRegs *r)
{
    data_new();
    r->d[0] = 0;
}

void ver_prg_new_structure(AllRegs *r) { r->d[0] = new_structure(r->d[0]); }
void ver_prg_del_structure(AllRegs *r) { delete_structure(program_of(r)); }

void ver_prg_set_banks(AllRegs *r) { set_banks(program_of(r)); }
void ver_prg_reset_banks(AllRegs *) { set_banks(*ptr<Program>(idata().Prg_Runned)); }

void ver_prg_new(AllRegs *r) { new_program(program_of(r), uint16_t(r->d[0])); }

void ver_prg_chg_ttexte(AllRegs *r)
{
    r->d[0] = change_text(program_of(r), r->d[0], r->a[0]);
    r->a[0] = r->d[0];
}

void ver_prg_load(AllRegs *r) { r->d[0] = load(program_of(r), int16_t(r->d[0]), &r->d[1]); }

void ver_prg_cpt_lines(AllRegs *r)
{
    Program &p = program_of(r);
    count_lines(p);
    r->d[0] = p.Prg_NLigne;
}

void ver_prg_acc_adr(AllRegs *r) { r->a[0] = r->d[0] = find_program(); }

// The original left the file open when writing failed
void ver_prg_save(AllRegs *r)
{
    bool ok = save(program_of(r));
    if (!ok)
        file_close();
    r->d[0] = ok ? 0 : uint32_t(-1);
}

}
