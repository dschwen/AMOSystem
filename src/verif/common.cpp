// What the C++ parts of the verifier share (see verif/verif.h).

#include "verif/verif.h"

using namespace amos;

extern "C" {
// the glue of the verifier's parts (verif/*_glue.s)
extern uint8_t ver_tables_run_entry[], ver_free_tables_entry[], ver_prg_pull_entry[];
uint32_t lib_d_close(RegFrame &);
[[noreturn]] void interp_jump(const Regs *r, const void *fn);
}

namespace amos::verif {

uint8_t *pc;

// VerErr: the tokens back, the includes, the equates' file and the tables
// freed, the program pulled; then its caller's error handler (Prg_JError)
// with d0 = -n (a message of the test), a0 = 0
void fail(uint16_t n)
{
    IData &d = idata();
    Regs run;
    entry_call(ver_tables_run_entry, run);
    d.VerPos = includes_address(d.VerPos);
    includes_clear();
    RegFrame close = {};
    lib_d_close(close);
    Regs tables;
    entry_call(ver_free_tables_entry, tables);
    d.VerCheck1_3 = 0;
    uint32_t handler = d.Prg_JError;
    Regs pull;
    entry_call(ver_prg_pull_entry, pull);
    Regs e;
    e.d[0] = uint32_t(-int32_t(n));
    e.a[0] = 0;
    e.a[2] = handler;
    interp_jump(&e, ptr<void>(handler));
}

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
