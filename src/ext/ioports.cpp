// The IO_Ports extension (extensions/+IO_Ports.s, slot 6): the serial
// ports (four channels), the printer and the parallel port, on the device
// channels of lib/devices.cpp. Its data stays in the extension (DT, the
// address in its ExtAdr), where its end routine closes the channels.

#include "core/amiga.h"
#include "lib/devices.h"
#include "lib/strings.h"

#include <devices/parallel.h>
#include <devices/printer.h>
#include <devices/serial.h>
#include <dos/dosextens.h>
#include <graphics/view.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::dev;

namespace {

constexpr uint32_t ExtNb = 6 - 1;
constexpr uint32_t NSerial = 4;
constexpr uint32_t StringMax = 0xFFC0;

// DT, the extension's data
enum : int {
    SerialIO = 0, PrinterIO = 48, ParallelIO = 60, BufIn = 72, Prt_Query = 76, Prt_Request = 80,
    Width = 84, Height = 86, ViewModes = 88, Depth = 90, Special = 112, SrcX = 114, SrcY = 116,
    PWidth = 118, PHeight = 120, DestCols = 126, DestRows = 130, ScreenBase = 134,
};

uint8_t *dt() { return ptr<uint8_t>(idata().ExtAdr[ExtNb * 4]); }
uint16_t &word(int at) { return *reinterpret_cast<uint16_t *>(dt() + at); }
uint32_t &longword(int at) { return *reinterpret_cast<uint32_t *>(dt() + at); }

DeviceChannel &channel(int at) { return *reinterpret_cast<DeviceChannel *>(dt() + at); }

// GetSerial, GetSerA1
DeviceChannel &serial(uint32_t n)
{
    if (n >= NSerial)
        fon_call();
    return channel(SerialIO + int(n) * 12);
}
struct IOExtSer *serial_io(uint32_t n) { return reinterpret_cast<struct IOExtSer *>(device_io(serial(n))); }
struct IOStdReq *std_io(DeviceChannel &c) { return reinterpret_cast<struct IOStdReq *>(device_io(c)); }

// Stpar
void set_params(uint32_t n) { do_command(serial(n), SDCMD_SETPARAMS); }

// A string of n characters read from the channel (n checked first)
uint32_t read_string(DeviceChannel &c, struct IOStdReq *io, uint32_t n)
{
    if (n >= StringMax)
        fon_call();
    uint32_t room = (n & 0xFFFE) + 2;
    uint8_t *s = str_alloc(room);
    idata().HiChaine = address(s) + 2 + uint16_t(room);
    poke_w(s, uint16_t(n));
    io->io_Data = s + 2;
    io->io_Length = n;
    do_command(c, CMD_READ);
    return address(s);
}

// The text of string s written with command
void write_string(DeviceChannel &c, const uint8_t *s, uint16_t command, bool wait)
{
    struct IOStdReq *io = std_io(c);
    uint16_t n = str_len(s);
    if (!n)
        fon_call();
    io->io_Length = n;
    io->io_Data = const_cast<uint8_t *>(s + 2);
    if (wait)
        do_command(c, command);
    else
        send_command(c, command);
}

void write_memory(DeviceChannel &c, uint32_t at, uint32_t length, uint16_t command, bool wait)
{
    struct IOStdReq *io = std_io(c);
    if (!length)
        fon_call();
    io->io_Data = ptr<void>(at);
    io->io_Length = length;
    if (wait)
        do_command(c, command);
    else
        send_command(c, command);
}

int32_t truth(uint32_t v) { return v ? -1 : 0; }

// The printer's and the parallel port's task: its requesters
struct Process *printer_task() { return reinterpret_cast<struct Process *>(FindTask(const_cast<char *>("printer.device"))); }

// GetScr: the current screen, for a dump
void dump_screen()
{
    Screen *s = ptr<Screen>(idata().ScOnAd);
    if (!s)
        error(47);
    longword(ScreenBase) = address(s);
    word(Width) = s->EcTx;
    word(Height) = s->EcTy;
    word(ViewModes) = s->EcCon0 & 0b1000100010000100;    // hires, HAM, dual playfield, halfbrite
    word(Depth) = s->EcNPlan;
}

// Dump2a: the area x1,y1 To x2,y2
void dump_area(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    word(PHeight) = uint16_t(y2);
    word(PWidth) = uint16_t(x2);
    word(SrcY) = uint16_t(y1);
    word(SrcX) = uint16_t(x1);
    dump_screen();
    word(PWidth) = uint16_t(word(PWidth) - word(SrcX));
    word(PHeight) = uint16_t(word(PHeight) - word(SrcY));
}

// Dump: the screen to the printer
void dump()
{
    DeviceChannel &c = channel(PrinterIO);
    auto io = reinterpret_cast<uint8_t *>(device_io(c));
    struct ColorMap *map = hal::get_color_map(32);
    // (the original took a word off the stack it had not put there)
    if (!map)
        return;
    const Screen *s = ptr<Screen>(longword(ScreenBase));
    for (int i = 0; i < 32; i++) {
        uint16_t rgb = s->EcPal[i];
        hal::set_rgb4_cm(map, i, rgb >> 8, (rgb >> 4) & 15, rgb & 15);
    }
    poke_l(io + 32, wdata().RastPort);
    poke_l(io + 36, address(map));
    poke_w(io + 42, word(ViewModes));
    poke_w(io + 44, word(SrcX));
    poke_w(io + 46, word(SrcY));
    poke_w(io + 48, word(PWidth));
    poke_w(io + 50, word(PHeight));
    poke_l(io + 52, longword(DestCols));
    poke_l(io + 56, longword(DestRows));
    poke_w(io + 60, word(Special));
    do_command(c, PRD_DUMPRPORT);
    hal::free_color_map(map);
}

// The scale of a dump of size of a whole screen of full: 1/(full/size)
uint32_t scale(uint16_t size, uint16_t full)
{
    if (!size)
        fon_call();
    uint16_t ratio = uint16_t(full / size);
    if (!ratio)
        fon_call();
    return uint32_t(uint16_t(0xFFFF / ratio)) << 16;
}

} // namespace

extern "C" {

// ---- serial ---------------------------------------------------------------

// SERIAL OPEN channel,unit[,shared,xdisabled,7wire]
void ext_serial_open5(uint32_t n, uint32_t unit, uint32_t shared, uint32_t xon, uint32_t wire7)
{
    DeviceChannel &c = serial(n);
    uint8_t flags = 0;
    if (wire7)
        flags |= SERF_7WIRE;
    flags |= SERF_XDISABLED;
    if (xon)
        flags &= ~SERF_XDISABLED;
    if (shared)
        flags |= SERF_SHARED;
    open_device(c, "serial.device", sizeof(struct IOExtSer), unit, 0, 145, 16,
                uint32_t(offsetof(struct IOExtSer, io_SerFlags)) << 16 | flags);
    if (unit) {
        auto io = reinterpret_cast<struct IOExtSer *>(c.io());
        io->io_Baud = 1200;
        io->io_ReadLen = 7;
        io->io_WriteLen = 7;
        io->io_StopBits = 1;
        io->io_SerFlags |= SERF_XDISABLED | SERF_PARTY_ON;
        io->io_SerFlags &= ~SERF_PARTY_ODD;
        io->io_ExtFlags &= ~(SEXTF_MSPON | SEXTF_MARK);
    }
    set_params(n);
}
void ext_serial_open2(uint32_t n, uint32_t unit) { ext_serial_open5(n, unit, 0, 0, 0); }

// SERIAL CLOSE [channel]
void ext_serial_close1(uint32_t n) { close_device(serial(n)); }
void ext_serial_close0()
{
    for (uint32_t n = 0; n < NSerial; n++)
        close_device(serial(n));
}

// SERIAL SEND channel,a$ / SERIAL OUT channel,address,length
void ext_serial_send(uint32_t n, const uint8_t *s) { write_string(serial(n), s, CMD_WRITE, false); }
void ext_serial_out(uint32_t n, uint32_t at, int32_t length)
{
    if (length <= 0)
        fon_call();
    write_memory(serial(n), at, uint32_t(length), CMD_WRITE, false);
}

// =SERIAL GET(channel): a character, or -1
int32_t ext_fn_serial_get(uint32_t n)
{
    struct IOExtSer *io = serial_io(n);
    do_command(serial(n), SDCMD_QUERY);
    if (!io->IOSer.io_Actual)
        return -1;
    io->IOSer.io_Data = dt() + BufIn;
    io->IOSer.io_Length = 1;
    do_command(serial(n), CMD_READ);
    return dt()[BufIn];
}

// =SERIAL INPUT$(channel): what has arrived
uint32_t ext_fn_serial_input(uint32_t n)
{
    struct IOExtSer *io = serial_io(n);
    do_command(serial(n), SDCMD_QUERY);
    uint32_t count = io->IOSer.io_Actual;
    if (!count)
        return str_empty();
    return read_string(serial(n), &io->IOSer, count);
}

// SERIAL SPEED / BITS / PARITY / X / BUF / FAST / SLOW
void ext_serial_speed(uint32_t n, uint32_t baud)
{
    serial_io(n)->io_Baud = baud;
    set_params(n);
}

void ext_serial_bits(uint32_t n, uint32_t bits, uint32_t stop)
{
    struct IOExtSer *io = serial_io(n);
    io->io_ReadLen = uint8_t(bits);
    io->io_WriteLen = uint8_t(bits);
    io->io_StopBits = uint8_t(stop);
    set_params(n);
}

// parity: -1 none, 0 even, 1 odd, 2 space, 3 mark
void ext_serial_parity(uint32_t n, uint32_t parity)
{
    struct IOExtSer *io = serial_io(n);
    io->io_SerFlags &= ~(SERF_PARTY_ON | SERF_PARTY_ODD);
    io->io_ExtFlags &= ~(SEXTF_MSPON | SEXTF_MARK);
    switch (int16_t(parity)) {
    case 0:
        io->io_SerFlags |= SERF_PARTY_ON;
        break;
    case 1:
        io->io_SerFlags |= SERF_PARTY_ON | SERF_PARTY_ODD;
        break;
    case 2:
        io->io_ExtFlags |= SEXTF_MSPON;
        break;
    case 3:
        io->io_ExtFlags |= SEXTF_MSPON | SEXTF_MARK;
        break;
    }
    set_params(n);
}

// SERIAL X channel,characters (-1: no xon/xoff)
void ext_serial_x(uint32_t n, uint32_t x)
{
    struct IOExtSer *io = serial_io(n);
    io->io_SerFlags |= SERF_XDISABLED;
    if (x != uint32_t(-1)) {
        io->io_SerFlags &= ~SERF_XDISABLED;
        io->io_CtlChar = x;
    }
    set_params(n);
}

void ext_serial_buf(uint32_t n, uint32_t length)
{
    serial_io(n)->io_RBufLen = length;
    set_params(n);
}

void ext_serial_fast(uint32_t n)
{
    struct IOExtSer *io = serial_io(n);
    io->io_SerFlags &= ~SERF_PARTY_ON;
    io->io_ExtFlags &= ~SEXTF_MSPON;
    io->io_SerFlags |= SERF_XDISABLED;
    io->io_ReadLen = 8;
    io->io_WriteLen = 8;
    io->io_SerFlags |= SERF_RAD_BOOGIE;
    set_params(n);
}

void ext_serial_slow(uint32_t n)
{
    serial_io(n)->io_SerFlags &= ~SERF_RAD_BOOGIE;
    set_params(n);
}

// =SERIAL CHECK / ERROR / STATUS / BASE (channel), SERIAL ABORT channel
int32_t ext_fn_serial_check(uint32_t n) { return truth(check_device(serial(n))); }
uint32_t ext_fn_serial_error(uint32_t n) { return uint8_t(serial_io(n)->IOSer.io_Error); }
uint32_t ext_fn_serial_status(uint32_t n)
{
    struct IOExtSer *io = serial_io(n);
    do_command(serial(n), SDCMD_QUERY);
    return io->io_Status;
}
uint32_t ext_fn_serial_base(uint32_t n) { return address(serial_io(n)); }
void ext_serial_abort(uint32_t n) { abort_device(serial(n)); }

// ---- printer --------------------------------------------------------------

uint32_t ext_fn_printer_base() { return address(device_io(channel(PrinterIO))); }
int32_t ext_fn_printer_check() { return truth(check_device(channel(PrinterIO))); }

// PRINTER OPEN: LPrint's channel closed; the device's requesters off
void ext_printer_open()
{
    Regs r;
    lib_call(L_PRT_Close, r);
    open_device(channel(PrinterIO), "printer.device", 64, 0, 0, 161, 7, 0);
    if (struct Process *p = printer_task()) {
        longword(Prt_Request) = address(p->pr_WindowPtr);
        p->pr_WindowPtr = reinterpret_cast<APTR>(-1);
    }
}

void ext_printer_close()
{
    if (longword(Prt_Request)) {
        if (struct Process *p = printer_task()) {
            p->pr_WindowPtr = ptr<void>(longword(Prt_Request));
            longword(Prt_Request) = 0;
        }
    }
    close_device(channel(PrinterIO));
}

uint32_t ext_fn_printer_error() { return uint8_t(std_io(channel(PrinterIO))->io_Error); }
void ext_printer_send(const uint8_t *s) { write_string(channel(PrinterIO), s, CMD_WRITE, false); }
void ext_printer_out(uint32_t at, uint32_t length) { write_memory(channel(PrinterIO), at, length, PRD_RAWWRITE, false); }
void ext_printer_abort() { abort_device(channel(PrinterIO)); }

// =PRINTER ONLINE
int32_t ext_fn_printer_online()
{
    DeviceChannel &c = channel(PrinterIO);
    struct IOStdReq *io = std_io(c);
    io->io_Data = dt() + Prt_Query;
    do_command(c, PRD_QUERY);
    return io->io_Actual == 1 && !(dt()[Prt_Query] & 1) ? -1 : 0;
}

// PRINTER DUMP: the current screen, all of it
void ext_printer_dump0()
{
    dump_screen();
    word(PWidth) = word(Width);
    word(PHeight) = word(Height);
    longword(DestCols) = 0;
    longword(DestRows) = 0;
    word(Special) = 0x8C;      // aspect, full rows, full columns
    dump();
}

// PRINTER DUMP x1,y1 To x2,y2: scaled as a part of the screen
void ext_printer_dump4(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    dump_area(x1, y1, x2, y2);
    longword(DestCols) = scale(word(PWidth), word(Width));
    longword(DestRows) = scale(word(PHeight), word(Height));
    word(Special) = 0xB0;
    dump();
}

// PRINTER DUMP x1,y1 To x2,y2,columns,rows,special (the original left
// three registers on the stack, and crashed)
void ext_printer_dump7(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t cols, uint32_t rows, uint32_t special)
{
    word(Special) = uint16_t(special);
    longword(DestRows) = rows;
    longword(DestCols) = cols;
    dump_area(x1, y1, x2, y2);
    dump();
}

// ---- parallel -------------------------------------------------------------

uint32_t ext_fn_parallel_base() { return address(device_io(channel(ParallelIO))); }
int32_t ext_fn_parallel_check() { return truth(check_device(channel(ParallelIO))); }

void ext_parallel_open()
{
    Regs r;
    lib_call(L_PRT_Close, r);
    open_device(channel(ParallelIO), "parallel.device", 62, 0, 0, 171, 7, 0);
}

void ext_parallel_close() { close_device(channel(ParallelIO)); }
void ext_parallel_send(const uint8_t *s) { write_string(channel(ParallelIO), s, CMD_WRITE, true); }
// (the original sent the printer's PRD_RAWWRITE, which for the parallel
// device is PDCMD_QUERY: nothing was written)
void ext_parallel_out(uint32_t at, uint32_t length) { write_memory(channel(ParallelIO), at, length, CMD_WRITE, true); }
void ext_parallel_abort() { abort_device(channel(ParallelIO)); }

uint32_t ext_fn_parallel_status()
{
    DeviceChannel &c = channel(ParallelIO);
    auto io = reinterpret_cast<struct IOExtPar *>(device_io(c));
    do_command(c, PDCMD_QUERY);
    return io->io_Status;
}

uint32_t ext_fn_parallel_error() { return uint8_t(std_io(channel(ParallelIO))->io_Error); }

// =PARALLEL INPUT$(length[,terminator])
uint32_t ext_fn_parallel_input1(int32_t length)
{
    DeviceChannel &c = channel(ParallelIO);
    auto io = reinterpret_cast<struct IOExtPar *>(device_io(c));
    io->io_ParFlags &= ~PARF_EOFMODE;
    if (length <= 0)
        fon_call();
    return read_string(c, &io->IOPar, uint32_t(length));
}

uint32_t ext_fn_parallel_input2(int32_t length, uint32_t terminator)
{
    DeviceChannel &c = channel(ParallelIO);
    auto io = reinterpret_cast<struct IOExtPar *>(device_io(c));
    bool was = io->io_ParFlags & PARF_EOFMODE;
    io->io_ParFlags |= PARF_EOFMODE;
    auto term = reinterpret_cast<uint8_t *>(&io->io_PTermArray);
    if (!was || term[0] != uint8_t(terminator)) {
        for (int i = 0; i < 8; i++)
            term[i] = uint8_t(terminator);
        do_command(c, PDCMD_SETPARAMS);
    }
    if (length <= 0)
        fon_call();
    return read_string(c, &io->IOPar, uint32_t(length));
}
}
