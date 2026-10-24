// Double precision floats <-> text of AMOSPro.Lib (+Lib.s): DoubleToAsc
// (Print's and Str$'s layout of a double) and AscToDouble (the text of a
// number), the compiled C of the original (Dtoa, Atod) on its software
// double arithmetic (lib/dfloat.cpp): the digits are those of the
// original.

#include <stdint.h>
#include "lib/dfloat.h"

using namespace amos::dbl;

namespace {

constexpr D One = {0x3FF00000, 0}, Ten = {0x40240000, 0};

// The roundings: 1, then 0.5, 0.05... 5E-17
constexpr D Round[17] = {
    {0x3FF00000, 0x00000000}, {0x3FE00000, 0x00000000}, {0x3FA99999, 0x9999999A},
    {0x3F747AE1, 0x47AE147B}, {0x3F40624D, 0xD2F1A9FC}, {0x3F0A36E2, 0xEB1C432D},
    {0x3ED4F8B5, 0x88E368F1}, {0x3EA0C6F7, 0xA0B5ED8E}, {0x3E6AD7F2, 0x9ABCAF49},
    {0x3E35798E, 0xE2308C3A}, {0x3E012E0B, 0xE826D695}, {0x3DCB7CDF, 0xD9D7BDBB},
    {0x3D95FD7F, 0xE1796496}, {0x3D619799, 0x812DEA12}, {0x3D2C25C2, 0x68497682},
    {0x3CF6849B, 0x86A12B9C}, {0x3CC203AF, 0x9EE75616},
};

// The character classes of the original's C library (0x10 space, 0x04
// digit), by byte: the ones of characters 128 to 255 are bytes of the
// table above, which its signed index reached
constexpr uint8_t Ctype[256] = {
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x30, 0x30, 0x30, 0x30, 0x30, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x90, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x40, 0x40, 0x40, 0x40, 0x40,
    0x40, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x40, 0x40, 0x40, 0x40, 0x20,
    0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xA9, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9A, 0x3F,
    0x74, 0x7A, 0xE1, 0x47, 0xAE, 0x14, 0x7B, 0x3F, 0x40, 0x62, 0x4D, 0xD2, 0xF1, 0xA9, 0xFC, 0x3F,
    0x0A, 0x36, 0xE2, 0xEB, 0x1C, 0x43, 0x2D, 0x3E, 0xD4, 0xF8, 0xB5, 0x88, 0xE3, 0x68, 0xF1, 0x3E,
    0xA0, 0xC6, 0xF7, 0xA0, 0xB5, 0xED, 0x8E, 0x3E, 0x6A, 0xD7, 0xF2, 0x9A, 0xBC, 0xAF, 0x49, 0x3E,
    0x35, 0x79, 0x8E, 0xE2, 0x30, 0x8C, 0x3A, 0x3E, 0x01, 0x2E, 0x0B, 0xE8, 0x26, 0xD6, 0x95, 0x3D,
    0xCB, 0x7C, 0xDF, 0xD9, 0xD7, 0xBD, 0xBB, 0x3D, 0x95, 0xFD, 0x7F, 0xE1, 0x79, 0x64, 0x96, 0x3D,
    0x61, 0x97, 0x99, 0x81, 0x2D, 0xEA, 0x12, 0x3D, 0x2C, 0x25, 0xC2, 0x68, 0x49, 0x76, 0x82, 0x3C,
    0xF6, 0x84, 0x9B, 0x86, 0xA1, 0x2B, 0x9C, 0x3C, 0xC2, 0x03, 0xAF, 0x9E, 0xE7, 0x56, 0x16, 0x00,
};

bool space(uint8_t c) { return Ctype[c] & 0x10; }
bool digit(uint8_t c) { return Ctype[c] & 0x04; }
int32_t value(uint8_t c) { return int8_t(c) - '0'; }

// 10 to the power n (once the product stays the same it stays so: a
// large n made the original turn for hours)
D power(int32_t n)
{
    D r = One;
    for (int32_t i = 1; i <= n; i++) {
        D next = mul(r, Ten);
        if (next.hi == r.hi && next.lo == r.lo)
            break;
        r = next;
    }
    return r;
}

D overflow(bool negative) { return {negative ? 0xC3A99999u : 0x43A99999u, 0x8D2C7175}; }

} // namespace

extern "C" {

// DoubleToAsc(x, buffer, digits, mode): mode 0 d.ddde+nnn with digits
// decimals, 1 digits decimals, 2 digits digits (the exponential form
// outside 1E-4 to 1E[digits]) without the zeros at the end; bit 4 an E
void lib_double_to_asc(uint32_t hi, uint32_t lo, char *to, int32_t digits, uint32_t mode)
{
    char *const start = to;
    D x = {hi, lo};
    // (bit 5 of the mode, set while it runs, keeps a point at the end and
    // the zeros)
    if ((uint16_t(hi >> 16) & 0x7FF0) == 0x7FF0) {
        char c = hi & 0x80000000u ? '-' : '+';
        while (digits--)
            *to++ = c;
        *to = 0;
    } else {
        int32_t e10 = 0;
        if (tst(x) < 0) {
            x = neg(x);
            *to++ = '-';
        }
        if (tst(x) > 0) {
            while (cmp(x, One) < 0) {
                x = mul(x, Ten);
                e10--;
            }
            while (cmp(x, Ten) >= 0) {
                x = div(x, Ten);
                e10++;
            }
        }
        int32_t form = mode & 3, count;
        if (form == 2) {
            if (!digits)
                digits = 1;
            if (e10 < -4 || e10 >= digits)
                form = -1;
            count = digits;
        } else if (form == 1) {
            count = digits + e10 + 1;
        } else {
            count = digits + 1;
        }
        if (count > 0) {
            x = add(Round[count > 16 ? 16 : count], x);
            if (cmp(x, Ten) >= 0) {
                x = One;
                e10++;
                if (form > 0)
                    count++;
            }
        }
        // the digits before the point
        int32_t before = 1;
        if (form > 0) {
            if (e10 >= 0) {
                before = e10 + 1;
            } else {
                *to++ = '0';
                *to++ = '.';
                int32_t zeros = count > 0 ? -e10 - 1 : digits;
                while (zeros--)
                    *to++ = '0';
                before = 0;
            }
        }
        if (count > 0) {
            for (int32_t i = 0;; i++) {
                if (i >= 16) {
                    *to++ = '0';
                } else {
                    int32_t d = to_int(x);
                    *to++ = char(d + '0');
                    x = mul(sub(x, from_int(d)), Ten);
                }
                if (!--count)
                    break;
                if (before && !--before)
                    *to++ = '.';
            }
        }
        if (before)
            *to++ = '.';
        if (form <= 0) {
            *to++ = mode & 0x10 ? 'E' : 'e';
            if (e10 < 0) {
                e10 = -e10;
                *to++ = '-';
            } else {
                *to++ = '+';
            }
            *to++ = char(e10 / 100 + '0');
            e10 %= 100;
            *to++ = char(e10 / 10 + '0');
            *to++ = char(e10 % 10 + '0');
        }
        *to = 0;
    }
    if ((mode & 0xDF) != 2)
        return;
    // mode 2: the zeros after the point (and the point) off
    char *p = start, *point = nullptr, *last = nullptr;
    for (;;) {
        char c = *p++;
        if (!c || c == 'e' || c == 'E')
            break;
        if (c == '.')
            point = p;
        else if (c != '0' && point)
            last = p;
    }
    if (point) {
        const char *from = p - 1;
        char *q = last ? last : point - 1;
        while ((*q++ = *from++))
            ;
    }
}

// AscToDouble(text): the number in text (spaces before it, a sign, digits
// with a point, e or E, a sign and the exponent's digits)
uint64_t lib_asc_to_double(const char *text)
{
    const uint8_t *s = reinterpret_cast<const uint8_t *>(text);
    bool negative = false;
    while (space(*s))
        s++;
    if (!*s)
        return 0;
    if (*s == '+') {
        s++;
    } else if (*s == '-') {
        negative = true;
        s++;
    } else if (!digit(*s) && *s != '.') {
        return 0;
    }
    // the integer part, from its last digit
    D acc = {0, 0};
    const uint8_t *end = s;
    while (digit(*end))
        end++;
    for (const uint8_t *p = end - 1; p >= s; p--) {
        D before = acc;
        acc = add(mul(power(int32_t(end - p - 1)), from_int(value(*p))), acc);
        if (cmp(acc, before) < 0)
            goto overflow;
    }
    const uint8_t *p;
    if (*end == '.') {
        for (p = end + 1; digit(*p); p++)
            acc = add(div(from_int(value(*p)), power(int32_t(p - end))), acc);
    } else {
        p = end;
    }
    if (*p == 'e' || *p == 'E') {
        if (p[1] == '-' || p[1] == '+')
            p++;
        const uint8_t *q = p + 1;
        if (digit(*q)) {
            int32_t e = 0;
            for (; digit(*q); q++) {
                int32_t before = e;
                e = int32_t(uint32_t(e) * 10 + uint32_t(value(*q)));
                if (e < before) {
                    if (*p != '-')
                        goto overflow;
                    acc = {0, 0};
                    e = 0;
                    break;
                }
            }
            if (*p == '-') {
                acc = div(acc, power(e));
            } else {
                D before = acc;
                acc = mul(power(e), acc);
                if (cmp(acc, before) < 0)
                    goto overflow;
            }
        }
    }
    if (negative)
        acc = neg(acc);
    return uint64_t(acc.hi) << 32 | acc.lo;
overflow:
    D o = overflow(negative);
    return uint64_t(o.hi) << 32 | o.lo;
}
}
