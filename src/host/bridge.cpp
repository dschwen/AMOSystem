// The bridge between the C++ and the assembler (core/call.s) on another
// host: there is no assembler, so a "call into the assembler" is what the
// glue at that address does (host/glue.cpp).

#include "host/host.h"
#include "host/glue.h"

#include <stdio.h>
#include <stdlib.h>

using namespace amos;

namespace {
Ctx context;            // the interpreter's registers (one task here)
}

uint8_t *amos_bridge;
uint8_t amos_task[52];

namespace amos {

Ctx &ctx() { return context; }

} // namespace amos

namespace amos::host {

[[noreturn]] void missing(const char *what, const void *address)
{
    fprintf(stderr, "amos: %s %p has no C++ on this host\n", what, address);
    exit(70);
}

} // namespace amos::host

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
    a.a[4] = address(c.a4);
    a.a[5] = address(c.a5);
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

namespace amos {

// The glue of each library routine with a C++ function (the Amiga's is
// build/gen/lib_direct_entries.s): none here, the routines are called
// through their entries
extern const uint32_t lib_direct_entry[Lib_Size];
const uint32_t lib_direct_entry[Lib_Size] = {};

} // namespace amos
