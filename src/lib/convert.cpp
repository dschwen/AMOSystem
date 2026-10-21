// Number to text conversions of AMOSPro.Lib

#include "lib/convert.h"

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
extern "C" char *lib_long_to_hex(char *out, uint32_t value, int32_t digits)
{
    return amos::long_to_hex(out, value, digits);
}

extern "C" char *lib_long_to_bin(char *out, uint32_t value, int32_t digits)
{
    return amos::long_to_bin(out, value, digits);
}
