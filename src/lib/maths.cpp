// Maths functions of AMOSPro.Lib: Abs, Sgn (integer and float paths),
// Str$ of a float, Int, the trigonometric, hyperbolic, exponential and
// logarithmic functions.

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

// ---- the float paths -------------------------------------------------
//
// The parameter collector gives the float (or double) in d3/d4; the work
// is done by the float layer (interp/float.cpp), swapped for the precision.

#include "core/amos.h"
#include "lib/strings.h"

using namespace amos;

namespace {

// mathffp / mathieeedoubbas, mathtrans / mathieeedoubtrans
constexpr int32_t SPAbs = -54, SPFloor = -90;
constexpr int32_t SPAtan = -30, SPSin = -36, SPCos = -42, SPTan = -48, SPSinh = -60, SPCosh = -66, SPTanh = -72,
                  SPExp = -78, SPLog = -84, SPSqrt = -96, SPAsin = -114, SPAcos = -120, SPLog10 = -126;

// What the glue hands back: d2 the type, d3/d4 the value
struct FloatOut {
    uint32_t d2, d3, d4;
};

FloatOut routine(int number, int32_t lvo, uint32_t d3, uint32_t d4)
{
    Regs r;
    r.d[2] = uint32_t(lvo);
    r.d[3] = d3;
    r.d[4] = d4;
    lib_call(number, r);
    return {r.d[2], r.d[3], r.d[4]};
}

// FlPos: an error for a negative number
void positive(uint32_t d3, uint32_t d4)
{
    Regs r;
    r.d[3] = d3;
    r.d[4] = d4;
    lib_call(L_FlPos, r);
}

} // namespace

extern "C" {

// =SGN(f): -1, 0 or 1
void lib_fn_sgn_f(FloatOut *o)
{
    Regs r;
    r.d[3] = o->d3;
    r.d[4] = o->d4;
    lib_call(L_Float_Test, r);
    *o = {0, r.d[0], o->d4};
}

// =STR$(f)
void lib_fn_str_f(FloatOut *o)
{
    uint8_t *s = str_alloc(40);
    Regs r;
    r.a[0] = address(s + 2);
    r.d[3] = o->d3;
    r.d[4] = o->d4;
    lib_call(L_Float2Ascii, r);
    *o = {2, str_finish(s, ptr<uint8_t>(r.a[0])), r.d[4]};
}

void lib_fn_abs_f(FloatOut *o) { *o = routine(L_Float_Fonction, SPAbs, o->d3, o->d4); }
void lib_fn_int_f(FloatOut *o) { *o = routine(L_Float_Fonction, SPFloor, o->d3, o->d4); }
void lib_fn_tan(FloatOut *o) { *o = routine(L_Math_Fonction, SPTan, o->d3, o->d4); }
void lib_fn_sin(FloatOut *o) { *o = routine(L_Math_Fonction, SPSin, o->d3, o->d4); }
void lib_fn_cos(FloatOut *o) { *o = routine(L_Math_Fonction, SPCos, o->d3, o->d4); }
void lib_fn_exp(FloatOut *o) { *o = routine(L_Math_Fonction, SPExp, o->d3, o->d4); }
void lib_fn_hsin(FloatOut *o) { *o = routine(L_Math_Fonction, SPSinh, o->d3, o->d4); }
void lib_fn_hcos(FloatOut *o) { *o = routine(L_Math_Fonction, SPCosh, o->d3, o->d4); }
void lib_fn_htan(FloatOut *o) { *o = routine(L_Math_Fonction, SPTanh, o->d3, o->d4); }
void lib_fn_asin(FloatOut *o) { *o = routine(L_AAngle, SPAsin, o->d3, o->d4); }
void lib_fn_acos(FloatOut *o) { *o = routine(L_AAngle, SPAcos, o->d3, o->d4); }
void lib_fn_atan(FloatOut *o) { *o = routine(L_AAngle, SPAtan, o->d3, o->d4); }

void lib_fn_sqr(FloatOut *o)
{
    positive(o->d3, o->d4);
    *o = routine(L_Math_Fonction, SPSqrt, o->d3, o->d4);
}
void lib_fn_log(FloatOut *o)
{
    positive(o->d3, o->d4);
    *o = routine(L_Math_Fonction, SPLog10, o->d3, o->d4);
}
void lib_fn_ln(FloatOut *o)
{
    positive(o->d3, o->d4);
    *o = routine(L_Math_Fonction, SPLog, o->d3, o->d4);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_maths.inc"
#endif
