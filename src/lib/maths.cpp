// Maths functions of AMOSPro.Lib (integer paths). The float paths
// (FnAbsF, FnSgnF) are still in +Lib.s.

#include <stdint.h>

// =ABS(n). Like the original neg.l, Abs(-2147483648) stays negative.
extern "C" int32_t lib_fn_abs(int32_t value)
{
    return value < 0 ? static_cast<int32_t>(0u - static_cast<uint32_t>(value)) : value;
}

// =SGN(n): -1, 0 or 1
extern "C" int32_t lib_fn_sgn(int32_t value)
{
    return value < 0 ? -1 : value > 0 ? 1 : 0;
}
