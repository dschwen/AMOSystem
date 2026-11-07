// Print, LPrint, Print #n (+ILib.s), with Using formats.
//
// The items are written into Buffer one by one (a long string in pieces
// of 121 characters) and each piece sent to the window / printer
// (L_ImpChaine) or the file. PrintPos is where the items continue: when
// the string space runs out, the instruction starts again and does not
// print what it printed already. PrintFlg's first byte: bit 7 a string is
// being printed, bit 6 the last piece is in the buffer.
//
// Using: a format string (in Buffer + 256) for the next item: for a
// number "#" digits, "+"/"-" its sign, "." the decimal point, ";" a space
// at the point, "^" the exponent; for a string "~" its characters.

#include "interp/interp.h"

#include "core/amiga.h"
#include "lib/convert.h"

#include <dos/dos.h>

#include "hal/files.h"

using namespace amos;
using namespace amos::interp;

namespace {

constexpr uint8_t FString = 1 << 7, FLast = 1 << 6;
constexpr int Piece = 121;
const char Power[] = "E+000  ";

uint8_t &flags() { return reinterpret_cast<uint8_t *>(&idata().PrintFlg)[0]; }
char *buffer() { return ptr<char>(idata().Buffer); }

bool finished(uint16_t t) { return !t || t == tk::DP || t == tk::Then || t == tk::Else; }

// The string being printed in pieces
const uint8_t *string_at;
uint32_t string_left;

bool digit(char c) { return c >= '0' && c <= '9'; }

// A number in Buffer formatted by the Using string
char *using_number(char *end)
{
    *end = 0;
    char *b = buffer();
    for (int i = 0; i < 128; i++) {
        b[128 + i] = b[i];
        b[i] = ' ';
    }
    char *out = b;
    const char *number = b + 128, *n = number;
    const char *format = b + 256, *f = format;
    while (*f && *f != '.' && *f != ';' && *f != '^')
        out++, f++;
    while (*n && *n != '.' && *n != 'E')
        n++;
    // the left of the point, backwards
    char *o = out;
    const char *ln = n, *lf = f;
    while (lf != format) {
        char c = *--lf;
        if (c == '#') {
            char x = ' ';
            if (ln != number) {
                char y = *--ln;
                if (digit(y))
                    x = y;
            }
            *--o = x;
        } else if (c == '-') {
            *--o = *number;
        } else if (c == '+') {
            *--o = *number == '-' ? '-' : '+';
        } else {
            *--o = c;
        }
    }
    // the right of it
    int power = 0;
    if (*n == '.')
        n++;
    for (;;) {
        char c = *f++;
        if (!c)
            return out;
        if (c == ';') {
            *out++ = ' ';
        } else if (c == '#') {
            if (digit(*n))
                *out++ = *n++;
            else
                *out++ = power ? ' ' : '0';
        } else if (c == '^') {
            if (!power) {
                while (*n && *n != 'E')
                    n++;
                power = *n ? -1 : 1;
            }
            if (power < 0) {
                char x;
                do
                    x = *n ? *n++ : 0;
                while (x == ' ');
                *out++ = x ? x : ' ';
            } else {
                *out++ = Power[power - 1];
                if (power != 6)
                    power++;
            }
        } else {
            *out++ = c;
        }
    }
}

// A string in Buffer formatted by the Using string
char *using_string(char *end)
{
    *end = 0;
    char *b = buffer();
    for (int i = 0; i < 128; i++)
        b[128 + i] = b[i];
    char *out = b;
    const char *s = b + 128, *f = b + 256;
    while (char c = *f++) {
        if (c == '~')
            *out++ = *s ? *s++ : ' ';
        else
            *out++ = c;
    }
    return out;
}

// ssprint: the next piece in Buffer; its length, -1 when all is printed
int32_t next_piece()
{
    IData &d = idata();
    char *out = buffer();
    if (!(flags() & FString)) {
        if (flags() & FLast)
            return -1;
        uint16_t t = peek_token();
        bool item = true;
        if (t == tk::Using) {
            const uint8_t *s = ptr<uint8_t>(evaluate_after().v1);
            uint16_t length = peek_w(s);
            if (length >= 120)
                fon_call();
            Regs r;
            r.a[0] = d.Buffer + 256;
            r.a[2] = address(s + 2);
            r.d[2] = length;
            lib_call(L_ChVerBuf2, r);
            d.UsingFlg = 1;
            pc() += 2;
        } else if (finished(t)) {
            item = false;
        } else {
            d.UsingFlg = 0;
        }
        if (item) {
            Value v = evaluate();
            if (kind(v) == TInt) {
                out = long_to_dec(out, int32_t(v.v1), -1, true);
                if (d.UsingFlg)
                    out = using_number(out);
            } else if (kind(v) == TFloat) {
                Regs r;
                r.a[0] = address(out);
                r.d[3] = v.v1;
                r.d[4] = v.v2;
                lib_call(L_Float2Ascii, r);
                out = ptr<char>(r.a[0]);
                if (d.UsingFlg)
                    out = using_number(out);
            } else {
                string_at = ptr<uint8_t>(v.v1) + 2;
                string_left = peek_w(ptr<uint8_t>(v.v1));
                if (!string_left) {
                    if (d.UsingFlg)
                        out = using_string(out);
                } else {
                    flags() |= FString;
                }
            }
        }
    }
    if (flags() & FString) {
        int n = Piece;
        while (n-- && string_left) {
            *out++ = char(*string_at++);
            string_left--;
        }
        if (!string_left) {
            flags() &= ~FString;
            if (d.UsingFlg)
                out = using_string(out);
        }
    }
    // the separator after the item
    d.UsingFlg = 0;
    if (!(flags() & FString)) {
        uint16_t t = peek_token();
        bool newline = true;
        if (t == tk::Vir || uint8_t(t) == uint8_t(tk::PVir)) {
            if (t == tk::Vir)
                *out++ = 9;
            pc() += 2;
            newline = false;
            if (finished(peek_token()))
                flags() |= FLast;
        }
        if (newline) {
            *out++ = 13;
            *out++ = 10;
            flags() |= FLast;
        }
    }
    *out = 0;
    if (!(flags() & FString))
        d.PrintPos = encode_context_cursor(pc());
    return out - buffer();
}

void print()
{
    IData &d = idata();
    if (d.PrintPos)
        pc() = decode_context_cursor(d.PrintPos);
    d.PrintFlg = 0;
    int32_t n;
    while ((n = next_piece()) >= 0) {
        if (!n)
            continue;
        if (d.PrintFile) {
            BPTR file = BPTR(peek_l(ptr<void>(d.PrintFile)));
            if (hal::write(file, buffer(), n) != n)
                disk_error();
        } else {
            Regs r;
            r.a[0] = d.Buffer;
            lib_call(L_ImpChaine, r);
        }
    }
    d.PrintPos = 0;
    d.PrintFile = 0;
}

} // namespace

extern "C" {

void interp_in_print()
{
    IData &d = idata();
    d.ImpFlg = 0;
    d.PrintFile = 0;
    print();
}

void interp_in_lprint()
{
    IData &d = idata();
    d.PrintFile = 0;
    d.ImpFlg = 1;
    print();
}

// Print #n,...
void interp_in_print_h()
{
    IData &d = idata();
    Value n = integer();
    pc() += 2;
    Regs r;
    r.d[0] = n.v1;
    lib_call(L_GetFile, r);
    uint8_t *channel = ptr<uint8_t>(r.a[2]);
    if (!peek_l(channel))
        error(97);
    if (!(channel[4] & 1) || uint16_t(r.d[0]) == 1)
        error(79 + 19);
    d.PrintFile = address(channel);
    d.ImpFlg = 0;
    print();
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_print.inc"
#endif
