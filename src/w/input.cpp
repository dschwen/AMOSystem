// The input handler of AMOS.library, at the head of input.device's chain:
// it takes the mouse moves and the keys while AMOS is in front, fills the
// key buffer (see w/keyboard.cpp) and the table of the keys down, and
// catches Amiga-A. Also the fake mouse event sent to keep screen blankers
// quiet, and the cyclic tests outside the interrupts (Amiga-A, the fake
// event, an AMOS stopped by another one).
//
// The handler runs in input.device's task: its glue (+W.s IoHandler) gives
// it AMOS.library's data zone as a5, which the C++ context gets (see
// core/call_go.s).

#include "core/amiga.h"
#include "core/ioreq.h"
#include "w/w.h"

#include <exec/tasks.h>

#include <devices/input.h>
#include <devices/inputevent.h>
#include <exec/io.h>

// +W.s

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"
#include "hal/workbench.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::w;

namespace {

constexpr uint8_t WFlagEvent = 1 << 1, WFlagAmigaA = 1 << 2, WFlagLoadView = 1 << 4;
constexpr uint16_t BitControl = 1 << 8;
constexpr uint16_t ClLong = 32 * 3;
constexpr uint16_t FFkLong = 24;
constexpr uint32_t FakeCode = 0x789A789A;

// The ASCII codes of the keys $40-$5F: $FF converted by the keymap, $FE
// function keys
const uint8_t special[32] = {
    0xff, 0x08, 0x09, 0x0d, 0x0d, 0x1b, 0x00, 0x00, // $40-$47
    0x00, 0x00, 0xff, 0x00, 0x1e, 0x1f, 0x1c, 0x1d, // $48-$4f
    0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, // $50-$57
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, // $58-$5f
};

// The fake mouse event (WSend_FakeEvent)
struct InputEvent fake_event;


// Cla_Stocke: a key into the key buffer (unless full) and ClLast
void store(WData &d, uint8_t code, uint8_t ascii, uint8_t shifts)
{
    uint16_t at = d.ClTete + 3;
    if (at >= ClLong)
        at = 0;
    if (at != d.ClQueue) {
        d.ClTete = at;
        d.ClBuffer[at] = shifts;
        d.ClBuffer[at + 1] = code;
        d.ClBuffer[at + 2] = ascii;
    }
    uint8_t *last = reinterpret_cast<uint8_t *>(&d.ClLast);
    last[0] = shifts;
    last[1] = code;
    last[3] = ascii;
}

// Cla_Event: a key event, here if AMOS is in front. True for Amiga-A.
bool key_event(WData &d, struct InputEvent *e, bool here)
{
    uint16_t code = e->ie_Code;
    if (code & 0x80) {
        code &= ~0x80;
        d.ClTable[code >> 3] &= ~(1 << (code & 7));
        return false;
    }
    // the low byte of the qualifier: the shifts, Control, the Amiga keys
    uint8_t shifts = uint8_t(e->ie_Qualifier);
    uint8_t ascii = 0;
    bool convert = code >= 0x68 || code < 0x40;
    if (!convert) {
        if (code >= 0x60)
            goto down;
        ascii = special[code - 0x40];
        if (ascii == 0xff) {
            convert = true;
        } else if (ascii == 0xfe) {
            // A function key with Amiga: its string into the buffer
            ascii = 0;
            const uint8_t *keys = shifts & 0x40 ? d.TFF1 : shifts & 0x80 ? d.TFF2 : nullptr;
            if (keys) {
                keys += (code - 0x50) * FFkLong;
                if (*keys) {
                    put_key(keys);
                    return false;
                }
            }
        }
    }
    if (convert) {
        // The keymap, without CONTROL
        e->ie_Qualifier &= ~0x08;
        uint8_t buffer[64];
        LONG n = hal::key_to_text(e, reinterpret_cast<char *>(buffer), sizeof buffer, ptr<void>(reinterpret_cast<IOReq *>(console_request())->io_Device));
        e->ie_Qualifier = (e->ie_Qualifier & 0xFF00) | shifts;
        code = e->ie_Code;
        ascii = n >= 1 ? buffer[0] : 0;
    }
    {
        if ((shifts & d.AmigA_Shifts) == d.AmigA_Shifts && (ascii == d.AmigA_Ascii1 || ascii == d.AmigA_Ascii2)) {
            d.ClTable[code >> 3] |= 1 << (code & 7);
            return true;
        }
        if (here) {
#if defined(__m68k__)
            if (shifts & 0x08 && (ascii == 'C' || ascii == 'c'))
#else
            // On the host Ctrl+Shift+C is the system clipboard's Copy, not the break
            if (shifts & 0x08 && !(shifts & 0x03) && (ascii == 'C' || ascii == 'c'))
#endif
                d.Actualise |= BitControl;
            else
                store(d, uint8_t(code), ascii, shifts);
        }
    }
down:
    d.ClTable[code >> 3] |= 1 << (code & 7);
    return false;
}

// What happens to a mouse event while AMOS is in front: true to remove it
bool mouse_event(WData &d, struct InputEvent *e)
{
    if (address(e->ie_position.ie_addr) == FakeCode && e->ie_Qualifier == IEQUALIFIER_RELATIVEMOUSE) {
        // The fake event goes through, without moving
        e->ie_position.ie_addr = nullptr;
        return false;
    }
    switch (d.MouYOld & 3) {
    case 0:
        // Normal: AMOS moves the mouse, the system sees nothing
        d.MouXOld = e->ie_Qualifier;
        if (e->ie_Qualifier & IEQUALIFIER_RELATIVEMOUSE) {
            d.MouseX += e->ie_position.ie_xy.ie_x;
            d.MouseY += e->ie_position.ie_xy.ie_y;
        }
        return true;
    case 1:
        return true;
    case 2:
        return false;
    default:
        // The moves go through, the buttons stay with AMOS
        d.MouXOld = e->ie_Qualifier;
        e->ie_Qualifier &= 0x8FFF;
        if ((e->ie_Code & 0x7f) == IECODE_LBUTTON || (e->ie_Code & 0x7f) == IECODE_RBUTTON)
            e->ie_Code = IECODE_NOBUTTON;
        return false;
    }
}

} // namespace

namespace amos::w {

// WSend_FakeEvent: a mouse event that does not move, so that a screen
// blanker sees activity
void send_fake_event()
{
    struct InputEvent &e = fake_event;
    e.ie_NextEvent = nullptr;
    e.ie_Class = IECLASS_RAWMOUSE;
    e.ie_SubClass = 0;
    e.ie_Code = IECODE_NOBUTTON;
    e.ie_Qualifier = IEQUALIFIER_RELATIVEMOUSE;
    e.ie_position.ie_addr = ptr<void>(FakeCode);
    auto io = input_request();
    io->io_Data = address(&e);
    io->io_Command = IND_WRITEEVENT;
    io->io_Length = 22;
    hal::do_io(io);
}

} // namespace amos::w

extern "C" {
// IoHandler: a0 the events; d0 what is left of them
void w_io_handler(AllRegs *r)
{
    WData &d = wdata();
    auto events = ptr<struct InputEvent>(r->a[0]);
    r->d[0] = address(events);
    // Inhibited: everything goes through
    if (d.Inhibit)
        return;
    bool here = d.AMOSHere >> 8;
    if (!here)
        d.WFlags |= WFlagEvent;
    struct InputEvent *previous = nullptr;
    for (struct InputEvent *e = events; e;) {
        bool remove = false;
        switch (e->ie_Class) {
        case IECLASS_RAWMOUSE:
            if (here) {
                d.WFlags |= WFlagEvent;
                remove = mouse_event(d, e);
            }
            break;
        case IECLASS_RAWKEY:
            d.WFlags |= WFlagEvent;
            if (!key_event(d, e, here) || d.NoFlip) {
                remove = here;
            } else if (d.WFlags & WFlagLoadView) {
                // Amiga-A: the switch happens in the cyclic tests
                d.WFlags |= WFlagAmigaA;
                remove = true;
            } else {
                amos_wb(here ? 0 : 1);
                remove = true;
            }
            break;
        case IECLASS_DISKINSERTED:
            d.WFlags |= WFlagEvent;
            d.DiscIn = 0xFFFF;
            break;
        case IECLASS_DISKREMOVED:
            d.WFlags |= WFlagEvent;
            d.DiscIn = 0;
            break;
        }
        struct InputEvent *next = e->ie_NextEvent;
        if (!remove)
            previous = e;
        else if (previous)
            previous->ie_NextEvent = next;
        else
            events = next;
        e = next;
    }
    r->d[0] = address(events);
}

// WSend_FakeEvent
void w_sy_send_fake_event(AllRegs *r)
{
    send_fake_event();
    r->a[0] = address(&fake_event);
}

// WTest_Cyclique: what the interrupts leave to the program
void w_sy_test_cyclic(AllRegs *)
{
    WData &d = wdata();
    // Amiga-A caught by the input handler
    if (d.WFlags & WFlagAmigaA) {
        d.WFlags &= ~WFlagAmigaA;
        amos_wb(d.AMOSHere >> 8 ? 0 : 1);
    }
    // The fake events for the screen blankers, every 2 seconds (V36+)
    if (hal::os_version() >= 36 && int16_t(--d.FakeEventCpt) < 0) {
        d.FakeEventCpt = 50 * 2;
        if (d.AMOSHere >> 8)
            send_fake_event();
    }
    // An AMOS started later stops this one: it changes the first letter of
    // the task name to "S", AMOS_Stopped makes it "W" and waits while it is
    // (with the Workbench in front and no interrupts)
    auto name = ptr<struct Task>(d.MyTask)->tc_Node.ln_Name;
    if (!name || *name != 'S')
        return;
    d.Inhibit = 0xFFFF;
    bool was_here = amos_wb(-1);
    amos_wb(0);
    rem_vbl();
    chips().dmacon = 0x000F;
    *reinterpret_cast<volatile char *>(name) = 'W';
    while (*reinterpret_cast<volatile char *>(name) == 'W')
        hal::wait_frame();
    add_vbl();
    if (was_here)
        amos_wb(1);
    d.Inhibit = 0;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/w_input.inc"
#endif
