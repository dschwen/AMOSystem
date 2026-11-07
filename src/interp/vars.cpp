// Variables (+ILib.s): assignment (InVar), a variable as an operand
// (FnVar), the address of a variable (FindVar) or of an array's element
// (GetTablo).
//
// In the program a variable is its token, then the offset of its slot
// (>= 0: from VarLoc, the procedure's; < 0: from VarGlo, global), the
// length of its name, its flags (bits 0-3 the type, bit 6 an array) and
// the name. A slot is a length byte (4, or 8 for a double), the flags,
// then the value; an array's value is the address of its block: the number
// of dimensions, the shift of an element's size, a limit and a multiplier
// word for each dimension, then the elements.

#include "interp/interp.h"

using namespace amos;

namespace amos::interp {

namespace {

constexpr uint8_t FArray = 1 << 6;

bool doubles() { return int8_t(idata().MathFlags) < 0; }

struct Variable {
    uint8_t *at;        // the value
    uint8_t flags;
    uint16_t offset;    // (d0 of the assembler)
    uint8_t name;       // the name's length (d1)
};

// The variable at pc (past its token), pc past it
Variable variable()
{
    IData &d = idata();
    uint16_t offset = next_token();
    uint8_t *at;
    if (int16_t(offset) < 0) {
        offset = uint16_t(-int16_t(offset));
        at = ptr<uint8_t>(d.VarGlo) + 1 + int16_t(offset);
    } else {
        at = ptr<uint8_t>(d.VarLoc) + 2 + int16_t(offset);
    }
    uint8_t length = next_byte();
    uint8_t flags = next_byte();
    pc() += length;
    return {at, flags, offset, length};
}

// v made of the type: an integer becomes a float, anything else an integer
Value converted(Value v, uint8_t type)
{
    if (kind(v) == type)
        return v;
    return kind(v) == TInt ? to_float(v) : to_integer(v);
}

void store(uint8_t *at, const Value &v)
{
    poke_l(at, v.v1);
    if (kind(v) == TFloat && doubles())
        poke_l(at + 4, v.v2);
}

} // namespace

// GetTablo: the element of the array whose slot is at, its indices in
// parentheses at pc
uint8_t *array_element(uint8_t *at)
{
    uint8_t *block = ptr<uint8_t>(peek_l(at));
    if (!block)
        error(ENonDim);
    uint16_t dimensions = block[0];
    uint8_t shift = block[1];
    uint8_t *limits = block + 2;
    uint32_t index = 0;
    for (;;) {
        Value i = evaluate_after();
        if (kind(i))
            i = to_integer(i);
        if (i.v1 > peek_w(limits))
            fon_call();
        if (!--dimensions)
            return limits + 4 + ((index + i.v1) << shift);
        index += (i.v1 & 0xFFFF) * peek_w(limits + 2);
        limits += 4;
    }
}

// InVar: Var = expression. A simple variable gets its length and flags
// written; the value is made of the variable's type (by the whole flags
// byte, as the original).
void assign()
{
    Variable v = variable();
    if (v.flags & FArray) {
        uint8_t *at = array_element(v.at);
        Value x = converted(evaluate_after(), v.flags & 0x0F);
        store(at, x);
        return;
    }
    Value x = converted(evaluate_after(), v.flags);
    v.at[-2] = kind(x) == TFloat && doubles() ? 8 : 4;
    v.at[-1] = v.flags;
    store(v.at, x);
}

// FnVar: the value of the variable at pc. An empty string variable gets
// the empty string.
Value value()
{
    Variable v = variable();
    uint8_t *at = v.flags & FArray ? array_element(v.at) : v.at;
    uint8_t type = v.flags & 0x0F;
    Value x = {type, peek_l(at), 0};
    if (type == TString) {
        if (!x.v1) {
            x.v1 = idata().ChVide;
            poke_w(at - 2, 0x0402);
            poke_l(at, x.v1);
        }
    } else if (type != TInt && doubles()) {
        x.v2 = peek_l(at + 4);
    }
    return x;
}

// FindVar: the address of the variable after the token at pc (a simple
// one made ready: its length and flags written, a string set), its type
FoundVar find_variable()
{
    IData &d = idata();
    pc() += 2;
    Variable v = variable();
    if (v.flags & FArray) {
        uint8_t *at = array_element(v.at);
        uint8_t type = v.flags & 0x0F;
        if (type == TString && !peek_l(at))
            poke_l(at, d.ChVide);
        return {at, type};
    }
    v.at[-2] = 4;
    v.at[-1] = v.flags;
    if (v.flags == TFloat) {
        if (doubles())
            v.at[-2] = 8;
    } else if (v.flags != TInt && !peek_l(v.at)) {
        poke_l(v.at, d.ChVide);
    }
    return {v.at, v.flags};
}

// Dim: the arrays at pc, each with its limits (below 65535, and fewer than
// 65536 elements but in the last dimension)
void dim()
{
    IData &d = idata();
    do {
        pc() += 2;
        Variable v = variable();
        if (peek_l(v.at))
            error(EAlrDim);
        uint8_t type = v.flags & 0x0F;
        uint8_t shift = type == TFloat && doubles() ? 3 : 2;
        uint16_t limits[255];
        uint16_t dimensions = 0;
        uint32_t product = 1, count;
        for (;;) {
            uint16_t end;
            pc() += 2;
            Value x = evaluate(&end);
            if (kind(x))
                x = to_integer(x);
            // (the dimensions' number is a byte: more would not fit)
            if (x.v1 >= 0xFFFF || dimensions == 255)
                fon_call();
            limits[dimensions++] = uint16_t(x.v1);
            count = (product & 0xFFFF) * ((x.v1 + 1) & 0xFFFF);
            if (end == tk::Par2)
                break;
            if (count >= 0x10000)
                fon_call();
            product = count;
            if (peek_token() != tk::Vir)
                error(22);
        }
        uint32_t bytes = count << shift;
        uint32_t header = dimensions * 4 + 2;
        uint8_t *block = ptr<uint8_t>(d.TabBas - bytes - header);
        if (address(block) < d.HiChaine) {
            Regs m;
            lib_call(L_Menage, m);
            if (address(block) < d.HiChaine)
                error(EOofBuf);
        }
        poke_l(v.at, address(block));
        d.TabBas = address(block);
        block[0] = uint8_t(dimensions);
        block[1] = shift;
        uint8_t *elements = block + header;
        uint8_t *p = elements;
        uint32_t multiplier = 1;
        for (int i = dimensions - 1; i >= 0; i--) {
            p -= 2;
            poke_w(p, uint16_t(multiplier));
            p -= 2;
            poke_w(p, limits[i]);
            multiplier = (multiplier & 0xFFFF) * uint32_t(limits[i] + 1);
        }
        poke_w(v.at - 2, type | 0x0440);
        uint32_t fill = type == TString ? d.ChVide : 0;
        uint32_t longs = bytes >> 2;
        do
            poke_l(elements, fill), elements += 4;
        while (--longs);
    } while (next_token() == tk::Vir);
    pc() -= 2;
}

// =Varptr(variable): the address of its value, or of a string's characters
uint32_t varptr()
{
    pc() += 2;
    FoundVar f = find_variable();
    pc() += 2;
    return uint8_t(f.type) == TString ? peek_l(f.at) + 2 : address(f.at);
}

// =Array(a(0...)): the address of the array's block (the indices are
// evaluated, and not used)
uint32_t array_address()
{
    pc() += 4;
    Variable v = variable();
    uint32_t block = peek_l(v.at);
    if (!block)
        error(ENonDim);
    pc() -= 2;
    for (uint8_t n = ptr<uint8_t>(block)[0]; n; n--)
        evaluate_after();
    return block;
}

// Swap a,b
void swap()
{
    uint8_t *a = find_variable().at;
    pc() += 2;
    FoundVar b = find_variable();
    uint32_t t = peek_l(b.at);
    poke_l(b.at, peek_l(a));
    poke_l(a, t);
    if (uint8_t(b.type) == TFloat && doubles()) {
        t = peek_l(b.at + 4);
        poke_l(b.at + 4, peek_l(a + 4));
        poke_l(a + 4, t);
    }
}

// Inc / Dec
void increment(int32_t by)
{
    uint8_t *at = find_variable().at;
    poke_l(at, peek_l(at) + by);
}

// Add v,n
void add()
{
    uint8_t *at = find_variable().at;
    poke_l(at, peek_l(at) + evaluate_after().v1);
}

// Add v,n,low To high: past one end, the other one
void add_range()
{
    uint8_t *at = find_variable().at;
    int32_t p[3];
    for (int32_t &x : p) {
        Value v = evaluate_after();
        if (kind(v) == TFloat)
            v = to_integer(v);
        x = int32_t(v.v1);
    }
    int32_t n = int32_t(peek_l(at) + uint32_t(p[0]));
    poke_l(at, uint32_t(n < p[1] ? p[2] : n > p[2] ? p[1] : n));
}

// Def Fn: the function's variable points to its definition; the rest of
// the line is skipped
void define_function()
{
    uint8_t *at = find_variable().at;
    poke_l(at, encode_context_cursor(pc()));
    ContextCursor line = decode_context_cursor(ctx().d7 - 4);
    pc() = line - 2 + 2 * byte_at(line);
}

// =Fn f(parameters): the parameters given to the definition's variables,
// its expression evaluated
Value call_function()
{
    uint8_t *at = find_variable().at;
    ContextCursor def = decode_context_cursor(peek_l(at));
    if (!def)
        error(15);
    uint16_t t = peek_token();
    if (t == tk::Par1) {
        pc() += 2;
        do {
            if (peek_w(def) != t)
                error(16);
            def += 2;
            Value x = evaluate();
            ContextCursor back = pc();
            pc() = def;
            FoundVar f = find_variable();
            def = pc();
            pc() = back;
            uint8_t type = uint8_t(f.type);
            if (kind(x) != type) {
                if (type > TFloat)
                    error(34);
                if (type == TFloat) {
                    x = to_float(x);
                } else {
                    if (kind(x) > TFloat)
                        error(34);
                    x = to_integer(x);
                }
            }
            if (type == TFloat && doubles())
                poke_l(f.at + 4, x.v2);
            poke_l(f.at, x.v1);
        } while ((t = next_token()) == tk::Vir);
        pc() -= 2;
        if (peek_w(def) != tk::Par2)
            error(16);
        def += 2;
    }
    if (peek_w(def) != tk::Eg)
        error(16);
    def += 2;
    Ctx &c = ctx();
    ContextCursor back = c.a6;
    ContextCursor a3 = c.a3;
    c.a6 = def;
    Value v = evaluate();
    c.a6 = back;
    c.a3 = a3;
    return v;
}

// Shared / Global a,b(),...: nothing to do when running
void shared()
{
    do {
        pc() += 6 + int8_t(host::detail::read_cursor_byte(pc() + 4));
        if (peek_token() == tk::Par1)
            pc() += 4;
    } while (next_token() == tk::Vir);
    pc() -= 2;
}

} // namespace amos::interp

using namespace amos::interp;

extern "C" {

// InVar
void interp_in_var() { assign(); }

// FnVar: d2-d4 the value
void interp_fn_var(EvalOut *o)
{
    Value v = value();
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}

// FindVar: a0 the address, d2 the type
void interp_find_var(EvalOut *o)
{
    FoundVar f = find_variable();
    *o = {0, 0, f.type, 0, 0, address(f.at)};
}

void interp_in_dim() { dim(); }
void interp_in_swap() { swap(); }
void interp_in_inc() { increment(1); }
void interp_in_dec() { increment(-1); }
void interp_in_add2() { add(); }
void interp_in_add4() { add_range(); }
void interp_in_dfn() { define_function(); }
void interp_in_shared() { shared(); }
void interp_fn_varptr(EvalOut *o) { *o = {0, 0, TInt, varptr(), 0, 0}; }
void interp_fn_array(EvalOut *o) { *o = {0, 0, TInt, array_address(), 0, 0}; }
void interp_fn_fn(EvalOut *o)
{
    Value v = call_function();
    *o = {0, 0, v.type, v.v1, v.v2, 0};
}

// GetTablo: a0 the slot. Out: a0 the element.
void interp_get_tablo(EvalOut *o) { o->a0 = address(array_element(ptr<uint8_t>(o->a0))); }

// The same with registers (L_FindVar: d0-d2/a0; L_GetTablo: a0)
uint32_t interp_find_var_reg(RegFrame &r)
{
    EvalOut o;
    interp_find_var(&o);
    r.d[0] = o.d0;
    r.d[1] = o.d1;
    r.d[2] = o.d2;
    r.a[0] = o.a0;
    return r.d[0];
}
uint32_t interp_get_tablo_reg(RegFrame &r)
{
    r.a[0] = address(array_element(ptr<uint8_t>(r.a[0])));
    return r.d[0];
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_vars.inc"
#endif
