// The verifier's routines of the instructions (+Verif.s VerRem, VerVar,
// VerDim, VerPr...), called by verif/main.cpp from the instruction's kind
// (the first byte of its entry in the token table). Each returns what
// comes next: the next line, the next instruction, or the end of this one
// (the routines still in the assembler, through its tables).

#include "verif/verif.h"

using namespace amos;
using namespace amos::verif;

namespace {

constexpr uint16_t MnNDim = 8;

IData &data() { return idata(); }

[[noreturn]] void syntax() { fail(ESyntax); }

void want(uint16_t token)
{
    if (next_word() != token)
        syntax();
}

// The program has instructions (VarBufFlg bit 0 of its first byte)
void used() { data().VarBufFlg |= 1 << 8; }

void no_direct()
{
    if (data().Direct)
        fail(EIllegalDirect);
}

uint8_t integer()
{
    uint8_t t = expression();
    if (t != '0')
        fail(EType);
    return t;
}

uint8_t string()
{
    uint8_t t = expression();
    if (t != '2')
        fail(EType);
    return t;
}

// VerGV: a variable alone (an array with its indices); its type
uint8_t variable()
{
    data().VerPos = address(pc);
    want(tk::Var);
    pc[3] &= 0x0F;
    Name n = name();
    uint8_t *e;
    if (word(n.after) == tk::Par1) {
        pc[3] |= 0x40;
        if (store(&e))
            fail(ENotDimensioned);          // an array is dimensioned first
        dimensions();
    } else {
        store(&e);
    }
    return n.type;
}

// VerVarA: a string variable
void string_variable()
{
    if (variable() != '2')
        fail(EType);
}

// VerVEnt: the variable at pc an integer one (pc unchanged)
void integer_variable()
{
    uint8_t *at = pc;
    want(tk::Var);
    if (name().flags & 7)
        fail(EType);
    pc = at;
}

// V1_CallProc: the call of a procedure (pc on its name, after its
// token): its parameters noted in the relocation for pass 2, which checks
// them against the procedure's (none allowed if bare)
void call_procedure(bool bare)
{
    IData &d = data();
    no_direct();
    reloc(0x88);
    Name n = name();
    poke_w(pc - 2, tk::Pro);
    pc[3] |= 0x80;
    pc = const_cast<uint8_t *>(n.after);
    d.VerPos = address(pc);
    if (word(pc) != tk::Bra1) {
        reloc(0x8E);
        return;
    }
    if (bare)
        syntax();
    do {
        pc += 2;
        uint8_t t = expression();
        reloc(0x8A);
        reloc_byte(t);
    } while (word(pc) == tk::Vir);
    d.VerPos = address(pc);
    want(tk::Bra2);
    reloc(0x8C);
}

} // namespace

// V1_GoProcedureNoParam: a procedure's name (as a procedure or a variable)
// called
void amos::verif::procedure_no_parameter()
{
    uint16_t t = next_word();
    if (t != tk::Pro && t != tk::Var)
        fail(ESyntax);
    call_procedure(false);
}

namespace {

// V1_IVariable: a variable as an instruction: an assignment, or the call
// of a procedure
void variable_instruction()
{
    poke_w(pc - 2, tk::Var);
    pc[3] &= 0x0F;
    Name n = name();
    uint8_t *e;
    uint16_t after = word(n.after);
    if (after == tk::Par1) {
        pc[3] |= 0x40;
        if (store(&e))
            fail(ENotDimensioned);
        if (e[4] != uint8_t(dimensions()))
            fail(EIllegalParameters);
    } else if (after == tk::Eg) {
        store(&e);
    } else {
        call_procedure(false);
        return;
    }
    want(tk::Eg);
    data().VerPos = address(pc);
    if (expression() != n.type)
        fail(EType);
}

// The routines

int32_t rem(uint16_t)
{
    no_direct();
    uint16_t n = next_word();
    pc += n + 2;
    return AtLine;
}

int32_t variable_line(uint16_t)
{
    used();
    variable_instruction();
    return AfterInstruction;
}

int32_t procedure_line(uint16_t t)
{
    no_direct();
    return variable_line(t);
}

int32_t data_line(uint16_t);

int32_t label(uint16_t)
{
    no_direct();
    used();
    label_store();
    if (word(pc) != tk::Data)
        return AtInstruction;
    pc += 2;
    return data_line(0);
}

// Set Double Precision, Set Stack: before any other instruction
[[noreturn]] void not_first() { fail(50); }

int32_t double_precision(uint16_t)
{
    IData &d = data();
    not_13();
    if (d.VarBufFlg & (1 << 8))
        not_first();
    if (d.VarBufFlg & (1 << 9))
        not_first();
    d.VarBufFlg |= 1 << 9;
    d.MathFlags = 0x83;
    return AfterInstruction;
}

int32_t set_stack(uint16_t)
{
    IData &d = data();
    no_direct();
    if (d.VarBufFlg & (1 << 8))
        not_first();
    if (d.VarBufFlg & (1 << 11))
        not_first();
    d.VarBufFlg |= 1 << 11;
    want(tk::Ent);
    uint32_t n = peek_l(pc);
    pc += 4;
    if (uint16_t(n) < 10)
        n = 10;
    d.Stack_Size = uint16_t(n + 1);
    return AfterInstruction;
}

// Set Buffer: the variables' buffer reserved again, all verified again
int32_t set_buffer(uint16_t)
{
    IData &d = data();
    no_direct();
    if (d.VarBufFlg & (1 << 8))
        fail(ENoBuffer);
    want(tk::Ent);
    uint32_t size = uint32_t(uint16_t(peek_l(pc))) * 1024;
    pc += 4;
    if (size == d.VarBufL)
        return AfterInstruction;
    if (d.VarBufFlg & (1 << 10))
        fail(ENoBuffer);
    d.VarBufFlg |= 1 << 10;
    resize_variables(size);
    return -1;
}

int32_t set_accessory(uint16_t)
{
    not_13();
    data().Prg_Accessory++;
    return AfterInstruction;
}

int32_t dim(uint16_t)
{
    for (;;) {
        used();
        want(tk::Var);
        pc[3] &= 0x0F;
        Name n = name();
        if (word(n.after) != tk::Par1)
            syntax();
        pc[3] |= 0x40;
        uint8_t *e;
        if (!store(&e))
            fail(EAlreadyDimensioned);
        e[4] = uint8_t(dimensions());
        if (next_word() != tk::Vir) {
            pc -= 2;
            return AfterInstruction;
        }
    }
}

int32_t sort(uint16_t)
{
    used();
    const uint8_t *at = pc;
    variable();
    if (at[5] & (1 << 6))
        return AfterInstruction;
    syntax();
}

int32_t swap(uint16_t)
{
    used();
    uint8_t t = variable();
    want(tk::Vir);
    if (variable() != t)
        fail(EType);
    return AfterInstruction;
}

// Def Fn: alone on its line
int32_t def_fn(uint16_t)
{
    used();
    want(tk::Var);
    pc[3] = uint8_t((pc[3] & 0x0F) | (1 << 3));
    uint8_t t = name().type;
    uint8_t *e;
    store(&e);
    if (word(pc) == tk::Par1) {
        pc += 2;
        do
            variable();
        while (next_word() == tk::Vir);
        if (word(pc - 2) != tk::Par2)
            syntax();
    }
    want(tk::Eg);
    if (expression() != t)
        fail(EType);
    if (word(pc))
        fail(EDataStart);
    return AfterInstruction;
}

// Print / LPrint [#channel,] [Using a$;] expressions
int32_t print(uint16_t)
{
    IData &d = data();
    used();
    if (word(pc) == tk::Dieze) {
        pc += 2;
        if (expression() == '2')
            fail(EType);
        if (word(pc) != tk::Vir)
            return AfterInstruction;
        pc += 2;
    }
    for (;;) {
        if (finished(word(pc)))
            return AfterInstruction;
        d.VerPos = address(pc);
        if (word(pc) == tk::Using) {
            pc += 2;
            string();
            want(tk::PVir);
        }
        expression();
        uint16_t t = next_word();
        if (t != tk::Vir && t != tk::PVir) {
            pc -= 2;
            return AfterInstruction;
        }
    }
}

// Print #
int32_t print_channel(uint16_t t)
{
    integer();
    want(tk::Vir);
    return print(t);
}

int32_t input_variables()
{
    do
        variable();
    while (next_word() == tk::Vir);
    if (word(pc - 2) != tk::PVir)
        pc -= 2;
    return AfterInstruction;
}

// Input / Line Input ["prompt";] variables
int32_t input(uint16_t)
{
    used();
    if (word(pc) != tk::Var) {
        if (expression() != '2')
            fail(EType);
        want(tk::PVir);
    }
    return input_variables();
}

// Input # / Line Input #
int32_t input_channel(uint16_t)
{
    integer();
    want(tk::Vir);
    return input_variables();
}

// Palette / Default Palette: up to 32 colours
int32_t palette(uint16_t)
{
    for (uint16_t n = 1;; n++) {
        used();
        integer();
        if (word(pc) != tk::Vir)
            return AfterInstruction;
        pc += 2;
        if (n >= 32)
            return AfterInstruction;
    }
}

// Fade speed[,colours] / Fade speed To screen[,mask]
int32_t fade(uint16_t t)
{
    used();
    integer();
    uint16_t end = last_token();
    if (end == tk::Vir) {
        pc += 2;
        return palette(t);
    }
    if (end != tk::To)
        return AfterInstruction;
    pc += 2;
    integer();
    if (word(pc) != tk::Vir)
        return AfterInstruction;
    pc += 2;
    integer();
    return AfterInstruction;
}

void menu_dimensions()
{
    if (dimensions() >= MnNDim)
        syntax();
}

// Menu$(,,)=title[,...]: up to four strings, any left out
int32_t menu_string(uint16_t)
{
    menu_dimensions();
    want(tk::Eg);
    for (int i = 0; i < 3; i++) {
        if (word(pc) != tk::Vir) {
            string();
            if (word(pc) != tk::Vir)
                return AfterInstruction;
        }
        pc += 2;
    }
    string();
    return AfterInstruction;
}

// Menu On / Off / ... (n) or Menu x(,,)
int32_t menu_flags(uint16_t)
{
    used();
    if (word(pc) != tk::Par1) {
        if (word(pc) >= tk::MnCl)
            syntax();
        integer();
        return AfterInstruction;
    }
    menu_dimensions();
    return AfterInstruction;
}

int32_t menu_del(uint16_t)
{
    used();
    if (word(pc) != tk::Par1)
        return AfterInstruction;
    menu_dimensions();
    return AfterInstruction;
}

// Set Menu (,,) To x,y
int32_t set_menu(uint16_t)
{
    used();
    menu_dimensions();
    want(tk::To);
    integer();
    want(tk::Vir);
    integer();
    return AfterInstruction;
}

// Menu Key (,,) [To key$ | To scancode[,shift]]
int32_t menu_key(uint16_t)
{
    used();
    menu_dimensions();
    if (word(pc) != tk::To)
        return AfterInstruction;
    pc += 2;
    uint8_t t = evaluate();
    if (t == '2')
        return AfterInstruction;
    if (t != '0')
        fail(EType);
    if (word(pc) != tk::Vir)
        return AfterInstruction;
    pc += 2;
    integer();
    return AfterInstruction;
}

// On Menu Goto / Gosub labels, On Menu Proc procedures
int32_t on_menu(uint16_t)
{
    uint16_t t = next_word();
    if (t == tk::Gto || t == tk::Gsb) {
        do
            label_or_expression();
        while (next_word() == tk::Vir);
    } else if (t == tk::Prc) {
        do
            procedure_no_parameter();
        while (next_word() == tk::Vir);
    } else {
        syntax();
    }
    pc -= 2;
    return AfterInstruction;
}

int32_t follow(uint16_t)
{
    if (finished(word(pc)))
        return AfterInstruction;
    do
        expression();
    while (next_word() == tk::Vir);
    pc -= 2;
    return AfterInstruction;
}

// Data: its distance from the start of the line poked
int32_t data_line(uint16_t)
{
    used();
    poke_w(pc, uint16_t(address(pc) - data().VDLigne));
    do {
        pc += 2;
        expression();
    } while (word(pc) == tk::Vir);
    return AfterInstruction;
}

int32_t read(uint16_t)
{
    for (;;) {
        used();
        variable();
        if (word(pc) != tk::Vir)
            return AfterInstruction;
        pc += 2;
    }
}

int32_t restore(uint16_t)
{
    if (!finished(word(pc)))
        label_or_expression();
    return AfterInstruction;
}

// Channel n To Screen Display / Offset / Size, Bob, Sprite, Rainbow n,
// or To an address
int32_t channel(uint16_t)
{
    used();
    integer();
    want(tk::To);
    uint16_t t = next_word();
    if (t != tk::ScD && t != tk::ScO && t != tk::ScS && t != tk::Bob && t != tk::Spr && t != tk::Rn)
        pc -= 2;
    integer();
    return AfterInstruction;
}

// Polyline / Polygon [x,y] To x,y [To ...]
int32_t poly(uint16_t)
{
    used();
    if (word(pc) != tk::To)
        goto point;
    for (;;) {
        if (next_word() != tk::To) {
            pc -= 2;
            return AfterInstruction;
        }
    point:
        integer();
        want(tk::Vir);
        integer();
    }
}

// Mid$ / Left$ / Right$ (a$,...)=b$: the string variable, then the
// parameters as the function's
int32_t mid(uint16_t token)
{
    IData &d = data();
    used();
    uint8_t *at = pc;
    d.Ver_NoReloc++;
    pc += 2;
    string_variable();
    d.Ver_NoReloc--;
    want(tk::Vir);
    pc = at;
    d.VerBase = d.AdTokens[0];
    uint8_t kind;
    function(old_definition(ptr<uint8_t>(d.AdTokens[0]), token, &kind));
    want(tk::Eg);
    if (expression() != '2')
        fail(EType);
    return AfterInstruction;
}

int32_t inc(uint16_t)
{
    used();
    integer_variable();
    variable();
    return AfterInstruction;
}

// Add v,n[,base To top]: the token of its two or four parameters
int32_t add(uint16_t)
{
    used();
    poke_w(pc - 2, tk::Ad2);
    uint8_t *at = pc;
    integer_variable();
    variable();
    want(tk::Vir);
    integer();
    if (word(pc) != tk::Vir)
        return AfterInstruction;
    poke_w(at - 2, tk::Ad4);
    pc += 2;
    integer();
    // (the token after the base taken, To or not)
    if (next_word() != tk::To)
        return AfterInstruction;
    integer();
    return AfterInstruction;
}

// Field #n,length As a$[,...]
int32_t field(uint16_t)
{
    used();
    integer();
    if (word(pc) != tk::Vir)
        syntax();
    do {
        pc += 2;
        integer();
        want(tk::As);
        string_variable();
    } while (word(pc) == tk::Vir);
    return AfterInstruction;
}

int32_t call_instruction(uint16_t)
{
    used();
    integer();
    while (word(pc) == tk::Vir) {
        pc += 2;
        expression();
    }
    return AfterInstruction;
}

// Ver_Normal: an instruction, its parameters against its definitions
int32_t normal(uint16_t token)
{
    IData &d = data();
    used();
    d.VerBase = d.AdTokens[0];
    uint8_t kind;
    instruction(definition(ptr<uint8_t>(d.AdTokens[0]), token, &kind));
    return AfterInstruction;
}

int32_t normal_pro(uint16_t token)
{
    not_13();
    return normal(token);
}

// Ver_DejaTestee: an instruction tested already (its form known)
int32_t tested(uint16_t token)
{
    used();
    uint8_t kind;
    instruction_tested(definition(ptr<uint8_t>(data().AdTokens[0]), token, &kind));
    return AfterInstruction;
}

int32_t tested_pro(uint16_t token)
{
    not_13();
    return tested(token);
}

// VerVR: a reserved variable assigned (=Tempo, Param...): its parameters,
// = and the value, of its type; the number of parameters
uint16_t reserved_variable(const uint8_t *def)
{
    uint8_t t = *def++;
    uint16_t n = function(def);
    data().VerPos = address(pc);
    want(tk::Eg);
    if (expression() != t)
        fail(EType);
    return n;
}

int32_t reserved(uint16_t token)
{
    IData &d = data();
    used();
    d.VerBase = d.AdTokens[0];
    uint8_t kind;
    reserved_variable(definition(ptr<uint8_t>(d.AdTokens[0]), token, &kind));
    return AfterInstruction;
}

int32_t reserved_pro(uint16_t token)
{
    not_13();
    return reserved(token);
}

// Ver_Extension: an instruction of an extension (slot, number of
// parameters, token): the number poked, -1 for a 2.0 library
int32_t extension(uint16_t)
{
    IData &d = data();
    used();
    int8_t slot = int8_t(*pc++);
    uint8_t *count = pc++;
    uint16_t token = next_word();
    if (!d.AdTokens[slot])
        fail(ENoExtension);
    const uint8_t *table = ptr<uint8_t>(d.AdTokens[slot]);
    constexpr int LB_Flags = -12;
    bool v20 = table[LB_Flags] & (1 << 2);
    d.VerBase = address(table);
    uint8_t kind;
    const uint8_t *def = old_definition(table, token, &kind);
    uint16_t n;
    if (kind == 'I')
        n = instruction(def);
    else if (kind == 'V')
        n = reserved_variable(def);
    else
        syntax();
    *count = v20 ? 0xFF : uint8_t(n);
    return AfterInstruction;
}

// VpSha: the variables of Shared / Global at pc (on its token) created
// where they are (a()  for an array)
void shared_create()
{
    IData &d = data();
    pc += 2;
    for (;;) {
        d.VerPos = address(pc);
        want(tk::Var);
        pc[3] &= 0x0F;
        Name n = name();
        uint8_t *e;
        if (word(n.after) != tk::Par1) {
            store(&e);
        } else {
            if (word(n.after + 2) != tk::Par2)
                fail(ENoParameter);
            pc = const_cast<uint8_t *>(n.after) + 4;
        }
        if (next_word() != tk::Vir)
            break;
    }
    pc -= 2;
    d.VerPos = address(pc);
    if (word(pc))
        syntax();
}

// VerSha: Shared / Global, alone on their line: the variables marked
// global in the main program's names (Global in the main program: made
// there first)
int32_t shared_line(uint16_t)
{
    IData &d = data();
    used();
    pc -= 2;
    no_direct();
    uint32_t names = d.DVNmBas;
    if (!d.Phase) {
        uint8_t *at = pc;
        shared_create();
        pc = at;
        names = d.VNmBas;
    }
    pc += 2;
    for (;;) {
        d.VerPos = address(pc);
        want(tk::Var);
        const uint8_t *nm = pc + 4;
        int16_t length = int8_t(pc[2]);
        pc[3] &= 0x0F;
        uint8_t flags = pc[3];
        pc += 4 + length;
        if (word(pc) == tk::Par1) {
            flags |= 0x40;
            const_cast<uint8_t *>(nm)[-1] |= 0x40;
            if (word(pc + 2) != tk::Par2)
                fail(ENoParameter);
            pc += 4;
        }
        uint8_t *a = ptr<uint8_t>(names);
        for (;; a += 6 + int16_t(int8_t(*a))) {
            if (!*a)
                fail(ENotDimensioned);
            if (int16_t(int8_t(*a)) != length || a[1] != flags)
                continue;
            bool same = true;
            for (int16_t i = 0; i < length; i += 2)
                if (peek_w(a + 6 + i) != peek_w(nm + i)) {
                    same = false;
                    break;
                }
            if (same)
                break;
        }
        if (a[5] != 2) {
            a[5] = 1;
            if (!d.Phase)
                a[5]++;
        }
        uint16_t t = next_word();
        if (t == tk::Vir)
            continue;
        if (t)
            fail(ESharedLine);
        return AtLine;
    }
}

// V1_ProcedureIn: in pass 1 of a procedure, its parameters stored (a
// machine code one: its variables' size, and done)
int32_t procedure_in()
{
    IData &d = data();
    if (pc[8] & (1 << 4)) {
        d.VarLong = peek_w(pc + 6);
        return PassTwo;
    }
    pc += 12;
    pc += 4 + int8_t(pc[2]);
    if (word(pc) != tk::Bra1)
        return AfterInstruction;
    do {
        pc += 4;
        uint8_t *e;
        store(&e);
    } while (word(pc) == tk::Vir);
    pc += 2;
    return AfterInstruction;
}

// ProCode: the lines of a locked procedure (at its Procedure token)
// coded or decoded, but for their first token; the flag changed. Not a
// compiled one.
void decode_procedure(uint8_t *proc)
{
    if (proc[8] & (1 << 4))
        return;
    const uint8_t *end = proc + 10 + 2 + 4 + peek_l(proc + 2);
    uint8_t *next = proc - 2 + 2 * proc[-2];
    uint32_t key = peek_l(proc + 2);
    key = ((key << 8) | (key >> 24)) & 0xFFFFFF00u;
    key |= proc[9];
    uint16_t step = 1;
    const uint16_t step2 = peek_w(proc + 6);
    for (;;) {
        uint8_t *line = next;
        next = line + 2 * line[0];
        uint8_t *p = line + 4;
        if (p == end)
            break;
        do {
            poke_w(p, peek_w(p) ^ uint16_t(key));
            p += 2;
            key = (key & 0xFFFF0000u) | uint16_t(key + step);
            step += step2;
            key = (key >> 1) | (key << 31);
        } while (p != next);
    }
    proc[8] ^= 1 << 5;
}

// V1_Procedure: in the main program, the procedure's name as a label (its
// address the start), its parameters, its Shared and Global; its body
// skipped to End Proc, the distance poked. Encrypted ones decoded first.
int32_t procedure_start(uint16_t)
{
    IData &d = data();
    used();
    pc -= 2;
    no_direct();
    if (d.Phase)
        return procedure_in();
    d.VerNInst--;
    if ((pc[8] & (1 << 6)) && (pc[8] & (1 << 5)))
        decode_procedure(pc);
    Entry *e = table_add(0, tk::Proc, 4, 1 << 2);
    uint8_t *start = pc;
    pc += 10;
    want(tk::Var);
    pc[3] = uint8_t((pc[3] & 0x0F) | 0x80);
    poke_l(label_store() + 4, address(start));
    if (word(pc) == tk::Bra1) {
        do {
            pc += 2;
            d.VerPos = address(pc);
            want(tk::Var);
            pc[3] &= 0x0F;
            pc += 4 + int8_t(pc[2]);
        } while (word(pc) == tk::Vir);
        d.VerPos = address(pc);
        want(tk::Bra2);
    }
    d.VerPos = address(pc);
    if (word(pc))
        fail(EProcedureTwice);
    pc += 2;
    if (start[8] & (1 << 4)) {
        // in machine code (compiled by APCmp: relocated)
        if (word(pc + 2) == tk::APCmp) {
            relocate_compiled(pc + 2);
            not_13();
        }
        pc = start + 12 + peek_l(start + 2);
    } else {
        for (;;) {
            uint16_t length = pc[0];
            if (!length)
                fail(EProcedureOpen);
            uint16_t t = word(pc + 2);
            if (t == tk::EndP) {
                pc += 2 * length;
                break;
            }
            if (t == tk::Sha || t == tk::Glo) {
                uint8_t *at = pc;
                pc += 2;
                shared_create();
                pc = at;
            }
            pc += 2 * length;
        }
    }
    pc -= 2;
    poke_l(reinterpret_cast<uint8_t *>(e) + 24, address(pc));
    poke_l(start + 2, uint32_t(address(pc) - address(start) - 10));
    pc += 2;
    return AtLine;
}

// V1_EndProc [expression]: the end of pass 1 of a procedure
int32_t procedure_end_line(uint16_t)
{
    no_direct();
    if (!data().Phase)
        fail(EProcedureNotOpen);
    if (next_word() == tk::Bra1)
        expression();
    return PassTwo;
}

// Proc name[params] / On Break Proc name: a procedure called
int32_t proc(uint16_t)
{
    no_direct();
    used();
    uint16_t t = word(pc);
    if (t != tk::Var && t != tk::Pro)
        syntax();
    uint8_t *at = pc;
    pc += 2;
    variable_instruction();
    if (word(at) != tk::Pro)
        fail(EProcedureUndefined);
    return AfterInstruction;
}

// Struc(address,"equate")=value / Struc$(...)=string
int32_t structure_instruction(uint16_t)
{
    used();
    if (structure() >= 7)
        fail(54);
    want(tk::Eg);
    if (expression() != '0')
        fail(EType);
    return AfterInstruction;
}

int32_t structure_string_instruction(uint16_t)
{
    if (structure() != 6)
        fail(54);
    want(tk::Eg);
    if (expression() != '2')
        fail(EType);
    return AfterInstruction;
}

int32_t syntax_error(uint16_t) { syntax(); }
int32_t end_of_line(uint16_t) { return AtLine; }

// In a line, what only starts one
int32_t shared_in_line(uint16_t) { fail(ESharedLine); }
int32_t data_in_line(uint16_t) { fail(EDataStart); }
int32_t procedure_in_line(uint16_t) { fail(EProcedureTwice); }

} // namespace

namespace amos::verif {

uint8_t variable_alone() { return variable(); }



// The routine of an instruction of kind, at the start of a line or not;
// null if it is in the assembler
Routine routine_of(int8_t kind, bool line_start)
{
    if (line_start) {
        switch (uint8_t(kind)) {
        case 0xFA: case 0xFB: return shared_line;
        case 0xFC: return def_fn;
        case 0xFD: return data_line;
        case 0xFE: return procedure_end_line;
        case 0xFF: return procedure_start;
        default: return nullptr;
        }
    }
    switch (uint8_t(kind)) {
    case 0xFA: case 0xFB: return shared_in_line;
    case 0xFC: case 0xFD: return data_in_line;
    case 0xFE: case 0xFF: return procedure_in_line;
    case 0x02: return rem;
    case 0x03: return set_buffer;
    case 0x04: return double_precision;
    case 0x05: return set_stack;
    case 0x06: return variable_line;
    case 0x07: return label;
    case 0x08: return procedure_line;
    case 0x09: return dim;
    case 0x0A: case 0x48: return print;
    case 0x0B: return print_channel;
    case 0x0C: case 0x49: return input;
    case 0x0D: case 0x4A: return input_channel;
    case 0x0E: case 0x16: return inc;
    case 0x11: case 0x12: return palette;
    case 0x13: return read;
    case 0x14: return restore;
    case 0x15: return channel;
    case 0x17: case 0x4F: return add;
    case 0x18: return poly;
    case 0x19: return field;
    case 0x1A: return call_instruction;
    case 0x1B: return menu_string;
    case 0x1C: return menu_del;
    case 0x1D: return set_menu;
    case 0x1E: return menu_key;
    case 0x1F: return menu_flags;
    case 0x20: return fade;
    case 0x21: return sort;
    case 0x22: return swap;
    case 0x23: return follow;
    case 0x24: return set_accessory;
    case 0x42: return on_menu;
    case 0x4B: case 0x4C: case 0x4D: case 0x4E: return mid;
    case 0x0F: case 0x41: return proc;
    case 0x00: case 0x10: case 0x51: case 0x53: case 0x54: case 0x56: return normal;
    case 0x29: case 0x50: case 0x55: return normal_pro;
    case 0x2D: return tested;
    case 0x2A: return tested_pro;
    case 0x2B: return reserved;
    case 0x2C: return reserved_pro;
    case 0x28: return extension;
    case 0x26: return structure_instruction;
    case 0x27: return structure_string_instruction;
    case 0x01: case 0x52: case 0x57: return syntax_error;
    case 0x2E: case 0x2F: return end_of_line;
    default: return loop_routine(kind);
    }
}

} // namespace amos::verif
