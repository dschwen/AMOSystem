// String functions of AMOSPro.Lib

#include "lib/strings.h"

using namespace amos;

namespace {

// Copy of s with every character mapped by convert (Upper$, Lower$)
template <typename F> uint32_t map_string(const uint8_t *s, F convert)
{
    uint16_t len = str_len(s);
    if (!len)
        return str_empty();
    uint8_t *result = str_alloc(len);
    const uint8_t *from = str_chars(s);
    uint8_t *to = result + 2;
    for (uint16_t i = 0; i < len; i++)
        *to++ = convert(*from++);
    return str_finish(result, to);
}

// n copies of a character pair (the old RString). The length is a word: as
// in the assembler, n=65536 gives the empty string.
uint32_t repeat_pair(int32_t n, uint16_t pair)
{
    if (n < 0)
        fon_call();
    uint8_t *result = str_alloc(n);
    uint16_t len = static_cast<uint16_t>(n);
    poke_w(result, len);
    uint8_t *to = result + 2;
    for (uint32_t words = (len + 1u) / 2; words; words--, to += 2)
        poke_w(to, pair);
    idata().HiChaine = address(to);
    return address(result);
}

} // namespace

// =UPPER$(a$): only a-z change
extern "C" uint32_t lib_fn_upper(const uint8_t *s)
{
    return map_string(s, [](uint8_t c) { return c >= 'a' && c <= 'z' ? uint8_t(c - 0x20) : c; });
}

// =LOWER$(a$): only A-Z change
extern "C" uint32_t lib_fn_lower(const uint8_t *s)
{
    return map_string(s, [](uint8_t c) { return c >= 'A' && c <= 'Z' ? uint8_t(c + 0x20) : c; });
}

// =SPACE$(n)
extern "C" uint32_t lib_fn_space(int32_t n)
{
    return repeat_pair(n, 0x2020);
}

// =STRING$(a$,n): n times the first character of a$. Empty if a$ is.
extern "C" uint32_t lib_fn_string(const uint8_t *s, int32_t n)
{
    if (!str_len(s))
        return repeat_pair(0, 0);
    uint8_t c = str_chars(s)[0];
    return repeat_pair(n, static_cast<uint16_t>(c << 8 | c));
}
