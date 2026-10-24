// Single precision floats <-> text of AMOSPro.Lib (+Lib.s): FloatToAsc
// (Print's and Str$'s layout of a fast float, with Fix and Exp) and
// AscToFloat (the text of a number). Below them the compiled C of the
// original (ffp2a, a2ffp) on the Motorola fast floating point routines
// (lib/ffp.cpp): the digits are those of the original.

#include "core/amos.h"
#include "lib/ffp.h"

using namespace amos;

namespace {

constexpr uint32_t One = 0x80000041, Two = 0x80000042, Ten = 0xA0000044;
constexpr uint32_t Half = 0x80000040, TwoTo24 = 0x80000059;

// ffp2a: x with prec digits (1 to 22: the digits after the first one) at
// p, "0." and zeros before a number < 1; its end returned
char *ffp2a(uint32_t x, char *p, int16_t prec)
{
    int16_t digits = prec <= 0 ? 1 : prec > 22 ? 23 : int16_t(prec + 1);
    int16_t exponent = 0;
    if (ffp::cmp(x, 0) < 0) {
        *p++ = '-';
        x = ffp::neg(x);
    }
    if (ffp::cmp(x, 0) > 0) {
        while (ffp::cmp(x, One) < 0) {
            x = ffp::mul(x, Ten);
            exponent--;
        }
    }
    while (ffp::cmp(x, Ten) >= 0) {
        x = ffp::div(x, Ten);
        exponent++;
    }
    digits = int16_t(digits + exponent);
    // rounded at the last digit
    uint32_t round = ffp::from_int(1);
    for (int16_t i = 1; i < digits; i++)
        round = ffp::div(round, Ten);
    x = ffp::add(x, ffp::div(round, Two));
    if (ffp::cmp(x, Ten) >= 0) {
        x = One;
        exponent++;
    }
    if (exponent < 0) {
        *p++ = '0';
        *p++ = '.';
        if (digits < 0)
            exponent = int16_t(exponent - digits);
        for (int16_t i = -1; i > exponent; i--)
            *p++ = '0';
    }
    for (int16_t i = 0; i < digits; i++) {
        int16_t digit = int16_t(ffp::to_int(x));
        *p++ = char(digit + '0');
        if (i == exponent)
            *p++ = '.';
        x = ffp::sub(x, ffp::from_int(digit));
        x = ffp::mul(x, Ten);
    }
    *p = 0;
    return p;
}

// The digits of BuFloat with the exponent e: "E+0e" / "E-0e" after them
char *exponent_out(char *to, int16_t e, bool negative)
{
    *to++ = 'E';
    *to++ = negative ? '-' : '+';
    *to++ = '0';
    while (uint8_t(e) >= 10) {
        to[-1]++;
        e = int16_t(e - 10);
    }
    *to++ = char(e + '0');
    *to = 0;
    return to;
}

// F2a: f into DeFloat (fix: the number of decimals, or < 0 for as many
// as needed; exp not 0: the exponential form)
void f2a(uint32_t f, int16_t fix, int16_t exp)
{
    IData &d = idata();
    char *const out = reinterpret_cast<char *>(d.DeFloat);
    char *const bu = reinterpret_cast<char *>(d.BuFloat);
    d.TempBuf = address(out);
    d.TempFl = f;
    if (!exp && fix >= 0) {
        // Fix n
        if (uint16_t(fix) >= 8)
            fix = 7;
        char *end = ffp2a(f, out, fix);
        if (!fix && end[-1] == '.')
            end[-1] = 0;
        return;
    }
    // as many digits as needed
    uint8_t e = f & 0x7F;
    ffp2a(f, bu, e >= 0x41 ? 7 : e >= 0x31 ? 7 + 3 : 16 + 6);
    const char *first = bu;
    if (*first == '-')
        first++;
    const char *p = first;
    int16_t n, decimals;
    bool zero = false;
    if (*p != '0') {
        // >= 1: the digits before the point
        char c;
        do
            c = *p++;
        while (c && c != '.');
        n = int16_t(p - first);
        if (exp || n >= 8)
            goto exponential;
        decimals = int16_t(7 - n);
        if (uint8_t(decimals) >= 5)
            decimals = 5;
    } else {
        // < 1: the zeros after the point
        p += 2;
        const char *zeros = p;
        char c;
        do
            c = *p++;
        while (c && c == '0');
        n = int16_t(p - zeros);
        if (n >= 16 + 6) {
            n = 6;
            zero = true;
        } else if (n >= 4) {
            goto small;
        }
        if (exp)
            goto small;
        decimals = int16_t(n + 6);
    }
    {
        // Clean: the trailing zeros (and point) off
        ffp2a(f, bu, decimals);
        const char *from = bu;
        char *to = out, *last;
        char c;
        for (;;) {
            c = *from++;
            if (!c) {
                *to = 0;
                return;
            }
            *to++ = c;
            if (c == '.')
                break;
        }
        last = to - 1;
        while ((c = *from++)) {
            *to++ = c;
            if (c != '0')
                last = to;
        }
        *last = 0;
        return;
    }

exponential: {
    // >= 1: d.ddddE+nn
    int16_t e10 = int16_t(n - 2);
    if (n >= 7)
        n = 7;
    ffp2a(f, bu, int16_t(9 - n));
    const char *from = bu;
    char *to = out;
    if (*from == '-')
        *to++ = *from++;
    *to++ = *from++;
    *to++ = '.';
    char *last = to - 1;
    int16_t count = fix < 0 || uint16_t(fix) >= 5 ? 5 : fix;
    for (;;) {
        char c = *from++;
        if (!c)
            break;
        if (c == '.')
            continue;
        *to++ = c;
        if (!--count)
            break;
    }
    *to = 0;
    if (fix < 0) {
        for (const char *q = last + 1; *q;)
            if (*q++ != '0')
                last = const_cast<char *>(q);
        to = last;
    }
    exponent_out(to, e10, false);
    return;
}

small: {
    // < 1: d.ddddE-nn; 0 as 0.000E+00 (the original crashed there: it
    // left four registers on the stack)
    static const char Zero[] = "0.0000000";
    int16_t e10;
    const char *from;
    if (zero) {
        e10 = 0;
        from = Zero;
    } else {
        e10 = n;
        ffp2a(f, bu, int16_t(n + 6));
        from = bu;
    }
    char *to = out;
    if (*from == '-')
        *to++ = *from++;
    const char *after = from + 2;
    // the first digit
    char c;
    for (;;) {
        c = *from++;
        if (!c) {
            from = after;
            c = '0';
            break;
        }
        if (c != '.' && c != '0')
            break;
    }
    int16_t count = fix < 0 || uint16_t(fix) >= 6 ? 6 : fix;
    *to++ = c;
    *to++ = '.';
    char *last = to - 1;
    if (count) {
        do {
            c = *from++;
            if (c) {
                *to++ = c;
                if (c != '0')
                    last = to;
            }
        } while (--count);
    }
    if (fix < 0)
        to = last;
    exponent_out(to, e10, !zero);
    return;
}
}

// a2ffp's parts: the digits at p as a float, the exponent at p
uint32_t digits_value(const char *p)
{
    uint32_t v = 0;
    while (int8_t(*p) >= '0' && *p <= '9') {
        v = ffp::mul(v, Ten);
        v = ffp::add(v, ffp::from_int(*p - '0'));
        p++;
    }
    return v;
}

int16_t exponent_value(const char *p)
{
    int16_t v = 0;
    bool negative = false;
    if (*p == '+') {
        p++;
    } else if (*p == '-') {
        p++;
        negative = true;
    }
    while (int8_t(*p) >= '0' && *p <= '9')
        v = int16_t(v * 10 + (*p++ - '0'));
    return negative ? int16_t(-v) : v;
}

// 10 to the power e
uint32_t power(int16_t e)
{
    uint32_t p = One;
    for (; e < 0; e++)
        p = ffp::div(p, Ten);
    for (; e > 0; e--)
        p = ffp::mul(p, Ten);
    return p;
}

// x renormalised: its 24 bits of mantissa made again
uint32_t normalise(uint32_t x)
{
    if (!ffp::cmp(x, 0))
        return 0;
    bool negative = false;
    if (ffp::cmp(x, 0) < 0) {
        x = ffp::neg(x);
        negative = true;
    }
    int16_t e = 0;
    while (ffp::cmp(x, One) >= 0) {
        e++;
        x = ffp::div(x, Two);
    }
    while (ffp::cmp(x, Half) < 0) {
        e--;
        x = ffp::mul(x, Two);
    }
    uint32_t r = uint32_t(ffp::to_int(ffp::mul(x, TwoTo24))) << 8 | ((e + 0x40) & 0x7F);
    return negative ? r | 0x80 : r;
}

} // namespace

extern "C" {

// FloatToAsc: d0 the float at a0 (a0 its end); d4 the decimals (Fix),
// its bit 31 set for no space before a positive number, d5 not 0 for
// the exponential form (Exp). With a number of decimals (not < 0) all
// of DeFloat; otherwise its zeros after the point off.
uint32_t lib_float_to_asc(RegFrame &r)
{
    IData &d = idata();
    char *to = ptr<char>(r.a[0]);
    const char *from = reinterpret_cast<const char *>(d.DeFloat);
    int16_t fix = int16_t(r.d[4]);
    f2a(r.d[0], fix, int16_t(r.d[5]));
    if (!(r.d[4] & 0x80000000u)) {
        *to++ = ' ';
        if (*from == '-')
            to--;
    }
    if (fix >= 0) {
        while ((*to++ = *from++))
            ;
    } else {
        for (;;) {
            char c = *from++;
            if (c == '.')
                break;
            *to++ = c;
            if (!c)
                goto done;
        }
        // the decimals up to the last one not 0
        const char *digits = from, *last = from, *q = from;
        for (;;) {
            char c = *q++;
            if (!c || c == 'E')
                break;
            if (c != '0')
                last = q;
        }
        const char *end = q - 1;
        if (last != digits) {
            *to++ = '.';
            while (digits != last)
                *to++ = *digits++;
        }
        if (*end == 'E')
            *to++ = ' ';
        while ((*to++ = *end++))
            ;
    }
done:
    r.a[0] = address(to - 1);
    return 0;
}

// AscToFloat(text): the number in text (spaces and tabs before it, a sign,
// digits with a point, e or E and the exponent)
uint32_t lib_asc_to_float(const char *s)
{
    // (the original's buffers were in its stack frame: a mantissa of 21
    // digits or more overwrote the saved registers, an exponent of 4
    // characters or more the mantissa's start)
    char mantissa[64], exponent[64];
    char *m = mantissa, *e = exponent;
    while (*s == ' ' || *s == 9)
        s++;
    bool negative = *s == '-';
    if (*s == '-' || *s == '+')
        s++;
    int16_t points = 0, decimals = 0;
    for (; *s && *s != 'e' && *s != 'E'; s++) {
        if (*s == '.') {
            points++;
        } else {
            if (m < mantissa + sizeof mantissa - 1)
                *m++ = *s;
            if (points)
                decimals++;
        }
    }
    *m = 0;
    bool eneg = false;
    if (*s == 'e' || *s == 'E') {
        s++;
        eneg = *s == '-';
        if (*s == '-' || *s == '+')
            s++;
        while (*s && e < exponent + sizeof exponent - 1)
            *e++ = *s++;
    }
    *e = 0;
    uint32_t value = digits_value(mantissa);
    int16_t ten = exponent_value(exponent);
    ten = int16_t((eneg ? -ten : ten) - decimals);
    uint32_t result = normalise(ffp::mul(power(ten), value));
    return negative ? result | 0x80 : result;
}
}
