// Keyboard functions of AMOS.library (SyCall): Inkey$, Put Key, the
// function keys, Key State, Key Shift, Key Speed. The input handler
// (w/input.cpp) fills the key buffer: three bytes a key (shifts, scan code,
// ASCII), T_ClTete the last one written, T_ClQueue the last one read.
// Also the opening of input.device and console.device. (ClKWait and
// TGetDisc answer in the condition codes and stay in +W.s.)

#include "core/amiga.h"
#include "w/w.h"

#include <devices/input.h>
#include <exec/io.h>
#include <exec/ports.h>
#include "hal/system.h"

using namespace amos;
using namespace amos::w;

// +W.s
extern "C" uint8_t w_con_io[];

namespace {

constexpr uint16_t ClLong = 32 * 3;
constexpr uint16_t FFkLong = 24;
constexpr uint32_t Lio = 0x30, Lmsg = 0x20;     // +WEqu.s

volatile uint16_t &head() { return *static_cast<volatile uint16_t *>(&wdata().ClTete); }

// The next position in the ring buffer after at
uint16_t next(uint16_t at)
{
    at += 3;
    return at < ClLong ? at : 0;
}

// CalRep: VBLs to a timeval in the request
void set_time(uint8_t *io, uint16_t vbls)
{
    int32_t v = int16_t(vbls);
    uint16_t seconds = uint16_t(uint32_t(v) / 50);
    uint32_t rest = uint16_t(uint32_t(v) % 50);
    poke_l(io + 0x20, seconds);
    poke_l(io + 0x24, int32_t(int16_t(rest)) * 20000);
}

} // namespace

namespace amos::w {

// The characters of text into the key buffer; chr$(1) then shifts, scan
// code and ASCII for a key; '...' a comment. Until full. Returns where it
// stopped.
const uint8_t *put_key(const uint8_t *text)
{
    WData &d = wdata();
    for (;;) {
        uint8_t shifts = 0, scan = 0, c = *text++;
        if (!c)
            break;
        if (c == '\'') {
            do
                c = *text++;
            while (c && c != '\'');
            if (!c)
                break;
            continue;
        }
        if (c == 1) {
            shifts = *text++;
            scan = *text++;
            c = *text++;
        }
        uint16_t at = next(head());
        if (at == d.ClQueue)
            break;
        d.ClBuffer[at] = shifts;
        d.ClBuffer[at + 1] = scan;
        d.ClBuffer[at + 2] = c;
        head() = at;
    }
    return text;
}

// OpInput: input.device opened with the request io, followed by its
// reply port (Lio bytes on, the original's layout)
void open_input(uint8_t *io)
{
    for (uint32_t i = 0; i < Lio + Lmsg; i++)
        io[i] = 0;
    auto port = reinterpret_cast<struct MsgPort *>(io + Lio);
    port->mp_SigTask = hal::find_task(nullptr);
    hal::add_port(port);
    hal::open_device(reinterpret_cast<CONST_STRPTR>("input.device"), 0, reinterpret_cast<struct IORequest *>(io), 0);
    reinterpret_cast<struct IORequest *>(io)->io_Message.mn_ReplyPort = port;
}

// ClInput
void close_input(uint8_t *io)
{
    hal::close_device(reinterpret_cast<struct IORequest *>(io));
    hal::remove_port(reinterpret_cast<struct MsgPort *>(io + Lio));
}

// OpConsole: console.device, for RawKeyConvert (w/input.cpp)
void open_console()
{
    for (uint32_t i = 0; i < Lio + Lmsg; i++)
        w_con_io[i] = 0;
    hal::open_device(reinterpret_cast<CONST_STRPTR>("console.device"), uint32_t(-1), reinterpret_cast<struct IORequest *>(w_con_io), 0);
}

void close_console() { hal::close_device(reinterpret_cast<struct IORequest *>(w_con_io)); }

// ClVide: the key buffer emptied
void clear_keys()
{
    WData &d = wdata();
    d.ClQueue = head();
    reinterpret_cast<uint8_t *>(&d.ClFlag)[0] = 0;
}

} // namespace amos::w

extern "C" {
// ClVide: Clear Key
void w_sy_clear_key(AllRegs *r)
{
    clear_keys();
    r->d[0] = 0;
}

// ClInky: d1 = shifts and scan code in the high word, ASCII in the low
// one, 0 if no key
void w_sy_inkey(AllRegs *r)
{
    WData &d = wdata();
    r->d[1] = 0;
    uint16_t at = d.ClQueue;
    if (at != head()) {
        at = next(at);
        const uint8_t *k = d.ClBuffer + at;
        r->d[1] = uint32_t(k[0]) << 24 | uint32_t(k[1]) << 16 | k[2];
        d.ClQueue = at;
        r->a[0] = address(d.ClBuffer);
    }
    r->d[2] = (r->d[2] & 0xFFFF0000) | at;
    r->d[0] = 0;
}

// ClSh: Key Shift
void w_sy_key_shift(AllRegs *r)
{
    r->d[1] = wdata().ClShift[0];
    r->d[0] = 0;
}

// ClInst: Key State(scan code): -1 down, 0 up
void w_sy_key_state(AllRegs *r)
{
    uint16_t code = r->d[1] & 0x7F;
    const uint8_t *bits = wdata().ClTable + (code >> 3);
    r->a[0] = address(bits);
    r->d[1] = *bits >> (code & 7) & 1 ? 0xFFFFFFFF : 0;
    r->d[0] = 0;
}

// ClPutK: Put Key a1 (see put_key)
void w_sy_put_key(AllRegs *r)
{
    r->a[1] = address(put_key(ptr<uint8_t>(r->a[1])));
    // (movem.w gives d0-d3 back sign-extended)
    for (int i = 0; i < 4; i++)
        r->d[i] = uint32_t(int32_t(int16_t(r->d[i])));
}

// ClFFk: function key d1 gets the string a1 (chr$(1) and 3 bytes a key,
// ` a return), up to 23 bytes
void w_sy_set_function_key(AllRegs *r)
{
    uint8_t *to = wdata().TFF1 + int16_t(uint16_t(r->d[1]) * FFkLong);
    const uint8_t *text = ptr<uint8_t>(r->a[1]);
    uint16_t n = 0;
    for (;;) {
        *to = 0;
        uint8_t c = *text++;
        if (!c)
            break;
        if (c == 1) {
            n += 4;
            text += 3;
            if (n >= FFkLong - 1)
                continue;
            *to++ = c;
            *to++ = text[-3];
            *to++ = text[-2];
            *to++ = text[-1];
        } else if (c == '`') {
            n += 2;
            if (n >= FFkLong - 1)
                continue;
            *to++ = 13;
            *to++ = 10;
        } else {
            if (++n >= FFkLong - 1)
                continue;
            *to++ = c;
        }
    }
    r->a[0] = address(text);
    r->a[1] = address(text);
    r->d[0] = 0;
}

// ClGFFk: a0 = function key d1
void w_sy_get_function_key(AllRegs *r)
{
    r->a[0] = address(wdata().TFF1 + int16_t(uint16_t(r->d[1]) * FFkLong));
    r->d[0] = 0;
}

// TKSpeed: Key Speed delay (d1), repeat (d2), in VBLs; a1 a free buffer
// for the input.device request
void w_sy_key_speed(AllRegs *r)
{
    uint8_t *io = ptr<uint8_t>(r->a[1]);
    open_input(io);
    set_time(io, r->d[1]);
    reinterpret_cast<struct IOStdReq *>(io)->io_Command = IND_SETTHRESH;
    hal::do_io(reinterpret_cast<struct IORequest *>(io));
    set_time(io, r->d[2]);
    reinterpret_cast<struct IOStdReq *>(io)->io_Command = IND_SETPERIOD;
    hal::do_io(reinterpret_cast<struct IORequest *>(io));
    close_input(io);
    r->d[0] = 0;
}
}
