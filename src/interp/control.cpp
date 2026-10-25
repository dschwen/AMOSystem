// The flow of the program (+ILib.s): loops, If, Goto / Gosub / On,
// labels, procedures, errors and On Error / Resume / Trap, Every, Break,
// On Menu, and the tests between instructions (Test_Force).
//
// The loops and calls keep their frames on the a3 stack (PLoop its top,
// BasA3 the last Gosub or procedure frame). A loop frame: a word, its
// size; the address of its end (Next, Until...); the address of its start;
// For then has the type, the step, the limit and the variable's address.
// A Gosub frame: "Gosb", the return address, the BasA3 below. A procedure
// frame: "Proc", the return address (0: called by a menu), DProc, AData,
// PData, ErrorOn, ErrorChr, OnErrLine, TabBas, VarLoc, BasA3.
//
// What jumps elsewhere (an error taken by On Error, Every, a menu) goes on
// with a new instruction: interp_continue resets the stack to BasSp and
// returns into the ChrGet loop with the registers of the context.

#include "core/source.h"
#include "interp/interp.h"
#include "hal/chips.h"

using namespace amos;
using namespace amos::interp;

extern "C" {
[[noreturn]] void interp_continue(uint32_t sp, uint32_t ret);
[[noreturn]] void interp_jump(const Regs *r, const void *fn);
[[noreturn]] void interp_menu_return(uint32_t pile);
// control_glue.s: the record of OMnEff for ClearVar
extern uint8_t interp_omn_record[];
// Next with floats (NextF): 1 stay in the loop
uint32_t interp_next_float(uint8_t *var, uint32_t step, uint32_t limit, void *floatbase);
}

namespace {

constexpr uint16_t TForNxt = 24, TRptUnt = 10, TWhlWnd = 10;
constexpr uint32_t Gosb = 0x476F7362, Proc = 0x50726F63;   // "Gosb", "Proc"
constexpr int BitControl = 8, BitMenu = 9, BitJump = 10, BitEvery = 11, BitEcrans = 12, BitBobs = 13,
              BitSprites = 14, BitVBL = 15;
constexpr uint8_t PaSaut = 1 << 0;      // Test_Flags: no jumps from the tests
constexpr int IDiaErrors = 120 - 1;

// The errors of this part
enum : uint32_t {
    ERetGsb = 1, EPopGsb = 2, ENoResume = 3, EResPLab = 4, ENoOnErr = 5, EResLNo = 6, ENoErr = 7, EProErr = 8,
    EStop = 9, EOofStack = 13, ENoFn = 15, ELbNDef = 40, ENoData = 41,
};

IData &data() { return idata(); }
uint8_t *&a3() { return ctx().a3; }

void push_l(uint32_t v)
{
    a3() -= 4;
    poke_l(a3(), v);
}
void push_w(uint16_t v)
{
    a3() -= 2;
    poke_w(a3(), v);
}
uint32_t pop_l()
{
    uint32_t v = peek_l(a3());
    a3() += 4;
    return v;
}
uint16_t pop_w()
{
    uint16_t v = peek_w(a3());
    a3() += 2;
    return v;
}

// OofStack if the loops' stack is full; PLoop the top
void check_stack()
{
    if (address(a3()) < data().MinLoop)
        error(EOofStack);
    data().PLoop = address(a3());
}

bool test_due() { return wdata().Actualise & (1 << BitVBL); }

bool finished(uint16_t t) { return !t || t == tk::DP || t == tk::Then || t == tk::Else; }

// Back into the ChrGet loop at pc, the instruction abandoned
[[noreturn]] void go_on()
{
    IData &d = data();
    interp_continue(d.BasSp, d.Prg_InsRet);
}

} // namespace

namespace amos::interp {

void test_force();

// Test_Normal: the tests, if the VBL asked for them
void test_normal()
{
    if (test_due())
        test_force();
}

// LGoto: the loops left by a jump to pc taken off the stack
void goto_fix()
{
    IData &d = data();
    uint8_t *p = a3();
    while (address(p) < d.BasA3) {
        uint32_t start = peek_l(p + 6), end = peek_l(p + 2);
        if (address(pc()) >= start && address(pc()) <= end)
            break;
        p += int16_t(peek_w(p));
    }
    a3() = p;
    d.PLoop = address(p);
}

// GetLabel: the label at pc (a name, or an expression: a number or a
// string), 0 if there is none. GLb3: from string name.
uint32_t find_label_named(const uint8_t *name)
{
    IData &d = data();
    uint16_t length = peek_w(name);
    if (!length || length >= 32)
        return 0;
    uint8_t *buffer = ptr<uint8_t>(d.BufLabel);
    for (uint16_t i = 0; i < length; i++) {
        uint8_t c = name[2 + i];
        buffer[i] = c >= 'A' && c <= 'Z' ? c + 32 : c;
    }
    if (length & 1)
        buffer[length++] = 0;
    for (const uint8_t *l = ptr<uint8_t>(d.LabBas); *l; l += 8 + int8_t(*l)) {
        int16_t n = int8_t(*l);
        if (n != length)
            continue;
        int16_t i = 0;
        while (i < n && l[8 + i] == buffer[i])
            i++;
        if (i == n)
            return peek_l(l + 4);
    }
    return 0;
}

uint32_t get_label()
{
    IData &d = data();
    uint16_t t = next_token();
    if (t == tk::Pro || t == tk::LGo) {
        int16_t offset = int16_t(next_token());
        pc() += 2 + int8_t(*pc());
        return peek_l(ptr<uint8_t>(d.LabHaut) + offset);
    }
    pc() -= 2;
    Value v = evaluate();
    if (kind(v) >= TString)
        return find_label_named(ptr<uint8_t>(v.v1));
    if (kind(v) == TFloat)
        v = to_integer(v);
    Regs r;
    r.a[0] = d.BufLabel;
    r.d[0] = v.v1;
    lib_call(L_LongToDec, r);
    // the number as a string, in place
    uint8_t *buffer = ptr<uint8_t>(d.BufLabel);
    uint16_t length = uint16_t(r.a[0] - d.BufLabel);
    if (!length)
        return 0;
    uint8_t name[34];
    poke_w(name, length);
    for (uint16_t i = 0; i < length && i < 32; i++)
        name[2 + i] = buffer[i];
    return find_label_named(name);
}

uint32_t label()
{
    uint32_t l = get_label();
    if (!l)
        error(ELbNDef);
    return l;
}

// Gos2: a Gosub frame (back to pc) and pc to the label
void gosub_to(uint32_t to)
{
    IData &d = data();
    push_l(d.BasA3);
    push_l(address(pc()));
    push_l(Gosb);
    check_stack();
    d.BasA3 = address(a3());
    pc() = ptr<uint8_t>(to);
}

// InProE / InPrOn2: a procedure called: proc its Procedure token, back the
// return address (0: stay at pc), errors its ErrorOn. Its parameters are at
// pc, in brackets.
void call_procedure(uint8_t *proc, uint32_t back, uint16_t errors)
{
    IData &d = data();
    uint8_t *p = proc + 6;
    uint8_t *top = ptr<uint8_t>(d.TabBas);
    if (address(top - 6 - 4 - peek_w(p)) < d.HiChaine) {
        Regs m;
        m.d[3] = 0;
        lib_call(L_Menage, m);
    }
    // the procedure's variables, below the others
    top -= 4;
    poke_l(top, d.VarLoc);
    top -= 2;
    poke_w(top, 0xFFFF);
    uint8_t *vars = top - peek_w(p);
    p += 4;
    for (uint8_t *v = vars; v < top; v++)
        *v = 0;
    p += 6 + int8_t(p[4]);
    if (peek_token() == tk::Bra1) {
        do {
            pc() += 2;
            uint8_t *param = p + 4;
            Value x = evaluate();
            p = param;
            int16_t offset = int16_t(peek_w(p));
            p += 2;
            uint8_t *at = offset < 0 ? ptr<uint8_t>(d.VarGlo) + 1 - offset : vars + 2 + offset;
            int8_t length = int8_t(*p++);
            uint8_t type = *p++ & 0x0F;
            p += length;
            at[-2] = reinterpret_cast<uint8_t *>(&d.Long_Var)[type];
            at[-1] = type;
            if (kind(x) != type)
                x = kind(x) ? to_integer(x) : to_float(x);
            poke_l(at, x.v1);
            if (at[-2] == 8)
                poke_l(at + 4, x.v2);
        } while (peek_token() == tk::Vir);
        pc() += 2;
        p += 2;
    }
    if (address(vars - 8) < d.HiChaine) {
        // the variables met the strings: room, and the call again
        Regs r;
        r.d[3] = d.TabBas - address(vars - 16);
        lib_call(L_Demande, r);
        fon_call();
    }
    if (back)
        pc() = ptr<uint8_t>(back);
    push_l(d.BasA3);
    push_l(d.VarLoc);
    push_l(d.TabBas);
    push_l(d.OnErrLine);
    push_l(d.ErrorChr);
    push_w(d.ErrorOn);
    push_l(d.PData);
    push_l(d.AData);
    push_l(d.DProc);
    push_l(address(pc()));
    push_l(Proc);
    check_stack();
    d.BasA3 = address(a3());
    d.VarLoc = address(vars);
    d.TabBas = address(vars);
    d.OnErrLine = 0;
    pc() = p;
    d.DProc = d.PData = address(p + 2);
    d.AData = 0;
    d.ErrorOn = errors;
}

// The procedure frame from a (word by word, to "Proc", as the original's
// protection) taken off: the return address
uint32_t pop_procedure(uint8_t *from, bool check)
{
    IData &d = data();
    a3() = from;
    while (peek_l(a3()) != Proc)
        a3() += 2;
    a3() += 4;
    uint32_t back = pop_l();
    d.DProc = pop_l();
    d.AData = pop_l();
    d.PData = pop_l();
    d.ErrorOn = pop_w();
    d.ErrorChr = pop_l();
    d.OnErrLine = pop_l();
    d.TabBas = pop_l();
    d.VarLoc = pop_l();
    if (check && !back)
        fon_call();
    d.BasA3 = pop_l();
    d.PLoop = address(a3());
    return back;
}

// PopP: out of the procedure (Pop Proc, errors in procedures)
void pop_proc() { pc() = ptr<uint8_t>(pop_procedure(ptr<uint8_t>(data().BasA3), true)); }

// FnEProc: End Proc[value] into Param
void procedure_result()
{
    IData &d = data();
    Value v = evaluate_after();
    if (kind(v) == TInt) {
        d.ParamE = v.v1;
    } else if (kind(v) == TFloat) {
        d.ParamF = v.v1;
        if (int8_t(d.MathFlags) < 0)
            d.ParamF2 = v.v2;
    } else {
        d.ParamC = v.v1;
    }
}

// The tests' jumps: Gosub, Proc or Goto from the instruction being run
// (it runs again on return)
[[noreturn]] void jump_from_test(uint16_t type, uint32_t to)
{
    if (type == tk::Gto) {
        pc() = ptr<uint8_t>(to);
        goto_fix();
    } else {
        pc() = ptr<uint8_t>(ctx().d7 - 2);
        if (type == tk::Gsb)
            gosub_to(to);
        else
            call_procedure(ptr<uint8_t>(to), 0, 0);
    }
    go_on();
}

// GoMenu: On Menu's jump for the item chosen
void go_menu(uint16_t mask, uint16_t actualise)
{
    IData &d = data();
    if ((d.Test_Flags & PaSaut) || d.Direct)
        return;
    uint16_t n = d.MnChoix[0];
    if (!n || n > d.OMnNb)
        return;
    d.ActuMask = mask & ~(1 << BitJump);
    wdata().Actualise = actualise;
    jump_from_test(d.OMnType, ptr<uint32_t>(d.OMnBase)[n - 1]);
}

// OnBreakGo: the On Break procedure
void on_break_go()
{
    IData &d = data();
    if ((d.Test_Flags & PaSaut) || !d.OnBreak)
        return;
    jump_from_test(tk::Prc, d.OnBreak);
}

// EveJump: the Every procedure / Gosub
void every_jump(uint16_t mask)
{
    IData &d = data();
    if (!d.EveLabel || (d.Test_Flags & PaSaut))
        return;
    d.ActuMask = mask;
    jump_from_test(d.EveType == tk::Gsb ? tk::Gsb : tk::Prc, d.EveLabel);
}

// Test_Force: between instructions, when the VBL asks: other AMOS, the
// dialogs, the menus, Control-C, On Menu, the screens and animations,
// the extensions, Every
void test_force()
{
    IData &d = data();
    WData &w = wdata();
    uint16_t mask = d.ActuMask;
    Regs c;
    sy_call(sy::Test_Cyclique, c);
    if (peek_l(ptr<void>(d.Cur_Dialogs))) {
        Regs r;
        lib_call(L_Dia_AutoTest, r);
        if (r.d[0])
            error(r.d[0] + IDiaErrors);
    }
    if ((mask & (1 << BitMenu)) && d.MnBase && !d.MnProc) {
        if (w.ClLast && !d.Direct) {
            Regs k;
            lib_call(L_MenuKeyExplore, k);
        }
        // the right mouse button
        if (!(hal::custom_word(0x016) & (1 << 10))) {
            Regs g;
            lib_call(L_MnGere, g);
            if (g.d[0])
                error(g.d[0]);
        }
    }
    uint16_t actualise = w.Actualise;
    bool update = true;
    if (actualise & (1 << BitControl)) {
        actualise &= ~(1 << BitControl);
        if (d.Mon_Base || (mask & (1 << BitControl))) {
            w.Actualise = actualise;
            error(EStop);
        }
        w.Actualise = actualise;
        on_break_go();
    } else {
        actualise &= mask;
        if (!actualise) {
            update = false;
        } else if (actualise & (1 << BitJump)) {
            actualise &= ~(1 << BitJump);
            go_menu(mask, actualise);
        }
    }
    if (update && uint16_t(w.VblCount - d.VBLOCount) >= d.VBLDelai) {
        d.VBLOCount = uint16_t(w.VblCount);
        if (actualise & (1 << BitBobs)) {
            actualise &= ~(1 << BitBobs);
            Regs r;
            sy_call(sy::EffBob, r);
            sy_call(sy::ActBob, r);
            sy_call(sy::AffBob, r);
            ec_call(ec::SwapScS, r);
        }
        if (actualise & (1 << BitSprites)) {
            actualise &= ~(1 << BitSprites);
            Regs r;
            sy_call(sy::ActHs, r);
            sy_call(sy::AffHs, r);
        }
        // the extensions: bit n of the low byte calls ExtTests[n] (not 0)
        uint8_t bits = uint8_t(actualise) >> 1;
        actualise &= 0xFF00;
        for (int i = 1; bits; i++, bits >>= 1) {
            if ((bits & 1) && d.ExtTests[i]) {
                Regs r;
                r.a[1] = address(&d.ExtTests[i]);
                amos_call(&r, ptr<void>(d.ExtTests[i]));
            }
        }
        if (actualise & (1 << BitEcrans)) {
            actualise &= ~(1 << BitEcrans);
            Regs r;
            ec_call(ec::CopMake, r);
        }
        actualise |= w.Actualise & (1 << BitControl);
    }
    w.Actualise = actualise;
    if ((mask & (1 << BitEvery)) && int16_t(w.EveCpt) <= 0) {
        w.EveCpt = d.EveCharge;
        every_jump(mask & ~(1 << BitEvery));
    }
    w.Actualise &= ~(1 << BitVBL);
}

} // namespace amos::interp

namespace {

using source::skip_token;

// Resume Next: the instruction after the one at from
uint8_t *next_instruction(uint8_t *from)
{
    for (;;) {
        uint16_t t = peek_w(from);
        from = skip_token(t, from + 2);
        if (!t && peek_w(from))
            from += 2;
        if (finished(t))
            return from;
    }
}

// Rpt0: a loop frame; its start past the next token (and the next line's
// header after the end of a line)
void loop_frame(uint16_t size, uint32_t end)
{
    if (!next_token())
        pc() += 2;
    push_l(address(pc()));
    push_l(end);
    push_w(size);
    check_stack();
}

// MMType: v made of type
Value of_type(Value v, uint8_t type)
{
    if (kind(v) == type)
        return v;
    return kind(v) ? to_integer(v) : to_float(v);
}

void exit_loop()
{
    int16_t jump = int16_t(next_token());
    a3() += int16_t(next_token());
    data().PLoop = address(a3());
    pc() += jump;
}

void while_test()
{
    uint8_t *back = pc();
    pc() = ptr<uint8_t>(peek_l(a3() + 6));
    if (integer().v1)
        return;
    pc() = back;
    a3() += TWhlWnd;
    data().PLoop = address(a3());
}

void otherwise()
{
    for (;;) {
        uint16_t jump = next_token();
        pc() += int16_t(jump & ~1);
        if (!(jump & 1) && peek_w(pc() - 4) != tk::Else) {
            goto_fix();
            return;
        }
        pc() -= 2;
    }
}

} // namespace

extern "C" {

void interp_test_force() { test_force(); }
void interp_test_normal() { test_normal(); }
void interp_test_pasaut()
{
    IData &d = data();
    d.Test_Flags |= PaSaut;
    test_normal();
    d.Test_Flags &= ~PaSaut;
}

// For v=a To b [Step c] ... Next [v]
void interp_in_for()
{
    int16_t jump = int16_t(next_token());
    uint32_t next = address(pc()) + jump;
    FoundVar var = find_variable();
    uint8_t type = uint8_t(var.type);
    Value start = of_type(evaluate_after(), type);
    poke_l(var.at, start.v1);
    push_l(address(var.at));
    Value limit = of_type(evaluate_after(), type);
    push_l(limit.v1);
    Value step = {TInt, 1, 0};
    if (peek_token() == tk::Stp)
        step = evaluate_after();
    step = of_type(step, type);
    push_l(step.v1);
    push_w(uint16_t(step.type));
    loop_frame(TForNxt, next);
}

void interp_in_next()
{
    if (test_due())
        test_force();
    uint8_t *f = a3();
    uint8_t *var = ptr<uint8_t>(peek_l(f + 20));
    int32_t step = int32_t(peek_l(f + 12)), limit = int32_t(peek_l(f + 16));
    bool stay;
    if (!f[11]) {
        int32_t v = int32_t(peek_l(var)) + step;
        poke_l(var, uint32_t(v));
        stay = step < 0 ? limit <= v : limit >= v;
    } else {
        // floats: with mathffp, in either precision (as the original)
        stay = interp_next_float(var, uint32_t(step), uint32_t(limit), ptr<void>(data().FloatBase));
    }
    if (stay) {
        pc() = ptr<uint8_t>(peek_l(f + 6));
        return;
    }
    a3() += TForNxt;
    data().PLoop = address(a3());
    if (peek_token() == tk::Var)
        find_variable();
}

void interp_in_do();
void interp_in_repeat()
{
    int16_t jump = int16_t(next_token());
    loop_frame(TRptUnt, address(pc()) + jump);
}

void interp_in_do() { interp_in_repeat(); }

void interp_in_until()
{
    if (test_due())
        test_force();
    if (!integer().v1) {
        pc() = ptr<uint8_t>(peek_l(a3() + 6));
        return;
    }
    a3() += TRptUnt;
    data().PLoop = address(a3());
}

void interp_in_while()
{
    int16_t jump = int16_t(next_token());
    push_l(address(pc()));
    pc() += jump;
    push_l(address(pc()));
    push_w(TWhlWnd);
    check_stack();
    while_test();
}

void interp_in_wend()
{
    if (test_due())
        test_force();
    while_test();
}

void interp_in_loop()
{
    if (test_due())
        test_force();
    pc() = ptr<uint8_t>(peek_l(a3() + 6));
}

void interp_in_exit()
{
    if (test_due())
        test_force();
    exit_loop();
}

void interp_in_exit_if()
{
    uint8_t *back = pc();
    pc() += 4;
    if (integer().v1) {
        pc() = back;
        if (test_due())
            test_force();
        exit_loop();
        return;
    }
    if (peek_token() == tk::Vir)
        pc() += 8;
}

void interp_in_if()
{
    for (;;) {
        uint8_t *back = pc();
        pc() += 2;
        if (integer().v1) {
            if (peek_token() == tk::Then)
                pc() += 2;
            return;
        }
        pc() = back;
        uint16_t jump = next_token();
        pc() += int16_t(jump & ~1);
        if (!(jump & 1)) {
            goto_fix();
            return;
        }
        pc() -= 2;
    }
}

void interp_in_else() { otherwise(); }
void interp_in_else_if()
{
    pc() -= 2;
    otherwise();
}

void interp_in_gosub()
{
    if (test_due())
        test_force();
    gosub_to(label());
}

void interp_in_return()
{
    IData &d = data();
    if (test_due())
        test_force();
    a3() = ptr<uint8_t>(d.BasA3);
    if (pop_l() != Gosb)
        error(ERetGsb);
    pc() = ptr<uint8_t>(pop_l());
    d.BasA3 = pop_l();
    d.PLoop = address(a3());
}

void interp_in_pop()
{
    IData &d = data();
    if (test_due())
        test_force();
    a3() = ptr<uint8_t>(d.BasA3);
    if (pop_l() != Gosb)
        error(EPopGsb);
    a3() += 4;
    d.BasA3 = pop_l();
    d.PLoop = address(a3());
    goto_fix();
}

void interp_in_goto()
{
    if (test_due())
        test_force();
    pc() = ptr<uint8_t>(label());
    goto_fix();
}

// A label as an instruction: a Goto to it (Goto2)
void interp_goto2()
{
    pc() = ptr<uint8_t>(peek_l(ptr<uint8_t>(data().LabHaut) + int16_t(peek_w(pc()))));
    goto_fix();
}

// On n Goto/Gosub/Proc a,b,...
void interp_in_on()
{
    IData &d = data();
    if (test_due())
        test_force();
    int16_t jump = int16_t(next_token());
    uint32_t end = address(pc()) + jump + 2;
    uint16_t count = next_token();
    uint32_t n = integer().v1 - 1;
    uint16_t type = next_token();
    if (int32_t(n) < 0 || (n && n >= count)) {
        pc() = ptr<uint8_t>(end);
        return;
    }
    for (; n; n--) {
        get_label();
        pc() += 2;
    }
    if (type == tk::Gto) {
        interp_in_goto();
        return;
    }
    if (type == tk::Prc) {
        uint32_t proc = get_label();
        call_procedure(ptr<uint8_t>(proc), end, 0);
        return;
    }
    push_l(d.BasA3);
    push_l(end);
    push_l(Gosb);
    if (address(a3()) < d.MinLoop)
        error(EOofStack);
    pc() = ptr<uint8_t>(label());
    d.PLoop = address(a3());
    d.BasA3 = address(a3());
}

// Restore [label]
void interp_in_restore()
{
    IData &d = data();
    if (finished(peek_token())) {
        d.PData = d.DProc;
        d.AData = 0;
        return;
    }
    uint8_t *l = ptr<uint8_t>(label());
    if (peek_w(l) != tk::Data)
        error(ENoData);
    l += 2;
    d.PData = address(l - peek_w(l));
    d.AData = 0;
}

void interp_in_lab() { pc() += 4 + pc()[2]; }
void interp_in_system() { error(1002); }
void interp_in_edit() { error(1000); }
void interp_in_direct() { error(1001); }

void interp_in_break_on()
{
    data().ActuMask |= 1 << BitControl;
    data().OnBreak = 0;
}
void interp_in_break_off() { data().ActuMask &= ~(1 << BitControl); }

void interp_in_on_break()
{
    data().OnBreak = label();
    data().ActuMask &= ~(1 << BitControl);
}

// On Error [Goto label | Proc name]
void interp_in_on_error()
{
    IData &d = data();
    if (d.ErrorOn)
        error(ENoResume);
    d.OnErrLine = 0;
    d.ErrorChr = 0;
    uint16_t t = peek_token();
    if (t == tk::Prc) {
        pc() += 4;
        int16_t offset = int16_t(next_token());
        pc() += 2 + int8_t(*pc());
        d.OnErrLine = peek_l(ptr<uint8_t>(d.LabHaut) + offset);
        d.ErrorChr |= 0x80000000;
        return;
    }
    if (t != tk::Gto)
        return;
    pc() += 2;
    if (peek_token() == tk::Ent && !peek_l(pc() + 2)) {
        pc() += 6;              // On Error Goto 0
        return;
    }
    d.OnErrLine = label();
}

// Resume Label [label]
void interp_in_resume_label()
{
    IData &d = data();
    if (!finished(peek_token())) {
        if (!d.OnErrLine || int32_t(d.ErrorChr) >= 0)
            error(ENoOnErr);
        d.ErrorChr = label() | 0x80000000;
        return;
    }
    test_normal();
    if (!d.ErrorOn)
        error(ENoErr);
    pop_proc();
    d.ErrorOn = 0;
    uint32_t to = d.ErrorChr;
    if (!(to & 0x80000000))
        error(ENoOnErr);
    to &= ~0x80000000;
    if (!to)
        error(EResLNo);
    pc() = ptr<uint8_t>(to);
    goto_fix();
}

// Resume [label]
void interp_in_resume()
{
    IData &d = data();
    test_normal();
    if (!d.ErrorOn)
        error(ENoErr);
    if (int16_t(d.ErrorChr >> 16) < 0) {
        if (!finished(peek_token()))
            error(EResPLab);
        pop_proc();
        d.ErrorOn = 0;
        goto_fix();
        return;
    }
    if (finished(peek_token())) {
        d.ErrorOn = 0;
        pc() = ptr<uint8_t>(d.ErrorChr);
    } else {
        uint32_t to = label();
        d.ErrorOn = 0;
        pc() = ptr<uint8_t>(to);
    }
    goto_fix();
}

void interp_in_resume_next()
{
    IData &d = data();
    test_normal();
    if (!d.ErrorOn)
        error(ENoErr);
    uint8_t *from;
    if (int16_t(d.ErrorChr >> 16) < 0) {
        pop_proc();
        from = pc();
    } else {
        from = ptr<uint8_t>(d.ErrorChr);
    }
    d.ErrorOn = 0;
    pc() = next_instruction(from);
    goto_fix();
}

void interp_in_trap()
{
    data().TrapAdr = address(pc() + 2);
    data().TrapErr = 0;
}

void interp_fn_err_trap(EvalOut *o) { *o = {0, 0, TInt, data().TrapErr, 0, 0}; }

// Every n Gosub/Proc label
void interp_in_every()
{
    IData &d = data();
    d.ActuMask &= ~(1 << BitEvery);
    uint32_t n = integer().v1;
    if (!n || n >= 32767)
        fon_call();
    d.EveCharge = uint16_t(n);
    wdata().EveCpt = uint16_t(n);
    d.EveType = next_token();
    d.EveLabel = label();
    d.ActuMask |= 1 << BitEvery;
}
void interp_in_every_off() { data().ActuMask &= ~(1 << BitEvery); }
void interp_in_every_on() { data().ActuMask |= 1 << BitEvery; }

// The Procedure line in the flow: its body skipped
void interp_in_procedure()
{
    uint32_t size = peek_l(pc());
    pc() += 8 + size;
}

// Proc name[params] / name[params]
void interp_call_proc()
{
    IData &d = data();
    if (test_due())
        test_force();
    int16_t offset = int16_t(next_token());
    pc() += 2 + int8_t(*pc());
    call_procedure(ptr<uint8_t>(peek_l(ptr<uint8_t>(d.LabHaut) + offset)), 0, 0);
}
void interp_in_proc()
{
    pc() += 2;
    interp_call_proc();
}

void interp_in_end_proc()
{
    IData &d = data();
    if (test_due())
        test_force();
    if (peek_token() == tk::Bra1)
        procedure_result();
    if (d.ErrorOn)
        error(EProErr);
    uint32_t back = pop_procedure(a3(), false);
    pc() = ptr<uint8_t>(back);
    if (!back)
        interp_menu_return(d.MnPile);
}

void interp_in_pop_proc()
{
    IData &d = data();
    if (test_due())
        test_force();
    if (d.ErrorOn)
        error(EProErr);
    if (peek_token() == tk::Bra1)
        procedure_result();
    pop_proc();
}

// A procedure in machine code: its parameters' values on the a3 stack, the
// code at pc called with the registers of CallReg (a4 CallReg, a6 the
// code), and out of the procedure
void interp_in_apml()
{
    IData &d = data();
    uint16_t jump = next_token();
    if (jump) {
        const uint8_t *p = pc() + jump;
        do {
            int16_t offset = int16_t(peek_w(p));
            p += 2;
            uint8_t *at = offset < 0 ? ptr<uint8_t>(d.VarGlo) + 1 - offset : ptr<uint8_t>(d.VarLoc) + 2 + offset;
            push_l(peek_l(at));
            uint8_t length = *p++;
            p += 1 + length;
            uint16_t t = peek_w(p);
            p += 4;
            if (t != tk::Vir)
                break;
        } while (true);
    }
    AllRegs r;
    for (int i = 0; i < 8; i++)
        r.d[i] = d.CallReg[i];
    for (int i = 0; i < 3; i++)
        r.a[i] = d.CallReg[8 + i];
    r.a[3] = address(a3());
    r.a[4] = address(d.CallReg);
    r.a[5] = address(ctx().a5);
    r.a[6] = address(pc());
    amos_call_all(&r, pc());
    for (int i = 0; i < 8; i++)
        d.CallReg[i] = r.d[i];
    for (int i = 0; i < 3; i++)
        d.CallReg[8 + i] = r.a[i];
    d.ParamE = r.d[0];
    pop_proc();
}

// On Menu Goto/Gosub/Proc a,b,...
void interp_in_on_menu()
{
    IData &d = data();
    Regs e;
    lib_call(L_OMnEff, e);
    uint16_t type = next_token();
    uint16_t n = 0;
    do {
        push_l(label());
        n++;
    } while (next_token() == tk::Vir);
    pc() -= 2;
    d.OMnNb = n;
    Regs m;
    m.d[0] = n * 4u;
    m.a[0] = w::reserve(m.d[0], w::Fast);
    if (!m.a[0])
        error(24);
    d.OMnBase = m.a[0];
    uint32_t *to = ptr<uint32_t>(m.a[0]) + n;
    for (uint16_t i = 0; i < n; i++)
        *--to = pop_l();
    d.OMnType = type;
    d.ActuMask &= ~(1 << BitJump);
    Regs r;
    r.a[1] = address(interp_omn_record);
    r.a[2] = address(&d.Sys_ClearRoutines);
    w::add_routine(r.a[1], r.a[2]);
}

// MenuProcedure: the procedure named a0 (an AMOS string) called by a menu;
// it runs on this stack, its End Proc comes back through MnPile. d0 = -1
// if there is none.
void interp_menu_procedure(EvalOut *o)
{
    IData &d = data();
    uint32_t proc = find_label_named(ptr<uint8_t>(o->a0));
    if (!proc) {
        *o = {uint32_t(-1), 0, 0, 0, 0, 0};
        return;
    }
    pc() = nullptr;
    call_procedure(ptr<uint8_t>(proc), 0, 0);
    Regs r;
    interp_jump(&r, ptr<void>(d.Prg_InsRet));
}

// Errors: d0 the number, d1 the first one On Error can take (19 for
// AMOS's), d2 -1 or the extension (a0 its messages)
[[noreturn]] void interp_error(uint32_t n, uint32_t first, uint32_t extension, uint32_t messages)
{
    IData &d = data();
    Ctx &c = ctx();
    if (d.ErrorRegs) {
        d.ErrorRegs = 0;
        c.d6 = d.ErrorSave[0];
        c.d7 = d.ErrorSave[1];
    }
    Regs routines;
    routines.a[1] = address(&d.Sys_ErrorRoutines);
    w::call_routines(ptr<uint32_t>(routines.a[1]));
    if (d.Patch_Errors) {
        Regs r;
        r.d[0] = n;
        r.d[1] = first;
        r.d[2] = extension;
        r.a[0] = messages;
        interp_jump(&r, ptr<void>(d.Patch_Errors));
    }
    d.PrintPos = 0;
    d.InputFlg = 0;
    d.ContFlg = 0;
    uint16_t number = uint16_t(n);
    bool trappable = (number == 11 || number >= uint16_t(first)) && number < 1000 && !d.Direct && !d.ErrorOn;
    if (trappable && d.TrapAdr == c.d7) {
        d.TrapAdr = 0;
        d.TrapErr = uint16_t(number | ((extension + 1) << 8));
        a3() = ptr<uint8_t>(d.PLoop);
        pc() = next_instruction(ptr<uint8_t>(c.d7 - 2));
        go_on();
    }
    if (trappable && d.OnErrLine) {
        d.TrapAdr = 0;
        d.TrapErr = 0;
        d.ErrorOn = uint16_t((number + 1) | ((extension + 1) << 8));
        a3() = ptr<uint8_t>(d.PLoop);
        uint32_t at = c.d7 - 2;
        if (int16_t(d.ErrorChr >> 16) >= 0) {
            d.ErrorChr = at;
            pc() = ptr<uint8_t>(d.OnErrLine);
        } else {
            pc() = ptr<uint8_t>(at);
            call_procedure(ptr<uint8_t>(d.OnErrLine), 0, d.ErrorOn);
        }
        go_on();
    }
    // to the editor (or whoever runs the program)
    Regs a;
    a.a[0] = c.d7 - 2;
    lib_call(L_Includes_Adr, a);
    d.VerPos = a.a[0];
    Regs i;
    lib_call(L_Includes_Clear, i);
    uint32_t handler = d.Prg_JError;
    Regs p;
    lib_call(L_Prg_Pull, p);
    Regs r;
    r.d[0] = n;
    r.a[0] = 0;
    if (int16_t(extension) >= 0) {
        const char *m = ptr<char>(messages);
        for (uint16_t k = uint16_t(n); k; k--)
            while (*m++)
                ;
        r.a[0] = address(m);
        r.d[0] = 1;
        r.d[1] = (n & 0xFFFF0000) | 0xFFFF;
    } else {
        r.d[1] = n;
    }
    r.d[2] = extension;
    interp_jump(&r, ptr<void>(handler));
}

void interp_omn_eff()
{
    Regs r;
    lib_call(L_OMnEff, r);
}
}
