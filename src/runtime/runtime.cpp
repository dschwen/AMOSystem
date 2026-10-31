// The machine parts of the interpreter on another host: what the
// assembler did with the stack pointer (the interpreter's stack at BasSp,
// PRUN's, the editor driven by a program, a menu's procedure) is done with
// marks set with setjmp; the stack pointers AMOS keeps (BasSp, MnPile) are
// their addresses. Also the entries of the assembler the interpreter calls
// (chrget_glue.s, control_glue.s, run_glue.s...).

#include "runtime/runtime.h"
#include "runtime/glue.h"
#include "runtime/marks.h"
#include "interp/interp.h"
#include "hal/chips.h"

#include <stdlib.h>

using namespace amos;
using namespace amos::host;

namespace amos::host {


// What runs at a mark: its target (then what follows it), or its C++
void run_at(Mark &m)
{
    AllRegs r = m.regs;
    if (m.regs_fn) {
        m.regs_fn(r);
    } else if (m.fn) {
        to_context(r);
        m.fn();
        r = context_regs();
    } else {
        glue_call(m.target, r);
        if (m.then)
            glue_call(m.then, r);
    }
    m.regs = r;
}

[[noreturn]] void mark_jump(uint32_t mark, const void *target, const AllRegs &r, const void *then)
{
    Mark &m = *ptr<Mark>(mark);
    m.target = target;
    m.then = then;
    m.fn = nullptr;
    m.regs_fn = nullptr;
    m.regs = r;
    longjmp(m.jb, MarkRun);
}

[[noreturn]] void mark_jump_regs(uint32_t mark, void (*fn)(AllRegs &), const AllRegs &r)
{
    Mark &m = *ptr<Mark>(mark);
    m.target = m.then = nullptr;
    m.fn = nullptr;
    m.regs_fn = fn;
    m.regs = r;
    longjmp(m.jb, MarkRun);
}

[[noreturn]] void mark_jump_cpp(uint32_t mark, void (*fn)(), const AllRegs &r)
{
    Mark &m = *ptr<Mark>(mark);
    m.target = m.then = nullptr;
    m.fn = fn;
    m.regs_fn = nullptr;
    m.regs = r;
    longjmp(m.jb, MarkRun);
}

[[noreturn]] void mark_return(uint32_t mark, const AllRegs &r)
{
    Mark &m = *ptr<Mark>(mark);
    m.regs = r;
    longjmp(m.jb, MarkReturn);
}

// The interpreter's base stack (BasSp), for the start: start() runs on it
[[noreturn]] void base_run(void (*start)(uint32_t base))
{
    static Mark base;
    mark_run(base, [&] { start(address(&base)); });
    missing("return to the base stack from", base.target);
}

} // namespace amos::host


extern "C" {
// interp/chrget.cpp, control.cpp
[[noreturn]] void interp_new_chrget();
[[noreturn]] void interp_chrget_continue();
void interp_run_instruction(uint32_t t);
}

namespace {

// New_ChrGet: the program starts on the interpreter's stack, a4 the tokens
void new_chrget()
{
    ctx().a4 = ptr<uint8_t>(idata().AdTokens[0]);
    interp_new_chrget();
}

} // namespace

namespace amos::host {

void host_New_ChrGet(AllRegs &r) { mark_jump_cpp(idata().BasSp, new_chrget, r); }

// Prg_InsRet: back into the ChrGet loop at a6
void host_interp_iret(AllRegs &r)
{
    to_context(r);
    interp_chrget_continue();
}

// PRun_Errors: the errors of a program run by PRUN come back to it
void host_interp_prun_errors(AllRegs &r) { mark_return(idata().BasSp, r); }

} // namespace amos::host

extern "C" {

// An instruction: its routine with d0 (the token), d1 (its entry) and the
// interpreter registers of the context, which get them back
void interp_exec(const void *routine, uint32_t d0, uint32_t d1)
{
    AllRegs r = context_regs();
    r.d[0] = d0;
    r.d[1] = d1;
    glue_call(routine, r);
    to_context(r);
}

// The same with d2-d4 from v (its last parameter)
void interp_exec_value(const void *routine, uint32_t d0, uint32_t d1, const interp::Value *v)
{
    AllRegs r = context_regs();
    r.d[0] = d0;
    r.d[1] = d1;
    r.d[2] = v->type;
    r.d[3] = v->v1;
    r.d[4] = v->v2;
    glue_call(routine, r);
    to_context(r);
}

void interp_step_instruction(uint32_t t) { interp_run_instruction(t); }

// The interpreter goes on at ret, the stack at sp (the instruction given up)
[[noreturn]] void interp_continue(uint32_t sp, uint32_t ret) { mark_jump(sp, ptr<void>(ret), context_regs()); }

// To fn with d0-d5/a0-a2 from r and the interpreter registers of the
// context, on the stack as it is
[[noreturn]] void interp_jump(const Regs *r, const void *fn)
{
    AllRegs a = context_regs();
    for (int i = 0; i < 6; i++)
        a.d[i] = r->d[i];
    for (int i = 0; i < 3; i++)
        a.a[i] = r->a[i];
    glue_call(fn, a);
    missing("return from a jump to", fn);
}

// An instruction started again: the stack at sp, its routine (r->a[0])
// with all of r, then ret
[[noreturn]] void interp_restart(uint32_t sp, uint32_t ret, const AllRegs *r)
{
    mark_jump(sp, ptr<void>(r->a[0]), *r, ptr<void>(ret));
}

// End Proc of a procedure a menu called: back to the call, d0 = 0
[[noreturn]] void interp_menu_return(uint32_t pile)
{
    AllRegs r = context_regs();
    r.d[0] = 0;
    mark_return(pile, r);
}

// PRUN: the program run on top of this one; its error (d0), or -1 if
// Prg_RunIt came back (out of memory)
uint32_t interp_prun_run(uint32_t program, const void *run_it)
{
    IData &d = idata();
    Ctx saved = ctx();
    uint32_t bas_sp = d.BasSp;
    Mark m;
    d.BasSp = address(&m);
    AllRegs r = context_regs();
    r.a[6] = program;
    r.d[0] = uint32_t(-1);
    r.a[1] = address(interp_prun_errors);
    r.a[2] = 0;
    AllRegs out = mark_run(m, [&] {
        glue_call(run_it, r);
        m.regs = r;
        m.regs.d[0] = uint32_t(-1);
    });
    idata().BasSp = bas_sp;
    ctx() = saved;
    return out.d[0];
}

// The editor driven by a program (Ed_ZapIn): it comes back through
// Ed_ZapOut to this mark; d0 and a0 its answer
void interp_zap_in(Regs *r, const void *zap_in)
{
    IData &d = idata();
    Ctx saved = ctx();
    uint32_t bas_sp = d.BasSp;
    Mark m;
    d.BasSp = address(&m);
    AllRegs a = context_regs();
    a.d[0] = r->d[0];
    a.d[1] = r->d[1];
    AllRegs out = mark_run(m, [&] {
        glue_call(zap_in, a);
        m.regs = a;
    });
    idata().BasSp = bas_sp;
    ctx() = saved;
    r->d[0] = out.d[0];
    r->a[0] = out.a[0];
}

// MnOCall: L_MenuProcedure (fn) for the procedure name with the
// interpreter registers regs; End Proc comes back through *pile
uint32_t mn_call_procedure(const void *fn, const Ctx *regs, be_u32 *pile, const uint8_t *name)
{
    Mark m;
    *pile = address(&m);
    AllRegs r = {};
    r.d[6] = regs->d6;
    r.d[7] = regs->d7;
    r.a[3] = address(regs->a3);
    r.a[4] = address(regs->a4);
    r.a[5] = address(regs->a5);
    r.a[6] = address(regs->a6);
    r.a[0] = address(name);
    AllRegs out = mark_run(m, [&] {
        glue_call(fn, r);
        m.regs = r;
    });
    return out.d[0];
}

// AMOS.library's routines of the VBL (VblRout...), a5 the data zone
void w_int_call(const void *fn, uint8_t *data)
{
    // a C++ function (the extensions'), or an entry of the assembler's
    if (!glue_entry(fn)) {
        reinterpret_cast<void (*)()>(const_cast<void *>(fn))();
        return;
    }
    AllRegs r = {};
    r.a[5] = address(data);
    r.a[6] = hal::CustomBase;
    glue_call(fn, r);
}

} // extern "C"

// The editor driven by a program (+Edit.s Ed_ZapIn, Ed_ZapOut): it comes
// back to interp_zap_in's mark (BasSp) with d0/a0 its answer


namespace amos::host {

namespace {

// .Go: the function on the editor's stack, then its loop
void zap_go(AllRegs &r)
{
    glue_call(ed_zap_prepare_entry, r);
    glue_call(ed_function_call_entry, r);
    glue_call(ed_loop, r);
}

[[noreturn]] void zap_back(AllRegs &r)
{
    glue_call(ed_zap_result_entry, r);
    mark_return(idata().BasSp, r);
}

} // namespace

void host_Ed_ZapIn(AllRegs &r)
{
    glue_call(ed_zap_in_entry, r);
    uint16_t w = uint16_t(r.d[0] - 1);
    r.d[0] = (r.d[0] & 0xFFFF0000) | w;
    if (int16_t(w) < 0)
        mark_jump_regs(idata().BasSp, zap_go, r);
    if (!w)
        glue_call(ed_zap_out_entry, r);
    zap_back(r);
}

void host_ed_zap_out(AllRegs &r)
{
    glue_call(ed_zap_out_entry, r);
    zap_back(r);
}

} // namespace amos::host
