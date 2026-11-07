// The line editor of AMOSPro.Lib (LEd_* in Dialog.s): one line of text
// edited in the current window, for Input, the edit zones of the Interface
// and the editor.

#include "core/guest_scratch.h"
#include "lib/dialog.h"
#include "hal/clipboard.h"
#if !defined(__m68k__)
#include "host/clipboard_text.h"
#include <string.h>
#endif

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
    r.a[1] = guest_text(s);
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

#if !defined(__m68k__)
// External selection state: do not enlarge the guest LineEdit layout.
bool clipboard_active;
LineEdit *selection_owner;
int selection_anchor = -1;
bool direct_line(LineEdit *e)
{
    return clipboard_active && idata().Direct && e == reinterpret_cast<LineEdit *>(idata().Es_LEd);
}
void selection_reset(LineEdit *e)
{
    if (selection_owner == e) selection_anchor = -1;
}
bool selection(LineEdit *e, unsigned &first, unsigned &last)
{
    if (!direct_line(e) || selection_owner != e || selection_anchor < 0) return false;
    first = unsigned(selection_anchor); last = e->LEd_Cur;
    if (first > last) { unsigned t = first; first = last; last = t; }
    return last <= e->LEd_Long && first != last;
}
void erase_selection(LineEdit *e, unsigned first, unsigned last)
{
    unsigned len = e->LEd_Long;
    memmove(buffer(e) + first, buffer(e) + last, len - last);
    e->LEd_Long = len - (last - first); e->LEd_Cur = first;
    buffer(e)[e->LEd_Long] = 0;
    selection_reset(e); state(e) |= StateClear;
}
bool direct_key(LineEdit *e, uint32_t key)
{
    if (!direct_line(e)) return false;
    if (selection_owner != e) { selection_owner = e; selection_anchor = -1; }
    unsigned first = 0, last = 0;
    bool selected = selection(e, first, last);
    auto command = hal::clipboard_command(key);
    if (command != hal::ClipboardCommand::None) {
        using hal::ClipboardCommand;
        if (command == ClipboardCommand::Ignored) return true;
        std::string text, utf8;
        if (command == ClipboardCommand::Copy || command == ClipboardCommand::Cut) {
            if (!selected) { first = 0; last = e->LEd_Long; }
            if (first == last) return true;
            text.assign(reinterpret_cast<char *>(buffer(e) + first), last - first);
            if (!host::clipboard::encode(text, utf8) || !hal::clipboard_write(utf8.c_str())) {
                hal::clipboard_message("Clipboard write failed; command unchanged"); return true;
            }
            if (command == ClipboardCommand::Cut) erase_selection(e, first, last);
        } else {
            char *native;
            if (!hal::clipboard_read(native)) { hal::clipboard_message("System clipboard has no available text"); return true; }
            bool valid = host::clipboard::decode(native, text);
            hal::clipboard_release(native);
            if (!valid || !host::clipboard::command_line(text)) {
                hal::clipboard_message("Paste one representable command line; multiline text belongs in the source editor"); return true;
            }
            if (text.empty()) return true;
            if (!selected) first = last = e->LEd_Cur;
            unsigned len = e->LEd_Long;
            if (len - (last - first) + text.size() > e->LEd_Max) {
                hal::clipboard_message("Clipboard: command exceeds the direct-mode line capacity"); return true;
            }
            memmove(buffer(e) + first + text.size(), buffer(e) + last, len - last);
            memcpy(buffer(e) + first, text.data(), text.size());
            e->LEd_Long = len - (last - first) + text.size();
            e->LEd_Cur = first + text.size(); buffer(e)[e->LEd_Long] = 0;
            selection_reset(e); state(e) |= StateClear;
        }
        return true;
    }
    unsigned scan = (key >> 16) & 255, shifts = key >> 24;
    if (scan == ScanLeft || scan == ScanRight || scan == 0x70 || scan == 0x71) {
        unsigned cur = e->LEd_Cur;
        if (shifts & 3) {
            if (selection_anchor < 0) selection_anchor = cur;
            if (scan == ScanLeft) { if (cur) --cur; }
            else if (scan == ScanRight) { if (cur < e->LEd_Long) ++cur; }
            else cur = scan == 0x70 ? 0 : unsigned(e->LEd_Long);
            e->LEd_Cur = cur; state(e) |= StateClear; return true;
        }
        selection_reset(e); state(e) |= StateClear;
        if (scan == 0x70 || scan == 0x71) {
            e->LEd_Cur = scan == 0x70 ? 0 : unsigned(e->LEd_Long); return true;
        }
        return false;
    }
    unsigned ascii = key & 255;
    if (selected && (scan == ScanBackSpace || scan == ScanDelete || (ascii >= 32 && ascii != 127))) {
        erase_selection(e, first, last);
        return scan == ScanBackSpace || scan == ScanDelete;
    }
    selection_reset(e); state(e) |= StateClear;
    return false;
}
#endif

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
#if !defined(__m68k__)
    unsigned first, last;
    if (selection(e, first, last)) {
        for (int i = 0; i < n; ++i) {
            unsigned at = e->LEd_Start + i;
            print(at >= first && at < last ? "\x1bW2" : "\x1bW0");
            chr_out(buffer(e)[at]);
        }
        print("\x1bW0");
    } else
#endif
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
#if !defined(__m68k__)
    selection_reset(e);
#endif
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
#if !defined(__m68k__) && !defined(__APPLE__)
        // Test_PaSaut jumps to the interrupt handler; consume direct-mode
        // copy first. Running programs and other line editors retain Ctrl+C.
        if (direct_line(e) && (w.Actualise & (1 << BitControl))) {
            w.Actualise &= ~(1 << BitControl);
            direct_key(e, 0x08330043); led_print(e); continue;
        }
#endif
        if (options(e) & LEdTests) {
            Regs r = {};
            lib_call(L_Test_PaSaut, r);
            uint16_t a = w.Actualise & idata().ActuMask;
            if (a & 1 << BitControl) {
                w.Actualise = static_cast<uint16_t>(a & ~(1 << BitControl));
#if !defined(__m68k__) && !defined(__APPLE__)
                if (direct_line(e)) { direct_key(e, 0x08330043); led_print(e); continue; }
#endif
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
#if !defined(__m68k__)
                                if (direct_line(e)) {
                                    selection_owner = e;
                                    Regs shift = {}; sy_call(sy::Shifts, shift);
                                    if (shift.d[1] & 3) {
                                        if (selection_anchor < 0) selection_anchor = e->LEd_Cur;
                                    } else selection_reset(e);
                                    state(e) |= StateClear;
                                }
#endif
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
#if !defined(__m68k__)
        if (direct_key(e, k)) { led_print(e); continue; }
#endif
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

#if !defined(__m68k__)
void amos::hal::clipboard_direct_active(bool active) { clipboard_active = active; }
#endif

#if !defined(__mc68000__)
#include "runtime/bindings/lib_lineedit.inc"
#endif
