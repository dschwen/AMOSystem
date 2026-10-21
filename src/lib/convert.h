// Number to text conversions of AMOSPro.Lib
#pragma once

#include <stdint.h>

namespace amos {

// L_LongToHex: "$" and the hexadecimal digits of value. digits > 0 and <= 8
// writes that many low digits; anything else leaves out the leading zeros.
// Returns the end of the text (not zero-terminated).
char *long_to_hex(char *out, uint32_t value, int32_t digits);

// L_LongToBin: the same in binary, "%" and up to 32 digits
char *long_to_bin(char *out, uint32_t value, int32_t digits);

// L_LongToAsc: value in decimal, "-" in front if negative, or " " if
// space is set. digits as above, out of 10.
char *long_to_dec(char *out, int32_t value, int32_t digits, bool space);

} // namespace amos
