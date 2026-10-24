// The verifier's table of loops and structures (TablA: Reserve_TablA,
// Free_TablA, New_TablA, Init_TablA, Find_TablA, Find_TablATest,
// Goto_Loops) and its labels (Get_Label, V1_StockLabel, V2_FindLabel,
// V1_GoLabel, V2_GoLabel, V1_SautLGoto), with the small helpers
// Doke_Distance and Find_End.
//
// TablA: in buffers of 1K, an entry per loop or structure instruction
// met in pass 1 (and procedure, label...), linked both ways, with the
// routine pass 2 calls for it. A label: its key (the length of the name,
// its flags, the procedure level), its address, the name; from LabBas up,
// a 0 length at the end.

#include "verif/verif.h"

using namespace amos;
using namespace amos::verif;


namespace {

constexpr uint32_t TablAStep = 1024;
constexpr uint8_t RelocLabel = 0x92;
constexpr uint8_t VF_If = 1;            // Vta_Flag: 1 << VF_If

IData &data() { return idata(); }

uint32_t memory(uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[0] = w::reserve(r.d[0], w::Fast);
    return r.a[0];
}

void free_memory(uint32_t address, uint32_t size)
{
    Regs r;
    r.a[1] = address;
    r.d[0] = size;
    w::release(r.a[1], r.d[0]);
}

Entry *entry(uint32_t a) { return ptr<Entry>(a); }

} // namespace

namespace amos::verif {

// Free_TablA
void tables_free()
{
    IData &d = data();
    for (uint32_t b = d.Ver_TablA; b;) {
        uint32_t next = peek_l(ptr<uint8_t>(b));
        free_memory(b, TablAStep);
        b = next;
    }
    d.Ver_TablA = 0;
    d.Ver_FTablA = 0;
}

// Reserve_TablA
void tables_reserve()
{
    IData &d = data();
    tables_free();
    d.Ver_CTablA = address(&d.Ver_TablA);
    d.Ver_PTablA = 0;
    d.Ver_PrevTablA = 0;
}

// New_TablA: a new buffer
static void tables_new()
{
    IData &d = data();
    uint32_t b = memory(TablAStep);
    if (!b)
        fail(EOutOfMemory);
    poke_l(ptr<uint8_t>(d.Ver_CTablA), b);
    d.Ver_CTablA = b;
    poke_l(ptr<uint8_t>(b), 0);
    d.Ver_PTablA = b + 4;
    d.Ver_FTablA = b + TablAStep - 8;
}

// Init_TablA: an entry for token at pc, size more bytes, for pass 2's
// routine jump
Entry *table_add(uint32_t jump, uint16_t token, int16_t size, uint8_t flag)
{
    IData &d = data();
    d.VarBufFlg |= 1 << 8;
    uint32_t e = d.Ver_PTablA;
    if (e + 24 + size >= d.Ver_FTablA) {
        tables_new();
        e = d.Ver_PTablA;
    }
    d.Ver_PTablA = e + 24 + size;
    Entry *n = entry(e);
    n->Vta_Next = 0;
    n->Vta_Prev = 0;
    n->Vta_Token = token;
    n->Vta_Flag = flag;
    n->Vta_UFlag = 0;
    n->Vta_Prog = address(pc);
    n->Vta_NBoucles = d.Ver_NBoucles;
    n->Vta_PBoucles = d.Ver_PBoucles;
    n->Vta_Jump = jump;
    if (uint32_t p = d.Ver_PrevTablA) {
        entry(p)->Vta_Next = e;
        n->Vta_Prev = p;
    }
    d.Ver_PrevTablA = e;
    return n;
}

// Find_TablA: the next entry after e at loop level level with flag and
// token (any if negative); null if none
Entry *table_find(Entry *&e, uint8_t flag, uint16_t level, int16_t token)
{
    for (;;) {
        if (!e->Vta_Next)
            return nullptr;
        e = entry(e->Vta_Next);
        if (e->Vta_NBoucles == level && e->Vta_Flag == flag && (token < 0 || e->Vta_Token == uint16_t(token)))
            return e;
    }
}

// Find_TablATest: the next Else / Else If / End If of the If structure
// after e (the Ifs inside skipped); null if none
Entry *table_find_test(Entry *&e, uint16_t t1, uint16_t t2, uint16_t t3, uint16_t *token)
{
    uint16_t depth = 0;
    for (;;) {
        if (!e->Vta_Next)
            return nullptr;
        e = entry(e->Vta_Next);
        if (e->Vta_Flag != (1 << VF_If))
            continue;
        if (e->Vta_UFlag) {
            // no Then in a structured test
            data().VerPos = e->Vta_Prog - 2;
            fail(ENoThen);
        }
        uint16_t t = e->Vta_Token;
        if (t == tk::If) {
            depth++;
            continue;
        }
        if (depth) {
            if (t == tk::EndI && int16_t(--depth) < 0)
                return nullptr;
            continue;
        }
        if (t == t1 || t == t2 || t == t3) {
            *token = t;
            return e;
        }
    }
}

// Doke_Distance: the distance to address poked at pc
void poke_distance(uint32_t to)
{
    uint32_t d = to - address(pc) - 2;
    if (d >= 0x10000)
        fail(ETooLong);
    poke_w(pc, uint16_t(d));
    pc += 2;
}

// Find_End: where the instruction at pc ends (its next one)
uint32_t instruction_end()
{
    const uint8_t *a = pc;
    uint16_t w = word(a);
    if (w) {
        if (w != tk::DP)
            fail(ESyntax);
        return address(a + 2);
    }
    if (word(a + 2))
        a += 4;
    return address(a);
}

// Goto_Loops: a jump from loop entry e to address to must not go into a
// loop
void goto_loops(Entry *e, uint32_t to)
{
    uint16_t level = e->Vta_NBoucles;
    if (to < address(pc)) {
        while (e->Vta_Prev) {
            e = entry(e->Vta_Prev);
            if (to >= e->Vta_Prog) {
                if (level < e->Vta_NBoucles)
                    fail(EGotoLoop);
                return;
            }
        }
        return;
    }
    for (;;) {
        Entry *from = e;
        if (!e->Vta_Next)
            return;
        e = entry(e->Vta_Next);
        if (to < e->Vta_Prog) {
            if (level < from->Vta_NBoucles)
                fail(EGotoLoop);
            return;
        }
    }
}

// Get_Label: the label at pc (after its token) at procedure level phase;
// its entry, or null
uint8_t *label_find(uint16_t phase, uint32_t *key)
{
    IData &d = data();
    uint8_t length = pc[2];
    *key = uint32_t(length) << 24 | uint32_t(pc[3]) << 16 | phase;
    for (uint8_t *a = ptr<uint8_t>(d.LabBas); *a; a += 8 + *a) {
        if (peek_l(a) != *key)
            continue;
        bool same = true;
        for (uint16_t i = 0; i < *a; i += 2)
            if (peek_w(a + 8 + i) != peek_w(pc + 4 + i)) {
                same = false;
                break;
            }
        if (same)
            return a;
    }
    return nullptr;
}

// V1_StockLabel: the label defined at pc stored, pc past it; its entry
uint8_t *label_store()
{
    IData &d = data();
    uint32_t key;
    if (label_find(d.Phase, &key))
        fail(ELabelTwice);
    uint8_t length = pc[2];
    uint8_t *a = ptr<uint8_t>(d.LabBas) - 8 - length;
    if (address(a) < d.LabMini)
        fail(EVariableBuffer);
    d.LabBas = address(a);
    poke_l(a, key);
    for (uint16_t i = 0; i < length; i += 2)
        poke_w(a + 8 + i, peek_w(pc + 4 + i));
    pc += 4 + length;
    const uint8_t *to = pc;
    if (!word(to) && word(to + 2))
        to += 4;
    poke_l(a + 4, address(to));
    return a;
}

// V2_FindLabel(P): the label used at pc found, its offset poked; its
// address (0 if none), its entry in *entry
uint32_t label_use(uint16_t phase, uint8_t **entry)
{
    IData &d = data();
    uint32_t key;
    uint8_t *a = label_find(phase, &key);
    *entry = a;
    if (!a)
        return 0;
    poke_w(pc, uint16_t(address(a) + 4 - d.LabHaut));
    return peek_l(a + 4);
}

// V1_GoLabel: after Goto & co, a label (its token made a label's), or an
// expression
void label_or_expression()
{
    uint16_t t = word(pc);
    if (t == tk::LGo) {
        t = tk::Var;
        poke_w(pc, t);
    }
    if (t == tk::Var && !(pc[5] & 0x0F)) {
        const uint8_t *after = pc + 6 + int8_t(pc[4]);
        uint16_t w = word(after);
        if (!w || w == tk::Vir || finished(w)) {
            poke_w(pc, tk::LGo);
            pc += 2;
            reloc(RelocLabel);
            pc = const_cast<uint8_t *>(after);
            return;
        }
    }
    evaluate();
}

// V2_GoLabel: the address of the label at pc, 0 if an expression
uint32_t label_address()
{
    if (next_word() != tk::LGo)
        return 0;
    uint16_t offset = next_word();
    pc += 2 + int8_t(*pc);
    return peek_l(ptr<uint8_t>(data().LabHaut + int16_t(offset)));
}

// V1_SautLGoto: a label after Goto skipped
void label_skip()
{
    data().VerPos = address(pc);
    if (next_word() != tk::LGo)
        fail(ESyntax);
    pc += 4 + int8_t(pc[2]);
}

} // namespace amos::verif
