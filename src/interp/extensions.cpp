// Instructions and functions of the extensions (+ILib.s InExtCall,
// FnExtCall): the extension token, the extension's number and then
//   - an AMOSPro extension (the second byte negative): the offset of the
//     token in the extension's table, whose routine is called as the
//     interpreter calls its own (it collects its parameters itself);
//   - an AMOS 1.3 one: the number of parameters and the offset; the
//     parameters (integers or strings) are evaluated onto the a3 stack
//     here, and the routine runs with the interpreter's d6/d7 kept in
//     ErrorSave (it may use them).

#include "runtime/detail/identity_data_memory.h"
#include "interp/interp.h"

using namespace amos;
using namespace amos::interp;

namespace {

Value run(host::RoutineArg routine, Regs &r)
{
    amos_eval(&r, routine);
    return {r.d[2], r.d[3], r.d[4]};
}

Value call(bool function)
{
    IData &d = idata();
    Ctx &c = ctx();
    uint8_t number = next_byte();
    int8_t n = int8_t(next_byte());
    uint8_t *table = ptr<uint8_t>(d.AdTokens[number]);
    Regs r;
    if (n < 0) {
        uint16_t token = next_token();
        int16_t jump = int16_t(peek_w(table + token));
        r.d[0] = token;
        r.d[1] = uint16_t(jump);
        r.d[2] = peek_w(table + token + 2);
        r.a[0] = peek_l(table + jump - LB_Size - (function ? 4 : 0));
        Collected p = collect(guest::abi::AddressCodec::decode<const guest::Byte>(d.AdTokens[number]), token, host::decode_routine(r.a[0]), function);
        if (p.has_value) {
            r.d[2] = p.last.type;
            r.d[3] = p.last.v1;
            r.d[4] = p.last.v2;
        }
        return run(p.body, r);
    }
    uint16_t token = next_token();
    host::RoutineArg routine = host::decode_routine(uint32_t(d.AdTokens[number]) + int32_t(int16_t(peek_w(table + token + (function ? 2 : 0)))));
    Value v = {};
    for (int8_t i = 0; i < n; i++) {
        v = function ? evaluate_after() : evaluate();
        if (kind(v) == TFloat)
            v = to_integer(v);
        c.a3 -= 4;
        host::detail::write_cursor_long(c.a3, v.v1);
        if (!function)
            pc() += 2;
    }
    if (n && !function)
        pc() -= 2;
    d.ErrorSave[0] = c.d6;
    d.ErrorSave[1] = c.d7;
    d.ErrorRegs = 1;
    r.d[2] = v.type;
    r.d[3] = v.v1;
    r.d[4] = v.v2;
    r.d[0] = uint16_t(n);
    r.d[1] = number * 4u;
    r.a[0] = host::encode_routine(routine);
    Value x = run(routine, r);
    c.d6 = d.ErrorSave[0];
    c.d7 = d.ErrorSave[1];
    d.ErrorRegs = 0;
    return x;
}

} // namespace

extern "C" {
void interp_in_ext_call() { call(false); }
void interp_fn_ext_call(EvalOut *o)
{
    Value v = call(true);
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_extensions.inc"
#endif
