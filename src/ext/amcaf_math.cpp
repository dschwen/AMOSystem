// AMCAF: bit and integer functions, the quick maths on tables (Qsin, Qcos, Qarc, Qsqr, Qrnd), raw
// reads, the characters of numbers (R015, R016, R085, R0b3-R0c8, R10f-R113, R115). The routines
// are the library's with the details of its arithmetic: 16-bit divisions that leave the dividend
// alone when they overflow, word compares, shifts by the low six bits of the count.

#include "ext/amcaf.h"
#include "ext/amcaf_pt_tables.h"
#include "hal/chips.h"
#include "lib/strings.h"

using namespace amos;
using namespace amos::amcaf;

namespace {

// DIVU.W: the 32-bit dividend by the 16-bit divisor: the remainder in the high word, the quotient in the
// low; the dividend as it was when the quotient does not fit
uint32_t divu(uint32_t dividend, uint16_t divisor)
{
    if (!divisor)
        fon_call();                                 // (the 68000's divide by zero)
    uint32_t quotient = dividend / divisor;
    if (quotient > 0xFFFF)
        return dividend;
    return (dividend % divisor) << 16 | quotient;
}

// DIVS.W
uint32_t divs(int32_t dividend, int16_t divisor)
{
    if (!divisor)
        fon_call();
    int32_t quotient = dividend / divisor;
    if (quotient < -32768 || quotient > 32767)
        return uint32_t(dividend);
    return uint32_t(dividend % divisor) << 16 | uint16_t(quotient);
}

// The sine as the library builds it at the start: 1024 words from a quarter, with the peaks +256 and -256
const int16_t *sine_table()
{
    static int16_t table[1024];
    static bool made;
    if (!made) {
        made = true;
        const uint16_t *q = math::SineQuarter;
        int i = 0;
        for (int k = 0; k < 255; k++)
            table[i++] = int16_t(q[k]);
        table[i++] = 256;
        for (int k = 255; k >= 0; k--)
            table[i++] = int16_t(q[k]);
        for (int k = 0; k < 255; k++)
            table[i++] = int16_t(-int16_t(q[k]));
        table[i++] = -256;
        for (int k = 255; k >= 0; k--)
            table[i++] = int16_t(-int16_t(q[k]));
    }
    return table;
}

}  // namespace

extern "C" {

// NOP, =NFN, =RNP: a statement and two functions that do nothing (R015, R016, R115)
void amcaf_nop() {}
int32_t amcaf_nfn() { return 0; }
int32_t amcaf_rnp() { return 0; }

// =EXTBASE(n) (R085): the data of extension n
int32_t amcaf_extbase(int32_t n)
{
    n -= 1;
    if (n < 0 || n >= 26)
        fon_call();
    return int32_t(idata().ExtAdr[4 * n]);
}

// =BINEXP(n) (R0c2): 2 to the n
int32_t amcaf_binexp(int32_t n)
{
    if (!n)
        return 1;
    if (n < 0 || n >= 32)
        fon_call();
    return int32_t(1u << n);
}

// =BINLOG(n) (R0c3): the number of the bit of a number that has exactly one
int32_t amcaf_binlog(int32_t n)
{
    if (!n)
        fon_call();
    uint32_t v = uint32_t(n);
    int32_t bit = 0;
    while (!(v & 1)) {
        v >>= 1;
        bit++;
    }
    if (v >> 1)
        fon_call();
    return bit;
}

// =LSL(n,count), =LSR(n,count) (R0c4, R0c5): arithmetic shifts, by the low six bits of the count
int32_t amcaf_lsl(int32_t n, int32_t count)
{
    count &= 63;
    return count >= 32 ? 0 : int32_t(uint32_t(n) << count);
}
int32_t amcaf_lsr(int32_t n, int32_t count)
{
    count &= 63;
    return count >= 32 ? (n < 0 ? -1 : 0) : n >> count;
}

// =WORDSWAP(n) (R0c6)
int32_t amcaf_wordswap(int32_t n) { return int32_t(uint32_t(n) << 16 | uint32_t(n) >> 16); }

// =SPEEK(address), =SDEEK(address) (R0c8, R0c7): a signed byte, a signed word
int32_t amcaf_speek(int32_t at) { return int8_t(host::detail::read_cursor_byte(decode_context_cursor(uint32_t(at)))); }
int32_t amcaf_sdeek(int32_t at)
{
    ContextCursor c = decode_context_cursor(uint32_t(at));
    return int16_t(uint16_t(host::detail::read_cursor_byte(c)) << 8 | host::detail::read_cursor_byte(c + 1));
}

// =CHR.W$(n), =CHR.L$(n) (R0b3, R0b4): the two or four bytes of the number as a string
uint32_t amcaf_chr_w_s(int32_t n)
{
    uint8_t bytes[2] = {uint8_t(n >> 8), uint8_t(n)};
    return str_from(bytes, 2);
}
uint32_t amcaf_chr_l_s(int32_t n)
{
    uint8_t bytes[4] = {uint8_t(n >> 24), uint8_t(n >> 16), uint8_t(n >> 8), uint8_t(n)};
    return str_from(bytes, 4);
}

// =ASC.W(a$), =ASC.L(a$) (R0b5, R0b6): the first two or four bytes as a number
int32_t amcaf_asc_w(GuestString s)
{
    if (str_len(s) < 2)
        fon_call();
    return int32_t(uint16_t(s.at(0)) << 8 | s.at(1));
}
int32_t amcaf_asc_l(GuestString s)
{
    if (str_len(s) < 4)
        fon_call();
    return int32_t(uint32_t(s.at(0)) << 24 | uint32_t(s.at(1)) << 16 | uint32_t(s.at(2)) << 8 | s.at(3));
}

// =VCLIP(n,low TO high) (R0b7): n kept in the range, the upper limit first (they are not put in order)
int32_t amcaf_vclip(int32_t n, int32_t low, int32_t high)
{
    if (n > high)
        n = high;
    if (!(low <= n))
        n = low;
    return n;
}

// =VIN(n,low TO high) (R0b8): -1 if n is in the range, the ends too
int32_t amcaf_vin(int32_t n, int32_t low, int32_t high)
{
    if (n > high)
        return 0;
    if (!(low <= n))
        return 0;
    return -1;
}

// =VMOD(n,high), =VMOD(n,low TO high) (R0b9, R0ba): n brought into the range by the remainder of a 16-bit
// division (a negative n: from the top, so that an exact multiple gives high + 1)
int32_t amcaf_vmod_2(int32_t n, int32_t high)
{
    if (high <= 0)
        fon_call();
    uint32_t span = uint32_t(high) + 1;
    if (n >= 0)
        return int32_t(divu(uint32_t(n), uint16_t(span)) >> 16);
    uint32_t r = divu(uint32_t(-n), uint16_t(span)) >> 16;
    return int32_t(-int32_t(r) + int32_t(span));
}
int32_t amcaf_vmod_3(int32_t n, int32_t low, int32_t high)
{
    int32_t span = high - low;
    if (span <= 0)
        fon_call();
    int32_t d3 = n - low;
    if (d3 >= 0) {
        span += 1;
        return int32_t(divs(d3, int16_t(span)) >> 16) + low;
    }
    d3 = -d3;
    span += 1;
    uint32_t r = divu(uint32_t(d3), uint16_t(span)) >> 16;
    return -int32_t(r) + span + low;
}

// =QSIN(angle,radius), =QCOS(angle,radius) (R112, R111): the radius times the sine of the angle (1024 to
// the circle) from the library's table, as a 16-bit product shifted by 8 with the last bit added
int32_t amcaf_qsin(int32_t angle, int32_t radius)
{
    if (!radius)
        return 0;
    int32_t product = int32_t(sine_table()[angle & 1023]) * int16_t(radius);
    int32_t rounded = (product >> 8) + ((product >> 7) & 1);
    return int16_t(rounded);
}
int32_t amcaf_qcos(int32_t angle, int32_t radius)
{
    // (the library adds 256 to the low word of the angle)
    uint32_t a = uint32_t(angle);
    a = (a & 0xFFFF0000u) | uint16_t(uint16_t(a) + 256);
    return amcaf_qsin(int32_t(a), radius);
}

// =QSQR(n) (R10f): the square root by Newton's iteration on 16-bit divisions, the result for a large n
// shifted back from n / 4 ...
int32_t amcaf_qsqr(int32_t n)
{
    if (!n)
        return 0;
    if (n < 0)
        fon_call();
    uint32_t value = uint32_t(n);
    unsigned shifts = 0;
    auto root = [](uint32_t v) {
        uint32_t guess;
        uint32_t next = v;
        do {
            guess = next;
            uint32_t quotient = divu(v, uint16_t(guess)) & 0xFFFF;
            next = (guess + quotient + 1) >> 1;
            // (the sum is a long; the carry of the shift is added: (g + q + 1) / 2 rounded up)
            next = (guess + quotient) >> 1;
            next += (guess + quotient) & 1;
        } while (guess != next);
        return next;
    };
    if (value < 65536)
        return int32_t(root(value));
    do {
        shifts++;
        value >>= 2;
    } while (value >= 65536);
    if (!uint16_t(value))
        return int32_t(1u << shifts);
    return int32_t(root(value) << shifts);
}

// The ratio table; an index past it (a vector wider than 16 bits) read the library's code after it: 0 here
uint16_t arc_ratio(uint16_t index) { return index < 513 ? math::ArcRatio[index] : 0; }

// =QARC(x,y) (R113): the angle (1024 to the circle) of a vector, from a table of ratios of 512ths
int32_t amcaf_qarc(int32_t x, int32_t y)
{
    if (!x && !uint16_t(y))
        return 0;
    uint32_t ax = x < 0 ? uint32_t(-x) : uint32_t(x), ay = y < 0 ? uint32_t(-y) : uint32_t(y);
    uint16_t angle;
    if (int32_t(ax - ay) >= 0) {
        uint32_t r = divu(ay << 8 << 1, uint16_t(ax));
        angle = arc_ratio(uint16_t(r));
    } else {
        uint32_t r = divu(ax << 8 << 1, uint16_t(ay));
        angle = uint16_t(256 - arc_ratio(uint16_t(r)));
    }
    if (int16_t(x) < 0) {
        if (int16_t(y) < 0)
            angle = uint16_t(angle - 512);
        else
            angle = uint16_t(512 - angle);
    } else if (int16_t(y) < 0) {
        angle = uint16_t(-int16_t(angle));
    }
    return angle & 1023;
}

// =QRND(n) (R110): a random number from 0 to n, from a seed the beam's position stirs (n = 0: the last again)
int32_t amcaf_qrnd(int32_t n)
{
    uint16_t beam = uint16_t(hal::custom().vhposr);
    unsigned turn = beam & 15;
    uint16_t rolled = uint16_t(beam << turn | beam >> ((16 - turn) & 15));
    static uint16_t seed, last;
    seed = uint16_t(seed + rolled);
    if (!n)
        return last;
    uint32_t product = uint32_t(uint16_t(seed >> 1)) * uint16_t(n);
    uint32_t value = (product >> 15) + ((product >> 14) & 1);
    last = uint16_t(value);
    return int32_t(value);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_math.inc"
#endif
