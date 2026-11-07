// Bits of a variable or of memory (+ILib.s memory.s): Bset, Bclr, Bchg,
// =Btst, Ror.B/W/L, Rol.B/W/L.
//
// "Bset n,v": the second parameter is a variable alone (its long; Ror.B
// and .W: its low byte and word) or else an address (a byte there, or the
// word / long at it). The bit number counts modulo the size, and a
// rotation modulo 64 then the size, as the 68000 does it.

#include "interp/interp.h"

using namespace amos;
using namespace amos::interp;

namespace {

struct Target {
    uint8_t *at;
    uint32_t bit;
    bool memory;        // an address, not a variable
};

bool finished(uint16_t t) { return !t || t == tk::DP || t == tk::Then || t == tk::Else; }

// BsRout
Target target()
{
    Value n = integer();
    if (int32_t(n.v1) < 0)
        fon_call();
    pc() += 2;
    if (peek_token() == tk::Var) {
        ContextCursor back = pc();
        FoundVar v = find_variable();
        uint16_t t = next_token();
        if (t == tk::Par2)
            return {v.at, n.v1, false};
        pc() -= 2;
        if (finished(t))
            return {v.at, n.v1, false};
        pc() = back;
    }
    return {ptr<uint8_t>(integer().v1), n.v1, true};
}

enum class Op { Set, Clear, Change };

void bit(Op op)
{
    Target t = target();
    if (t.memory) {
        uint8_t m = uint8_t(1 << (t.bit & 7));
        *t.at = op == Op::Set ? *t.at | m : op == Op::Clear ? *t.at & ~m : *t.at ^ m;
    } else {
        uint32_t m = 1u << (t.bit & 31);
        uint32_t v = peek_l(t.at);
        poke_l(t.at, op == Op::Set ? v | m : op == Op::Clear ? v & ~m : v ^ m);
    }
}

uint32_t rotate(uint32_t v, uint32_t count, int bits, bool right)
{
    count = (count & 63) % bits;
    uint32_t mask = bits == 32 ? 0xFFFFFFFFu : (1u << bits) - 1;
    v &= mask;
    if (!count)
        return v;
    if (right)
        return ((v >> count) | (v << (bits - count))) & mask;
    return ((v << count) | (v >> (bits - count))) & mask;
}

void rotation(int bits, bool right)
{
    Target t = target();
    // a variable: its low byte or word
    uint8_t *at = t.memory ? t.at : t.at + 4 - bits / 8;
    switch (bits) {
    case 8:
        *at = uint8_t(rotate(*at, t.bit, 8, right));
        break;
    case 16:
        poke_w(at, uint16_t(rotate(peek_w(at), t.bit, 16, right)));
        break;
    default:
        poke_l(at, rotate(peek_l(at), t.bit, 32, right));
    }
}

} // namespace

extern "C" {
void interp_in_bset() { bit(Op::Set); }
void interp_in_bclr() { bit(Op::Clear); }
void interp_in_bchg() { bit(Op::Change); }

// =Btst(n,v)
void interp_fn_btst(EvalOut *o)
{
    pc() += 2;
    Target t = target();
    bool set = t.memory ? *t.at & (1 << (t.bit & 7)) : peek_l(t.at) & (1u << (t.bit & 31));
    *o = {0, 0, TInt, set ? 0xFFFFFFFFu : 0, 0, 0};
}

void interp_in_ror_b() { rotation(8, true); }
void interp_in_ror_w() { rotation(16, true); }
void interp_in_ror_l() { rotation(32, true); }
void interp_in_rol_b() { rotation(8, false); }
void interp_in_rol_w() { rotation(16, false); }
void interp_in_rol_l() { rotation(32, false); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_bits.inc"
#endif
