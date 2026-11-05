// Memory access instructions of AMOSPro.Lib

#include "lib/banks.h"
#include "lib/strings.h"

using namespace amos;

namespace {

constexpr uint32_t String_Max = 0xFFC0;

// L_TransMem: copy length bytes, the right way round for overlapping zones
void transfer(const uint8_t *from, uint8_t *to, uint32_t length)
{
    if (to > from && to < from + length) {
        while (length--)
            to[length] = from[length];
    } else {
        for (uint32_t i = 0; i < length; i++)
            to[i] = from[i];
    }
}

// The registers of the machine code calls: d0-d7, then a0-a6
be_u32 *call_register(uint32_t n, uint32_t count, uint32_t first)
{
    if (n >= count)
        fon_call();
    return &idata().CallReg[first + n];
}

// L_FPeekD: up to len bytes from address, stopping before the byte stop
// (or not, if stop is negative)
uint32_t peek_string(const uint8_t *address, uint32_t len, int32_t stop)
{
    uint16_t max = static_cast<uint16_t>(len < String_Max ? len : String_Max);
    uint16_t count = 0;
    for (const uint8_t *p = address; count < max; count++) {
        uint8_t c = *p++;
        if (stop >= 0 && c == static_cast<uint8_t>(stop))
            break;
    }
    return count ? str_from(address, count) : str_empty();
}

} // namespace

// L_TransMem: a0 = from, a1 = to, d0 = length. Out: a0, a1 after the
// zones.
extern "C" uint32_t lib_trans_mem(RegFrame &r)
{
    transfer(ptr<uint8_t>(r.a[0]), ptr<uint8_t>(r.a[1]), r.d[0]);
    r.a[0] += r.d[0];
    r.a[1] += r.d[0];
    return 0;
}

// COPY start,end TO destination
extern "C" void lib_in_copy(uint32_t start, uint32_t end, uint32_t to)
{
    if (end <= start)
        fon_call();
    transfer(ptr<uint8_t>(start), ptr<uint8_t>(to), end - start);
}

// FILL start TO end,pattern: the long pattern over the zone
extern "C" void lib_in_fill(int32_t start, uint32_t end, uint32_t pattern)
{
    uint8_t *p = ptr<uint8_t>(bank_or_address(start));
    if (end < address(p))
        fon_call();
    uint32_t length = end - address(p);
    if (uint32_t blocks = length >> 4) {
        // a dbra loop: the count is a word
        for (uint32_t n = ((blocks - 1) & 0xFFFF) + 1; n; n--)
            for (int i = 0; i < 4; i++, p += 4)
                poke_l(p, pattern);
    }
    for (uint16_t rest = length & 15; rest; rest--) {
        pattern = pattern << 8 | pattern >> 24;
        *p++ = static_cast<uint8_t>(pattern);
    }
}

// =HUNT(start TO end,a$): the first address where a$ is, or 0
extern "C" uint32_t lib_fn_hunt(int32_t start, uint32_t end, const uint8_t *s)
{
    uint16_t len = str_len(s);
    const char *text = str_to_buffer(s);
    const uint8_t *p = ptr<uint8_t>(bank_or_address(start));
    if (!len)
        return 0;
    for (; address(p) < end; p++) {
        uint16_t i = 0;
        while (i < len && p[i] == static_cast<uint8_t>(text[i]))
            i++;
        if (i == len)
            return address(p);
    }
    return 0;
}

// AREG(n)=v, =AREG(n), DREG(n)=v, =DREG(n): the registers for Call,
// Execall...
extern "C" void lib_in_areg(uint32_t n, uint32_t v) { *call_register(n, 7, 8) = v; }
extern "C" uint32_t lib_fn_areg(uint32_t n) { return *call_register(n, 7, 8); }
extern "C" void lib_in_dreg(uint32_t n, uint32_t v) { *call_register(n, 8, 0) = v; }
extern "C" uint32_t lib_fn_dreg(uint32_t n) { return *call_register(n, 8, 0); }

// POKE, DOKE, LOKE. Words and longs may be at odd addresses (written byte
// by byte); at even ones they must be single accesses, for the hardware
// registers.
extern "C" void lib_in_poke(uint32_t a, uint32_t v) { *ptr<volatile uint8_t>(a) = static_cast<uint8_t>(v); }

extern "C" void lib_in_doke(uint32_t a, uint32_t v)
{
    if (!(a & 1)) {
        *ptr<volatile be_u16>(a) = static_cast<uint16_t>(v);
        return;
    }
    volatile uint8_t *p = ptr<volatile uint8_t>(a);
    p[1] = static_cast<uint8_t>(v);
    p[0] = static_cast<uint8_t>(v >> 8);
}

extern "C" void lib_in_loke(uint32_t a, uint32_t v)
{
    if (!(a & 1)) {
        *ptr<volatile be_u32>(a) = v;
        return;
    }
    volatile uint8_t *p = ptr<volatile uint8_t>(a);
    for (int i = 3; i >= 0; i--)
        p[i] = static_cast<uint8_t>(v >> (24 - 8 * i));
}

// =PEEK, =DEEK, =LEEK
extern "C" uint32_t lib_fn_peek(uint32_t a) { return *ptr<volatile uint8_t>(a); }

extern "C" uint32_t lib_fn_deek(uint32_t a)
{
    if (!(a & 1))
        return *ptr<volatile be_u16>(a);
    const volatile uint8_t *p = ptr<volatile uint8_t>(a);
    return static_cast<uint32_t>(p[0]) << 8 | p[1];
}

extern "C" uint32_t lib_fn_leek(uint32_t a)
{
    if (!(a & 1))
        return *ptr<volatile be_u32>(a);
    const volatile uint8_t *p = ptr<volatile uint8_t>(a);
    return static_cast<uint32_t>(p[0]) << 24 | static_cast<uint32_t>(p[1]) << 16 |
           static_cast<uint32_t>(p[2]) << 8 | p[3];
}

// POKE$ address,a$
extern "C" void lib_in_poke_string(uint32_t a, const uint8_t *s)
{
    uint8_t *to = ptr<uint8_t>(a);
    const uint8_t *from = str_chars(s);
    for (uint16_t i = str_len(s); i; i--)
        *to++ = *from++;
}

// =PEEK$(address,length[,stop$])
extern "C" uint32_t lib_fn_peek_string2(uint32_t a, uint32_t len) { return peek_string(ptr<uint8_t>(a), len, -1); }

extern "C" uint32_t lib_fn_peek_string3(uint32_t a, uint32_t len, const uint8_t *stop)
{
    return peek_string(ptr<uint8_t>(a), len, str_len(stop) ? str_chars(stop)[0] : -1);
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_memory.inc"
#endif
