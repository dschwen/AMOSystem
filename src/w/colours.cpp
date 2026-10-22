// Colours of AMOS.library's screen table (EcCall): Colour, Palette,
// Colour Back, and the setting up of Flash, Shift and Fade, whose tables
// the VBL interrupt (+W.s FlInt, Shifter, FadeI) works through.
//
// A colour is set in the screen's palette (EcPal) and in each copper list
// showing the screen: T_CopMark holds, per screen, the addresses of the
// colour moves, zero-terminated, for colours 0-15 and at +64 for 16-31.

#include "w/w.h"

using namespace amos;
using namespace amos::w;

namespace {

constexpr uint16_t PalMax = 16;

// Keeps the compiler from moving memory accesses across it: the VBL
// interrupt reads the tables once the flag that stops it is cleared
inline void barrier() { asm volatile("" ::: "memory"); }

uint8_t *copper_marks(const Screen *s) { return wdata().CopMark + (s->EcNumber << 7); }

// Colour n (offset n*4 in a list) in the copper lists of a screen
void poke_copper(uint8_t *marks, uint16_t offset, uint16_t colour)
{
    const uint8_t *list = offset >= PalMax * 4 ? marks + 64 : marks;
    for (; peek_l(list); list += 4)
        poke_w(ptr<uint8_t>(peek_l(list)) + 2 + offset, colour);
}

// EcSCol: colour d1 (0-31) to d2
void set_colour(AllRegs *r)
{
    Screen *s = current_screen();
    uint16_t n = r->d[1] & 31;
    uint16_t colour = r->d[2] & 0xFFF;
    s->EcPal[n] = colour;
    poke_copper(copper_marks(s), n << 2, colour);
    r->d[0] = 0;
}

// EcGCol: colour d1 in d1
void get_colour(AllRegs *r)
{
    r->d[1] = current_screen()->EcPal[r->d[1] & 31];
    r->d[0] = 0;
}

// EcSPal: the 32 colours at a1, negative ones unchanged
void set_palette(AllRegs *r)
{
    Screen *s = current_screen();
    uint8_t *marks = copper_marks(s);
    const uint8_t *palette = ptr<uint8_t>(r->a[1]);
    for (uint16_t n = 0; n < 32; n++, palette += 2) {
        int16_t colour = peek_w(palette);
        if (colour < 0)
            continue;
        colour &= 0xFFF;
        s->EcPal[n] = colour;
        poke_copper(marks, n << 2, colour);
    }
    r->d[0] = 0;
}

// EcSColB: the colour behind the screens
void set_colour_back(AllRegs *r)
{
    wdata().EcFond = r->d[1] & 0xFFF;
    r->d[0] = 0;
}

// ---- Flash -----------------------------------------------------------------------

// T_NbFlash: the number of colours flashing (high byte) and a counter that
// stops the interrupt while not zero (low byte)
constexpr uint32_t FlMax = 16, FlashSize = 2 + 2 + 4 + 2 + 16 * 4 + 2;

volatile uint8_t &flash_count() { return reinterpret_cast<volatile uint8_t *>(&wdata().NbFlash)[0]; }
volatile uint8_t &flash_stop() { return reinterpret_cast<volatile uint8_t *>(&wdata().NbFlash)[1]; }

void flash_hold()
{
    flash_stop() = flash_stop() + 1;
    barrier();
}

void flash_release()
{
    barrier();
    flash_stop() = flash_stop() - 1;
}

// FlCalc: count the colours flashing. An entry: the colour number * 2 + 2
// (0: free), the counter, the screen, the position in the list, then
// pairs of speed and colour.
void flash_count_entries()
{
    flash_hold();
    uint8_t n = 0;
    const uint8_t *entry = wdata().TFlash;
    for (uint32_t i = 0; i < FlMax; i++, entry += FlashSize)
        if (peek_w(entry))
            n++;
    flash_count() = n;
    flash_release();
}

// FlStop: stop the flashes of the current screen. d0: the screen.
void flash_off(AllRegs *r)
{
    uint32_t screen = wdata().EcCourant;
    flash_hold();
    uint8_t *entry = wdata().TFlash;
    for (uint32_t i = 0; i < FlMax; i++, entry += FlashSize)
        if (peek_w(entry) && peek_l(entry + 4) == screen)
            poke_w(entry, 0);
    flash_count_entries();
    flash_release();
    r->d[0] = screen;
}

// MiniGet: the next character but spaces, in capitals. 0 at the end.
struct Reader {
    const uint8_t *p;
    uint8_t c;

    // 1 a digit, -1 anything else, 0 the end
    int get()
    {
        do
            c = *p++;
        while (c == ' ');
        if (!c)
            return 0;
        if (int8_t(c) >= '0' && c <= '9')
            return 1;
        if (c >= 'a')
            c -= 32;
        return -1;
    }

    // Gethexa: a hexadecimal digit, or -1
    int hex()
    {
        if (!get())
            return -1;
        uint8_t v = c - '0';
        if (v > 9)
            v -= 7;
        return v > 15 ? -1 : v;
    }

    // dechexa: a decimal number, maybe negative; false if none
    bool decimal(uint16_t *n)
    {
        uint32_t v = 0;
        bool negative = false;
        int kind = get();
        if (kind <= 0) {
            if (!kind || c != '-')
                return false;
            negative = true;
            kind = get();
        }
        while (kind > 0) {
            v = uint32_t(uint16_t(v)) * 10;
            v = (v & 0xFFFF0000) | uint16_t(v + ((c - 48) & 0xFF));
            kind = get();
        }
        *n = negative ? uint16_t(-uint16_t(v)) : uint16_t(v);
        return true;
    }
};

// FlStart: colour d1 flashes as a1 says: "(rgb,speed)(rgb,speed)...", up
// to 16; an empty string stops it. Errors 7 (too many) and 8 (syntax).
void flash(AllRegs *r)
{
    if (flash_count() >= FlMax) {
        r->d[0] = 7;
        return;
    }
    flash_hold();
    uint16_t number = uint16_t(r->d[1] + 1) << 1;
    uint32_t screen = wdata().EcCourant;
    uint8_t *entry = wdata().TFlash;
    uint32_t i = 0;
    for (; i < FlMax; i++, entry += FlashSize)
        if (!peek_w(entry) || (peek_w(entry) == number && peek_l(entry + 4) == screen))
            break;
    uint32_t error = 0;
    auto syntax = [&] {
        poke_w(entry, 0);
        error = 8;
    };
    if (i == FlMax) {
        error = 8;
    } else {
        for (uint32_t b = 0; b < FlashSize; b++)
            entry[b] = 0;
        Reader text = {ptr<uint8_t>(r->a[1]), 0};
        if (*text.p) {
            poke_w(entry, number);
            poke_w(entry + 2, 1);
            poke_l(entry + 4, screen);
            poke_w(entry + 8, 0);
            uint8_t *step = entry + 10;
            int colours = -1;
            for (;;) {
                uint8_t c = *text.p++;
                if (c != '(') {
                    if (c)
                        syntax();
                    break;
                }
                if (++colours >= 16) {
                    syntax();
                    break;
                }
                int red = text.hex(), green = red < 0 ? -1 : text.hex(), blue = green < 0 ? -1 : text.hex();
                if (blue < 0) {
                    syntax();
                    break;
                }
                poke_w(step + 2, uint16_t(red << 8 | green << 4 | blue));
                uint16_t speed;
                if (*text.p++ != ',' || !text.decimal(&speed) || !speed) {
                    syntax();
                    break;
                }
                poke_w(step, speed);
                step += 4;
                if (text.c != ')') {
                    syntax();
                    break;
                }
            }
        }
    }
    flash_count_entries();
    flash_release();
    r->d[0] = error;
}

// ---- Shift -----------------------------------------------------------------------

// The shift (T_TShift): counter (0: off), speed, screen, first and last
// colour * 2, direction, rotation
void shift(AllRegs *r)
{
    uint8_t *t = wdata().TShift;
    poke_w(t, 0);
    barrier();
    poke_w(t + 2, r->d[2]);
    poke_l(t + 4, wdata().EcCourant);
    uint16_t first = r->d[3] & 31, last = r->d[4] & 31;
    if (last <= first) {
        r->d[0] = 9;
        return;
    }
    poke_w(t + 8, first << 1);
    poke_w(t + 10, last << 1);
    t[12] = r->d[5];
    t[13] = r->d[6];
    barrier();
    poke_w(t, 1);
    r->d[0] = 0;
}

// ShStop: stop the shift if it is the current screen's
void shift_off(AllRegs *r)
{
    uint8_t *t = wdata().TShift;
    if (peek_w(t) && peek_l(t + 4) == wdata().EcCourant)
        poke_w(t, 0);
    r->d[0] = 0;
}

// ---- Fade ------------------------------------------------------------------------

// FadeTOn: fade the current screen to the palette at a1 (negative: keep
// the colour), a step every d1 VBLs. T_FadeCol gets an entry per colour
// that changes: its offset, then current and target of red, green, blue.
void fade(AllRegs *r)
{
    WData &d = wdata();
    Screen *s = current_screen();
    d.FadeFlag = 0;
    barrier();
    d.FadeCpt = 1;
    d.FadeVit = r->d[1];
    d.FadeCop = address(copper_marks(s));
    d.FadePal = address(s->EcPal);
    const uint8_t *palette = ptr<uint8_t>(r->a[1]);
    uint8_t *entry = d.FadeCol;
    uint16_t count = 0;
    for (uint16_t n = 0; n < 32; n++, palette += 2) {
        uint16_t target = peek_w(palette);
        if (int16_t(target) < 0)
            continue;
        uint16_t now = s->EcPal[n];
        bool changes = false;
        poke_w(entry, n << 1);
        for (int shift = 8, i = 0; shift >= 0; shift -= 4, i++) {
            uint8_t from = now >> shift & 15, to = target >> shift & 15;
            entry[2 + 2 * i] = from;
            entry[3 + 2 * i] = to;
            if (from != to)
                changes = true;
        }
        if (changes) {
            count++;
            entry += 8;
        }
    }
    d.FadeNb = count - 1;
    barrier();
    d.FadeFlag = count;
    r->d[0] = 0;
}

void fade_off(AllRegs *r)
{
    wdata().FadeFlag = 0;
    r->d[0] = 0;
}

// FaStop: stop the fade if it is the current screen's
void fade_stop(AllRegs *)
{
    if (wdata().FadePal == address(current_screen()->EcPal))
        wdata().FadeFlag = 0;
}

} // namespace

extern "C" {
void w_ec_set_colour(AllRegs *r) { set_colour(r); }
void w_ec_get_colour(AllRegs *r) { get_colour(r); }
void w_ec_set_palette(AllRegs *r) { set_palette(r); }
void w_ec_colour_back(AllRegs *r) { set_colour_back(r); }
void w_ec_flash_off(AllRegs *r) { flash_off(r); }
void w_ec_flash(AllRegs *r) { flash(r); }
void w_ec_shift(AllRegs *r) { shift(r); }
void w_ec_shift_off(AllRegs *r) { shift_off(r); }
void w_ec_fade(AllRegs *r) { fade(r); }
void w_ec_fade_off(AllRegs *r) { fade_off(r); }
void w_ec_fade_stop(AllRegs *r) { fade_stop(r); }
}
