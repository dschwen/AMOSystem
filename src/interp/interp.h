// The interpreter's core (+ILib.s): expressions, and what the rest of the
// interpreter shares. The program pointer is ctx().a6 (pc()): the
// assembler that calls C++ hands it over in the context, and gets back
// where the C++ left it.
#pragma once

#include "core/amos.h"
#include "runtime/detail/identity_data_memory.h"
#include "core/source.h"
#include "lib/strings.h"

namespace amos::interp {

// A value as the assembler holds it: d2 its type (the low byte: 0 an
// integer, 1 a float, 2 a string: the address of its length word), d3 the
// value (a float's high half), d4 a double's low half
struct Value {
    uint32_t type, v1, v2;
};

constexpr uint8_t TInt = 0, TFloat = 1, TString = 2;
inline uint8_t kind(const Value &v) { return uint8_t(v.type); }

// Run-time errors raised in several places
enum : uint32_t {
    EOofBuf = 11, EDByZero = 20, ENonDim = 27, EAlrDim = 28, EOverFlow = 29, EOofData = 33, EInpTL = 79 + 20,
};

inline ContextCursor &pc() { return ctx().a6; }
// The program is walked by the source module on its native view of it
inline ContextCursor skip_token(uint16_t t, ContextCursor at)
{
    return host::detail::capture_cursor(amos::source::skip_token(t, ptr<uint8_t>(encode_context_cursor(at))));
}
inline uint8_t byte_at(ContextCursor at, int offset = 0) { return host::detail::read_cursor_byte(at + offset); }
inline uint8_t next_cursor_byte(ContextCursor &at) { uint8_t b = byte_at(at); at += 1; return b; }
inline uint8_t next_byte() { uint8_t b = host::detail::read_cursor_byte(pc()); pc() += 1; return b; }
inline uint16_t peek_token() { return peek_w(pc()); }
inline uint16_t next_token()
{
    uint16_t t = peek_w(pc());
    pc() += 2;
    return t;
}

// The work of the parameter collector a routine starts with (params.cpp):
// the parameters evaluated, all but the last one on the a3 stack. The body
// to call (past the collector's call; a maths function's float routine)
// gets the last one in d2/d3/d4 (if has_value).
struct Collected {
    host::RoutineArg body;
    Value last;
    bool has_value;
};
using TokenTable = guest::GuestPtr<const guest::Byte>;
Collected collect(TokenTable table, uint16_t t, host::RoutineArg routine, bool function);
int collector_of(host::RoutineArg routine);      // its number (0-39), or -1

// New_Evalue: the expression at pc; pc at the token after it (past a
// closing parenthesis). *end gets that token.
Value evaluate(uint16_t *end = nullptr);
// Fn_New_Evalue: the same after a token (an opening parenthesis, a comma)
Value evaluate_after();
// New_Expentier: an integer (a float made one), or a string
Value integer();

// Constants in the program, at pc (FnCEntier, FnCstFl, FnCstDFl, FnCstCh)
Value integer_constant();
Value float_constant();
Value double_constant();
Value string_constant();

// The float layer of AMOSPro.Lib (L_IntToFl1, L_FlToInt1... swapped for
// double precision)
Value to_float(const Value &v);
Value to_integer(const Value &v);
// FFP2Ieee, Ieee2FFP, Sp2Dp, Dp2Sp (interp/float.cpp)
uint32_t ffp_to_ieee(uint32_t ffp);
uint32_t ieee_to_ffp(uint32_t single);
void single_to_double(uint32_t single, uint32_t *high, uint32_t *low);
uint32_t double_to_single(uint32_t high, uint32_t low);

// L_Float_Compare of a and b: -1 less, 0 equal, 1 greater
int float_compare(const Value &a, const Value &b);

// compch: two strings compared, -1 a < b, 0 equal, 1 a > b
int compare_strings(GuestString a, GuestString b);

// interp/vars.cpp
struct FoundVar {
    ContextCursor at;
    uint32_t type;
};
ContextCursor array_element(ContextCursor at);    // GetTablo
void assign();                          // InVar
Value value();                          // FnVar
FoundVar find_variable();               // FindVar

// interp/menus.cpp: MnDim
struct MenuDim {
    uint8_t *flags;
    MenuItem *item;     // none for a level alone
};
MenuDim menu_dim();

// interp/chrget.cpp: function t (its routine r) called directly, if it is
// a C++ one; v its value
bool native_function(uint16_t t, host::RoutineArg r, Value &v);

// What C++ hands back to the assembler that called it (eval_glue.s):
// d0-d4, a0
struct EvalOut {
    uint32_t d0, d1, d2, d3, d4, a0;
};

} // namespace amos::interp
