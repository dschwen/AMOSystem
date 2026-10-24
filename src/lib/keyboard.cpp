// Keyboard and joystick functions of AMOSPro.Lib

#include "lib/strings.h"

using namespace amos;

namespace {

uint32_t joy(uint32_t port)
{
    if (port > 1)
        fon_call();
    Regs r = {};
    r.d[1] = port;
    sy_call(sy::Joy, r);
    return r.d[1];
}

uint32_t joy_bit(uint32_t port, int bit) { return joy(port) & (1u << bit) ? -1u : 0; }

} // namespace

// =INKEY$: the next key, or the empty string. Keeps its scan code and
// shifts for Scancode and Scanshift.
extern "C" uint32_t lib_fn_inkey()
{
    Regs r = {};
    sy_call(sy::Inkey, r);
    if (!r.d[1])
        return str_empty();
    idata().SScan = static_cast<uint16_t>(r.d[1] >> 16);
    uint8_t *s = str_alloc(2);
    poke_w(s, 1);
    s[2] = static_cast<uint8_t>(r.d[1]);
    idata().HiChaine = address(s + 4);
    return address(s);
}

// =SCANCODE, =SCANSHIFT of the last Inkey$; reading clears them
extern "C" uint32_t lib_fn_scancode()
{
    auto scan = reinterpret_cast<uint8_t *>(&idata().SScan);
    uint8_t code = scan[1];
    scan[1] = 0;
    return code;
}

extern "C" uint32_t lib_fn_scanshift()
{
    auto scan = reinterpret_cast<uint8_t *>(&idata().SScan);
    uint8_t shift = scan[0];
    scan[0] = 0;
    return shift;
}

// =KEY STATE(scancode)
extern "C" uint32_t lib_fn_key_state(uint32_t code)
{
    if (code >= 128)
        fon_call();
    Regs r = {};
    r.d[1] = code;
    sy_call(sy::Instant, r);
    return r.d[1];
}

// =KEY SHIFT
extern "C" uint32_t lib_fn_key_shift()
{
    Regs r = {};
    sy_call(sy::Shifts, r);
    return r.d[1];
}

// =JOY(port), =JUP, =JDOWN, =JLEFT, =JRIGHT, =FIRE
extern "C" uint32_t lib_fn_joy(uint32_t port) { return joy(port); }
extern "C" uint32_t lib_fn_jup(uint32_t port) { return joy_bit(port, 0); }
extern "C" uint32_t lib_fn_jdown(uint32_t port) { return joy_bit(port, 1); }
extern "C" uint32_t lib_fn_jleft(uint32_t port) { return joy_bit(port, 2); }
extern "C" uint32_t lib_fn_jright(uint32_t port) { return joy_bit(port, 3); }
extern "C" uint32_t lib_fn_fire(uint32_t port) { return joy_bit(port, 4); }

// PUT KEY a$: type a$ into the keyboard buffer
extern "C" void lib_in_put_key(const uint8_t *s)
{
    if (str_len(s) >= 64)
        error(21);
    Regs r = {};
    r.a[1] = address(str_to_buffer(s));
    sy_call(sy::PutKey, r);
}

// CLEAR KEY
extern "C" void lib_in_clear_key()
{
    Regs r = {};
    sy_call(sy::ClearKey, r);
}

// KEY$(n)=a$: define function key n (1-20)
extern "C" void lib_in_key(uint32_t n, const uint8_t *s)
{
    char *text = str_to_buffer(s);
    if (n - 1 >= 20)
        fon_call();
    Regs r = {};
    r.d[1] = n - 1;
    r.a[1] = address(text);
    sy_call(sy::SetFunk, r);
}

// =KEY$(n): the definition of function key n, made printable: Return
// becomes "`" (the character after it is dropped), "'" is left out and
// other control characters become ".".
extern "C" uint32_t lib_fn_key(uint32_t n)
{
    if (n - 1 >= 20)
        fon_call();
    Regs r = {};
    r.d[1] = n - 1;
    sy_call(sy::GetFunk, r);
    const uint8_t *from = ptr<uint8_t>(r.a[0]);
    char *buffer = ptr<char>(idata().Buffer);
    char *to = buffer;
    for (;;) {
        *to = 0;
        uint8_t c = *from++;
        if (!c)
            break;
        if (c == 13) {
            c = '`';
            from++;
        } else if (c == '\'' || c == 1) {
            continue;
        } else if (c < 32) {
            c = '.';
        }
        *to++ = static_cast<char>(c);
    }
    return str_from_cstr(buffer);
}

// =SCAN$(scancode[,shifts]): the string Put Key reads as that key
extern "C" uint32_t lib_fn_scan2(uint32_t code, uint32_t shifts)
{
    if (code >= 256 || shifts >= 256)
        fon_call();
    const char text[4] = {1, static_cast<char>(shifts), static_cast<char>(code), 0};
    return str_from(text, 4);
}

extern "C" uint32_t lib_fn_scan1(uint32_t code) { return lib_fn_scan2(code, 0); }
