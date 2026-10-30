// Errors, random numbers, timers and waits of AMOSPro.Lib

#include "lib/convert.h"
#include "lib/strings.h"

using namespace amos;

namespace {

// The VBL counters, changed by the interrupt
inline uint32_t vbl_count() { return *static_cast<volatile be_u32 *>(&wdata().VblCount); }

void test_normal()
{
    Regs r = {};
    lib_call(L_Test_Normal, r);
}

// Sleep until something happens (L_Sys_WaitMul), then look at it
void wait_multi()
{
    boot::wait_vbl_multi();
    test_normal();
}

// L_Wait_Normal: wait n VBLs, letting Every and the menus run
void wait_normal(uint32_t n)
{
    uint32_t until = n + vbl_count();
    while (until > vbl_count())
        wait_multi();
}

// L_Mulu32: the assembler's 32-bit multiplication. It adds the cross
// products swapped, not shifted, which Rnd's sequence depends on.
uint32_t mulu32(uint32_t a, uint32_t b)
{
    auto swap = [](uint32_t v) { return v << 16 | v >> 16; };
    uint32_t a_lo = a & 0xFFFF, a_hi = a >> 16, b_lo = b & 0xFFFF, b_hi = b >> 16;
    return a_lo * b_lo + swap(a_hi * b_lo) + swap(a_lo * b_hi);
}

// The next random number of the sequence, 24 bits
uint32_t next_random()
{
    IData &d = idata();
    d.Seed = mulu32(d.Seed, 0xbb40e62d) + 1;
    return d.Seed >> 8;
}

} // namespace

// =ERRN: the last error number
extern "C" uint32_t lib_fn_errn()
{
    uint16_t n = idata().ErrorOn;
    return n ? n - 1u : 0;
}

// =ERR$(n): the text of an error (L_Mes2Chaine)
extern "C" uint32_t lib_fn_err(uint32_t n)
{
    Regs r = {};
    r.d[0] = n + 1;
    r.a[0] = idata().Ed_RunMessages;
    r.a[0] = address(boot::get_message(ptr<uint8_t>(r.a[0]), r.d[0]));
    const uint8_t *text = ptr<uint8_t>(r.a[0]);
    uint8_t len = text[-1];
    if (!len || len == 0xFF)
        return str_empty();
    return str_from(text, len);
}

// =STR$(n), integer
extern "C" uint32_t lib_fn_str_int(int32_t n)
{
    uint8_t *s = str_alloc(16);
    return str_finish(s, reinterpret_cast<uint8_t *>(long_to_dec(reinterpret_cast<char *>(s + 2), n, -1, true)));
}

// RADIAN, DEGREE
extern "C" void lib_in_radian() { idata().Angle = 0; }
extern "C" void lib_in_degree() { idata().Angle = 0xFFFF; }

// FIX n: digits after the point; negative for the exponent form, 16 and
// over for "as many as needed"
extern "C" void lib_in_fix(int32_t n)
{
    IData &d = idata();
    d.ExpFlg = n < 0;
    uint32_t digits = n < 0 ? 0u - static_cast<uint32_t>(n) : static_cast<uint32_t>(n);
    d.FixFlg = digits < 16 ? static_cast<uint16_t>(digits) : 0xFFFF;
}

// =RND(n): 0 to n. For a positive n the beam position stirs it in. 0
// gives the last number again.
extern "C" uint32_t lib_fn_rnd(int32_t n)
{
    IData &d = idata();
    if (!n)
        return d.OldRnd;
    uint32_t max = n < 0 ? 0u - static_cast<uint32_t>(n) : static_cast<uint32_t>(n);
    uint16_t beam_mask = n < 0 ? 0 : 0xFFFF;
    // the smallest mask 2^k-1 that covers max, as the assembler finds it
    uint32_t mask = 0xFFFFFF, carry = 0;
    for (int i = 0; i < 24; i++) {
        carry = mask & 1;
        mask >>= 1;
        if (mask < max)
            break;
    }
    mask = mask << 1 | carry;
    uint32_t value;
    do {
        value = next_random();
        uint16_t beam = *reinterpret_cast<volatile be_u16 *>(0xdff006) & beam_mask;
        value = (value & 0xFFFF0000u) | static_cast<uint16_t>(value + beam);
        value &= mask;
    } while (value > max);
    return d.OldRnd = value;
}

// L_Mulu32: d2 * d3 into d1 (d0 is lost and d3 comes back swapped)
extern "C" uint32_t lib_mulu32(RegFrame &r)
{
    uint32_t a = r.d[2], b = r.d[3];
    r.d[1] = mulu32(a, b);
    r.d[0] = (a & 0xFFFF) * (b >> 16);
    r.d[3] = b << 16 | b >> 16;
    return r.d[1];
}

// RANDOMIZE n
extern "C" void lib_in_randomize(uint32_t n) { idata().Seed = n; }

// TIMER=n, =TIMER
extern "C" void lib_in_timer(uint32_t n) { wdata().VblTimer = n; }
extern "C" uint32_t lib_fn_timer() { return *static_cast<volatile be_u32 *>(&wdata().VblTimer); }

// =PI#, as a float in d3 or a double in d3/d4
extern "C" uint64_t lib_fn_pi()
{
    return static_cast<uint64_t>(idata().ValPi[0]) << 32 | idata().ValPi[1];
}

// WAIT n: n VBLs; 0 waits for ever (until Every, a menu or Break)
extern "C" void lib_in_wait(int32_t n)
{
    if (n < 0)
        fon_call();
    if (n)
        wait_normal(n);
    else
        for (;;)
            wait_multi();
}

// L_WaitRout (extensions): d3 VBLs, without Every and the menus
extern "C" uint32_t lib_wait_rout(RegFrame &r)
{
    uint32_t until = r.d[3] + vbl_count();
    while (until > vbl_count()) {
        Regs t = {};
        lib_call(L_Test_PaSaut, t);
    }
    return 0;
}

// MULTI WAIT, WAIT VBL, WAIT KEY
extern "C" void lib_in_multi_wait() { wait_multi(); }

extern "C" void lib_in_wait_vbl()
{
    Regs r = {};
    sy_call(sy::WaitVbl, r);
    test_normal();
}

extern "C" void lib_in_wait_key()
{
    for (;;) {
        Regs r = {};
        sy_call(sy::Inkey, r);
        if (r.d[1])
            return;
        wait_multi();
    }
}

// KEY SPEED delay,speed
extern "C" void lib_in_key_speed(int32_t delay, int32_t speed)
{
    if (speed < 0 || delay < 0)
        fon_call();
    Regs r = {};
    r.d[1] = delay;
    r.d[2] = speed;
    r.a[1] = idata().Buffer;
    sy_call(sy::KeySpeed, r);
}

// =PARAM$, =PARAM: what the last procedure returned
extern "C" uint32_t lib_fn_param_string() { return idata().ParamC; }
extern "C" uint32_t lib_fn_param_int() { return idata().ParamE; }
