// The line editor of AMOSPro.Lib (LEd_* in Dialog.s): one line of text
// edited in the current window, for Input, the edit zones of the Interface
// and the editor.

#include "lib/dialog.h"

using namespace amos;

namespace {

// LEd_Flags: the options are its high byte, the state its low byte
uint8_t &options(LineEdit *e) { return reinterpret_cast<uint8_t *>(&e->LEd_Flags)[0]; }
uint8_t &state(LineEdit *e) { return reinterpret_cast<uint8_t *>(&e->LEd_Flags)[1]; }

enum : uint8_t {
    StateEraseRight = 1 << 0,   // a character to erase after the line
    StateClear = 1 << 1,        // clear the whole line first
    StateCursor = 1 << 2,       // the cursor is shown
};

constexpr uint8_t ScanLeft = 0x4F, ScanRight = 0x4E, ScanBackSpace = 0x41, ScanDelete = 0x46;
constexpr uint32_t BitControl = 8;

uint8_t *buffer(LineEdit *e) { return ptr<uint8_t>(e->LEd_Buffer); }

void print(const char *s)
{
    Regs r = {};
    r.a[1] = address(s);
    wi_call(wi::Print, r);
}

void chr_out(uint8_t c)
{
    Regs r = {};
    r.d[1] = c;
    wi_call(wi::ChrOut, r);
}

void locate(uint32_t x, uint32_t y)
{
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    wi_call(wi::Locate, r);
}

// LEd_FauCu: the fake cursor, a space drawn in complement mode
const char fake_cursor[] = "\x1bI1\x1bW2 \x1bW0\x1bI0\x1d";

// LEd_Lettre: a letter or a digit
bool is_letter(uint8_t c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c >= 128;
}

} // namespace

// LEd_CuStoppe / LEd_CuMarche: hide / show the cursor (the real one, or
// the fake one)
void amos::led_cursor_off(LineEdit *e)
{
    if (state(e) & StateCursor) {
        state(e) &= ~StateCursor;
        print(options(e) & LEdCursor ? "\x1b" "C0" : fake_cursor);
    }
}

void amos::led_cursor_on(LineEdit *e)
{
    if (!(state(e) & StateCursor)) {
        state(e) |= StateCursor;
        print(options(e) & LEdCursor ? "\x1b" "C1" : fake_cursor);
    }
}

// LEd_Print: show the line, scrolled to keep the cursor in sight
void amos::led_print(LineEdit *e)
{
    led_cursor_off(e);
    locate(e->LEd_X, e->LEd_Y);
    if (state(e) & StateClear) {
        state(e) &= ~StateClear;
        uint16_t n = e->LEd_Large;
        do
            chr_out(' ');
        while (--n);
        locate(e->LEd_X, e->LEd_Y);
    }
    uint16_t cur = e->LEd_Cur;
    if (cur < e->LEd_Start)
        e->LEd_Start = cur;
    if (static_cast<int16_t>(cur) > static_cast<int16_t>(e->LEd_Start + e->LEd_Large)) {
        e->LEd_Start = static_cast<uint16_t>(cur - e->LEd_Large);
        state(e) |= StateEraseRight;
    }
    int16_t n = static_cast<int16_t>(e->LEd_Long - e->LEd_Start);
    if (n > static_cast<int16_t>(e->LEd_Large)) {
        state(e) &= ~StateEraseRight;
        n = static_cast<int16_t>(e->LEd_Large + 1);
    }
    Regs r = {};
    r.a[1] = address(buffer(e) + e->LEd_Start);
    r.d[1] = static_cast<uint16_t>(n);
    wi_call(wi::Print2, r);
    if (state(e) & StateEraseRight) {
        state(e) &= ~StateEraseRight;
        chr_out(' ');
    }
    locate(static_cast<uint16_t>(e->LEd_X + e->LEd_Cur - e->LEd_Start), e->LEd_Y);
    led_cursor_on(e);
}

// LEd_New: replace the line with text (length characters, -1: up to the
// zero) and show it, cursor at the end
void amos::led_new(LineEdit *e, const uint8_t *text, uint16_t length)
{
    uint8_t *to = buffer(e);
    uint16_t room = e->LEd_Max;
    if (length) {
        for (;;) {
            uint8_t c = *text++;
            *to++ = c;
            if (!c) {
                to--;
                break;
            }
            if (!--length || !--room)
                break;
        }
    }
    *to = 0;
    e->LEd_Long = e->LEd_Cur = static_cast<uint16_t>(to - buffer(e));
    e->LEd_Start = 0;
    state(e) |= StateClear;
    led_print(e);
    led_cursor_off(e);
}

// LEd_Init: start editing at the cursor of the current window. text is
// shown first; store holds the line (max characters), width is the width
// on screen (negative: up to the edge of the window). False if there is
// no room.
bool amos::led_init(LineEdit *e, const uint8_t *text, uint8_t *store, uint8_t flags, uint16_t max, int16_t width)
{
    e->LEd_Buffer = address(store);
    options(e) = flags;
    e->LEd_Max = max;
    e->LEd_Cur = 0;
    e->LEd_Start = 0;
    for (be_u32 &m : e->LEd_Mask)
        m = ~0u;
    Regs r = {};
    wi_call(wi::XYCuWi, r);
    e->LEd_X = static_cast<uint16_t>(r.d[1]);
    e->LEd_Y = static_cast<uint16_t>(r.d[2]);
    e->LEd_Large = static_cast<uint16_t>(width);
    if (width < 0) {
        Regs s = {};
        wi_call(wi::SXSYCuWi, s);
        int16_t w = static_cast<int16_t>(s.d[1] - e->LEd_X - 1);
        if (w < 0)
            return false;
        // One less with Scroll On
        if (s.d[0] & 1 << 8 && --w < 0)
            return false;
        e->LEd_Large = static_cast<uint16_t>(w);
    }
    state(e) &= ~StateCursor;
    print("\x1b" "C0");
    e->LEd_Screen = ptr<Screen>(wdata().EcCourant)->EcNumber;
    led_new(e, text, 0xFFFF);
    return true;
}

// LEd_Loop: edit until something to return. Returns the code: 1 Return,
// 2 the mouse outside the line (LEdMouse), -1 Control-C (LEdTests), 0 a
// key not used (LEdKeys) or nothing (LEdOnce); *key gets the key.
int16_t amos::led_loop(LineEdit *e, uint32_t *key)
{
    // The line always ends with a zero when it comes back
    auto done = [e, key](int16_t code, uint32_t k) {
        *key = k;
        buffer(e)[e->LEd_Long] = 0;
        return code;
    };
    led_cursor_on(e);
    WData &w = wdata();
    for (;;) {
        if (options(e) & LEdMulti)
            dia_wait_mul();
        if (options(e) & LEdTests) {
            Regs r = {};
            lib_call(L_Test_PaSaut, r);
            uint16_t a = w.Actualise & idata().ActuMask;
            if (a & 1 << BitControl) {
                w.Actualise = static_cast<uint16_t>(a & ~(1 << BitControl));
                return done(-1, 0);
            }
        }
        // The mouse: moves the cursor in the line, or returns
        if (mouse_key() & 3) {
            bool out = true;
            if (options(e) & LEdMouCur) {
                // (Over no screen the assembler compared a leftover d0)
                Regs r = {};
                sy_call(sy::MouScrFront, r);
                if (static_cast<int32_t>(r.d[1]) >= 0 && static_cast<uint16_t>(r.d[0]) == e->LEd_Screen) {
                    sy_call(sy::XyWin, r);
                    uint16_t x = static_cast<uint16_t>(r.d[1]);
                    if (static_cast<int32_t>(r.d[0]) >= 0 && static_cast<uint16_t>(r.d[2]) == e->LEd_Y
                        && x >= e->LEd_X) {
                        x = static_cast<uint16_t>(x - e->LEd_X + e->LEd_Start);
                        if (x <= e->LEd_Long) {
                            out = false;
                            if (x != e->LEd_Cur) {
                                e->LEd_Cur = x;
                                led_print(e);
                                if (options(e) & LEdTests)
                                    dia_patch();
                                continue;
                            }
                        }
                    }
                }
            }
            if (out && options(e) & LEdMouse)
                return done(2, 0);
        }
        // A key
        uint8_t *b = buffer(e);
        Regs r = {};
        sy_call(sy::Inkey, r);
        uint32_t k = r.d[1];
        if (!k) {
            if (options(e) & LEdOnce)
                return done(0, 0);
            continue;
        }
        uint16_t high = static_cast<uint16_t>(k >> 16);      // shifts and scan code
        uint8_t scan = static_cast<uint8_t>(high);
        uint8_t ascii = static_cast<uint8_t>(k);
        uint16_t cur = e->LEd_Cur, len = e->LEd_Long;
        if (scan == ScanLeft) {
            if (!cur)
                continue;
            if (high & 0x0300) {
                // A word to the left
                do
                    e->LEd_Cur = --cur;
                while (cur && is_letter(b[cur - 1]));
            } else if (high & 0x1800) {
                e->LEd_Cur = 0;
            } else {
                e->LEd_Cur--;
            }
        } else if (scan == ScanRight) {
            if (cur >= len)
                continue;
            if (high & 0x0300) {
                do
                    e->LEd_Cur = ++cur;
                while (cur < len && is_letter(b[cur - 1]));
            } else if (high & 0x1800) {
                e->LEd_Cur = len;
            } else {
                e->LEd_Cur++;
            }
        } else if (scan == ScanBackSpace || scan == ScanDelete) {
            // (The assembler tested the shifts in the ASCII word, where they
            // never are)
            if (k & 0x0300) {
                e->LEd_Long = e->LEd_Cur = 0;
                state(e) |= StateClear;
            } else {
                if (scan == ScanBackSpace) {
                    if (!cur)
                        continue;
                    e->LEd_Cur = --cur;
                }
                if (cur >= len)
                    continue;
                for (uint16_t i = cur; i < len; i++)
                    b[i] = b[i + 1];
                e->LEd_Long--;
                state(e) |= StateEraseRight;
            }
        } else if (ascii == 13) {
            return done(1, k);
        } else if (ascii < 32) {
            if (options(e) & LEdKeys)
                return done(0, k);
            continue;
        } else {
            if (options(e) & LEdFilter) {
                // Only the characters of the mask
                if (ascii >= 128)
                    goto nothing;
                const uint8_t *mask = reinterpret_cast<const uint8_t *>(e->LEd_Mask) - 4;
                if (!(mask[ascii >> 3] >> (7 - (ascii & 7)) & 1))
                    goto nothing;
            }
            if (len >= e->LEd_Max) {
                // Full: only the last character changes
                if (cur != len)
                    continue;
                b[cur - 1] = ascii;
            } else {
                for (uint16_t i = len; i > cur; i--)
                    b[i] = b[i - 1];
                b[cur] = ascii;
                e->LEd_Cur++;
                e->LEd_Long++;
            }
        }
        led_print(e);
        if (options(e) & LEdTests)
            dia_patch();
        continue;
    nothing:
        // A filtered character counts as no key
        if (options(e) & LEdOnce)
            return done(0, k);
    }
}

// ---- register entries --------------------------------------------------------

// LEd_Init: a0 = structure, a1 = text, a2 = buffer, d0 = flags, d2 =
// length, d3 = width. Out: d0 = 0, or -1 (no room).
extern "C" uint32_t lib_led_init(RegFrame &r)
{
    bool ok = led_init(ptr<LineEdit>(r.a[0]), ptr<uint8_t>(r.a[1]), ptr<uint8_t>(r.a[2]),
                       static_cast<uint8_t>(r.d[0]), static_cast<uint16_t>(r.d[2]), static_cast<int16_t>(r.d[3]));
    return r.d[0] = ok ? 0 : -1u;
}

// LEd_Loop: a0 = structure. Out: a0 = d0... the line (d0 its length), d1 =
// the key, d2 = the code (TST.W).
extern "C" uint32_t lib_led_loop(RegFrame &r)
{
    LineEdit *e = ptr<LineEdit>(r.a[0]);
    uint32_t key = 0;
    int16_t code = led_loop(e, &key);
    r.d[0] = e->LEd_Long;
    r.a[0] = e->LEd_Buffer;
    r.d[1] = key;
    r.d[2] = static_cast<uint32_t>(static_cast<int32_t>(code));
    return static_cast<uint32_t>(static_cast<int32_t>(code));
}

// LEd_New: a0 = structure, a1 = text, d0 = length (-1: up to the zero)
extern "C" uint32_t lib_led_new(RegFrame &r)
{
    led_new(ptr<LineEdit>(r.a[0]), ptr<uint8_t>(r.a[1]), static_cast<uint16_t>(r.d[0]));
    return r.d[0];
}
