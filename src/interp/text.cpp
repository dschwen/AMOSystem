// Strings of +ILib.s: Left$, Right$, Mid$ as functions and as
// instructions (Left$(a$,n)=b$ writes into a$), =Val, =Resource$.
//
// A part is a start (1 = the first character; 0 counts as 1) and a length;
// past the end it is cut, a negative start or length is an error.

#include "interp/interp.h"

#include "lib/strings.h"

using namespace amos;
using namespace amos::interp;

namespace {

constexpr uint32_t All = 0xFFFF;

// An integer or string parameter after a token: Fn_New_Expentier
Value parameter()
{
    pc() += 2;
    return integer();
}

// RFnMid: the part of string s (length 0: the empty string)
uint32_t part(const uint8_t *s, int32_t start, int32_t length)
{
    uint32_t size = peek_w(s);
    if (start < 0)
        fon_call();
    if (start)
        start--;
    if (uint32_t(start) >= size || !length)
        return idata().ChVide;
    if (length < 0)
        fon_call();
    uint32_t end = uint32_t(start) + uint32_t(length);
    if (end > size)
        end = size;
    return str_from(s + 2 + start, uint16_t(end - start));
}

// The string parameter and the first number of =Left$(a$,n)...
const uint8_t *string_parameter() { return ptr<uint8_t>(parameter().v1); }

// RInMid: the string variable between parentheses, given a copy of its
// own (it may share its characters); its characters and length
struct Target {
    uint8_t *chars;
    uint32_t length;
};

Target target()
{
    pc() += 2;
    uint8_t *at = find_variable().at;
    const uint8_t *s = ptr<uint8_t>(peek_l(at));
    uint32_t length = peek_w(s);
    if (!length)
        return {const_cast<uint8_t *>(s) + 2, 0};
    Regs r;
    r.d[3] = length;
    lib_call(L_Demande, r);
    uint8_t *copy = ptr<uint8_t>(r.a[1]);
    poke_w(copy, uint16_t(length));
    // in longs, as the original
    uint8_t *to = copy + 2;
    const uint8_t *from = s + 2;
    for (uint32_t i = 0; i < ((length - 1) & 0xFFFF) / 4 + 1; i++, to += 4, from += 4)
        poke_l(to, peek_l(from));
    idata().HiChaine = address(to);
    poke_l(at, r.a[0]);
    return {ptr<uint8_t>(r.a[0]) + 2, length};
}

// RInMid2: = b$, written over the part of the target
void write_part(const Target &t, int32_t start, int32_t length)
{
    const uint8_t *s = ptr<uint8_t>(parameter().v1);
    uint32_t size = peek_w(s);
    if (start < 0)
        fon_call();
    if (start)
        start--;
    if (uint32_t(start) >= t.length)
        return;
    if (length < 0)
        fon_call();
    if (!length)
        return;
    uint32_t end = uint32_t(start) + uint32_t(length);
    if (end > t.length)
        end = t.length;
    uint32_t n = end - start;
    if (n > size)
        n = size;
    for (uint32_t i = 0; i < n; i++)
        t.chars[start + i] = s[2 + i];
}

} // namespace

extern "C" {

void interp_fn_left(EvalOut *o)
{
    const uint8_t *s = string_parameter();
    int32_t n = int32_t(parameter().v1);
    *o = {0, 0, TString, part(s, 0, n), 0, 0};
}

void interp_fn_right(EvalOut *o)
{
    const uint8_t *s = string_parameter();
    int32_t n = int32_t(parameter().v1);
    if (n < 0)
        fon_call();
    uint32_t size = peek_w(s);
    if (uint32_t(n) >= size)
        n = int32_t(size);
    *o = {0, 0, TString, part(s, int32_t(size) - n + 1, All), 0, 0};
}

void interp_fn_mid2(EvalOut *o)
{
    const uint8_t *s = string_parameter();
    int32_t start = int32_t(parameter().v1);
    *o = {0, 0, TString, part(s, start, All), 0, 0};
}

void interp_fn_mid3(EvalOut *o)
{
    const uint8_t *s = string_parameter();
    int32_t start = int32_t(parameter().v1);
    int32_t n = int32_t(parameter().v1);
    *o = {0, 0, TString, part(s, start, n), 0, 0};
}

void interp_in_left()
{
    Target t = target();
    int32_t n = int32_t(parameter().v1);
    write_part(t, 0, n);
}

void interp_in_right()
{
    Target t = target();
    int32_t n = int32_t(parameter().v1);
    if (n < 0)
        fon_call();
    int32_t start = uint32_t(n) >= t.length ? 0 : int32_t(t.length) - n + 1;
    write_part(t, start, n);
}

void interp_in_mid2()
{
    Target t = target();
    int32_t start = int32_t(parameter().v1);
    write_part(t, start, All);
}

void interp_in_mid3()
{
    Target t = target();
    int32_t start = int32_t(parameter().v1);
    int32_t n = int32_t(parameter().v1);
    write_part(t, start, n);
}

// =Val(a$): the number at the start of the string (d3 = a$)
void interp_fn_val(EvalOut *o)
{
    const uint8_t *s = ptr<uint8_t>(o->d3);
    if (!peek_w(s)) {
        *o = {0, 0, TInt, 0, 0, 0};
        return;
    }
    Regs r;
    r.a[0] = address(str_to_buffer(s));
    r.d[0] = 1;                 // with a sign
    lib_call(L_ValRout, r);
    *o = {r.d[0], r.d[1], r.d[2], r.d[3], r.d[4], r.a[0]};
}

// =Resource$(n): message n of the resource bank; 0 the system's path;
// -1 to -1000 a system message, -1001... -6000 the editor's messages
void interp_fn_resource(EvalOut *o)
{
    IData &d = idata();
    int32_t n = int32_t(o->d3);
    const char *text;
    Regs m;
    if (n > 0) {
        Regs p;
        lib_call(L_Dia_GetPuzzle, p);
        m.a[0] = p.a[2];
        m.d[0] = uint32_t(n);
        m.a[0] = address(boot::get_message(ptr<uint8_t>(m.a[0]), m.d[0]));
        text = ptr<char>(m.a[0]);
    } else if (!n) {
        text = reinterpret_cast<const char *>(d.Sys_Pathname);
    } else {
        static const int Tables = 6;
        uint32_t tables[Tables] = {d.Sys_Messages, d.Ed_Systeme, d.EdM_Messages, d.Ed_Messages, d.Ed_TstMessages,
                                   d.Ed_RunMessages};
        uint32_t k = uint32_t(-n);
        int i = 0;
        while (k >= 1001) {
            if (++i == Tables)
                fon_call();
            k -= 1000;
        }
        m.a[0] = tables[i];
        m.d[0] = k;
        m.a[0] = address(boot::get_message(ptr<uint8_t>(m.a[0]), m.d[0]));
        text = ptr<char>(m.a[0]);
    }
    *o = {0, 0, TString, str_from_cstr(text), 0, 0};
}
}
