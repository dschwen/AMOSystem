// Amiga libraries and devices (Lib Open, Dev Open, Dev Do..., Exec), and
// the amiga.lib port and IO request routines

#include "core/amiga.h"
#include "lib/devices.h"
#include "lib/dos.h"
#include "lib/strings.h"

#include <exec/io.h>
#include <exec/memory.h>
#include <exec/ports.h>

#define DOS_BASE_NAME (reinterpret_cast<struct DosLibrary *>(amos::idata().DosBase))
#include <proto/dos.h>

// The records that close the libraries and devices when the program is
// cleared (dos_glue.s)
extern "C" uint8_t lib_close_libs_on_clear[], lib_close_devs_on_clear[];

using namespace amos;

// ---- amiga.lib ----------------------------------------------------------
//
// The extensions call these with the C convention, arguments on the stack.

static void new_list(struct List *list)
{
    list->lh_Head = reinterpret_cast<struct Node *>(&list->lh_Tail);
    list->lh_Tail = nullptr;
    list->lh_TailPred = reinterpret_cast<struct Node *>(&list->lh_Head);
}

extern "C" struct MsgPort *lib_create_port(const char *name, int32_t priority)
{
    int8_t signal = AllocSignal(-1);
    if (signal == -1)
        return nullptr;
    auto port = static_cast<struct MsgPort *>(AllocMem(sizeof(struct MsgPort), MEMF_CLEAR | MEMF_PUBLIC));
    if (!port) {
        FreeSignal(signal);
        return nullptr;
    }
    port->mp_Node.ln_Name = const_cast<char *>(name);
    port->mp_Node.ln_Pri = static_cast<BYTE>(priority);
    port->mp_Node.ln_Type = NT_MSGPORT;
    port->mp_Flags = PA_SIGNAL;
    port->mp_SigBit = signal;
    port->mp_SigTask = FindTask(nullptr);
    if (name)
        AddPort(port);
    else
        new_list(&port->mp_MsgList);
    return port;
}

extern "C" void lib_delete_port(struct MsgPort *port)
{
    if (port->mp_Node.ln_Name)
        RemPort(port);
    port->mp_Node.ln_Type = 0xFF;
    port->mp_MsgList.lh_Head = reinterpret_cast<struct Node *>(-1);
    FreeSignal(port->mp_SigBit);
    FreeMem(port, sizeof(struct MsgPort));
}

extern "C" struct IORequest *lib_create_ext_io(struct MsgPort *port, int32_t size)
{
    if (!port)
        return nullptr;
    auto io = static_cast<struct IORequest *>(AllocMem(size, MEMF_CLEAR | MEMF_PUBLIC));
    if (!io)
        return nullptr;
    io->io_Message.mn_Node.ln_Type = NT_REPLYMSG;
    io->io_Message.mn_Length = static_cast<UWORD>(size);
    io->io_Message.mn_ReplyPort = port;
    return io;
}

extern "C" void lib_delete_ext_io(struct IORequest *io)
{
    if (!io)
        return;
    io->io_Message.mn_Node.ln_Type = 0xFF;
    io->io_Device = reinterpret_cast<struct Device *>(-1);
    io->io_Unit = reinterpret_cast<struct Unit *>(-1);
    FreeMem(io, io->io_Message.mn_Length);
}

namespace {

constexpr uint32_t LibMax = 7, DevMax = 7;

// ---- libraries ----------------------------------------------------------

// Lib.GetA2: library channel n (0-7)
uint32_t &library(uint32_t n)
{
    if (n > LibMax)
        fon_call();
    return idata().Lib_List[n];
}

void close_library(uint32_t n)
{
    uint32_t &base = library(n);
    if (uint32_t b = base) {
        base = 0;
        CloseLibrary(ptr<struct Library>(b));
    }
}

void close_libraries()
{
    for (int n = LibMax; n >= 0; n--)
        close_library(n);
}

} // namespace

// ---- devices ------------------------------------------------------------

namespace amos::dev {

DeviceChannel &device(uint32_t n)
{
    if (n > DevMax)
        fon_call();
    return *reinterpret_cast<DeviceChannel *>(idata().Dev_List + n * 12);
}

void abort_device(DeviceChannel &d)
{
    if (d.opened() && d.state()) {
        AbortIO(d.io());
        WaitIO(d.io());
    }
}

void close_device(DeviceChannel &d)
{
    abort_device(d);
    if (d.opened()) {
        d.opened() = 0;
        CloseDevice(d.io());
    }
    if (struct IORequest *io = d.io()) {
        d.set_io(nullptr);
        lib_delete_ext_io(io);
    }
    if (struct MsgPort *port = d.port()) {
        d.set_port(nullptr);
        lib_delete_port(port);
    }
}

void close_devices()
{
    for (int n = DevMax; n >= 0; n--)
        close_device(device(n));
}

// Dev.Error: an error of the device (positive: its own message, if it has
// one) or of exec (negative)
[[noreturn]] void device_error(DeviceChannel &d, int16_t code)
{
    if (code < 0)
        error(code == -1 ? 142 : code == -3 ? 143 : 144);
    if (static_cast<uint16_t>(code) > d.errors())
        error(144);
    error(d.first_error() + code - 1);
}

// Dev.Open: errors from first_error, count of them; set: a byte to set in
// the IO request (offset in the high word, value in the low byte)
void open_device(DeviceChannel &d, const char *name, uint32_t length, uint32_t unit, uint32_t flags,
                 uint8_t first_error, uint8_t errors, uint32_t set)
{
    if (d.opened())
        error(140);
    Regs r = {};
    r.a[1] = address(lib_close_devs_on_clear);
    r.a[2] = address(&idata().Sys_ClearRoutines);
    w::add_routine(r.a[1], r.a[2]);
    d.state() = 0;
    d.first_error() = first_error;
    d.errors() = errors;
    int32_t err = 0;
    d.set_port(lib_create_port(nullptr, 0));
    if (d.port())
        d.set_io(lib_create_ext_io(d.port(), length));
    if (d.port() && d.io()) {
        if (set)
            reinterpret_cast<uint8_t *>(d.io())[static_cast<int16_t>(set >> 16)] = static_cast<uint8_t>(set);
        err = static_cast<int8_t>(OpenDevice(const_cast<char *>(name), unit, d.io(), flags));
        if (!err) {
            d.opened() = 1;
            return;
        }
    }
    close_device(d);
    device_error(d, static_cast<int16_t>(err));
}

// Dev.GetIO
struct IORequest *device_io(DeviceChannel &d)
{
    if (!d.opened())
        error(141);
    return d.io();
}

// Dev.SendIO / Dev.DoIO: command, waiting for the one sent before
struct IORequest *start_command(DeviceChannel &d, uint16_t command)
{
    struct IORequest *io = device_io(d);
    if (d.state() == 2)
        WaitIO(io);
    io->io_Command = command;
    return io;
}

void send_command(DeviceChannel &d, uint16_t command)
{
    SendIO(start_command(d, command));
    d.state() = 2;
}

void do_command(DeviceChannel &d, uint16_t command)
{
    int8_t err = DoIO(start_command(d, command));
    d.state() = 1;
    if (err)
        device_error(d, err);
}

// Dev.CheckIO: -1 if nothing was started, else exec's CheckIO
uint32_t check_device(DeviceChannel &d)
{
    struct IORequest *io = device_io(d);
    if (!d.state())
        return -1u;
    return address(CheckIO(io));
}

} // namespace amos::dev

using namespace amos::dev;

// ---- register entries ---------------------------------------------------

// Lib.Close, Lib.CloseD0 (d0 = channel), Lib.GetA2 (d0 = channel. Out:
// a2 = where its base is)
extern "C" uint32_t lib_lib_close(RegFrame &)
{
    close_libraries();
    return 0;
}

extern "C" uint32_t lib_lib_close_d0(RegFrame &r)
{
    close_library(r.d[0]);
    return 0;
}

extern "C" uint32_t lib_lib_get_a2(RegFrame &r) { return r.a[2] = address(&library(r.d[0])); }

// Dev.GetA2: d0 = channel. Out: a2 = it.
extern "C" uint32_t lib_dev_get_a2(RegFrame &r)
{
    r.a[2] = address(&device(r.d[0]));
    r.d[0] *= 12;
    return r.a[2];
}

// Dev.Open: a0 = name, a2 = channel, d0 = IO length, d1 = unit, d2 =
// flags, d3 = first error message, d4 = number of them, d5 = byte to set
extern "C" uint32_t lib_dev_open(RegFrame &r)
{
    open_device(*ptr<DeviceChannel>(r.a[2]), ptr<char>(r.a[0]), r.d[0], r.d[1], r.d[2],
                static_cast<uint8_t>(r.d[3]), static_cast<uint8_t>(r.d[4]), r.d[5]);
    return 0;
}

// Dev.Close (all), Dev.CloseA2 (a2 = channel)
extern "C" uint32_t lib_dev_close(RegFrame &)
{
    close_devices();
    return 0;
}

extern "C" uint32_t lib_dev_close_a2(RegFrame &r)
{
    close_device(*ptr<DeviceChannel>(r.a[2]));
    return 0;
}

// Dev.GetIO: a2 = channel. Out: a1 = its IO request.
extern "C" uint32_t lib_dev_get_io(RegFrame &r) { return r.a[1] = address(device_io(*ptr<DeviceChannel>(r.a[2]))); }

// Dev.SendIO, Dev.DoIO: a2 = channel, d0 = command. Out: a1 = IO request,
// d1 = command.
extern "C" uint32_t lib_dev_send_io(RegFrame &r)
{
    DeviceChannel &d = *ptr<DeviceChannel>(r.a[2]);
    send_command(d, static_cast<uint16_t>(r.d[0]));
    r.a[1] = address(d.io());
    r.d[1] = r.d[0];
    return 0;
}

extern "C" uint32_t lib_dev_do_io(RegFrame &r)
{
    DeviceChannel &d = *ptr<DeviceChannel>(r.a[2]);
    do_command(d, static_cast<uint16_t>(r.d[0]));
    r.a[1] = address(d.io());
    r.d[1] = r.d[0];
    r.d[0] = 0;
    return 0;
}

// Dev.CheckIO: a2 = channel. Out: d0.
extern "C" uint32_t lib_dev_check_io(RegFrame &r)
{
    DeviceChannel &d = *ptr<DeviceChannel>(r.a[2]);
    r.a[1] = address(device_io(d));
    return r.d[0] = check_device(d);
}

// Dev.AbortIO: a2 = channel
extern "C" uint32_t lib_dev_abort_io(RegFrame &r)
{
    abort_device(*ptr<DeviceChannel>(r.a[2]));
    return 0;
}

// Dev.Error: a2 = channel, d0.w = error. Never returns.
extern "C" uint32_t lib_dev_error(RegFrame &r)
{
    device_error(*ptr<DeviceChannel>(r.a[2]), static_cast<int16_t>(r.d[0]));
}

// The code of the clear records
extern "C" void lib_close_libs() { close_libraries(); }
extern "C" void lib_close_devs() { close_devices(); }

// ---- instructions -------------------------------------------------------

// LIB OPEN channel,"name",version
extern "C" void lib_in_lib_open(uint32_t n, const uint8_t *name, uint32_t version)
{
    if (!str_len(name))
        fon_call();
    const char *text = str_to_buffer(name);
    uint32_t &base = library(n);
    if (base)
        error(168);
    struct Library *lib = OpenLibrary(text, version);
    if (!lib)
        error(170);
    base = address(lib);
    Regs r = {};
    r.a[1] = address(lib_close_libs_on_clear);
    r.a[2] = address(&idata().Sys_ClearRoutines);
    w::add_routine(r.a[1], r.a[2]);
}

// LIB CLOSE [channel], =LIB BASE(channel)
extern "C" void lib_in_lib_close0() { close_libraries(); }
extern "C" void lib_in_lib_close1(uint32_t n) { close_library(n); }
extern "C" uint32_t lib_fn_lib_base(uint32_t n) { return library(n); }

// DEV OPEN channel,"name",IO length,unit,flags
extern "C" void lib_in_dev_open(uint32_t n, const uint8_t *name, int32_t length, uint32_t unit, uint32_t flags)
{
    if (!str_len(name))
        fon_call();
    const char *text = str_to_buffer(name);
    DeviceChannel &d = device(n);
    if (length <= 0)
        fon_call();
    open_device(d, text, length, unit, flags, 145, 1, 0);
}

// DEV CLOSE [channel], =DEV BASE(channel)
extern "C" void lib_in_dev_close0() { close_devices(); }
extern "C" void lib_in_dev_close1(uint32_t n) { close_device(device(n)); }
extern "C" uint32_t lib_fn_dev_base(uint32_t n) { return peek_l(&device(n)); }

// DEV DO channel,command / DEV SEND channel,command / DEV ABORT channel
extern "C" void lib_in_dev_do(uint32_t n, uint32_t command) { do_command(device(n), static_cast<uint16_t>(command)); }
extern "C" void lib_in_dev_send(uint32_t n, uint32_t command) { send_command(device(n), static_cast<uint16_t>(command)); }
extern "C" void lib_in_dev_abort(uint32_t n) { abort_device(device(n)); }

// =DEV CHECK(channel)
extern "C" uint32_t lib_fn_dev_check(uint32_t n) { return check_device(device(n)); }

// EXEC "command": run a CLI command, input and output on NIL:
extern "C" void lib_in_exec(const uint8_t *command)
{
    if (!str_len(command))
        fon_call();
    const char *text = str_to_buffer(command);
    BPTR nil = Open("NIL:", ModeOldFile);
    LONG ok = Execute(text, nil, nil);
    Close(nil);
    if (!ok)
        disk_error();
}

// ---- Library calls -------------------------------------------------------

namespace {

// Lib.Call: function offset of the library at base, with d0-d7/a0-a5 from
// CallReg (they are not written back); its d0
uint32_t library_call(uint32_t base, uint32_t offset)
{
    IData &d = idata();
    AllRegs r;
    for (int i = 0; i < 8; i++)
        r.d[i] = d.CallReg[i];
    for (int i = 0; i < 6; i++)
        r.a[i] = d.CallReg[8 + i];
    r.a[6] = base;
    amos_call_all(&r, ptr<uint8_t>(base) + offset);
    return r.d[0];
}

} // namespace

// Lib.Call: a0 the base, d3 the offset. Out: d3 the result.
extern "C" uint32_t lib_lib_call(RegFrame &r)
{
    r.d[3] = library_call(r.a[0], r.d[3]);
    r.d[2] = 0;
    return 0;
}

// =EXECALL(f), =GFXCALL(f), =DOSCALL(f), =INTCALL(f)
extern "C" uint32_t lib_fn_exe_call(uint32_t f) { return library_call(peek_l(ptr<void>(4)), f); }
extern "C" uint32_t lib_fn_gfx_call(uint32_t f) { return library_call(wdata().GfxBase, f); }
extern "C" uint32_t lib_fn_dos_call(uint32_t f) { return library_call(idata().DosBase, f); }
extern "C" uint32_t lib_fn_int_call(uint32_t f) { return library_call(wdata().IntBase, f); }

// =LIB CALL(channel,f)
extern "C" uint32_t lib_fn_lib_call(uint32_t channel, uint32_t f)
{
    uint32_t base = library(channel);
    if (!base)
        error(169);
    return library_call(base, f);
}
