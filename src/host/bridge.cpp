// The bridge between the C++ and the assembler (core/call.s) on another
// host: there is no assembler, so a "call into the assembler" is a call of
// the C++ function behind the glue entry the address names (the WEntry
// table, build/gen/wentry_table.cpp). Anything else has no C++ yet: the
// host stops and says where.

#include "host/host.h"

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

extern "C" {

void amos_call(Regs *r, const void *fn)
{
    if (WFunction f = wentry_function(fn))
        return w_direct_call(f, *r);
    host::missing("routine", fn);
}

void amos_call_all(AllRegs *r, const void *fn)
{
    if (WFunction f = wentry_function(fn)) {
        Ctx saved = ctx();
        Ctx &c = ctx();
        c.d6 = r->d[6];
        c.d7 = r->d[7];
        c.a3 = ptr<uint8_t>(r->a[3]);
        c.a4 = ptr<uint8_t>(r->a[4]);
        c.a5 = ptr<uint8_t>(r->a[5]);
        c.a6 = ptr<uint8_t>(r->a[6]);
        f(r);
        ctx() = saved;
        return;
    }
    host::missing("routine", fn);
}

// The routine reads the program: a3 and a6 stay as it leaves them
void amos_eval(Regs *r, const void *fn)
{
    if (WFunction f = wentry_function(fn)) {
        AllRegs a = {};
        Ctx &c = ctx();
        for (int i = 0; i < 8; i++)
            a.d[i] = r->d[i];
        for (int i = 0; i < 3; i++)
            a.a[i] = r->a[i];
        a.a[3] = address(c.a3);
        a.a[4] = address(c.a4);
        a.a[5] = address(c.a5);
        a.a[6] = address(c.a6);
        f(&a);
        for (int i = 0; i < 8; i++)
            r->d[i] = a.d[i];
        for (int i = 0; i < 3; i++)
            r->a[i] = a.a[i];
        c.a3 = ptr<uint8_t>(a.a[3]);
        c.a6 = ptr<uint8_t>(a.a[6]);
        return;
    }
    host::missing("evaluating routine", fn);
}

int32_t amos_call_cc(Regs *, const void *fn) { host::missing("routine with condition codes", fn); }

[[noreturn]] void amos_goto(const void *fn, uint32_t) { host::missing("jump to", fn); }

} // extern "C"

namespace amos {

// The glue of each library routine with a C++ function (the Amiga's is
// build/gen/lib_direct_entries.s): filled by the host's library start
extern const uint32_t lib_direct_entry[Lib_Size];
const uint32_t lib_direct_entry[Lib_Size] = {};

} // namespace amos
