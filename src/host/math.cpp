// The math libraries of the Amiga on another host (interp_lvo, as
// float_glue.s calls them): mathffp with the ports of Motorola's fast
// floating point routines (lib/ffp.cpp), mathtrans through doubles, the
// IEEE double libraries with the host's doubles. The registers and
// results are the libraries' (their autodocs): mathffp's two operands are
// d0 (the left one) and d1, a comparison gives d0 against d1; the IEEE ones take d0/d1 and d2/d3.

#include "runtime/runtime.h"
#include "lib/ffp.h"

#include <fenv.h>
#include <math.h>
#include <string.h>

using namespace amos;

namespace amos::host {
const char *library_name(uint32_t base);
}

namespace {

// Fast floating point <-> double
double from_ffp(uint32_t f)
{
    if (!(f & 0xFF))
        return 0;
    double m = double(f >> 8) / 16777216.0;
    double v = ldexp(m, int(f & 0x7F) - 64);
    return f & 0x80 ? -v : v;
}

uint32_t to_ffp(double v)
{
    if (v == 0 || isnan(v))
        return 0;
    uint32_t sign = v < 0 ? 0x80 : 0;
    int e;
    double m = frexp(fabs(v), &e);          // 0.5 <= m < 1
    uint32_t mant = uint32_t(m * 16777216.0 + 0.5);
    if (mant >= 0x1000000) {
        mant >>= 1;
        e++;
    }
    e += 64;
    if (e > 127)
        return 0xFFFFFF7Fu | sign;
    if (e < 0)
        return 0;
    return mant << 8 | sign | uint32_t(e);
}

double from_ieee(uint32_t hi, uint32_t lo)
{
    uint64_t b = uint64_t(hi) << 32 | lo;
    double d;
    memcpy(&d, &b, 8);
    return d;
}

void to_ieee(double d, uint32_t *r)
{
    uint64_t b;
    memcpy(&b, &d, 8);
    r[0] = uint32_t(b >> 32);
    r[1] = uint32_t(b);
}

float from_single(uint32_t s)
{
    float f;
    memcpy(&f, &s, 4);
    return f;
}

uint32_t to_single(float f)
{
    uint32_t s;
    memcpy(&s, &f, 4);
    return s;
}

int compare(double a, double b) { return a > b ? 1 : a < b ? -1 : 0; }

// Fix: towards zero, saturated
int32_t fix(double v)
{
    if (v >= 2147483647.0)
        return 0x7FFFFFFF;
    if (v <= -2147483648.0)
        return int32_t(0x80000000);
    return int32_t(v);
}

// The functions of mathtrans and mathieeedoubtrans (the same offsets)
bool transcendental(int32_t offset, double x, double y, double &out)
{
    switch (offset) {
    case -30: out = atan(x); break;
    case -36: out = sin(x); break;
    case -42: out = cos(x); break;
    case -48: out = tan(x); break;
    case -60: out = sinh(x); break;
    case -66: out = cosh(x); break;
    case -72: out = tanh(x); break;
    case -78: out = exp(x); break;
    case -84: out = log(x); break;
    case -90: out = pow(x, y); break;
    case -96: out = sqrt(x); break;
    case -114: out = asin(x); break;
    case -120: out = acos(x); break;
    case -126: out = log10(x); break;
    default: return false;
    }
    return true;
}

// The IEEE libraries' basic results are truncated (rounded toward zero). Their transcendental
// functions, and mathtrans's, are the host's libm, in the default rounding: a libm is made for it,
// and in a directed mode its last digits depend on the library (Apple's x86_64 one was 50 units off
// glibc's in Cos(60), where round to nearest gives the Amiga's own result). Their last digit can
// still differ from the Amiga's.
struct TowardZero {
    int old = fegetround();
    TowardZero() { fesetround(FE_TOWARDZERO); }
    ~TowardZero() { fesetround(old); }
};

int32_t ffp_call(uint32_t *r, int32_t offset)
{
    uint32_t a = r[0], b = r[1];
    switch (offset) {
    case -30: r[0] = uint32_t(ffp::to_int(a)); return int32_t(r[0]);        // SPFix
    case -36: r[0] = ffp::from_int(int32_t(a)); return int32_t(r[0]);       // SPFlt
    case -42: {                                                             // SPCmp
        int c = compare(from_ffp(a), from_ffp(b));
        r[0] = uint32_t(c);
        return c;
    }
    case -48: {                                                             // SPTst (d1)
        int c = (b & 0xFF) ? (b & 0x80 ? -1 : 1) : 0;
        r[0] = uint32_t(c);
        return c;
    }
    case -54: r[0] = a & ~0x80u; return int32_t(r[0]);                      // SPAbs
    case -60: r[0] = ffp::neg(a); return int32_t(r[0]);                     // SPNeg
    case -66: r[0] = ffp::add(a, b); return int32_t(r[0]);                  // SPAdd
    case -72: r[0] = ffp::sub(a, b); return int32_t(r[0]);                  // SPSub
    case -78: r[0] = ffp::mul(a, b); return int32_t(r[0]);                  // SPMul
    case -84: r[0] = ffp::div(a, b); return int32_t(r[0]);                  // SPDiv
    case -90: r[0] = to_ffp(floor(from_ffp(a))); return int32_t(r[0]);     // SPFloor
    case -96: r[0] = to_ffp(ceil(from_ffp(a))); return int32_t(r[0]);      // SPCeil
    }
    host::missing("mathffp function", reinterpret_cast<const void *>(intptr_t(offset)));
}

int32_t trans_call(uint32_t *r, int32_t offset)
{
    double out;
    if (offset == -102) {                                                   // SPTieee
        r[0] = to_single(float(from_ffp(r[0])));
        return int32_t(r[0]);
    }
    if (offset == -108) {                                                   // SPFieee
        r[0] = to_ffp(from_single(r[0]));
        return int32_t(r[0]);
    }
    if (!transcendental(offset, from_ffp(r[0]), from_ffp(r[1]), out))
        host::missing("mathtrans function", reinterpret_cast<const void *>(intptr_t(offset)));
    r[0] = to_ffp(out);
    return int32_t(r[0]);
}

int32_t ieee_call(uint32_t *r, int32_t offset)
{
    TowardZero toward_zero;
    double y = from_ieee(r[0], r[1]), z = from_ieee(r[2], r[3]);
    switch (offset) {
    case -30: r[0] = uint32_t(fix(y)); return int32_t(r[0]);                // IEEEDPFix
    case -36: to_ieee(double(int32_t(r[0])), r); return 0;                  // IEEEDPFlt
    case -42: {                                                             // IEEEDPCmp
        int c = compare(y, z);
        r[0] = uint32_t(c);
        return c;
    }
    case -48: {                                                             // IEEEDPTst
        int c = compare(y, 0);
        r[0] = uint32_t(c);
        return c;
    }
    case -54: to_ieee(fabs(y), r); return 0;
    case -60: to_ieee(-y, r); return 0;
    case -66: to_ieee(y + z, r); return 0;
    case -72: to_ieee(y - z, r); return 0;
    case -78: to_ieee(y * z, r); return 0;
    case -84: to_ieee(y / z, r); return 0;
    case -90: to_ieee(floor(y), r); return 0;
    case -96: to_ieee(ceil(y), r); return 0;
    }
    host::missing("mathieeedoubbas function", reinterpret_cast<const void *>(intptr_t(offset)));
}

int32_t ieee_trans_call(uint32_t *r, int32_t offset)
{
    double out;
    if (offset == -102) {                                                   // IEEEDPTieee: to single
        TowardZero toward_zero;
        r[0] = to_single(float(from_ieee(r[0], r[1])));
        return 0;
    }
    if (offset == -108) {                                                   // IEEEDPFieee
        to_ieee(double(from_single(r[0])), r);
        return 0;
    }
    if (offset == -90) {                                                    // IEEEDPPow(exp d2/d3, arg d0/d1)
        to_ieee(pow(from_ieee(r[0], r[1]), from_ieee(r[2], r[3])), r);
        return 0;
    }
    if (!transcendental(offset, from_ieee(r[0], r[1]), 0, out))
        host::missing("mathieeedoubtrans function", reinterpret_cast<const void *>(intptr_t(offset)));
    to_ieee(out, r);
    return 0;
}

} // namespace

extern "C" {

// The math library function at base + offset with d0-d3 from r[0..3];
// d0/d1 back into r[0..1]; the condition codes: -1, 0, 1
int32_t interp_lvo(uint32_t *r, uint32_t base, int32_t offset)
{
    const char *name = host::library_name(base);
    if (!name)
        host::missing("library", ptr<void>(base));
    int32_t cc;
    if (!strcmp(name, "mathffp.library"))
        cc = ffp_call(r, offset);
    else if (!strcmp(name, "mathtrans.library"))
        cc = trans_call(r, offset);
    else if (!strcmp(name, "mathieeedoubbas.library"))
        cc = ieee_call(r, offset);
    else if (!strcmp(name, "mathieeedoubtrans.library"))
        cc = ieee_trans_call(r, offset);
    else
        host::missing(name, reinterpret_cast<const void *>(intptr_t(offset)));
    return cc < 0 ? -1 : cc > 0;
}

// The Music extension's library calls (the narrator, the trackers' players)
uint32_t music_lvo(uint32_t *r, uint32_t base, int32_t offset)
{
    const char *name = host::library_name(base);
    // translator.library's Translate(a0 text, d0 its length, a1 buffer, d1
    // its size): the text itself for the phonemes (the narrator is silent)
    if (name && !strcmp(name, "translator.library") && offset == -30) {
        uint32_t length = r[0], room = r[1];
        const uint8_t *text = ptr<const uint8_t>(r[2]);
        uint8_t *to = ptr<uint8_t>(r[3]);
        uint32_t n = length < room ? length : room;
        for (uint32_t i = 0; i < n; i++)
            to[i] = text[i];
        r[0] = 0;
        return 0;
    }
    host::missing(name ? name : "library", reinterpret_cast<const void *>(intptr_t(offset)));
    return r[0];
}

} // extern "C"

