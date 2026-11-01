// The start and the end of AMOSPro (+B.s Cold_Start, Init_Fin, TheEnd):
// the command line or the Workbench message, AMOS.library, the data zone,
// the configuration, the system libraries, the libraries and extensions,
// the automatic assigns; then the program to run or the editor. At the
// end, everything is given back in the reverse order. The stack parts
// (the stack of its own, the way out) stay in +B.s.

#include "main/main.h"

#include "core/amiga.h"

#include <dos/dosextens.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>

// The system libraries, in variables: some are used before the data zone
// exists, or after it is freed
namespace {
Library *dos_base, *intuition_base, *icon_base;
}
#include "hal/files.h"
#include "hal/workbench.h"
#include "hal/system.h"


using namespace amos;
using namespace amos::boot;


extern "C" {
// +W.s: the start of AMOS.library, its entry table

// +B.s: the way out: the stack back, the Workbench message (if any)
// replied
[[noreturn]] void main_get_out(uint32_t message);
// main_glue.s: where the errors of a program run at the start go

}

namespace amos::boot {

const char panic_oomem[] = "Not enough free memory";
const char panic_editor[] = "Cannot load editor";
const char panic_install[] = "Program not installed, start \"Install.AMOS\" first";
const char panic_apsystem[] = "Cannot find APSystem folder.";
const char panic_cantread[] = "Cannot read system files: check APSystem folder.";
const char panic_badext[] = "Cannot load extension: use default interpreter config.";
bool run_only;

} // namespace amos::boot

namespace {

const char panic_config[] = "Cannot find AMOSPro_Interpreter_Config.";
const char panic_command[] = "Bad command line.";
const char panic_noprogram[] = "Run only (-r): no program to run.";
const char panic_version[] = "I need AMOS.library V2.0 or over!";

// The message shown when AMOSPro stops
const char *panic;

constexpr uint32_t TBuffer = 1024;
constexpr uint32_t AAreaSize = 16;
// The work buffers below the AMOS.library data
constexpr uint32_t LDataWork = 32 + Bob::BbLong + 12 * 2 + (AAreaSize * 5 + 6) + 24 + 16 + 12 + 70 * 3 + 4 +
                               20 + 108 + 108 + 256 + TBuffer;

// What the start finds before the data zone exists (the SP_ buffer)
struct Start {
    WBStartup *message = nullptr;
    BPTR auto_lock = 0;
    char auto_name[128] = {};
    char config[128] = {};
    bool workbench = true;
    const char *command_line = nullptr;
};

// The registers of a call with all of them: the context's d6-d7/a3-a6
AllRegs context_regs()
{
    Ctx &c = ctx();
    AllRegs r = {};
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    r.a[3] = address(c.a3);
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = address(c.a6);
    return r;
}

// Panic_Message: the message of the panic, in the CLI's window or in a
// requester under the Workbench
void panic_message(bool from_workbench)
{
    if (!panic)
        return;
    if (!from_workbench) {
        BPTR out = hal::output();
        if (!out)
            return;
        char line[128];
        char *to = line;
        for (const char *p = panic; (*to = *p++); to++)
            ;
        *to++ = 10;
        *to = 0;
        hal::write(out, line, to - line);
        return;
    }
    hal::panic_requester(panic);
}

} // namespace

namespace amos::boot {
// (the host's exit status: 1 when AMOSPro stopped with a message)
int exit_status() { return panic ? 1 : 0; }
} // namespace amos::boot

namespace {

// Boot_Fatal: an error before the data zone
[[noreturn]] void fatal(const Start &s, const char *message)
{
    panic = message;
    panic_message(s.message);
    hal::close_library(dos_base);
    hal::close_library(intuition_base);
    main_get_out(address(s.message));
}

// .CliNom: a name of the command line, maybe in quotes
const char *command_name(const char *p, char *to)
{
    char end = ' ';
    if (*p == '"' || *p == '\'')
        end = *p++;
    char *last = to + 127;      // (the original had no limit)
    while (char c = *p) {
        p++;
        if (c == end)
            break;
        if (to < last)
            *to++ = c;
    }
    *to = 0;
    return p;
}

// The command line: [-c config] [-w] [-r] [program [its command line]]
// (-r: run only, a deviation)
void command_line(Start &s, char *p, uint32_t length)
{
    if (uint16_t(length) <= 1)
        return;
    p[uint16_t(length) - 1] = 0;
    for (;;) {
        char c = *p++;
        if (!c)
            return;
        if (c == ' ')
            continue;
        if (c != '-') {
            p = const_cast<char *>(command_name(p - 1, s.auto_name));
            while (*p++ == ' ')
                ;
            s.command_line = p - 1;
            return;
        }
        c = *p++;
        if (c >= 'a' && c <= 'z')
            c = char(c - 0x20);
        if (c == 'C')
            p = const_cast<char *>(command_name(p, s.config));
        else if (c == 'W')
            s.workbench = false;
        else if (c == 'R')
            boot::run_only = true;
        else
            fatal(s, panic_command);
    }
}

// The Workbench's message: the program's icon's directory, the program to
// load if an icon was given with it
void workbench_message(Start &s, Process *process)
{
    hal::wait_port(&process->pr_MsgPort);
    s.message = reinterpret_cast<WBStartup *>(hal::get_message(&process->pr_MsgPort));
    int32_t lock = -1;
    WBArg *args = s.message->sm_ArgList;
    if (args) {
        lock = int32_t(args[0].wa_Lock);
        if (lock)
            hal::current_dir(BPTR(lock));
    }
    if (s.message->sm_NumArgs != 2 || !args)
        return;
    s.auto_lock = args[1].wa_Lock;
    if (const char *name = reinterpret_cast<const char *>(args[1].wa_Name)) {
        char *to = s.auto_name;
        while ((*to++ = *name++))
            ;
        if (!lock && s.auto_lock)
            hal::current_dir(s.auto_lock);
    }
}

// The interpreter's configuration: "PId1" its values, "PIt1" its texts
void load_config(const Start &s)
{
    IData &d = idata();
    static const char *const names[] = {"AMOSPro_Interpreter_Config", "s/AMOSPro_Interpreter_Config",
                                        "s:AMOSPro_Interpreter_Config"};
    bool open = false;
    if (*s.config) {
        open = sys_open(s.config, 1005);
    } else {
        for (const char *name : names)
            if ((open = sys_open(name, 1005)))
                break;
    }
    if (!open)
        the_end(panic_config);
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    if (!sys_read(buffer, 8) || peek_l(buffer) != 0x50496431)      // "PId1"
        the_end(panic_cantread);
    if (!sys_read(reinterpret_cast<uint8_t *>(&d) + IData::PI_Start, peek_l(buffer + 4)))
        the_end(panic_cantread);
    sys_read(buffer, 8);
    if (peek_l(buffer) != 0x50497431)                               // "PIt1"
        the_end(panic_cantread);
    uint32_t texts = a5_reserve(d.Sys_Messages, peek_l(buffer + 4), MemClear | MemPublic);
    if (!texts)
        the_end(panic_oomem);
    sys_read(ptr<void>(texts), peek_l(buffer + 4));
    sys_close();
}

// RExist: Name1 exists
bool name1_exists()
{
    Regs r;
    lib_call(L_RExist, r);
    return r.d[3];
}

char *copy(char *to, const char *from)
{
    while ((*to++ = *from++))
        ;
    return to - 1;
}

// DoAssign: name (then its folder, next to APSystem) assigned with
// c:assign, unless it exists already or the folder does not. 1 if done.
int assign(const char *name, BPTR nil)
{
    IData &d = idata();
    char *name1 = ptr<char>(d.Name1);
    copy(name1, name);
    if (name1_exists())
        return 0;
    const char *folder = name;
    while (*folder++)
        ;
    char *to = copy(name1, reinterpret_cast<const char *>(d.Sys_Pathname));
    *to++ = '/';                // up a folder
    copy(to, folder);
    if (!name1_exists())
        return 0;
    char *buffer = ptr<char>(d.Buffer);
    to = copy(copy(buffer, "c:assign "), name);
    *to++ = ' ';
    *to++ = '"';
    to = copy(to, name1);
    *to++ = '"';
    *to = 0;
    hal::execute(buffer, nil, nil);
    return 1;
}

// AutoAssigns: the folders of AMOSPro, if c:assign is there
void auto_assigns()
{
    IData &d = idata();
    copy(ptr<char>(d.Name1), "c:assign");
    if (!name1_exists())
        return;
    Regs a;
    lib_call(L_AskDir, a);
    BPTR nil = hal::open("NIL:", hal::ModeReadWrite);
    if (!nil)
        return;
    d.ParamE = uint32_t(nil);
    static const char *const folders[] = {
        "AMOSPro_Accessories:\0Accessories", "AMOSPro_Productivity1:\0Productivity1",
        "AMOSPro_Productivity2:\0Productivity2", "AMOSPro_Tutorial:\0Tutorial",
        "AMOSPro_Examples:\0Examples", "AMOSPro_Compiler:\0Compiler", "AMOSPro_Extras:\0Extras"};
    int done = 0;
    for (const char *f : folders)
        done += assign(f, nil);
    if (done)
        assign("AMOSPro_System:\0", nil);
    hal::close(nil);
}

// The data zone: the work buffers, the AMOS.library's data, the
// interpreter's. Returns a5.
uint8_t *data_zone(uint32_t w_data)
{
    uint32_t size = LDataWork + w_data + IData::DataLong;
    uint8_t *zone = static_cast<uint8_t *>(hal::allocate(size, MemPublic | MemClear));
    if (!zone)
        return nullptr;
    uint8_t *a5 = zone + LDataWork + w_data;
    ctx().a5 = a5;
    IData &d = idata();
    d.Sys_AData = address(zone);
    d.Sys_LData = size;
    uint32_t p = address(zone + LDataWork);
    auto buffer = [&p](void *field, uint32_t length) {
        p -= length;
        poke_l(field, p);
    };
    buffer(&d.BufLabel, 32);
    buffer(&d.BufBob, Bob::BbLong);
    buffer(&d.MnTDraw, 12 * 2);
    buffer(&d.AAreaBuf, AAreaSize * 5 + 6);
    buffer(&d.AAreaInfo, 24);
    buffer(&d.BufAMSC, 16);
    buffer(&d.BufCCRT, 12);
    buffer(&d.BufCMAP, 70 * 3);
    buffer(&d.BufCAMG, 4);
    buffer(&d.BufBMHD, 20);
    buffer(&d.DirFNeg, 108);
    buffer(&d.Name2, 108);
    buffer(&d.Name1, 256);
    buffer(&d.Buffer, TBuffer);
    if (d.Buffer & 3)
        halt();
    return a5;
}

} // namespace

extern "C" {

// Cold_Start, from its stack (+B.s): command/length of a CLI, sp the stack
// of the interpreter (BasSp)
void main_cold_start(char *command, uint32_t length, uint32_t sp)
{
    Start s;
    dos_base = hal::open_library("dos.library", 0);
    intuition_base = hal::open_library("intuition.library", 0);
    hal::files_start(dos_base);
    hal::workbench_start(intuition_base, nullptr);

    auto process = reinterpret_cast<Process *>(hal::find_task(nullptr));
    if (!process->pr_CLI)
        workbench_message(s, process);
    else
        command_line(s, command, length);

    // AMOS.library, linked in (it was loaded from APSystem or libs)
    uint32_t w = address(w_debut);
    ctx().a4 = ptr<uint8_t>(w);

    if (!data_zone(peek_l(ptr<void>(w + 8))))
        fatal(s, panic_oomem);
    IData &d = idata();
    d.Sys_Message = address(s.message);
    d.DosBase = address(dos_base);
    wdata().IntBase = address(intuition_base);

    // the AMOS.library's system start
    d.Sys_WAd = w;
    wdata().CBridge = address(amos_bridge);
    Regs v;
    v.d[0] = 0x56322E30;        // "V2.0"
    v.d[1] = 0x0200;
    amos_call(&v, ptr<void>(w + 12));
    if (v.d[0] != 0x57322E30)   // "W2.0"
        the_end(panic_version);

    uint16_t version = hal::os_version();
    if (version >= 36)
        d.WB2_0 = version;

    load_config(s);
    if (!a5_reserve(d.PathAct, 256, MemPublic | MemClear))
        the_end(panic_oomem);

    // the full path of the system folder
    BPTR lock = hal::lock(sys_message(1), hal::LockShared);
    if (!lock)
        the_end(panic_apsystem);
    ask_dir2(uint32_t(lock));
    copy(reinterpret_cast<char *>(d.Sys_Pathname), ptr<char>(d.Buffer + 384));

    d.FloatBase = address(hal::open_library("mathffp.library", 0));
    if (!(wdata().GfxBase = address(hal::open_library("graphics.library", 0))))
        the_end(panic_cantread);
    if (d.PI_Icons && (icon_base = hal::open_library("icon.library", 0))) {
        hal::workbench_start(intuition_base, icon_base);
        d.IconBase = address(icon_base);
        sys_add_path(sys_message(4));
        if (DiskObject *icon = hal::get_icon(ptr<char>(d.Name1))) {
            d.AdrIcon = address(icon);
            icon->do_CurrentX = NO_ICON_POSITION;
            icon->do_CurrentY = NO_ICON_POSITION;
        }
    }

    if (libraries_load())
        the_end(panic_badext);
    library_patch();
    if (libraries_reloc())
        the_end(panic_badext);
    d.Sys_LibStarted = 1;

    load_mouse();
    load_fsel();
    auto_assigns();

    // the interrupts: the AMOS.library started
    AllRegs r = context_regs();
    r.a[3] = address(sys_message(3));      // the font
    r.a[0] = address(&d.PI_ParaTrap);
    r.a[1] = d.PI_AdMouse;
    r.a[2] = address(d.PI_DefEPa);
    r.d[0] = 0;
    r.d[1] = d.PI_DefAmigA;
    r.a[4] = w;
    r.a[5] = address(ctx().a5);
    r.a[6] = d.Sys_WAd;
    amos_call_all(&r, ptr<void>(d.Sys_WAd));
    if (r.d[0])
        the_end(nullptr);
    d.Sys_WStarted = 0xFF;

    if (libraries_init())
        the_end(panic_badext);
    *ptr<char>(d.PathAct) = 0;

    // the program to load: the one asked for, or the autoexec
    char *name1 = ptr<char>(d.Name1);
    poke_w(name1, 0);
    if (s.auto_lock)
        hal::current_dir(s.auto_lock);
    copy(name1, *s.auto_name ? s.auto_name : sys_message(5));
    d.WBench = s.workbench;
    // run only: the program's folder the current one (its files are there)
    if (run_only && *s.auto_name) {
        char folder[128];
        char *to = folder, *end = folder;
        for (const char *p = s.auto_name; *p && to < folder + sizeof folder - 1; p++) {
            *to++ = *p;
            if (*p == '/' || *p == ':')
                end = to;
        }
        *end = 0;
        if (*folder)
            if (hal::DirLock l = hal::lock(folder, hal::LockShared))
                hal::current_dir(l);
    }

    // its command line, for COMMAND LINE$
    if (const char *line = s.command_line) {
        uint8_t *to = ptr<uint8_t>(d.Buffer + TBuffer - 256 - 6);
        poke_l(to, 0x436D644C);                // "CmdL"
        char *end = copy(reinterpret_cast<char *>(to + 6), line);
        poke_w(to + 4, uint16_t(end - (reinterpret_cast<char *>(to + 6))));
    }

    d.BasSp = sp;

    // the memory there is
    if (!d.WB2_0) {
        // under 1.3, about it
        uint32_t chip = hal::available_memory(MemPublic | MemChip) + 38 * 1024;
        uint32_t fast = hal::available_memory(MemPublic | MemFast);
        if (fast)
            fast += 256 * 1024;
        else
            chip += 256 * 1024;
        d.MemFastTotal = fast;
        d.MemChipTotal = chip;
    } else {
        d.MemChipTotal = hal::available_memory(MemPublic | MemChip | hal::MemoryTotal);
        d.MemFastTotal = hal::available_memory(MemPublic | MemFast | hal::MemoryTotal);
    }

    Regs a;
    lib_call(L_AskDir, a);
    Regs c;
    lib_call(L_CopyPath, c);
    if (d.PI_AutoWB) {
        w_option();
        Regs b;
        lib_call(L_WB_Close, b);
    }
}

// Init_Fin, the init hunk freed (+B.s): the program asked for (or the
// autoexec) is run, else the editor starts
[[noreturn]] void main_init_fin()
{
    IData &d = idata();
    AllRegs r = context_regs();
    r.d[0] = d.PI_DefSize;
    amos_call_all(&r, lib_address(L_Prg_NewStructure));
    uint32_t program = r.d[0];
    if (!program)
        the_end(panic_oomem);
    auto call = [program](int routine, uint32_t d0) {
        AllRegs p = context_regs();
        p.d[0] = d0;
        p.a[6] = program;
        amos_call_all(&p, lib_address(routine));
        return p.d[0];
    };
    call(L_Prg_New, uint32_t(-2));                      // no screen behind
    if (!call(L_Prg_Load, uint32_t(-1))) {
        AllRegs p = context_regs();
        // (as an accessory, without the default screen; run only, as the
        // editor's Run: the default screen opened)
        p.d[0] = boot::run_only ? 0 : uint32_t(-1);
        p.a[1] = address(main_run_error_run_only);
        p.a[6] = program;
        amos_call_all(&p, lib_address(L_Prg_RunIt));    // back if out of memory
    }
    if (boot::run_only)
        the_end(panic_noprogram);
    // no program: the editor
    if (!installed())
        the_end(panic_install);
    if (edit_load())
        the_end(panic_editor);
    call(L_Prg_New, uint32_t(-1));
    Regs c;
    c.d[0] = uint32_t(-1);                              // the title
    lib_call(L_Ed_Cold, c);
    if (c.d[0])
        the_end(panic_editor);
    w_option();
    Regs t;
    lib_call(L_Ed_Title, t);
    amos_goto(lib_address(L_Ed_Loop), 0);
}

// TheEnd: the general way out
[[noreturn]] void main_the_end() { the_end(nullptr); }

} // extern "C"

namespace amos::boot {

[[noreturn]] void the_end(const char *message)
{
    if (message)
        panic = message;
    IData &d = idata();
    if (d.Sys_LibStarted) {
        Regs e;
        e.a[1] = address(d.Sys_EndRoutines);
        w::call_routines(ptr<be_u32>(e.a[1]));
        static const int routines[] = {L_CloAll, L_PRT_Close, L_Equ_Free, L_Includes_Clear};
        for (int routine : routines) {
            Regs r;
            lib_call(routine, r);
        }
        if (d.Edit_Segment) {
            Regs r;
            lib_call(L_Ed_End, r);
        }
        // the edition structures left (after KILL EDITOR)
        for (uint32_t edt = d.Edt_List; edt;) {
            uint32_t next = ptr<EditWindow>(edt)->Edt_Next;
            ram_free(edt, EditWindow::Edt_Long);
            edt = next;
        }
        while (uint32_t program = d.Prg_List) {
            AllRegs p = context_regs();
                p.a[6] = program;
            amos_call_all(&p, lib_address(L_Prg_DelStructure));
        }
        edit_free();
        mon_free();
        erase_fsel();
        erase_mouse();
        Regs w;
        lib_call(L_WB_Open, w);
        libraries_stop();
    }
    sys_close();
    if (d.Sys_WStarted) {
        Regs r;
        amos_call(&r, ptr<void>(d.Sys_WAd + 4));
    }
    a5_free(d.Sys_Messages);
    a5_free(d.PathAct);
    icon_base = ptr<Library>(d.IconBase);
    if (d.AdrIcon)
        hal::free_icon(ptr<DiskObject>(d.AdrIcon));
    if (icon_base)
        hal::close_library(icon_base);
    if (d.FloatBase)
        hal::close_library(ptr<Library>(d.FloatBase));
    math_close();
    libraries_free();
    // the AMOS.library out
    Regs r;
    amos_call(&r, ptr<void>(d.Sys_WAd + 16));
    dos_base = ptr<Library>(d.DosBase);
    intuition_base = ptr<Library>(wdata().IntBase);
    panic_message(d.Sys_Message);
    if (uint32_t gfx = wdata().GfxBase)
        hal::close_library(ptr<Library>(gfx));
    hal::close_library(dos_base);
    hal::close_library(intuition_base);
    uint32_t wb = d.Sys_Message;
    hal::release(ptr<void>(d.Sys_AData), d.Sys_LData);
    main_get_out(wb);
}

} // namespace amos::boot
