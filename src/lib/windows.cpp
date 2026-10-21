// Text window instructions of AMOSPro.Lib

#include "core/amos.h"

using namespace amos;

namespace {

// Errors of the screen and window functions of AMOS.library
[[noreturn]] void ec_wi_error(uint32_t code) { lib_jump(L_EcWiErr, code); }

// The current screen, or the "screen not opened" error
void check_screen()
{
    if (!idata().ScOn)
        lib_jump(L_ScNOp, 0);
}

} // namespace

// LOCATE x,y. An omitted coordinate arrives as EntNul, which the window
// library leaves unchanged.
extern "C" void lib_in_locate(int32_t x, int32_t y)
{
    check_screen();
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    wi_call(wi::Locate, r);
    if (r.d[0])
        ec_wi_error(r.d[0]);
}
