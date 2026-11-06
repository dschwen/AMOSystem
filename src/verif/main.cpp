// The verifier's loops (+Verif.s SsTest): pass 1 over the program (or a
// procedure, or a line of direct mode), each instruction to its routine
// through the tables of the assembler; then pass 2, the relocation table
// read again (variables, labels, procedure calls) and the routines of the
// loops and structures (TablA).
//
// The instructions' routines (verif/handlers.cpp, verif/loops.cpp) return
// what comes next (-1 from Set Buffer: the variables' buffer changed, all
// to verify again).

#include "verif/verif.h"

using namespace amos;
using namespace amos::verif;


namespace {

constexpr uint8_t RelocEnd = 0x80;

IData &data() { return idata(); }

// An instruction's routine
int32_t routine(int8_t kind, bool line_start, uint16_t token) { return routine_of(kind, line_start)(token); }

// VerDP: after an instruction, a colon, Else or the end of the line
int32_t after_instruction()
{
    IData &d = data();
    d.VerPos = address(pc);
    uint16_t t = next_word();
    if (!t)
        return AtLine;
    if (t == tk::DP)
        return AtInstruction;
    if (t != tk::Else)
        fail(ESyntax);
    pc -= 2;
    return AtInstruction;
}

// Pass 2: the procedure called (V2_CallProc*): its parameters' definition
const uint8_t *parameters;

// V2_StoVar: a variable's offset, now in the variables (< 0: global)
void variable_offset()
{
    IData &d = data();
    int16_t at = int16_t(word(pc));
    if (at < 0) {
        poke_w(pc, peek_w(ptr<uint8_t>(d.VNmHaut + at) + 2));
    } else {
        uint16_t v = peek_w(ptr<uint8_t>(d.DVNmHaut - at) + 2);
        poke_w(pc, uint16_t(-int16_t(v + 1)));
    }
}

void call_parameters()
{
    IData &d = data();
    d.VerPos = address(pc);
    uint8_t *e;
    uint32_t start = label_use(0, &e);
    if (!start)
        fail(EProcedureUndefined);
    const uint8_t *p = ptr<uint8_t>(start) + 12;
    p += 4 + int8_t(p[2]);
    parameters = word(p) == tk::Bra1 ? p + 2 : nullptr;
}

void call_parameter(const uint8_t *&reloc)
{
    IData &d = data();
    if (!parameters)
        fail(EIllegalParameters);
    d.VerPos = address(pc);
    if (word(parameters) == tk::Bra2)
        fail(EIllegalParameters);
    if (word(parameters) == tk::Vir)
        parameters += 2;
    uint8_t t = parameters[5] & 0x0F;
    if (t == 1)
        t = 0;
    if (uint8_t('0' + t) != *reloc++)
        fail(EType);
    parameters += 6 + int8_t(parameters[4]);
}

// VerX: pass 2
void pass_two()
{
    IData &d = data();
    d.Passe++;
    reloc(RelocEnd);
    const uint8_t *r = ptr<uint8_t>(d.Ver_Reloc) + 4;
    d.Ver_CReloc = d.Ver_Reloc;
    pc = ptr<uint8_t>(d.Prg_Test);
    for (bool more = true; more;) {
        uint8_t b = *r++;
        if (b < 0x80) {
            pc += b * 2;
            continue;
        }
        switch (b) {
        case 0x80:                      // the end
            more = false;
            break;
        case 0x82:                      // a variable
            variable_offset();
            break;
        case 0x84: {                    // a long jump
            uint16_t n = uint16_t(r[0] << 8 | r[1]);
            r += 2;
            pc += n;
            break;
        }
        case 0x86:                      // the next buffer
            d.Ver_CReloc = peek_l(ptr<uint8_t>(d.Ver_CReloc));
            r = ptr<uint8_t>(d.Ver_CReloc) + 4;
            break;
        case 0x88:                      // a procedure called
            call_parameters();
            break;
        case 0x8A:                      // one of its parameters
            call_parameter(r);
            break;
        case 0x8C:                      // after them
            d.VerPos = address(pc);
            if (!parameters || word(parameters) != tk::Bra2)
                fail(EIllegalParameters);
            break;
        case 0x8E:                      // none given
            if (parameters)
                fail(EIllegalParameters);
            break;
        case 0x92: {                    // a label
            d.VerPos = address(pc) - 2;
            uint8_t *e;
            if (!label_use(d.Phase, &e))
                fail(ELabelUndefined);
            break;
        }
        default:
            // (Reloc_Debug, only in the debugging version)
            break;
        }
    }

    // The loops and structures
    if (uint32_t t = d.Ver_TablA) {
        for (uint32_t e = t + 4; e; e = peek_l(ptr<uint8_t>(e))) {
            uint32_t jump = peek_l(ptr<uint8_t>(e) + 20);
            if (!jump)
                continue;
            pc = ptr<uint8_t>(peek_l(ptr<uint8_t>(e) + 12));
            d.VerPos = address(pc) - 2;
            AllRegs a = {};
            a.a[0] = e;
            ctx().a4 = decode_context_cursor(e);
            call(host::decode_routine(jump), a);
        }
    }
    reloc_free();
    // an error found on the way, reported now
    if (d.ErrRet) {
        d.VerPos = d.ErrRAd;
        fail(uint16_t(d.ErrRet));
    }
}

} // namespace

namespace amos::verif {

// Ver_Verif / Ver_Run: the token tables of AMOS and the extensions that
// have one for the verifier (LB_Verif: its length, then a long per token)
// swapped with it, the first long of each token's entry, so that the
// tokens lead to the verifier's routines; or back for running
void tables_swap(bool verify)
{
    constexpr int LBVerif = -24, LBFlags = -12;
    IData &d = data();
    for (int i = 0; i < 27; i++) {
        uint8_t *tokens = ptr<uint8_t>(d.AdTokens[i]);
        if (!tokens || !peek_l(tokens + LBVerif))
            continue;
        uint8_t &flags = tokens[LBFlags];
        if (bool(flags & 1) == verify)
            continue;
        flags ^= 1;
        uint8_t *table = ptr<uint8_t>(peek_l(tokens + LBVerif));
        const uint8_t *table_end = table + 2 + int16_t(peek_w(table));
        table += 2;
        uint8_t *t = tokens;
        do {
            uint32_t v = peek_l(t);
            poke_l(t, peek_l(table));
            poke_l(table, v);
            t += 4;
            table += 4;
            while (!(*t++ & 0x80))
                ;
            while (!(*t++ & 0x80))
                ;
            t += address(t) & 1;
        } while (table < table_end);
    }
}

// SsTest: 0, or -1 to verify again
int32_t test()
{
    IData &d = data();
    d.ErrRet = 0;
    d.Passe = 0;
    d.Ver_NBoucles = 0;
    d.Ver_PBoucles = 0;
    tables_swap(true);
    reloc_reserve();
    tables_reserve();
    pc = ptr<uint8_t>(d.Prg_Test);
    ctx().a3 = pc;

    int32_t next = d.DirFlag || !d.Phase ? int32_t(AtLine) : int32_t(LineStart);
    for (;;) {
        if (next == AtLine) {
            d.VDLigne = address(pc);
            if (!next_word()) {
                pass_two();
                return 0;
            }
            next = LineStart;
        }
        const uint8_t *tokens = ptr<uint8_t>(d.AdTokens[0]);
        int8_t kind;
        uint16_t t;
        if (next == LineStart) {
            // the first instruction of a line
            d.VerPos = address(pc);
            t = next_word();
            if (!t) {
                next = AtLine;
                continue;
            }
            if (int16_t(t) < 0)
                fail(ESyntax);
            kind = int8_t(tokens[t]);
            if (kind < 0) {
                d.VerNInst++;
                next = routine(kind, true, t);
                goto done;
            }
        } else {
            d.VerPos = address(pc);
            t = next_word();
            if (!t) {
                next = AtLine;
                continue;
            }
            if (int16_t(t) < 0)
                fail(ESyntax);
            kind = int8_t(tokens[t]);
        }
        d.VerNInst++;
        next = routine(kind, false, t);
    done:
        switch (next) {
        case AtLine:
        case AtInstruction:
            break;
        case AfterInstruction:
            next = after_instruction();
            break;
        case PassTwo:
            pass_two();
            return 0;
        default:
            // a routine returning from SsTest itself (VerSBu: -1)
            return next;
        }
    }
}

} // namespace amos::verif

extern "C" {
void ver_test(AllRegs *r)
{
    enter(r);
    r->d[0] = uint32_t(test());
    leave(r);
}
}

namespace {

// Free_VerTables
void free_tables()
{
    IData &d = data();
    reloc_free();
    tables_free();
    d.Ver_TablA = d.Ver_MainTablA;
    d.Ver_MainTablA = 0;
    tables_free();
}

// The names of a new pass: none but the global ones
void names_start()
{
    IData &d = data();
    d.VarLong = 0;
    d.VNmHaut = d.DVNmBas;
    d.VNmBas = d.DVNmBas - 2;
    poke_w(ptr<uint8_t>(d.VNmBas), 0);
}

// PTest: the program tested: the main program (again after Set Buffer),
// then each procedure; the global variables made; AMOS 1.3 compatible
// if no bank beyond 16
void program_test()
{
    IData &d = data();
    d.VerNot1_3 = 0;
    d.Prg_Run = d.Prg_Source;
    includes_get();
    if (d.Prg_FullSource)
        d.Prg_Run = d.Prg_FullSource;
    d.Prg_Test = d.Prg_Run;
    resize_variables(8 * 1024);
    resize_names(d.PI_VNmMax);
    d.Phase = 0;
    // (the high word of ErrRet only)
    poke_w(reinterpret_cast<uint8_t *>(&d.ErrRet), 0);
    d.DirFlag = 0;
    d.VarBufFlg = 0;
    d.Stack_Size = 51;
    d.Prg_Accessory = 0;
    d.MathFlags = 0;
    d.Ver_SPConst = 0;
    d.Ver_DPConst = 0;
    d.VerNInst = 0;
    do
        names_start();
    while (test());

    // The procedures, found in the main program's table
    uint32_t e = d.Ver_TablA;
    d.Ver_MainTablA = e;
    if (e)
        e += 4;
    d.Ver_TablA = 0;
    d.DVNmBas = d.VNmBas;
    d.DVNmHaut = d.VNmHaut;
    d.GloLong = d.VarLong;
    while (e) {
        Entry *p = ptr<Entry>(e);
        e = p->Vta_Next;
        if (p->Vta_Flag != (1 << 2))
            continue;
        d.Prg_Test = p->Vta_Prog;
        d.Phase++;
        names_start();
        locals();
        test();
        poke_w(ptr<uint8_t>(d.Prg_Test) + 6, d.VarLong);
    }

    // The global variables, under the labels
    uint8_t *top = ptr<uint8_t>(d.LabBas) - 6;
    poke_l(top + 2, 0);
    poke_w(top, 0xFFFF);
    uint8_t *bottom = top - d.GloLong;
    if (address(bottom) < d.HiChaine)
        fail(EVariableBuffer);
    d.VarGlo = d.VarLoc = d.TabBas = address(bottom);
    for (uint8_t *v = bottom; v < top; v++)
        *v = 0;
    globals();
    free_tables();
    tables_swap(false);

    // AMOS 1.3 has 16 banks
    d.VerCheck1_3 = 0;
    for (uint32_t b = peek_l(ptr<uint8_t>(d.Cur_Banks)); b; b = peek_l(ptr<uint8_t>(b)))
        if (peek_l(ptr<uint8_t>(b) + 8) > 16) {
            d.VerNot1_3 = 1;
            break;
        }
}

// Ver_Direct: a line of direct mode tested, its variables in the direct
// mode's own
void direct_test()
{
    IData &d = data();
    if (!d.VarBuf)
        resize_variables(4 * 1024);
    direct_variables();
    if (!d.VNmMini)
        resize_names(d.PI_VNmMax);
    if (!d.Stack_Size)
        d.Stack_Size = 10;
    d.Prg_Test = d.Ed_BufT;
    d.Prg_Run = d.Prg_Test;
    d.Edt_Runned = 0;
    d.Prg_Runned = 0;
    d.Phase = 1;
    d.DirFlag = 1;
    test();
    free_tables();
    tables_swap(false);
}

} // namespace

extern "C" {
void ver_program_test(AllRegs *r)
{
    enter(r);
    program_test();
}

void ver_direct_test(AllRegs *r)
{
    enter(r);
    direct_test();
}

void ver_free_tables(AllRegs *) { free_tables(); }
}

extern "C" void ver_tables_run(AllRegs *) { tables_swap(false); }

#if !defined(__mc68000__)
#include "runtime/bindings/verif_main.inc"
#endif
