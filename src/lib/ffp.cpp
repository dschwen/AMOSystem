// Motorola fast floating point arithmetic, as AMOSPro.Lib carries it for
// its float <-> text conversions (+Lib.s, after FloatToAsc: compiler
// output and the Motorola routines). A fast float: the mantissa in bits
// 31-8 (bit 31 set unless 0), the sign in bit 7, the exponent + 64 in
// bits 6-0; 0 is zero.
//
// The routines are followed instruction by instruction: their results,
// rounding and overflows included, are those of the original. (The
// normalisation loop of the subtraction counts down with dbmi on a word
// whose high byte is the caller's: it can only stop early on its way to an
// underflow, which gives 0 either way.)

#include "lib/ffp.h"

namespace amos::ffp {

namespace {

uint8_t lo(uint32_t v) { return uint8_t(v); }
uint32_t set_lo(uint32_t v, uint8_t b) { return (v & 0xFFFFFF00u) | b; }

// The overflow result: the largest number with the sign of sign
uint32_t largest(uint8_t sign) { return 0xFFFFFF7Fu | (sign & 0x80); }

} // namespace

// FFPNEG
uint32_t neg(uint32_t a) { return lo(a) ? a ^ 0x80 : a; }

// FFPCMP: as the condition codes after it, for bge / blt...
int cmp(uint32_t a, uint32_t b)
{
    auto sign = [](int v) { return v < 0 ? -1 : v > 0 ? 1 : 0; };
    if (int8_t(lo(b)) < 0 && int8_t(lo(a)) < 0) {
        // both negative: b - a
        if (lo(a) != lo(b))
            return sign(int8_t(lo(b)) - int8_t(lo(a)));
        return int32_t(b) < int32_t(a) ? -1 : b != a;
    }
    if (lo(a) != lo(b))
        return sign(int8_t(lo(a)) - int8_t(lo(b)));
    return int32_t(a) < int32_t(b) ? -1 : a != b;
}

// FFPADD / FFPSUB: a + b (a - b)
uint32_t add(uint32_t a, uint32_t b)
{
    uint32_t d7 = a, d6 = b, d3;
    uint8_t d4 = lo(d6), d5;
    bool carry;
    if (d4 & 0x80)
        goto L28484;
    if (!d4)
        return d7;
    d5 = lo(d7);
    if (d5 & 0x80)
        goto L2848A;
    if (!d5)
        goto L28460;
L2842E:
    d5 = uint8_t(d5 - d4);
    if (d5 & 0x80)
        goto L2846A;
    d4 = lo(d7);
    if (d5 >= 0x18)
        return d7;
    d3 = (d6 & 0xFFFFFF00u) >> d5;
    d7 = set_lo(d7, 0x80);
    carry = uint64_t(d7) + d3 > 0xFFFFFFFFu;
    d7 += d3;
    if (carry)
        goto L2844C;
L28448:
    return set_lo(d7, d4);
L2844C: {
    // roxr.l #1 with X = the carry, then the exponent up
    d7 = (d7 >> 1) | 0x80000000u;
    bool overflow = d4 == 0x7F;
    bool c = d4 == 0xFF;
    d4 = uint8_t(d4 + 1);
    if (!overflow && !c)
        goto L28448;
    return set_lo(0xFFFFFFFFu, uint8_t(d4 - 1));
}
L28460:
    return set_lo(d6, d4);
L2846A:
    if (int8_t(d5) <= int8_t(0xE8))
        goto L28460;
    d5 = uint8_t(-d5);
    d3 = d6;
    d7 = set_lo(d7, 0) >> d5;
    d3 = set_lo(d3, 0x80);
    carry = uint64_t(d7) + d3 > 0xFFFFFFFFu;
    d7 += d3;
    if (carry)
        goto L2844C;
    return set_lo(d7, d4);
L28484:
    d5 = lo(d7);
    if (d5 & 0x80)
        goto L2842E;
    if (!d5)
        goto L28460;
L2848A: {
    // the signs differ: a subtraction of the magnitudes
    d3 = 0xFFFFFF80u;
    d5 ^= 0x80;
    d5 = uint8_t(d5 - d4);
    if (!d5)
        goto L284E2;
    if (d5 & 0x80)
        goto L284D0;
    if (d5 >= 0x18)
        return d7;
    d4 = lo(d7);
    d7 = set_lo(d7, 0x80);
    d3 = d6;
}
L284A0:
    d3 = set_lo(d3, 0) >> d5;
    d7 -= d3;
    if (int32_t(d7) < 0)
        goto L28448;
L284A8:
    d5 = d4;
L284AA: {
    d7 = set_lo(d7, 0);
    d4--;
    if (d7 <= 0x7FFF) {
        d7 = d7 << 16 | d7 >> 16;
        d4 = uint8_t(d4 - 0x10);
    }
    // add.l d7,d7 / dbmi d4
    for (;;) {
        d7 += d7;
        if (int32_t(d7) < 0)
            break;
        d4--;
    }
    if ((d5 ^ d4) & 0x80)
        return 0;
    d7 = set_lo(d7, d4);
    return d4 ? d7 : 0;
}
L284D0:
    if (int8_t(d5) <= int8_t(0xE8))
        goto L28460;
    d5 = uint8_t(-d5);
    d3 = d7;
    d7 = set_lo(d6, 0x80);
    goto L284A0;
L284E2: {
    // the same exponent
    uint8_t e = lo(d7);
    d5 = d4;
    d4 = e;
    d7 = set_lo(d7, lo(d6));
    d7 -= d6;
    if (!d7)
        return 0;
    if (int32_t(d7) >= 0)
        goto L284A8;
    d7 = -d7;
    d4 = d5;
    goto L284AA;
}
}

// FFPSUB: a - b (b's sign flipped into the addition: the low byte of b
// is used nowhere else)
uint32_t sub(uint32_t a, uint32_t b) { return lo(b) ? add(a, b ^ 0x80) : a; }

// FFPMUL: a * b
uint32_t mul(uint32_t a, uint32_t b)
{
    uint8_t ea = lo(a), eb = lo(b);
    if (!ea)
        return a;
    if (!eb)
        return 0;
    // the exponents (without their signs, doubled) added
    uint8_t xa = uint8_t((ea << 1) ^ 0x80), xb = uint8_t((eb << 1) ^ 0x80);
    int sum = int8_t(xa) + int8_t(xb);
    if (sum > 127)
        return largest(ea ^ eb);
    if (sum < -128)
        return 0;
    uint8_t exponent = uint8_t(((ea ^ eb) & 0x80) | (uint8_t(uint8_t(sum) ^ 0x80) >> 1));
    // the mantissas (24 bits each, the low bytes cleared)
    uint32_t ma = a & 0xFFFFFF00u, mb = b & 0xFFFFFF00u;
    uint32_t alo = ma & 0xFFFF, ahi = ma >> 16, blo = mb & 0xFFFF, bhi = mb >> 16;
    uint32_t d4 = blo * alo;
    d4 = d4 << 16 | d4 >> 16;
    d4 += ahi * blo;
    uint32_t t = bhi * alo;
    bool x = uint64_t(d4) + t > 0xFFFFFFFFu;
    d4 += t;
    d4 = (x ? 0x10000u : 0) | (d4 >> 16);
    uint32_t d7 = ahi * bhi + d4;
    if (int32_t(d7) < 0) {
        d7 = set_lo(d7 + 0x80, exponent);
        return exponent ? d7 : 0;
    }
    // one bit short: the exponent down, rounded
    if (exponent == 0x80 || exponent == 0)
        return 0;
    exponent--;
    d7 += 0x40;
    bool carry = int32_t(d7) < 0;
    d7 += d7;
    if (carry) {
        d7 = (d7 >> 1) | 0x80000000u;
        exponent++;
    }
    d7 = set_lo(d7, exponent);
    return exponent ? d7 : 0;
}

// FFPDIV: a / b
uint32_t div(uint32_t a, uint32_t b)
{
    uint8_t eb = lo(b);
    if (!eb)
        return largest(lo(a));     // (the original divides by 0: a trap)
    if (!a)
        return a;
    uint8_t xa = uint8_t((lo(a) << 1) ^ 0x80), xb = uint8_t((eb << 1) ^ 0x80);
    int difference = int8_t(xa) - int8_t(xb);
    if (difference > 127)
        return largest(lo(a) ^ eb);
    if (difference < -128)
        return 0;
    uint8_t d4 = uint8_t(difference);
    uint32_t d7 = a & 0xFFFFFF00u;
    uint16_t bhi = uint16_t(b >> 16);
    if (uint16_t(d7 >> 16) >= bhi) {
        // the mantissa halved, the exponent up
        if (int8_t(d4) > 125)
            return largest(eb);    // (a's sign already cleared: b's)
        d4 = uint8_t(d4 + 2);
        d7 >>= 1;
    }
    d4 = uint8_t(((lo(a) ^ eb) & 0x80) | uint8_t(d4 ^ 0x80) >> 1);
    // the 16 high bits of the quotient, then the 16 low ones
    uint32_t q1 = d7 / bhi;
    d7 -= q1 * bhi;
    d7 = d7 << 16 | d7 >> 16;
    uint32_t t = (b & 0xFF00) * q1;
    if (d7 < t) {
        d7 -= t;
        d7 += b & 0xFFFFFF00u;
        q1--;
    } else {
        d7 -= t;
    }
    q1 &= 0xFFFF;
    d7 &= 0xFFFF0000u;
    // divu: unchanged on an overflow
    if (d7 / bhi <= 0xFFFF)
        d7 = (d7 % bhi) << 16 | d7 / bhi;
    uint32_t d5 = q1 << 16 | (d7 & 0xFFFF);
    if (!(q1 & 0x8000)) {
        d5 += d5;
        d4--;
    }
    d7 = set_lo(d5 + 0x80, d4);
    return d4 ? d7 : 0;
}

// The compiled C: an integer as a float
uint32_t from_int(int32_t v)
{
    bool negative = v < 0;
    uint32_t x = negative ? 0u - uint32_t(v) : uint32_t(v);
    if (!x)
        return 0;
    int32_t e = 24;
    while (x & 0x7F000000u) {
        x = uint32_t(int32_t(x) >> 1);
        e++;
    }
    // (the original loops forever on -2^31)
    while (!(x & 0x800000u) && x) {
        x <<= 1;
        e--;
    }
    x = (x << 8) | uint32_t((e + 64) & 0x7F);
    return negative ? x | 0x80 : x;
}

// The compiled C: a float as an integer, truncated (saturated)
int32_t to_int(uint32_t f)
{
    int16_t e = int16_t((f & 0x7F) - 64);
    if (!f || e < 0)
        return 0;
    bool negative = f & 0x80;
    if (e > 31)
        return negative ? int32_t(0x80000000u) : 0x7FFFFFFF;
    int32_t m = (int32_t(f) >> 8) & 0xFFFFFF;
    for (e -= 24; e < 0; e++)
        m >>= 1;
    for (; e > 0; e--)
        m = int32_t(uint32_t(m) << 1);
    return negative ? -m : m;
}

} // namespace amos::ffp
