// The verifier's expressions (+Verif.s Ver_Evalue, Ver_Operande, Ope_*,
// the operators' tests Tst_*) and the parameters of instructions and
// functions (VerI, VerF, VerC and their "already tested" forms).
//
// A type is a character: '0' a number, '2' a string. An instruction's or
// function's definition in its token table: after the name, its type,
// then its parameters' types each followed by its separator (',' or 't'
// for To), -1 at the end; -2 at the end: another form follows, -3 a
// function form of an instruction.

#include "verif/verif.h"

using namespace amos;
using namespace amos::verif;

// +Verif.s: the operators' table (a test kind before each, see Tst_Jumps)
// and the operands still in the assembler, entered as from Ver_Operande
extern "C" uint8_t ver_operators[], ver_equ_nul[], ver_equ_lvo[];

namespace {

constexpr int LB_Flags = -12;
constexpr uint8_t LBF_20 = 2;

// The test of an operator (Tst_Jumps)
enum : uint16_t { Figures, Comparison, Mixed, Power };

IData &data() { return idata(); }

[[noreturn]] void syntax() { fail(ESyntax); }
[[noreturn]] void type() { fail(EType); }

const uint8_t *tokens() { return ptr<uint8_t>(data().AdTokens[0]); }

uint16_t last;                  // the token after the last expression (d0)

} // namespace

// Ver_DInst: the parameters of the definition of token in table; its type
// in *kind
const uint8_t *amos::verif::definition(const uint8_t *table, uint16_t token, uint8_t *kind)
{
    const uint8_t *a = table + token;
    a += int8_t(a[2]);
    uint8_t c = *a++;
    if (int8_t(c) < 0)
        a--;
    *kind = c;
    return a;
}

// Ver_OlDInst: the same, the old way (after the name)
const uint8_t *amos::verif::old_definition(const uint8_t *table, uint16_t token, uint8_t *kind)
{
    const uint8_t *a = table + token + 4;
    while (int8_t(*a++) >= 0) {
    }
    uint8_t c = *a++;
    if (int8_t(c) < 0)
        a--;
    *kind = c;
    return a;
}

namespace {

// The parameters given: their types and separators, -1 after them
struct Given {
    uint8_t s[20];
    uint16_t n;
    void add(uint8_t c)
    {
        s[n] = c;
        s[n + 1] = 0xFF;
        if (++n >= 19)
            n--;
    }
};

// VerC: the parameters given against the definition (and its other
// forms, the instruction's token changed to the one that fits)
void compare(const Given &g, const uint8_t *def, uint8_t *instruction)
{
    if (!def)
        return;                                 // a procedure
    const uint8_t *a = g.s;
    if (int8_t(*def) < 0) {
        if (int8_t(*a) < 0)
            return;
    } else if (int8_t(*a) >= 0) {
        goto params;
    }
    goto next_form;
params:
    for (;;) {
        int8_t d = int8_t(*def++);
        if (d < 0)
            goto end_of_form;
        uint8_t t = *a++;
        if (d != '3') {
            if (d != '2')
                d = '0';
            if (t != uint8_t(d))
                type();
        }
        int8_t sep = int8_t(*a++);
        if (sep < 0) {
            if (int8_t(*def++) >= 0)
                goto next_form;
            return;
        }
        d = int8_t(*def++);
        if (d < 0)
            goto end_of_form;
        if (sep != d)
            goto next_form;
    }
next_form:
    while (int8_t(*def++) >= 0) {
    }
end_of_form:
    if (def[-1] != 0xFE)
        syntax();
    def += address(def) & 1;
    poke_w(instruction, uint16_t(address(def) - data().VerBase));
    def += 4;
    while (int8_t(*def++) >= 0) {
    }
    a = g.s;
    if (*def++ == 'V')
        def++;
    goto params;
}

// Ope_CheckType: numbers all '0' (the flags of the maths used set)
uint8_t check_type(uint8_t t)
{
    IData &d = data();
    switch (uint8_t(t - '0')) {
    case 1:                     // float
        d.MathFlags |= 1;
        return '0';
    case 4:                     // integer or float
        return '0';
    case 5:                     // an angle
        d.MathFlags |= 3;
        return '0';
    default:                    // integer, string
        return t;
    }
}

uint8_t operand();

// V1_FVariable: a variable (an array with its indices) in an expression;
// its type
uint8_t variable_operand()
{
    pc[3] &= 0x0F;
    Name n = name();
    uint8_t *e;
    if (word(n.after) != tk::Par1) {
        store(&e);
        return n.type;
    }
    pc[3] |= 0x40;
    if (store(&e))
        fail(ENotDimensioned);
    if (e[4] != uint8_t(dimensions()))
        fail(EIllegalParameters);
    return n.type;
}

// =Varptr(variable)
uint8_t varptr()
{
    if (next_word() != tk::Par1 || next_word() != tk::Var)
        syntax();
    variable_operand();
    if (next_word() != tk::Par2)
        syntax();
    return '0';
}

// =Fn name(parameters): a function defined before
uint8_t fn()
{
    IData &d = data();
    if (next_word() != tk::Var)
        syntax();
    pc[3] = uint8_t((pc[3] & 0x0F) | (1 << 3));
    uint8_t t = name().type;
    uint8_t *e;
    if (store(&e))
        fail(EUserFunction);
    if (word(pc) == tk::Par1) {
        pc += 2;
        uint16_t parenth = d.Parenth;
        for (;;) {
            evaluate();
            if (d.Parenth)
                break;
            if (next_word() != tk::Vir)
                syntax();
        }
        if (d.Parenth != 0xFFFF)
            syntax();
        d.Parenth = parenth;
    }
    return t;
}

// =X Menu(,,) / =Y Menu(,,) / =Menu(,,)
uint8_t menu_function()
{
    if (dimensions() >= 8)          // MnNDim
        syntax();
    return '0';
}

} // namespace

// VStru: Struc(address,"equate"): the type of the equate
uint8_t amos::verif::structure()
{
    uint8_t *at = pc;
    pc += 6;
    if (next_word() != tk::Par1)
        syntax();
    if (expression() != '0')
        fail(EType);
    if (next_word() != tk::Vir)
        syntax();
    equate(ver_equ_nul, at);
    uint8_t t = at[4];
    if (next_word() != tk::Par2)
        syntax();
    return t;
}

namespace {

// =Equ("equate") / =Lvo("function")
uint8_t equate_function(const uint8_t *header)
{
    uint8_t *at = pc;
    pc += 6;
    if (next_word() != tk::Par1)
        syntax();
    equate(header, at);
    if (next_word() != tk::Par2)
        syntax();
    return '0';
}

// =Struc(,) / =Struc$(,)
uint8_t structure_function(bool string)
{
    IData &d = data();
    uint16_t parenth = d.Parenth;
    uint8_t t = structure();
    d.Parenth = parenth;
    if (string ? t != 6 : t >= 7)
        fail(EType);
    return string ? '2' : '0';
}

// Ope_Extension: an instruction of an extension: its slot, the number of
// parameters poked (-1 for a 2.0 library)
uint8_t extension_operand()
{
    IData &d = data();
    int8_t slot = int8_t(*pc++);
    uint8_t *count = pc;
    pc++;
    uint16_t token = next_word();
    if (!d.AdTokens[slot])
        fail(ENoExtension);
    const uint8_t *table = ptr<uint8_t>(d.AdTokens[slot]);
    bool v20 = table[LB_Flags] & (1 << LBF_20);
    d.VerBase = address(table);
    uint8_t kind;
    const uint8_t *def = old_definition(table, token, &kind);
    if (kind == 'I')
        syntax();
    uint8_t t = kind;
    if (kind == 'V')
        t = *def++;
    uint16_t n = function(def);
    *count = v20 ? 0xFF : uint8_t(n);
    return check_type(t);
}

// Ope_InstFonction: an instruction token where a function is wanted: its
// function form
uint8_t instruction_function(uint16_t token)
{
    IData &d = data();
    d.VerBase = data().AdTokens[0];
    const uint8_t *a = tokens() + token;
    for (;;) {
        const uint8_t *entry = a;
        a += 4;
        while (int8_t(*a++) >= 0) {
        }
        uint8_t t = *a++;
        if (t != 'I') {
            poke_w(pc - 2, uint16_t(address(entry) - d.VerBase));
            function(a);
            return check_type(t);
        }
        while (int8_t(*a++) >= 0) {
        }
        if (a[-1] != 0xFD)
            syntax();
        a += address(a) & 1;
    }
}

// Ope_MinMax: =Min(a,b) / =Max(a,b), both of the same type
uint8_t min_max()
{
    IData &d = data();
    if (next_word() != tk::Par1)
        syntax();
    uint16_t parenth = d.Parenth;
    uint8_t first = expression();
    if (next_word() != tk::Vir)
        syntax();
    uint8_t second = evaluate();
    if (d.Parenth != 0xFFFF)
        syntax();
    if (first != second)
        type();
    d.Parenth = parenth;
    return second;
}

// Ope_Match / Ope_Array: a function of an array
uint8_t array_function(const uint8_t *table, uint16_t token, bool pro)
{
    if (pro)
        not_13();
    const uint8_t *at = pc;
    data().VerBase = address(table);
    uint8_t kind;
    function(definition(table, token, &kind));
    // (the type is a number, whatever the function)
    if (at[5 + 2] & (1 << 6))
        return check_type('0');
    syntax();
}

// Ver_Operande: an operand; its type
uint8_t operand()
{
    IData &d = data();
    bool sign = false;
    for (;;) {
        uint16_t token = next_word();
        if (!token) {
            // the end: a parameter left out, after a comma
            if (word(pc - 4) != tk::Vir)
                syntax();
            pc -= 2;
            if (sign)
                syntax();
            return '0';
        }
        if (int16_t(token) < 0) {
            if (token != tk::M || sign)
                syntax();
            sign = true;
            continue;
        }
        const uint8_t *table = tokens();
        uint8_t kind;
        switch (table[token + 1]) {
        case 0x13:                      // a maths function
            d.MathFlags |= 3;
            [[fallthrough]];
        case 0x00: case 0x05: case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F: case 0x21: case 0x22:
        case 0x23: case 0x24: case 0x25: case 0x26: case 0x27: {
            d.VerBase = address(table);
            const uint8_t *def = definition(table, token, &kind);
            function(def);
            return check_type(kind);
        }
        case 0x01:
            syntax();
        case 0x02:                      // the end of the expression
        case 0x03:                      // ended by a comma: left out
            if (table[token + 1] == 0x02 && word(pc - 4) != tk::Vir)
                syntax();
            pc -= 2;
            if (sign)
                syntax();
            return '0';
        case 0x04:                      // (
            d.Parenth++;
            return revalue();
        case 0x06:
            return extension_operand();
        case 0x07:
            return variable_operand();
        case 0x08:
            return varptr();
        case 0x09:
            return fn();
        case 0x0A: {                    // Not
            uint16_t parenth = d.Parenth;
            uint8_t t = evaluate();
            if (d.Parenth)
                syntax();
            d.Parenth = parenth;
            if (t == '2')
                type();
            return t;
        }
        case 0x0B:
            return menu_function();
        case 0x0C:
            return equate_function(ver_equ_nul);
        case 0x0D:
            return array_function(table, token, false);
        case 0x0E:
            return array_function(table, token, true);
        case 0x0F: case 0x20:
            return min_max();
        case 0x10:
            return equate_function(ver_equ_lvo);
        case 0x11:
            return structure_function(false);
        case 0x12:
            return structure_function(true);
        case 0x14:                      // an integer constant
            pc += 4;
            return '0';
        case 0x15:                      // a float
            pc += 4;
            d.Ver_SPConst = 1;
            d.MathFlags |= 1;
            return '0';
        case 0x16:                      // a double
            pc += 8;
            d.Ver_DPConst = 1;
            d.MathFlags |= 1;
            not_13();
            return '0';
        case 0x17: {                    // a string
            uint16_t n = next_word();
            pc += n + (n & 1);
            return '2';
        }
        case 0x18:
            return instruction_function(token);
        case 0x19: {                    // already tested
            const uint8_t *def = definition(table, token, &kind);
            uint8_t t = kind;
            const uint8_t *a = def;
            if (int8_t(*a++) >= 0) {
                uint16_t parenth = d.Parenth;
                for (;;) {
                    pc += 2;
                    evaluate();
                    if (int8_t(*a++) < 0)
                        break;
                    a++;
                }
                d.Parenth = parenth;
            }
            return check_type(t);
        }
        case 0x1A: {                    // a reserved variable
            d.VerBase = address(table);
            const uint8_t *def = definition(table, token, &kind);
            uint8_t t = *def++;
            function(def);
            return check_type(t);
        }
        default:
            // (beyond the assembler's table)
            syntax();
        }
    }
}

// Tst_*: the types on both sides of an operator
uint8_t operation(uint16_t op, uint8_t left, uint8_t right)
{
    switch (peek_w(ver_operators + int16_t(op) + 2)) {
    case Comparison:
        if (left != right)
            type();
        return '0';
    case Mixed:
        if (left != right)
            type();
        return right;
    case Power:
        data().MathFlags |= 3;
        [[fallthrough]];
    default:
        if (left != right || right != '0')
            type();
        return right;
    }
}

} // namespace

namespace amos::verif {

// Ver_REvalue: the operators by priority (their tokens: the higher, the
// first); the end of the expression, or the parenthesis closing it
uint8_t revalue()
{
    IData &d = data();
    struct Pending {
        uint16_t op;
        uint8_t left;
    } stack[24];
    int n = 0;
    uint16_t top = 0x7FFF;
    uint8_t t;
    for (;;) {
        uint32_t base = d.VerBase;
        t = operand();
        d.VerBase = base;
        for (;;) {
            uint16_t token = next_word();
            if (token > top) {
                if (n == int(sizeof(stack) / sizeof(stack[0])))
                    syntax();
                stack[n++] = {top, t};
                top = token;
                break;
            }
            pc -= 2;
            last = token;
            if (!(top & 0x8000)) {
                if (token == tk::Par2) {
                    d.Parenth--;
                    pc += 2;
                }
                return t;
            }
            t = operation(top, stack[n - 1].left, t);
            top = stack[--n].op;
        }
    }
}

uint16_t last_token() { return last; }

uint8_t evaluate()
{
    data().Parenth = 0;
    return revalue();
}

uint8_t expression()
{
    IData &d = data();
    d.VerPos = address(pc);
    uint8_t t = evaluate();
    if (d.Parenth)
        syntax();
    return t;
}

// VerI: the parameters of an instruction, up to the end of it
uint16_t instruction(const uint8_t *def)
{
    IData &d = data();
    uint8_t *at = pc - 2;
    Given g = {{0xFF}, 0};
    if (!finished(word(pc))) {
        for (;;) {
            g.add(evaluate());
            if (d.Parenth)
                syntax();
            uint16_t w = word(pc);
            uint8_t sep;
            if (w == tk::Vir)
                sep = ',';
            else if (w == tk::To)
                sep = 't';
            else
                break;
            pc += 2;
            g.add(sep);
        }
    }
    compare(g, def, at);
    return uint16_t((g.n + 1) >> 1);
}

// VerF: the parameters of a function, in parentheses
uint16_t function(const uint8_t *def)
{
    IData &d = data();
    uint16_t parenth = d.Parenth;
    uint8_t *at = pc - 2;
    Given g = {{0xFF}, 0};
    bool params = false;
    if (word(pc) == tk::Par1) {
        pc += 2;
        if (next_word() != tk::Par2) {
            pc -= 2;
            params = true;
        }
    }
    while (params) {
        g.add(evaluate());
        if (d.Parenth == 0xFFFF)
            break;
        if (d.Parenth)
            syntax();
        uint16_t w = next_word();
        uint8_t sep;
        if (w == tk::Vir)
            sep = ',';
        else if (w == tk::To)
            sep = 't';
        else
            syntax();
        g.add(sep);
    }
    compare(g, def, at);
    d.Parenth = parenth;
    return uint16_t((g.n + 1) >> 1);
}

// VerI_DejaTestee: an instruction tested already (no other forms)
void instruction_tested(const uint8_t *a)
{
    if (int8_t(*a++) >= 0) {
        for (;;) {
            evaluate();
            if (int8_t(*a++) < 0)
                break;
            a++;
            pc += 2;
        }
    }
}

} // namespace amos::verif
