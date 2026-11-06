// Expressions (+ILib.s Evalue.s: New_Evalue, the operators, compch,
// Max/Min).
//
// An operand is a token whose function routine (through the token table
// and the jump table, as the assembler finds it) leaves the value in
// d2/d3/d4; a minus sign before it negates it. Operators are the tokens
// $FF3E (xor) to $FFF6 (^), their offset back from the operators' table
// end: the higher the token, the tighter it binds, and equal ones go left
// to right.
//
// A float operation goes through AMOSPro.Lib's float layer (swapped for
// double precision), which takes the left operand on the a3 stack, as the
// assembler's evaluator kept it.

#include "interp/interp.h"
#include "runtime/detail/identity_data_memory.h"

using namespace amos;

// lib/strings.cpp: InstrFind
extern "C" uint32_t lib_instr_find(const uint8_t *hay, uint32_t hay_len, const uint8_t *needle, uint32_t needle_len,
                                   uint32_t start);

namespace amos::interp {

namespace {

constexpr uint16_t Top = 0x7FFF;        // below every operator
constexpr uint32_t StringMax = 0xFFC0;

// The operators' tokens
enum : uint16_t {
    OpXor = 0xFF3E, OpOr = 0xFF4C, OpAnd = 0xFF58, OpDiff1 = 0xFF66, OpDiff2 = 0xFF70, OpInfEg1 = 0xFF7A,
    OpInfEg2 = 0xFF84, OpSupEg1 = 0xFF8E, OpSupEg2 = 0xFF98, OpEgal = 0xFFA2, OpInf = 0xFFAC, OpSup = 0xFFB6,
    OpPlus = 0xFFC0, OpMoins = 0xFFCA, OpModulo = 0xFFD4, OpMult = 0xFFE2, OpDiv = 0xFFEC, OpPuis = 0xFFF6,
};

// Math library entries (mathffp / mathieeedoubbas, mathtrans / mathieeedoubtrans)
constexpr int32_t LVOAdd = -66, LVOSub = -72, LVOMul = -78, LVODiv = -84, LVOPow = -90;

[[noreturn]] void overflow() { error(EOverFlow); }
[[noreturn]] void division_by_zero() { error(EDByZero); }

Value result(const Regs &r) { return {r.d[2], r.d[3], r.d[4]}; }

Value float_routine(int routine, const Value &v)
{
    Regs r;
    r.d[2] = v.type;
    r.d[3] = v.v1;
    r.d[4] = v.v2;
    lib_call(routine, r);
    return result(r);
}

// The left operand on the a3 stack, as the float layer wants it
void push(const Value &v)
{
    uint8_t *&a3 = ctx().a3;
    a3 -= 12;
    poke_l(a3, v.type);
    poke_l(a3 + 4, v.v1);
    poke_l(a3 + 8, v.v2);
}

void pop() { ctx().a3 += 12; }

// L_Float_Operation / L_Math_Operation: a (op) b
Value float_operation(int routine, int32_t lvo, const Value &a, const Value &b)
{
    push(a);
    Regs r;
    r.d[2] = uint32_t(lvo);
    r.d[3] = b.v1;
    r.d[4] = b.v2;
    lib_call(routine, r);
    pop();
    return result(r);
}



bool float_zero(const Value &v)
{
    Regs r;
    r.d[3] = v.v1;
    r.d[4] = v.v2;
    return lib_call_cc(L_Float_Test, r) == 0;
}

// Compat: both operands of one kind, an integer made a float if the other
// is one (whatever the other is). Returns the kind to work in.
enum Kind { Int, Float, String };
Kind compat(Value &a, Value &b)
{
    uint8_t ta = kind(a), tb = kind(b);
    if (ta == tb)
        return ta == TInt ? Int : ta == TFloat ? Float : String;
    if (tb == TInt) {
        b = to_float(b);
    } else {
        // L_IntToFl2: the type's low byte only
        Value f = to_float(a);
        a = {(a.type & ~0xFFu) | TFloat, f.v1, f.v2};
    }
    return Float;
}

// QuEntier: both made integers
void integers(Value &a, Value &b)
{
    if (kind(b))
        b = to_integer(b);
    if (kind(a)) {
        Value i = to_integer(a);
        a.v1 = i.v1;
        a.type &= ~0xFFu;
    }
}

// Demande: room for a string of length bytes (may collect the garbage and
// start the instruction again). Returns where to write (a0) and the start
// (a1).
Regs demande(uint32_t length)
{
    Regs r;
    r.d[3] = length;
    lib_call(L_Demande, r);
    return r;
}

const uint8_t *string(const Value &v) { return ptr<uint8_t>(v.v1); }

Value concatenate(const Value &a, const Value &b)
{
    const uint8_t *sa = string(a), *sb = string(b);
    uint32_t lb = peek_w(sb);
    if (!lb)
        return {b.type, a.v1, b.v2};
    uint32_t la = peek_w(sa);
    if (!la)
        return b;
    uint32_t total = la + lb;
    if (total >= StringMax)
        error(21);
    Regs r = demande(total);
    uint8_t *to = ptr<uint8_t>(r.a[0]);
    poke_w(to, uint16_t(total));
    to += 2;
    for (uint32_t i = 0; i < la; i++)
        *to++ = sa[2 + i];
    for (uint32_t i = 0; i < lb; i++)
        *to++ = sb[2 + i];
    to += address(to) & 1;
    idata().HiChaine = address(to);
    return {b.type, r.a[1], b.v2};
}

// a$ - b$: every b$ taken out of a copy of a$
Value subtract(const Value &a, const Value &b)
{
    IData &d = idata();
    const uint8_t *sa = string(a), *sb = string(b);
    uint32_t length = peek_w(sa);
    Regs r = demande(length);
    uint8_t *to = ptr<uint8_t>(r.a[0]);
    poke_w(to, uint16_t(length));
    to += 2;
    for (uint32_t i = 0; i < (length + 1) / 2; i++, to += 2)
        poke_w(to, peek_w(sa + 2 + 2 * i));
    d.HiChaine = address(to);
    uint8_t *chars = ptr<uint8_t>(r.a[1]) + 2;
    const uint8_t *needle = sb + 2;
    uint32_t needle_length = peek_w(sb);
    for (;;) {
        uint32_t at = lib_instr_find(chars, length, needle, needle_length, 0);
        if (!at)
            break;
        uint32_t keep = at - 1;
        uint8_t *dest = chars + keep;
        const uint8_t *from = chars + keep + needle_length;
        uint32_t rest = length - keep - needle_length;
        for (uint32_t i = 0; i < rest; i++)
            *dest++ = *from++;
        // as the original: the end of the strings where the copy came from
        d.HiChaine = address(from) + (address(from) & 1);
        length = keep + rest;
        poke_w(chars - 2, uint16_t(length));
    }
    return {TString, r.a[1], 0};
}

// The integer multiplication: a fast 16 x 16 bits one without overflow
// test (65535 * 65535 gives -131071), else 32 x 32 bits that stops at
// 31 bits
int32_t multiply(int32_t lhs, int32_t rhs)
{
    uint32_t a = uint32_t(lhs), b = uint32_t(rhs);
    bool negative = false;
    if (int32_t(b) < 0) {
        b = -b;
        negative = !negative;
    }
    if (int32_t(a) < 0) {
        a = -a;
        negative = !negative;
    }
    if (b < 0x10000 && a < 0x10000) {
        uint32_t p = a * b;
        return int32_t(negative ? 0u - p : p);
    }
    uint32_t alo = a & 0xFFFF, ahi = a >> 16, blo = b & 0xFFFF, bhi = b >> 16;
    int32_t sum = int32_t(alo * blo);
    if (sum < 0)
        overflow();
    uint32_t p = ahi * blo;
    if (p >= 0x8000 || __builtin_add_overflow(sum, int32_t(p << 16), &sum))
        overflow();
    if (ahi * bhi != 0)
        overflow();
    p = alo * bhi;
    if (p >= 0x8000 || __builtin_add_overflow(sum, int32_t(p << 16), &sum))
        overflow();
    return int32_t(negative ? 0u - uint32_t(sum) : uint32_t(sum));
}

// The integer division: of the sizes, signs apart
int32_t divide(int32_t lhs, int32_t rhs)
{
    uint32_t a = uint32_t(lhs), b = uint32_t(rhs);
    bool negative = false;
    if (lhs < 0) {
        negative = true;
        a = -a;
    }
    if (!b)
        division_by_zero();
    if (rhs < 0) {
        negative = !negative;
        b = -b;
    }
    uint32_t q = a / b;
    return int32_t(negative ? 0u - q : q);
}

// Mod: the remainder of the unsigned left operand by the size of the right
// one (-7 mod 4 = 1); mod 0 gives the left operand
int32_t modulo(int32_t lhs, int32_t rhs)
{
    uint32_t b = rhs < 0 ? -uint32_t(rhs) : uint32_t(rhs);
    return b ? int32_t(uint32_t(lhs) % b) : lhs;
}

Value truth(bool t) { return {TInt, t ? 0xFFFFFFFFu : 0, 0}; }

// A comparison: <0, 0 or >0 for a < b, a = b, a > b in the operands' kind
int compare(Value &a, Value &b)
{
    switch (compat(a, b)) {
    case Int:
        return int32_t(a.v1) < int32_t(b.v1) ? -1 : a.v1 != b.v1;
    case Float:
        return float_compare(a, b);
    default:
        return compare_strings(string(a), string(b));
    }
}

Value apply(uint16_t op, Value a, Value b)
{
    switch (op) {
    case OpPlus:
        switch (compat(a, b)) {
        case Int: {
            int32_t s;
            if (__builtin_add_overflow(int32_t(b.v1), int32_t(a.v1), &s))
                overflow();
            return {b.type, uint32_t(s), b.v2};
        }
        case Float:
            return float_operation(L_Float_Operation, LVOAdd, a, b);
        default:
            return concatenate(a, b);
        }
    case OpMoins:
        switch (compat(a, b)) {
        case Int: {
            int32_t s;
            if (__builtin_sub_overflow(int32_t(a.v1), int32_t(b.v1), &s))
                overflow();
            return {b.type, uint32_t(s), b.v2};
        }
        case Float:
            return float_operation(L_Float_Operation, LVOSub, a, b);
        default:
            return subtract(a, b);
        }
    case OpMult:
        if (compat(a, b) == Int)
            return {TInt, uint32_t(multiply(int32_t(a.v1), int32_t(b.v1))), b.v2};
        return float_operation(L_Float_Operation, LVOMul, a, b);
    case OpDiv:
        if (compat(a, b) == Int)
            return {TInt, uint32_t(divide(int32_t(a.v1), int32_t(b.v1))), b.v2};
        if (float_zero(b))
            division_by_zero();
        return float_operation(L_Float_Operation, LVODiv, a, b);
    case OpPuis:
        if (kind(b) == TInt)
            b = to_float(b);
        compat(a, b);
        return float_operation(L_Math_Operation, LVOPow, a, b);
    case OpModulo:
        integers(a, b);
        return {TInt, uint32_t(modulo(int32_t(a.v1), int32_t(b.v1))), b.v2};
    case OpAnd:
        integers(a, b);
        return {b.type, a.v1 & b.v1, b.v2};
    case OpOr:
        integers(a, b);
        return {b.type, a.v1 | b.v1, b.v2};
    case OpXor:
        integers(a, b);
        return {b.type, a.v1 ^ b.v1, b.v2};
    case OpEgal:
        return truth(compare(a, b) == 0);
    case OpDiff1:
    case OpDiff2:
        return truth(compare(a, b) != 0);
    case OpInf:
        return truth(compare(a, b) < 0);
    case OpInfEg1:
    case OpInfEg2:
        return truth(compare(a, b) <= 0);
    case OpSup:
        return truth(compare(a, b) > 0);
    default:            // OpSupEg1, OpSupEg2
        return truth(compare(a, b) >= 0);
    }
}

// The function routine of token t
Value function(uint16_t t)
{
    uint8_t *tokens = ctx().a4;
    int16_t offset = int16_t(peek_w(tokens + t));
    host::RoutineArg routine = host::decode_routine(peek_l(tokens + offset - LB_Size - 4));
    Value v;
    if (native_function(t, routine, v))
        return v;
    Collected p = collect(host::detail::legacy_table_address(tokens), t, routine, true);
    Regs r;
    r.d[0] = t;
    r.d[1] = uint16_t(offset);
    r.a[0] = host::encode_routine(routine);
    if (p.has_value) {
        r.d[2] = p.last.type;
        r.d[3] = p.last.v1;
        r.d[4] = p.last.v2;
    }
    amos_eval(&r, p.body);
    return result(r);
}

// An operand: minus signs (any number: one negation) then a token whose
// function routine gives the value. Variables and constants are read here.
Value operand()
{
    bool minus = false;
    uint16_t t;
    while ((t = next_token()) & 0x8000)
        minus = true;
    Value v;
    switch (t) {
    case tk::Var:
        v = value();
        break;
    case tk::Ent:
    case tk::Hex:
    case tk::Bin:
        v = integer_constant();
        break;
    case tk::Fl:
        v = float_constant();
        break;
    case tk::DFl:
        v = double_constant();
        break;
    case tk::Ch1:
    case tk::Ch2:
        v = string_constant();
        break;
    default:
        v = function(t);
    }
    if (minus) {
        if (!kind(v))
            v.v1 = -v.v1;
        else if (int8_t(idata().MathFlags) < 0)
            v.v1 ^= 0x80000000u;
        else
            v.v1 ^= 0x80;       // the sign of a Motorola fast float
    }
    return v;
}

// The operand and what binds tighter than operator priority to it
Value climb(uint16_t priority)
{
    Value v = operand();
    for (;;) {
        uint16_t op = peek_token();
        if (op <= priority)
            return v;
        pc() += 2;
        Value rhs = climb(op);
        v = apply(op, v, rhs);
    }
}

} // namespace

// FnCEntier
Value integer_constant()
{
    Value v = {TInt, peek_l(pc()), 0};
    pc() += 4;
    return v;
}

// FnCstFl: a single precision float, made a double if needed
Value float_constant()
{
    uint32_t f = peek_l(pc());
    pc() += 4;
    if (int8_t(idata().MathFlags) >= 0)
        return {TFloat, f, 0};
    Value v = {TFloat, 0, 0};
    single_to_double(ffp_to_ieee(f), &v.v1, &v.v2);
    return v;
}

// FnCstDFl: a double, made a single precision float if needed
Value double_constant()
{
    uint32_t hi = peek_l(pc()), lo = peek_l(pc() + 4);
    pc() += 8;
    if (int8_t(idata().MathFlags) < 0)
        return {TFloat, hi, lo};
    return {TFloat, ieee_to_ffp(double_to_single(hi, lo)), 0};
}

// FnCstCh: the string in the program; in direct mode a copy (the line
// goes away)
Value string_constant()
{
    uint8_t *s = pc();
    uint16_t length = peek_w(s);
    pc() += 2 + length + (length & 1);
    if (!idata().Direct)
        return {TString, address(s), 0};
    Regs r = demande(length);
    uint8_t *to = ptr<uint8_t>(r.a[0]);
    poke_w(to, length);
    to += 2;
    for (uint32_t i = 0; i < (length + 1u) / 2; i++, to += 2)
        poke_w(to, peek_w(s + 2 + 2 * i));
    idata().HiChaine = address(to);
    return {TString, r.a[1], 0};
}

// L_Float_Compare: the condition codes it leaves (-1 less, 0 equal, 1 greater)
int float_compare(const Value &a, const Value &b)
{
    push(a);
    Regs r;
    r.d[2] = b.type;
    r.d[3] = b.v1;
    r.d[4] = b.v2;
    int cc = lib_call_cc(L_Float_Compare, r);
    pop();
    return cc;
}

Value to_float(const Value &v) { return float_routine(L_IntToFl1, v); }
Value to_integer(const Value &v) { return float_routine(L_FlToInt1, v); }

int compare_strings(const uint8_t *a, const uint8_t *b)
{
    uint16_t la = peek_w(a), lb = peek_w(b);
    a += 2;
    b += 2;
    for (uint16_t i = 0; i < la && i < lb; i++)
        if (a[i] != b[i])
            return a[i] < b[i] ? -1 : 1;
    return la < lb ? -1 : la != lb;
}

Value evaluate(uint16_t *end)
{
    Value v = climb(Top);
    uint16_t t = peek_token();
    if (t == tk::Par2)
        pc() += 2;
    if (end)
        *end = t;
    return v;
}

Value evaluate_after()
{
    pc() += 2;
    return evaluate();
}

Value integer()
{
    Value v = evaluate();
    return kind(v) == TFloat ? to_integer(v) : v;
}

// =Not(n): an integer's bits inverted
Value bit_not()
{
    Value v = evaluate();
    if (kind(v))
        v = to_integer(v);
    v.v1 = ~v.v1;
    return v;
}

} // namespace amos::interp

using namespace amos::interp;

namespace {

// Max(a,b) / Min(a,b). The original compared two strings by the first
// one's address and the second one's d4, whatever they held.
Value min_max(bool max)
{
    Value a = evaluate_after();
    Value b = evaluate_after();
    int c = compare(a, b);
    bool second = max ? c <= 0 : c > 0;
    return second ? b : Value{b.type, a.v1, a.v2};
}

} // namespace

extern "C" {

void interp_new_evalue(EvalOut *o)
{
    uint16_t end;
    Value v = evaluate(&end);
    // d1 as the original: the bottom of the operators' stack
    *o = {end, 0x7FFF, v.type, v.v1, v.v2, 0};
}

void interp_fn_max(EvalOut *o)
{
    Value v = min_max(true);
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}

void interp_fn_not(EvalOut *o)
{
    Value v = bit_not();
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}

void interp_fn_constant(EvalOut *o, Value (*read)())
{
    Value v = read();
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}

void interp_fn_c_entier(EvalOut *o) { interp_fn_constant(o, integer_constant); }
void interp_fn_cst_fl(EvalOut *o) { interp_fn_constant(o, float_constant); }
void interp_fn_cst_dfl(EvalOut *o) { interp_fn_constant(o, double_constant); }
void interp_fn_cst_ch(EvalOut *o) { interp_fn_constant(o, string_constant); }

void interp_fn_min(EvalOut *o)
{
    Value v = min_max(false);
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_eval.inc"
#endif
