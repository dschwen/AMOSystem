// The programs AMOSPro loads (+B.s): the editor and the monitor, their
// resource banks, the mouse and the file selector's bank; the instructions
// that close and kill the editor or call the monitor, and the return to an
// editor that is not there any more after a program stops.

#include "main/main.h"

#include "core/amiga.h"

#include <dos/dos.h>

#include "hal/files.h"

using namespace amos;
using namespace amos::boot;

extern "C" {
// main_glue.s: where a program's error goes without the editor
extern uint8_t main_run_error_run_only[], main_run_error_no_editor[];
// The entry tables of the editor and the monitor (+Edit.s, +Monitor.s)
extern const uint8_t ed_debut[], mon_debut[];
[[noreturn]] void interp_jump(const Regs *r, const void *fn);
}

namespace {

void lib(int routine)
{
    Regs r;
    lib_call(routine, r);
}

// Program_Load: the editor or the monitor, linked into AMOSPro (they were
// programs of their own, loaded with LoadSeg): *segment marks it loaded,
// its entry table (offsets from its start, then 0, then the address of its
// amos_bridge) goes into the jump table from routine first+1 on
uint32_t program_load(be_u32 &segment, const uint8_t *module, uint32_t first)
{
    IData &d = idata();
    segment = address(module);
    uint32_t base = address(module);
    const be_u32 *table = ptr<be_u32>(base);
    be_u32 *jump = ptr<be_u32>(d.AdTokens[0] - int16_t(uint16_t((first << 2) + LB_Size + 4)));
    while (uint32_t offset = *table++)
        *--jump = base + offset;
    // its C++ uses AMOSPro's contexts (src/core/call.s)
    poke_l(ptr<void>(*table), wdata().CBridge);
    return 0;
}

void program_free(be_u32 &segment) { segment = 0; }

// Bnk.Load from the system file: d0 the number of the bank (-1 all), the
// error as a word
int16_t load_banks(int32_t n)
{
    Regs r;
    r.d[0] = uint32_t(n);
    lib_call(L_Bnk_Load, r);
    return int16_t(r.d[0]);
}

// Prg_FirstRun: KILL/CLOSE EDITOR and MONITOR in a program run from the
// editor only, not in an accessory or a program run by another one
bool first_run()
{
    IData &d = idata();
    if (d.Direct)
        error(17);              // illegal direct mode
    if (d.Prg_Accessory)
        return false;
    return !ptr<Program>(d.Prg_Runned)->Prg_Previous;
}

// The end of CLOSE EDITOR and KILL EDITOR (CloCloX)
void editor_closed()
{
    IData &d = idata();
    a5_free(d.Equ_Base);
    lib(L_Prg_ReSetBanks);
    if (uint16_t screen = d.ScOn) {
        Regs r;
        r.d[1] = uint32_t(screen - 1);
        ec_call(ec::Active, r);
    }
}

// Reset_Request: the requesters back to AMOS (the Request extension's
// default routine)
void reset_request()
{
    if (uint32_t routine = idata().ExtAdr[2 * 4 + 1]) {
        Regs r;
        amos_call(&r, ptr<void>(routine));
    }
}

// MemMaximum: as much memory as possible: no variables, no screens, few
// sprite lines, the system's memory flushed, no monitor
void memory_maximum()
{
    IData &d = idata();
    lib(L_ClearVar);
    d.DefFlag = 0xFFFE;
    lib(L_DefRun1);
    be_u32 &lines = *(&d.PI_ParaTrap + 4);
    uint32_t kept = lines;
    lines = 20;
    lib(L_DefRun2);
    lines = kept;
    Regs m;
    m.d[0] = 10000 * 1024;          // that is too much: the system gives memory back
    m.a[0] = w::reserve(m.d[0], w::Chip);
    if (!d.Mon_Base)
        mon_free();
}

// The dialogue of MemDelBanks: a length word, then the program
#define DELETE_BANKS_DBL                                                                                   \
    "SI\tSW,SH;"                                                                                          \
    "BA\t0,0;"                                                                                            \
    "BO\t0,0,1,SX,SY;"                                                                                    \
    "PO\t21MECX,4,21ME,0,7;"                                                                              \
    "PO\t22MECX,14,22ME,2,3;"                                                                             \
    "BU\t1,16,SY22-,64,16,0,0,1;[UN 0,0,13BP+; PR 3MECXBP+,4,3ME,7;][BQ;]"                                \
    "KY\t13,0;"                                                                                           \
    "BU\t2,SX72-,YA,XBXA-,YBYA-,0,0,1;[UN 0,0,13BP+; PR 4MECXBP+,4,4ME,7;][BQ;]"                          \
    "KY\t27,0;"                                                                                           \
    "BU\t3,0,0,SX,SY,0,0,0;[][SM;]"                                                                       \
    "RU\t0,3;"                                                                                            \
    "EX;"
alignas(2) const struct {
    uint16_t length;
    char text[sizeof DELETE_BANKS_DBL];
} delete_banks_dbl = {sizeof DELETE_BANKS_DBL, DELETE_BANKS_DBL};

// MemDelBanks: if the program has banks, asks on a line of the screen
// whether to erase them. True if yes.
bool ask_delete_banks()
{
    IData &d = idata();
    Regs l;
    lib_call(L_Bnk_GetLength, l);
    if (!(l.d[0] + l.d[1] + l.d[2]))
        return false;
    const uint8_t *resource = ptr<uint8_t>(d.Sys_Resource);
    uint32_t graphics = address(resource) + peek_l(resource + 2);
    uint32_t messages = address(resource) + peek_l(resource + 2 + 4);
    Regs s;
    s.a[0] = graphics;
    s.d[0] = 10;                    // EcFsel
    s.d[1] = 640;
    s.d[2] = 48;
    s.d[3] = 0;
    s.d[4] = uint32_t(-1);
    lib_call(L_Dia_RScOpen, s);
    if (s.d[0])
        return false;
    recop();
    Regs q;
    for (int i = 0; i < 6; i++)
        q.d[i] = 0;
    q.d[6] = 512;
    q.a[0] = address(&delete_banks_dbl);
    q.a[1] = graphics;
    q.a[2] = messages;
    lib_call(L_Dia_RunQuick, q);
    Regs e;
    e.d[1] = 10;
    ec_call(ec::Del, e);
    return !q.d[0] && uint16_t(q.d[1]) != 2;
}

// Ed_Cold: the editor started, 0 or an error (1: out of memory)
uint32_t editor_cold(uint32_t title)
{
    Regs r;
    r.d[0] = title;
    lib_call(L_Ed_Cold, r);
    return r.d[0];
}

} // namespace

namespace amos::boot {

uint32_t edit_load() { return program_load(idata().Edit_Segment, ed_debut, L_Ed_Start); }
void edit_free() { program_free(idata().Edit_Segment); }

// Mon_Load: the monitor and its resource bank
int32_t mon_load()
{
    IData &d = idata();
    uint32_t banks = d.Cur_Banks;
    int32_t e = 0;
    // (the original kept the file of the bank open on an error)
    program_load(d.Mon_Segment, mon_debut, L_Mon_Start);
    if (!d.Mon_Banks) {
        d.Cur_Banks = address(&d.Mon_Banks);
        sys_add_path(sys_message(11));
        if (!sys_open(ptr<char>(d.Name1), 1005))
            e = -2;                 // file not found
        else
            e = load_banks(-1);
        sys_close();
    }
    d.Cur_Banks = banks;
    if (e)
        mon_free();
    return e;
}

void mon_free()
{
    IData &d = idata();
    uint32_t banks = d.Cur_Banks;
    program_free(d.Mon_Segment);
    if (d.Mon_Banks) {
        d.Cur_Banks = address(&d.Mon_Banks);
        lib(L_Bnk_EffAll);
    }
    d.Cur_Banks = banks;
}

void erase_mouse() { a5_free(idata().PI_AdMouse); }

void erase_fsel()
{
    IData &d = idata();
    d.Cur_Banks = address(&d.Sys_Banks);
    Regs r;
    r.d[0] = 16;
    lib_call(L_Bnk_Eff, r);
}

// LdMouse: the mouse bank of the configuration, if there is one
void load_mouse()
{
    IData &d = idata();
    const char *name = sys_message(2);
    if (!*name)
        return;
    sys_add_path(name);
    if (!sys_open(ptr<char>(d.Name1), 1005))
        the_end(panic_cantread);
    // its size (the original stayed at the end of the file, and could not
    // read it)
    sys_seek(0, OFFSET_END);
    uint32_t size = uint32_t(sys_seek(0, OFFSET_BEGINNING));
    uint32_t bank = a5_reserve(d.PI_AdMouse, size, MemChip | MemPublic | MemClear);
    if (!bank)
        the_end(panic_oomem);
    wdata().MouBank = bank;
    if (!sys_read(ptr<void>(bank), size) || peek_l(ptr<void>(bank)) != 0x416D5370)    // "AmSp"
        the_end(panic_cantread);
    sys_close();
}

// LdFSel: the resource bank of the file selector, bank 16 of the system's
void load_fsel()
{
    IData &d = idata();
    sys_add_path(sys_message(8));
    d.Cur_Banks = address(&d.Sys_Banks);
    if (!sys_open(ptr<char>(d.Name1), 1005))
        the_end(panic_cantread);
    if (load_banks(16))
        the_end(panic_oomem);
    sys_close();
    Regs r;
    r.d[0] = 16;
    lib_call(L_Bnk_GetAdr, r);
    if (peek_l(ptr<void>(r.a[0] - 8)) != 0x5265736F)       // "Reso"
        the_end(panic_cantread);
    d.Sys_Resource = r.a[0];
}

} // namespace amos::boot

extern "C" {

// Edit_Load, Mon_Load: 0 if loaded (BNE: an error in d0). Edit_Free,
// Mon_Free.
uint32_t main_edit_load(RegFrame &r) { return r.d[0] = edit_load(); }
uint32_t main_edit_free(RegFrame &)
{
    edit_free();
    return 0;
}
uint32_t main_mon_load(RegFrame &r) { return r.d[0] = uint32_t(mon_load()); }
uint32_t main_mon_free(RegFrame &)
{
    mon_free();
    return 0;
}

uint32_t main_mem_maximum(RegFrame &)
{
    memory_maximum();
    return 0;
}

// MemDelBanks: d0 = 1 to erase the banks (BNE), else 0
uint32_t main_mem_del_banks(RegFrame &r) { return r.d[0] = ask_delete_banks(); }

// MONITOR
void main_in_monitor()
{
    IData &d = idata();
    if (!first_run())
        return;
    if (!d.Edit_Segment)
        error(13);
    lib(L_Ed_CloseEditor);
    lib(L_Prg_ReSetBanks);
    if (int32_t e = mon_load()) {
        if (int16_t(e) == -1)
            error(24);
        disk_error();
    }
    Regs r;
    lib_call(L_Mon_In_Program, r);
    if (r.d[0])
        error(24);
}

// KILL EDITOR: closes the editor and, if allowed and the monitor is not
// running, takes it out of memory; the program's errors then go to
// RunErr_NoEditor
void main_in_kill_editor()
{
    IData &d = idata();
    if (!first_run() || !d.Edit_Segment)
        return;
    if (d.PI_CloseEd) {
        lib(L_Ed_CloseEditor);
        if (d.PI_KillEd && !d.Mon_Base) {
            lib(L_Ed_KillEditor);
            edit_free();
            mon_free();
            d.Prg_JError = address(main_run_error_no_editor);
            wdata().AMOState = 0;           // run-only
        }
    }
    editor_closed();
}

// CLOSE EDITOR
void main_in_close_editor()
{
    IData &d = idata();
    if (!first_run() || !d.PI_CloseEd || !d.Edit_Segment)
        return;
    lib(L_Ed_CloseEditor);
    editor_closed();
}

// RunErr_RunOnly / RunErr_NoEditor: a program run without the editor
// stopped (e: its error registers, d0-d1/a0-a1). The editor is loaded,
// with all the memory it takes; if there is not enough for it, a run-only
// program ends AMOSPro, else the program's banks go if the user agrees.
[[noreturn]] void main_run_error(uint32_t run_only, Regs *e)
{
    IData &d = idata();
    uint32_t banks = d.Cur_Banks, dialogs = d.Cur_Dialogs;
    if (uint16_t(e->d[0]) == 1002 || !installed())         // System
        the_end(nullptr);
    reset_request();
    bool cold = false;
    if (!edit_load()) {
        uint32_t error = editor_cold(0);
        if (!error)
            cold = true;
        else if (uint8_t(error) != 1)                       // not out of memory
            the_end(panic_editor);
    }
    if (!cold) {
        d.Cur_Dialogs = dialogs;
        d.Cur_Banks = banks;
        memory_maximum();
        if (edit_load() || editor_cold(0)) {
            if (uint16_t(run_only))
                the_end(nullptr);
            d.Cur_Dialogs = dialogs;
            d.Cur_Banks = banks;
            if (!ask_delete_banks())
                the_end(panic_oomem);
            lib(L_Bnk_EffAll);
            lib(L_Bnk_Change);
            if (edit_load() || editor_cold(0))
                the_end(panic_editor);
        }
    }
    if (!d.Edt_Runned)
        d.Edt_Runned = d.Edt_List;
    w_option();
    d.Cur_Dialogs = dialogs;
    d.Cur_Banks = banks;
    interp_jump(e, lib_address(L_Ed_ErrRun));
}

} // extern "C"
