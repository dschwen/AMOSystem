// The parameter collectors of AMOSPro.Lib (+ILib.s Parameters) in C++.
// The loader (main/libraries.cpp Library_GetParams) marks each
// instruction's and function's routine with parameters with the number
// of its collector, in the 4 bytes before it ("PA", the number); here
// the collector's work is done: the parameters evaluated and converted,
// all but the last on the a3 stack, the last one in d2/d3(/d4) for the
// routine.

#include "interp/interp.h"
#include "runtime/detail/legacy_code_reader.h"

using namespace amos;
using host::detail::IdentityDataMemory;
using host::detail::require_guest;

namespace amos::interp {

namespace {

constexpr uint16_t ParamsMark = 0x5041; // "PA"

enum Kind { Integers, Floats, Variable, Angle, Maths };

// The collectors of Parameters' table (+ILib.s): kind and number of
// parameters (-1: the number of the token's definition). The extensions'
// (20-39) are the same, the number of theirs given in d2.
struct Collector {
    Kind kind;
    int8_t count;
};
constexpr Collector collectors[20] = {
    {Integers, 0}, {Integers, 1}, {Integers, 2}, {Integers, 3}, {Integers, 4}, {Integers, -1},
    {Floats, 0},   {Floats, 1},   {Floats, 2},   {Floats, -1},  {Floats, -1},  {Floats, -1},
    {Variable, 0}, {Variable, 1}, {Variable, 2}, {Variable, 3}, {Variable, 4}, {Variable, -1},
    {Angle, 1},    {Maths, 1}};

} // namespace

// The collector of a routine (its number), or -1
int collector_of(host::RoutineArg routine)
{
    host::detail::LegacyCodeReader r(routine);
    return r.word_at(-4) == ParamsMark ? int(r.word_at(-2)) : -1;
}

namespace {

Value convert(Kind kind, const Value &v)
{
    if (kind == Floats) {
        if (interp::kind(v) == TInt)
            return to_float(v);
    } else if (interp::kind(v) == TFloat) {
        return to_integer(v);
    }
    return v;
}

void push(uint32_t value)
{
    ctx().a3 -= 4;
    poke_l(ctx().a3, value);
}

} // namespace

Collected collect(TokenTable table, uint16_t t, host::RoutineArg routine, bool function)
{
    Collected c = {routine, {}, false};
    int k = collector_of(routine);
    if (k < 0)
        return c;
    Collector co = collectors[k % 20];
    int count = co.count >= 0 ? co.count : require_guest(IdentityDataMemory{}.load_be16(table + t + 2));
    Value v = {};
    switch (co.kind) {
    case Integers:
    case Floats:
        for (int i = 0; i < count; i++) {
            if (i)
                push(v.v1);
            if (i || function)
                pc() += 2;
            v = convert(co.kind, evaluate());
        }
        c.has_value = count > 0;
        break;
    case Variable:
        // the parameters in brackets, then = and the value
        for (int i = 0; i < count; i++) {
            pc() += 2;
            push(convert(Integers, evaluate()).v1);
        }
        pc() += 2;
        v = convert(Integers, evaluate());
        c.has_value = true;
        break;
    case Angle:
        pc() += 2;
        v = evaluate();
        if (interp::kind(v) == TInt)
            v = to_float(v);
        {
            Regs a;
            a.d[2] = v.type;
            a.d[3] = v.v1;
            a.d[4] = v.v2;
            lib_call(L_FFAngle, a);
            v = {a.d[2], a.d[3], a.d[4]};
        }
        c.has_value = true;
        break;
    case Maths: {
        // an integer to the routine, a float to the one after it (which
        // has no collector)
        pc() += 2;
        v = evaluate();
        if (interp::kind(v) != TInt) {
            int16_t offset = int16_t(require_guest(IdentityDataMemory{}.load_be16(table + t)));
            c.body = host::decode_routine(require_guest(IdentityDataMemory{}.load_be32(table + offset - LB_Size - 8)));
        }
        c.has_value = true;
        break;
    }
    }
    c.last = v;
    return c;
}

} // namespace amos::interp
