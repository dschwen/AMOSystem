// Walking the lines of a program in memory (+Verif.s Tk_FindL...), for
// AMOSPro and the editor. A line starts with its length / 2 (0: the end of
// the program); a closed procedure (Procedure token with bit 7 of its flags,
// byte 10 of the line) counts as one line, its contents skipped with the
// length in bytes 4-7.
#pragma once

#include <stdint.h>

namespace amos::source {

struct Found {
    uint8_t *line;
    uint8_t *procedure;         // the open procedure the line is in, or 0
};

// Tk_FindL: line n from start (Tk_FindN: n = 1, the next one)
Found find_line(uint8_t *start, uint16_t n);
// Tk_SizeL: the size of the line (a closed procedure: all of it)
uint32_t line_size(const uint8_t *line);
// Tk_EditL: not a closed procedure
bool editable(const uint8_t *line);

// Tk_FindA: the number of the line at or before address at, from start,
// its start and the start of the closed procedure it is in (or 0)
struct Number {
    uint32_t number;
    uint8_t *line;
    uint8_t *procedure;
};
Number find_address(uint8_t *start, const uint8_t *at);

// TInst: past the data of token t at a (0: the end of the line)
const uint8_t *skip_token(uint16_t t, const uint8_t *a);
inline uint8_t *skip_token(uint16_t t, uint8_t *a)
{
    return const_cast<uint8_t *>(skip_token(t, static_cast<const uint8_t *>(a)));
}

} // namespace amos::source
