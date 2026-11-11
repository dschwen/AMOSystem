// Turbo Plus: the integer functions, the memory searches and moves, the mouse buttons, the task's
// priority (R002-R00d, R03c, R041, R086-R090, R095-R099, R0a9, R0aa, R0b5). The routines are the
// library's, with the details of their arithmetic (the shifts' counts, the 16-bit divisions, the
// signed compares); the comments name them.

#include "ext/third_party.h"
#include "hal/chips.h"
#include "hal/system.h"
#include "lib/banks.h"
#include "lib/strings.h"

using namespace amos;

namespace {

// A shift as the 68000 does it: by the low six bits of the count, a width's worth of bits, the rest of
// the long as it was
uint32_t shift_left(uint32_t value, uint32_t count, unsigned width)
{
    count &= 63;
    uint32_t mask = width == 32 ? 0xFFFFFFFFu : (1u << width) - 1;
    uint32_t field = count >= width ? 0 : (value << count) & mask;
    return (value & ~mask) | field;
}

uint32_t shift_right(uint32_t value, uint32_t count, unsigned width)
{
    count &= 63;
    uint32_t mask = width == 32 ? 0xFFFFFFFFu : (1u << width) - 1;
    uint32_t field = count >= width ? 0 : (value & mask) >> count;
    return (value & ~mask) | field;
}

uint8_t byte_at(uint32_t at) { return host::detail::read_cursor_byte(decode_context_cursor(at)); }

}  // namespace

extern "C" {

// MULTI YES, MULTI NO (R002, R003): the task's priority 0 or 20
void turbo_multi_yes() { hal::set_task_priority(0); }
void turbo_multi_no() { hal::set_task_priority(20); }

// =LSL.B(n,count) ... =LSR.L(n,count) (R004-R009): the shift of the low byte, word or long (the rest stays)
int32_t turbo_lsl_b(int32_t n, int32_t count) { return int32_t(shift_left(uint32_t(n), uint32_t(count), 8)); }
int32_t turbo_lsl_w(int32_t n, int32_t count) { return int32_t(shift_left(uint32_t(n), uint32_t(count), 16)); }
int32_t turbo_lsl_l(int32_t n, int32_t count) { return int32_t(shift_left(uint32_t(n), uint32_t(count), 32)); }
int32_t turbo_lsr_b(int32_t n, int32_t count) { return int32_t(shift_right(uint32_t(n), uint32_t(count), 8)); }
int32_t turbo_lsr_w(int32_t n, int32_t count) { return int32_t(shift_right(uint32_t(n), uint32_t(count), 16)); }
int32_t turbo_lsr_l(int32_t n, int32_t count) { return int32_t(shift_right(uint32_t(n), uint32_t(count), 32)); }

// =TEST.B(a,b), =TEST.W(a,b) (R00b, R00c): -1 if the low bytes (words) are the same
int32_t turbo_test_b(int32_t a, int32_t b) { return uint8_t(a) == uint8_t(b) ? -1 : 0; }
int32_t turbo_test_w(int32_t a, int32_t b) { return uint16_t(a) == uint16_t(b) ? -1 : 0; }

// VBL WAIT line (R00d): until the beam's line (the low byte of VHPOSR's high byte) is that
void turbo_vbl_wait(int32_t line)
{
    while (uint8_t(uint16_t(hal::custom().vhposr) >> 8) != uint8_t(line))
        hal::idle();
}

// =RANGE(n,low TO high) (R08f): n between, the upper limit first (they are not put in order)
int32_t turbo_range(int32_t n, int32_t low, int32_t high)
{
    if (n > high)
        return high;
    if (n < low)
        return low;
    return n;
}

// =TEXP(test,a,b) (R090): a if test is not 0, else b (all three are worked out before)
int32_t turbo_texp(int32_t test, int32_t a, int32_t b) { return test ? a : b; }

// =T CLIP(n,m) (R095): n made a multiple of m (toward 0), by a signed 16-bit division and multiplication
int32_t turbo_t_clip(int32_t n, int32_t m)
{
    if (m <= 0)
        fon_call();
    int16_t divisor = int16_t(m);
    if (!divisor)
        fon_call();                                  // (the 68000's divide by zero)
    int32_t quotient = n / divisor;
    int32_t d3 = n;
    if (quotient >= -32768 && quotient <= 32767)
        d3 = int32_t(uint32_t(int32_t(n % divisor)) << 16 | uint16_t(quotient));
    return int32_t(int16_t(d3)) * int32_t(divisor);
}

// =BETWEEN(a,n,b) (R096): -1 if n is strictly between a and b (in either order); the value tested is the
// middle one
int32_t turbo_between(int32_t a, int32_t n, int32_t b)
{
    int32_t high = b, low = a;
    if (!(b > a)) {
        high = a;
        low = b;
    }
    if (n < high && n > low)
        return -1;
    return 0;
}

// The mask of a field of `width` bits: the library keeps a table of them
uint32_t field_mask(uint32_t width) { return width ? 0xFFFFFFFFu << width : 0xFFFFFFFFu; }

// =BIT FIELD EXT(n,start,width) (R087): the field, from the bit start, width bits (width modulo 32; 0: n as it is)
int32_t turbo_bit_field_ext(int32_t n, int32_t start, int32_t width)
{
    if (width < 0)
        fon_call();
    width &= 31;
    if (!width)
        return n;
    if (start < 0)
        fon_call();
    start &= 31;
    return int32_t((uint32_t(n) & (~field_mask(uint32_t(width)) << start)) >> start);
}

// =BIT FIELD INS(n,start,width,value) (R086): n with the field replaced by the low bits of value
int32_t turbo_bit_field_ins(int32_t n, int32_t start, int32_t width, int32_t value)
{
    if (width < 0)
        fon_call();
    width &= 31;
    if (!width)
        return n;
    if (start < 0)
        fon_call();
    start &= 31;
    if (uint16_t(start + width) > 32)
        fon_call();
    uint32_t mask = field_mask(uint32_t(width));
    uint32_t keep = (mask << start | mask >> (32 - start)) & uint32_t(n);       // (the mask rotated left)
    if (!start)
        keep = mask & uint32_t(n);
    uint32_t put = (uint32_t(value) & ~mask) << start;
    return int32_t(keep | put);
}

// =F SQR(n) (R041): the integer square root by the library's restoring loop, then 16-bit sign-extended
int32_t turbo_f_sqr(int32_t n)
{
    int32_t rest = n;
    uint32_t root = 0, bit = 0x40000000u;
    do {
        uint32_t trial = root + bit;
        root >>= 1;
        if (!(rest <= int32_t(trial))) {
            rest -= int32_t(trial);
            root |= bit;
        }
        bit >>= 2;
    } while (bit);
    if (!(rest < int32_t(root)))
        root++;
    return int32_t(int16_t(root));
}

// =BYTE HUNT(start TO end,action,low TO high) (R089): the first byte in the range (both ends in it, the
// lower first) that is low (action 0), inside low to high (above 0), outside it (below 0); signed bytes. 0 if none
int32_t turbo_byte_hunt(int32_t start, int32_t end, int32_t action, int32_t low, int32_t high)
{
    int32_t from = start, to = end;
    if (!(to > from)) {
        int32_t t = from;
        from = to;
        to = t;
    }
    int32_t left = int32_t(uint32_t(to) - uint32_t(from));
    uint32_t at = uint32_t(from);
    int8_t lo = int8_t(low), hi = int8_t(high);
    for (;;) {
        int8_t b = int8_t(byte_at(at));
        at++;
        bool found = action == 0 ? b == lo : action < 0 ? (b < lo || b > hi) : !(b < lo || b > hi);
        if (found)
            return int32_t(at - 1);
        if (--left < 0)
            return 0;
    }
}

// MEMORY FILL start TO end,a$ (R08c): a$ repeated over the range, the end included (the lower first)
void turbo_memory_fill(int32_t start, int32_t end, GuestString s)
{
    uint16_t length = str_len(s);
    if (!length)
        fon_call();
    uint32_t low = uint32_t(start), high = uint32_t(end);
    if (!(int32_t(high) >= int32_t(low))) {
        uint32_t t = low;
        low = high;
        high = t;
    }
    int32_t left = int32_t(high - low);
    uint8_t *to = ptr<uint8_t>(low);
    uint16_t at = 0;
    for (;;) {
        *to++ = s.at(at);
        if (++at == length)
            at = 0;
        if (--left < 0)
            return;
    }
}

// MOVE MEM start,end TO destination (R0b5): the bytes from start up to end (not including it) copied,
// the right way round for an overlap
void turbo_move_mem(int32_t start, int32_t end, int32_t to)
{
    uint32_t length = uint32_t(end) - uint32_t(start);
    if (uint32_t(end) <= uint32_t(start))
        fon_call();
    uint8_t *from = ptr<uint8_t>(uint32_t(start)), *destination = ptr<uint8_t>(uint32_t(to));
    if (destination < from) {
        for (uint32_t i = 0; i < length; i++)
            destination[i] = from[i];
    } else {
        for (uint32_t i = length; i--;)
            destination[i] = from[i];
    }
}

// =BANK END(bank) (R099, R138): the address after a bank's data; an icon or sprite bank: minus its images
int32_t turbo_bank_end(int32_t number)
{
    ContextCursor bank = number >= 0 && number < 1024 ? bank_find(uint32_t(number)) : ContextCursor{};
    if (!bank) {
        bank_or_address(number);                    // (the error of a bank that is not there)
        fon_call();
    }
    uint32_t name = peek_l(bank - 8);
    if (name == 0x49636F6E || name == 0x53707269)   // "Icon", "Spri"
        return -int32_t(int16_t(peek_w(bank)));
    return int32_t(encode_context_cursor(bank) + bank_size(bank));
}

// =STRING HUNT(start TO end,action,step,a$) (R0a9): the first place a$ is in the range, stepping by `step`
// (negative: from the end back); action 0 the text itself, another: no byte of it the same
int32_t turbo_string_hunt(int32_t start, int32_t end, int32_t action, int32_t step, GuestString s)
{
    uint16_t length = str_len(s);
    if (!step)
        fon_call();
    if (end <= start)
        fon_call();
    if (!length)
        return 0;                                   // (the library compares 65536 bytes of what follows)
    int32_t at = step < 0 ? end : start;
    int32_t left = end - start;
    for (;;) {
        uint16_t i = 0;
        for (; i < length; i++) {
            uint8_t a = s.at(i), b = byte_at(uint32_t(at) + i);
            if (action == 0 ? a != b : a == b)
                break;
        }
        if (i == length)
            return at;
        at += step;
        left -= step < 0 ? -step : step;
        if (left < 0)
            return 0;
    }
}

// =LEFT CLICK (R03c): -1 if the button of port 1's mouse (CIA A's /FIR0) is down
int32_t turbo_left_click() { return hal::cia_a_pra() & 0x40 ? 0 : -1; }

// =RIGHT CLICK (R0aa): -1 if the right button (POTGOR's bit 10) is down
int32_t turbo_right_click() { return uint16_t(hal::custom().potinp) & 0x0400 ? 0 : -1; }
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_turbo_misc.inc"
#endif
