// The verifier's variables and relocation (+Verif.s VarA0, V1_StoVar,
// VerTablo, Locale, Globale, Reserve_Reloc, Free_Reloc, New_Reloc,
// Out_Reloc).
//
// A variable in the program: its offset (a word, poked here), the length
// of its name, its flags (bits 0-2 the type: 0 integer, 1 float, 2
// string; bit 3 a function, bit 6 an array, bit 7 a procedure), the name.
// In the table of names (from VNmBas up to VNmHaut, a 0 length at the
// end): the length, the flags, its place in the variables (VarLong), the
// number of dimensions, global (1) or not, the name.
//
// The relocation table: what pass 2 has to see again, as the distances
// between the places (a byte, half of it; 127: 254 bytes on) and a code
// ($80 and up: Reloc_*). Its pointer and the last place are in a4/a3 of
// the assembler, here the context's.

#include "verif/verif.h"

using namespace amos;
using namespace amos::verif;

namespace {

constexpr uint32_t RelocStep = 1024;
constexpr uint8_t RelocLong = 0x84, RelocNewBuffer = 0x86, RelocVar = 0x82;

IData &data() { return idata(); }

uint32_t memory(uint32_t size)
{
    Regs r;
    r.d[0] = size;
    sy_call(sy::MemFastClear, r);
    return r.a[0];
}

void free_memory(uint32_t address, uint32_t size)
{
    Regs r;
    r.a[1] = address;
    r.d[0] = size;
    sy_call(sy::MemFree, r);
}

} // namespace

namespace amos::verif {

// Free_Reloc
void reloc_free()
{
    IData &d = data();
    for (uint32_t b = d.Ver_Reloc; b;) {
        uint32_t next = peek_l(ptr<uint8_t>(b));
        free_memory(b, RelocStep);
        b = next;
    }
    d.Ver_Reloc = 0;
    d.Ver_FReloc = 0;
}

// Reserve_Reloc
void reloc_reserve()
{
    IData &d = data();
    reloc_free();
    d.Ver_CReloc = address(&d.Ver_Reloc);
    d.Ver_FReloc = 0;
    d.Ver_NoReloc = 0;
    ctx().a4 = nullptr;
}

// New_Reloc: the distance from the last place to pc, then code
void reloc(uint8_t code)
{
    IData &d = data();
    if (d.Ver_NoReloc)
        return;
    Ctx &c = ctx();
    if (address(c.a4) >= d.Ver_FReloc) {
        // a new buffer
        if (d.Ver_FReloc)
            *c.a4++ = RelocNewBuffer;
        uint32_t b = memory(RelocStep);
        if (!b)
            fail(EOutOfMemory);
        poke_l(ptr<uint8_t>(d.Ver_CReloc), b);
        d.Ver_CReloc = b;
        c.a4 = ptr<uint8_t>(b) + 4;
        poke_l(ptr<uint8_t>(b), 0);
        d.Ver_FReloc = b + RelocStep - 32;
        return reloc(code);
    }
    uint32_t distance = uint32_t(pc - c.a3);
    if (distance) {
        if (int32_t(distance) < 0)
            __builtin_trap();                   // (the assembler: illegal)
        for (;;) {
            if (distance <= 254) {
                *c.a4++ = uint8_t(distance >> 1);
                break;
            }
            if (distance <= 254 * 3) {
                *c.a4++ = 127;
                distance -= 254;
                continue;
            }
            if (distance <= 65534) {
                *c.a4++ = RelocLong;
                *c.a4++ = uint8_t(distance >> 8);
                *c.a4++ = uint8_t(distance);
                break;
            }
            *c.a4++ = RelocLong;
            *c.a4++ = 0xFF;
            *c.a4++ = 0xFE;
            distance -= 65534;
        }
    }
    c.a3 = pc;
    *c.a4++ = code;
}

// Out_Reloc: a byte
void reloc_byte(uint8_t b)
{
    if (!data().Ver_NoReloc)
        *ctx().a4++ = b;
}

// VarA0: the variable at pc (after its token)
Name name()
{
    Name n;
    n.length = int8_t(pc[2]);
    n.flags = pc[3];
    uint8_t t = n.flags & 7;
    n.type = t == 0 || t == 1 ? '0' : '2';
    n.after = pc + 4 + int16_t(n.length);
    return n;
}

// V1_StoVar: the variable at pc found or created in the table of names,
// its offset poked (> 0: a global one), pc past it; true if new, the
// entry in *entry
bool store(uint8_t **entry)
{
    IData &d = data();
    uint16_t length = uint16_t(int16_t(int8_t(pc[2])));
    uint8_t flags = pc[3];
    const uint8_t *name = pc + 4;
    bool created = false;
    uint8_t *e = nullptr;
    auto same = [&](const uint8_t *a) {
        for (uint16_t i = 0; i < length; i += 2)
            if (peek_w(a + i) != peek_w(name + i))
                return false;
        return true;
    };
    if (d.Phase) {
        // a global variable, in a procedure
        for (uint8_t *a = ptr<uint8_t>(d.DVNmBas); *a; a += 6 + int16_t(int8_t(*a))) {
            if (a[5] && int8_t(*a) == int16_t(length) && a[1] == flags && same(a + 6)) {
                poke_w(pc, uint16_t(-int16_t(address(a) - d.DVNmHaut)));
                e = a;
                goto found;
            }
        }
    }
    for (uint8_t *a = ptr<uint8_t>(d.VNmBas);; a += 6 + int16_t(int8_t(*a))) {
        if (!*a) {
            // a new one
            uint8_t *n = ptr<uint8_t>(d.VNmBas) - 6 - length;
            if (address(n) < d.VNmMini)
                fail(ENameSpace);
            d.VNmBas = address(n);
            n[0] = uint8_t(length);
            n[1] = flags;
            poke_w(n + 2, d.VarLong);
            poke_w(n + 4, 0);
            d.VarLong += 6;
            // a double precision float takes 10 bytes
            if (flags == 1 && int8_t(d.MathFlags) < 0)
                d.VarLong += 4;
            for (uint16_t i = 0; i < length; i += 2)
                poke_w(n + 6 + i, peek_w(name + i));
            e = n;
            created = true;
            break;
        }
        if (*a == uint8_t(length) && a[1] == flags && same(a + 6)) {
            e = a;
            break;
        }
    }
    poke_w(pc, uint16_t(address(e) - d.VNmHaut));
found:
    reloc(RelocVar);
    pc += 4 + int16_t(length);
    *entry = e;
    return created;
}

// VerTablo: the indices of an array, numbers; how many
uint16_t dimensions()
{
    IData &d = data();
    d.VerPos = address(pc);
    if (next_word() != tk::Par1)
        fail(ESyntax);
    uint16_t parenth = d.Parenth;
    uint16_t n = 0;
    for (;;) {
        d.VerPos = address(pc);
        n++;
        if (evaluate() != '0')
            fail(EType);
        if (d.Parenth)
            break;
        if (next_word() != tk::Vir)
            fail(ESyntax);
    }
    if (d.Parenth != 0xFFFF)
        fail(ESyntax);
    d.Parenth = parenth;
    return n;
}

// Locale / Globale: the variables of the main program not global (but the
// ones declared so), or all global
void locals()
{
    for (uint8_t *a = ptr<uint8_t>(data().DVNmBas); *a; a += 6 + int16_t(int8_t(*a)))
        if (a[5] != 2)
            a[5] = 0;
}

void globals()
{
    for (uint8_t *a = ptr<uint8_t>(data().DVNmBas); *a; a += 6 + int16_t(int8_t(*a)))
        a[5] = 1;
}

} // namespace amos::verif
