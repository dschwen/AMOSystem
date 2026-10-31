// The monitor's patches on AMOS.library (+Monitor.s Set_WPatch, EcIn,
// SyIn): while it runs, the screen functions that remake the display keep
// its screens in front, the screens made get their place in the view, and
// the mouse's coordinates over the view become those of the screen it
// shows. The assembler replaced the vectors with tables of its own; the
// patches are now called by ec_call and sy_call before the vectors.

#include "monitor/monitor.h"

using namespace amos;
using namespace amos::monitor;

namespace amos::w {
bool (*sy_patch)(int function, Regs &r);
bool (*ec_patch)(int function, Regs &r);
} // namespace amos::w

namespace {

constexpr uint16_t EcFsel = 10, EcReq = 11;
constexpr uint32_t EntNul = 0x80000000;
// The view's red frame: where the screen shown is
constexpr int16_t RedX = 16, RedY = 9, RedTX = 320, RedTY = 100;

uint32_t low(uint32_t r, uint16_t w) { return (r & 0xFFFF0000u) | w; }

// Tt_first: the monitor's screens (and the file selector's, the
// requester's) in front
void screens_first()
{
    static const uint16_t screens[] = {Ecran1, Ecran2, EcFsel, EcReq};
    for (uint16_t n : screens) {
        Regs f;
        f.d[1] = n;
        ec_vector_call(ec::First, f);
    }
}

// TT_WPatchC: a screen made; its place in the view
void screen_made(Regs &r)
{
    ec_vector_call(ec::Cree, r);
    if (uint16_t(r.d[0]))
        return;
    Screen *s = ptr<Screen>(r.a[0]);
    uint16_t n = s->EcNumber;
    if (n >= 8)
        return;
    be_u16 *v = &mon().ViewScrols[4 * n];
    v[0] = 0;
    v[1] = 0;
    v[2] = s->EcTx;
    uint16_t height = s->EcTy;
    if (!(s->EcCon0 & 0x8000))
        height >>= 1;
    v[3] = height;
}

bool ec_patch(int function, Regs &r)
{
    switch (function) {
    case ec::CopMake:
    case 2:                     // (Libre)
    case ec::CopForce:
        screens_first();
        return false;
    case ec::Cree:
        screen_made(r);
        return true;
    }
    return false;
}

// TT_SHere: the current screen one of the program's, the monitor present
bool here()
{
    if (mon().Tt_NotHere)
        return false;
    uint32_t current = wdata().EcCourant;
    return current && ptr<Screen>(current)->EcNumber < 8;
}

// TT_EcIn: the hard coordinates d1/d2 in the view (the monitor's first
// screen): the number of the screen shown, -1 if outside
int32_t in_view(Regs &r)
{
    uint16_t x = uint16_t(r.d[1]), y = uint16_t(r.d[2]);
    Regs s = r;
    s.d[3] = Ecran1 + 1;
    sy_vector_call(sy::XyScr, s);
    r.d[1] = s.d[1];
    r.d[2] = s.d[2];
    if (x) {
        int16_t v = int16_t(int16_t(r.d[1]) - RedX);
        r.d[1] = low(r.d[1], uint16_t(v));
        if (v < 0 || uint16_t(v) >= RedTX)
            return -1;
    }
    if (y) {
        int16_t v = int16_t(int16_t(r.d[2]) - RedY);
        r.d[2] = low(r.d[2], uint16_t(v));
        if (v < 0 || uint16_t(v) >= RedTY)
            return -1;
    }
    return int16_t(mon().Tt_View);
}

uint32_t extended(uint32_t r) { return uint32_t(int32_t(int16_t(r))); }

// XY2Ec: d1/d2 in the view to the coordinates in screen s (View_ScAd: the
// view's scrolling); d1-d3 extended from their words, as movem.w left them
void view_to_screen(Regs &r, const Screen *s)
{
    int16_t view = int16_t(mon().Tt_View);
    const be_u16 *v = &mon().ViewScrols[4 * view];
    for (int i = 1; i <= 3; i++)
        r.d[i] = extended(r.d[i]);
    r.d[1] = low(r.d[1], uint16_t(r.d[1] + v[0]));
    r.d[2] = low(r.d[2], uint16_t(r.d[2] + v[1]));
    if (!(s->EcCon0 & 0x8000))
        r.d[2] = low(r.d[2], uint16_t(r.d[2] << 1));
}

// TT_Hard2Screen: d1/d2 hard to the coordinates of screen d3 (0: the
// current one) if the view shows it; 0, 3 for a screen that is not
// there, -1 if the mouse is not over it
int32_t hard_to_screen(Regs &r)
{
    int16_t n = int16_t(r.d[3]);
    if (n < 0)
        return -1;
    uint32_t screen = n ? uint32_t(wdata().EcAdr[n - 1]) : uint32_t(wdata().EcCourant);
    // (d3 the screen's address, as TT_EcToA1 left it)
    if (n)
        r.d[3] = screen;
    if (!screen)
        return 3;
    const Screen *s = ptr<Screen>(screen);
    int32_t view = in_view(r);
    if (view < 0)
        return -1;
    r.d[3] = low(r.d[3], uint16_t(s->EcNumber + 1));
    if (uint16_t(view) != s->EcNumber)
        return -1;
    view_to_screen(r, s);
    return 0;
}

bool sy_patch(int function, Regs &r)
{
    if (function != sy::XyScr && function != sy::ZoHd && function != sy::ScIn)
        return false;
    if (!here())
        return false;
    switch (function) {
    case sy::XyScr:             // TT_SXyScr
        if (hard_to_screen(r) < 0) {
            r.d[1] = 0;
            r.d[2] = 0;
        }
        r.d[0] = 0;
        return true;
    case sy::ZoHd:              // TT_SZoHd
        if (hard_to_screen(r) < 0) {
            r.d[1] = 0;
            r.d[0] = 0;
            return true;
        }
        r.d[5] = 8;
        sy_vector_call(sy::ZoGr, r);
        return true;
    default: {                  // TT_SGSIn
        int32_t view = in_view(r);
        uint32_t screen = view >= 0 ? uint32_t(wdata().EcAdr[view]) : 0;
        if (screen) {
            const Screen *s = ptr<Screen>(screen);
            view_to_screen(r, s);
            if (uint16_t(r.d[1]) < s->EcTx && uint16_t(r.d[2]) < s->EcTy) {
                r.d[1] = s->EcNumber;
                r.d[0] = 0;
                return true;
            }
        }
        r.d[1] = EntNul;
        r.d[0] = 0;
        return true;
    }
    }
}

} // namespace

namespace amos::monitor {

// Set_WPatch / Del_WPatch
void patches(bool on)
{
    w::ec_patch = on ? ec_patch : nullptr;
    w::sy_patch = on ? sy_patch : nullptr;
}

} // namespace amos::monitor
