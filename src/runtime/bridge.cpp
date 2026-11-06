// The bridge between the C++ and the assembler's entries (core/amos.h
// amos_call...): a "call into the assembler" is what the glue at that
// address does (runtime/glue.cpp).

#include "runtime/runtime.h"
#include "runtime/glue.h"


using namespace amos;


namespace {

// The registers of a call: d0-d7/a0-a2 from r, a3-a6 from the context
AllRegs all(const Regs &r)
{
    const Ctx &c = ctx();
    AllRegs a;
    for (int i = 0; i < 8; i++)
        a.d[i] = r.d[i];
    for (int i = 0; i < 3; i++)
        a.a[i] = r.a[i];
    a.a[3] = address(c.a3);
    a.a[4] = encode_context_cursor(c.a4);
    a.a[5] = encode_data_zone(c.a5);
    a.a[6] = address(c.a6);
    return a;
}

void back(Regs &r, const AllRegs &a)
{
    for (int i = 0; i < 8; i++)
        r.d[i] = a.d[i];
    for (int i = 0; i < 3; i++)
        r.a[i] = a.a[i];
}

} // namespace

#if defined(__mc68000__)
extern "C" {

// The context is kept: the callee may leave other values
void amos_call(Regs *r, const void *fn)
{
    Ctx saved = ctx();
    AllRegs a = all(*r);
    host::glue_call(fn, a);
    back(*r, a);
    ctx() = saved;
}

void amos_call_all(AllRegs *r, const void *fn)
{
    Ctx saved = ctx();
    host::glue_call(fn, *r);
    ctx() = saved;
}

// The routine reads the program: a3 and a6 stay as it leaves them
void amos_eval(Regs *r, const void *fn)
{
    Ctx saved = ctx();
    AllRegs a = all(*r);
    host::glue_call(fn, a);
    back(*r, a);
    ctx() = saved;
    ctx().a3 = ptr<uint8_t>(a.a[3]);
    ctx().a6 = ptr<uint8_t>(a.a[6]);
}

// -1 "less than", 0 "equal", 1 "greater than"
int32_t amos_call_cc(Regs *r, const void *fn)
{
    Ctx saved = ctx();
    AllRegs a = all(*r);
    int32_t cc = host::glue_call(fn, a);
    back(*r, a);
    ctx() = saved;
    return cc < 0 ? -1 : cc > 0;
}

[[noreturn]] void amos_goto(const void *fn, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    AllRegs a = all(r);
    host::glue_call(fn, a);
    host::missing("return from a jump to", fn);
}

} // extern "C"

#else
namespace amos {

// The context is kept: the callee may leave other values
void amos_call(Regs *r, host::RoutineEntry fn)
{
    Ctx saved = ctx();
    AllRegs a = all(*r);
    host::glue_call(fn, a);
    back(*r, a);
    ctx() = saved;
}

void amos_call_all(AllRegs *r, host::RoutineEntry fn)
{
    Ctx saved = ctx();
    host::glue_call(fn, *r);
    ctx() = saved;
}

// The routine reads the program: a3 and a6 stay as it leaves them
void amos_eval(Regs *r, host::RoutineEntry fn)
{
    Ctx saved = ctx();
    AllRegs a = all(*r);
    host::glue_call(fn, a);
    back(*r, a);
    ctx() = saved;
    ctx().a3 = ptr<uint8_t>(a.a[3]);
    ctx().a6 = ptr<uint8_t>(a.a[6]);
}

// -1 "less than", 0 "equal", 1 "greater than"
int32_t amos_call_cc(Regs *r, host::RoutineEntry fn)
{
    Ctx saved = ctx();
    AllRegs a = all(*r);
    int32_t cc = host::glue_call(fn, a);
    back(*r, a);
    ctx() = saved;
    return cc < 0 ? -1 : cc > 0;
}

[[noreturn]] void amos_goto(host::RoutineEntry fn, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    AllRegs a = all(r);
    host::glue_call(fn, a);
    host::missing("return from a jump to", fn);
}

} // namespace amos

#endif
