// String functions of AMOSPro.Lib

#include "lib/convert.h"
#include "lib/strings.h"

using namespace amos;

namespace {

// Copy of s with every character mapped by convert (Upper$, Lower$)
template <typename F> uint32_t map_string(GuestString s, F convert)
{
    uint16_t len = str_len(s);
    if (!len)
        return str_empty();
    StringBuilder result(len);
    for (uint16_t i = 0; i < len; i++)
        result.put(convert(s.at(i)));
    return result.finish();
}

// n copies of a character pair (the old RString). The length is a word: as
// in the assembler, n=65536 gives the empty string.
uint32_t repeat_pair(int32_t n, uint16_t pair)
{
    if (n < 0)
        fon_call();
    StringBuilder result(n);
    uint16_t len = static_cast<uint16_t>(n);
    for (uint32_t words = (len + 1u) / 2; words; words--)
        result.put_word(pair);
    return result.finish_raw(len);
}

} // namespace

// =UPPER$(a$): only a-z change
extern "C" uint32_t lib_fn_upper(GuestString s)
{
    return map_string(s, [](uint8_t c) { return c >= 'a' && c <= 'z' ? uint8_t(c - 0x20) : c; });
}

// =LOWER$(a$): only A-Z change
extern "C" uint32_t lib_fn_lower(GuestString s)
{
    return map_string(s, [](uint8_t c) { return c >= 'A' && c <= 'Z' ? uint8_t(c + 0x20) : c; });
}

// =SPACE$(n)
extern "C" uint32_t lib_fn_space(int32_t n)
{
    return repeat_pair(n, 0x2020);
}

// =STRING$(a$,n): n times the first character of a$. Empty if a$ is.
extern "C" uint32_t lib_fn_string(GuestString s, int32_t n)
{
    if (!str_len(s))
        return repeat_pair(0, 0);
    uint8_t c = s.at(0);
    return repeat_pair(n, static_cast<uint16_t>(c << 8 | c));
}

namespace {

constexpr uint32_t EntNul = 0x80000000;     // an omitted parameter

// The one-character strings (L_FinChr)
uint32_t char_string(uint8_t c)
{
    StringBuilder s(2);
    s.put(c);
    s.skip(1);
    return s.finish_raw(1);
}

// "Illegal function call" of the window library (L_WFonCall)
[[noreturn]] void w_fon_call() { screen_error(16); }

} // namespace

// =FLIP$(a$). The assembler took the length from a register whose high
// word was not cleared, and failed with "out of string space".
extern "C" uint32_t lib_fn_flip(GuestString s)
{
    uint16_t len = str_len(s);
    if (!len)
        return str_empty();
    StringBuilder result(len);
    for (uint16_t i = len; i; i--)
        result.put(s.at(i - 1));
    return result.finish();
}

// =LEN(a$)
extern "C" uint32_t lib_fn_len(GuestString s) { return str_len(s); }

// =ASC(a$): the first character, 0 for the empty string
extern "C" uint32_t lib_fn_asc(GuestString s) { return str_len(s) ? s.at(0) : 0; }

// =CHR$(n)
extern "C" uint32_t lib_fn_chr(uint32_t n)
{
    if (n >= 0x100)
        fon_call();
    return char_string(static_cast<uint8_t>(n));
}

// =TAB$, =CLEFT$, =CRIGHT$, =CUP$, =CDOWN$
extern "C" uint32_t lib_fn_tab() { return char_string(9); }
extern "C" uint32_t lib_fn_cleft() { return char_string(29); }
extern "C" uint32_t lib_fn_cright() { return char_string(28); }
extern "C" uint32_t lib_fn_cup() { return char_string(30); }
extern "C" uint32_t lib_fn_cdown() { return char_string(31); }

namespace {

// ESC code n (L_FPn): Pen$, Paper$
uint32_t escape_colour(char code, uint32_t n)
{
    if (n >= 32)
        w_fon_call();
    const char text[3] = {27, code, static_cast<char>('0' + n)};
    return str_from(text, 3);
}

// Coordinates of At and Cmove$: an ESC sequence for each one given (y
// first in the parameters' order, but x first in the string)
template <typename Check, typename Encode>
uint32_t escape_xy(int32_t x, int32_t y, char x_code, char y_code, Check given, Encode encode)
{
    char text[6];
    int len = 0;
    bool has_y = given(y);
    bool has_x = given(x);
    if (!has_x && !has_y)
        return str_empty();
    if (has_x) {
        text[len++] = 27;
        text[len++] = x_code;
        text[len++] = encode(x);
    }
    if (has_y) {
        text[len++] = 27;
        text[len++] = y_code;
        text[len++] = encode(y);
    }
    return str_from(text, static_cast<uint16_t>(len));
}

// Repeat$, Border$, Zone$ (L_FinRpt): ESC code "0", a$, ESC code n
uint32_t escape_around(char code, GuestString s, uint32_t n)
{
    uint16_t len = str_len(s);
    StringBuilder result(len + 6u);
    result.put(27);
    result.put(code);
    result.put('0');
    result.put(s, 0, len);
    result.put(27);
    result.put(code);
    result.put(static_cast<uint8_t>('0' + n));
    return result.finish();
}

} // namespace

// =PEN$(n), =PAPER$(n)
extern "C" uint32_t lib_fn_pen(uint32_t n) { return escape_colour('P', n); }
extern "C" uint32_t lib_fn_paper(uint32_t n) { return escape_colour('B', n); }

// =AT(x,y): either may be omitted
extern "C" uint32_t lib_fn_at(int32_t x, int32_t y)
{
    auto given = [](int32_t v) {
        if (static_cast<uint32_t>(v) == EntNul)
            return false;
        if (static_cast<uint32_t>(v) > 255 - 48)
            w_fon_call();
        return true;
    };
    return escape_xy(x, y, 'X', 'Y', given, [](int32_t v) { return static_cast<char>('0' + v); });
}

// =CMOVE$(dx,dy): 0 or omitted moves nothing
extern "C" uint32_t lib_fn_cmove(int32_t x, int32_t y)
{
    auto given = [](int32_t v) {
        if (!v || static_cast<uint32_t>(v) == EntNul)
            return false;
        if (v >= 128 || v <= -128)
            w_fon_call();
        return true;
    };
    return escape_xy(x, y, 'N', 'O', given, [](int32_t v) { return static_cast<char>(v + 128); });
}

// =REPEAT$(a$,n)
extern "C" uint32_t lib_fn_repeat(GuestString s, uint32_t n)
{
    if (!n || n >= 207)
        w_fon_call();
    return escape_around('R', s, n);
}

// =BORDER$(a$,n)
extern "C" uint32_t lib_fn_border(GuestString s, uint32_t n)
{
    if (!n || n >= 16)
        w_fon_call();
    return escape_around('E', s, n);
}

// =ZONE$(a$,n)
extern "C" uint32_t lib_fn_zone(GuestString s, uint32_t n)
{
    if (!n || n >= 255 - 48)
        w_fon_call();
    return escape_around('Z', s, n);
}

// =HEX$(n[,digits]), =BIN$(n[,digits]) (L_BinHex)
extern "C" uint32_t lib_fn_hex1(uint32_t n)
{
    StringBuilder s(9);
    char text[9];
    s.put(text, static_cast<uint32_t>(long_to_hex(text, n, -1) - text));
    return s.finish();
}

extern "C" uint32_t lib_fn_hex2(uint32_t n, int32_t digits)
{
    StringBuilder s(9);
    char text[9];
    s.put(text, static_cast<uint32_t>(long_to_hex(text, n, digits) - text));
    return s.finish();
}

extern "C" uint32_t lib_fn_bin1(uint32_t n)
{
    StringBuilder s(33);
    char text[33];
    s.put(text, static_cast<uint32_t>(long_to_bin(text, n, -1) - text));
    return s.finish();
}

extern "C" uint32_t lib_fn_bin2(uint32_t n, int32_t digits)
{
    StringBuilder s(33);
    char text[33];
    s.put(text, static_cast<uint32_t>(long_to_bin(text, n, digits) - text));
    return s.finish();
}

namespace {

// The search of L_InstrFind: position (from 1) of the needle in the haystack,
// searching from position start, or 0. The lengths are words, as in the
// assembler, and the result keeps the high word of start-1. h(i) and n(i)
// are the characters i of the haystack and the needle.
template <typename H, typename N>
uint32_t instr_scan(H h, uint32_t hay_len, N n, uint32_t needle_len, uint32_t start)
{
    if (!needle_len)
        return 0;
    if (start)
        start--;
    uint32_t hay = start;
    uint32_t pos = start;
    auto next = [](uint32_t v) { return (v & 0xFFFF0000u) | ((v + 1) & 0xFFFF); };
    uint16_t len = static_cast<uint16_t>(hay_len);
    uint16_t nlen = static_cast<uint16_t>(needle_len);
    for (;;) {
        uint32_t ni;
        do {
            ni = 0;
            pos = next(pos);
            if (static_cast<uint16_t>(pos) > len)
                return 0;
        } while (h(hay++) != n(ni++));
        uint32_t hi = hay;
        uint16_t at = static_cast<uint16_t>(pos);
        uint16_t matched = 0;
        for (;;) {
            if (++matched >= nlen)
                return pos;
            if (++at > len)
                return 0;
            if (h(hi++) != n(ni++))
                break;
        }
    }
}

uint32_t instr_strings(GuestString hay, GuestString needle, uint32_t start)
{
    return instr_scan([&](uint32_t i) { return hay.at(i); }, hay.size(),
                      [&](uint32_t i) { return needle.at(i); }, needle.size(), start);
}

} // namespace

// The same for characters at cursors
uint32_t amos::instr_find_cursor(ContextCursor hay, uint32_t hay_len, ContextCursor needle, uint32_t needle_len,
                                  uint32_t start)
{
    return instr_scan([&](uint32_t i) { return host::detail::read_cursor_byte(hay + i); }, hay_len,
                      [&](uint32_t i) { return host::detail::read_cursor_byte(needle + i); }, needle_len, start);
}

// The same for characters in host memory
extern "C" uint32_t lib_instr_find(const uint8_t *hay, uint32_t hay_len, const uint8_t *needle,
                                   uint32_t needle_len, uint32_t start)
{
    return instr_scan([&](uint32_t i) { return hay[i]; }, hay_len, [&](uint32_t i) { return needle[i]; },
                      needle_len, start);
}

// The same with registers. In: a1/d1 = string and length, a2/d2 = string
// to find and length, d4 = start. Out: d3 = d0 = position or 0.
extern "C" uint32_t lib_instr_find_reg(RegFrame &r)
{
    ContextCursor hay = decode_context_cursor(r.a[1]);
    ContextCursor needle = decode_context_cursor(r.a[2]);
    r.d[3] = r.d[0] = instr_scan([&](uint32_t i) { return host::detail::read_cursor_byte(hay + i); }, r.d[1],
                                 [&](uint32_t i) { return host::detail::read_cursor_byte(needle + i); }, r.d[2],
                                 r.d[4]);
    return r.d[0];
}

// =INSTR(a$,b$[,start])
extern "C" uint32_t lib_fn_instr2(GuestString hay, GuestString needle)
{
    return instr_strings(hay, needle, 0);
}

extern "C" uint32_t lib_fn_instr3(GuestString hay, GuestString needle, int32_t start)
{
    if (start < 0)
        fon_call();
    return instr_strings(hay, needle, start);
}

// =FREE: the free string space, after a garbage collection
extern "C" uint32_t lib_fn_free()
{
    Regs r = {};
    r.d[3] = static_cast<uint32_t>(-100);
    lib_call(L_Menage, r);
    return idata().TabBas - idata().HiChaine;
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_strings.inc"
#endif
