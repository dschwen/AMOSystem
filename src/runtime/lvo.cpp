// The float loops' next (control_glue.s), on the math library's
// functions: interp_lvo (the Amiga's machine code, src/amiga/native.cpp;
// another host's emulation, src/host/math.cpp).

#include "core/amos.h"

using namespace amos;

extern "C" int32_t interp_lvo(uint32_t *r, uint32_t base, int32_t offset);

extern "C" {

// Next of a float loop (control_glue.s): var += step with mathffp (as the
// original, in either precision); 1 if the loop goes on: a step of 0 or
// more while var is below the limit, a negative one while it is not
uint32_t interp_next_float(uint8_t *var, uint32_t step, uint32_t limit, void *base)
{
    uint32_t r[4] = {0, step, 0, 0};
    int32_t sign = interp_lvo(r, address(base), -48);           // SPTst(step)
    r[0] = step;
    r[1] = peek_l(var);
    interp_lvo(r, address(base), -66);                          // SPAdd
    poke_l(var, r[0]);
    r[1] = limit;
    int32_t c = interp_lvo(r, address(base), -42);              // SPCmp(var, limit)
    // (as the original: equal to the limit ends a loop of positive step)
    if (c < 0)
        return sign >= 0;
    return sign < 0;
}

} // extern "C"
