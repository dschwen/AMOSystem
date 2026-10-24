// Input, Line Input, Input #n, Line Input #n (+ILib.s).
//
// A line is read into Buffer (from the keyboard with the line editor, or
// from the file up to the separator or the end of the line), then given to
// the variables: each number or string up to a comma (Line Input: the
// whole line). A missing item asks again ("?? "); a bad number starts the
// whole Input again ("Redo from start"; in a file a type mismatch).

#include "interp/interp.h"

#include "core/amiga.h"

using namespace amos;
using namespace amos::interp;

namespace {

constexpr int LEdFCursor = 2, LEdFTests = 5, LEdFMulti = 6;      // LEd_F*

void window_print(const char *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

// Reads the next line: its length
uint32_t read_line(uint8_t separator)
{
    IData &d = idata();
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    *buffer = 0;
    if (d.PrintFile) {
        uint8_t eol1 = reinterpret_cast<uint8_t *>(&d.ChrInp)[1];
        int8_t eol2 = int8_t(reinterpret_cast<uint8_t *>(&d.ChrInp)[0]);
        uint8_t *to = buffer;
        uint32_t n = 0;
        for (;;) {
            Regs r;
            r.a[2] = d.PrintFile;
            lib_call(L_GetByte, r);
            uint8_t c = uint8_t(r.d[0]);
            if (c == separator)
                break;
            if (c == eol1) {
                if (eol2 >= 0) {
                    Regs s;
                    s.a[2] = d.PrintFile;
                    lib_call(L_GetByte, s);
                }
                break;
            }
            *to++ = c;
            if (++n >= 1000)
                lib_jump(L_InpTL, 0);
        }
        *to = 0;
        return n;
    }
    Regs r;
    r.a[0] = address(d.Es_LEd);
    r.a[1] = r.a[2] = d.Buffer;
    r.d[0] = (1 << LEdFCursor) | (1 << LEdFTests) | (1 << LEdFMulti);
    r.d[1] = 0;
    r.d[2] = 256;
    r.d[3] = uint32_t(-1);
    lib_call(L_LEd_Init, r);
    if (r.d[0])
        fon_call();
    Regs l;
    l.a[0] = address(d.Es_LEd);
    lib_call(L_LEd_Loop, l);
    if (int16_t(l.d[2]) < 0) {
        // (as the original: the length goes into T_Actualise)
        wdata().Actualise = uint16_t(l.d[0]);
        error(9);
    }
    return l.d[0];
}

// The prompt: the string given, or "? "
void prompt()
{
    if (peek_token() == tk::Var) {
        window_print("? ");
        poke_l(idata().DeFloat, 0);
        return;
    }
    const uint8_t *s = ptr<uint8_t>(evaluate().v1);
    pc() += 2;
    uint16_t length = peek_w(s);
    if (length) {
        // zero-terminated for a moment
        uint8_t *end = const_cast<uint8_t *>(s) + 2 + length;
        uint8_t keep = *end;
        *end = 0;
        window_print(reinterpret_cast<const char *>(s + 2));
        *end = keep;
    }
}

enum class Outcome { Done, Redo };

// The variables at pc from the lines read
Outcome fill(uint8_t separator, uint32_t length)
{
    IData &d = idata();
    const uint8_t *in = ptr<uint8_t>(d.Buffer);
    for (;;) {
        FoundVar var = find_variable();
        uint8_t type = uint8_t(var.type);
        if (type == TString) {
            poke_l(var.at, d.ChVide);
            Regs r;
            r.d[3] = length;
            lib_call(L_DDemande, r);
            uint8_t *s = ptr<uint8_t>(r.a[1]);
            uint8_t *to = s + 2;
            uint8_t c;
            do {
                c = *in++;
                *to++ = c;
            } while (c && c != separator);
            to--;
            in--;
            uint32_t n = to - s - 2;
            poke_w(s, uint16_t(n));
            d.HiChaine = address(to) + (n & 1);
            poke_l(var.at, address(s));
        } else {
            Regs r;
            r.a[0] = address(in);
            r.d[0] = 1;
            lib_call(L_ValRout, r);
            in = ptr<uint8_t>(r.a[0]);
            if (*in && *in != separator)
                return Outcome::Redo;
            Value v = {r.d[2], r.d[3], r.d[4]};
            if (kind(v) != type)
                v = type ? to_float(v) : to_integer(v);
            poke_l(var.at, v.v1);
            if (kind(v) == TFloat && int8_t(d.MathFlags) < 0)
                poke_l(var.at + 4, v.v2);
        }
        if (next_token() != tk::Vir) {
            pc() -= 2;
            return Outcome::Done;
        }
        if (*in++ == ',')
            continue;
        if (!d.PrintFile) {
            window_print("\r\n?? ");
            *ptr<uint8_t>(d.Buffer) = 0;
        }
        length = read_line(separator);
        in = ptr<uint8_t>(d.Buffer);
    }
}

void input(uint8_t separator, bool file)
{
    IData &d = idata();
    for (;;) {
        if (!file) {
            poke_l(d.DeFloat, 0);
            d.PrintFile = 0;
            if (!d.ScOn)
                lib_jump(L_ScNOp, 0);
            *ptr<uint8_t>(d.Buffer) = 0;
            prompt();
        }
        uint32_t length = read_line(separator);
        Outcome o = fill(separator, length);
        Regs e;
        lib_call(L_EndByte, e);
        if (o == Outcome::Done)
            break;
        if (d.PrintFile)
            lib_jump(L_TypeMis, 0);
        window_print("\r\n");
        Regs m;
        m.d[0] = 15;
        lib_call(L_Def_GetMessage, m);
        window_print(ptr<char>(m.a[0]));
        window_print("\r\n");
        pc() = ptr<uint8_t>(ctx().d7);
        file = false;
    }
    if (!d.PrintFile) {
        if (next_token() != tk::PVir) {
            pc() -= 2;
            window_print("\r\n");
        }
    }
}

// Input #n: the file
void input_file(uint8_t separator)
{
    IData &d = idata();
    Value n = integer();
    pc() += 2;
    Regs r;
    r.d[0] = n.v1;
    lib_call(L_GetFile, r);
    if (!r.d[1])
        error(97);
    d.PrintFile = r.a[2];
    poke_l(d.DeFloat, 0);
    input(separator, true);
}

} // namespace

extern "C" {
void interp_in_input() { input(',', false); }
void interp_in_line_input() { input(0, false); }
void interp_in_input_h() { input_file(','); }
void interp_in_line_input_h() { input_file(0); }
}
