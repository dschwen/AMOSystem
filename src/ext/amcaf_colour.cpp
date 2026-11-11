// AMCAF: colour values (R050-R05b): the notation of Red Val and its kin, the colour words, the mixing of
// colours and the nearest pen of the palette.

#include "ext/amcaf.h"
#include "lib/banks.h"

using namespace amos;

namespace {

bool notation4() { return peek_w(amcaf::state_at(amcaf::Notation)) == 4; }

// the components of a colour as the library takes them apart: the blue and green nibbles, the red as what
// is left over above them (a word; 16-bit arithmetic follows)
struct Parts {
    uint16_t r, g, b;
};
Parts parts(uint32_t colour)
{
    uint16_t c = uint16_t(colour);
    return {uint16_t(c >> 8), uint16_t((c >> 4) & 15), uint16_t(c & 15)};
}

uint32_t join(uint16_t r, uint16_t g, uint16_t b) { return uint16_t(uint16_t(b) | uint16_t(g << 4) | uint16_t(r << 8)); }

// R053: the pen of the range whose colour is nearest to the colour word (an exact one at once)
int32_t best_pen_in(int32_t colour, int32_t lo, int32_t hi)
{
    static const int8_t cost[16] = {0, 1, 3, 5, 8, 12, 16, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    if (hi < 0 || uint16_t(hi) > 63 || lo < 0 || uint16_t(lo) > 63 || hi - lo < 0)
        fon_call();
    const Screen *s = ptr<Screen>(idata().ScOnAd);
    int32_t best = 0;
    int16_t closest = 1000;
    uint16_t c = uint16_t(colour);
    uint16_t pen = uint16_t(lo);
    for (int32_t n = hi - lo; n >= 0; n--, pen++) {
        uint16_t word = s->EcPal[pen & 31];
        if (pen > 31)
            word = uint16_t((word & 0x0EEE) >> 1);
        if (c == word)
            return pen;
        int16_t dr = int16_t(((c & 0x0F00) >> 8) - (word & 0x0F00) / 256);
        int16_t dg = int16_t(((c & 0x00F0) >> 4) - ((word & 0x00F0) >> 4));
        int16_t db = int16_t((c & 15) - (word & 15));
        int16_t d = int16_t(cost[(dr < 0 ? -dr : dr) & 15] + cost[(dg < 0 ? -dg : dg) & 15] + cost[(db < 0 ? -db : db) & 15]);
        if (closest >= d) {
            closest = d;
            best = pen;
        }
    }
    return best;
}

// R0a2's measure (R4546): 0 for the same colour, else the sum of the costs of the differences of the three
// nibbles
int16_t colour_distance(uint16_t candidate, uint16_t reference)
{
    static const int8_t cost[16] = {0, 1, 3, 5, 8, 12, 16, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    if (reference == candidate)
        return 0;
    auto diff = [](int a, int b) { int d = a - b; return d < 0 ? -d : d; };
    int dr = diff((reference & 0x0F00) >> 8, (candidate & 0x0F00) >> 8);
    int dg = diff((reference & 0xF0) >> 4, (candidate & 0xF0) >> 4);
    int db = diff(reference & 15, candidate & 15);
    return int16_t(cost[dr & 15] + cost[dg & 15] + cost[db & 15]);
}

} // namespace

extern "C" {

// =HAM COLOUR(code,colour): the colour with one of its nibbles replaced as a HAM code says: 0-15 the
// pen's colour (read from the screen at +62, as the library does), 16-31 the blue, 32-47 the red, 48-63 the
// green
int32_t amcaf_ham_colour(int32_t code, int32_t colour)
{
    uint32_t d3 = uint32_t(colour);
    uint16_t d0 = uint16_t(code);
    if (d0 <= 15) {
        uint32_t screen = idata().ScOnAd;
        if (!screen)
            error(47);
        uint32_t at = screen + 62 + 2u * d0;
        return int32_t((d3 & 0xFFFF0000u) | peek_w(decode_context_cursor(at)));
    }
    if (d0 <= 31)
        return int32_t((d3 & 0xFFFFFF00u) | uint8_t((d3 & 0xF0) | uint8_t(d0 - 16)));
    if (d0 <= 47)
        return int32_t((d3 & 0xFFFF0000u) | uint16_t((d3 & 0xFF) | uint16_t((d0 - 32) << 8)));
    return int32_t((d3 & 0xFFFFFF00u) | uint8_t((d3 & 15) | uint8_t(uint8_t(d0 - 48) << 4)));
}

// =HAM BEST(from,to): the code, of the 64, that brings the colour from nearest to the colour to (the same
// colours: its blue nibble as a blue code); the pens' colours are those of the screen's palette
int32_t amcaf_ham_best(int32_t from, int32_t to)
{
    uint32_t d6 = uint32_t(from);
    uint16_t reference = uint16_t(d6), target = uint16_t(to);
    if (reference == target)
        return int32_t((d6 & 0xFFFF0000u) | uint16_t((d6 & 15) + 16));
    const Screen *s = ptr<Screen>(idata().ScOnAd);
    int16_t best = 1000;
    uint16_t result = 0;
    for (int pen = 15; pen >= 0; pen--) {
        int16_t d = colour_distance(s->EcPal[pen], reference);
        if (d == 0)
            return pen;
        if (best >= d) {
            best = d;
            result = uint16_t(pen);
        }
    }
    struct Try {
        uint16_t keep, mask, shift, offset;
    };
    // the red, the green and the blue nibble of the reference put in the target in turn
    static const Try tries[3] = {{0x00FF, 0x0F00, 8, 32}, {0x0F0F, 0x00F0, 4, 48}, {0x0FF0, 0x000F, 0, 16}};
    for (const Try &t : tries) {
        uint16_t nibble = reference & t.mask;
        uint16_t trial = uint16_t((target & t.keep) | nibble);
        nibble >>= t.shift;
        int16_t d = colour_distance(trial, reference);
        if (d == 0)
            return nibble + t.offset;
        if (best >= d) {
            best = d;
            result = uint16_t(nibble + t.offset);
        }
    }
    return result;
}

// AMCAF AGA NOTATION ON / OFF: the values of Red Val... in 4 bits (on) or 8 (off)
void amcaf_aga_notation_on() { poke_w(amcaf::state_at(amcaf::Notation), 4); }
void amcaf_aga_notation_off() { poke_w(amcaf::state_at(amcaf::Notation), 8); }

// =RED VAL(colour), =GREEN VAL, =BLUE VAL
int32_t amcaf_red_val(int32_t colour)
{
    return notation4() ? int32_t(uint32_t(colour) << 8 >> 16) : int32_t(uint32_t(colour) >> 16);
}
int32_t amcaf_green_val(int32_t colour) { return notation4() ? (uint32_t(colour) >> 4) & 15 : (uint32_t(colour) >> 8) & 255; }
int32_t amcaf_blue_val(int32_t colour) { return notation4() ? colour & 15 : colour & 255; }

// =RRGGBB TO RGB(c): the 24-bit colour in 12 bits (the high nibbles kept)
int32_t amcaf_rrggbb_to_rgb(int32_t c)
{
    uint32_t v = uint32_t(c);
    return int32_t(((v >> 4) & 15) | ((v >> 8) & 0xF0) | ((v >> 12) & 0xF00));
}

// =RGB TO RRGGBB(c): the 12-bit colour in 24 bits (each nibble to the high nibble of a byte)
int32_t amcaf_rgb_to_rrggbb(int32_t c)
{
    uint32_t d0 = uint32_t(c) & 0xFFF;
    uint32_t d1 = (d0 & 0xF00) << 12 | (d0 & 0xF0) << 8 | (d0 & 15) << 4;
    return int32_t(d1);
}

// =GLUE COLOUR(r,g,b): the nibbles of the three in a colour word (the upper word of b stays, as in the
// library)
int32_t amcaf_glue_colour(int32_t r, int32_t g, int32_t b)
{
    uint32_t d3 = (uint32_t(b) & 0xFFFF0000u) | (uint32_t(b) & 15);
    d3 |= uint8_t((g & 15) << 4);
    d3 |= uint16_t((r & 15) << 8);
    return int32_t(d3);
}

// =MIX COLOUR(a,b): the mean of two colours. (The sums are of the low bytes of the red parts, then
// halved as words)
int32_t amcaf_mix_colour_2(int32_t a, int32_t b)
{
    Parts x = parts(uint32_t(b)), y = parts(uint32_t(a));
    auto add = [](uint16_t w, uint16_t v) { return uint16_t((w & 0xFF00) | ((w + v) & 0xFF)); };
    uint16_t r = uint16_t(add(y.r, x.r) >> 1), g = uint16_t(add(y.g, x.g) >> 1), bl = uint16_t(add(y.b, x.b) >> 1);
    return int32_t(join(r, g, bl));
}

// =MIX COLOUR(c,amount,lo TO hi): c brightened by the nibbles of amount up to hi, or darkened by those of
// the negative amount down to lo
int32_t amcaf_mix_colour_4(int32_t c, int32_t amount, int32_t lo, int32_t hi)
{
    Parts base = parts(uint32_t(c));
    uint16_t r = base.r, g = base.g, b = base.b;
    if (amount < 0) {
        Parts d = parts(uint32_t(-amount));
        Parts floor = parts(uint32_t(lo));
        r = uint16_t(r - d.r);
        g = uint16_t(g - d.g);
        b = uint16_t(b - d.b);
        if (int16_t(r) < int16_t(floor.r))
            r = floor.r;
        if (int16_t(g) < int16_t(floor.g))
            g = floor.g;
        if (int16_t(b) < int16_t(floor.b))
            b = floor.b;
    } else {
        Parts d = parts(uint32_t(amount));
        Parts ceiling = parts(uint32_t(hi));
        r = uint16_t(r + d.r);
        g = uint16_t(g + d.g);
        b = uint16_t(b + d.b);
        if (int16_t(r) > int16_t(ceiling.r))
            r = ceiling.r;
        if (int16_t(g) > int16_t(ceiling.g))
            g = ceiling.g;
        if (int16_t(b) > int16_t(ceiling.b))
            b = ceiling.b;
    }
    return int32_t(join(r, g, b));
}

// =BEST PEN(colour): the pen of the screen nearest to the colour; =BEST PEN(colour,lo TO hi)
int32_t amcaf_best_pen_1(int32_t colour)
{
    const Screen *s = ptr<Screen>(idata().ScOnAd);
    int32_t hi = int32_t(uint16_t(1u << (s->EcNPlan & 15))) - 1;
    return best_pen_in(colour, 0, hi);
}
int32_t amcaf_best_pen_3(int32_t colour, int32_t lo, int32_t hi) { return best_pen_in(colour, lo, hi); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_colour.inc"
#endif
