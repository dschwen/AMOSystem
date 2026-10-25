// AMOS.library and the system: the switch between AMOS's display and the
// Workbench's (Amiga-A, the system requesters), the joysticks, the choice
// of requester.

#include "core/amiga.h"
#include "w/w.h"

#include <intuition/screens.h>

#define INTUITION_BASE_NAME (amos::ptr<struct IntuitionBase>(amos::wdata().IntBase))
#include <proto/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"
#include "hal/chips.h"

using namespace amos;
using namespace amos::w;

namespace {

constexpr uint8_t WFlagWBClosed = 1 << 3, WFlagLoadView = 1 << 4;

// The screen opened in front of the Workbench while AMOS is, when AMOS
// takes the display with LoadView (AA)
struct NewScreen new_screen = {
    0, 0, 480, 12, 1, 1, 0, 0x2000, 6, nullptr, nullptr, nullptr, nullptr,
};

// The direction bits of Joy() for the counter bits 9,8,1,0 of JOYxDAT
const uint8_t directions[16] = {
    0b0000, 0b0010, 0b1010, 0b1000, 0b0001, 0b0000, 0b0000, 0b1001,
    0b0101, 0b0000, 0b0000, 0b0000, 0b0100, 0b0110, 0b0000, 0b0000,
};

} // namespace

namespace amos::w {

// TAMOSWb: 0 the Workbench in front, >0 AMOS, <0 nothing. Returns -1 if
// AMOS is in front, 0 if not. While it swaps, the low byte of T_AMOSHere
// keeps the requesters off.
int32_t amos_wb(int32_t to)
{
    WData &d = wdata();
    auto here = reinterpret_cast<volatile uint8_t *>(&d.AMOSHere);
    if (to > 0 && !here[0]) {
        here[1] = 0xFF;
        if (d.WFlags & WFlagLoadView) {
            // A screen of our own in front of the Workbench, an empty view
            if (d.WFlags & WFlagWBClosed)
                CloseWorkBench();
            d.IntScreen = address(OpenScreen(&new_screen));
            hal::wait_frame();
            hal::wait_frame();
            hal::load_view(nullptr);
            hal::wait_frame();
            hal::wait_frame();
            if ((chips().deniseid & 0xFF) == 0xF8) {
                // AA: the sprites back to lowres, the sprite palette
                chips().fmode = 0;
                chips().bplcon3 = 0x0C00;
            }
        }
        chips().joytest = 0x8080;
        chips().cop1lc = d.CopPhysic;
        chips().copjmp1 = 0;
        here[0] = 0xFF;
        here[1] = 0;
    } else if (to == 0 && here[0]) {
        here[0] = 0;
        here[1] = 0xFF;
        chips().dmacon = d.OldDma;
        chips().cop1lc = peek_l(ptr<uint8_t>(d.GfxBase) + 38);       // GfxBase->copinit
        chips().copjmp1 = 0;
        if (d.WFlags & WFlagLoadView) {
            if (d.WFlags & WFlagWBClosed)
                OpenWorkBench();
            CloseScreen(ptr<struct Screen>(d.IntScreen));
            hal::wait_frame();
            hal::wait_frame();
        }
        here[1] = 0;
    }
    return int8_t(here[0]);
}

} // namespace amos::w

extern "C" {
// TAMOSWb: d1 the direction, d1 = -1 if AMOS is in front, 0 if not
void w_sy_amos_wb(AllRegs *r)
{
    r->d[1] = uint32_t(amos_wb(int32_t(int16_t(r->d[1]))));
    r->d[0] = 0;
}

// ClJoy: Joy(d1): bits 0-3 the directions (up, down, left, right), 4 the
// fire button. 0 while the Workbench is in front.
void w_sy_joy(AllRegs *r)
{
    WData &d = wdata();
    uint16_t port = uint16_t(r->d[1]);
    uint32_t bits = 0;
    if (d.AMOSHere >> 8) {
        // JOY0DAT, JOY1DAT; the buttons in CIA-A's port A
        uint16_t counters = (&chips().joy0dat)[port];
        if (!(hal::cia_a_pra() & 1 << ((6 + port) & 7)))
            bits |= 1 << 4;
        uint16_t index = (counters >> 6 & 0x0C) | (counters & 3);
        bits |= directions[index];
        r->d[2] = (r->d[2] & 0xFFFF0000) | index;
    }
    r->d[1] = bits;
    r->d[0] = 0;
}

// WRequest_OnOff: d0 the requester: >0 the system's, <0 AMOS's, 0 none
void w_sy_request_on_off(AllRegs *r)
{
    wdata().ReqFlag = uint16_t(r->d[0]);
}
}
