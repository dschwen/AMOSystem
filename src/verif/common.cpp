// What the C++ parts of the verifier share (see verif/verif.h).

#include "verif/verif.h"

using namespace amos;

// +Verif.s
extern "C" uint8_t ver_error[];

namespace amos::verif {

uint8_t *pc;

void fail(uint16_t n) { amos_goto(ver_error, n); }

bool finished(uint16_t token) { return !token || token == tk::DP || token == tk::Then || token == tk::Else; }

void not_13()
{
    IData &d = idata();
    d.VerNot1_3 = 1;
    if (d.VerCheck1_3)
        fail(47);
}

void call(const void *routine, AllRegs &r)
{
    Ctx &c = ctx();
    r.a[3] = address(c.a3);
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = address(pc);
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    amos_call_all(&r, routine);
    // a3/a4: the relocation's place and pointer (New_Reloc), d6/d7
    pc = ptr<uint8_t>(r.a[6]);
    c.a3 = ptr<uint8_t>(r.a[3]);
    c.a4 = ptr<uint8_t>(r.a[4]);
    c.d6 = r.d[6];
    c.d7 = r.d[7];
}

void enter(AllRegs *r) { pc = ptr<uint8_t>(r->a[6]); }

void leave(AllRegs *r)
{
    Ctx &c = ctx();
    r->a[6] = address(pc);
    r->a[3] = address(c.a3);
    r->a[4] = address(c.a4);
    r->d[6] = c.d6;
    r->d[7] = c.d7;
}

} // namespace amos::verif
