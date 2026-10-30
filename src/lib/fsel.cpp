// The file selector of AMOSPro.Lib (Fsel.s): Fsel$ and the editor's
// requesters for file names.
//
// The selector is an Interface program of the default resource. A task
// of its own (FsI_*) runs the dialog's zones and passes the zone used in
// Fs_Input; the main task reads the directory meanwhile and acts on it.
// The two share the FileSelector structure: Fs_Command (bits: show the
// files, show the stored directories, wait, quit), Fs_Waiting and
// Fs_Input. Directories already read can be kept ("store"), in the
// Fs_Liste list.

#include "core/amiga.h"
#include "lib/dialog.h"
#include "lib/dos.h"
#include "lib/strings.h"

#include <dos/dos.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/tasks.h>

#include <dos/dos.h>

#include "hal/files.h"
#include "hal/system.h"

using namespace amos;

// dirs.cpp, files.cpp, dialog.cpp, convert.cpp
extern "C" uint32_t lib_fill_get(RegFrame &r);
extern "C" uint32_t lib_fill_dev(RegFrame &r);
extern "C" uint32_t lib_fill_first(RegFrame &r);
extern "C" uint32_t lib_fill_nxt(RegFrame &r);
extern "C" uint32_t lib_fill_ffree(RegFrame &r);
extern "C" uint32_t lib_fill_sort(RegFrame &r);
extern "C" uint32_t lib_fill_fpoke(RegFrame &r);
extern "C" uint32_t lib_fill_ffind(RegFrame &r);
extern "C" uint32_t lib_no_req(RegFrame &r);
extern "C" uint32_t lib_yes_req(RegFrame &r);
extern "C" uint32_t lib_lock_get(RegFrame &r);
extern "C" uint32_t lib_copy_path(RegFrame &r);
extern "C" uint32_t lib_dsk_path_it(RegFrame &r);
namespace amos {
uint8_t *lst_new(be_u32 *list, uint32_t length);
void lst_del(be_u32 *list, uint8_t *element);
}
// fsel_glue (dialog_glue.s)
extern "C" uint8_t lib_fs_flush_record[], lib_fs_end_record[];
extern "C" void lib_fs_task_entry();
extern "C" void lib_fs_error_patch();
extern "C" void lib_fs_get_name();
extern "C" uint32_t lib_fs_task_a5;

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint32_t Channel = 0xAABBCCDD;            // Fs_ChannelN
constexpr uint32_t SliderN = 12, ListN = 13, PathN = 14, FileN = 15, SliderS = 18;
constexpr uint32_t EcFsel = 10;
constexpr int MaxStore = 10;

// The variables of the dialog (FsV_*)
enum : int {
    VTitle0 = 0, VTitle1 = 1, VSort = 7, VSize = 8, VPList = 10, VArray = 11, VTx = 12, VTy = 13, VFile = 14,
    VPath = 15, VStore = 16, VPosFirst = 25, VAffFlag = 26, VMax = 27,
};

// Fs_Command
enum : uint16_t { CmdFiles = 1 << 0, CmdStores = 1 << 1, CmdWait = 1 << 2, CmdQuit = 1 << 15 };

FileSelector *fs() { return ptr<FileSelector>(idata().Fs_Base); }
be_i32 *vars() { return ptr<be_i32>(fs()->Fs_Variables); }
uint8_t &var_byte(int v) { return reinterpret_cast<uint8_t *>(&vars()[v])[3]; }
char *name1() { return ptr<char>(idata().Name1); }
char *name2() { return ptr<char>(idata().Name2); }
uint8_t *buffer() { return ptr<uint8_t>(idata().Buffer); }

// The words both tasks change
volatile be_u16 &command() { return fs()->Fs_Command; }
// (The assembler set the bits with one bset)
void send(uint16_t bits)
{
    hal::forbid();
    command() |= bits;
    hal::permit();
}
volatile be_u16 &waiting() { return fs()->Fs_Waiting; }
volatile be_u16 &input() { return fs()->Fs_Input; }

void wait_tof()
{
    boot::wait_vbl_multi();
}

// Fs_Multi: wait for the display (both tasks: no AMOS tests)
void multi() { dia_wait_mul(); }

void call(uint32_t (*fn)(RegFrame &))
{
    RegFrame f = {};
    fn(f);
}

uint8_t upper(uint8_t c) { return c >= 'a' && c <= 'z' ? static_cast<uint8_t>(c - 0x20) : c; }

// Fs_GetPuzzle: the default resource
struct Puzzle {
    const uint8_t *programs, *graphics, *messages;
};
Puzzle puzzle()
{
    const uint8_t *b = ptr<uint8_t>(idata().Sys_Resource);
    return {b + peek_l(b + 10), b + peek_l(b + 2), b + peek_l(b + 6)};
}

// ---- leaving ---------------------------------------------------------------------

// The selector's frame: Fs_Close and Fs_Error come back to it (Fs_Sp)
void **frame;
enum Action { NoAction, Close, DiskError } action;
uint32_t result;

[[noreturn]] void close(uint32_t length)
{
    action = Close;
    result = length;
    __builtin_longjmp(frame, 1);
}

// Fs_Cancel: no file
[[noreturn]] void cancel();

// ---- the dialog task --------------------------------------------------------------

// _AffF: the list of files and its slider. False if the dialog is wrong.
bool show_files()
{
    FileSelector *f = fs();
    f->Fs_ASize = idata().FillFNb;
    uint8_t *list = dia_zone_address(Channel, ListN, 0);
    if (!list)
        return false;
    reinterpret_cast<DiaList *>(list)->Dia_LiMaxAct = f->Fs_ASize;
    vars()[VAffFlag] = 0;
    if (dia_update(Channel, SliderN, static_cast<uint32_t>(vars()[VPList]), EntNul, f->Fs_ASize).error)
        return false;
    vars()[VAffFlag]++;
    return true;
}

// Fs_CptStore: the stored directories
uint16_t stores()
{
    uint16_t n = 0;
    for (uint32_t e = idata().Fs_Liste; e; e = peek_l(ptr<void>(e)))
        n++;
    return n;
}

// _SliStore: the slider of the stored directories
bool show_stores() { return !dia_update(Channel, SliderS, 0, 1, stores() + 1u).error; }

} // namespace

// FsI_Entry: the dialog task (lib_fs_task_entry sets a5 and comes here
// through CCall, which fills this task's context)
extern "C" void lib_fs_task()
{
    for (;;) {
        hal::forbid();
        uint16_t c = command();
        if (c && !(c & CmdQuit))
            command() = 0;
        hal::permit();
        bool wait = false;
        if (c) {
            if (c & CmdQuit)
                break;
            // (On a failure the assembler went on with the main task's
            // stack pointer: the task stops here instead)
            if ((c & CmdFiles && !show_files()) || (c & CmdStores && !show_stores()))
                break;
            if (!(c & CmdWait))
                continue;
            wait = true;
        } else {
            multi();
            dia_auto_test(1);
            DiaResult r = dia_return(Channel);
            if (r.error)
                break;
            input() = static_cast<uint16_t>(r.value);
            if (!input()) {
                if (!(mouse_key() & 2))
                    continue;
                input() = 20;           // the right button
            }
            wait = true;
        }
        if (wait) {
            waiting() = 1;
            while (waiting())
                multi();
        }
    }
    waiting() = 0xFFFF;
}

namespace {

// FsI_Stop / FsI_ReStart / FsI_SliStore / FsI_AffF / FsI_Quit
void task_stop()
{
    if (waiting())
        return;
    send(CmdWait);
    input() = 0;
    while (!waiting())
        wait_tof();
}
void task_restart() { waiting() = 0; }
void task_stores() { send(CmdStores); }
void task_files() { send(CmdFiles); }
void task_quit()
{
    waiting() = 0;
    send(CmdQuit);
    while (!waiting())
        wait_tof();
}

// The task (amiga.lib's CreateTask), with 16K of stack: the assembler gave
// it 2048 bytes, too few for the C++ it runs
void create_task()
{
    static const char name[] = "AMOSPro File Selector";
    constexpr uint32_t stack = 16 * 1024;
    struct Task *t = static_cast<struct Task *>(hal::allocate(sizeof(struct Task), MEMF_PUBLIC | MEMF_CLEAR));
    uint8_t *s = static_cast<uint8_t *>(hal::allocate(stack, MEMF_PUBLIC | MEMF_CLEAR));
    if (!t || !s) {
        if (t)
            hal::release(t, sizeof(struct Task));
        if (s)
            hal::release(s, stack);
        return;
    }
    // Freed with the task
    struct MemList *ml = static_cast<struct MemList *>(
        hal::allocate(sizeof(struct MemList) + sizeof(struct MemEntry), MEMF_PUBLIC | MEMF_CLEAR));
    t->tc_Node.ln_Type = NT_TASK;
    t->tc_Node.ln_Pri = -1;
    t->tc_Node.ln_Name = const_cast<char *>(name);
    t->tc_SPLower = s;
    t->tc_SPUpper = s + stack;
    t->tc_SPReg = s + stack;
    // NewList
    struct List *l = &t->tc_MemEntry;
    l->lh_Head = reinterpret_cast<struct Node *>(&l->lh_Tail);
    l->lh_Tail = nullptr;
    l->lh_TailPred = reinterpret_cast<struct Node *>(&l->lh_Head);
    if (ml) {
        ml->ml_NumEntries = 2;
        ml->ml_ME[0].me_Addr = t;
        ml->ml_ME[0].me_Length = sizeof(struct Task);
        ml->ml_ME[1].me_Addr = s;
        ml->ml_ME[1].me_Length = stack;
        hal::add_head(&t->tc_MemEntry, &ml->ml_Node);
    }
    lib_fs_task_a5 = address(ctx().a5);
    hal::add_task(t, reinterpret_cast<APTR>(lib_fs_task_entry), nullptr);
}

// ---- directories ----------------------------------------------------------------------

// A stored directory: the list of its files, their number, the position
// in the list, the sort flag, then Name1 and Name2
uint8_t *stored_names(uint32_t e) { return ptr<uint8_t>(e) + 8 + 4 + 2 + 4 + 1; }

// Fs_DelStore
void delete_store(uint32_t e)
{
    for (uint32_t f = peek_l(ptr<uint8_t>(e) + 8); f;) {
        uint32_t next = peek_l(ptr<void>(f));
        Regs r = {};
        r.a[1] = f;
        r.d[0] = static_cast<uint32_t>(static_cast<int16_t>(peek_w(ptr<uint8_t>(f) + 4)));
        w::release(r.a[1], r.d[0]);
        f = next;
    }
    lst_del(&idata().Fs_Liste, ptr<uint8_t>(e));
}

void delete_stores()
{
    while (uint32_t e = idata().Fs_Liste)
        delete_store(e);
}

// Fs_NumStore: element n (1-based), or the last one
uint32_t store_number(uint16_t n)
{
    uint32_t e = 0;
    if (!n)
        return 0;
    for (be_u32 *p = &idata().Fs_Liste; *p; p = ptr<be_u32>(*p)) {
        e = *p;
        if (!--n)
            break;
    }
    return e;
}

// Fs_DelLast: keep n
void keep_stores(uint16_t n)
{
    if (uint32_t e = store_number(n))
        while (uint32_t next = peek_l(ptr<void>(e)))
            delete_store(next);
}

// Fs_FindStore: the stored directory Name1/Name2 (any case)
uint32_t find_store()
{
    if (!idata().PI_FsStore)
        return 0;
    for (uint32_t e = idata().Fs_Liste; e; e = peek_l(ptr<void>(e))) {
        const uint8_t *s = stored_names(e);
        bool same = true;
        const char *names[] = {name1(), name2()};
        for (const char *n : names) {
            for (;;) {
                uint8_t a = upper(*s++), b = upper(static_cast<uint8_t>(*n++));
                if (a != b) {
                    same = false;
                    break;
                }
                if (!b)
                    break;
            }
            if (!same)
                break;
        }
        if (same)
            return e;
    }
    return 0;
}

// Fs_Store: keep the directory shown
void store()
{
    IData &d = idata();
    if (d.PI_FsStore && !fs()->Fs_DevFlag && d.BufFillF) {
        if (uint32_t e = find_store())
            delete_store(e);
        keep_stores(MaxStore);
        if (uint8_t *e = lst_new(&d.Fs_Liste, 16 + 128)) {
            uint8_t *p = e + 8;
            poke_l(p, d.BufFillF);
            poke_w(p + 4, d.FillFNb);
            poke_l(p + 6, static_cast<uint32_t>(vars()[VPList]));
            p[10] = d.FillFSorted;
            d.BufFillF = 0;
            d.FillFNb = 0;
            d.FillFSorted = 0;
            char *to = reinterpret_cast<char *>(p + 11);
            for (const char *n = name1(); (*to++ = *n++);) {
            }
            for (const char *n = name2(); (*to++ = *n++);) {
            }
        }
    }
    call(lib_fill_ffree);
}

// Fs_Branch: show stored directory e again
void branch(uint32_t e)
{
    IData &d = idata();
    call(lib_fill_ffree);
    const uint8_t *p = ptr<uint8_t>(e) + 8;
    d.BufFillF = peek_l(p);
    d.FillFNb = peek_w(p + 4);
    vars()[VPList] = static_cast<int32_t>(peek_l(p + 6));
    d.FillFSorted = p[10];
    fs()->Fs_DirOn = 0;
    lst_del(&d.Fs_Liste, ptr<uint8_t>(e));
}

// Fs_NomDir: the path zone, as path (Name1) and filter (Name2)
void path_names()
{
    DiaValue v = dia_value(Channel, PathN, 0);
    if (v.error)
        cancel();
    const char *s = ptr<char>(v.value);
    char *a = name1(), *b = name2();
    *a = *b = 0;
    char *last = a;
    bool wild = false;
    for (;;) {
        char c = *s++;
        *a++ = c;
        if (!c)
            break;
        *a = 0;
        if (c == '*' || c == '?')
            wild = true;
        if ((c == ':' || c == '/') && !wild)
            last = a;
    }
    if (wild) {
        for (const char *f = last; (*b++ = *f++);) {
        }
        *last = 0;
    }
    char *n = name1();
    if (*n) {
        while (*n)
            n++;
        if (n[-1] != ':' && n[-1] != '/') {
            *n++ = '/';
            *n = 0;
        }
    }
    call(lib_dsk_path_it);
}

// An AMOS string at Buffer+512 for the zones
uint8_t *zone_string(uint8_t *end)
{
    uint8_t *s = buffer() + 512;
    poke_w(s, static_cast<uint16_t>(end - (s + 2)));
    return s;
}

// Fs_New: no names any more
void clear_names()
{
    call(lib_fill_ffree);
    fs()->Fs_DirOn = 0;
    fs()->Fs_Click = 0xFFFF;
    vars()[VPList] = 0;
    task_files();
}

// Fs_First: read the directory of the path zone (or take it from the
// stored ones)
void read_directory()
{
    FileSelector *f = fs();
    f->Fs_DevFlag = 0;
    f->Fs_Click = 0xFFFF;
    idata().FillFSorted = vars()[VSort] ? 0xFF : 0;
    vars()[VPosFirst] = -1;
    path_names();
    if (uint32_t e = find_store()) {
        branch(e);
        task_files();
        task_stores();
        return;
    }
    call(lib_lock_get);
    call(lib_fill_first);
    task_stores();
    f->Fs_DirOn = 1;
}

// Fs_Next: one more name of the directory being read
void next_name()
{
    FileSelector *f = fs();
    if (!f->Fs_DirOn)
        return;
    f->Fs_DirOn = 0;
    RegFrame r = {};
    int32_t got = static_cast<int32_t>(lib_fill_nxt(r));
    if (got > 0)
        f->Fs_DirOn = 1;
    if (got == 1)
        return;
    // Sorted: a name poked above the first one shown moves the list down
    if (idata().FillFSorted) {
        int32_t first = vars()[VPosFirst];
        int16_t poke = static_cast<int16_t>(idata().FillFPosPoke);
        if (first >= 0 && poke >= 0 && static_cast<uint16_t>(first) >= static_cast<uint16_t>(poke)) {
            vars()[VPList]++;
            vars()[VPosFirst]++;
        }
    }
    task_files();
}

// Fs_NewDir: the path plus the directory name, then read it
void new_directory(const char *name)
{
    clear_names();
    uint8_t *start = buffer() + 514;
    char *to = reinterpret_cast<char *>(start);
    for (const char *n = name1(); *n;)
        *to++ = *n++;
    while (*name)
        *to++ = *name++;
    const char *filter = name2();
    if (*filter) {
        if (to != reinterpret_cast<char *>(start) && to[-1] != '/' && to[-1] != ':')
            *to++ = '/';
        while ((*to = *filter++))
            to++;
    }
    *to = 0;
    uint8_t *s = zone_string(reinterpret_cast<uint8_t *>(to));
    if (peek_w(s) >= 256)
        cancel();
    dia_update(Channel, PathN, address(s), EntNul, EntNul);
    fs()->Fs_Click = 0xFFFF;
    read_directory();
}

// Fs_NewName: the name zone
void new_name(const char *name)
{
    char *to = reinterpret_cast<char *>(buffer() + 514);
    while ((*to = *name++))
        to++;
    dia_update(Channel, FileN, address(zone_string(reinterpret_cast<uint8_t *>(to))), EntNul, EntNul);
}

// Fs_GetFName: the name zone as path (Buffer+640) and name (Buffer+768)
void file_name(char *&path, char *&name)
{
    path = reinterpret_cast<char *>(buffer() + 640);
    name = path + 128;
    *path = *name = 0;
    const char *s = ptr<char>(dia_value(Channel, FileN, 0).value);
    const char *split = s;
    for (const char *p = s; *p;) {
        char c = *p++;
        if (c == ':' || c == '/')
            split = p;
    }
    if (split != s) {
        char *to = path;
        for (const char *p = s; p != split;)
            *to++ = *p++;
        *to = 0;
    }
    for (char *to = name; (*to++ = *split++);) {
    }
}

// ---- the buttons ---------------------------------------------------------------------

// Fs_Ok: the path and name chosen
[[noreturn]] void ok()
{
    store();
    path_names();
    for (char *a = name1(), *b = ptr<char>(idata().PathAct); (*b++ = *a++);) {
    }
    const char *file = ptr<char>(dia_value(Channel, FileN, 0).value);
    if (!*file)
        cancel();
    char *to = name1();
    while (*to)
        to++;
    while ((*to = *file++))
        to++;
    close(static_cast<uint32_t>(to - name1()));
}

[[noreturn]] void cancel()
{
    store();
    *name1() = 0;
    close(0);
}

// Fs_Help: the list goes to the first file that starts like the name
void help()
{
    char *path, *name;
    file_name(path, name);
    if (!*name)
        return;
    *--name = ' ';
    RegFrame r = {};
    r.a[0] = address(name);
    int16_t pos = -1;
    if (!idata().FillFSorted) {
        r.d[0] = static_cast<uint32_t>(vars()[VPList] + 1);
        pos = static_cast<int16_t>(lib_fill_ffind(r));
    }
    if (pos < 0) {
        RegFrame s = {};
        s.a[0] = address(name);
        pos = static_cast<int16_t>(lib_fill_ffind(s));
    }
    if (pos >= 0) {
        uint16_t ty = static_cast<uint16_t>(vars()[VTy]);
        uint16_t p = static_cast<uint16_t>(pos + ty);
        if (p > idata().FillFNb)
            p = idata().FillFNb;
        p = p >= ty ? static_cast<uint16_t>(p - ty) : 0;
        vars()[VPList] = vars()[VPosFirst] = p;
    }
    task_files();
}

// Return in the name zone: a path in it changes the directory
void name_return()
{
    char *path, *name;
    file_name(path, name);
    if (!*path)
        ok();
    store();
    task_stores();
    new_name(name);
    for (const char *p = path; *p; p++)
        if (*p == ':') {
            *name1() = 0;
            break;
        }
    new_directory(path);
}

// Fs_StoreList: the stored directories as the list
void store_list()
{
    IData &d = idata();
    if (!d.PI_FsStore)
        return;
    store();
    clear_names();
    fs()->Fs_DevFlag = 3;
    d.FillFSorted = 0xFF;
    // A FileInfoBlock at Buffer+128: the name at +8, the size at +124
    uint8_t *fib = buffer() + 128;
    poke_l(fib + 124, 0);
    fib[8] = 0;
    RegFrame p = {};
    p.a[2] = address(fib);
    p.d[2] = ' ';
    lib_fill_fpoke(p);
    d.FillFNb = 0;
    for (uint32_t e = d.Fs_Liste; e; e = peek_l(ptr<void>(e))) {
        char *b = ptr<char>(d.Buffer);
        char *to = b;
        const char *s = reinterpret_cast<const char *>(stored_names(e));
        while ((*to = *s++))
            to++;
        while ((*to++ = *s++)) {
        }
        uint16_t length = static_cast<uint16_t>(to - 1 - b);
        uint16_t room = static_cast<uint16_t>(vars()[VTx]);
        if (length > room)
            length = static_cast<uint16_t>(room - 1);
        const char *from = to - (length + 1);
        for (char *n = reinterpret_cast<char *>(fib + 8); (*n++ = *from++);) {
        }
        // The size field holds the element (no size shown)
        poke_l(fib + 124, (e >> 1) | 0x80000000);
        RegFrame q = {};
        q.a[2] = address(fib);
        q.d[2] = ' ';
        lib_fill_fpoke(q);
    }
    task_files();
    task_stores();
}

// Fs_Dev: the devices or the assigns
void devices(char which, uint8_t flag)
{
    store();
    name1()[0] = which;
    name2()[0] = 0;
    clear_names();
    call(lib_fill_dev);
    fs()->Fs_DevFlag = flag;
    task_files();
    task_stores();
}

// Fs_StoDir2: the stored directory e
void stored_directory(uint32_t e)
{
    const char *s = reinterpret_cast<const char *>(stored_names(e));
    for (char *a = name1(); (*a++ = *s++);) {
    }
    for (char *b = name2(); (*b++ = *s++);) {
    }
    new_directory(s - 1);
}

// A click in the list
void list_click()
{
    DiaValue v = dia_value(Channel, ListN, 0);
    uint16_t n = static_cast<uint16_t>(v.value);
    RegFrame g = {};
    g.d[0] = v.value;
    if (!lib_fill_get(g))
        return;
    char *name = ptr<char>(g.a[0]) + 10;
    FileSelector *f = fs();
    switch (f->Fs_DevFlag) {
    case 1:
    case 2:
        f->Fs_Click = 0xFFFF;
        f->Fs_DevFlag = 0;
        path_names();
        *name1() = 0;
        new_directory(name + 1);
        return;
    case 3:
        f->Fs_Click = 0xFFFF;
        f->Fs_DevFlag = 0;
        stored_directory(peek_l(name - 4) << 1);
        return;
    }
    if (*name++ == ' ') {
        // A file: a second click chooses it
        if (f->Fs_Click == n)
            ok();
        f->Fs_Click = n;
        new_name(name);
        return;
    }
    store();
    path_names();
    new_directory(name);
}

// Fs_Parent
void parent()
{
    path_names();
    char *n = name1();
    if (!*n)
        return;
    char *p = n;
    while (*p)
        p++;
    if (*--p != '/')
        return;
    for (;;) {
        if (p == n)
            break;
        char c = *--p;
        if (c == '/' || c == ':') {
            p++;
            break;
        }
    }
    store();
    task_stores();
    *p = 0;
    new_directory("");
}

// The right button: devices, assigns, stored directories, files
void right_button()
{
    switch (fs()->Fs_DevFlag) {
    case 0:
        devices('D', 1);
        break;
    case 1:
        devices('A', 2);
        break;
    case 2:
        if (idata().PI_FsStore) {
            store_list();
            break;
        }
        [[fallthrough]];
    default:
        path_names();
        new_directory("");
    }
    do
        multi();
    while (mouse_key() & 2);
}

// The zone the task passed (Fs_Jumps; the assembler ran past the end of the
// table for other numbers)
void zone_used(uint16_t zone)
{
    IData &d = idata();
    switch (zone) {
    case 1:
        ok();
    case 2:
        cancel();
    case 3:
        parent();
        break;
    case 4:
        devices('D', 1);
        break;
    case 5:
        devices('A', 2);
        break;
    case 6:
    case 14:
        clear_names();
        read_directory();
        break;
    case 7:
        if (vars()[VSort]) {
            call(lib_fill_sort);
            d.FillFSorted = 0xFF;
            task_files();
        }
        break;
    case 8:
        task_files();
        break;
    case 13:
        list_click();
        break;
    case 15:
        name_return();
        break;
    case 16:
        d.PI_FsStore = var_byte(VStore);
        if (!d.PI_FsStore) {
            delete_stores();
            task_stores();
        }
        break;
    case 17:
        if (d.PI_FsStore && d.Fs_Liste) {
            stored_directory(store_number(126));
            task_stores();
        }
        break;
    case 18:
        store_list();
        break;
    case 19:
        help();
        break;
    case 20:
        right_button();
        break;
    }
}

// ---- opening -------------------------------------------------------------------------

// Fs_ScOpen
uint8_t *open_screen(int16_t w, int16_t h)
{
    uint32_t error;
    return dia_resource_screen(puzzle().graphics, EcFsel, static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1, -1,
                               &error);
}

// Fs_Appear / Fs_DisAppear: the screen grows from its middle line
void appear(bool show)
{
    IData &d = idata();
    Screen *s = ptr<Screen>(fs()->Fs_AdEc);
    Regs a = {};
    a.a[2] = address(s);
    if (show) {
        s->EcAWX = d.PI_FsDWx;
        reinterpret_cast<uint8_t *>(&s->EcAW)[0] |= 1 << 1;
        a.d[7] = d.PI_FsDVApp;
        a.d[6] = 1;
        a.d[5] = static_cast<uint16_t>(d.PI_FsDWy + (s->EcTy >> 1));
    } else {
        a.d[7] = static_cast<uint16_t>(-d.PI_FsDVApp);
        a.d[6] = s->EcTy >> 1;
        a.d[5] = static_cast<uint16_t>(d.PI_FsDWy + a.d[6]);
    }
    lib_call(L_AppCentre, a);
}

// Fs_GetInputs: the parameters (title 2 on top of the stack)
void inputs(const be_u32 *params)
{
    be_i32 *v = vars();
    const uint8_t *t1 = ptr<uint8_t>(params[0]), *t0 = ptr<uint8_t>(params[1]);
    if (t1[1])
        v[VTitle1] = static_cast<int32_t>(params[0]);
    if (t0[1])
        v[VTitle0] = static_cast<int32_t>(params[1]);
    v[VFile] = static_cast<int32_t>(params[2]);
    const uint8_t *path = ptr<uint8_t>(params[3]);
    char *to = name1();
    for (uint16_t i = 0, n = str_len(path); i < n; i++)
        *to++ = static_cast<char>(str_chars(path)[i]);
    *to = 0;
    call(lib_dsk_path_it);
    char *p = reinterpret_cast<char *>(buffer() + 514);
    for (const char *n = name1(); (*p++ = *n++);) {
    }
    v[VPath] = static_cast<int32_t>(address(zone_string(reinterpret_cast<uint8_t *>(p - 1))));
}

uint32_t avail_public() { return hal::available_memory(hal::MemoryPublic); }

// Fs_LowMemory: a small selector without its task
[[noreturn]] void low_memory(const be_u32 *params)
{
    static uint16_t palette[32 + 2];
    Puzzle p = puzzle();
    const uint8_t *pal = p.graphics + 6 + (peek_w(p.graphics) << 2);
    for (int i = 0; i < 32; i++)
        palette[i] = peek_w(pal + 2 * i);
    palette[0] = palette[2];
    palette[1] = palette[3];
    Regs c = {};
    c.d[1] = EcFsel;
    c.d[2] = static_cast<uint32_t>(static_cast<int16_t>(idata().PI_FsDSx));
    c.d[3] = 64;
    c.d[4] = 1;
    c.d[5] = 0x8000;
    c.d[6] = 2;
    c.d[7] = 0;
    c.a[1] = address(palette);
    ec_call(ec::Cree, c);
    if (c.d[0])
        cancel();
    FileSelector *f = fs();
    f->Fs_AdEc = c.a[0];
    appear(true);
    Regs pr = {};
    pr.a[1] = address("\x1b" "C0");
    wi_call(wi::Print, pr);
    f->Fs_Opened = 1;
    DiaOpen o = dia_open(Channel, p.programs + peek_l(p.programs + 14), p.graphics, p.messages, 1024, 32, false);
    if (o.error)
        cancel();
    f->Fs_Channel = -1u;    // no task
    f->Fs_Variables = address(dia_vars(o.d));
    inputs(params);
    DiaResult r = dia_run(Channel, -1, 0, 0);
    if (r.error)
        cancel();
    if (r.value == 1)
        ok();
    cancel();
}

// Fs_Error: a disk error while reading: show it, go on
void show_disk_error()
{
    task_stop();
    static const int16_t known[] = {204, 205, 210, 213, 218, 225, 226};
    int32_t io = hal::io_error();
    uint32_t message = 32;
    for (unsigned i = 0; i < sizeof known / sizeof known[0]; i++)
        if (known[i] == io)
            message = 25 + i;
    Puzzle p = puzzle();
    Regs m = {};
    m.a[0] = address(p.messages);
    m.d[0] = message;
    m.a[0] = address(boot::get_message(ptr<uint8_t>(m.a[0]), m.d[0]));
    clear_names();
    DiaResult r = dia_run_quick(p.programs + peek_l(p.programs + 10), p.graphics, p.messages, 0, 0, 0,
                                m.a[0] - 2, 0, false, 256);
    task_restart();
    if (r.error)
        cancel();
    input() = 0;
}

void selector(const be_u32 *params)
{
    IData &d = idata();
    FileSelector *f = fs();
    d.Fs_ErrPatch = address(lib_fs_error_patch);
    Screen *current = ptr<Screen>(wdata().EcCourant);
    f->Fs_OldEc = current ? current->EcNumber : 0xFFFF;
    // The mouse stays on the selector
    WData &w = wdata();
    be_u16 *limits = &w.MouXMin;
    for (int i = 0; i < 4; i++)
        f->Fs_LimSave[i] = limits[i];
    Regs l = {};
    l.d[1] = 128;
    l.d[2] = 30;
    l.d[3] = 448;
    l.d[4] = 312;
    sy_call(sy::LimitM, l);
    if (avail_public() < 32 * 1024)
        low_memory(params);
    uint8_t *s = open_screen(static_cast<int16_t>(d.PI_FsDSx), static_cast<int16_t>(d.PI_FsDSy));
    if (!s)
        s = open_screen(320, 128);
    if (!s)
        low_memory(params);
    f->Fs_AdEc = address(s);
    Regs pr = {};
    pr.a[1] = address("\x1b" "C0");
    wi_call(wi::Print, pr);
    if (avail_public() < 12 * 1024)
        low_memory(params);
    Puzzle p = puzzle();
    DiaOpen o = dia_open(Channel, p.programs + peek_l(p.programs + 6), p.graphics, p.messages, 256 * 5, VMax, false);
    if (o.error)
        cancel();
    f->Fs_Channel = address(o.d);
    f->Fs_Variables = address(dia_vars(o.d));
    o.d->Dia_Flags |= 1 << 4;          // Return does not leave the edit zones
    inputs(params);
    var_byte(VSort) = d.PI_FsSort;
    var_byte(VSize) = d.PI_FsSize;
    var_byte(VStore) = d.PI_FsStore;
    // The list's "array": a routine gives the names
    f->Fs_Array = 1;
    reinterpret_cast<uint8_t *>(&f->Fs_Array)[1] = 2;
    f->Fs_ASize = 0;
    f->Fs_AMagic = 0x4D67496B;
    f->Fs_ACall = address(lib_fs_get_name);
    vars()[VArray] = static_cast<int32_t>(address(&f->Fs_Array));
    if (dia_run(Channel, -1, 0, 0).error)
        cancel();
    task_stores();
    appear(true);
    f->Fs_Opened = 1;
    create_task();
    call(lib_no_req);
    if (!*ptr<char>(d.PathAct)) {
        boot::ask_dir();
        call(lib_copy_path);
    }
    call(lib_fill_ffree);
    d.FillFSize = 64;
    d.FillF32 = 0;
    read_directory();
}

void events()
{
    for (;;) {
        if (fs()->Fs_DirOn)
            next_name();
        else
            wait_tof();
        if (static_cast<int16_t>(waiting()) < 0)
            cancel();
        if (uint16_t zone = input()) {
            zone_used(zone);
            waiting() = 0;
            input() = 0;
        }
    }
}

// Fs_Close: all back as it was
void close_selector()
{
    IData &d = idata();
    FileSelector *f = fs();
    store();
    call(lib_yes_req);
    d.Fs_ErrPatch = 0;
    if (f->Fs_Channel) {
        if (f->Fs_Channel != -1u) {
            task_quit();
            d.PI_FsSort = var_byte(VSort);
            d.PI_FsSize = var_byte(VSize);
            d.PI_FsStore = var_byte(VStore);
        }
        dia_close(Channel);
    }
    if (f->Fs_Opened) {
        Screen *s = ptr<Screen>(f->Fs_AdEc);
        d.PI_FsDWx = s->EcAWX;
        d.PI_FsDWy = s->EcAWY;
        appear(false);
    }
    if (f->Fs_AdEc) {
        Regs del = {};
        del.d[1] = EcFsel;
        ec_call(ec::Del, del);
        if (static_cast<int16_t>(f->Fs_OldEc) >= 0) {
            Regs act = {};
            act.d[1] = f->Fs_OldEc;
            ec_call(ec::Active, act);
        }
    }
    be_u16 *limits = &wdata().MouXMin;
    for (int i = 0; i < 4; i++)
        limits[i] = f->Fs_LimSave[i];
    dia_clear_key();
    Regs fr = {};
    fr.a[1] = address(f);
    fr.d[0] = FileSelector::Fs_Long;
    w::release(fr.a[1], fr.d[0]);
    d.Fs_Base = 0;
}

// Dsk.FileSelector: params = title 2, title 1, default name, path. Returns
// the length of the name chosen (in Name1), 0 for none.
uint32_t file_selector(const be_u32 *params)
{
    IData &d = idata();
    if (d.Patch_Errors)
        fon_call();
    Regs fl = {};
    fl.a[1] = address(lib_fs_flush_record);
    w::add_routine(fl.a[1], address(&wdata().MemFlush));
    add_routine(lib_fs_end_record, address(&d.Sys_EndRoutines));
    Regs m = {};
    m.d[0] = FileSelector::Fs_Long;
    m.a[0] = w::reserve(m.d[0], w::FastClear);
    // (Without memory the assembler cancelled through a null structure)
    if (!m.a[0]) {
        *name1() = 0;
        return 0;
    }
    d.Fs_Base = m.a[0];
    void *buf[5];
    frame = buf;
    action = NoAction;
    for (;;) {
        if (!__builtin_setjmp(buf)) {
            if (action == NoAction)
                selector(params);
            events();
        }
        if (action == Close)
            break;
        // A disk error: show it and go on
        action = NoAction;
        show_disk_error();
        action = DiskError;
    }
    close_selector();
    return result;
}

} // namespace

// Fs_Error: AMOS's disk errors come here while the selector runs
extern "C" void lib_fs_error()
{
    action = DiskError;
    __builtin_longjmp(frame, 1);
}

// The flush and end records (low memory / the end of AMOS): forget the
// stored directories
extern "C" void lib_fs_flush()
{
    delete_stores();
    idata().PI_FsStore = 0;
}
extern "C" void lib_fs_end() { delete_stores(); }

// Fs_GetName: the list's line d4. Out: a1 = the text, d1 = its length.
extern "C" uint32_t lib_fs_get_name_c(RegFrame &r)
{
    RegFrame g = {};
    g.d[0] = r.d[4];
    if (!lib_fill_get(g))
        return r.d[0] = 0;
    char *name = ptr<char>(g.a[0]) + 10;
    uint32_t size = peek_l(ptr<uint8_t>(g.a[0]) + 6);
    be_i32 *v = ptr<be_i32>(fs()->Fs_Variables);
    r.a[1] = address(name);
    if (!v[VSize] || size & 0x80000000 || *name == '*') {
        uint32_t n = 0;
        while (name[n])
            n++;
        r.d[1] = n;
        return r.d[0] = 1;
    }
    // The size at the right
    char *line = reinterpret_cast<char *>(buffer() + 896);
    char *to = line;
    while ((*to = *name++))
        to++;
    char *size_at = line + static_cast<int16_t>(v[VTx]) - 8;
    while (to < size_at)
        *to++ = ' ';
    to = size_at;
    *to++ = ' ';
    Regs c = {};
    c.d[0] = size;
    c.a[0] = address(to);
    lib_call(L_LongToDec, c);
    to = ptr<char>(c.a[0]);
    while (to < size_at + 8)
        *to++ = ' ';
    r.a[1] = address(line);
    r.d[1] = static_cast<uint32_t>(to - line);
    return r.d[0];
}

// Dsk.FileSelector: the four parameters on the a3 stack, which it pops.
// Out: d0 = length, a0 = Name1.
extern "C" uint32_t lib_dsk_file_selector(RegFrame &r)
{
    const auto *params = reinterpret_cast<const be_u32 *>(ctx().a3);
    uint32_t n = file_selector(params);
    ctx().a3 += 16;
    r.a[0] = idata().Name1;
    return r.d[0] = n;
}

// =FSEL$(path$[,default$[,title1$[,title2$]]])
extern "C" uint32_t lib_fn_file_selector4(uint32_t path, uint32_t name, uint32_t t1, uint32_t t2)
{
    const be_u32 params[4] = {be_u32(t2), be_u32(t1), be_u32(name), be_u32(path)};
    uint32_t n = file_selector(params);
    return n ? str_from(ptr<char>(idata().Name1), static_cast<uint16_t>(n)) : str_empty();
}
extern "C" uint32_t lib_fn_file_selector3(uint32_t p, uint32_t n, uint32_t t) { return lib_fn_file_selector4(p, n, t, idata().ChVide); }
extern "C" uint32_t lib_fn_file_selector2(uint32_t p, uint32_t n) { return lib_fn_file_selector4(p, n, idata().ChVide, idata().ChVide); }
extern "C" uint32_t lib_fn_file_selector1(uint32_t p) { return lib_fn_file_selector2(p, idata().ChVide); }
