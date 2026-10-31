// The tokenizer and detokenizer of the editor (also used by the monitor
// and the interpreter through the editor's entry table): Tokenise turns
// a line of text into tokens, Detok back, TInst gives the size of a
// token's data.
//
// Written with the labels and register roles of the assembler (+Edit.s
// Tokenise, Detok), which it follows step by step: the token bytes it
// writes are those of .AMOS files.
//
// A line: a byte with its length / 2, a byte with its indentation + 1,
// the tokens, a 0 word. The tokens: a word (an offset in a token table, or
// one of the special tokens of amos::tk), then data that depends on it:
//   variables, labels, procedure calls, label calls: a 0 word, the length
//     of the name (even), a flag byte (1 float, 2 string), the name
//   strings: the length word, the characters (even)
//   numbers: a long (two for double precision)
//   Rem: the length word, the text (even)
//   extensions: the extension number byte, a 0 byte, the offset word
//   structure tokens (For, If...): 2 to 8 bytes the verifier fills in

#include "core/amos.h"
#include "core/source.h"

using namespace amos;

// +Edit.s: the operators' token table (their tokens are offsets from its
// end), the name of a missing extension's instructions (the letter
// written into it)
extern "C" uint8_t ed_operators[], ed_operators_end[], ed_extension_missing[];
extern "C" void lib_double_to_asc(uint32_t hi, uint32_t lo, char *to, int32_t digits, uint32_t mode);

namespace {

uint8_t lower(uint8_t c) { return c >= 'A' && c <= 'Z' ? c + 32 : c; }
uint8_t upper(uint8_t c) { return c >= 'a' && c <= 'z' ? c - 32 : c; }

// TklNext / EdlNext: past a token's name and parameters, even. False at
// the end of the table.
bool next_token(const uint8_t *&a1)
{
    while (!(*a1++ & 0x80))
        ;
    while (!(*a1++ & 0x80))
        ;
    if (address(a1) & 1)
        a1++;
    return peek_w(a1) != 0;
}

} // namespace


namespace {

using amos::source::skip_token;

// The result of Tokenise: 1 tokens, 0 an empty line, -1 too long; the
// length of the line in bytes
struct Tokenised {
    int32_t result;
    uint32_t length;
};

// L_ValRout on the number at p (a4 = AdTokens, as it wants): false if
// there is none, else the token, its value and the end of the number
bool number(uint8_t *p, uint16_t &token, uint32_t &v1, uint32_t &v2, uint8_t *&end)
{
    IData &d = idata();
    Ctx &c = ctx();
    uint8_t *a4 = c.a4;
    c.a4 = ptr<uint8_t>(d.AdTokens[0]);
    Regs r;
    r.a[0] = address(p);
    r.d[0] = 0;
    amos_call(&r, lib_address(L_ValRout));
    c.a4 = a4;
    if (r.a[0] == address(p))
        return false;
    end = ptr<uint8_t>(r.a[0]);
    token = uint16_t(r.d[1]);
    v1 = r.d[3];
    v2 = r.d[4];
    return true;
}

// Tokenise: the text at a3 into tokens at a1 (512 bytes and more). The
// text may change: the number reader writes the E of exponents in
// lower case.
Tokenised tokenise(uint8_t *a3, uint8_t *a1)
{
    IData &d = idata();
    // the token being written (T_TkAd of the assembler, used by nothing
    // else)
    uint8_t *tk_ad = nullptr;
    uint8_t &tk_char = reinterpret_cast<uint8_t *>(&d.TkChCar)[0];
    uint8_t *a4 = a1;
    const uint8_t *limit = a1 + 512;
    // d5: 0 string, 1 variable, 3 past the start of the line, 4 line
    // number, 5 Rem
    uint16_t d5 = 0;
    uint8_t d0 = 0;
    uint32_t d1, d2, d3, d6 = 0, d7;
    uint16_t token = 0;
    const uint8_t *table = nullptr;     // a1 in the table searches
    const uint8_t *a2 = nullptr;
    const uint8_t *d4 = nullptr;        // the token found
    uint8_t *a0 = nullptr;
    uint8_t *found_end = nullptr;
    const uint8_t *found = nullptr;
    uint32_t found_table = 0;
    int32_t result;

    poke_w(a4, 0);
    a4 += 2;
    // The indentation
    d1 = 0;
    for (;;) {
        d1++;
        d0 = *a3++;
        if (!d0)
            goto TokVide;
        if (d0 != ' ')
            break;
    }
    a3--;
    if (d1 >= 127)
        d1 = 127;
    a4[-1] = uint8_t(d1);
    // A line number
    d0 = *a3;
    if (d0 >= '0' && d0 <= '9') {
        d5 |= 1 << 1 | 1 << 4;
        tk_ad = a4;
        poke_w(a4, tk::Var);
        poke_l(a4 + 2, 0);
        a4 += 6;
        *a4++ = *a3++;
    }
    // A quote at the start: Rem
    if (d0 == '\'') {
        a3++;
        poke_w(a4, tk::Rem2);
        a4 += 2;
        goto TkKt2;
    }

TokLoop:
    if (a4 > limit)
        goto TokFin;
    d0 = *a3++;
    if (!d0)
        goto TokFin;
    if (d5 & 1 << 5) {
        *a4++ = d0;
        goto TokLoop;
    }
    if (d5 & 1 << 1)
        goto TkVD;
    if (!(d5 & 1))
        goto TkC2;
    if (d0 == tk_char)
        goto TkC1;
    *a4++ = d0;
    goto TokLoop;
    // The end of a string
TkChf:
    a3--;
TkC1:
    d5 &= ~1;
    poke_w(tk_ad + 2, uint16_t(a4 - tk_ad - 4));      // without the even byte
    if (address(a4) & 1)
        *a4++ = 0;
    goto TokLoop;
    // The start of a string?
TkC2:
    if (d0 != '"' && d0 != '\'')
        goto TkOtre;
    tk_char = d0;
    tk_ad = a4;
    poke_w(a4, d0 == '\'' ? tk::Ch2 : tk::Ch1);
    poke_w(a4 + 2, 0);
    a4 += 4;
    d5 |= 1;
    goto TokLoop;

    // A variable going on
TkVD:
    d0 = lower(d0);
TkFV:
    d1 = 0;
    a0 = tk_ad;
    if (d5 & 1 << 4) {
        // the line number
        if (d0 >= '0' && d0 <= '9')
            goto TkV3;
        d5 |= 1 << 3;
        d5 &= ~(1 << 4);
        if (d0 != ':')
            a3--;
        goto TkV1;
    }
    if (d0 == '_' || (d0 >= '0' && d0 <= '9') || (d0 >= 'a' && d0 <= 'z') || d0 > 128)
        goto TkV3;
    goto TkV4;
TkV3:
    *a4++ = d0;
    goto TokLoop;
    // The end of the variable, label or label call
TkV4:
    {
        bool not_start = d5 & 1 << 3;
        d5 |= 1 << 3;
        if (not_start || d0 != ':')
            goto TkV5;
    }
TkV1:
    poke_w(a0, tk::Lab);
    goto TkV7;
TkV5:
    a3--;
    d1 = d0 == '$' ? 2 : d0 == '#' ? 1 : 0;
    if (d1)
        a3++;
TkV7:
    if (address(a4) & 1)
        *a4++ = 0;
    a0[4] = uint8_t(a4 - a0 - 6);
    a0[5] = uint8_t(d1);
    d5 &= ~(1 << 1);
    goto TokLoop;

TkOtre:
    if (d0 == ' ')
        goto TokLoop;
    // A number?
    {
        uint16_t t;
        uint32_t v1, v2;
        uint8_t *end;
        if (number(a3 - 1, t, v1, v2, end)) {
            a3 = end;
            poke_w(a4, t);
            poke_l(a4 + 2, v1);
            a4 += 6;
            if (t == tk::DFl) {
                poke_l(a4, v2);
                a4 += 4;
            }
            goto TokLoop;
        }
    }

    // The token tables: the operators first, one by one, then the main
    // library and the extensions: for a letter through the tables of the
    // tokens by first letter (AdTTokens), the longest name kept; else one
    // by one, the first name kept.
    d7 = uint32_t(-4);
    d3 = 0;
    d2 = lower(a3[-1]);
    table = ed_operators;
    goto TkLIn;
TkUn:
    if (uint8_t(d2) < 'a' || uint8_t(d2) > 'z')
        goto Tkl1;
    d2 |= 0x80000000;
    d6 = (d6 & 0xFFFF0000) | uint16_t((uint16_t(d2) - 'a') << 1);
Tkr1:
    {
        uint32_t fast = d.AdTTokens[d7 / 4];
        if (!fast)
            goto Tkl1;
        a2 = ptr<uint8_t>(fast) + peek_w(ptr<uint8_t>(fast) + 4 + uint16_t(d6));
        d6 |= 0x80000000;
        goto TkRNext;
    }
Tkl1:
    {
        uint32_t t = d.AdTokens[d7 / 4];
        if (!t)
            goto TkNext;
        table = ptr<uint8_t>(t) + 6;
    }
TkLIn:
    d6 &= ~0x80000000;
    if (uint8_t(d2) == '!')
        goto TkKF;
    if (uint8_t(d2) == '?') {
        // ? for Print, ?# for Print #
        const uint8_t *p = a3;
        for (;;) {
            uint8_t c = *p++;
            if (c == '#') {
                a3 = const_cast<uint8_t *>(p);
                token = tk::HPr;
                goto TkKt0;
            }
            if (!c || c != ' ')
                break;
        }
        token = tk::Pr;
        goto TkKt0;
    }
Tkl2:
    d4 = table;
    table += 4;
    for (;;) {
        uint8_t c = *table++;
        if (c & 0x80) {
            // Tkl4: a name of one letter?
            table--;
            c &= 0x7F;
            if (c == ' ')
                goto TklT;
            if (c != uint8_t(d2))
                goto Tkl3;
            goto TklT;
        }
        if (c == ' ' || c == '!')
            continue;
        if (c == uint8_t(d2))
            goto TkRe0;
        break;
    }
Tkl3:
    if (next_token(table))
        goto Tkl2;
TkNext:
    d7 += 4;
    if (!d7)
        goto TkUn;
    if (d7 >= 4 * 26) {
        if (!uint16_t(d3))
            goto TkKF;
        d4 = found;
        a3 = found_end;
        d7 = (d7 & 0xFFFF0000) | uint16_t(found_table);
        goto TklT;
    }
    if (int32_t(d2) >= 0)
        goto Tkl1;
    goto Tkr1;

TkRNext:
    {
        uint16_t offset = peek_w(a2);
        a2 += 2;
        if (!offset)
            goto TkNext;
        table = ptr<uint8_t>(d.AdTokens[d7 / 4]) + offset;
        d4 = table;
        table += 5;
        if (table[-1] == '!' || table[-1] == ' ')
            table++;
    }
    // The other letters of the name
TkRe0:
    a0 = a3;
    {
        uint8_t c, n;
    TkRe1:
        c = lower(*a0++);
    TkRe2:
        n = *table++;
        if (n & 0x80)
            goto TkKt;
        if (n == ' ') {
            if (n != c)
                goto TkRe2;
            goto TkRe1;
        }
        if (c == n)
            goto TkRe1;
    TkRe4:
        if (int32_t(d6) >= 0)
            goto Tkl3;
        goto TkRNext;
        // The name ends: found?
    TkKt:
        a0--;
        table--;
        n &= 0x7F;
        if (n != ' ') {
            if (n != c)
                goto TkRe4;
            a0++;
        }
        if (int32_t(d6) < 0) {
            uint16_t length = uint16_t(table - d4);
            if (length <= uint16_t(d3))
                goto TkRe4;
            d3 = (d3 & 0xFFFF0000) | length;
            found = d4;
            found_end = a0;
            found_table = d7;
            goto TkRe4;
        }
        a3 = a0;
    }
TklT:
    if (int16_t(d7) > 0)
        goto TkKtE;
    if (int16_t(d7) == 0)
        token = uint16_t(d4 - ptr<uint8_t>(d.AdTokens[0]));
    else
        token = uint16_t(d4 - ed_operators_end);
TkKt0:
    poke_w(a4, token);
    a4 += 2;
    d5 &= ~(1 << 4);
    d5 |= 1 << 3;
    if (token >= tk::Equ && token <= tk::StruS)
        goto TkKt5;
    if (token == tk::On) {
        poke_l(a4, 0);
        a4 += 4;
        goto TokLoop;
    }
    if (token == tk::Data)
        goto TkKt3;
    if (token == tk::Rem1)
        goto TkKt2;
    if (token == tk::For || token == tk::Rpt || token == tk::Whl || token == tk::Do)
        goto TkKt3;
    if (token == tk::Exit || token == tk::ExIf)
        goto TkKt4;
    if (token == tk::If)
        goto TkKt3;
    if (token == tk::Else)
        goto TkKta;
    if (token == tk::ElsI)
        goto TkKt3;
    if (token == tk::Then)
        goto TkKtb;
    if (token == tk::Proc)
        goto TkKt6;
    if (token == tk::DPre)
        d.MathFlags |= 0x83;            // double precision
    goto TokLoop;
    // A Rem
TkKt2:
    poke_w(a4, 0);
    a4 += 2;
    tk_ad = a4;
    d5 |= 1 << 5;
    goto TokLoop;
    // The room the verifier fills in
TkKt6:
    poke_w(a4, 0);
    a4 += 2;
TkKt5:
    poke_w(a4, 0);
    a4 += 2;
TkKt4:
    poke_w(a4, 0);
    a4 += 2;
TkKt3:
    poke_w(a4, 0);
    a4 += 2;
    goto TokLoop;
    // An extension: Ext, its number, 0, the offset in its table
TkKtE:
    poke_w(a4, tk::Ext);
    a4[2] = uint8_t(uint16_t(d7) >> 2);
    a4[3] = 0;
    poke_w(a4 + 4, uint16_t(d4 - ptr<uint8_t>(d.AdTokens[d7 / 4])));
    a4 += 6;
    d5 &= ~(1 << 4);
    d5 |= 1 << 3;
    goto TokLoop;
    // Else / Then: a line number after them is a label call
TkKta:
    poke_w(a4, 0);
    a4 += 2;
TkKtb:
    a0 = a3;
    for (;;) {
        d0 = *a0++;
        if (!d0)
            goto TokLoop;
        if (d0 != ' ')
            break;
    }
    if (d0 < '0' || d0 > '9')
        goto TokLoop;
    a3 = a0;
    token = tk::LGo;
    goto TkKf2;

    // Nothing found: the start of a variable
TkKF:
    token = tk::Var;
    d0 = lower(a3[-1]);
    if (d0 != '_' && d0 < 128 && (d0 < 'a' || d0 > 'z'))
        goto TokLoop;
TkKf2:
    tk_ad = a4;
    poke_w(a4, token);
    poke_l(a4 + 2, 0);
    a4 += 6;
    *a4++ = d0;
    d5 |= 1 << 1;
    goto TokLoop;

TokFin:
    if (d5 & 1 << 1)
        goto TkFV;
    if (d5 & 1)
        goto TkChf;
    result = 1;
    if (d5 & 1 << 5) {
        // the Rem's length, even
        if (address(a4) & 1)
            *a4++ = ' ';
        poke_w(tk_ad - 2, uint16_t(a4 - tk_ad));
    }
    goto TokPaR;
TokVide:
    result = 0;
TokPaR:
    poke_w(a4, 0);
    a4 += 2;
    poke_w(a4, 0);
    {
        uint16_t length = uint16_t(a4 - a1);
        if (length >= 510) {
            poke_w(a1, 0);
            return {-1, 0};
        }
        a1[0] = uint8_t(length >> 1);
        return {result, uint32_t(int16_t(length & ~1))};
    }
}

// The name of the token at a0 written at a4 the way the editor is set
// to (DtkMaj1: 0 lower case, 1 upper case, 2 capitals)
uint8_t *write_name(const uint8_t *a0, uint8_t *a4)
{
    uint8_t c, d2;
    switch (idata().DtkMaj1) {
    case 0:
        do {
            c = *a0++;
            *a4++ = c;
        } while (!(c & 0x80));
        a4[-1] &= 0x7F;
        return a4;
    case 1:
        do {
            d2 = *a0++;
            *a4++ = upper(d2 & 0x7F);
        } while (!(d2 & 0x80));
        return a4;
    default:
        for (;;) {
            d2 = *a0++;
            *a4++ = upper(d2 & 0x7F);
            if (d2 & 0x80)
                return a4;
            for (;;) {
                c = *a0++;
                *a4++ = c;
                if (c & 0x80) {
                    a4[-1] &= 0x7F;
                    return a4;
                }
                if (c == ' ')
                    break;
            }
        }
    }
}

// A number through one of AMOSPro.Lib's routines: d0 the value, a0 the
// output, d3 = -1 for no leading zeros (hexadecimal and binary; the
// original left d3 as other code had it); the end of it
uint8_t *convert(int routine, uint32_t value, uint8_t *to)
{
    Regs r;
    r.d[0] = value;
    r.d[3] = 0xFFFFFFFF;
    r.a[0] = address(to);
    lib_call(routine, r);
    return ptr<uint8_t>(r.a[0]);
}

// Detok: the line at a6 as text at a1 (a length word, the characters); in
// the monitor's mode a6 starts at the tokens. Returns the position in the
// text of the token at at, and the length.
struct Detokenised {
    uint16_t position;
    uint32_t length;
};

Detokenised detokenise(const uint8_t *a6, uint8_t *a1, const uint8_t *at, bool monitor)
{
    IData &d = idata();
    uint8_t *a4 = a1 + 2;
    uint8_t *const a2 = a4;
    uint16_t position = 0;
    uint16_t d5 = 0;                    // bit 0: a variable or constant just written
    if (!monitor) {
        if (!a6[0])
            goto DtkFin;
        for (int n = int(a6[1]) - 2; n >= 0; n--)
            *a4++ = ' ';
        a6 += 2;
    }
    for (;;) {
        if (a6 == at)
            position = uint16_t(a4 - a2);
        if (int32_t(address(at)) < 0) {
            // a routine to call at each token, its address negated (the
            // monitor's Tt_Decoupe, which notes where its instructions are)
            Ctx &c = ctx();
            AllRegs r = {};
            r.a[3] = address(c.a3);
            r.a[4] = address(a4);
            r.a[5] = address(c.a5);
            r.a[6] = address(a6);
            r.d[6] = c.d6;
            r.d[7] = c.d7;
            amos_call_all(&r, ptr<uint8_t>(-int32_t(address(at))));
        }
        uint16_t t = peek_w(a6);
        a6 += 2;
        if (!t)
            break;
        if (t <= tk::LGo) {
            // A variable, label, procedure or label call
            if (d5 & 1 && a4[-1] != ' ')
                *a4++ = ' ';
            uint16_t length = a6[2];
            uint16_t n = length - 1;
            uint8_t flag = a6[3];
            const uint8_t *a0 = a6 + 4;
            int kind = 0;                       // 1 label, -1 line number
            if (t == tk::Lab)
                kind = a0[0] >= '0' && a0[0] <= '9' ? -1 : 1;
            uint8_t mode = d.DtkMaj2;
            // (the original's mode 2 took the capital from the token, not
            // the name, and lost its place in the line)
            if (mode >= 2 && a0[0]) {
                *a4++ = upper(*a0++);
                n--;
                mode = 0;
            }
            if (n != 0xFFFF) {
                do {
                    uint8_t c = *a0++;
                    if (!c)
                        break;
                    *a4++ = mode == 1 ? upper(c) : c;
                } while (n-- != 0);
            }
            d5 |= 1;
            a6 += 4 + length;
            if (kind > 0) {
                *a4++ = ':';
            } else if (!kind) {
                if ((flag & 3) == 1)
                    *a4++ = '#';
                else if ((flag & 3) == 2)
                    *a4++ = '$';
            }
            continue;
        }
        if (t < tk::Ext || t == tk::DFl) {
            // A constant (after a variable a space, but not before a
            // double precision one: it went the instructions' way first)
            if (t == tk::DFl)
                d5 &= ~1;
            if (d5 & 1) {
                d5 &= ~1;
                if (a4[-1] != ' ')
                    *a4++ = ' ';
            }
            uint8_t *start = a4;
            if (t == tk::Ent) {
                a4 = convert(L_LongToDec, peek_l(a6), a4);
                a6 += 4;
                continue;
            }
            if (t == tk::Hex) {
                a4 = convert(L_LongToHex, peek_l(a6), a4);
                a6 += 4;
                continue;
            }
            if (t == tk::Bin) {
                a4 = convert(L_LongToBin, peek_l(a6), a4);
                a6 += 4;
                continue;
            }
            if (t == tk::Fl) {
                Regs r;
                r.d[0] = peek_l(a6);
                r.a[0] = address(a4);
                r.d[4] = 0xFFFFFFFF;
                r.d[5] = 0;
                lib_call(L_FloatToAsc, r);
                a4 = ptr<uint8_t>(r.a[0]);
                a6 += 4;
                d5 = 0;
            } else if (t == tk::DFl) {
                lib_double_to_asc(peek_l(a6), peek_l(a6 + 4), reinterpret_cast<char *>(a4), 15, 2);
                a6 += 8;
                while (*a4)
                    a4++;
            } else {
                // A string
                uint8_t quote = t == tk::Ch1 ? '"' : '\'';
                *a4++ = quote;
                uint16_t n = peek_w(a6);
                a6 += 2;
                if (n && n <= 0x8000) {
                    while (n--)
                        *a4++ = *a6++;
                    if (address(a6) & 1)
                        a6++;
                }
                *a4++ = quote;
                continue;
            }
            // A float without "." or "E" gets ".0"
            for (const uint8_t *p = start;; p++) {
                if (!*p) {
                    *a4++ = '.';
                    *a4++ = '0';
                    break;
                }
                if (*p == '.' || *p == 'E')
                    break;
            }
            continue;
        }
        d5 &= ~1;
        if (t == tk::Par1) {
            // "(": never a space before
            if (a4 != a2 && a4[-1] == ' ')
                a4--;
            *a4++ = '(';
            a6 = skip_token(t, a6);
            continue;
        }
        const uint8_t *a0;
        uint8_t d3;
        if (int16_t(t) < 0) {
            a0 = ed_operators_end + 4 + int16_t(t);
        } else if (t == tk::Ext) {
            uint8_t n = a6[0];
            uint32_t ext = d.AdTokens[int8_t(n)];
            if (!ext) {
                // "Extension X", an instruction
                uint8_t *p = ed_extension_missing;
                while (!(*p++ & 0x80))
                    ;
                p[-1] = uint8_t(n + 'A' + 0x80);
                a0 = ed_extension_missing;
                d3 = 'I';
                goto Dtk3a;
            }
            a0 = ptr<uint8_t>(ext) + 4 + int16_t(peek_w(a6 + 2));
        } else {
            a0 = ptr<uint8_t>(d.AdTokens[0]) + 4 + int16_t(t);
        }
        {
            // The type after the name: functions (0-8, O, V) get no space
            const uint8_t *p = a0;
            while (!(*p++ & 0x80))
                ;
            d3 = *p;
            if (d3 == 'O' || d3 == 'V' || (d3 >= '0' && d3 < '9'))
                goto Dtk4;
        }
    Dtk3a:
        if (a4 != a2 && a4[-1] != ' ')
            *a4++ = ' ';
    Dtk4:
        // A name of $80-$9F: the name of an earlier token
        {
            uint8_t d1;
            for (;;) {
                d1 = *a0;
                if (d1 < 0x80 || d1 > 0x9F)
                    break;
                a0 -= 4;
                d1 -= 0x80;
                if (d1) {
                    a0 -= int8_t(d1);
                    break;
                }
                do
                    d1 = *--a0;
                while (d1 != '!' && d1 != 0x80);
                if (d1 == '!')
                    break;
            }
            if (d1 == '!')
                a0++;
        }
        a4 = write_name(a0, a4);
        if (t == tk::Rem1 || t == tk::Rem2) {
            a6 += 2;
            while (*a6)
                *a4++ = *a6++;
            continue;
        }
        if (d3 == 'I')
            *a4++ = ' ';
        a6 = skip_token(t, a6);
    }
DtkFin:
    uint32_t length = uint32_t(a4 - a2);
    poke_w(a2 - 2, uint16_t(length));
    return {position, length};
}

} // namespace

namespace amos::ed {

uint32_t detok(const uint8_t *line, uint8_t *buffer) { return detokenise(line, buffer, nullptr, false).length; }

int32_t tokenise_line(uint8_t *text, uint8_t *buffer) { return tokenise(text, buffer).result; }

} // namespace amos::ed

extern "C" {
// Tokenise: a0 the text, a1 the buffer; d0 1, 0 (empty) or -1 (too long),
// d1 the length
void ed_tokenise(AllRegs *r)
{
    Tokenised t = tokenise(ptr<uint8_t>(r->a[0]), ptr<uint8_t>(r->a[1]));
    r->d[0] = uint32_t(t.result);
    r->d[1] = t.length;
}

// Detok / Mon_Detok: a0 the line, a1 the buffer, d0 the address whose
// position in the text is wanted; d0 that position (low word), a0 the
// length. A negative d0 is the address, negated, of a routine to call at
// each token, with a6 the token and a4 where its text goes.
void ed_detok(AllRegs *r, bool monitor)
{
    Detokenised t = detokenise(ptr<uint8_t>(r->a[0]), ptr<uint8_t>(r->a[1]), ptr<uint8_t>(r->d[0]), monitor);
    r->d[0] = (r->d[0] & 0xFFFF0000) | t.position;
    r->a[0] = t.length;
}

void ed_detok_editor(AllRegs *r) { ed_detok(r, false); }
void ed_detok_monitor(AllRegs *r) { ed_detok(r, true); }

// TInst: a0 past the data of token d0; d1 = 1 (unchanged at the end)
void ed_skip_token(AllRegs *r)
{
    uint16_t t = uint16_t(r->d[0]);
    r->a[0] = address(skip_token(t, ptr<uint8_t>(r->a[0])));
    if (t)
        r->d[1] = 1;
}
}
