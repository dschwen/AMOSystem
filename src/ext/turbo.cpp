// The Turbo Plus extension (AMOSPro_TURBO_Plus.Lib V2.15; default slot 12) in C++:
// docs/THIRD-PARTY-EXTENSIONS.md. The tables are written by tools/ext_gen.py
// (data/extensions/turbo.json says which commands are implemented). The routines are named by
// the library's own numbers (R031...) where a comment says what they were.

#include "core/guest_scratch.h"
#include "ext/third_party.h"
#include "hal/system.h"
#include "lib/strings.h"

#include <graphics/rastport.h>

using namespace amos;
using ext3::Id;

namespace {

// The extension's data (ExtAdr): the library's own 1752 bytes
constexpr uint32_t StateSize = 1752;
enum : uint32_t {
    SceneIconBank = 952,        // word: the icon bank Scene Icon uses (2)
    MaskPalette = 920,          // word, -1
    MaskColours = 936,          // word, -1
    Unknown956 = 956,           // long, -1
    PlaneMask = 1084,           // word: Set Planes (-1: all)
};

uint8_t *state() { static uint8_t *zone = guest_object<uint8_t>(StateSize); return zone; }
uint8_t *state_at(uint32_t at) { return state() + at; }

Screen *screen() { return ptr<Screen>(idata().ScOnAd); }

// The byte of a plane's row that holds pixel x of line y: the line's start (a word product), then
// the byte of x added to its low word only, as the library does (the high word stays)
struct Place {
    uint32_t offset;
    uint8_t mask;
};

// R031's and R033's tests of a pixel in the screen: false when it is not in it
bool place(const Screen *s, int32_t x, int32_t y, Place &p)
{
    if (y < 0 || int16_t(y) >= int16_t(uint16_t(s->EcTy)))
        return false;
    if (x < 0)
        return false;
    uint16_t row = s->EcTLigne;
    if (int16_t(x) >= int16_t(row << 3))
        return false;
    uint32_t product = uint32_t(uint16_t(y)) * row;
    uint16_t low = uint16_t(uint16_t(product) + uint16_t(int16_t(x) >> 3));
    p.offset = (product & 0xFFFF0000u) | low;
    p.mask = uint8_t(0x80 >> (x & 7));
    return true;
}

// R032: the colour's bits into the planes of the screen being drawn in (EcCurrent)
void plot_planes(const Screen *s, const Place &p, uint32_t colour)
{
    uint32_t planes = s->EcNPlan;
    for (uint32_t i = 0; i < planes && i < 6; i++) {
        ContextCursor at = decode_context_cursor(s->EcCurrent[i] + p.offset);
        uint8_t v = host::detail::read_cursor_byte(at);
        v = colour >> i & 1 ? uint8_t(v | p.mask) : uint8_t(v & ~p.mask);
        host::detail::write_cursor_byte(at, v);
    }
}

// An EcCall (the Autoback steps around a drawing)
void autoback(int function)
{
    Regs r = {};
    ec_call(function, r);
}

} // namespace

extern "C" {

void turbo_default();

// =L SWAP(n): the two halves of the long exchanged
int32_t turbo_l_swap(int32_t n) { return int32_t(uint32_t(n) << 16 | uint32_t(n) >> 16); }

// F PLOT x,y,colour (R031): a pixel, with the low bits of the colour in the planes; out of the
// screen nothing happens. With Autoback it is drawn in the other buffer too, as AMOS's own drawing is
void turbo_f_plot(int32_t x, int32_t y, int32_t colour)
{
    Screen *s = screen();
    if (colour < 0)
        fon_call();
    Place p;
    if (!place(s, x, y, p))
        return;
    if (!s->EcAuto) {
        plot_planes(s, p, uint32_t(colour));
        return;
    }
    autoback(ec::AutoBack1);
    if (s->EcFlags & (1 << 13)) {
        plot_planes(s, p, uint32_t(colour));
        autoback(ec::AutoBack2);
        plot_planes(s, p, uint32_t(colour));
        autoback(ec::AutoBack3);
    } else {
        plot_planes(s, p, uint32_t(colour));
        autoback(ec::AutoBack4);
    }
}

// =F POINT(x,y) (R033): the colour of a pixel of the logic screen, -1 out of it
int32_t turbo_f_point(int32_t x, int32_t y)
{
    const Screen *s = screen();
    Place p;
    if (!place(s, x, y, p))
        return -1;
    int32_t colour = 0;
    uint32_t planes = s->EcNPlan;
    for (uint32_t i = 0; i < planes && i < 6; i++)
        if (host::detail::read_cursor_byte(decode_context_cursor(s->EcLogic[i] + p.offset)) & p.mask)
            colour |= 1 << i;
    return colour;
}

// SET PLANES mask (R04c): the planes drawing reaches (the RastPort's mask), kept for Scene and Blit
void turbo_set_planes(int32_t mask)
{
    uint8_t m = uint8_t(mask);
    ptr<struct RastPort>(wdata().RastPort)->Mask = m;
    poke_w(state_at(PlaneMask), m);
}

// AMOS PRI priority (R07d): the task's priority (-128 to 20); another is ignored. There is no task to
// give it to on another host
void turbo_amos_pri(int32_t priority)
{
    if (priority < -128 || priority > 20)
        return;
    hal::set_task_priority(priority);
}

// =WORD HUNT(start To end,action,value1 To value2) (R09f): the address of the first word in the
// range (both addresses made even, the lower first; the word at the end not read, but one word at
// least) that is value1 (action 0), inside value1 to value2 (action over 0), outside (under 0);
// signed words. 0 if there is none
int32_t turbo_word_hunt(int32_t start, int32_t end, int32_t action, int32_t value1, int32_t value2)
{
    uint32_t from = uint32_t(start) & ~1u, to = uint32_t(end) & ~1u;
    if (int32_t(to) <= int32_t(from)) {
        uint32_t t = from;
        from = to;
        to = t;
    }
    int32_t left = int32_t(to - from);
    int16_t low = int16_t(value1), high = int16_t(value2);
    ContextCursor at = decode_context_cursor(from);
    for (;;) {
        int16_t word = int16_t(peek_w(at));
        at += 2;
        bool found = action == 0 ? word == low
                   : action < 0 ? word < low || word > high
                                : word >= low && word <= high;
        if (found)
            return int32_t(encode_context_cursor(at) - 2);
        left -= 2;
        if (left <= 0)
            return 0;
    }
}

// The cold start (routine 0): the library's data, and the routines AMOS calls at each Default and at the end
uint32_t turbo_cold(RegFrame &r)
{
    uint32_t result = ext3::cold_start(r, Id::turbo, 0x0200);
    if (int32_t(result) >= 0) {
        idata().ExtAdr[result * 4] = address(state());
        idata().ExtAdr[result * 4 + 1] = address(AMOS_DATA(ext_turbo_default_entry));
        idata().ExtAdr[result * 4 + 2] = address(AMOS_DATA(ext_turbo_end_entry));
        turbo_default();
    }
    return result;
}

// R12e from $5614: the extension back to its start (at each Default, and at the end). What is
// here is what the commands implemented keep
void turbo_default()
{
    poke_w(state_at(SceneIconBank), 2);
    poke_w(state_at(PlaneMask), 0xFFFF);
    poke_l(state_at(Unknown956), 0xFFFFFFFFu);
    poke_w(state_at(MaskPalette), 0xFFFF);
    poke_w(state_at(MaskColours), 0xFFFF);
}

void turbo_end() { turbo_default(); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_turbo.inc"
#endif
