// Number to text conversions of AMOSPro.Lib

#include "lib/convert.h"

#include "core/amos.h"

namespace amos {

namespace {

// Digits of value in base 1 << bits, most significant first. As in the
// assembler, a digit count outside 0..count (or negative) means "no
// leading zeros", and 0 writes every digit.
char *to_base(char *out, uint32_t value, int32_t digits, int bits, int count)
{
    int32_t skip = digits < 0 ? digits : static_cast<int32_t>(static_cast<uint32_t>(count) - static_cast<uint32_t>(digits));
    bool started = false;
    uint32_t mask = (1u << bits) - 1;
    for (int i = count - 1; i >= 0; i--) {
        uint32_t digit = (value >> (i * bits)) & mask;
        if (skip > 0) {
            skip--;
            continue;
        }
        if (skip < 0 && !started && i != 0) {
            if (!digit)
                continue;
            started = true;
        }
        *out++ = static_cast<char>(digit < 10 ? '0' + digit : 'A' + digit - 10);
    }
    return out;
}

} // namespace

char *long_to_dec(char *out, int32_t value, int32_t digits, bool space)
{
    uint32_t v = static_cast<uint32_t>(value);
    if (value < 0) {
        *out++ = '-';
        v = 0u - v;
    } else if (space) {
        *out++ = ' ';
    }
    int32_t skip = digits < 0 ? digits : static_cast<int32_t>(10u - static_cast<uint32_t>(digits));
    bool started = false;
    uint32_t power = 1000000000;
    for (int i = 9; i >= 0; i--, power /= 10) {
        uint32_t digit = v / power;
        v %= power;
        if (skip > 0) {
            skip--;
            continue;
        }
        if (skip < 0 && !started && i != 0) {
            if (!digit)
                continue;
            started = true;
        }
        *out++ = static_cast<char>('0' + digit);
    }
    return out;
}

char *long_to_hex(char *out, uint32_t value, int32_t digits)
{
    *out++ = '$';
    return to_base(out, value, digits, 4, 8);
}

char *long_to_bin(char *out, uint32_t value, int32_t digits)
{
    *out++ = '%';
    return to_base(out, value, digits, 1, 32);
}

} // namespace amos

// Register entries for the assembler (the editor uses them too)

// L_LongToAsc: d0 = value, d3 = digits, d4.w = space for positive, a0 =
// output. Out: a0 after the text.
extern "C" uint32_t lib_long_to_asc(amos::RegFrame &r)
{
    r.a[0] = amos::address(amos::long_to_dec(amos::ptr<char>(r.a[0]),
                                                           static_cast<int32_t>(r.d[0]),
                                                           static_cast<int32_t>(r.d[3]),
                                                           static_cast<uint16_t>(r.d[4]) != 0));
    return 0;
}

// L_LongToDec: d0 = value, a0 = output, no leading zeros or space
extern "C" uint32_t lib_long_to_dec(amos::RegFrame &r)
{
    r.a[0] = amos::address(amos::long_to_dec(amos::ptr<char>(r.a[0]),
                                                           static_cast<int32_t>(r.d[0]), -1, false));
    return 0;
}
extern "C" char *lib_long_to_hex(char *out, uint32_t value, int32_t digits)
{
    return amos::long_to_hex(out, value, digits);
}

extern "C" char *lib_long_to_bin(char *out, uint32_t value, int32_t digits)
{
    return amos::long_to_bin(out, value, digits);
}
