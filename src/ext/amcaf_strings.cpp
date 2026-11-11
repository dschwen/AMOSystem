// AMCAF: the string functions (R0b1, R0b2, R0bb-R0bf, R116): numbers with leading zeros or spaces,
// strings inserted, cut and replaced in, an item of a list, the name of a key. They build their
// strings as the library does (a string of the length it computes), with its edge cases: Cutstr$
// takes the character after the end of the string when it is asked to cut up to the end.

#include "core/guest_scratch.h"
#include "ext/amcaf.h"
#include "ext/amcaf_pt_tables.h"
#include "lib/strings.h"

using namespace amos;
using namespace amos::amcaf;

namespace {

// The powers of 10 the numbers are divided by, the last `digits` of them
constexpr uint32_t Powers[10] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};

uint8_t byte_after(GuestString s, uint32_t i) { return host::detail::read_cursor_byte(s.chars() + i); }

}  // namespace

extern "C" {

// =LZSTR$(n,digits) (R0b1): n with leading zeros in `digits` places (1 to 10): all 9s if it does not fit
uint32_t amcaf_lzstr_s(int32_t value, int32_t digits)
{
    if (!digits || uint16_t(digits) > 10)
        fon_call();
    if (uint32_t(digits) >> 16)
        error(11);                                  // (the library asks for 65536 bytes or more: "string too long")
    uint16_t length = uint16_t(digits);
    StringBuilder b(length);
    uint8_t out[16] = {};
    unsigned at = 0;
    int16_t left = int16_t(length) - 1;
    const uint32_t *power = Powers + 10 - length;
    int32_t d0 = value;
    bool nines = false;
    if (d0 < 0) {
        out[at++] = '-';
        if (--left < 0)
            goto done;
        {
            uint32_t divisor = *power++;
            d0 = -d0;
            if (d0 >= int32_t(divisor))
                nines = true;
        }
    }
    if (!nines) {
        for (;;) {
            out[at] = '0';
            uint32_t divisor = *power++;
            while (d0 >= int32_t(divisor)) {
                if (out[at] == '9') {
                    nines = true;
                    break;
                }
                out[at]++;
                d0 -= int32_t(divisor);
            }
            if (nines)
                break;
            at++;
            if (--left < 0)
                goto done;
        }
    }
    for (; left >= 0; left--)
        out[at++] = '9';
done:
    b.put(out, length);
    return b.finish_raw(length);
}

// =LSSTR$(n,digits) (R0b2): n with leading spaces in `digits` places (1 to 10): all 9s if it does not fit
uint32_t amcaf_lsstr_s(int32_t value, int32_t digits)
{
    if (!digits || uint16_t(digits) > 10)
        fon_call();
    if (uint32_t(digits) >> 16)
        error(11);                                  // (the library asks for 65536 bytes or more: "string too long")
    uint16_t length = uint16_t(digits);
    StringBuilder b(length);
    uint8_t out[16] = {};
    unsigned at = 0;
    int16_t left = int16_t(length) - 1;
    const uint32_t *power = Powers + 10 - length;
    int32_t d0 = value;
    uint16_t seen = 0, position = 0, negative = 0;
    bool overflow = false;
    if (d0 < 0) {
        d0 = -d0;
        negative = 1;
        if (!(d0 < int32_t(*power)))
            overflow = true;
    }
    if (overflow) {
        out[at++] = '-';
        if (--left >= 0)
            for (; left >= 0; left--)
                out[at++] = '9';
    } else {
        for (;;) {
            out[at] = '0';
            uint32_t divisor = *power++;
            bool nines = false;
            if (d0 < int32_t(divisor)) {
                if (!seen && left != 0)
                    out[at] = ' ';
            } else {
                for (;;) {
                    if (negative) {
                        negative = 0;
                        if (position)
                            out[at - 1] = '-';
                    }
                    if (out[at] == '9') {
                        nines = true;
                        break;
                    }
                    out[at]++;
                    d0 -= int32_t(divisor);
                    if (!(d0 >= int32_t(divisor)))
                        break;
                }
                if (nines) {
                    for (; left >= 0; left--)
                        out[at++] = '9';
                    break;
                }
                seen = 0xFFFF;
            }
            position++;
            at++;
            if (--left < 0)
                break;
        }
    }
    b.put(out, length);
    return b.finish_raw(length);
}

// =INSSTR$(a$,b$,n) (R0bb): b$ put into a$ after its n first characters
uint32_t amcaf_insstr_s(GuestString base, GuestString inserted, int32_t n)
{
    if (n < 0)
        fon_call();
    uint16_t base_length = str_len(base);
    if (uint16_t(n) > base_length)
        fon_call();
    uint16_t add = str_len(inserted);
    if (!add)
        return base.word();
    uint16_t total = uint16_t(base_length + add);
    StringBuilder b(total);
    b.put(base, 0, uint16_t(n));
    b.put(inserted, 0, add);
    b.put(base, uint16_t(n), uint16_t(base_length - n));
    return b.finish_raw(total);
}

// =CUTSTR$(a$,first TO last) (R0bc): a$ without the characters from first to last. The library keeps the
// string as long as the length less (last - first) and so takes one character past the end of a$
// when `last` is its end
uint32_t amcaf_cutstr_s(GuestString s, int32_t first, int32_t last)
{
    if (last < 0)
        fon_call();
    if (first < 0 || !first)
        fon_call();
    int32_t span = last - first;
    if (span < 0)
        fon_call();
    int16_t length = int16_t(str_len(s));
    if (int16_t(length - int16_t(first)) < 0)
        fon_call();
    if (int16_t(length - int16_t(last)) < 0)
        fon_call();
    uint16_t result = uint16_t(uint16_t(length) - uint16_t(span));
    StringBuilder b(result);
    uint16_t copied = 0;
    int16_t left = length;
    for (int16_t i = 1; i != int16_t(first); i++, left--)
        b.put(byte_after(s, copied++));
    copied = uint16_t(copied + span + 1);
    left = int16_t(left - int16_t(span));
    while (left != 0) {
        left--;
        b.put(byte_after(s, copied++));
    }
    return b.finish_raw(result);
}

// =REPLACESTR$(a$,old$ TO new$) (R0bd): every old$ in a$ (from the left, not overlapping) made new$
uint32_t amcaf_replacestr_s(GuestString text, GuestString old, GuestString replacement)
{
    uint16_t text_length = str_len(text);
    if (!text_length)
        return text.word();
    uint16_t old_length = str_len(old);
    if (!old_length)
        fon_call();
    // the matches
    auto matches_at = [&](uint32_t at) {
        if (at + old_length > text_length)
            return false;
        for (uint16_t k = 0; k < old_length; k++)
            if (byte_after(text, at + k) != byte_after(old, k))
                return false;
        return true;
    };
    uint32_t count = 0;
    for (uint32_t at = 0; at < text_length;) {
        if (matches_at(at)) {
            count++;
            at += old_length;
        } else {
            at++;
        }
    }
    if (!count)
        return text.word();
    uint16_t replacement_length = str_len(replacement);
    int32_t total = int16_t(uint16_t(replacement_length - old_length)) * int16_t(count);
    uint16_t length = uint16_t(total + text_length);
    StringBuilder b(length);
    for (uint32_t at = 0; at < text_length;) {
        if (matches_at(at)) {
            b.put(replacement, 0, replacement_length);
            at += old_length;
        } else {
            b.put(byte_after(text, at++));
        }
    }
    return b.finish_raw(length);
}

// =ITEMSTR$(a$,n[,separator$]) (R0be, R0bf): the item n (from 0) of a list a$ whose items end with the
// separator (one character: "|" by default)
uint32_t amcaf_itemstr_s_3(GuestString list, int32_t n, GuestString separator)
{
    if (str_len(separator) != 1)
        fon_call();
    uint8_t sep = separator.at(0);
    if (n < 0)
        fon_call();
    uint16_t left = str_len(list);
    if (!left)
        fon_call();
    uint32_t at = 0;
    uint16_t items = uint16_t(n);
    while (items) {
        uint8_t c = byte_after(list, at++);
        if (c == sep) {
            items--;
            if (!items)
                break;
            if (!--left)
                fon_call();
            continue;
        }
        if (!--left)
            fon_call();
    }
    uint16_t length = 0;
    uint16_t remaining = left;
    uint32_t start = at, scan = at;
    for (;;) {
        if (!--remaining)
            break;
        if (byte_after(list, scan++) == sep)
            break;
        length++;
    }
    if (!length)
        return str_empty();
    StringBuilder b(length);
    for (uint16_t i = 0; i < length; i++)
        b.put(byte_after(list, start + i));
    return b.finish_raw(length);
}

uint32_t amcaf_itemstr_s_2(GuestString list, int32_t n)
{
    static const uint8_t bar[4] = {0, 1, '|', 0};
    return amcaf_itemstr_s_3(list, n, GuestString::from_word(guest_constant(bar, sizeof bar)));
}

// =SCANSTR$(code) (R116): the name of a key, from its scancode (0 to 103; "" is an error)
uint32_t amcaf_scanstr_s(int32_t code)
{
    if (uint16_t(code) > 103)
        fon_call();
    const uint8_t *p = keys::Names;
    unsigned n = code & 127;
    while (n) {
        while (*p++)
            ;
        n--;
    }
    if (!*p)
        fon_call();
    uint16_t length = 0;
    while (p[length])
        length++;
    return str_from(p, length);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_strings.inc"
#endif
