// The float layer of AMOSPro.Lib (+ILib.s, from Start_FloatSwap): each
// routine in single precision (mathffp / mathtrans, a float in d3) and in
// double precision (mathieeedoubbas / mathieeedoubtrans, d3/d4). The
// library swaps the two of each pair in its jump table for Set Double
// Precision, so the callers simply call the first.
//
// "1" routines work on the value in d2/d3/d4, "2" ones on the one below it
// on the a3 stack (its type in byte 3, the value at 4); an operation takes
// the left operand there and the right one in d3/d4, the library offset in
// d2. Comparisons and tests answer in the condition codes (the glue tests
// the C function's -1 / 0 / 1).

#include "core/amiga.h"
#include "interp/interp.h"
#include "hal/system.h"

using namespace amos;

extern "C" {
// float_glue.s: call the library function at base + offset with d0-d3
// from r[0..3], d0/d1 back into r[0..1]; returns the condition codes as
// -1 (less), 0 (equal), 1
int32_t interp_lvo(uint32_t *r, uint32_t base, int32_t offset);
}

namespace {

constexpr int32_t SPFix = -30, SPFlt = -36, SPCmp = -42, SPTst = -48, SPMul = -78, SPDiv = -84;

IData &data() { return idata(); }
uint8_t *stacked() { return ctx().a3; }

struct Result {
    uint32_t d0, d1;
    int32_t cc;
};

Result call(uint32_t base, int32_t offset, uint32_t d0, uint32_t d1 = 0, uint32_t d2 = 0, uint32_t d3 = 0)
{
    uint32_t r[4] = {d0, d1, d2, d3};
    int32_t cc = interp_lvo(r, base, offset);
    return {r[0], r[1], cc};
}

uint32_t ffp() { return data().FloatBase; }
uint32_t trans() { return data().MathBase; }
uint32_t ieee() { return data().DFloatBase; }
uint32_t ieee_trans() { return data().DMathBase; }

// Ret_Float / Ret_Int
uint32_t real(RegFrame &r)
{
    r.d[2] = 1;
    return 1;
}
uint32_t integer(RegFrame &r)
{
    r.d[2] = 0;
    return 0;
}

int16_t lvo(const RegFrame &r) { return int16_t(r.d[2]); }

} // namespace

extern "C" {

// IntToFl1 / FlToInt1: d3(/d4) converted
uint32_t interp_int_to_fl1(RegFrame &r)
{
    r.d[3] = r.d[0] = call(ffp(), SPFlt, r.d[3]).d0;
    return real(r);
}
uint32_t interp_dint_to_fl1(RegFrame &r)
{
    Result x = call(ieee(), SPFlt, r.d[3]);
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}
uint32_t interp_fl_to_int1(RegFrame &r)
{
    r.d[3] = r.d[0] = call(ffp(), SPFix, r.d[3]).d0;
    return integer(r);
}
uint32_t interp_dfl_to_int1(RegFrame &r)
{
    r.d[3] = r.d[0] = call(ieee(), SPFix, r.d[3], r.d[4]).d0;
    return integer(r);
}

// IntToFl2 / FlToInt2: the value on the stack
uint32_t interp_int_to_fl2(RegFrame &r)
{
    uint8_t *s = stacked();
    r.d[0] = call(ffp(), SPFlt, peek_l(s + 4)).d0;
    poke_l(s + 4, r.d[0]);
    s[3] = 1;
    return 0;
}
uint32_t interp_dint_to_fl2(RegFrame &r)
{
    uint8_t *s = stacked();
    Result x = call(ieee(), SPFlt, peek_l(s + 4), peek_l(s + 8));
    poke_l(s + 4, r.d[0] = x.d0);
    poke_l(s + 8, r.d[1] = x.d1);
    s[3] = 1;
    return 0;
}
uint32_t interp_fl_to_int2(RegFrame &r)
{
    uint8_t *s = stacked();
    r.d[0] = call(ffp(), SPFix, peek_l(s + 4)).d0;
    poke_l(s + 4, r.d[0]);
    s[3] = 0;
    return 0;
}
uint32_t interp_dfl_to_int2(RegFrame &r)
{
    uint8_t *s = stacked();
    r.d[0] = call(ieee(), SPFix, peek_l(s + 4), peek_l(s + 8)).d0;
    poke_l(s + 4, r.d[0]);
    s[3] = 0;
    return 0;
}

// Math_Fonction / Float_Fonction: d2 the offset in mathtrans / mathffp
uint32_t interp_math_fonction(RegFrame &r)
{
    r.d[3] = r.d[0] = call(trans(), lvo(r), r.d[3]).d0;
    return real(r);
}
uint32_t interp_dmath_fonction(RegFrame &r)
{
    Result x = call(ieee_trans(), lvo(r), r.d[3], r.d[4]);
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}
uint32_t interp_float_fonction(RegFrame &r)
{
    r.d[3] = r.d[0] = call(ffp(), lvo(r), r.d[3]).d0;
    return real(r);
}
uint32_t interp_dfloat_fonction(RegFrame &r)
{
    Result x = call(ieee(), lvo(r), r.d[3], r.d[4]);
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}

// Float_Compare: the stacked value against d3/d4
uint32_t interp_float_compare(RegFrame &r)
{
    Result x = call(ffp(), SPCmp, peek_l(stacked() + 4), r.d[3]);
    r.d[0] = x.d0;
    return uint32_t(x.cc);
}
uint32_t interp_dfloat_compare(RegFrame &r)
{
    uint8_t *s = stacked();
    Result x = call(ieee(), SPCmp, peek_l(s + 4), peek_l(s + 8), r.d[3], r.d[4]);
    r.d[0] = x.d0;
    return uint32_t(x.cc);
}

// Float_Operation / Math_Operation: stacked (d2's operation) d3/d4
uint32_t interp_float_operation(RegFrame &r)
{
    r.d[3] = r.d[0] = call(ffp(), lvo(r), peek_l(stacked() + 4), r.d[3]).d0;
    return real(r);
}
uint32_t interp_dfloat_operation(RegFrame &r)
{
    uint8_t *s = stacked();
    Result x = call(ieee(), lvo(r), peek_l(s + 4), peek_l(s + 8), r.d[3], r.d[4]);
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}
uint32_t interp_math_operation(RegFrame &r)
{
    r.d[3] = r.d[0] = call(trans(), lvo(r), peek_l(stacked() + 4), r.d[3]).d0;
    return real(r);
}
uint32_t interp_dmath_operation(RegFrame &r)
{
    uint8_t *s = stacked();
    Result x = call(ieee_trans(), lvo(r), peek_l(s + 4), peek_l(s + 8), r.d[3], r.d[4]);
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}

// Float_Test: d3/d4 against 0
uint32_t interp_float_test(RegFrame &r)
{
    Result x = call(ffp(), SPTst, 0, r.d[3]);
    r.d[0] = x.d0;
    return uint32_t(x.cc);
}
uint32_t interp_float_testf(RegFrame &r)
{
    Result x = call(ieee(), SPTst, r.d[3], r.d[4]);
    r.d[0] = x.d0;
    return uint32_t(x.cc);
}

// FlPos: a function call error for a negative float
uint32_t interp_fl_pos(RegFrame &r)
{
    if (r.d[3] & 0x80)
        fon_call();
    return 0;
}
uint32_t interp_fl_pos_d(RegFrame &r)
{
    if (r.d[3] & 0x80000000)
        fon_call();
    return 0;
}

// AAngle: the function d2 of mathtrans, its result made degrees if Degree
uint32_t interp_aangle(RegFrame &r)
{
    IData &d = data();
    uint32_t v = call(trans(), lvo(r), r.d[3]).d0;
    if (d.Angle) {
        v = call(ffp(), SPDiv, v, d.ValPi[0]).d0;
        v = call(ffp(), SPMul, v, d.Val180[0]).d0;
    }
    r.d[3] = r.d[0] = v;
    return real(r);
}
uint32_t interp_aangle_d(RegFrame &r)
{
    IData &d = data();
    Result x = call(ieee_trans(), lvo(r), r.d[3], r.d[4]);
    if (d.Angle) {
        x = call(ieee(), SPDiv, x.d0, x.d1, d.ValPi[0], d.ValPi[1]);
        x = call(ieee(), SPMul, x.d0, x.d1, d.Val180[0], d.Val180[1]);
    }
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}

// FFAngle: d3/d4 made radians if Degree
uint32_t interp_ffangle(RegFrame &r)
{
    IData &d = data();
    if (!d.Angle)
        return 0;
    uint32_t v = call(ffp(), SPDiv, r.d[3], d.Val180[0]).d0;
    v = call(ffp(), SPMul, v, d.ValPi[0]).d0;
    r.d[3] = r.d[0] = v;
    return real(r);
}
uint32_t interp_fangle_d(RegFrame &r)
{
    IData &d = data();
    if (!d.Angle)
        return 0;
    Result x = call(ieee(), SPDiv, r.d[3], r.d[4], d.Val180[0], d.Val180[1]);
    x = call(ieee(), SPMul, x.d0, x.d1, d.ValPi[0], d.ValPi[1]);
    r.d[0] = r.d[3] = x.d0;
    r.d[1] = r.d[4] = x.d1;
    return real(r);
}

// =Param#
uint32_t interp_fn_param_f(RegFrame &r)
{
    r.d[3] = data().ParamF;
    return real(r);
}
uint32_t interp_fn_param_d(RegFrame &r)
{
    r.d[3] = data().ParamF;
    r.d[4] = data().ParamF2;
    return real(r);
}
}

// ---- conversions between the formats (+Lib.s FFP2Ieee, Ieee2FFP, Sp2Dp,
// Dp2Sp: compiler output, followed instruction by instruction) ----------

namespace {

uint32_t swap(uint32_t v) { return v << 16 | v >> 16; }
uint32_t rol(uint32_t v, int n) { return v << n | v >> (32 - n); }
uint32_t ror(uint32_t v, int n) { return v >> n | v << (32 - n); }
uint32_t low_byte(uint32_t v, uint8_t b) { return (v & 0xFFFFFF00) | b; }

} // namespace

namespace amos::interp {

// FFP2Ieee: a Motorola fast float as an IEEE single
uint32_t ffp_to_ieee(uint32_t d0)
{
    d0 <<= 1;
    if (!d0)
        return 0;
    uint8_t b = uint8_t(d0) ^ 0x80;
    b = uint8_t(int8_t(b) >> 1);
    b = uint8_t(b - 0x82);
    return rol(swap(low_byte(d0, b)), 7);
}

// Ieee2FFP: an IEEE single as a fast float (too large: the largest, with
// its sign; too small: 0)
uint32_t ieee_to_ffp(uint32_t d0)
{
    d0 = ror(swap(d0), 7);
    uint8_t b = uint8_t(d0) ^ 0x80;
    bool carry = b & 0x80;
    bool overflow = int8_t(b) != int8_t(b << 1) >> 1;
    b = uint8_t(b << 1);
    d0 = low_byte(d0, b);
    bool x = carry;
    if (!overflow) {
        bool v = int8_t(b) + 5 != int8_t(uint8_t(b + 5));
        x = uint16_t(b) + 5 > 0xFF;
        b = uint8_t(b + 5);
        d0 = low_byte(d0, b);
        if (!v)
            return ror(low_byte(d0, b ^ 0x80), 1);
    } else if (carry) {
        if (b == 0x7C || b == 0x7E) {
            d0 = ror(low_byte(d0, uint8_t(b + 0x85)), 1);
            return uint8_t(d0) ? d0 : 0;
        }
        return 0;
    } else if (b == 0xFE) {
        // lsr.l #8 then #1: X the last bit out
        d0 >>= 8;
        x = d0 & 1;
        d0 >>= 1;
        if (d0)
            return 0;
        return low_byte(0xFFFFFFFF, uint8_t(0x7F | (x ? 0x80 : 0)));
    }
    // L2141DA: lsl.w #8 (X its last bit out), then -1 with X rolled in
    x = d0 & 0x100;
    return low_byte(0xFFFFFFFF, uint8_t(0x7F | (x ? 0x80 : 0)));
}

// Sp2Dp: an IEEE single as a double (high, low)
void single_to_double(uint32_t s, uint32_t *high, uint32_t *low)
{
    uint32_t d0 = swap(s);
    if (!s) {
        *high = 0;
        *low = 0;
        return;
    }
    uint16_t w = uint16_t(d0), sign = w & 0x8000;
    uint16_t e = uint16_t((int16_t(w & 0x7F80) >> 3) + 0x3800);
    d0 = swap((d0 & 0xFFFF0000) | uint16_t(e | sign));
    uint32_t r = ror(s, 3);
    *high = d0 | (r & 0xFFFFF);
    *low = r & 0xE0000000;
}

// Dp2Sp: a double as an IEEE single (too large: the largest)
uint32_t double_to_single(uint32_t d0, uint32_t d1)
{
    if (!d0)
        return 0;
    d0 = swap(d0);
    d1 = (d1 & 0xFFFF0000) | uint16_t(d0);
    d1 = swap(d1);
    d0 = swap(d0);
    for (int i = 0; i < 3; i++) {
        bool x = d1 & 0x8000;
        d1 = (d1 & 0xFFFF0000) | uint16_t(d1 << 1);
        d0 = d0 << 1 | (x ? 1 : 0);
    }
    d0 = swap(d0);
    d0 = (d0 & 0xFFFF0000) | (d0 & 0x7F);
    d1 = swap(d1);
    uint16_t a0 = uint16_t(d1);
    d0 |= d1 & 0x8000;
    int16_t e = int16_t((a0 & 0x7FF0) - 0x3800);
    if (e < 0)
        return 0;
    if (e > 0xFF0)
        return swap(d0 | 0xFFFF7FFF);
    d0 |= uint16_t(e << 3);
    return swap(d0);
}

} // namespace amos::interp

// control.cpp: run-time error n
extern "C" [[noreturn]] void interp_error(uint32_t n, uint32_t first, uint32_t extension, uint32_t messages);

// Open_MathLibraries (+ILib.s): the math libraries of the program's
// precision, and the float routines of the libraries swapped to it
namespace {

constexpr int LB_Flags = -12, LB_DFloatSwap = -30, LB_FFloatSwap = -32, LBF_DFloat = 1;

uint32_t open_library(const char *name)
{
    uint32_t base = address(hal::open_library(name, 0));
    if (!base) {
        // Cannot open math libraries, at the start of the program
        ctx().a6 = ptr<uint8_t>(data().Prg_Run);
        interp_error(12, 19, uint32_t(-1), 0);
    }
    return base;
}

// Lib_FSwap: the pairs of routines from DFloatSwap to FFloatSwap of the
// library at lib exchanged in its jump table (the flag of the main
// library changed)
void float_swap(uint8_t *lib, uint8_t *main)
{
    main[LB_Flags] ^= 1 << LBF_DFloat;
    int16_t first = int16_t(peek_w(lib + LB_DFloatSwap));
    auto *entry = reinterpret_cast<be_u32 *>(lib - LB_Size - 4 - 4 * first);
    uint16_t n = uint16_t((uint32_t(int32_t(int16_t(peek_w(lib + LB_FFloatSwap) - first))) >> 1) - 1);
    if (int16_t(n) < 0)
        return;
    do {
        uint32_t t = entry[-1];
        entry[-1] = entry[-2];
        entry[-2] = t;
        entry -= 2;
    } while (n--);
}

} // namespace

extern "C" void interp_open_math_libraries()
{
    IData &d = data();
    d.ValPi[0] = 0xC90FD942;
    d.Val180[0] = 0xB4000048;
    d.Long_Var = 0x04040404;
    if ((d.MathFlags & 2) && !d.MathBase)
        d.MathBase = open_library("mathtrans.library");
    bool dble = int8_t(d.MathFlags) < 0;
    if (dble) {
        d.Val180[0] = 0x40668000;
        d.Val180[1] = 0;
        d.ValPi[0] = 0x400921FB;
        d.ValPi[1] = 0x54442EEA;
        d.Long_Var = 0x04080404;
        if (!d.DFloatBase)
            d.DFloatBase = open_library("mathieeedoubbas.library");
        if (!d.DMathBase)
            d.DMathBase = open_library("mathieeedoubtrans.library");
    }
    // Lib_FloatSwap: by the flag of the main library (a4)
    uint8_t *main = ctx().a4;
    for (uint32_t lib : d.AdTokens) {
        if (!lib || !peek_w(ptr<uint8_t>(lib) + LB_FFloatSwap))
            continue;
        bool swapped = main[LB_Flags] & (1 << LBF_DFloat);
        if (dble != swapped)
            float_swap(ptr<uint8_t>(lib), main);
    }
    Regs r;
    lib_call(L_Sys_ClearCache, r);
}

// The conversions of the float layer: lib/floatasc.cpp, lib/doubleasc.cpp
extern "C" {
uint32_t lib_float_to_asc(RegFrame &r);
uint32_t lib_asc_to_float(const char *s);
void lib_double_to_asc(uint32_t hi, uint32_t lo, char *to, int32_t digits, uint32_t mode);
uint64_t lib_asc_to_double(const char *text);

// Ascii2Float(D): the number at a0 in d0 (d0:d1)
uint32_t interp_ascii2float(RegFrame &r)
{
    r.d[0] = lib_asc_to_float(ptr<char>(r.a[0]));
    return 0;
}
uint32_t interp_ascii2float_d(RegFrame &r)
{
    uint64_t v = lib_asc_to_double(ptr<char>(r.a[0]));
    r.d[0] = uint32_t(v >> 32);
    r.d[1] = uint32_t(v);
    return 0;
}

// Float2Ascii(D): d3(/d4) at a0 as Print shows it (Fix, Exp), a space
// before a positive number; a0 its end
uint32_t interp_float2ascii(RegFrame &r)
{
    IData &d = data();
    r.d[0] = r.d[3];
    r.d[4] = (r.d[4] & 0x7FFF0000) | d.FixFlg;
    r.d[5] = (r.d[5] & 0xFFFF0000) | d.ExpFlg;
    return lib_float_to_asc(r);
}
uint32_t interp_float2ascii_d(RegFrame &r)
{
    IData &d = data();
    // mode 2: 15 digits (no Fix) or Fix's; with Exp mode 0, Fix's decimals
    uint32_t mode = 2, digits = 15;
    if (int16_t(d.FixFlg) >= 0) {
        digits = d.FixFlg;
        if (d.ExpFlg)
            mode = 0;
    }
    r.d[0] = mode;
    r.d[1] = digits;
    char *to = ptr<char>(r.a[0]);
    if (!(r.d[3] & 0x80000000u))
        *to++ = ' ';
    lib_double_to_asc(r.d[3], r.d[4], to, int32_t(r.d[1]), mode);
    while (*to)
        to++;
    r.a[0] = address(to);
    return 0;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_float.inc"
#endif
