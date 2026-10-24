// ValRout (+ILib.s): the number at the start of a text, for Val, Input,
// the tokenizer and the equates: an integer (decimal, $hexadecimal,
// %binary) or a float. Spaces are skipped inside decimal and binary
// numbers; an E of an exponent is made lower case in the text.

#include "interp/interp.h"

using namespace amos;
using namespace amos::interp;

namespace {

struct Text {
    const uint8_t *p;
};

// minichr: the next character as a digit (letters upper case, spaces
// skipped), 0xFF at the end
uint8_t digit(Text &t, bool spaces)
{
    for (;;) {
        uint8_t c = *t.p++;
        if (!c)
            return 0xFF;
        if (spaces && c == ' ')
            continue;
        if (c >= 'a')
            c -= 'a' - 'A';
        return uint8_t(c - '0');
    }
}

// declong / hexalong / binlong: 0 a number, 1 too large (the text where
// it started), -1 no digit. The text past the number.
struct Long {
    int status;
    uint32_t value;
};

Long decimal_from(Text &t, const uint8_t *start, uint32_t v, uint32_t count)
{
    for (;;) {
        uint8_t d = digit(t, true);
        if (d >= 10)
            break;
        uint32_t high = (v >> 16) * 10, low = (v & 0xFFFF) * 10;
        if (high >> 16) {
            t.p = start;
            return {1, v};
        }
        uint32_t n = (high << 16) + low;
        if (n < low) {
            t.p = start;
            return {1, n};
        }
        n += d;
        if (int32_t(n) < 0) {
            t.p = start;
            return {1, n};
        }
        v = n;
        count++;
    }
    t.p--;
    return {count ? 0 : -1, v};
}

Long hexadecimal(Text &t)
{
    const uint8_t *start = t.p;
    uint32_t v = 0, count = 0;
    for (;;) {
        uint8_t d = digit(t, false);
        if (d >= 10) {
            if (d < 17)
                break;
            d -= 7;
        }
        if (d >= 16)
            break;
        v = (v << 4) | d;
        if (++count == 9) {
            t.p = start;
            return {1, v};
        }
    }
    t.p--;
    return {count ? 0 : -1, v};
}

Long binary(Text &t)
{
    const uint8_t *start = t.p;
    uint32_t v = 0, count = 0;
    for (;;) {
        uint8_t d = digit(t, true);
        if (d >= 2)
            break;
        bool out = v & 0x80000000;
        v = (v << 1) | d;
        if (out) {
            t.p = start;
            return {1, v};
        }
        // (as the original: a 33rd digit goes on as a decimal number)
        if (++count == 33)
            return decimal_from(t, start, v, count);
    }
    t.p--;
    return {count ? 0 : -1, v};
}

struct Number {
    bool ok;
    uint16_t token;
    Value value;
    const uint8_t *end;
};

Number read_number(uint8_t *text, bool sign)
{
    IData &d = idata();
    Text t = {text};
    uint8_t *from = text;
    bool negative = false;
    Number bad = {false, 0, {TInt, 0, 0}, text};
    uint8_t c;
    if (sign) {
        do {
            c = *t.p++;
            if (!c)
                return bad;
        } while (c == ' ');
        from = const_cast<uint8_t *>(t.p) - 1;
        if (c == '-')
            negative = true;
        else if (c != '+')
            t.p--;
    }
    do {
        c = *t.p++;
        if (!c)
            return bad;
    } while (c == ' ');
    Long l;
    uint16_t token;
    if (c == '$') {
        l = hexadecimal(t);
        token = tk::Hex;
    } else if (c == '%') {
        l = binary(t);
        token = tk::Bin;
    } else {
        if (c != '.' && (c < '0' || c > '9'))
            return bad;
        t.p--;
        // a float: a point (the second one ends it) or an exponent
        uint8_t *q = const_cast<uint8_t *>(t.p);
        bool point = false, real = false;
        for (;;) {
            c = *q++;
            if (!c)
                break;
            if (c == ' ' || (c >= '0' && c <= '9'))
                continue;
            if (c == '.') {
                if (point)
                    break;
                point = true;
                continue;
            }
            if (c != 'e' && c != 'E')
                break;
            q[-1] = 'e';
            do
                c = *q++;
            while (c == ' ');
            if (c == '+' || c == '-') {
                real = true;
                do
                    c = *q++;
                while (c == ' ');
            }
            while (c >= '0' && c <= '9') {
                real = true;
                do
                    c = *q++;
                while (c == ' ');
            }
            break;
        }
        if (point || real) {
            // into BuFloat without the spaces, then the float layer
            q--;
            uint8_t *to = d.BuFloat;
            int room = 33;
            for (uint8_t *p = from; p != q && room;) {
                uint8_t x = *p++;
                if (x == ' ')
                    continue;
                *to++ = x;
                room--;
            }
            to[0] = to[1] = 0;
            Regs r;
            r.a[0] = address(d.BuFloat);
            lib_call(L_Ascii2Float, r);
            if (int8_t(d.MathFlags) < 0)
                return {true, tk::DFl, {TFloat, r.d[0], r.d[1]}, q};
            return {true, tk::Fl, {TFloat, r.d[0], 0}, q};
        }
        l = decimal_from(t, t.p, 0, 0);
        token = tk::Ent;
    }
    if (l.status)
        return bad;
    uint32_t v = negative ? 0u - l.value : l.value;
    return {true, token, {TInt, v, l.value}, t.p};
}

} // namespace

extern "C" {

// ValRout: a0 the text, d0 not 0 to take a sign. Out: d2/d3/d4 the
// value, d1 its token, a0 past it; the condition codes BEQ if there was
// one (d0 the integer before its sign, 0 for a float), else d0 = 1, the
// value 0 and a0 where it started.
uint32_t interp_val_rout(RegFrame &r)
{
    Number n = read_number(ptr<uint8_t>(r.a[0]), uint16_t(r.d[0]));
    if (!n.ok) {
        r.d[2] = 0;
        r.d[3] = 0;
        r.d[0] = 1;
        return 1;
    }
    r.d[1] = (r.d[1] & 0xFFFF0000) | n.token;
    r.d[2] = n.value.type;
    r.d[3] = n.value.v1;
    if (n.value.type == TFloat) {
        r.d[0] = 0;
        if (n.token == tk::DFl)
            r.d[4] = n.value.v2;
    } else {
        r.d[0] = n.value.v2;
    }
    r.a[0] = address(n.end);
    return 0;
}
}
