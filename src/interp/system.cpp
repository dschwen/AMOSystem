// Machine code and system structures (+ILib.s): Call, =Equ / =Lvo,
// Struc and Struc$.
//
// An equate in the program: the token, the value (the verifier finds it in
// the equates file), a word whose high byte is its type, then
// ("name") - for Struc (address,"name"). Types: 0 byte, 1 word, 2 long,
// 3 unsigned byte, 4 unsigned word, 5 and 6 unsigned long; 7 (past the
// original's jump table) works as 0.

#include "interp/interp.h"

#include "lib/strings.h"

using namespace amos;
using namespace amos::interp;

namespace {

// After the name: ',', the string's token, its length, the characters,
// ')'
void skip_name()
{
    uint16_t length = peek_w(pc() + 4);
    pc() += 2 + 4 + 2 + length + (length & 1);
}

// GStruc: the address of the field and its type
struct Field {
    uint8_t *at;
    uint8_t type;
};

Field field()
{
    uint32_t offset = peek_l(pc());
    uint8_t type = pc()[4];
    pc() += 6 + 2;
    uint32_t base = integer().v1;
    skip_name();
    return {ptr<uint8_t>(base + offset), type};
}

void even(const Field &f)
{
    if (address(f.at) & 1)
        error(25);
}

} // namespace

extern "C" {

// Call address/bank[,parameters]: the routine gets the registers of
// CallReg (and leaves its own there), a3 the parameters (which stay on
// the stack, as in the original), a4 its address, a6 CallReg
void interp_in_call()
{
    IData &d = idata();
    Regs b;
    b.d[0] = integer().v1;
    lib_call(L_Bnk_OrAdr, b);
    uint32_t routine = b.a[0];
    Ctx &c = ctx();
    while (peek_token() == tk::Vir) {
        pc() += 2;
        uint32_t v = integer().v1;
        c.a3 -= 4;
        poke_l(c.a3, v);
    }
    AllRegs r;
    for (int i = 0; i < 8; i++)
        r.d[i] = d.CallReg[i];
    for (int i = 0; i < 3; i++)
        r.a[i] = d.CallReg[8 + i];
    r.a[3] = address(c.a3);
    r.a[4] = routine;
    r.a[5] = address(c.a5);
    r.a[6] = address(d.CallReg);
    amos_call_all(&r, ptr<void>(routine));
    for (int i = 0; i < 8; i++)
        d.CallReg[i] = r.d[i];
    for (int i = 0; i < 3; i++)
        d.CallReg[8 + i] = r.a[i];
}

// =Equ("name") / =Lvo("name")
void interp_fn_equ(EvalOut *o)
{
    uint32_t v = peek_l(pc());
    uint16_t length = peek_w(pc() + 10);
    pc() += 14 + length + (length & 1);
    *o = {0, 0, TInt, v, 0, 0};
}

// Struc(address,"name")=value. The original wrote the type's index
// instead of the value into a byte field.
void interp_in_struc()
{
    Field f = field();
    pc() += 2;
    uint32_t v = integer().v1;
    switch (f.type) {
    case 1:
    case 4:
        even(f);
        poke_w(f.at, uint16_t(v));
        break;
    case 2:
    case 5:
    case 6:
        even(f);
        poke_l(f.at, v);
        break;
    default:
        *f.at = uint8_t(v);
    }
}

// =Struc(address,"name")
void interp_fn_struc(EvalOut *o)
{
    Field f = field();
    uint32_t v;
    switch (f.type) {
    case 1:
        even(f);
        v = uint32_t(int32_t(int16_t(peek_w(f.at))));
        break;
    case 4:
        even(f);
        v = peek_w(f.at);
        break;
    case 2:
    case 5:
    case 6:
        even(f);
        v = peek_l(f.at);
        break;
    case 3:
        v = *f.at;
        break;
    default:
        v = uint32_t(int32_t(int8_t(*f.at)));
    }
    *o = {0, 0, TInt, v, 0, 0};
}

// Struc$(address,"name")=a$: the field points to the characters of a copy
// of the string, zero-terminated (its length word counting the zero);
// "|00|" leaves it 0. The original pointed to the length word, so the
// field held no C string and =Struc$ read "" back.
void interp_in_struc_d()
{
    IData &d = idata();
    Field f = field();
    even(f);
    poke_l(f.at, 0);
    const uint8_t *s = ptr<uint8_t>(evaluate_after().v1);
    uint16_t length = peek_w(s);
    if (peek_l(s + 2) == 0x7C30307C)        // "|00|"
        return;
    Regs r;
    r.d[3] = length + 2u;
    lib_call(L_Demande, r);
    uint32_t end = r.a[0] + 2 + length + 2;
    d.HiChaine = end + (end & 1);
    uint8_t *copy = ptr<uint8_t>(r.a[1]);
    poke_l(f.at, address(copy + 2));
    poke_w(copy, uint16_t(length + 1));
    for (uint16_t i = 0; i < length; i++)
        copy[2 + i] = s[2 + i];
    copy[2 + length] = 0;
}

// =Struc$(address,"name"): the string the field points to (the original
// crashed on a field of 0)
void interp_fn_struc_d(EvalOut *o)
{
    Field f = field();
    even(f);
    uint32_t text = peek_l(f.at);
    uint32_t s = idata().ChVide;
    if (text) {
        Regs r;
        r.a[0] = text;
        lib_call(L_A0ToChaine, r);
        s = r.a[0];
    }
    *o = {0, 0, TString, s, 0, 0};
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_system.inc"
#endif
