// The software IEEE double arithmetic that AMOSPro.Lib carries for its
// double <-> text conversions (+Lib.s, after DoubleToAsc: compiler
// output). A double is hi:lo, as in d0:d1. A zero exponent is a zero
// (no denormals); the rounding is the routines' own.
//
// The routines are followed instruction by instruction: their results,
// roundings and overflows included, are those of the original.

#include "lib/dfloat.h"

namespace amos::dbl {

namespace {

uint32_t swap(uint32_t v) { return v << 16 | v >> 16; }
uint16_t exponent_field(uint32_t hi) { return uint16_t(hi >> 16) & 0x7FF0; }

// The mantissa with its leading 1 at bit 63
void unpack(D a, uint32_t &h, uint32_t &l)
{
    h = ((a.hi & 0xFFFFF) | 0x100000) << 11 | a.lo >> 21;
    l = a.lo << 11;
}

void shift_right(uint32_t &h, uint32_t &l)
{
    l = l >> 1 | h << 31;
    h >>= 1;
}

// The result of mantissa h:l (normalised here, rounded to 53 bits) times
// 2 to the power e; sign not 0 for a negative one
D pack(uint32_t h, uint32_t l, uint16_t e, uint8_t sign)
{
    if (!h) {
        h = l;
        l = 0;
        e = uint16_t(e - 32);
        if (!h)
            return {0, 0};
    }
    while (!(h & 0x80000000u)) {
        e--;
        h = h << 1 | l >> 31;
        l <<= 1;
    }
    uint16_t rest = l & 0x7FF;
    if (rest > 0x400 || (rest == 0x400 && (l & 0x800))) {
        uint64_t v = (uint64_t(h) << 32 | l) + 0x800;
        bool carry = v < 0x800;
        h = uint32_t(v >> 32);
        l = uint32_t(v);
        if (carry) {
            l = l >> 1 | h << 31;
            h = h >> 1 | 0x80000000u;
            e++;
        }
    }
    l = l >> 11 | h << 21;
    h = (h >> 11) & 0xFFFFF;
    e = uint16_t(e + 0x3FF);
    if (int16_t(e) < 0)
        return {0x100000u | (sign ? 0x80000000u : 0), 0};
    if (int16_t(e) > 0x7FF)
        return {0x7FFFFFFFu | (sign ? 0x80000000u : 0), 0xFFFFFFFFu};
    if (sign)
        e |= 0x800;
    e = uint16_t(e << 4);
    return {h | uint32_t(e) << 16, l};
}

} // namespace

int tst(D a)
{
    if (!exponent_field(a.hi))
        return 0;
    return int32_t(a.hi) < 0 ? -1 : 1;
}

int cmp(D a, D b)
{
    if (!exponent_field(a.hi))
        a = {0, 0};
    if (!exponent_field(b.hi))
        b = {0, 0};
    if (int32_t(b.hi) < 0) {
        if (int32_t(a.hi) >= 0)
            return 1;
        // both negative: the magnitudes the other way round
        if (int32_t(b.hi) != int32_t(a.hi))
            return int32_t(b.hi) < int32_t(a.hi) ? -1 : 1;
        return b.lo < a.lo ? -1 : b.lo > a.lo;
    }
    if (int32_t(a.hi) < 0)
        return -1;
    if (a.hi != b.hi)
        return int32_t(a.hi) < int32_t(b.hi) ? -1 : 1;
    return a.lo < b.lo ? -1 : a.lo > b.lo;
}

D neg(D a)
{
    if (!exponent_field(a.hi))
        return {0, 0};
    return {a.hi ^ 0x80000000u, a.lo};
}

D add(D a, D b)
{
    uint16_t ea = exponent_field(a.hi);
    if (!ea)
        return exponent_field(b.hi) ? b : D{0, 0};
    uint16_t eb = exponent_field(b.hi);
    if (!eb)
        return a;
    uint8_t sa = int32_t(a.hi) < 0 ? 0xFF : 0, sb = int32_t(b.hi) < 0 ? 0xFF : 0;
    int16_t xa = int16_t((ea >> 4) - 0x3FF), xb = int16_t((eb >> 4) - 0x3FF);
    uint32_t d0, d1, d2, d3;
    unpack(a, d0, d1);
    unpack(b, d2, d3);
    if (xa != xb) {
        if (xa < xb) {
            uint32_t t = d0;
            d0 = d2;
            d2 = t;
            t = d1;
            d1 = d3;
            d3 = t;
            int16_t x = xa;
            xa = xb;
            xb = x;
            uint8_t s = sa;
            sa = sb;
            sb = s;
        }
        // b aligned on a: its bits shifted out kept in bit 0 (but under 5)
        int16_t n = int16_t(xa - xb);
        if (n >= 0x37) {
            d2 = d3 = 0;
        } else if (n < 5) {
            while (n--)
                shift_right(d2, d3);
        } else if (n < 32) {
            bool sticky = (d3 >> n << n) != d3;
            d3 >>= n;
            if (sticky)
                d3 |= 1;
            d3 |= d2 << (32 - n);
            d2 >>= n;
        } else {
            n = int16_t(n - 32);
            if (d3) {
                d3 = d2 >> n | 1;
            } else {
                d3 = d2 >> n;
                if (d3 << n != d2)
                    d3 |= 1;
            }
            d2 = 0;
        }
    }
    uint16_t e = uint16_t(xa);
    if (sa == sb) {
        uint64_t s = (uint64_t(d0) << 32 | d1) + (uint64_t(d2) << 32 | d3);
        bool carry = s < (uint64_t(d2) << 32 | d3);
        d0 = uint32_t(s >> 32);
        d1 = uint32_t(s);
        if (carry) {
            d1 = d1 >> 1 | d0 << 31;
            d0 = d0 >> 1 | 0x80000000u;
            e++;
        }
        return pack(d0, d1, e, sa);
    }
    if (sa > sb) {
        // a negative: b - |a|
        uint32_t t = d0;
        d0 = d2;
        d2 = t;
        t = d1;
        d1 = d3;
        d3 = t;
        sa = sb;
    }
    uint64_t x = uint64_t(d0) << 32 | d1, y = uint64_t(d2) << 32 | d3;
    uint64_t r = x - y;
    if (x < y) {
        sa = 0xFF;
        r = 0 - r;
    }
    return pack(uint32_t(r >> 32), uint32_t(r), e, sa);
}

D sub(D a, D b) { return add(a, {b.hi ^ 0x80000000u, b.lo}); }

D mul(D a, D b)
{
    uint16_t ea = exponent_field(a.hi), eb = exponent_field(b.hi);
    if (!ea || !eb)
        return {0, 0};
    uint32_t a0 = uint16_t(uint16_t(ea + eb) >> 4) - 0x7FDu;
    uint32_t a1 = uint8_t((int32_t(a.hi) < 0 ? 0xFF : 0) ^ (int32_t(b.hi) < 0 ? 0xFF : 0));
    uint32_t d0, d1, d2, d3;
    unpack(a, d0, d1);
    unpack(b, d2, d3);
    // the products of the 16-bit words into d4:d5:d6 (the lowest ones
    // left out)
    uint32_t a2 = d2;
    d2 = d3;
    d3 = 0;
    uint32_t d4 = 0, d5 = 0, d6 = 0;
    bool x = false;
    auto mulu = [](uint32_t p, uint32_t q) { return (p & 0xFFFF) * (q & 0xFFFF); };
    auto word = [](uint32_t &to, uint32_t from) { to = (to & 0xFFFF0000u) | (from & 0xFFFF); };
    auto addl = [&](uint32_t &to, uint32_t v) {
        uint64_t s = uint64_t(to) + v;
        x = s >> 32;
        to = uint32_t(s);
    };
    auto addxl = [&](uint32_t &to, uint32_t v) {
        uint64_t s = uint64_t(to) + v + x;
        x = s >> 32;
        to = uint32_t(s);
    };
    auto addw = [&](uint32_t &to, uint32_t v, bool with_x) {
        uint32_t s = (to & 0xFFFF) + (v & 0xFFFF) + (with_x && x);
        x = s >> 16;
        word(to, s);
    };
    // to the high word of r
    auto addw_high = [&](uint32_t &r, uint32_t v) {
        r = swap(r);
        addw(r, v, false);
        r = swap(r);
    };
    if (d2 & 0xFFFF) {
        word(d3, d0);
        if (d3 & 0xFFFF) {
            d3 = swap(mulu(d2, d3));
            word(d6, d3);
        }
        d3 = swap(d0);
        if (d3 & 0xFFFF)
            addl(d6, mulu(d2, d3));
    }
    d2 = swap(d2);
    if (d2 & 0xFFFF) {
        d3 = swap(d1);
        if (d3 & 0xFFFF) {
            addl(d6, mulu(d2, d3) >> 16);
            addw(d5, 0, true);
        }
        word(d3, d0);
        if (d3 & 0xFFFF) {
            addl(d6, mulu(d2, d3));
            addw(d5, 0, true);
        }
        d3 = mulu(d2, swap(d0));
        addw_high(d6, d3);
        addxl(d5, d3 >> 16);
    }
    d2 = a2;
    if (d2 & 0xFFFF) {
        word(d3, d1);
        if (d3 & 0xFFFF) {
            addl(d6, mulu(d2, d3) >> 16);
            addxl(d5, 0);
        }
        d3 = swap(d1);
        if (d3 & 0xFFFF) {
            addl(d6, mulu(d2, d3));
            addxl(d5, 0);
        }
        word(d3, d0);
        if (d3 & 0xFFFF) {
            d3 = mulu(d2, d3);
            addw_high(d6, d3);
            addxl(d5, d3 >> 16);
        }
        addl(d5, mulu(d2, swap(d0)));
        addxl(d4, 0);
    }
    d2 = swap(d2);
    word(d3, d1);
    if (d3 & 0xFFFF) {
        addl(d6, mulu(d2, d3));
        addxl(d5, 0);
        addxl(d4, 0);
    }
    d1 = swap(d1);
    word(d3, d1);
    if (d3 & 0xFFFF) {
        d3 = mulu(d2, d3);
        addw_high(d6, d3);
        addxl(d5, d3 >> 16);
        addxl(d4, 0);
    }
    word(d3, d0);
    if (d3 & 0xFFFF) {
        addl(d5, mulu(d2, d3));
        addxl(d4, 0);
    }
    d0 = swap(d0);
    d3 = mulu(d2, d0);
    addw_high(d5, d3);
    addxl(d4, d3 >> 16);
    auto shift3 = [&] {
        d6 = d6 >> 1 | d5 << 31;
        d5 = d5 >> 1 | d4 << 31;
        d4 >>= 1;
    };
    if (d4 > 0xFFFF) {
        a1++;      // (the original's slip: its sign; never reached)
        shift3();
    }
    // rounded at bit 16 of d6
    uint16_t low = uint16_t(d6);
    if (low == 0x8000) {
        d6 |= 0x10000;
    } else if (low > 0x8000) {
        addw_high(d6, 1);
        addxl(d5, 0);
        addxl(d4, 0);
        if (d4 > 0xFFFF) {
            a0++;
            shift3();
        }
    }
    word(d6, d5);
    d1 = swap(d6);
    word(d5, d4);
    d0 = swap(d5);
    return pack(d0, d1, uint16_t(a0), uint8_t(a1));
}

D div(D a, D b)
{
    uint16_t ea = exponent_field(a.hi), eb = exponent_field(b.hi);
    if (!ea)
        return {0, 0};
    uint8_t sa = int32_t(a.hi) < 0 ? 0xFF : 0, sb = int32_t(b.hi) < 0 ? 0xFF : 0;
    if (!eb)
        return {0x7FFFFFFFu | (sa ? 0x80000000u : 0), 0xFFFFFFFFu};
    uint16_t e = uint16_t((int16_t(uint16_t(ea - eb)) >> 4) - 1);
    uint8_t sign = sa ^ sb;
    uint32_t d0, d1, d2, d3;
    unpack(a, d0, d1);
    unpack(b, d2, d3);
    if (!(d2 & 0xFFFF) && !d3) {
        // a divisor of 16 bits: four divisions
        uint32_t v = d2 >> 16;
        if ((d0 >> 16) >= v) {
            e++;
            shift_right(d0, d1);
        }
        uint32_t q1 = d0 / v, r = d0 % v;
        uint32_t n = r << 16 | d1 >> 16;
        uint32_t q2 = n / v;
        r = n % v;
        n = r << 16 | (d1 & 0xFFFF);
        uint32_t q3 = n / v;
        r = n % v;
        uint32_t q4 = (r << 16) / v;
        return pack(q1 << 16 | q2, q3 << 16 | q4, e, sign);
    }
    if (d2 == d0 && d3 == d1)
        return pack(0x80000000u, 0, uint16_t(e + 1), sign);
    if (!(d2 > d0 || (d2 == d0 && d3 > d1))) {
        e++;
        shift_right(d0, d1);
    }
    shift_right(d2, d3);
    shift_right(d0, d1);
    // non-restoring, 64 bits of quotient
    uint64_t rem = uint64_t(d0) << 32 | d1, divisor = uint64_t(d2) << 32 | d3;
    uint32_t q = 0, first = 0;
    int16_t bits;
    int16_t rounds = 2;
next_round:
    q = 0;
    bits = 31;
subtract:
    q <<= 1;
    rem = (rem << 1) - divisor;
    if (int64_t(rem) < 0)
        goto negative;
positive:
    q |= 1;
    if (--bits != -1)
        goto subtract;
    if (!--rounds)
        goto done;
    first = q;
    goto next_round;
add_round:
    first = q;
    q = 0;
    bits = 31;
add_back:
    q <<= 1;
    rem = (rem << 1) + divisor;
    if (int64_t(rem) >= 0)
        goto positive;
negative:
    if (--bits != -1)
        goto add_back;
    if (--rounds)
        goto add_round;
done:
    return pack(first, q, e, sign);
}

D from_int(int32_t v)
{
    if (!v)
        return {0, 0};
    uint32_t m = v < 0 ? 0u - uint32_t(v) : uint32_t(v);
    return pack(m, 0, 31, v < 0 ? 0xFF : 0);
}

int32_t to_int(D a)
{
    int16_t e = int16_t(((a.hi >> 20) & 0x7FF) - 0x3FF);
    bool negative = int32_t(a.hi) < 0;
    if (e < 0)
        return 0;
    if (e > 31)
        return negative ? -0x7FFFFFFF : 0x7FFFFFFF;
    uint32_t m = (a.hi << 11 | 0x80000000u | a.lo >> 21) >> (31 - e);
    return int32_t(negative ? 0u - m : m);
}

} // namespace amos::dbl
