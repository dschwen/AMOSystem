// The verifier's loops, tests and jumps (+Verif.s V1_For ... V1_Every and
// their pass 2 routines V2_For ... V2_OnGoto): in pass 1 each gets its
// entry in TablA (verif/tables.cpp), with the routine pass 2 calls for it
// (the address of its assembler entry, see loops_glue.s): there the
// distances are poked (from a For to its Next...). The closing
// instructions get an error routine instead, which their opening one
// clears when it finds them.
//
// An entry's variable part (Vta_Variable, at 24): For: the variable's
// offset; Next: the end of the instruction, the variable's offset; If and
// Else: the address after them and the start of their line; Until, Wend,
// Loop, Else If, End If: the address after them; Exit: the number of
// loops.

#include "verif/verif.h"

using namespace amos;
using namespace amos::verif;

// loops_glue.s: the entries of the routines of pass 2; +Verif.s



namespace {

// Vta_Flag
constexpr uint8_t Loop = 1 << 0, Test = 1 << 1, Exit = 1 << 3, Jump = 1 << 4;
// the sizes of the loops on the interpreter's stack
constexpr uint16_t TForNxt = 24, TRptUnt = 10, TWhlWnd = 10, TDoLoop = 10;

IData &data() { return idata(); }

[[noreturn]] void syntax() { fail(ESyntax); }

void want(uint16_t token)
{
    if (next_word() != token)
        syntax();
}

void integer()
{
    if (expression() != '0')
        fail(EType);
}

uint8_t *var_part(Entry *e) { return reinterpret_cast<uint8_t *>(e) + 24; }

void loop_in(uint16_t size)
{
    IData &d = data();
    d.Ver_NBoucles++;
    d.Ver_PBoucles += size;
}

void loop_out(uint16_t size)
{
    IData &d = data();
    d.Ver_NBoucles--;
    d.Ver_PBoucles -= size;
}

// The word for the distance, left free for pass 2
void room()
{
    poke_w(pc, 0);
    pc += 2;
}

// The end of a loop: where its instruction ends
int32_t loop_end(uint32_t routine, uint16_t token, uint16_t size)
{
    loop_out(size);
    Entry *e = table_add(routine, token, 4, Loop);
    poke_l(var_part(e), instruction_end());
    return AfterInstruction;
}

} // namespace

// Pass 1

namespace {

int32_t for_(uint16_t token)
{
    loop_in(TForNxt);
    Entry *e = table_add(address(AMOS_DATA(ver_for_two)), token, 2, Loop);
    room();
    uint8_t *at = pc;
    uint8_t t = variable_alone();
    poke_w(var_part(e), peek_w(at + 2));
    if (t != '0')
        fail(EType);
    want(tk::Eg);
    if (expression() != t)
        fail(EType);
    data().VerPos = address(pc);
    want(tk::To);
    if (expression() != t)
        fail(EType);
    if (word(pc) == tk::Stp) {
        pc += 2;
        if (expression() != t)
            fail(EType);
    }
    return AfterInstruction;
}

int32_t next(uint16_t token)
{
    loop_out(TForNxt);
    Entry *e = table_add(address(AMOS_DATA(ver_next_alone)), token, 6, Loop);
    if (!finished(word(pc))) {
        uint8_t *at = pc;
        variable_alone();
        poke_w(var_part(e) + 4, peek_w(at + 2));
        e->Vta_UFlag++;
    }
    poke_l(var_part(e), instruction_end());
    return AfterInstruction;
}

int32_t repeat(uint16_t token)
{
    loop_in(TRptUnt);
    table_add(address(AMOS_DATA(ver_repeat_two)), token, 0, Loop);
    room();
    return AfterInstruction;
}

int32_t until(uint16_t token)
{
    loop_out(TRptUnt);
    Entry *e = table_add(address(AMOS_DATA(ver_until_alone)), token, 4, Loop);
    expression();
    poke_l(var_part(e), instruction_end());
    return AfterInstruction;
}

int32_t while_(uint16_t token)
{
    loop_in(TWhlWnd);
    table_add(address(AMOS_DATA(ver_while_two)), token, 0, Loop);
    room();
    expression();
    return AfterInstruction;
}

int32_t wend(uint16_t token) { return loop_end(address(AMOS_DATA(ver_wend_alone)), token, TWhlWnd); }

int32_t do_(uint16_t token)
{
    loop_in(TDoLoop);
    table_add(address(AMOS_DATA(ver_do_two)), token, 0, Loop);
    room();
    return AfterInstruction;
}

int32_t loop(uint16_t token) { return loop_end(address(AMOS_DATA(ver_loop_alone)), token, TDoLoop); }

// Exit [n]
int32_t exit(uint16_t token)
{
    Entry *e = table_add(address(AMOS_DATA(ver_exit_two)), token, 4, Exit);
    poke_l(pc, 0);
    pc += 4;
    poke_l(var_part(e), 1);
    if (word(pc) != tk::Ent)
        return AfterInstruction;
    poke_l(var_part(e), peek_l(pc + 2));
    pc += 6;
    return AfterInstruction;
}

// Exit If test[,n]
int32_t exit_if(uint16_t token)
{
    Entry *e = table_add(address(AMOS_DATA(ver_exit_two)), token, 4, Exit);
    poke_l(pc, 0);
    pc += 4;
    integer();
    poke_l(var_part(e), 1);
    if (word(pc) != tk::Vir)
        return AfterInstruction;
    if (word(pc + 2) != tk::Ent)
        syntax();
    poke_l(var_part(e), peek_l(pc + 4));
    pc += 8;
    return AfterInstruction;
}

// If test [Then ...]
int32_t if_(uint16_t token)
{
    Entry *e = table_add(address(AMOS_DATA(ver_if_two)), token, 8, Test);
    room();
    poke_l(var_part(e), address(pc));
    poke_l(var_part(e) + 4, data().VDLigne);
    integer();
    if (word(pc) != tk::Then)
        return AfterInstruction;
    pc += 2;
    poke_l(var_part(e), address(pc));
    e->Vta_UFlag = 1;
    e->Vta_Jump = address(AMOS_DATA(ver_if_then_two));
    if (word(pc) != tk::LGo)
        return AtInstruction;
    label_skip();
    return AfterInstruction;
}

int32_t else_if(uint16_t token)
{
    Entry *e = table_add(address(AMOS_DATA(ver_else_alone)), token, 4, Test);
    room();
    poke_l(var_part(e), address(pc));
    integer();
    return AfterInstruction;
}

int32_t else_(uint16_t token)
{
    Entry *e = table_add(address(AMOS_DATA(ver_else_alone)), token, 8, Test);
    room();
    poke_l(var_part(e), address(pc));
    poke_l(var_part(e) + 4, data().VDLigne);
    if (word(pc) == tk::DP)
        return AfterInstruction;
    if (word(pc) != tk::LGo)
        return AtInstruction;
    label_skip();
    return AfterInstruction;
}

int32_t end_if(uint16_t token)
{
    Entry *e = table_add(address(AMOS_DATA(ver_end_if_alone)), token, 4, Test);
    poke_l(var_part(e), address(pc));
    return AfterInstruction;
}

int32_t gosub(uint16_t)
{
    label_or_expression();
    return AfterInstruction;
}

// Goto (and On Error Goto, Resume label)
int32_t goto_(uint16_t token)
{
    table_add(address(AMOS_DATA(ver_goto_two)), token, 0, Jump);
    label_or_expression();
    return AfterInstruction;
}

// On n Goto / Gosub / Proc: the number of branches and the length of the
// instruction poked
int32_t on(uint16_t)
{
    uint8_t *at = pc;
    poke_l(pc, 0);
    pc += 4;
    integer();
    uint16_t t = next_word();
    uint16_t n = 0;
    if (t == tk::Prc) {
        do {
            n++;
            procedure_no_parameter();
        } while (next_word() == tk::Vir);
    } else if (t == tk::Gto || t == tk::Gsb) {
        if (t == tk::Gto)
            table_add(address(AMOS_DATA(ver_on_goto_two)), tk::On, 0, Jump);
        do {
            n++;
            label_or_expression();
        } while (next_word() == tk::Vir);
    } else {
        syntax();
    }
    pc -= 2;
    poke_w(at + 2, n);
    poke_w(at, uint16_t(address(pc) - address(at) - 4));
    return AfterInstruction;
}

int32_t on_error(uint16_t token)
{
    if (word(pc) == tk::Prc)
        return AtInstruction;
    if (word(pc) != tk::Gto)
        return AfterInstruction;
    pc += 2;
    return goto_(token);
}

int32_t resume(uint16_t token)
{
    if (finished(word(pc)))
        return AfterInstruction;
    return goto_(token);
}

int32_t resume_label(uint16_t)
{
    if (!finished(word(pc)))
        label_or_expression();
    return AfterInstruction;
}

// Trap: an instruction after it
int32_t trap(uint16_t)
{
    not_13();
    if (!finished(word(pc)))
        return AtInstruction;
    fail(43);
}

int32_t pop_proc(uint16_t)
{
    if (!data().Phase)
        fail(EProcedureNotOpen);
    if (word(pc) != tk::Bra1)
        return AfterInstruction;
    pc += 2;
    expression();
    if (next_word() == tk::Bra2)
        return AfterInstruction;
    syntax();
}

// Every n Gosub label / Proc procedure
int32_t every(uint16_t)
{
    integer();
    uint16_t t = next_word();
    if (t == tk::Gsb)
        label_or_expression();
    else if (t == tk::Prc)
        procedure_no_parameter();
    else
        syntax();
    return AfterInstruction;
}

// (a kind beyond the assembler's table)
int32_t unknown(uint16_t) { syntax(); }

} // namespace

// Pass 2

namespace {

// V2_IfDoke: the distance from the test (or Else) of entry from to to
// poked at its start, bit 0 set for an Else If (the next test to do)
void test_distance(Entry *from, uint32_t to, bool else_if)
{
    pc = ptr<uint8_t>(from->Vta_Prog);
    goto_loops(from, to);
    uint32_t d = to - address(pc) - 2;
    if (d >= 0x10000)
        fail(ETooLong);
    if (else_if)
        d |= 1;
    poke_w(pc, uint16_t(d));
    pc += 2;
}

// The end of the line an entry of its line is in
uint32_t end_of_line(Entry *e)
{
    const uint8_t *line = ptr<uint8_t>(peek_l(var_part(e) + 4));
    return address(line + 2 * line[0] - 2);
}

// V2_IfThenLabel: then the label after Then (or Else) checked
void test_label(Entry *e, uint32_t to)
{
    test_distance(e, to, false);
    pc = ptr<uint8_t>(peek_l(var_part(e)));
    if (next_word() != tk::LGo)
        return;
    ContextCursor l;
    uint32_t a = label_use(data().Phase, &l);
    if (!a)
        fail(ELabelUndefined);
    goto_loops(e, a);
}

// V2_For / V2_Repeat / V2_While / V2_Do: its end found (and cleared of
// its error), the distance to it poked
void loop_two(Entry *e, uint16_t end, uint16_t error)
{
    Entry *f = e;
    Entry *n = table_find(f, Loop, uint16_t(e->Vta_NBoucles - 1), int16_t(end));
    if (!n)
        fail(error);
    if (end == tk::Nxt && n->Vta_UFlag && peek_w(var_part(n) + 4) != peek_w(var_part(e)))
        fail(error);
    n->Vta_Jump = 0;
    poke_distance(peek_l(var_part(n)));
}

// V2_Exit: the distance to the end of the loop left and the place to
// take off the interpreter's stack
void exit_two(Entry *e)
{
    uint8_t *at = pc;
    uint16_t loops = uint16_t(peek_l(var_part(e)));
    int16_t level = int16_t(e->Vta_NBoucles - loops);
    if (level < 0)
        fail(ENotEnoughLoops);
    Entry *f = e;
    Entry *l = table_find(f, Loop, uint16_t(level), -1);
    if (!l)
        fail(ENotEnoughLoops);
    uint32_t d = peek_l(var_part(l)) - address(at) - 4;
    if (d >= 0x10000)
        syntax();
    poke_w(at, uint16_t(d));
    poke_w(at + 2, uint16_t(e->Vta_PBoucles - l->Vta_PBoucles));
}

// V2_If / V2_ElsI: the next Else, Else If or End If of the structure
void if_two(Entry *e, uint16_t error)
{
    Entry *f = e;
    uint16_t t;
    Entry *n = table_find_test(f, tk::Else, tk::ElsI, tk::EndI, &t);
    if (!n)
        fail(error);
    pc = ptr<uint8_t>(peek_l(var_part(n)));
    uint32_t to;
    bool else_if = false;
    if (t == tk::Else) {
        n->Vta_Jump = address(AMOS_DATA(ver_else_two));
        to = address(pc);
    } else if (t == tk::ElsI) {
        n->Vta_Jump = address(AMOS_DATA(ver_else_if_two));
        else_if = true;
        to = address(pc);
    } else {
        to = instruction_end();
        n->Vta_Jump = 0;
    }
    test_distance(e, to, else_if);
}

// V2_IfThen: If ... Then on one line: to its Else on the line, or its end
void if_then_two(Entry *e)
{
    uint32_t line = peek_l(var_part(e) + 4);
    uint16_t depth = 1;
    Entry *a = e;
    while (a->Vta_Next) {
        a = ptr<Entry>(a->Vta_Next);
        if (a->Vta_Flag != Test)
            continue;
        if (peek_l(var_part(a) + 4) != line)
            break;
        if (a->Vta_Token == tk::Else) {
            if (--depth)
                continue;
            a->Vta_Jump = address(AMOS_DATA(ver_else_then_two));
            test_label(e, peek_l(var_part(a)));
            return;
        }
        if (a->Vta_Token == tk::If)
            depth++;
    }
    test_label(e, end_of_line(e));
}

// V2_Else: to its End If
void else_two(Entry *e)
{
    Entry *f = e;
    uint16_t t;
    Entry *n = table_find_test(f, 0xFFFF, 0xFFFF, tk::EndI, &t);
    if (!n)
        fail(EEndIf);
    n->Vta_Jump = 0;
    pc = ptr<uint8_t>(peek_l(var_part(n)));
    test_distance(e, instruction_end(), false);
}

// V2_Goto: a jump into a loop refused
void goto_two(Entry *e)
{
    if (uint32_t a = label_address())
        goto_loops(e, a);
}

} // namespace

namespace amos::verif {

Routine loop_routine(int8_t kind)
{
    switch (uint8_t(kind)) {
    case 0x25: return trap;
    case 0x30: return for_;
    case 0x31: return next;
    case 0x32: return repeat;
    case 0x33: return until;
    case 0x34: return while_;
    case 0x35: return wend;
    case 0x36: return do_;
    case 0x37: return loop;
    case 0x38: return exit;
    case 0x39: return exit_if;
    case 0x3A: return if_;
    case 0x3B: return else_;
    case 0x3C: return else_if;
    case 0x3D: return end_if;
    case 0x3E: return goto_;
    case 0x3F: return gosub;
    case 0x40: return on_error;
    case 0x43: return on;
    case 0x44: return resume;
    case 0x45: return resume_label;
    case 0x46: return pop_proc;
    case 0x47: return every;
    default: return unknown;
    }
}

} // namespace amos::verif

// The routines of pass 2 (a0 the entry, a6 its place) and the errors of
// the closing instructions left alone
extern "C" {
static Entry *two(AllRegs *r)
{
    enter(r);
    return ptr<Entry>(r->a[0]);
}
#define TWO(name, body)                                                                                            \
    void name(AllRegs *r)                                                                                          \
    {                                                                                                              \
        Entry *e = two(r);                                                                                         \
        body;                                                                                                      \
        leave(r);                                                                                                  \
    }
TWO(ver_for_two_c, loop_two(e, tk::Nxt, EForNext))
TWO(ver_repeat_two_c, loop_two(e, tk::Unt, ERepeatUntil))
TWO(ver_while_two_c, loop_two(e, tk::Wnd, EWhileWend))
TWO(ver_do_two_c, loop_two(e, tk::Loo, EDoLoop))
TWO(ver_exit_two_c, exit_two(e))
TWO(ver_if_two_c, if_two(e, EIfElse))
TWO(ver_else_if_two_c, if_two(e, EElseIf))
TWO(ver_if_then_two_c, if_then_two(e))
TWO(ver_else_two_c, else_two(e))
TWO(ver_else_then_two_c, test_label(e, end_of_line(e)))
TWO(ver_goto_two_c, goto_two(e))
TWO(ver_on_goto_two_c, do goto_two(e); while (next_word() == tk::Vir))
#undef TWO

void ver_next_alone_c(AllRegs *) { fail(ENextFor); }
void ver_until_alone_c(AllRegs *) { fail(EUntilRepeat); }
void ver_wend_alone_c(AllRegs *) { fail(EWendWhile); }
void ver_loop_alone_c(AllRegs *) { fail(ELoopDo); }
void ver_else_alone_c(AllRegs *) { fail(EElseIf); }
void ver_end_if_alone_c(AllRegs *) { fail(EEndIf); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/verif_loops.inc"
#endif
