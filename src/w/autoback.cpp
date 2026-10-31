// Autoback (EcCall AutoBack1-4): the steps around a drawing on a screen
// with Autoback 1 (draw in the physical bitmap too) or 2 (the bobs taken
// off, drawn in both buffers, and put back).

#include "w/w.h"

#include <graphics/gfx.h>

using namespace amos;
using namespace amos::w;

// w/bobs_glue.s, w/screens_glue.s, +W.s


namespace {

constexpr uint16_t BitBobs = 1 << 13;
constexpr int EcMaxPlans = 6;

// The bitmap and current planes from planes (EcLogic or EcPhysic)
void draw_into(Screen *s, const be_u32 *planes)
{
    struct BitMap *bitmap = ptr<struct BitMap>(s->Ec_BitMap);
    for (int p = 0; p < EcMaxPlans; p++) {
        bitmap->Planes[p] = ptr<UBYTE>(planes[p]);
        s->EcCurrent[p] = planes[p];
    }
}

// The bobs drawn, the screens swapped, the bobs of the other buffer
// taken off
void swap_with_bobs()
{
    call(w_bob_update);
    call(w_bob_show);
    call(w_sc_swap_all);
    wait_vbl();
}

// EcAuto - 1 in the low word of d0
uint32_t mode(Screen *s, uint32_t d0) { return (d0 & 0xFFFF0000) | uint16_t(s->EcAuto - 1); }

} // namespace

extern "C" {
// TAbk1: before: in mode 2, the bobs off
void w_ec_autoback1(AllRegs *r)
{
    Screen *s = current_screen();
    r->d[0] = mode(s, r->d[0]);
    if (int16_t(r->d[0]) > 0) {
        wait_vbl();
        call(w_bob_erase);
        r->d[0] = 0;
    }
}

// TAbk2: between the two drawings: mode 1 draws in the physical bitmap,
// mode 2 swaps
void w_ec_autoback2(AllRegs *r)
{
    Screen *s = current_screen();
    r->d[0] = mode(s, r->d[0]);
    int16_t m = r->d[0];
    if (m < 0)
        return;
    if (!m) {
        draw_into(s, s->EcPhysic);
        r->d[0] = 0xFFFF;       // (a dbra left it so)
        return;
    }
    swap_with_bobs();
    call(w_bob_erase);
    r->d[0] = 0;
}

// TAbk3: after: mode 1 back to the logic bitmap, mode 2 swaps again
void w_ec_autoback3(AllRegs *r)
{
    Screen *s = current_screen();
    r->d[0] = mode(s, r->d[0]);
    int16_t m = r->d[0];
    if (m < 0)
        return;
    if (!m) {
        draw_into(s, s->EcLogic);
        r->d[0] = 0xFFFF;       // (a dbra left it so)
        return;
    }
    swap_with_bobs();
    wdata().Actualise &= ~BitBobs;
    r->d[0] = 0;
}

// TAbk4: after, single buffer: mode 2 draws the bobs again
void w_ec_autoback4(AllRegs *r)
{
    Screen *s = current_screen();
    r->d[0] = mode(s, r->d[0]);
    if (int16_t(r->d[0]) > 0) {
        call(w_bob_update);
        call(w_bob_show);
        r->d[0] = 0;
    }
}
}
