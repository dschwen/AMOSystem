// Rainbows of AMOS.library's screen table (EcCall): Set Rainbow, Rainbow,
// Rainbow Del, Rain(), and hiding them. The copper list builder (+W.s)
// shows them.

#include "w/w.h"

using namespace amos;
using namespace amos::w;

namespace {

constexpr uint32_t NbRain = 4;
constexpr uint16_t PalMax = 16;
constexpr uint16_t BitEcrans = 1 << 12;     // T_Actualise: rebuild the copper list

inline void barrier() { asm volatile("" ::: "memory"); }

// A rainbow (T_RainTable): the lines it covers on the display (the
// copper builder's), its colour, its table of colours, what Rainbow
// changed (RnAct, its high byte: 1 height, 2 base, 4 y) and the values
struct Rainbow {
    uint16_t RnDY, RnFY, RnTY, RnBase, RnColor, RnLong;
    uint32_t RnBuf;
    uint16_t RnAct, RnX, RnY, RnI;
};
static_assert(sizeof(Rainbow) == 24);

// RainAd
Rainbow *rainbow(uint32_t n)
{
    if (int32_t(n) < 0 || uint16_t(n) >= NbRain)
        return nullptr;
    return reinterpret_cast<Rainbow *>(wdata().RainTable + int16_t(n * sizeof(Rainbow)));
}

uint8_t &action(Rainbow *r) { return reinterpret_cast<uint8_t *>(&r->RnAct)[0]; }

// RnDel: free rainbow n's table. 1 if no such rainbow.
uint32_t delete_one(uint32_t n)
{
    Rainbow *r = rainbow(n);
    if (!r)
        return 1;
    if (r->RnBuf) {
        r->RnDY = 0;
        r->RnFY = 0;
        barrier();
        uint32_t buffer = r->RnBuf;
        r->RnBuf = 0;
        uint16_t length = r->RnLong;
        r->RnLong = 0;
        free_mm(buffer, int16_t(length));
    }
    return 0;
}

// TRDel: rainbow n, or all of them (n < 0)
uint32_t delete_rainbow(uint32_t n)
{
    if (int32_t(n) >= 0)
        return delete_one(n);
    WData &d = wdata();
    d.RainBow = 0;
    d.OldRain = 0;
    for (uint16_t i = NbRain - 1; i; i--)
        delete_one(i);
    return delete_one(0);
}

// ---- the colour strings ---------------------------------------------------------

// The AMAL parser's AniChr/AniLong, which Set Rainbow uses
struct Parser {
    const uint8_t *p;
    bool failed = false;

    // The next character that counts: printable up to "Z", "|" or "!";
    // lower case and spaces are skipped, an escape sequence too. *end is
    // what the assembler's Z flag said: the end, or "Z", "|" or "!".
    uint8_t next(bool *end = nullptr)
    {
        for (;;) {
            uint8_t c = *p++;
            if (!c || (c >= 33 && c <= 'Z') || c == '|' || c == '!') {
                if (end)
                    *end = !c || c == 'Z' || c == '|' || c == '!';
                return c;
            }
            if (c == 27)
                p += 2;
        }
    }

    static int hex_digit(uint8_t c)
    {
        uint8_t v = c - '0';
        if (c < '0')
            return -1;
        if (v < 10)
            return v;
        v = c - 'A';
        if (c < 'A' || v >= 6)
            return -1;
        return v + 10;
    }

    // A decimal or $hexadecimal number, maybe negative
    int32_t number()
    {
        bool negative = false;
        uint8_t c = next();
        if (c == '-') {
            negative = true;
            c = next();
        }
        uint32_t v;
        if (c == '$') {
            int d = hex_digit(next());
            if (d < 0) {
                failed = true;
                return 0;
            }
            v = d;
            while ((d = hex_digit(next())) >= 0)
                v = (v << 4) + d;
        } else {
            uint8_t d = c - '0';
            if (c < '0' || d >= 10) {
                failed = true;
                return 0;
            }
            v = d;
            for (;;) {
                c = next();
                d = c - '0';
                if (c < '0' || d >= 10)
                    break;
                v = v * 10 + d;
            }
        }
        p--;
        return negative ? -int32_t(v) : int32_t(v);
    }
};

// RainTok: the string (length word, then the text) as a list of
// (count, step, times) words at to, ending with -1 (then 4 zero bytes;
// all zero for an empty string). Returns past it, or null for a syntax
// error.
uint8_t *tokenise(const uint8_t *string, uint8_t *to)
{
    poke_l(to, 0);
    poke_w(to + 4, 0);
    uint16_t length = peek_w(string);
    uint8_t *text = const_cast<uint8_t *>(string) + 2;
    uint8_t kept = text[length];
    text[length] = 0;
    Parser in = {text};
    bool end;
    uint8_t c = in.next(&end);
    bool ok = true;
    while (!end) {
        if (c != '(') {
            ok = false;
            break;
        }
        int32_t count = in.number();
        if (in.failed || count <= 0) {
            ok = false;
            break;
        }
        poke_w(to, count);
        if (in.next() != ',') {
            ok = false;
            break;
        }
        int32_t step = in.number();
        if (in.failed) {
            ok = false;
            break;
        }
        poke_w(to + 2, step);
        if (in.next() != ',') {
            ok = false;
            break;
        }
        int32_t times = in.number();
        if (in.failed || times < 0) {
            ok = false;
            break;
        }
        poke_w(to + 4, times);
        to += 6;
        if (in.next() != ')') {
            ok = false;
            break;
        }
        poke_w(to, 0xFFFF);
        poke_l(to + 2, 0);
        c = in.next(&end);
    }
    text[length] = kept;
    return ok ? to + 6 : nullptr;
}

// One component while the table is filled: the value, what is added to
// it every speed lines, the lines left before that, how many times
// before the next entry of the list, where that is
struct Component {
    uint16_t value, plus, count, speed, moves, position;
    const uint8_t *list;

    // One line further
    void step()
    {
        if (!count || --count)
            return;
        count = speed;
        value = (value + plus) & 15;
        if (!moves || --moves)
            return;
        uint16_t at = position;
        while (int16_t(count = peek_w(list + int16_t(at))) < 0)
            at = 0;
        speed = count;
        plus = peek_w(list + int16_t(at) + 2);
        moves = peek_w(list + int16_t(at) + 4);
        position = at + 6;
    }
};

// TRSet: rainbow d1, of d2 lines, changes colour d3 (0-15), from colour
// d7, with the strings d4/d5/d6 for red, green and blue. 1 for a syntax
// error (the rainbow is deleted), -1 out of memory.
uint32_t set_rainbow(AllRegs *r)
{
    WData &d = wdata();
    d.AMALSp = 0;
    if (delete_one(r->d[1]))
        return 1;
    Rainbow *rb = rainbow(r->d[1]);
    auto syntax = [&] {
        delete_rainbow(r->d[1]);
        return 1u;
    };
    uint16_t colour = r->d[3] & 31;
    if (colour >= PalMax)
        return syntax();
    rb->RnColor = colour;
    uint16_t start = r->d[7];
    uint16_t lines = r->d[2];
    uint32_t bytes = uint32_t(int32_t(int16_t(lines))) << 1;
    uint32_t buffer = fast_mm2(bytes);
    if (!buffer)
        return 0xFFFFFFFF;
    rb->RnBuf = buffer;
    rb->RnLong = bytes;
    rb->RnAct = 0;
    rb->RnI = 0xFFFF;
    rb->RnX = 0;
    rb->RnY = 0;
    rb->RnDY = 0;
    rb->RnFY = 0;
    rb->RnTY = 0;

    // Blue, green and red lists, one after the other in the Buffer
    uint8_t *list = ptr<uint8_t>(idata().Buffer);
    Component c[3];
    const uint32_t strings[3] = {r->d[4], r->d[5], r->d[6]};
    for (int i = 2; i >= 0; i--) {
        c[i] = {uint16_t((start >> (4 * (2 - i))) & 15), 0, 1, 0, 1, 0, list};
        list = tokenise(ptr<uint8_t>(strings[i]), list);
        if (!list)
            return syntax();
    }
    uint16_t *to = ptr<uint16_t>(buffer);
    uint16_t n = lines - 1;
    do {
        for (int i = 0; i < 3; i++)
            c[i].step();
        *to++ = uint16_t(c[0].value << 8 | c[1].value << 4 | c[2].value);
    } while (n-- != 0);
    d.RainBow = 1;
    return 0;
}

// TRDo: Rainbow n,base (d2),y (d3),height (d4); EntNul keeps one
uint32_t show(AllRegs *r)
{
    Rainbow *rb = rainbow(r->d[1]);
    if (!rb || !rb->RnLong)
        return 1;
    uint8_t act = action(rb);
    if (r->d[2] != EntNul) {
        rb->RnX = r->d[2];
        act |= 2;
    }
    if (r->d[3] != EntNul) {
        rb->RnY = r->d[3];
        act |= 4;
    }
    if (r->d[4] != EntNul) {
        rb->RnI = r->d[4];
        act |= 1;
    }
    action(rb) = act;
    wdata().Actualise |= BitEcrans;
    return 0;
}

// TRVar: a0 = the colour of line d2 of rainbow d1
uint32_t line_address(AllRegs *r)
{
    Rainbow *rb = rainbow(r->d[1]);
    if (!rb || !rb->RnBuf || int32_t(r->d[2]) < 0)
        return 1;
    uint16_t offset = uint16_t(r->d[2]) << 1;
    if (offset >= rb->RnLong)
        return 1;
    r->a[0] = rb->RnBuf + int16_t(offset);
    return 0;
}

} // namespace

extern "C" {
void w_ec_rain_set(AllRegs *r) { r->d[0] = set_rainbow(r); }
void w_ec_rain_do(AllRegs *r) { r->d[0] = show(r); }
void w_ec_rain_var(AllRegs *r) { r->d[0] = line_address(r); }
void w_ec_rain_del(AllRegs *r) { r->d[0] = delete_rainbow(r->d[1]); }

// TRHide: d1 < 0 hides the rainbows, else shows them again
void w_ec_rain_hide(AllRegs *r)
{
    WData &d = wdata();
    if (int16_t(r->d[1]) < 0) {
        d.OldRain = d.RainBow;
        d.RainBow = 0;
    } else {
        d.RainBow = d.OldRain;
    }
}
}
