// The Interface of AMOSPro.Lib (Dialog.s): dialog channels, the
// interpreter of the Interface language, active zones (buttons, edit
// zones, sliders, lists, hypertext), and the button and slider code the
// editor shares.
//
// A channel is an element of the Cur_Dialogs list: the Dialog structure,
// the variables, the labels, then the buffer. The buffer holds the zones
// and other entries the program creates (a length word, a mark word, then
// the data), ended by a zero long word, and the temporary strings of the
// expressions. The interpreter's value stack (a3 in the assembler) grows
// down from the end of the buffer (Dia_Pile).
//
// Errors of the program stop the innermost interpreter loop (Dia_Quit);
// the loops above it stop at their next instruction. This follows the
// assembler, which reset the stack to the loop's (Dia_Sp): here each loop
// is a local setjmp target, so no C++ object with a destructor may
// live in this code.

#include "core/amiga.h"
#include "lib/dialog.h"
#include "core/local_jump.h"

#include <graphics/gfx.h>
#include <graphics/rastport.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

// graphics/gfxbase.h defines NTSC, the name of an AMOS.library function
#undef NTSC

using namespace amos;

// screens.cpp, unpack.cpp
extern "C" uint32_t lib_sco0(RegFrame &r);
extern "C" uint32_t lib_unpack_bitmap(RegFrame &r);
// banks.cpp
namespace amos {
uint8_t *lst_new(be_u32 *list, uint32_t length);
void lst_del(be_u32 *list, uint8_t *element);
}
// dialog_glue.s: the routine records of Dia_ClearVar


namespace amos {
void add_routine(uint8_t *record, uint32_t list);
}

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint16_t BitControl = 1 << 8;

constexpr uint16_t mark(char a, char b) { return static_cast<uint16_t>(a << 8 | b); }
constexpr uint16_t BtMark = mark('B', 't'), StMark = mark('S', 't'), EdMark = mark('E', 'd'),
                   KyMark = mark('K', 'y'), BlMark = mark('B', 'l'), ZoMark = mark('Z', 'o'),
                   SlMark = mark('S', 'l'), LiMark = mark('L', 'i'), TxMark = mark('T', 'x'),
                   TaMark = mark('T', 'a'), TdMark = mark('T', 'd');

// Dia_RFlags
enum : uint8_t { RShown = 1 << 0, RRun = 1 << 1, RFrozen = 1 << 2 };
// Dia_Flags (RUn), and Dia_ZoFlags
enum : uint8_t { FlagExitKey = 1 << 2, FlagExitMouse = 1 << 3, FlagReturnNext = 1 << 4 };
enum : uint8_t { ZoNoWait = 1 << 5, ZoChanging = 1 << 6, ZoQuit = 1 << 7 };
// Shift keys
enum : uint8_t { Shf = 0x03, Ctr = 0x08, Alt = 0x30, Ami = 0xC0 };

constexpr int TxDispZone = 8;

// ---- the running program ----------------------------------------------------

struct Run {
    Dialog *d;
    const uint8_t *pc;      // a6
    be_u32 *sp;           // a3: the value stack
    local_jump::Buffer *quit;            // the innermost loop
};

uint8_t *entry_at(uint32_t a) { return ptr<uint8_t>(a); }
uint16_t entry_length(const uint8_t *e) { return peek_w(e); }
uint16_t entry_id(const uint8_t *e) { return peek_w(e + 2); }
bool is_zone(const uint8_t *e) { return peek_w(e + 4) == ZoMark; }
DiaZone *zone(uint8_t *e) { return reinterpret_cast<DiaZone *>(e); }
DiaButton *button(uint8_t *e) { return reinterpret_cast<DiaButton *>(e); }
DiaList *list(uint8_t *e) { return reinterpret_cast<DiaList *>(e); }
DiaText *text(uint8_t *e) { return reinterpret_cast<DiaText *>(e); }
DiaEdit *edit(uint8_t *e) { return reinterpret_cast<DiaEdit *>(e); }
LineEdit *line_edit(uint8_t *e) { return reinterpret_cast<LineEdit *>(e + 28); }
Slider *slider(uint8_t *e) { return reinterpret_cast<Slider *>(e + 28); }
uint8_t *slider_zone(Slider *s) { return reinterpret_cast<uint8_t *>(s) - 28; }
uint8_t &zone_flags_low(uint8_t *e) { return reinterpret_cast<uint8_t *>(&zone(e)->Dia_ZoFlags)[1]; }

struct RastPort *rastport() { return ptr<struct RastPort>(wdata().RastPort); }
const uint8_t *programs(Dialog *d) { return ptr<uint8_t>(d->Dia_Programs); }

// The zone the program in brackets belongs to (Dia_Vars-3*4)
uint8_t *current_zone(Dialog *d) { return entry_at(static_cast<uint32_t>(dia_vars(d)[-3])); }

[[noreturn]] void quit(Run &r) { local_jump::restore(r.quit); }

// Dia_Er: stop with an error at the current position
[[noreturn]] void fail(Run &r, uint16_t error)
{
    r.d->Dia_Error = error;
    r.d->Dia_ErrorPos = static_cast<uint16_t>(r.pc - programs(r.d));
    quit(r);
}
[[noreturn]] void syntax(Run &r) { fail(r, EDiaSyntax); }
[[noreturn]] void bad_call(Run &r) { fail(r, EDiaFCall); }

void push(Run &r, uint32_t v) { *--r.sp = v; }
uint32_t pop(Run &r) { return *r.sp++; }

// ---- AMOS.library ------------------------------------------------------------------

uint32_t wi(int function, uint32_t d1 = 0, uint32_t a1 = 0)
{
    Regs r = {};
    r.d[1] = d1;
    r.a[1] = a1;
    wi_call(function, r);
    return r.d[0];
}

void print(const char *s) { wi(wi::Print, 0, address(s)); }
void chr_out(uint8_t c) { wi(wi::ChrOut, c); }

uint32_t ec(int function, uint32_t d1 = 0)
{
    Regs r = {};
    r.d[1] = d1;
    ec_call(function, r);
    return r.d[0];
}

void locate(uint32_t x, uint32_t y)
{
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    wi_call(wi::Locate, r);
}

// WiCall Print3: text up to its zero, at most width characters. Returns
// the cursor x after it.
uint16_t print_limited(const uint8_t *s, uint16_t width)
{
    Regs r = {};
    r.a[1] = address(s);
    r.d[3] = width;
    wi_call(wi::Print3, r);
    return static_cast<uint16_t>(r.d[1]);
}

// "ESC B<paper> ESC P<pen>": the colours as digits
void print_colours(uint8_t paper, uint8_t pen)
{
    char s[] = "\x1b" "B0\x1b" "P0";
    s[2] = static_cast<char>(paper);
    s[5] = static_cast<char>(pen);
    print(s);
}

void set_writing(Dialog *d, uint16_t mode)
{
    d->Dia_Writing = mode;
    hal::set_draw_mode(rastport(), mode);
}

// ---- reading the program ---------------------------------------------------------

// The characters of the Interface (Dia_Chr). Small letters, spaces, brackets
// and anything above 127 are skipped: they only make the program readable.
enum Class : uint8_t { CEnd, CSep, COp, CDigit, CLetter, CSkip };

Class char_class(uint8_t c)
{
    if (!c)
        return CEnd;
    if (c >= 128 || c <= ' ' || c == '(' || c == ')' || c == '.' || c >= 'a')
        return c == '`' ? CSep : CSkip;
    if (c == ',' || c == ':' || c == ';' || c == '`')
        return CSep;
    if (c == '$' || c == '%' || (c >= '0' && c <= '9'))
        return CDigit;
    if (c >= 'A' && c <= 'Z')
        return CLetter;
    return COp;
}

struct Chr {
    uint8_t c;
    Class k;
};

Chr next(const uint8_t *&pc)
{
    for (;;) {
        uint8_t c = *pc++;
        Class k = char_class(c);
        if (k != CSkip)
            return {c, k};
    }
}
Chr next(Run &r) { return next(r.pc); }

// Dia_FChif: a number starting with first (a digit, "$" or "%"); pc ends
// on the character after it. The digits are read as the assembler did:
// a "-" in front of a decimal number is taken as a digit worth 253.
int32_t number(const uint8_t *&pc, uint8_t first)
{
    uint32_t n = 0;
    if (first == '%') {
        for (uint8_t c; (c = static_cast<uint8_t>(*pc++ - '0')) < 2;)
            n = n << 1 | c;
    } else if (first == '$') {
        for (;;) {
            uint8_t c = static_cast<uint8_t>(*pc++ - '0');
            if (static_cast<int8_t>(c) < 0)
                break;
            if (c >= 10) {
                c = static_cast<uint8_t>(c + '0' - 'A');
                if (static_cast<int8_t>(c) < 0 || c >= 6)
                    break;
                c = static_cast<uint8_t>(c + 10);
            }
            n = n << 4 | c;
        }
    } else {
        n = static_cast<uint8_t>(first - '0');
        for (uint8_t c; static_cast<int8_t>(c = static_cast<uint8_t>(*pc++ - '0')) >= 0 && c < 10;)
            n = n * 10 + c;
    }
    pc--;
    return static_cast<int32_t>(n);
}

// Dia_GetRout: a routine in brackets after an instruction. Returns its
// offset in the program, 0 for "[]".
uint16_t routine(Run &r)
{
    if (next(r).c != '[')
        syntax(r);
    const uint8_t *start = r.pc;
    Chr c = next(r);
    if (c.k == CEnd)
        syntax(r);
    if (c.c == ']')
        return 0;
    // Dia_EndP: up to the matching bracket
    for (int depth = 1; depth;) {
        uint8_t b = *r.pc++;
        if (!b)
            syntax(r);
        depth += b == '[' ? 1 : b == ']' ? -1 : 0;
    }
    return static_cast<uint16_t>(start - programs(r.d));
}

// Dia_GetLabel: the offset of label n in the program, 0 if none
uint16_t label_offset(Dialog *d, uint32_t n)
{
    for (const uint8_t *l = ptr<uint8_t>(d->Dia_Labels);; l += 6) {
        int32_t v = static_cast<int32_t>(peek_l(l));
        if (v < 0)
            return 0;
        if (static_cast<uint32_t>(v) == n)
            return peek_w(l + 4);
    }
}

// ---- the buffer -------------------------------------------------------------------

// Dia_GetBuffer: an entry of length bytes, cleared but for its mark
uint8_t *get_buffer(Run &r, uint16_t length)
{
    Dialog *d = r.d;
    uint8_t *e = entry_at(d->Dia_ABuffer);
    length = static_cast<uint16_t>((length + 1) & ~1);
    uint8_t *end = e + length;
    if (address(end) >= address(r.sp))
        fail(r, EDiaOBuffer);
    d->Dia_ABuffer = address(end);
    poke_w(e, length);
    uint8_t *p = e + 4;
    do {
        poke_w(p, 0);
        p += 2;
    } while (address(end) > address(p));
    poke_l(p, 0);
    return e;
}

// Dia_SetBuffer: keep what is in the buffer
void keep_buffer(Dialog *d) { d->Dia_PBuffer = d->Dia_ABuffer; }

// Strings of the Interface are a byte, the length byte, then the
// characters. Dia_StDebut starts one in the buffer, Dia_StFini ends it.
struct NewString {
    uint8_t *start;         // the string (its first byte)
    uint8_t *to;            // where the characters go
};

NewString string_start(Dialog *d)
{
    uint8_t *e = entry_at(d->Dia_ABuffer);
    poke_w(e, 0);
    poke_w(e + 2, StMark);
    poke_w(e + 4, 0);
    return {e + 4, e + 6};
}

uint32_t string_end(Dialog *d, NewString s)
{
    s.start[1] = static_cast<uint8_t>(s.to - s.start - 2);
    uint32_t end = (address(s.to) + 1) & ~1u;
    d->Dia_ABuffer = end;
    poke_l(ptr<void>(end), 0);
    poke_w(s.start - 4, static_cast<uint16_t>(end - address(s.start - 4)));
    return address(s.start);
}

uint8_t string_length(uint32_t s) { return ptr<uint8_t>(s)[1]; }
const uint8_t *string_chars(uint32_t s) { return ptr<uint8_t>(s) + 2; }

// Dia_DecToAsc: n in decimal at to. Returns the end.
uint8_t *decimal(int32_t n, uint8_t *to)
{
    uint32_t v = static_cast<uint32_t>(n);
    if (n < 0) {
        *to++ = '-';
        v = -v;
    }
    static const uint32_t powers[] = {1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1};
    bool digits = false;
    for (int i = 0; i < 10; i++) {
        uint8_t digit = 0;
        while (v >= powers[i]) {
            v -= powers[i];
            digit++;
        }
        if (digit || digits || i == 9) {
            *to++ = static_cast<uint8_t>('0' + digit);
            digits = true;
        }
    }
    return to;
}

// ---- functions ----------------------------------------------------------------------

// The function names (Dia_FTokens, bin/+Dialog_Funcs.bin): for each first
// letter from "@", the offset of its list after the operators' list at 32;
// the lists hold the function number * 4 and the second letter.
const uint8_t function_table[] = {
    0x00, 0x1e, 0x24, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x3c, 0x00,
    0x40, 0x00, 0x00, 0x54, 0x5e, 0x00, 0x66, 0x00, 0x6a, 0x70, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x2b, 0x14, 0x2d, 0x18, 0x2a, 0x1c, 0x2f, 0x38, 0x22, 0x3c, 0x27, 0x58, 0x23, 0x5c, 0x21,
    0xa0, 0x3d, 0xa4, 0x5c, 0xa8, 0x3c, 0xac, 0x3e, 0xb0, 0x26, 0xb4, 0x7c, 0x00, 0x00, 0x74, 0x52,
    0xc4, 0x53, 0x00, 0x00, 0x00, 0x58, 0x04, 0x59, 0x54, 0x50, 0x00, 0x00, 0x40, 0x58, 0x00, 0x00,
    0x24, 0x45, 0x28, 0x45, 0x4c, 0x49, 0x50, 0x41, 0x60, 0x5a, 0x00, 0x00, 0x20, 0x45, 0x00, 0x00,
    0x7c, 0x31, 0x80, 0x32, 0x84, 0x33, 0x88, 0x34, 0x8c, 0x35, 0x90, 0x36, 0x94, 0x37, 0x98, 0x38,
    0x9c, 0x39, 0x00, 0x00, 0x08, 0x58, 0x0c, 0x59, 0x44, 0x57, 0x48, 0x48, 0x00, 0x00, 0x2c, 0x57,
    0x30, 0x48, 0xc0, 0x4c, 0x00, 0x00, 0x34, 0x41, 0x00, 0x00, 0x64, 0x41, 0x6c, 0x42, 0x00, 0x00,
    0x68, 0x41, 0x70, 0x42, 0x00, 0x00, 0x78, 0x50, 0xb8, 0x56, 0xbc, 0x4e, 0x00, 0x00,
};

// The list for a first letter (an operator: the operators' list)
const uint8_t *function_list(uint8_t first) { return function_table + 32 + function_table[first - 64]; }

// The function second (searched like the assembler: in the lists that
// follow too). -1 if there is none at all.
int function_number(const uint8_t *list, uint8_t second, bool verify)
{
    for (const uint8_t *p = list; p + 1 < function_table + sizeof function_table; p += 2) {
        if (!p[0] && !p[1] && verify)
            return -1;
        if (p[1] == second)
            return p[0] >> 2;
    }
    return -1;
}

enum Function {
    FBx, FBy, FSx, FSy, FPlus, FMinus, FMul, FDiv, FNeg, FMess, FMess2, FTw, FTh, FVar, FString1, FString2,
    FCx, FSw, FSh, FMin, FMax, FBp, FDecimal, FPlusString, FStZero, FXa, FYa, FXb, FYb, FArray, FZp,
    FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FEq, FNe, FLt, FGt, FAnd, FOr, FZVar, FZNum, FTLen,
    FASize,
};

// An array of the program: one dimension of 4 byte values. Its size.
uint16_t array_size(Run &r, uint32_t a)
{
    if (static_cast<int32_t>(a) <= 1024)
        bad_call(r);
    const uint8_t *p = ptr<uint8_t>(a);
    if (p[0] != 1 || p[1] != 2)
        bad_call(r);
    return peek_w(p + 2);
}

// Dia_FDecimal: the top of the stack as a string
void to_decimal(Run &r, int16_t n)
{
    if (n < 1)
        syntax(r);
    NewString s = string_start(r.d);
    int32_t v = static_cast<int32_t>(*r.sp);
    *r.sp = address(s.start);
    s.to = decimal(v, s.to);
    string_end(r.d, s);
}

void call_function(Run &r, int f, int16_t &n)
{
    Dialog *d = r.d;
    be_i32 *v = dia_vars(d);
    auto two = [&] {
        if (n < 2)
            syntax(r);
        n--;
    };
    switch (f) {
    case FBx:
        push(r, d->Dia_BaseX), n++;
        break;
    case FBy:
        push(r, d->Dia_BaseY), n++;
        break;
    case FSx:
        push(r, d->Dia_Sx), n++;
        break;
    case FSy:
        push(r, d->Dia_Sy), n++;
        break;
    case FXa:
        push(r, static_cast<uint32_t>(static_cast<int16_t>(d->Dia_XA))), n++;
        break;
    case FYa:
        push(r, static_cast<uint32_t>(static_cast<int16_t>(d->Dia_YA))), n++;
        break;
    case FXb:
        push(r, static_cast<uint32_t>(static_cast<int16_t>(d->Dia_XB))), n++;
        break;
    case FYb:
        push(r, static_cast<uint32_t>(static_cast<int16_t>(d->Dia_YB))), n++;
        break;
    case FSw:
        push(r, ptr<Screen>(wdata().EcCourant)->EcTx), n++;
        break;
    case FSh:
        push(r, ptr<Screen>(wdata().EcCourant)->EcTy), n++;
        break;
    case FTh:
        push(r, static_cast<uint32_t>(static_cast<int16_t>(rastport()->TxHeight))), n++;
        break;
    case FBp:
    case FZp:
        push(r, static_cast<uint32_t>(v[-1])), n++;
        break;
    case FPlus: {
        two();
        uint32_t b = pop(r);
        *r.sp += b;
        break;
    }
    case FMinus: {
        two();
        uint32_t b = pop(r);
        *r.sp -= b;
        break;
    }
    case FMul: {
        two();
        uint32_t b = pop(r);
        // muls: the low words
        *r.sp = static_cast<uint32_t>(static_cast<int16_t>(*r.sp) * static_cast<int16_t>(b));
        break;
    }
    case FDiv: {
        two();
        uint32_t b = pop(r);
        // divs of the low words (a zero low word was a division by zero
        // trap in the assembler)
        if (!static_cast<int16_t>(b))
            syntax(r);
        int16_t x = static_cast<int16_t>(*r.sp), y = static_cast<int16_t>(b);
        int16_t q = x == -32768 && y == -1 ? x : static_cast<int16_t>(x / y);
        *r.sp = static_cast<uint32_t>(static_cast<int32_t>(q));
        break;
    }
    case FEq:
    case FNe:
    case FLt:
    case FGt: {
        two();
        int32_t b = static_cast<int32_t>(pop(r)), a = static_cast<int32_t>(*r.sp);
        bool t = f == FEq ? a == b : f == FNe ? a != b : f == FLt ? a < b : a > b;
        *r.sp = t ? -1u : 0;
        break;
    }
    case FAnd: {
        two();
        uint32_t b = pop(r);
        *r.sp &= b;
        break;
    }
    case FOr: {
        two();
        uint32_t b = pop(r);
        *r.sp |= b;
        break;
    }
    case FMin:
    case FMax: {
        two();
        int32_t b = static_cast<int32_t>(pop(r)), a = static_cast<int32_t>(*r.sp);
        if (f == FMin ? b < a : b > a)
            *r.sp = static_cast<uint32_t>(b);
        break;
    }
    case FNeg:
        if (!n)
            syntax(r);
        *r.sp = -*r.sp;
        break;
    case FVar: {
        if (n <= 0)
            syntax(r);
        // The variable number times 4, as a word, then indexed with the
        // whole register
        uint32_t i = *r.sp;
        i = (i & 0xFFFF0000) | static_cast<uint16_t>(i << 2);
        *r.sp = peek_l(reinterpret_cast<uint8_t *>(v) + static_cast<int32_t>(i));
        break;
    }
    case FZVar: {
        n++;
        uint8_t *z = current_zone(d);
        push(r, z ? zone(z)->Dia_ZoVar : d->Dia_NextZone);
        break;
    }
    case FZNum: {
        uint8_t *z = current_zone(d);
        if (!z)
            syntax(r);
        push(r, zone(z)->Dia_ZoNumber);
        n++;
        break;
    }
    case FP1:
    case FP2:
    case FP3:
    case FP4:
    case FP5:
    case FP6:
    case FP7:
    case FP8:
    case FP9: {
        uint16_t p = static_cast<uint16_t>(f - FP1 + 1);
        if (!d->Dia_PUsers || p > d->Dia_NPUsers)
            syntax(r);
        push(r, ptr<be_u32>(d->Dia_PUsers)[d->Dia_NPUsers - p]);
        n++;
        break;
    }
    case FArray: {
        if (n < 2)
            fail(r, EDiaNPar);
        n--;
        uint32_t i = pop(r);
        uint16_t size = array_size(r, *r.sp);
        if (i >= size)
            bad_call(r);
        *r.sp = peek_l(ptr<uint8_t>(*r.sp) + 6 + 4 * i);
        break;
    }
    case FASize:
        if (n < 1)
            fail(r, EDiaNPar);
        if (*r.sp)
            *r.sp = array_size(r, *r.sp);
        break;
    case FMess:
    case FMess2: {
        if (!n)
            syntax(r);
        const uint8_t *m = ptr<uint8_t>(d->Dia_Messages);
        int32_t i = static_cast<int32_t>(*r.sp);
        if (i <= 0)
            bad_call(r);
        while (--i) {
            if (m[1] >= 128)
                bad_call(r);
            m += 2 + m[1];
        }
        *r.sp = address(m);
        break;
    }
    case FTw:
    case FCx: {
        if (!n)
            syntax(r);
        uint32_t s = *r.sp;
        uint32_t w = 0;
        if (uint8_t l = string_length(s))
            w = static_cast<uint32_t>(static_cast<int16_t>(
                hal::text_length(rastport(), reinterpret_cast<STRPTR>(const_cast<uint8_t *>(string_chars(s))), l)));
        if (f == FCx) {
            int32_t c = static_cast<int32_t>(d->Dia_Sx - w);
            w = static_cast<uint32_t>(c < 0 ? 0 : c) >> 1;
        }
        *r.sp = w;
        break;
    }
    case FTLen:
        if (n < 1)
            syntax(r);
        *r.sp = string_length(*r.sp);
        break;
    case FString1:
    case FString2: {
        uint8_t quote = f == FString1 ? '"' : '\'';
        NewString s = string_start(d);
        for (;;) {
            uint8_t c = *r.pc++;
            if (c < ' ') {
                r.pc--;
                break;
            }
            if (c == quote)
                break;
            *s.to++ = c;
        }
        push(r, address(s.start));
        n++;
        string_end(d, s);
        break;
    }
    case FPlusString: {
        two();
        NewString s = string_start(d);
        uint32_t b = pop(r), a = *r.sp;
        for (uint8_t i = 0, l = string_length(a); i < l; i++)
            *s.to++ = string_chars(a)[i];
        for (uint8_t i = 0, l = string_length(b); i < l; i++)
            *s.to++ = string_chars(b)[i];
        *r.sp = address(s.start);
        string_end(d, s);
        break;
    }
    case FStZero: {
        two();
        NewString s = string_start(d);
        int16_t max = static_cast<int16_t>(pop(r));
        const uint8_t *from = ptr<uint8_t>(*r.sp);
        for (;;) {
            uint8_t c = *from++;
            if (c < ' ')
                break;
            *s.to++ = c;
            if (--max <= 0)
                break;
        }
        *r.sp = address(s.start);
        string_end(d, s);
        break;
    }
    case FDecimal:
        to_decimal(r, n);
        break;
    default:
        syntax(r);
    }
}

// Dia_Evalue: an expression, in reverse Polish notation, up to a separator
int32_t evaluate(Run &r)
{
    int16_t n = 0;
    for (;;) {
        Chr c = next(r);
        int f;
        if (c.k == CDigit) {
            push(r, static_cast<uint32_t>(number(r.pc, c.c)));
            n++;
            continue;
        } else if (c.k == COp) {
            f = function_number(function_table + 32, c.c, false);
        } else if (c.k == CLetter) {
            Chr second = next(r);
            f = function_number(function_list(c.c), second.c, false);
        } else {
            if (n != 1)
                syntax(r);
            return static_cast<int32_t>(pop(r));
        }
        if (f < 0)
            syntax(r);
        call_function(r, f, n);
    }
}

// ---- instructions ------------------------------------------------------------------

const char instruction_names[] = "EXUNLIBOSIBAPUSVILPRPOINSFSWSLSPBUJPRUBREDJSRTBCKYSADIBLLABQHSVSALILZCUIGBGSGLIFSZXYNWVTVLHTCASMGEGPSS";
constexpr int Instructions = sizeof instruction_names / 2;
const uint8_t instruction_params[Instructions] = {
    0, 3, 4, 5, 2, 2, 1, 2, 0, 4, 5, 3, 2, 1, 1, 2, 8, 1, 2, 1, 8, 1, 0, 2, 2, 1,
    8, 1, 1, 0, 9, 9, 10, 10, 2, 0, 4, 4, 4, 1, 1, 4, 0, 4, 4, 10, 1, 0, 4, 3, 8,
};
enum Instruction {
    IEx, IUnpack, ILine, IBox, ISize, IBase, IPuzzle, ISetVar, IIllegal, IPrint, IOutline, IInk, ISetFont,
    ISetWriting, ISetLine, ISetPattern, IButton, IJump, IRun, IButtonReturn, IEdit, IJumpSub, IRts,
    IButtonChange, IKey, ISave, IDigit, IBlock, ILabel, IButtonQuit, ISliderH, ISliderV, IActiveList,
    IInactiveList, IZoneChange, IUser, IGBox, IGSquare, IGLine, IIf, ISetZoneVar, IXY, INoWait, IVText,
    IVLine, IHyperText, ICall, IScreenMove, IGEllipse, IGPlot, ISetSlider,
};

int instruction_number(uint16_t name)
{
    for (int i = 0; i < Instructions; i++)
        if (name == static_cast<uint16_t>(instruction_names[2 * i] << 8 | instruction_names[2 * i + 1]))
            return i;
    return -1;
}

uint16_t loop(Run &r);
void button_draw(Run &r, uint8_t *z);
uint32_t zone_change(Run &r, uint8_t *z);
void zone_update(Run &r, uint32_t number, uint32_t a, uint32_t b, uint32_t c);
uint16_t run_events(Run &r, int32_t timer, int32_t flags);
uint16_t tests(Run &r);
void list_draw(Run &r, uint8_t *z);
void text_draw(Run &r, uint8_t *z);
void edit_first(Dialog *d);
void slider_draw(Slider *s, bool active);
void slider_click(Slider *s, Run *r);

// The routines of the sliders of dialogs (Dia_RSlider): an address only,
// slider_call recognises it
const uint8_t dia_slider_routines[16] = {};

// The coordinates of the box an instruction drew
void set_box(Dialog *d, int32_t xa, int32_t ya, int32_t xb, int32_t yb)
{
    d->Dia_XA = static_cast<uint16_t>(xa);
    d->Dia_YA = static_cast<uint16_t>(ya);
    d->Dia_XB = static_cast<uint16_t>(xb);
    d->Dia_YB = static_cast<uint16_t>(yb);
}

// UNpack x,y,image: an image of the resource
void unpack(Run &r, int32_t x, int32_t y, int32_t image)
{
    Dialog *d = r.d;
    const uint8_t *puzzle = ptr<uint8_t>(d->Dia_Puzzle);
    uint32_t i = static_cast<uint32_t>(image) + d->Dia_PuzzleI;
    if (!i || static_cast<uint16_t>(i) > peek_w(puzzle))
        syntax(r);
    puzzle += peek_l(puzzle + 2 + static_cast<int16_t>((i - 1) << 2));
    set_box(d, x, y, x, y);
    RegFrame f = {};
    f.a[0] = address(puzzle);
    f.a[1] = d->Dia_Screen;
    f.d[1] = static_cast<uint32_t>(x) + d->Dia_BaseX;
    f.d[2] = static_cast<uint32_t>(y) + d->Dia_BaseY;
    lib_unpack_bitmap(f);
    uint32_t size = f.d[0];
    d->Dia_PuzzleSx = (d->Dia_PuzzleSx & 0xFFFF0000) | static_cast<uint16_t>(size);
    d->Dia_XB = static_cast<uint16_t>(d->Dia_XB + size);
    d->Dia_PuzzleSy = (d->Dia_PuzzleSy & 0xFFFF0000) | (size >> 16);
    d->Dia_YB = static_cast<uint16_t>(d->Dia_YB + (size >> 16));
}

// After LIne / VLine / BOx: XA/YA the start (x a multiple of 8)
void line_end(Dialog *d, int32_t x, int32_t y)
{
    d->Dia_XA = static_cast<uint16_t>(x & ~7);
    d->Dia_YA = static_cast<uint16_t>(y);
}

// LIne x,y,image,x2: image+1 repeated, image on the left, image+2 on the
// right
void hline(Run &r, int32_t x, int32_t y, int32_t image, int32_t x2)
{
    int32_t left = x & ~7, right = x2 & ~7;
    int32_t room = right - left;
    if (room <= 0)
        bad_call(r);
    int32_t px = left;
    for (;;) {
        unpack(r, px, y, image + 1);
        int32_t w = static_cast<int32_t>(r.d->Dia_PuzzleSx);
        if (!w)
            break;
        px += w;
        room -= w;
        if (room < w)
            break;
    }
    unpack(r, left, y, image);
    unpack(r, right - static_cast<int32_t>(r.d->Dia_PuzzleSx), y, image + 2);
    line_end(r.d, x, y);
}

// VLine x,y,image,y2
void vline(Run &r, int32_t x, int32_t y, int32_t image, int32_t y2)
{
    x &= ~7;
    int32_t room = y2 - y;
    if (room <= 0)
        bad_call(r);
    int32_t py = y;
    for (;;) {
        unpack(r, x, py, image + 1);
        int32_t h = static_cast<int32_t>(r.d->Dia_PuzzleSy);
        if (!h)
            break;
        py += h;
        room -= h;
        if (room < h)
            break;
    }
    unpack(r, x, y, image);
    unpack(r, x, y2 - static_cast<int32_t>(r.d->Dia_PuzzleSy), image + 2);
    line_end(r.d, x, y);
}

// Dia_ScCopy: copy x1,y1 to x2,y2 to x3,y3 in the current screen
void screen_copy(Dialog *d, int32_t x1, int32_t y1, int32_t x3, int32_t y3, int32_t x2, int32_t y2)
{
    Screen *s = ptr<Screen>(wdata().EcCourant);
    idata().SccEcO = idata().SccEcD = address(s->EcCurrent);
    RegFrame f = {};
    f.a[0] = f.a[1] = address(s);
    f.d[0] = static_cast<uint32_t>(x1) + d->Dia_BaseX;
    f.d[2] = static_cast<uint32_t>(x3) + d->Dia_BaseX;
    f.d[4] = static_cast<uint32_t>(x2) + d->Dia_BaseX;
    f.d[1] = static_cast<uint32_t>(y1) + d->Dia_BaseY;
    f.d[3] = static_cast<uint32_t>(y3) + d->Dia_BaseY;
    f.d[5] = static_cast<uint32_t>(y2) + d->Dia_BaseY;
    f.d[6] = 0xCC;
    lib_sco0(f);
}

// BOx x,y,image,x2,y2: three lines of images (image, +3, +6), the middle
// one copied down
void box(Run &r, int32_t x, int32_t y, int32_t image, int32_t x2, int32_t y2)
{
    Dialog *d = r.d;
    int32_t room = y2 - y;
    if (room <= 0)
        bad_call(r);
    hline(r, x, y, image + 3, x2);
    int32_t h = static_cast<int32_t>(d->Dia_PuzzleSy);
    if (h && (room -= h) > 0) {
        int32_t py = y;
        do {
            int32_t from = py;
            py += static_cast<int32_t>(d->Dia_PuzzleSy);
            screen_copy(d, x, from, x, py, x2, py);
            room -= static_cast<int32_t>(d->Dia_PuzzleSy);
        } while (room >= static_cast<int32_t>(d->Dia_PuzzleSy));
    }
    hline(r, x, y, image, x2);
    hline(r, x, y2 - static_cast<int32_t>(d->Dia_PuzzleSy), image + 6, x2);
    line_end(d, x, y);
}

// PRint x,y,text,ink
void text_at(Run &r, int32_t x, int32_t y, uint32_t s, int32_t ink)
{
    Dialog *d = r.d;
    struct RastPort *rp = rastport();
    if (ink >= 0)
        hal::set_a_pen(rp, static_cast<ULONG>(ink));
    set_box(d, x, y, x, y);
    rp->cp_x = static_cast<WORD>(x + d->Dia_BaseX);
    rp->cp_y = static_cast<WORD>(y + d->Dia_BaseY + rp->TxBaseline);
    if (uint8_t l = string_length(s)) {
        auto chars = reinterpret_cast<STRPTR>(const_cast<uint8_t *>(string_chars(s)));
        hal::text(rp, chars, l);
        d->Dia_YB = static_cast<uint16_t>(d->Dia_YB + rp->TxHeight);
        d->Dia_XB = static_cast<uint16_t>(d->Dia_XB + hal::text_length(rp, chars, l));
    }
}

// VText x,y,text,ink: downwards
void vtext(Run &r, int32_t x, int32_t y, uint32_t s, int32_t ink)
{
    Dialog *d = r.d;
    struct RastPort *rp = rastport();
    if (ink >= 0)
        hal::set_a_pen(rp, static_cast<ULONG>(ink));
    set_box(d, x, y, x, y);
    int16_t px = static_cast<int16_t>(x + d->Dia_BaseX);
    int16_t py = static_cast<int16_t>(y + d->Dia_BaseY + rp->TxBaseline);
    const uint8_t *c = string_chars(s);
    for (uint8_t l = string_length(s); l; l--, c++) {
        rp->cp_x = px;
        rp->cp_y = py;
        hal::text(rp, reinterpret_cast<STRPTR>(const_cast<uint8_t *>(c)), 1);
        py = static_cast<int16_t>(py + rp->TxHeight);
        d->Dia_XB = static_cast<uint16_t>(d->Dia_XB + rp->TxHeight);
    }
}

// The zone header of BUtton and the sliders (Dia_GetEntete): number, x,
// y, width, height
uint8_t *zone_header(Run &r, uint16_t length)
{
    Dialog *d = r.d;
    uint8_t *e = get_buffer(r, length);
    DiaZone *z = zone(e);
    z->Dia_ZoNumber = static_cast<uint16_t>(evaluate(r));
    z->Dia_ZoId = ZoMark;
    int32_t x = evaluate(r);
    z->Dia_ZoX = static_cast<uint16_t>(x + d->Dia_BaseX);
    int32_t y = evaluate(r);
    z->Dia_ZoY = static_cast<uint16_t>(y + d->Dia_BaseY);
    z->Dia_ZoSx = static_cast<uint16_t>(evaluate(r));
    z->Dia_ZoSy = static_cast<uint16_t>(evaluate(r));
    set_box(d, x, y, static_cast<int16_t>(x + z->Dia_ZoSx), static_cast<int16_t>(y + z->Dia_ZoSy));
    z->Dia_ZoVar = d->Dia_NextZone;
    return e;
}

// The zone header of the edit zones, lists and hypertext (Dia_EdDiRout):
// number, x, y, width in characters. The window is at x, a multiple of
// 16, y. Returns the width (even).
uint16_t window_zone_header(Run &r, uint8_t *e, int32_t &wx, int32_t &wy)
{
    Dialog *d = r.d;
    DiaZone *z = zone(e);
    z->Dia_ZoNumber = static_cast<uint16_t>(evaluate(r));
    z->Dia_ZoId = ZoMark;
    int32_t x = evaluate(r);
    d->Dia_XA = d->Dia_XB = static_cast<uint16_t>(x);
    wx = static_cast<int32_t>((static_cast<uint32_t>(x) + d->Dia_BaseX) & 0xFFF0);
    z->Dia_ZoX = static_cast<uint16_t>(wx);
    int32_t y = evaluate(r);
    d->Dia_YA = d->Dia_YB = static_cast<uint16_t>(y);
    wy = static_cast<int32_t>(static_cast<uint32_t>(y) + d->Dia_BaseY);
    z->Dia_ZoY = static_cast<uint16_t>(wy);
    uint16_t w = static_cast<uint16_t>((evaluate(r) + 1) & 0xFFFE);
    if (!w)
        syntax(r);
    d->Dia_XB = static_cast<uint16_t>(d->Dia_XB + w);
    d->Dia_YB = static_cast<uint16_t>(d->Dia_YB + 8);
    z->Dia_ZoSx = static_cast<uint16_t>(w << 3);
    z->Dia_ZoSy = 8;
    z->Dia_ZoVar = d->Dia_NextZone;
    return w;
}

// WiCall WindOp: window number at x,y, w x h characters
uint32_t open_window(uint32_t number, int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    Regs r = {};
    r.d[1] = number;
    r.d[2] = static_cast<uint32_t>(x);
    r.d[3] = static_cast<uint32_t>(y);
    r.d[4] = w;
    r.d[5] = h;
    r.d[6] = 0;
    r.d[7] = 0;     // no border
    wi_call(wi::WindOp, r);
    return r.d[0];
}

// "Cursor off, paper, pen, clear, no scroll" for the windows of the zones
void window_init(uint8_t paper, uint8_t pen)
{
    char s[] = "\x1b" "C0\x1b" "B0\x1b" "P0\x1b" "JO\x19\x1b" "V0";
    s[5] = static_cast<char>('0' + paper);
    s[8] = static_cast<char>('0' + pen);
    print(s);
}

// Dia_Edit2: the rest of EDit and DIgit: copies text into store, opens the
// window, reads paper and pen and starts the line editor
void edit_start(Run &r, uint8_t *e, uint32_t s, uint8_t *store, uint8_t flags, uint16_t max, int32_t wx, int32_t wy,
                uint16_t width)
{
    Dialog *d = r.d;
    uint8_t *to = store;
    if (s) {
        uint16_t n = string_length(s);
        if (n >= max)
            n = max;
        const uint8_t *from = string_chars(s);
        while (n--) {
            uint8_t c = *from++;
            if (c >= ' ')
                *to++ = c;
        }
    }
    *to = 0;
    uint32_t number = zone(e)->Dia_ZoNumber + 1000u;
    d->Dia_WindOn = static_cast<uint16_t>(number);
    open_window(static_cast<uint16_t>(number), wx, wy, width, 1);
    uint8_t paper = static_cast<uint8_t>(evaluate(r));
    uint8_t pen = static_cast<uint8_t>(evaluate(r));
    window_init(paper, pen);
    if (!led_init(line_edit(e), store, store, flags, max, -1))
        bad_call(r);
    keep_buffer(d);
}

constexpr uint8_t EditFlags = LEdMouCur | LEdMouse | LEdOnce | LEdKeys;

// EDit number,x,y,width,max,text,paper,pen
void edit_zone(Run &r)
{
    Dialog *d = r.d;
    uint8_t *e = get_buffer(r, DiaEdit::Dia_EdLong);
    poke_w(e + 2, EdMark);
    d->Dia_LastZone = address(e);
    int32_t wx, wy;
    uint16_t width = window_zone_header(r, e, wx, wy);
    int32_t max = evaluate(r);
    if (!max || static_cast<uint32_t>(max) > 1024)
        bad_call(r);
    uint8_t *store = get_buffer(r, static_cast<uint16_t>(max + 6));
    poke_w(store + 2, StMark);
    uint32_t s = static_cast<uint32_t>(evaluate(r));
    edit_start(r, e, s, store + 4, EditFlags, static_cast<uint16_t>(max), wx, wy, width);
}

// DIgit number,x,y,width,value,show,paper,pen
void digit_zone(Run &r)
{
    Dialog *d = r.d;
    uint8_t *e = get_buffer(r, DiaEdit::Dia_DiLong);
    poke_w(e + 2, EdMark);
    d->Dia_LastZone = address(e);
    int32_t wx, wy;
    uint16_t width = window_zone_header(r, e, wx, wy);
    edit(e)->Dia_DiValue = static_cast<uint32_t>(evaluate(r));
    uint32_t s = 0;
    if (evaluate(r) & 1) {
        push(r, edit(e)->Dia_DiValue);
        to_decimal(r, 1);
        s = pop(r);
    }
    edit_start(r, e, s, edit(e)->Dia_DiBuffer, EditFlags | LEdFilter, static_cast<uint16_t>(width - 1), wx, wy, width);
    // Digits, "+", "-" and "/"... only
    LineEdit *l = line_edit(e);
    l->LEd_Mask[0] = 0x0015FFC0;
    l->LEd_Mask[1] = 0;
    l->LEd_Mask[2] = 0;
}

// AList / IList number,x,y,width,height,array,position,flags,paper,pen;[]
void list_zone(Run &r)
{
    Dialog *d = r.d;
    uint8_t *e = get_buffer(r, DiaList::Dia_LiLong);
    poke_w(e + 2, LiMark);
    DiaList *l = list(e);
    int32_t wx, wy;
    uint16_t width = window_zone_header(r, e, wx, wy);
    l->Dia_LiTx = width;
    uint16_t height = static_cast<uint16_t>(evaluate(r));
    l->Dia_LiTy = height;
    zone(e)->Dia_ZoSy = static_cast<uint16_t>(height << 3);
    uint32_t a = static_cast<uint32_t>(evaluate(r));
    uint16_t count = 0;
    if (a) {
        if (static_cast<int32_t>(a) <= 1024)
            bad_call(r);
        l->Dia_LiArray = a;
        const uint8_t *p = ptr<uint8_t>(a);
        if (p[0] != 1 || p[1] != 2 || peek_w(p + 2) >= 32768)
            bad_call(r);
        count = peek_w(p + 2);
    }
    l->Dia_LiMaxAct = count;
    uint16_t position = static_cast<uint16_t>(evaluate(r));
    l->Dia_LiPos = position < count ? position : count;
    l->Dia_LiActNumber = 0xFFFF;
    zone(e)->Dia_ZoPos = -1u;
    uint32_t number = zone(e)->Dia_ZoNumber + 2000u;
    d->Dia_WindOn = static_cast<uint16_t>(number);
    if (open_window(static_cast<uint16_t>(number), wx, wy, width, height))
        bad_call(r);
    zone_flags_low(e) = static_cast<uint8_t>(evaluate(r));
    uint8_t paper = static_cast<uint8_t>(evaluate(r));
    uint8_t pen = static_cast<uint8_t>(evaluate(r));
    window_init(paper, pen);
    keep_buffer(d);
    zone(e)->Dia_ZoRChange = routine(r);
    // The width of the numbers
    uint8_t digits[12];
    l->Dia_LiLArray = static_cast<uint16_t>(decimal(count, digits) - digits);
    list_draw(r, e);
}

// HText number,x,y,width,height,text,position,zones,paper,pen;[]
void hypertext_zone(Run &r)
{
    Dialog *d = r.d;
    uint8_t *e = get_buffer(r, DiaText::Dia_TxLong);
    poke_w(e + 2, TxMark);
    DiaText *t = text(e);
    int32_t wx, wy;
    uint16_t width = window_zone_header(r, e, wx, wy);
    t->Dia_TxTx = width;
    uint16_t height = static_cast<uint16_t>(evaluate(r));
    t->Dia_TxTy = height;
    zone(e)->Dia_ZoSy = static_cast<uint16_t>(height << 3);
    int32_t a = evaluate(r);
    if (a <= 1024)
        bad_call(r);
    t->Dia_TxText = static_cast<uint32_t>(a);
    t->Dia_TxPos = static_cast<uint16_t>(evaluate(r));
    uint32_t number = zone(e)->Dia_ZoNumber + 3000u;
    d->Dia_WindOn = static_cast<uint16_t>(number);
    if (open_window(static_cast<uint16_t>(number), wx, wy, width, height))
        bad_call(r);
    uint16_t zones = static_cast<uint16_t>(evaluate(r));
    t->Dia_TxDispMax = zones;
    t->Dia_TxDispSize = static_cast<uint16_t>(zones * TxDispZone + 4 + 4);
    uint8_t paper = static_cast<uint8_t>('0' + evaluate(r));
    uint8_t pen = static_cast<uint8_t>('0' + evaluate(r));
    t->Dia_TxPaper = paper;
    t->Dia_TxPen = pen;
    const char pp[8] = {27, 'B', static_cast<char>(paper), 27, 'P', static_cast<char>(pen), 0, 0};
    for (int i = 0; i < 8; i++)
        t->Dia_TxPp[i] = static_cast<uint8_t>(pp[i]);
    print(reinterpret_cast<const char *>(t->Dia_TxPp));
    char clw[] = "\x1b" "C0\x19\x1b" "T0\x1b" "V0";
    clw[6] = static_cast<char>('0' + idata().CurTab);
    print(clw);
    zone(e)->Dia_ZoRChange = routine(r);
    // The lines: (offset << 8) | length, in an entry of their own
    uint32_t limit = address(r.sp) - 16;
    const uint8_t *s = ptr<uint8_t>(t->Dia_TxText);
    uint8_t *lines = entry_at(d->Dia_ABuffer) + 4;
    t->Dia_TxAdress = address(lines);
    uint8_t *p = lines;
    uint16_t count = 0;
    const uint8_t *start = s, *end = s;
    for (;;) {
        start = end = s;
        if (address(p) >= limit)
            fail(r, EDiaOBuffer);
        count++;
        poke_l(p, static_cast<uint32_t>(s - ptr<uint8_t>(t->Dia_TxText)) << 8);
        p += 4;
        uint8_t c;
        for (;;) {
            c = *s++;
            if (c >= ' ')
                continue;
            if (!c || c == 10 || c == 13)
                break;
            if (c == 27)
                s += 2;
        }
        if (!c)
            break;
        // (13 is taken as 10: CR LF gives an empty line)
        end = s - 1;
        if (*s == 13)
            s++;
        p[-1] = static_cast<uint8_t>(end - start);
    }
    // The last line gets the length 0, as in the assembler: it never shows
    p[-1] = 0;
    t->Dia_TxNLine = count;
    d->Dia_NextZone = count;
    uint8_t *lines_entry = entry_at(d->Dia_ABuffer);
    d->Dia_ABuffer = address(p);
    poke_l(p, 0);
    poke_w(lines_entry, static_cast<uint16_t>(p - lines_entry));
    poke_w(lines_entry + 2, TaMark);
    uint8_t *display = get_buffer(r, static_cast<uint16_t>(t->Dia_TxTy * t->Dia_TxDispSize + 4));
    poke_w(display + 2, TdMark);
    t->Dia_TxDisplay = address(display + 4);
    keep_buffer(d);
    text_draw(r, e);
}

// HSlider / VSlider number,x,y,width,height,position,window,total,step;[]
void slider_zone(Run &r, bool vertical)
{
    Dialog *d = r.d;
    uint8_t *e = zone_header(r, DiaSlider::Dia_SlLong);
    poke_w(e + 2, SlMark);
    DiaZone *z = zone(e);
    Slider *s = slider(e);
    s->Sl_X = z->Dia_ZoX;
    s->Sl_Y = z->Dia_ZoY;
    s->Sl_Sx = static_cast<uint16_t>(z->Dia_ZoSx - 1);
    s->Sl_Sy = static_cast<uint16_t>(z->Dia_ZoSy - 1);
    s->Sl_Start = s->Sl_X;
    s->Sl_Size = s->Sl_Sx;
    if (vertical) {
        s->Sl_Flags |= 1 << 8;      // Sl_FlagVertical, in the high byte
        s->Sl_Start = s->Sl_Y;
        s->Sl_Size = s->Sl_Sy;
    }
    int32_t position = evaluate(r);
    s->Sl_Position = static_cast<uint16_t>(position);
    z->Dia_ZoPos = static_cast<uint32_t>(position);
    s->Sl_Window = static_cast<uint16_t>(evaluate(r));
    s->Sl_Global = static_cast<uint16_t>(evaluate(r));
    s->Sl_Scroll = static_cast<uint16_t>(evaluate(r));
    s->Sl_Routines = address(&dia_slider_routines);
    for (int i = 0; i < 8; i++) {
        s->Sl_Inactive[i] = d->Dia_SlDefault[i];
        s->Sl_Active[i] = d->Dia_SlDefault[8 + i];
    }
    keep_buffer(d);
    z->Dia_ZoRChange = routine(r);
    slider_draw(s, false);
    hal::set_draw_mode(rastport(), d->Dia_Writing);
}

// BUtton number,x,y,width,height,position,min,max;[draw][change]
void button_zone(Run &r)
{
    Dialog *d = r.d;
    uint8_t *e = zone_header(r, DiaButton::Dia_BtLong);
    poke_w(e + 2, BtMark);
    d->Dia_LastZone = address(e);
    zone(e)->Dia_ZoPos = static_cast<uint32_t>(evaluate(r));
    button(e)->Dia_BtMin = static_cast<uint16_t>(evaluate(r));
    button(e)->Dia_BtMax = static_cast<uint16_t>(evaluate(r));
    reinterpret_cast<uint8_t *>(&zone(e)->Dia_ZoFlags)[0] = 0;
    keep_buffer(d);
    button(e)->Dia_BtRDraw = routine(r);
    zone(e)->Dia_ZoRChange = routine(r);
    button_draw(r, e);
}

// BC number,position: the other buttons with this number
void button_change(Run &r, uint32_t number, uint32_t position)
{
    Dialog *d = r.d;
    uint8_t *self = current_zone(d);
    if (!self)
        syntax(r);
    if (zone(self)->Dia_ZoFlags & ZoChanging)
        return;
    for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e)) {
        if (entry_id(e) != BtMark || zone(e)->Dia_ZoNumber != static_cast<uint16_t>(number) || e == self)
            continue;
        zone(e)->Dia_ZoFlags |= ZoChanging;
        if (zone(e)->Dia_ZoPos != position) {
            zone(e)->Dia_ZoPos = position;
            button_draw(r, e);
            zone_change(r, e);
        }
        zone(e)->Dia_ZoFlags &= ~(ZoChanging | ZoQuit);
    }
}

// The instructions that take at most six parameters, in d2-d7
void instruction(Run &r, int i, const int32_t *p)
{
    Dialog *d = r.d;
    struct RastPort *rp = rastport();
    be_i32 *v = dia_vars(d);
    switch (i) {
    case IUnpack:
        unpack(r, p[0], p[1], p[2]);
        break;
    case ILine:
        hline(r, p[0], p[1], p[2], p[3]);
        break;
    case IVLine:
        vline(r, p[0], p[1], p[2], p[3]);
        break;
    case IBox:
        box(r, p[0], p[1], p[2], p[3], p[4]);
        break;
    case ISize:
        d->Dia_Sx = static_cast<uint32_t>((p[0] + 15) & ~15);
        d->Dia_Sy = static_cast<uint32_t>(p[1]);
        break;
    case IXY:
        set_box(d, p[0], p[1], p[2], p[3]);
        break;
    case IBase:
        d->Dia_BaseX = static_cast<uint32_t>(p[0] & ~15);
        d->Dia_BaseY = static_cast<uint32_t>(p[1]);
        break;
    case IPuzzle:
        d->Dia_PuzzleI = static_cast<uint32_t>(p[0]);
        break;
    case ISetZoneVar:
        d->Dia_NextZone = static_cast<uint32_t>(p[0]);
        break;
    case ISetVar:
        // The number times 4 as a word, sign extended
        *reinterpret_cast<be_i32 *>(reinterpret_cast<uint8_t *>(v) + static_cast<int16_t>(p[0] << 2)) = p[1];
        keep_buffer(d);
        break;
    case ICall: {
        AllRegs a = {};
        for (int k = 0; k < 6; k++)
            a.d[2 + k] = static_cast<uint32_t>(p[k]);
        a.a[3] = address(r.sp);
        a.a[4] = address(d);
        a.a[5] = address(ctx().a5);
        a.a[6] = address(r.pc);
        amos_call_all(&a, host::decode_routine(static_cast<uint32_t>(p[0])));
        break;
    }
    case IScreenMove: {
        Screen *s = ptr<Screen>(wdata().EcCourant);
        Regs m = {};
        sy_call(sy::XyMou, m);
        uint16_t offset = static_cast<uint16_t>(m.d[2] - s->EcAWY);
        do {
            sy_call(sy::XyMou, m);
            uint16_t y = static_cast<uint16_t>(m.d[2] - offset);
            if (y != s->EcAWY) {
                s->EcAWY = y;
                reinterpret_cast<uint8_t *>(&s->EcAW)[0] |= 1 << 2;
                ec(ec::CopForce);
                dia_wait_mul();
            }
        } while (mouse_key() & 1);
        break;
    }
    case IIllegal:
        // A debugger trap in the assembler
    case IUser:
        // UI defines instructions: running into one is an error
        syntax(r);
    case IPrint:
        text_at(r, p[0], p[1], static_cast<uint32_t>(p[2]), p[3]);
        break;
    case IOutline:
        set_writing(d, 0);
        text_at(r, p[0] - 1, p[1], static_cast<uint32_t>(p[2]), p[3]);
        text_at(r, p[0] + 1, p[1], static_cast<uint32_t>(p[2]), -1);
        text_at(r, p[0], p[1] - 1, static_cast<uint32_t>(p[2]), -1);
        text_at(r, p[0], p[1] + 1, static_cast<uint32_t>(p[2]), -1);
        text_at(r, p[0], p[1], static_cast<uint32_t>(p[2]), p[4]);
        break;
    case IVText:
        vtext(r, p[0], p[1], static_cast<uint32_t>(p[2]), p[3]);
        break;
    case IInk:
        if (p[2] >= 0)
            rp->AOlPen = static_cast<UBYTE>(p[2]);
        if (p[1] >= 0)
            hal::set_b_pen(rp, static_cast<ULONG>(p[1]));
        if (p[0] >= 0)
            hal::set_a_pen(rp, static_cast<ULONG>(p[0]));
        break;
    case ISetFont:
        if (p[0] >= 0)
            ec(ec::SFont, static_cast<uint32_t>(p[0]));
        if (p[1] >= 0)
            rastport()->AlgoStyle = static_cast<UBYTE>(p[1]);
        break;
    case ISetWriting:
        if (p[0] >= 0)
            set_writing(d, static_cast<uint16_t>(p[0]));
        break;
    case ISetLine:
        if (p[0] >= 0)
            rp->LinePtrn = static_cast<UWORD>(p[0]);
        break;
    case ISetPattern:
        if (p[1] >= 0) {
            rp->Flags &= ~0x10;
            if (static_cast<uint16_t>(p[1]))
                rp->Flags |= 0x10;
        }
        ec(ec::Pattern, static_cast<uint32_t>(p[0]));
        break;
    case IGBox: {
        set_box(d, p[0], p[1], p[2], p[3]);
        int32_t x1 = p[0] + static_cast<int32_t>(d->Dia_BaseX), y1 = p[1] + static_cast<int32_t>(d->Dia_BaseY);
        int32_t x2 = p[2] + static_cast<int32_t>(d->Dia_BaseX), y2 = p[3] + static_cast<int32_t>(d->Dia_BaseY);
        if (x2 <= x1 || y2 <= y1)
            bad_call(r);
        hal::rect_fill(rp, x1, y1, x2, y2);
        break;
    }
    case IGSquare: {
        set_box(d, p[0], p[1], p[2], p[3]);
        int16_t x1 = static_cast<int16_t>(p[0] + d->Dia_BaseX), y1 = static_cast<int16_t>(p[1] + d->Dia_BaseY);
        int16_t x2 = static_cast<int16_t>(p[2] + d->Dia_BaseX), y2 = static_cast<int16_t>(p[3] + d->Dia_BaseY);
        uint8_t *e = get_buffer(r, 16);
        rp = rastport();
        rp->cp_x = x1;
        rp->cp_y = y1;
        // (PolyDraw's points: words of the 68000's)
        be_u16 *points = reinterpret_cast<be_u16 *>(e + 4);
        const WORD square[8] = {x1, y2, x2, y2, x2, y1, x1, y1};
        for (int k = 0; k < 8; k++)
            points[k] = static_cast<uint16_t>(square[k]);
        hal::poly_draw(rp, 4, reinterpret_cast<WORD *>(points));
        break;
    }
    case IGLine:
        set_box(d, p[0], p[1], p[2], p[3]);
        rp->cp_x = static_cast<WORD>(p[0] + d->Dia_BaseX);
        rp->cp_y = static_cast<WORD>(p[1] + d->Dia_BaseY);
        hal::draw(rp, p[2] + static_cast<int32_t>(d->Dia_BaseX), p[3] + static_cast<int32_t>(d->Dia_BaseY));
        break;
    case IGEllipse:
        // (XA/YA get x, XB/YB y)
        if (p[2] <= 0 || p[3] <= 0)
            bad_call(r);
        d->Dia_XA = d->Dia_YA = static_cast<uint16_t>(p[0]);
        d->Dia_XB = d->Dia_YB = static_cast<uint16_t>(p[1]);
        hal::draw_ellipse(rp, p[0] + static_cast<int32_t>(d->Dia_BaseX), p[1] + static_cast<int32_t>(d->Dia_BaseY), p[2],
                    p[3]);
        break;
    case IGPlot:
        // (XA/YA get x, XB/YB y+1, and the base is not added: as in the
        // assembler)
        if (p[2] >= 0)
            hal::set_a_pen(rp, static_cast<ULONG>(p[2]));
        d->Dia_XA = d->Dia_YA = static_cast<uint16_t>(p[0]);
        d->Dia_XB = d->Dia_YB = static_cast<uint16_t>(p[1] + 1);
        hal::write_pixel(rp, p[0], p[1]);
        break;
    case IJump:
    case ILabel:
        r.pc = programs(d) + label_offset(d, static_cast<uint32_t>(p[0]));
        break;
    case IJumpSub:
        push(r, address(r.pc));
        push(r, 0x4D6F694D);    // "MoiM"
        r.pc = programs(d) + label_offset(d, static_cast<uint32_t>(p[0]));
        break;
    case IRts:
        if (pop(r) != 0x4D6F694D)
            syntax(r);
        r.pc = ptr<uint8_t>(pop(r));
        break;
    case IIf: {
        if (next(r).c != '[')
            syntax(r);
        if (p[0])
            loop(r);
        // Skip the block (after it ran too: the loop comes back to its start)
        for (int depth = 1; depth;) {
            uint8_t b = *r.pc++;
            if (!b)
                syntax(r);
            depth += b == '[' ? 1 : b == ']' ? -1 : 0;
        }
        break;
    }
    case IButtonReturn:
        v[-2] = p[0];
        break;
    case IButtonQuit:
    case INoWait: {
        uint8_t *z = current_zone(d);
        if (!z)
            syntax(r);
        zone(z)->Dia_ZoFlags |= i == IButtonQuit ? ZoQuit : ZoNoWait;
        break;
    }
    case IButtonChange:
        button_change(r, static_cast<uint32_t>(p[0]), static_cast<uint32_t>(p[1]));
        break;
    case IZoneChange:
        if (!current_zone(d))
            syntax(r);
        zone_update(r, static_cast<uint32_t>(p[0]), static_cast<uint32_t>(p[1]), EntNul, EntNul);
        break;
    case IKey: {
        uint8_t *e = get_buffer(r, DiaKey::Dia_KyLong);
        poke_w(e + 2, KyMark);
        DiaKey *k = reinterpret_cast<DiaKey *>(e);
        k->Dia_KyCode = static_cast<uint8_t>(p[0]);
        k->Dia_KyShift = static_cast<uint8_t>(p[1]);
        k->Dia_KyZone = d->Dia_LastZone;
        keep_buffer(d);
        break;
    }
    case ISave:
    case IBlock: {
        if (idata().Dia_Magic == 0x4E6F624C)        // "NobL"
            break;
        if (i == ISave) {
            uint8_t *e = get_buffer(r, DiaBlock::Dia_BlLong);
            poke_w(e + 2, BlMark);
            reinterpret_cast<DiaBlock *>(e)->Dia_BlNumber = static_cast<uint16_t>(p[0]);
            keep_buffer(d);
        }
        Regs b = {};
        b.d[1] = static_cast<uint32_t>(p[0]);
        uint32_t x = d->Dia_BaseX;
        b.d[2] = x & 0xFFFFFFF0;
        b.d[3] = d->Dia_BaseY;
        b.d[4] = (d->Dia_Sx + (x & 15) + 15) & 0xFFFFFFF0;
        b.d[5] = d->Dia_Sy;
        b.d[6] = 0;
        ec_call(ec::BlGet, b);
        if (b.d[0])
            syntax(r);
        break;
    }
    case IRun:
        run_events(r, p[0], p[1]);
        break;
    }
}

// The instructions that read their own parameters
void own_instruction(Run &r, int i)
{
    switch (i) {
    case IButton:
        button_zone(r);
        break;
    case IEdit:
        edit_zone(r);
        break;
    case IDigit:
        digit_zone(r);
        break;
    case ISliderH:
    case ISliderV:
        slider_zone(r, i == ISliderV);
        break;
    case IActiveList:
    case IInactiveList:
        list_zone(r);
        break;
    case IHyperText:
        hypertext_zone(r);
        break;
    case ISetSlider: {
        uint8_t *colours = r.d->Dia_SlDefault;
        for (int k = 0; k < 8; k++)
            colours[k] = static_cast<uint8_t>(evaluate(r));
        for (int k = 0; k < 8; k++)
            colours[8 + k] = colours[k];
        break;
    }
    }
}

// A user instruction, defined with UI: its parameters are P1-P9
void user_instruction(Run &r, uint16_t name)
{
    Dialog *d = r.d;
    const uint8_t *l = ptr<uint8_t>(d->Dia_Labels);
    for (;; l += 6) {
        int16_t n = static_cast<int16_t>(peek_w(l));
        if (n < 0)
            syntax(r);
        if (static_cast<uint16_t>(n) == name)
            break;
    }
    if (d->Dia_Users >= 10)
        bad_call(r);
    be_u32 *sp = r.sp;
    const uint8_t *body = programs(d) + peek_w(l + 4);
    uint16_t params = peek_w(l + 2);
    if (params) {
        for (uint16_t k = 0; k < params; k++)
            push(r, static_cast<uint32_t>(evaluate(r)));
    } else {
        next(r);
    }
    const uint8_t *back = r.pc;
    r.pc = body;
    uint32_t users = d->Dia_PUsers;
    uint16_t nusers = d->Dia_NPUsers;
    d->Dia_PUsers = address(r.sp);
    d->Dia_NPUsers = params;
    d->Dia_Users++;
    loop(r);
    d->Dia_Users--;
    d->Dia_NPUsers = nusers;
    d->Dia_PUsers = users;
    r.pc = back;
    r.sp = sp;
}

// Dia_Loop: run the program up to "]", EXit or an error. pc and sp come
// back as they were. Returns the error.
uint16_t loop(Run &r)
{
    Dialog *d = r.d;
    local_jump::Buffer buf;
    local_jump::Buffer *outer = r.quit;
    const uint8_t *pc = r.pc;
    be_u32 *sp = r.sp;
    r.quit = &buf;
    if (!AMOS_LOCAL_SETJMP(buf)) {
        for (;;) {
            if (d->Dia_Error)
                break;
            Chr c = next(r);
            if (c.k == CEnd)
                syntax(r);
            if (c.c == ']')
                break;
            uint16_t name = static_cast<uint16_t>(c.c << 8 | next(r).c);
            int i = instruction_number(name);
            if (i < 0) {
                user_instruction(r, name);
                continue;
            }
            d->Dia_ABuffer = d->Dia_PBuffer;
            uint8_t n = instruction_params[i];
            int32_t p[6] = {0, 0, 0, 0, 0, 0};
            if (n >= 7) {
                own_instruction(r, i);
            } else {
                if (!n)
                    next(r);
                for (uint8_t k = 0; k < n; k++)
                    p[k] = evaluate(r);
                if (i == IEx)
                    break;
                instruction(r, i, p);
            }
            poke_l(entry_at(d->Dia_PBuffer), 0);
        }
    }
    r.quit = outer;
    r.pc = pc;
    r.sp = sp;
    return d->Dia_Error;
}


// ---- services ------------------------------------------------------------------

uint8_t *mem_fast(uint32_t length)
{
    Regs r = {};
    r.d[0] = length;
    r.a[0] = w::reserve(r.d[0], w::Fast);
    return ptr<uint8_t>(r.a[0]);
}

void mem_free(uint32_t address, uint32_t length)
{
    Regs r = {};
    r.a[1] = address;
    r.d[0] = length;
    w::release(r.a[1], r.d[0]);
}

// The monitor's hook to bring its screen to the front, while RUn waits
void screen_front()
{
    if (host::RoutineArg patch = idata().Patch_ScFront.load()) {
        Regs r = {};
        amos_call(&r, patch);
    }
}

// The mouse in the coordinates of the current screen
void mouse_screen(int16_t &x, int16_t &y)
{
    Regs r = {};
    sy_call(sy::XyMou, r);
    r.d[3] = 0;
    sy_call(sy::XyScr, r);
    x = static_cast<int16_t>(r.d[1]);
    y = static_cast<int16_t>(r.d[2]);
}

// ---- windows of the zones ----------------------------------------------------------

// Dia_WActive: make window n current
void window_active(Dialog *d, uint16_t n)
{
    if (n != d->Dia_WindOn) {
        d->Dia_WindOn = n;
        wi(wi::QWindow, n);
    }
}

// Dia_EdActive / Dia_EdOn / Dia_EdInactive: the window of an edit zone,
// with or without its cursor
void edit_on(Dialog *d, uint8_t *e) { window_active(d, static_cast<uint16_t>(zone(e)->Dia_ZoNumber + 1000)); }
void edit_active(Dialog *d, uint8_t *e)
{
    edit_on(d, e);
    led_cursor_on(line_edit(e));
}
void edit_inactive(uint8_t *e) { led_cursor_off(line_edit(e)); }

// Dia_EdEnd, Dia_LiEnd, Dia_TxEnd: close the window of a zone
void window_close(Dialog *d, uint16_t n)
{
    window_active(d, n);
    print("\x1b" "J0");     // Dia_WiZero
    wi(wi::WinDel);
    d->Dia_WindOn = 0xFFFF;
}

void list_active(Dialog *d, uint8_t *e) { window_active(d, static_cast<uint16_t>(zone(e)->Dia_ZoNumber + 2000)); }
void text_active(Dialog *d, uint8_t *e) { window_active(d, static_cast<uint16_t>(zone(e)->Dia_ZoNumber + 3000)); }

// Dia_EdNext / Dia_EdFirst: edit the next edit zone (after e) / the first
void edit_next(Dialog *d, uint8_t *e)
{
    for (; entry_length(e); e += entry_length(e))
        if (entry_id(e) == EdMark) {
            d->Dia_Edited = address(e);
            edit_active(d, e);
            return;
        }
}

void edit_first(Dialog *d)
{
    d->Dia_Edited = 0;
    edit_next(d, entry_at(d->Dia_Buffer));
}

// ---- running zone routines -------------------------------------------------------

// What a zone routine may change, put back afterwards
struct Saved {
    be_u32 base_x, base_y, sx, sy;
    be_u16 xa, ya, xb, yb;
    be_i32 v1, v2, v3;
};

Saved save_state(Dialog *d)
{
    be_i32 *v = dia_vars(d);
    return {d->Dia_BaseX, d->Dia_BaseY, d->Dia_Sx, d->Dia_Sy, d->Dia_XA, d->Dia_YA, d->Dia_XB, d->Dia_YB,
            v[-1], v[-2], v[-3]};
}

void restore_state(Dialog *d, const Saved &s)
{
    be_i32 *v = dia_vars(d);
    d->Dia_BaseX = s.base_x;
    d->Dia_BaseY = s.base_y;
    d->Dia_Sx = s.sx;
    d->Dia_Sy = s.sy;
    d->Dia_XA = s.xa;
    d->Dia_YA = s.ya;
    d->Dia_XB = s.xb;
    d->Dia_YB = s.yb;
    v[-1] = s.v1;
    v[-2] = s.v2;
    v[-3] = s.v3;
}

// Dia_BtDraw: the drawing routine of a button, in the button's box
void button_draw(Run &r, uint8_t *e)
{
    Dialog *d = r.d;
    uint16_t routine = button(e)->Dia_BtRDraw;
    if (!routine)
        return;
    Saved s = save_state(d);
    DiaZone *z = zone(e);
    d->Dia_Sx = z->Dia_ZoSx;
    d->Dia_Sy = z->Dia_ZoSy;
    d->Dia_BaseX = z->Dia_ZoX;
    d->Dia_BaseY = z->Dia_ZoY;
    dia_vars(d)[-1] = static_cast<int32_t>(z->Dia_ZoPos);
    dia_vars(d)[-3] = static_cast<int32_t>(address(e));
    const uint8_t *pc = r.pc;
    r.pc = programs(d) + routine;
    loop(r);
    r.pc = pc;
    restore_state(d, s);
}

// Dia_ZoChange: the change routine of a zone. Returns the position it
// leaves in the BR variable (or the zone's position).
uint32_t zone_change(Run &r, uint8_t *e)
{
    Dialog *d = r.d;
    DiaZone *z = zone(e);
    uint32_t position = z->Dia_ZoPos;
    if (!z->Dia_ZoRChange)
        return position;
    Saved s = save_state(d);
    be_i32 *v = dia_vars(d);
    v[-1] = v[-2] = static_cast<int32_t>(position);
    v[-3] = static_cast<int32_t>(address(e));
    z->Dia_ZoFlags &= ~(ZoQuit | ZoNoWait);
    const uint8_t *pc = r.pc;
    r.pc = programs(d) + z->Dia_ZoRChange;
    loop(r);
    r.pc = pc;
    position = static_cast<uint32_t>(v[-2]);
    restore_state(d, s);
    return position;
}

// ---- lists --------------------------------------------------------------------------

// Dia_LiAff: line n of a list
void list_line(Run &r, uint8_t *e, uint16_t n)
{
    Dialog *d = r.d;
    DiaList *l = list(e);
    int16_t row = static_cast<int16_t>(n - l->Dia_LiPos);
    if (row < 0 || static_cast<uint16_t>(row) >= l->Dia_LiTy)
        return;
    locate(0, static_cast<uint16_t>(row));
    print(n == l->Dia_LiActNumber ? "\x1b" "I1\x1a" : "\x1b" "I0\x1a");
    uint32_t a = l->Dia_LiArray;
    if (!a || n >= peek_w(ptr<uint8_t>(a) + 2))
        return;
    uint16_t prefix = 0;
    if (zone_flags_low(e) & 1) {
        // The number of the line, right aligned
        uint8_t *start = entry_at(d->Dia_ABuffer) + 4;
        uint8_t *to = decimal(n + (zone_flags_low(e) & 2 ? 1 : 0), start);
        int16_t spaces = static_cast<int16_t>(l->Dia_LiLArray - (to - start));
        *to++ = ' ';
        *to++ = '-';
        *to++ = ' ';
        *to = 0;
        prefix = static_cast<uint16_t>(to - entry_at(d->Dia_ABuffer) - 2);
        for (; spaces > 0; spaces--)
            chr_out(' ');
        print(reinterpret_cast<const char *>(start));
    }
    const uint8_t *chars;
    uint16_t length;
    if (peek_l(ptr<uint8_t>(a) + 4) == 0x4D67496B) {
        // "MgIk": a routine gives the line (the file selector's)
        AllRegs c = {};
        c.d[4] = n;
        c.d[6] = prefix;
        c.a[2] = address(e);
        c.a[3] = address(r.sp);
        c.a[4] = address(d);
        c.a[5] = address(ctx().a5);
        c.a[6] = address(r.pc);
        amos_call_all(&c, host::decode_routine(peek_l(ptr<uint8_t>(a) + 8)));
        chars = ptr<uint8_t>(c.a[1]);
        length = static_cast<uint16_t>(c.d[1]);
    } else {
        uint32_t st = peek_l(ptr<uint8_t>(a) + 6 + 4 * n);
        if (static_cast<int32_t>(st) <= 8 * 1024)
            return;
        length = peek_w(ptr<void>(st));
        chars = ptr<uint8_t>(st) + 2;
    }
    int16_t room = static_cast<int16_t>(l->Dia_LiTx - prefix);
    if (static_cast<int16_t>(length) > room)
        length = static_cast<uint16_t>(room);
    Regs w = {};
    w.a[1] = address(chars);
    w.d[1] = length;
    wi_call(wi::Print2, w);
}

// Dia_LiDraw: the visible lines
void list_draw(Run &r, uint8_t *e)
{
    DiaList *l = list(e);
    uint16_t n = l->Dia_LiPos, rows = l->Dia_LiTy;
    do
        list_line(r, e, n++);
    while (--rows);
}

// ---- hypertext ----------------------------------------------------------------------

// The display table of a line: the line's address, then 8 bytes for each
// active word: its start and end in the line, its first and last column,
// its pen and paper (digits), the start of its answer
uint8_t *display_line(DiaText *t, uint16_t row)
{
    return ptr<uint8_t>(t->Dia_TxDisplay) + static_cast<int16_t>(row * t->Dia_TxDispSize);
}

// .PaPe: a decimal number as a colour digit; *end gets the character after
uint8_t colour_digit(uint8_t *&p, uint8_t *end)
{
    uint32_t n = 0;
    for (;;) {
        uint8_t c = *p++;
        uint8_t v = static_cast<uint8_t>(c - '0');
        if (c < '0' || v > 9) {
            *end = c;
            return static_cast<uint8_t>(n + '0');
        }
        n = n * 10 + v;
    }
}

// Dia_TxAff: line n of a hypertext. "{[answer,paper,pen]text}" are the
// active words.
void text_line(uint8_t *e, uint16_t n)
{
    DiaText *t = text(e);
    int16_t row = static_cast<int16_t>(n - t->Dia_TxPos);
    if (row < 0 || static_cast<uint16_t>(row) >= t->Dia_TxTy)
        return;
    uint8_t *disp = display_line(t, static_cast<uint16_t>(row));
    poke_l(disp, 0);
    poke_w(disp + 4, 0);
    locate(0, static_cast<uint16_t>(row));
    print(reinterpret_cast<const char *>(t->Dia_TxPp));
    chr_out(26);
    if (n >= t->Dia_TxNLine)
        return;
    const uint8_t *table = ptr<uint8_t>(t->Dia_TxAdress) + static_cast<int16_t>(n << 2);
    uint8_t *line = ptr<uint8_t>(t->Dia_TxText) + (peek_l(table) >> 8);
    poke_l(disp, address(line));
    disp += 4;
    uint8_t *end = line + table[3];
    uint8_t end_char = *end;
    *end = 0;
    uint16_t active = 0;
    uint8_t *seg = line;
    for (;;) {
        // Normal text up to "{" or the end
        uint8_t *a = seg;
        while (*a && *a != '{')
            a++;
        uint8_t c = *a;
        *a = 0;
        uint16_t x = print_limited(seg, t->Dia_TxTx);
        *a = c;
        if (x >= t->Dia_TxTx || c != '{')
            break;
        // An active word
        uint8_t *p = a + 1;
        bool ok = active < t->Dia_TxDispMax && *p == '[';
        bool keyword = false;
        if (ok) {
            p++;
            disp[4] = t->Dia_TxPen;
            disp[5] = t->Dia_TxPaper;
            Regs w = {};
            wi_call(wi::XYCuWi, w);
            disp[2] = static_cast<uint8_t>(w.d[1]);
            disp[6] = static_cast<uint8_t>(p - line);
            keyword = *p != ',';
            for (;;) {
                uint8_t b = *p++;
                if (!b || b == ']') {
                    ok = b;
                    break;
                }
                if (b == ',') {
                    uint8_t stop;
                    disp[4] = colour_digit(p, &stop);
                    if (stop != ',') {
                        ok = false;
                        break;
                    }
                    disp[5] = colour_digit(p, &stop);
                    ok = stop == ']';
                    break;
                }
            }
        }
        if (!ok) {
            // Not an active word: "{" is text
            poke_l(disp, 0);
            poke_w(disp + 4, 0);
            seg = a;
            uint8_t *b = a + 1;
            while (*b && *b != '{')
                b++;
            // print from "{" up to the next one as normal text
            uint8_t cb = *b;
            *b = 0;
            x = print_limited(seg, t->Dia_TxTx);
            *b = cb;
            if (x >= t->Dia_TxTx || cb != '{')
                break;
            seg = b;
            continue;
        }
        uint8_t *word = p;
        uint8_t *we = word;
        while (*we && *we != '}')
            we++;
        // Without "}" the end is after the zero, as in the assembler
        if (!*we)
            we++;
        disp[0] = static_cast<uint8_t>(word - line);
        disp[1] = static_cast<uint8_t>(we - line);
        print_colours(disp[4], disp[5]);
        uint8_t wc = *we;
        *we = 0;
        x = print_limited(word, t->Dia_TxTx);
        *we = wc;
        active++;
        disp[3] = static_cast<uint8_t>(x);
        disp[7] = 0;
        if (!keyword) {
            // No answer: never active
            disp[2] = 1;
            disp[3] = 0;
        }
        disp += TxDispZone;
        poke_w(disp, 0);
        if (x >= t->Dia_TxTx || wc != '}')
            break;
        seg = we + 1;
        print(reinterpret_cast<const char *>(t->Dia_TxPp));
    }
    *end = end_char;
}

// Dia_TxDraw: the visible lines
void text_draw(Run &, uint8_t *e)
{
    DiaText *t = text(e);
    uint16_t n = t->Dia_TxPos, rows = t->Dia_TxTy;
    do
        text_line(e, n++);
    while (--rows);
    t->Dia_TxAct = 0;
}

// Dia_TxAffActive: an active word, highlighted if it is TxAct
void text_word(uint8_t *e, uint8_t *word, uint16_t row)
{
    DiaText *t = text(e);
    uint8_t *line = ptr<uint8_t>(peek_l(display_line(t, row)));
    Regs l = {};
    l.d[1] = word[2];
    l.d[2] = row;
    wi_call(wi::Locate, l);
    if (l.d[0])
        return;
    bool lit = address(word) == t->Dia_TxAct;
    print_colours(lit ? word[5] : word[4], lit ? word[4] : word[5]);
    uint8_t *end = line + word[1];
    uint8_t c = *end;
    *end = 0;
    print_limited(line + word[0], t->Dia_TxTx);
    *end = c;
}

// ---- sliders and buttons (shared with the editor) -------------------------------

void slider_draw(Slider *s, bool active)
{
    const be_u16 *colours = active ? s->Sl_Active : s->Sl_Inactive;
    Regs c = {};
    for (int i = 0; i < 8; i++)
        c.d[i] = static_cast<uint32_t>(static_cast<int16_t>(colours[i]));
    ec_call(ec::SetSli, c);
    hal::set_draw_mode(rastport(), 1);
    auto words = [s](Regs &w) {
        const be_u16 *v = &s->Sl_Sx;
        for (int i = 0; i < 7; i++)
            w.d[1 + i] = static_cast<uint32_t>(static_cast<int16_t>(v[i]));
    };
    bool vertical = s->Sl_Flags & 1 << 8;
    Regs w = {};
    words(w);
    ec_call(vertical ? ec::VerSli : ec::HorSli, w);
    Regs p = {};
    words(p);
    p.d[0] = vertical ? p.d[2] : p.d[1];
    ec_call(ec::PourSli, p);
    s->Sl_Mouse1 = static_cast<uint16_t>(p.d[6]);
    s->Sl_Mouse2 = static_cast<uint16_t>(p.d[6] + p.d[7]);
}

// Dia_SlDraw: the slider of a zone
void zone_slider_draw(Run &r, uint8_t *e)
{
    slider_draw(slider(e), false);
    hal::set_draw_mode(rastport(), r.d->Dia_Writing);
}

// Sl_Call: routine offset of a slider (0 update, 4 back, 8 on, 12 moved).
// The sliders of dialogs call their change routine.
void slider_call(Slider *s, uint16_t offset, Run *r)
{
    if (!s->Sl_Routines)
        return;
    if (s->Sl_Routines == address(dia_slider_routines)) {
        if (offset && r) {
            uint8_t *e = slider_zone(s);
            zone(e)->Dia_ZoPos = s->Sl_Position;
            zone_change(*r, e);
        }
        return;
    }
    Regs c = {};
    c.d[0] = offset;
    c.a[0] = s->Sl_Routines;
    c.a[2] = address(s);
    amos_call(&c, host::decode_routine(uint32_t(s->Sl_Routines) + uint32_t(offset)));
}

// Sl_XYMouEc: the mouse along the slider
int16_t slider_mouse(Slider *s)
{
    int16_t x, y;
    mouse_screen(x, y);
    return s->Sl_Flags & 1 << 8 ? y : x;
}

// Sl_Clic: the left button on a slider: moves it while the button is down
void slider_click(Slider *s, Run *r)
{
    slider_call(s, 0, r);
    int16_t m = slider_mouse(s);
    uint16_t at = static_cast<uint16_t>(m - s->Sl_Start);
    if (at < s->Sl_Mouse1) {
        // Back by Sl_Scroll, repeating with the right button
        do {
            slider_draw(s, true);
            int16_t p = static_cast<int16_t>(s->Sl_Position - s->Sl_Scroll);
            if (p < 0)
                p = 0;
            if (static_cast<uint16_t>(p) != s->Sl_Position) {
                s->Sl_Position = static_cast<uint16_t>(p);
                slider_call(s, 4, r);
                dia_patch();
            }
            uint32_t k;
            while ((k = mouse_key()) & 1 && !(k & 2))
                dia_wait_mul();
            if (!(k & 1))
                break;
        } while (true);
    } else if (at >= s->Sl_Mouse2) {
        do {
            slider_draw(s, true);
            uint16_t p = static_cast<uint16_t>(s->Sl_Position + s->Sl_Scroll);
            if (static_cast<uint16_t>(p + s->Sl_Window) > s->Sl_Global) {
                int16_t q = static_cast<int16_t>(s->Sl_Global - s->Sl_Window);
                p = static_cast<uint16_t>(q < 0 ? 0 : q);
            }
            if (p != s->Sl_Position) {
                s->Sl_Position = p;
                slider_call(s, 8, r);
                dia_patch();
            }
            uint32_t k;
            while ((k = mouse_key()) & 1 && !(k & 2))
                dia_wait_mul();
            if (!(k & 1))
                break;
        } while (true);
    } else {
        // Drag the middle
        int16_t grab = static_cast<int16_t>(at - s->Sl_Mouse1);
        slider_draw(s, true);
        for (;;) {
            int16_t v = static_cast<int16_t>(m - grab);
            if (v < 0)
                v = 0;
            v = static_cast<int16_t>(v - s->Sl_Start);
            if (v < 0)
                v = 0;
            uint32_t prod = static_cast<uint32_t>(static_cast<uint16_t>(v)) * static_cast<uint16_t>(s->Sl_Global + 1);
            uint16_t div = static_cast<uint16_t>(s->Sl_Size + 1);
            // divu: on an overflow the product stays
            uint16_t p = div && prod / div <= 0xFFFF ? static_cast<uint16_t>(prod / div) : static_cast<uint16_t>(prod);
            int16_t max = static_cast<int16_t>(s->Sl_Global - s->Sl_Window);
            if (max < 0)
                max = 0;
            if (static_cast<int16_t>(p) > max)
                p = static_cast<uint16_t>(max);
            if (p != s->Sl_Position) {
                s->Sl_Position = p;
                slider_draw(s, true);
                if (mouse_key() & 2)
                    slider_call(s, 12, r);
            }
            if (!(mouse_key() & 1))
                break;
            dia_wait_mul();
            dia_patch();
            m = slider_mouse(s);
        }
        slider_call(s, 12, r);
        dia_patch();
    }
    slider_draw(s, false);
}

// Sl_Init: the zone of a slider, then draws it
void slider_init(Slider *s)
{
    if (s->Sl_Zone) {
        Regs z = {};
        z.d[1] = s->Sl_Zone;
        z.d[2] = static_cast<uint16_t>(s->Sl_X - s->Sl_ZDx);
        z.d[3] = static_cast<uint16_t>(s->Sl_Y - s->Sl_ZDy);
        z.d[4] = static_cast<uint16_t>(s->Sl_X + s->Sl_Sx + s->Sl_ZDx);
        z.d[5] = static_cast<uint16_t>(s->Sl_Y + s->Sl_Sy + s->Sl_ZDy);
        sy_call(sy::SetZone, z);
    }
    slider_call(s, 0, nullptr);
    s->Sl_Start = s->Sl_X;
    s->Sl_Size = s->Sl_Sx;
    if (s->Sl_Flags & 1 << 8) {
        s->Sl_Start = s->Sl_Y;
        s->Sl_Size = s->Sl_Sy;
    }
    slider_draw(s, false);
}

// Bt_Call: routine n (1-based) of a button of the editor
void button_call(Button *b, uint8_t n)
{
    if (!n)
        return;
    Regs c = {};
    c.d[0] = static_cast<uint32_t>(static_cast<int8_t>(n)) << 2;
    c.a[0] = host::encode_routine(b->Bt_Routines.load());
    c.a[2] = address(b);
    amos_call(&c, host::decode_routine(host::encode_routine(b->Bt_Routines.load()) + uint32_t(int32_t(int16_t(c.d[0]))) - 4));
}

void button_init(Button *b, bool draw)
{
    button_call(b, b->Bt_RPos);
    if (draw)
        button_call(b, b->Bt_RDraw);
    Regs z = {};
    z.d[1] = b->Bt_Zone;
    z.d[2] = static_cast<uint16_t>(b->Bt_X + b->Bt_Dx);
    z.d[3] = static_cast<uint16_t>(b->Bt_Y + b->Bt_Dy);
    z.d[4] = static_cast<uint16_t>(z.d[2] + b->Bt_Sx);
    z.d[5] = static_cast<uint16_t>(z.d[3] + b->Bt_Sy);
    sy_call(sy::SetZone, z);
}

// Bt_Gere: a click on the buttons from b on. Returns the button, or null.
Button *buttons_click(Button *b, uint16_t *keys)
{
    *keys = static_cast<uint16_t>(mouse_key());
    if (!(*keys & 3))
        return nullptr;
    Regs z = {};
    sy_call(sy::GetZone, z);
    uint16_t zn = static_cast<uint16_t>(z.d[1] >> 16);
    for (;;) {
        if (zn == b->Bt_Zone)
            break;
        b++;
        if (!b->Bt_Number)
            return nullptr;
    }
    constexpr uint8_t FlagNew = 1 << 0, FlagNoWait = 1 << 1, FlagOnOf = 1 << 2;
    if (!(b->Bt_Flags & FlagOnOf)) {
        // One shot: down while the button is held
        b->Bt_Pos ^= 1;
        button_call(b, b->Bt_RDraw);
        dia_no_mkey();
        b->Bt_Pos ^= 1;
        button_call(b, b->Bt_RDraw);
    } else {
        b->Bt_Pos ^= 1;
        button_call(b, b->Bt_RDraw);
        if (!(b->Bt_Flags & FlagNoWait))
            dia_no_mkey();
        b->Bt_Flags |= FlagNew;
    }
    button_call(b, b->Bt_RChange);
    return b;
}

// ---- zone updates ----------------------------------------------------------------

// Dia_ZUpdate: the zones with this number take the values a, b, c
// (EntNul: unchanged)
void zone_update(Run &r, uint32_t number, uint32_t a, uint32_t b, uint32_t c)
{
    Dialog *d = r.d;
    uint8_t *self = current_zone(d);
    for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e)) {
        if (!is_zone(e) || zone(e)->Dia_ZoNumber != static_cast<uint16_t>(number) || e == self)
            continue;
        DiaZone *z = zone(e);
        switch (entry_id(e)) {
        case BtMark:
            if (a != EntNul) {
                if (z->Dia_ZoPos == a)
                    goto quit;
                z->Dia_ZoPos = a;
            }
            button_draw(r, e);
            zone_change(r, e);
            break;
        case SlMark: {
            Slider *s = slider(e);
            if (c != EntNul)
                s->Sl_Global = static_cast<uint16_t>(c);
            if (b != EntNul)
                s->Sl_Window = static_cast<uint16_t>(b);
            if (a != EntNul) {
                z->Dia_ZoPos = a;
                s->Sl_Position = static_cast<uint16_t>(a);
            }
            zone_slider_draw(r, e);
            zone_change(r, e);
            break;
        }
        case LiMark: {
            DiaList *l = list(e);
            if (c != EntNul)
                l->Dia_LiMaxAct = static_cast<uint16_t>(c);
            if (b != EntNul) {
                int16_t old = static_cast<int16_t>(l->Dia_LiActNumber);
                if (old >= 0) {
                    l->Dia_LiActNumber = 0xFFFF;
                    list_active(d, e);
                    list_line(r, e, static_cast<uint16_t>(old));
                }
                l->Dia_LiActNumber = static_cast<uint16_t>(b);
                z->Dia_ZoPos = b;
            }
            if (a != EntNul)
                l->Dia_LiPos = static_cast<uint16_t>(a);
            list_active(d, e);
            list_draw(r, e);
            break;
        }
        case TxMark:
            if (a == EntNul)
                continue;
            text(e)->Dia_TxPos = static_cast<uint16_t>(a);
            text_active(d, e);
            text_draw(r, e);
            break;
        case EdMark: {
            if (a == EntNul)
                continue;
            uint32_t s;
            if (entry_length(e) == DiaEdit::Dia_EdLong) {
                if (static_cast<int32_t>(a) <= 1024)
                    bad_call(r);
                s = a;
            } else {
                push(r, a);
                to_decimal(r, 1);
                s = pop(r);
            }
            edit_on(d, e);
            led_new(line_edit(e), string_chars(s), string_length(s));
            continue;
        }
        default:
            continue;
        }
    quit:
        z->Dia_ZoFlags &= ~ZoQuit;
    }
    dia_patch();
}

// ---- events -------------------------------------------------------------------------

// Dia_GetZ: the zone at x,y of the screen; x,y become relative to it.
// (The assembler also looked at the end marker of the list.)
uint8_t *zone_at(Dialog *d, int16_t &x, int16_t &y)
{
    for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e)) {
        if (!is_zone(e))
            continue;
        DiaZone *z = zone(e);
        uint16_t ux = static_cast<uint16_t>(x), uy = static_cast<uint16_t>(y);
        if (ux >= z->Dia_ZoX && ux < static_cast<uint16_t>(z->Dia_ZoX + z->Dia_ZoSx) && uy >= z->Dia_ZoY
            && uy < static_cast<uint16_t>(z->Dia_ZoY + z->Dia_ZoSy)) {
            x = static_cast<int16_t>(x - z->Dia_ZoX);
            y = static_cast<int16_t>(y - z->Dia_ZoY);
            return e;
        }
    }
    return nullptr;
}

// The keys of KY, with the shifts of the key
bool key_matches(const DiaKey *k, uint8_t ascii, uint8_t scan, uint8_t shifts)
{
    int8_t code = static_cast<int8_t>(k->Dia_KyCode);
    if (code > 0) {
        if (static_cast<uint8_t>(code) != ascii)
            return false;
    } else if (code < 0) {
        if (code != -1 && (code & 0x7F) != scan)
            return false;
    } else {
        return false;
    }
    static const uint8_t masks[] = {Shf, Ami, Ctr, Alt};
    for (uint8_t m : masks)
        if (!(k->Dia_KyShift & m) != !(shifts & m))
            return false;
    return true;
}

// The left button on an active word of a hypertext (x, y in the zone)
void text_click(Run &r, uint8_t *e, bool click, bool here, int16_t x, int16_t y, be_u16 &exits)
{
    Dialog *d = r.d;
    DiaText *t = text(e);
    uint8_t *found = nullptr;
    uint16_t row = 0;
    if (here) {
        row = static_cast<uint16_t>(y) >> 3;
        uint8_t col = static_cast<uint8_t>(static_cast<uint16_t>(x) >> 3);
        uint8_t *disp = display_line(t, row);
        if (peek_l(disp))
            for (uint8_t *w = disp + 4; peek_w(w); w += TxDispZone)
                if (col >= w[2] && col < w[3]) {
                    found = w;
                    break;
                }
    }
    if (t->Dia_TxAct && t->Dia_TxAct != address(found)) {
        zone(e)->Dia_ZoPos = -1u;
        text_active(d, e);
        uint8_t *old = ptr<uint8_t>(t->Dia_TxAct);
        t->Dia_TxAct = 0;
        text_word(e, old, t->Dia_TxYAct);
        dia_patch();
    }
    if (found && t->Dia_TxAct != address(found)) {
        t->Dia_TxAct = address(found);
        t->Dia_TxYAct = row;
        text_active(d, e);
        text_word(e, found, row);
        dia_patch();
    }
    if (!click || !found)
        return;
    d->Dia_Return = zone(e)->Dia_ZoNumber;
    // The answer: a number, or a word (capitals: the reader skips small
    // letters, as everywhere in the Interface)
    const uint8_t *answer = ptr<uint8_t>(peek_l(display_line(t, row))) + found[6];
    const uint8_t *pc = answer;
    Chr c = next(pc);
    if (c.k == CDigit) {
        zone(e)->Dia_ZoPos = static_cast<uint32_t>(number(pc, c.c));
        t->Dia_TxBuffer[0] = 0;
    } else {
        pc--;
        uint8_t *to = t->Dia_TxBuffer;
        uint8_t *last = reinterpret_cast<uint8_t *>(t) + DiaText::Dia_TxBufferEnd - 2;
        for (;;) {
            uint8_t b = *pc++;
            if (to >= last || b == ',' || b == ']')
                break;
            *to++ = b;
        }
        *to = 0;
        zone(e)->Dia_ZoPos = 0;
    }
    zone_change(r, e);
    dia_patch();
    if (!(zone(e)->Dia_ZoFlags & ZoNoWait)) {
        dia_no_mkey();
        dia_patch();
    }
    zone(e)->Dia_ZoFlags &= ~ZoNoWait;
    (void)exits;
}

// Restart the timer of RUn after an action
void timer_reset(Dialog *d)
{
    // (The assembler read Dia_Timer at the stack pointer, not the channel)
    if (d->Dia_Timer)
        d->Dia_TimerPos = wdata().VblCount;
}

// Dia_Tests: one round of the zones: the edited line, the keys, the mouse.
// Returns the number of reasons to stop (BQ, the flags of RUn).
uint16_t tests(Run &r)
{
    Dialog *d = r.d;
    d->Dia_Exit = 0;
    bool click = false;
    uint8_t *target = nullptr;
    int16_t mx = 0, my = 0;

    // A button of NW held down
    if (uint8_t *e = entry_at(d->Dia_Release)) {
        d->Dia_Return = zone(e)->Dia_ZoNumber;
        if (mouse_key() & 1) {
            zone_change(r, e);
            // (The assembler tested Dia_Flags of the zone, past its end)
            if (zone(e)->Dia_ZoFlags & ZoQuit) {
                zone(e)->Dia_ZoFlags &= ~ZoQuit;
                d->Dia_Exit++;
            }
            return d->Dia_Exit;
        }
        d->Dia_Return = 0;
        d->Dia_Release = 0;
        button_draw(r, e);
        dia_patch();
    }

    uint32_t key = 0;
    bool keyed = false;
    if (d->Dia_Edited && d->Dia_Edited != -1u) {
        timer_reset(d);
        uint8_t *e = entry_at(d->Dia_Edited);
        edit_active(d, e);
        int16_t code = led_loop(line_edit(e), &key);
        bool next_zone = false;
        if (code == 1) {
            d->Dia_Return = zone(entry_at(d->Dia_Edited))->Dia_ZoNumber;
            next_zone = !(d->Dia_Flags & FlagReturnNext);
        } else if (key) {
            dia_patch();
            if (static_cast<uint8_t>(key >> 16) == 0x42)    // Tab
                next_zone = true;
            else
                keyed = true;
        }
        if (next_zone) {
            edit_inactive(e);
            uint8_t *from = entry_at(d->Dia_Edited);
            d->Dia_Edited = -1u;
            edit_next(d, from + entry_length(from));
            dia_clear_key();
            dia_patch();
        }
    } else {
        key = wdata().ClLast;
        keyed = true;
    }

    if (keyed) {
        d->Dia_LastKey = key;
        if (key) {
            timer_reset(d);
            if (d->Dia_Flags & FlagExitKey)
                d->Dia_Exit++;
            uint8_t ascii = static_cast<uint8_t>(key);
            if (ascii >= 'a' && ascii <= 'z')
                ascii = static_cast<uint8_t>(ascii - 0x20);
            uint8_t scan = static_cast<uint8_t>(key >> 16), shifts = static_cast<uint8_t>(key >> 24);
            bool matched = false;
            for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e)) {
                if (entry_id(e) != KyMark || !key_matches(reinterpret_cast<DiaKey *>(e), ascii, scan, shifts))
                    continue;
                matched = true;
                uint32_t z = reinterpret_cast<DiaKey *>(e)->Dia_KyZone;
                if (z) {
                    // As a click on its zone
                    dia_clear_key();
                    click = true;
                    target = entry_at(z);
                    goto zones;
                }
                break;
            }
            if (!matched && d->Dia_Edited == -1u && static_cast<uint8_t>(d->Dia_LastKey >> 16) == 0x42) {
                edit_first(d);
                dia_clear_key();
                dia_patch();
                return d->Dia_Exit;
            }
        }
    }

    // The mouse
    click = mouse_key() & 1;
    mouse_screen(mx, my);
    target = zone_at(d, mx, my);

zones:
    for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e)) {
        if (!is_zone(e))
            continue;
        DiaZone *z = zone(e);
        bool here = e == target;
        switch (entry_id(e)) {
        case BtMark: {
            if (!click || !here)
                break;
            d->Dia_Return = z->Dia_ZoNumber;
            uint32_t p = z->Dia_ZoPos;
            uint16_t v = static_cast<uint16_t>(p + 1);
            if (static_cast<int16_t>(v) > static_cast<int16_t>(button(e)->Dia_BtMax))
                v = button(e)->Dia_BtMin;
            z->Dia_ZoPos = (p & 0xFFFF0000) | v;
            button_draw(r, e);
            uint32_t back = zone_change(r, e);
            dia_patch();
            if (z->Dia_ZoFlags & ZoNoWait) {
                z->Dia_ZoFlags &= ~ZoNoWait;
                z->Dia_ZoPos = back;
                d->Dia_Release = address(e);
            } else {
                dia_no_mkey();
                if (z->Dia_ZoPos != back) {
                    z->Dia_ZoPos = back;
                    button_draw(r, e);
                    dia_patch();
                }
            }
            if (z->Dia_ZoFlags & ZoQuit) {
                z->Dia_ZoFlags &= ~ZoQuit;
                d->Dia_Exit++;
            }
            break;
        }
        case EdMark: {
            if (!click || !here)
                break;
            uint32_t old = d->Dia_Edited;
            d->Dia_Edited = address(e);
            // (The assembler also "stopped" the zone at address 0)
            if (old != address(e) && old != -1u && old) {
                edit_inactive(entry_at(old));
                dia_patch();
            }
            break;
        }
        case SlMark:
            if (!click || !here)
                break;
            d->Dia_Return = z->Dia_ZoNumber;
            slider_click(slider(e), &r);
            hal::set_draw_mode(rastport(), d->Dia_Writing);
            z->Dia_ZoFlags &= ~ZoQuit;
            break;
        case LiMark: {
            DiaList *l = list(e);
            int16_t n = -1;
            if (here) {
                n = static_cast<int16_t>((static_cast<uint16_t>(my) >> 3) + l->Dia_LiPos);
                if (n >= static_cast<int16_t>(l->Dia_LiMaxAct))
                    n = -1;
            }
            bool press = zone_flags_low(e) & 4;
            int16_t old = static_cast<int16_t>(l->Dia_LiActNumber);
            if (old >= 0) {
                if (old == n)
                    goto list_key;
                if (press && (!click || n < 0))
                    goto list_key;
                l->Dia_LiActNumber = 0xFFFF;
                z->Dia_ZoPos = -1u;
                list_active(d, e);
                list_line(r, e, static_cast<uint16_t>(old));
                dia_patch();
            }
            if (press && !click)
                goto list_key;
            if (n >= 0) {
                l->Dia_LiActNumber = static_cast<uint16_t>(n);
                list_active(d, e);
                list_line(r, e, static_cast<uint16_t>(n));
                dia_patch();
            }
        list_key:
            if (!click || n < 0)
                break;
            d->Dia_Return = z->Dia_ZoNumber;
            z->Dia_ZoPos = static_cast<uint16_t>(n);
            zone_change(r, e);
            dia_patch();
            if (z->Dia_ZoFlags & ZoNoWait) {
                z->Dia_ZoFlags &= ~ZoNoWait;
            } else {
                dia_no_mkey();
                dia_patch();
            }
            if (z->Dia_ZoFlags & ZoQuit) {
                z->Dia_ZoFlags &= ~ZoQuit;
                d->Dia_Exit++;
            }
            break;
        }
        case TxMark:
            text_click(r, e, click, here, mx, my, d->Dia_Exit);
            break;
        }
    }
    if (click) {
        timer_reset(d);
        if (d->Dia_Flags & FlagExitMouse)
            d->Dia_Exit++;
    }
    return d->Dia_Exit;
}

// RUn timer,flags: handle the zones until one stops the dialog, the timer
// runs out or Control-C
uint16_t run_events(Run &r, int32_t timer, int32_t flags)
{
    Dialog *d = r.d;
    WData &w = wdata();
    d->Dia_Flags = static_cast<uint8_t>(flags);
    d->Dia_Timer = static_cast<uint32_t>(timer);
    d->Dia_TimerPos = w.VblCount;
    d->Dia_RFlags |= RRun;
    if (flags & 1) {
        Regs k = {};
        sy_call(sy::ClearKey, k);
    }
    if (flags & 2)
        dia_no_mkey();
    edit_first(d);
    uint16_t exits;
    for (;;) {
        boot::wait_vbl_multi();
        if (d->Dia_Timer && w.VblCount - d->Dia_TimerPos >= d->Dia_Timer)
            break;
        if (w.Actualise & BitControl) {
            w.Actualise = static_cast<uint16_t>(w.Actualise & ~BitControl);
            break;
        }
        if ((exits = tests(r))) {
            screen_front();
            return exits;
        }
        screen_front();
    }
    d->Dia_Return = 0;
    screen_front();
    return 0;
}

// ---- channels --------------------------------------------------------------------------

be_u32 *dialogs() { return ptr<be_u32>(idata().Cur_Dialogs); }

// Dia_Active: the dialog's screen becomes current. 0, or EDiaScreen if it
// is not there any more.
uint16_t activate(Dialog *d)
{
    WData &w = wdata();
    if (w.EcAdr[static_cast<int16_t>(d->Dia_ScreenNb)] != d->Dia_Screen)
        return EDiaScreen;
    d->Dia_ScreenOld = 0xFFFF;
    uint16_t current = ptr<Screen>(w.EcCourant)->EcNumber;
    if (current != d->Dia_ScreenNb) {
        d->Dia_ScreenOld = current;
        ec(ec::Active, d->Dia_ScreenNb);
    }
    Regs g = {};
    wi_call(wi::GAdr, g);
    d->Dia_WindOld = d->Dia_WindOn = static_cast<uint16_t>(g.d[1]);
    return 0;
}

// Dia_ReActive: back to the screen and window before
void deactivate(Dialog *d)
{
    if (static_cast<int32_t>(d->Dia_Edited) > 0) {
        edit_active(d, entry_at(d->Dia_Edited));
    } else if (d->Dia_WindOld != d->Dia_WindOn) {
        d->Dia_WindOn = d->Dia_WindOld;
        wi(wi::QWindow, d->Dia_WindOld);
    }
    if (static_cast<int16_t>(d->Dia_ScreenOld) >= 0) {
        ec(ec::Active, d->Dia_ScreenOld);
        d->Dia_ScreenOld = 0xFFFF;
    }
}

// The blocks saved by SAve are put back last first
void put_blocks(uint8_t *e)
{
    for (; entry_length(e); e += entry_length(e))
        if (entry_id(e) == BlMark) {
            put_blocks(e + entry_length(e));
            Regs b = {};
            b.d[1] = reinterpret_cast<DiaBlock *>(e)->Dia_BlNumber;
            b.d[2] = b.d[3] = b.d[4] = b.d[5] = EntNul;
            b.a[1] = idata().Buffer;
            ec_call(ec::BlPut, b);
            Regs x = {};
            x.d[1] = reinterpret_cast<DiaBlock *>(e)->Dia_BlNumber;
            ec_call(ec::BlDel, x);
            return;
        }
}

// Dia_EffChanA0: erase what the dialog shows
void erase(Dialog *d)
{
    if (!(d->Dia_RFlags & RShown))
        return;
    d->Dia_RFlags &= ~RShown;
    if (activate(d))
        return;
    for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e)) {
        switch (entry_id(e)) {
        case EdMark:
            window_close(d, static_cast<uint16_t>(zone(e)->Dia_ZoNumber + 1000));
            break;
        case LiMark:
            window_close(d, static_cast<uint16_t>(zone(e)->Dia_ZoNumber + 2000));
            break;
        case TxMark:
            window_close(d, static_cast<uint16_t>(zone(e)->Dia_ZoNumber + 3000));
            break;
        }
    }
    put_blocks(entry_at(d->Dia_Buffer));
    dia_patch();
    d->Dia_Edited = 0;
    deactivate(d);
}

// Dia_CloseA0
void close_channel(Dialog *d)
{
    if (d->Dia_Programs && d->Dia_ProgLong)
        mem_free(d->Dia_Programs, d->Dia_ProgLong);
    lst_del(dialogs(), reinterpret_cast<uint8_t *>(d) - 8);
}

// Run fn with a target for the errors of the program outside a loop (the
// assembler went back to a stale stack pointer there)
template <typename F> void guarded(Run &r, F fn)
{
    local_jump::Buffer buf;
    local_jump::Buffer *outer = r.quit;
    r.quit = &buf;
    if (!AMOS_LOCAL_SETJMP(buf))
        fn();
    r.quit = outer;
}

// ---- checking a program when its channel opens -------------------------------------

struct Check {
    const uint8_t *pc;
    local_jump::Buffer *quit;
    uint16_t error;
};

[[noreturn]] void check_fail(Check &c, uint16_t error)
{
    c.error = error;
    local_jump::restore(c.quit);
}

// .Evalue: skip an expression. Returns the separator after it.
uint8_t check_expression(Check &c)
{
    for (;;) {
        Chr ch = next(c.pc);
        if (ch.k == CDigit) {
            const uint8_t *p = c.pc;
            number(p, ch.c);
            c.pc = p;
            continue;
        }
        if (ch.k == CEnd)
            check_fail(c, EDiaSyntax);
        if (ch.k == CSep)
            return ch.c;
        const uint8_t *list;
        uint8_t second;
        if (ch.k == COp) {
            list = function_table + 32;
            second = ch.c;
        } else {
            if (!function_table[ch.c - 64])
                check_fail(c, EDiaSyntax);
            list = function_list(ch.c);
            second = next(c.pc).c;
        }
        for (;; list += 2) {
            if (!list[0] && !list[1])
                check_fail(c, EDiaSyntax);
            if (list[1] == second)
                break;
        }
        if (second == '"' || second == '\'') {
            for (;;) {
                uint8_t b = *c.pc++;
                if (b < ' ')
                    check_fail(c, EDiaSyntax);
                if (b == second)
                    break;
            }
        }
    }
}

// The labels (LA n) and user instructions (UI xx,n;[...]): 6 bytes each,
// ended by -1
void check_program(Check &c, Dialog *d, uint8_t *&labels)
{
    const uint8_t *table = ptr<uint8_t>(d->Dia_Labels);
    for (;;) {
        Chr ch = next(c.pc);
        if (ch.k == CEnd)
            return;
        if (ch.c == '[' || ch.c == ']')
            continue;
        Chr c2 = next(c.pc);
        if (c2.k == CEnd)
            check_fail(c, EDiaSyntax);
        uint16_t name = static_cast<uint16_t>(ch.c << 8 | c2.c);
        int i = instruction_number(name);
        if (i < 0) {
            // A user instruction: its parameters
            Chr p = next(c.pc);
            if (p.k == CEnd)
                check_fail(c, EDiaSyntax);
            if (p.k == CSep)
                continue;
            c.pc--;
            while (check_expression(c) == ',') {
            }
            continue;
        }
        if (i == ILabel) {
            Chr n = next(c.pc);
            if (n.k != CDigit)
                check_fail(c, EDiaSyntax);
            uint32_t v = static_cast<uint32_t>(number(c.pc, n.c));
            if (v >= 65536)
                check_fail(c, EDiaSyntax);
            poke_l(labels, -1u);
            for (const uint8_t *l = table;; l += 6) {
                int32_t x = static_cast<int32_t>(peek_l(l));
                if (x < 0)
                    break;
                if (static_cast<uint32_t>(x) == v)
                    check_fail(c, EDiaLabAD);
            }
            Chr s = next(c.pc);
            if (s.k != CSep)
                check_fail(c, EDiaSyntax);
            poke_l(labels, v);
            poke_w(labels + 4, static_cast<uint16_t>(c.pc - programs(d)));
            labels += 6;
            poke_l(labels, -1u);
            continue;
        }
        if (i == IUser) {
            Chr a = next(c.pc);
            if (a.k != CLetter && a.k != COp)
                check_fail(c, EDiaSyntax);
            Chr b = next(c.pc);
            if (b.k == CEnd || b.k == CSep)
                check_fail(c, EDiaSyntax);
            uint16_t user = static_cast<uint16_t>(a.c << 8 | b.c);
            if (instruction_number(user) >= 0)
                check_fail(c, EDiaSyntax);
            for (const uint8_t *l = table;; l += 6) {
                int16_t x = static_cast<int16_t>(peek_w(l));
                if (x < 0)
                    break;
                if (static_cast<uint16_t>(x) == user)
                    check_fail(c, EDiaLabAD);
            }
            poke_w(labels, user);
            if (next(c.pc).c != ',')
                check_fail(c, EDiaSyntax);
            Chr n = next(c.pc);
            if (n.k != CDigit)
                check_fail(c, EDiaSyntax);
            uint32_t params = static_cast<uint32_t>(number(c.pc, n.c));
            if (params > 9)
                check_fail(c, EDiaSyntax);
            poke_w(labels + 2, static_cast<uint16_t>(params));
            if (next(c.pc).k > CSep)
                check_fail(c, EDiaSyntax);
            if (next(c.pc).c != '[')
                check_fail(c, EDiaSyntax);
            poke_w(labels + 4, static_cast<uint16_t>(c.pc - programs(d)));
            labels += 6;
            poke_l(labels, -1u);
            continue;
        }
        uint8_t n = instruction_params[i];
        if (!n) {
            Chr s = next(c.pc);
            if (s.k != CSep)
                check_fail(c, EDiaSyntax);
            continue;
        }
        while (n--) {
            uint8_t sep = check_expression(c);
            if (n && sep != ',')
                check_fail(c, EDiaNPar);
        }
    }
}

} // namespace

// ---- the interface of dialog.h ------------------------------------------------------

void amos::dia_wait_mul() { hal::wait_frame(); }

void amos::dia_patch()
{
    if (host::RoutineArg patch = idata().Patch_ScCopy.load()) {
        Regs r = {};
        amos_call(&r, patch);
    }
}

void amos::dia_no_mkey()
{
    do
        dia_wait_mul();
    while (mouse_key() & 1);
}

void amos::dia_clear_key()
{
    Regs r = {};
    sy_call(sy::ClearKey, r);
    wdata().ClLast = 0;
}

uint32_t amos::mouse_key()
{
    Regs r = {};
    sy_call(sy::MouseKey, r);
    return r.d[1];
}

Dialog *amos::dia_channel(uint32_t number)
{
    for (uint32_t e = *dialogs(); e; e = peek_l(ptr<void>(e)))
        if (peek_l(ptr<uint8_t>(e) + 8) == number)
            return ptr<Dialog>(e + 8);
    return nullptr;
}

DiaOpen amos::dia_open(uint32_t number, const uint8_t *program, const uint8_t *puzzle, const uint8_t *messages,
                       uint32_t buffer, uint32_t variables, bool copy)
{
    buffer &= 0xFFFE;
    uint32_t size = (variables + 1) * 4 + buffer + Dialog::Dia_Vars;
    if (dia_channel(number))
        return {EDiaChanAD, 0, nullptr};
    if (!wdata().EcCourant)
        return {EDiaScreen, 0, nullptr};
    uint8_t *e = lst_new(dialogs(), size);
    if (!e)
        return {EDiaOMem, 0, nullptr};
    Dialog *d = reinterpret_cast<Dialog *>(e + 8);
    d->Dia_Channel = number;
    d->Dia_NVar = variables;
    d->Dia_Puzzle = address(puzzle);
    d->Dia_Messages = address(messages);
    // Dia_ClearVar: closed when the program is cleared
    IData &id = idata();
    add_routine(lib_dia_on_clear, address(&id.Sys_ClearRoutines));
    add_routine(lib_dia_on_default, address(&id.Sys_DefaultRoutines));
    id.GoTest_Dialog.store(lib_entry(L_Dia_AutoTest));
    Screen *s = ptr<Screen>(wdata().EcCourant);
    d->Dia_Screen = address(s);
    d->Dia_ScreenNb = s->EcNumber;
    static const uint8_t slider_colours[16] = {0, 0, 0, 1, 4, 4, 4, 1, 0, 0, 0, 1, 3, 3, 3, 1};
    for (int i = 0; i < 16; i++)
        d->Dia_SlDefault[i] = slider_colours[i];
    uint16_t length = peek_w(program);
    // (The assembler gave a meaningless error position here)
    if (!length) {
        close_channel(d);
        return {EDiaSyntax, 0, nullptr};
    }
    d->Dia_Programs = address(program + 2);
    d->Dia_ProgLong = 0;
    if (copy) {
        uint8_t *p = mem_fast(length + 4u);
        if (!p) {
            close_channel(d);
            return {EDiaOMem, 0, nullptr};
        }
        d->Dia_ProgLong = length + 4u;
        d->Dia_Programs = address(p);
        for (uint16_t i = 0; i < length; i++)
            p[i] = program[2 + i];
        p[length] = 0;
    }
    uint8_t *labels = reinterpret_cast<uint8_t *>(dia_vars(d)) + variables * 4;
    d->Dia_Labels = d->Dia_Buffer = address(labels);
    d->Dia_Pile = address(labels + buffer);
    poke_l(labels, -1u);
    Check c = {programs(d), nullptr, 0};
    // Keep mutable validation state in this caller, outside setjmp's function.
    // Standard longjmp makes modified nonvolatile capture-frame locals indeterminate.
    auto validate = [](Check &state, Dialog *dialog, uint8_t *&table) {
        local_jump::Buffer buf;
        state.quit = &buf;
        if (AMOS_LOCAL_SETJMP(buf)) { state.quit = nullptr; return false; }
        check_program(state, dialog, table);
        state.quit = nullptr;
        return true;
    };
    if (!validate(c, d, labels)) {
        uint32_t position = static_cast<uint32_t>(c.pc - programs(d));
        close_channel(d);
        return {c.error, position, nullptr};
    }
    poke_l(labels, -1u);
    labels += 4;
    d->Dia_Buffer = d->Dia_PBuffer = address(labels);
    poke_l(labels, 0);
    return {0, 0, d};
}

uint16_t amos::dia_erase(uint32_t number)
{
    Dialog *d = dia_channel(number);
    if (!d)
        return EDiaChanND;
    erase(d);
    return 0;
}

uint16_t amos::dia_close(uint32_t number)
{
    dia_erase(number);
    Dialog *d = dia_channel(number);
    if (!d)
        return EDiaChanND;
    close_channel(d);
    return 0;
}

void amos::dia_close_all()
{
    while (uint32_t e = *dialogs())
        dia_close(peek_l(ptr<uint8_t>(e) + 8));
}

void amos::dia_freeze_all(bool freeze)
{
    for (uint32_t e = *dialogs(); e; e = peek_l(ptr<void>(e)))
        dia_freeze(peek_l(ptr<uint8_t>(e) + 8), freeze);
}

uint16_t amos::dia_freeze(uint32_t number, bool freeze)
{
    Dialog *d = dia_channel(number);
    if (!d)
        return EDiaChanND;
    d->Dia_RFlags = static_cast<uint8_t>(freeze ? d->Dia_RFlags | RFrozen : d->Dia_RFlags & ~RFrozen);
    return 0;
}

DiaResult amos::dia_run(uint32_t number, int32_t label, uint32_t x, uint32_t y)
{
    dia_erase(number);
    Dialog *d = dia_channel(number);
    if (!d)
        return {EDiaChanND, 0};
    if (x != EntNul)
        d->Dia_BaseX = x;
    if (y != EntNul)
        d->Dia_BaseY = y;
    Run r = {d, programs(d), ptr<be_u32>(d->Dia_Pile), nullptr};
    if (label >= 0) {
        uint16_t offset = label_offset(d, static_cast<uint32_t>(label));
        if (!offset)
            return {EDiaLabND, 0};
        r.pc += offset;
    }
    poke_l(entry_at(d->Dia_Buffer), 0);
    d->Dia_PBuffer = d->Dia_Buffer;
    d->Dia_LastKey = d->Dia_Edited = d->Dia_LastZone = 0;
    d->Dia_XA = d->Dia_YA = d->Dia_XB = d->Dia_YB = 0;
    d->Dia_Return = d->Dia_Error = d->Dia_Users = d->Dia_NPUsers = 0;
    d->Dia_PUsers = d->Dia_Release = 0;
    if (activate(d)) {
        // (The screen error itself was lost: Dia_Error is 0 here)
        erase(d);
        return {d->Dia_Error, static_cast<int16_t>(d->Dia_ErrorPos)};
    }
    set_writing(d, 0);
    d->Dia_RFlags = static_cast<uint8_t>((d->Dia_RFlags | RShown) & ~(RRun | RFrozen));
    dia_clear_key();
    loop(r);
    dia_clear_key();
    if (d->Dia_RFlags & RRun) {
        erase(d);
        if (!d->Dia_Error)
            return {0, static_cast<int16_t>(d->Dia_Return)};
        return {d->Dia_Error, static_cast<int16_t>(d->Dia_ErrorPos)};
    }
    edit_first(d);
    dia_patch();
    deactivate(d);
    // (The assembler returned a leftover register as the answer here)
    if (!d->Dia_Error)
        return {0, 0};
    erase(d);
    return {d->Dia_Error, static_cast<int16_t>(d->Dia_ErrorPos)};
}

DiaResult amos::dia_run_quick(const uint8_t *program, const uint8_t *puzzle, const uint8_t *messages, uint32_t x,
                              uint32_t y, uint32_t v0, uint32_t v1, uint32_t v2, bool copy, uint32_t buffer)
{
    uint32_t number = 65536;
    while (dia_channel(number))
        number++;
    DiaOpen o = dia_open(number, program, puzzle, messages, buffer, 16, copy);
    if (o.error)
        return {o.error, static_cast<int32_t>(o.position)};
    be_i32 *v = dia_vars(o.d);
    v[0] = static_cast<int32_t>(v0);
    v[1] = static_cast<int32_t>(v1);
    v[2] = static_cast<int32_t>(v2);
    DiaResult result = dia_run(number, -1, x, y);
    dia_close(number);
    return result;
}

DiaResult amos::dia_update(uint32_t number, uint32_t z, uint32_t a, uint32_t b, uint32_t c)
{
    Dialog *d = dia_channel(number);
    if (!d)
        return {EDiaChanND, 0};
    if (!(d->Dia_RFlags & RShown))
        return {0, 0};
    if (activate(d))
        return {EDiaChanND, 0};
    Run r = {d, programs(d), ptr<be_u32>(d->Dia_Pile), nullptr};
    guarded(r, [&] { zone_update(r, z, a, b, c); });
    deactivate(d);
    if (!d->Dia_Error)
        return {0, 0};
    return {d->Dia_Error, static_cast<int16_t>(d->Dia_ErrorPos)};
}

DiaResult amos::dia_return(uint32_t number)
{
    Dialog *d = dia_channel(number);
    if (!d)
        return {EDiaChanND, 0};
    if (!(d->Dia_RFlags & RShown))
        return {0, -1};
    int32_t v = d->Dia_Return;
    d->Dia_Return = 0;
    return {0, v};
}

uint8_t *amos::dia_zone_address(uint32_t number, uint32_t z, uint32_t which)
{
    Dialog *d = dia_channel(number);
    if (!d)
        return nullptr;
    int16_t skip = static_cast<int16_t>(which - 1);
    for (uint8_t *e = entry_at(d->Dia_Buffer); entry_length(e); e += entry_length(e))
        if (is_zone(e) && zone(e)->Dia_ZoNumber == static_cast<uint16_t>(z) && skip-- <= 0)
            return e;
    return nullptr;
}

DiaValue amos::dia_value(uint32_t channel, uint32_t z, uint32_t which)
{
    uint8_t *e = dia_zone_address(channel, z, which);
    if (!e)
        return {EDiaChanND, 0, 0};
    switch (entry_id(e)) {
    case BtMark:
    case LiMark:
    case SlMark:
        return {0, zone(e)->Dia_ZoPos, 0};
    case TxMark:
        if (zone(e)->Dia_ZoPos)
            return {0, zone(e)->Dia_ZoPos, 0};
        return {0, address(text(e)->Dia_TxBuffer), 2};
    case EdMark: {
        LineEdit *l = line_edit(e);
        if (entry_length(e) != DiaEdit::Dia_DiLong)
            return {0, l->LEd_Buffer, 2};
        const uint8_t *pc = ptr<uint8_t>(l->LEd_Buffer);
        uint8_t first = *pc++;
        if (first >= ' ')
            edit(e)->Dia_DiValue = static_cast<uint32_t>(number(pc, first));
        return {0, edit(e)->Dia_DiValue, 0};
    }
    }
    return {0, 0, 0};
}

be_i32 *amos::dia_variable(uint32_t number, uint32_t n, uint16_t *error)
{
    Dialog *d = dia_channel(number);
    if (!d) {
        *error = EDiaChanND;
        return nullptr;
    }
    if (n > d->Dia_NVar) {
        *error = EDiaVarND;
        return nullptr;
    }
    *error = 0;
    return reinterpret_cast<be_i32 *>(reinterpret_cast<uint8_t *>(dia_vars(d)) + static_cast<int16_t>(n << 2));
}

DiaResult amos::dia_zone_at(uint32_t number, uint32_t x, uint32_t y)
{
    Dialog *d = dia_channel(number);
    if (!d)
        return {EDiaChanND, 0};
    int16_t zx = static_cast<int16_t>(x), zy = static_cast<int16_t>(y);
    uint8_t *e = zone_at(d, zx, zy);
    return {0, e ? static_cast<int16_t>(zone(e)->Dia_ZoNumber) : -1};
}

uint16_t amos::dia_auto_test(uint32_t channels)
{
    uint32_t e = *dialogs();
    for (uint16_t n = static_cast<uint16_t>(channels - 1); e; n--) {
        Dialog *d = ptr<Dialog>(e + 8);
        if ((d->Dia_RFlags & RShown) && !(d->Dia_RFlags & RFrozen)) {
            uint16_t error = activate(d);
            if (!error) {
                if (static_cast<int32_t>(d->Dia_Edited) > 0)
                    edit_active(d, entry_at(d->Dia_Edited));
                Run r = {d, programs(d), ptr<be_u32>(d->Dia_Pile), nullptr};
                uint16_t exits = 0;
                guarded(r, [&] { exits = tests(r); });
                if (exits)
                    erase(d);
                deactivate(d);
                error = d->Dia_Error;
            }
            if (error) {
                // Frozen, so that the error does not come again
                d->Dia_RFlags |= RFrozen;
                return error;
            }
        }
        e = peek_l(ptr<void>(e));
        if (!e || n == 0)
            break;
    }
    return 0;
}

uint8_t *amos::dia_resource_screen(const uint8_t *puzzle, uint32_t number, uint32_t width, uint32_t height,
                                   uint32_t flash, int32_t interlace, uint32_t *error)
{
    const uint8_t *p = puzzle + 2 + (peek_w(puzzle) << 2);
    int32_t colours = static_cast<int16_t>(peek_w(p));
    uint32_t mode = peek_w(p + 2) & 0x8004;
    if (static_cast<int16_t>(interlace) >= 0) {
        mode &= ~4u;
        if (static_cast<int16_t>(interlace))
            mode |= 4;
    }
    uint32_t planes;
    if (colours == 4096) {
        planes = 6;
        mode |= 0x0800;
        colours = 64;
    } else {
        planes = 1;
        for (int32_t c = 2; c != colours && planes < 7; c <<= 1)
            planes++;
    }
    Regs c = {};
    c.d[1] = number;
    c.d[2] = width;
    c.d[3] = height;
    c.d[4] = planes;
    c.d[5] = mode;
    c.d[6] = static_cast<uint32_t>(colours);
    c.a[1] = address(p + 4);
    ec_call(ec::Cree, c);
    *error = c.d[0];
    if (c.d[0])
        return nullptr;
    Screen *s = ptr<Screen>(c.a[0]);
    if (!flash) {
        print("\x1b" "C0");
    } else {
        if (flash >= s->EcNbCol) {
            *error = 1;
            return nullptr;
        }
        Regs m = {};
        m.d[0] = 46;
        m.a[0] = address(boot::sys_message(m.d[0]));
        Regs f = {};
        f.d[1] = flash;
        f.a[1] = m.a[0];
        ec_call(ec::Flash, f);
        if (f.d[0]) {
            *error = f.d[0];
            return nullptr;
        }
        char cu[] = "\x1b" "D0";
        cu[2] = static_cast<char>('0' + flash);
        print(cu);
    }
    *error = 0;
    return reinterpret_cast<uint8_t *>(s);
}

// ---- register entries (the editor, the monitor, the file selector, Input
// and Read Text call these) ---------------------------------------------------------

// Dia_OpenChannel: a0 = program, a1 = graphics, a2 = messages, d0 =
// channel, d1 = buffer, d2 = variables, d3 bit 0 = copy the program. Out:
// d0 = error, d1 = its position, a0 = the channel, a1 = its variables.
extern "C" uint32_t lib_dia_open_channel(RegFrame &r)
{
    DiaOpen o = dia_open(r.d[0], ptr<uint8_t>(r.a[0]), ptr<uint8_t>(r.a[1]), ptr<uint8_t>(r.a[2]), r.d[1], r.d[2],
                         r.d[3] & 1);
    r.d[1] = o.position;
    if (!o.error) {
        r.a[0] = address(o.d);
        r.a[1] = address(dia_vars(o.d));
    }
    return r.d[0] = o.error;
}

extern "C" uint32_t lib_dia_close_channel(RegFrame &r) { return r.d[0] = dia_close(r.d[0]); }
extern "C" uint32_t lib_dia_close_channels(RegFrame &r)
{
    dia_close_all();
    return r.d[0];
}
extern "C" uint32_t lib_dia_eff_channel(RegFrame &r) { return r.d[0] = dia_erase(r.d[0]); }
extern "C" uint32_t lib_dia_freeze_channels(RegFrame &r)
{
    dia_freeze_all(static_cast<uint16_t>(r.d[1]));
    return r.d[0];
}
extern "C" uint32_t lib_dia_freeze_channel(RegFrame &r)
{
    return r.d[0] = dia_freeze(r.d[0], static_cast<uint16_t>(r.d[1]));
}

// Dia_RunQuick: a0 = program, a1 = graphics, a2 = messages, d0/d1 = x/y,
// d2-d4 = variables 0-2, d5 bit 0 = copy, d6 = buffer. Out: d0 = error
// (TST.W), d1 = answer or position.
extern "C" uint32_t lib_dia_run_quick(RegFrame &r)
{
    DiaResult x = dia_run_quick(ptr<uint8_t>(r.a[0]), ptr<uint8_t>(r.a[1]), ptr<uint8_t>(r.a[2]), r.d[0], r.d[1],
                                r.d[2], r.d[3], r.d[4], r.d[5] & 1, r.d[6]);
    r.d[0] = x.error;
    r.d[1] = static_cast<uint32_t>(x.value);
    return x.error;
}

// Dia_Update: d0 = channel, d1 = zone, d2-d4 = values. Out: d0 = error
// (d1 = position).
extern "C" uint32_t lib_dia_update(RegFrame &r)
{
    DiaResult x = dia_update(r.d[0], r.d[1], r.d[2], r.d[3], r.d[4]);
    if (x.error)
        r.d[1] = static_cast<uint32_t>(x.value);
    return r.d[0] = x.error;
}

// Dia_RunProgram: d0 = channel, d1 = label (-1: start), d2/d3 = x/y.
// Out: d0 = error, d1 = answer or position.
extern "C" uint32_t lib_dia_run_program(RegFrame &r)
{
    DiaResult x = dia_run(r.d[0], static_cast<int32_t>(r.d[1]), r.d[2], r.d[3]);
    r.d[1] = static_cast<uint32_t>(x.value);
    return r.d[0] = x.error;
}

// Dia_GetVariable: d0 = channel, d1 = number. Out: d0 = error, a0 = it.
extern "C" uint32_t lib_dia_get_variable(RegFrame &r)
{
    uint16_t error;
    be_i32 *v = dia_variable(r.d[0], r.d[1], &error);
    if (v)
        r.a[0] = address(v);
    return r.d[0] = error;
}

// Dia_GetZone: d0 = channel, d1/d2 = x/y. Out: d0 = error, d1 = zone.
extern "C" uint32_t lib_dia_get_zone(RegFrame &r)
{
    DiaResult x = dia_zone_at(r.d[0], r.d[1], r.d[2]);
    if (!x.error)
        r.d[1] = static_cast<uint32_t>(x.value);
    return r.d[0] = x.error;
}

// Dia_GetValue: d0 = channel, d1 = zone, d2 = which one of that number.
// Out: d0 = error, d1 = value, d2 = 0 a number / 2 a string, a0 = zone.
extern "C" uint32_t lib_dia_get_value(RegFrame &r)
{
    uint8_t *e = dia_zone_address(r.d[0], r.d[1], r.d[2]);
    DiaValue v = dia_value(r.d[0], r.d[1], r.d[2]);
    if (!v.error) {
        r.a[0] = address(e);
        r.d[1] = v.value;
        r.d[2] = v.type;
    }
    return r.d[0] = v.error;
}

// Dia_GetZoneAd: d0 = channel, d1 = zone, d2 = which. Out: a0 = d0 = the
// zone, or 0.
extern "C" uint32_t lib_dia_get_zone_ad(RegFrame &r)
{
    uint8_t *e = dia_zone_address(r.d[0], r.d[1], r.d[2]);
    if (e)
        r.a[0] = address(e);
    return r.d[0] = address(e);
}

// Dia_SetVFlags: a0 = variables, d0 = bits, d1 = count: one bit in each
extern "C" uint32_t lib_dia_set_vflags(RegFrame &r)
{
    be_u32 *v = ptr<be_u32>(r.a[0]);
    uint16_t bits = static_cast<uint16_t>(r.d[0]);
    for (uint16_t n = static_cast<uint16_t>(r.d[1]); n; n--, bits >>= 1)
        *v++ = bits & 1;
    r.d[0] = (r.d[0] & 0xFFFF0000) | bits;
    r.d[1] = (r.d[1] & 0xFFFF0000) | 0xFFFF;
    r.a[0] = address(v);
    return r.d[0];
}

// Dia_GetVFlags: a0 = variables, d1 = count. Out: d0 = the bits.
extern "C" uint32_t lib_dia_get_vflags(RegFrame &r)
{
    const be_u32 *v = ptr<be_u32>(r.a[0]);
    uint16_t n = static_cast<uint16_t>(r.d[1]);
    uint16_t bits = 0;
    for (uint16_t i = n; i; i--)
        bits = static_cast<uint16_t>(bits << 1 | (v[i - 1] ? 1 : 0));
    r.d[1] = (r.d[1] & 0xFFFF0000) | 0xFFFF;
    return r.d[0] = bits;
}

// Dia_GetReturn: d0 = channel. Out: d0 = error, d1 = answer (-1 erased).
extern "C" uint32_t lib_dia_get_return(RegFrame &r)
{
    DiaResult x = dia_return(r.d[0]);
    if (!x.error)
        r.d[1] = static_cast<uint32_t>(x.value);
    return r.d[0] = x.error;
}

// Dia_RScOpen: a0 = graphics, d0 = screen, d1/d2 = size, d3 = flash
// colour, d4 = interlace (-1 as the resource). Out: a0 = screen, d0 =
// error (TST.W).
extern "C" uint32_t lib_dia_rsc_open(RegFrame &r)
{
    uint32_t error;
    uint8_t *s = dia_resource_screen(ptr<uint8_t>(r.a[0]), r.d[0], r.d[1], r.d[2], r.d[3],
                                     static_cast<int32_t>(r.d[4]), &error);
    if (s)
        r.a[0] = address(s);
    r.d[0] = error;
    return static_cast<uint16_t>(error);
}

// Dia_GetChannel: d0 = channel. Out: a0 = a1 = it, or 0 (BEQ).
extern "C" uint32_t lib_dia_get_channel(RegFrame &r)
{
    r.a[0] = r.a[1] = address(dia_channel(r.d[0]));
    return r.a[0];
}

// Dia_AutoTest(2): the zones of the channels (d0 of them). Out: d0 = error.
extern "C" uint32_t lib_dia_auto_test(RegFrame &r) { return r.d[0] = dia_auto_test(127); }
extern "C" uint32_t lib_dia_auto_test2(RegFrame &r) { return r.d[0] = dia_auto_test(r.d[0]); }

// The buttons and sliders of the editor (Bt_*, Sl_*): a0 = the structure
extern "C" uint32_t lib_bt_init_list(RegFrame &r)
{
    for (Button *b = ptr<Button>(r.a[0]);; b++) {
        button_init(b, static_cast<uint16_t>(r.d[0]));
        if (!b[1].Bt_Number)
            break;
    }
    return r.d[0];
}
extern "C" uint32_t lib_bt_init(RegFrame &r)
{
    button_init(ptr<Button>(r.a[0]), static_cast<uint16_t>(r.d[0]));
    return r.d[0];
}
extern "C" uint32_t lib_bt_end(RegFrame &r)
{
    Regs z = {};
    z.d[1] = ptr<Button>(r.a[0])->Bt_Zone;
    sy_call(sy::RazZone, z);
    return r.d[0];
}
// Bt_Gere: Out: d0 = the button clicked, or 0; d1 = the mouse buttons
extern "C" uint32_t lib_bt_gere(RegFrame &r)
{
    uint16_t keys;
    Button *b = buttons_click(ptr<Button>(r.a[0]), &keys);
    r.d[1] = (r.d[1] & 0xFFFF0000) | keys;
    return r.d[0] = address(b);
}
extern "C" uint32_t lib_sl_init(RegFrame &r)
{
    slider_init(ptr<Slider>(r.a[0]));
    return r.d[0] = 0;
}
extern "C" uint32_t lib_sl_clic(RegFrame &r)
{
    slider_click(ptr<Slider>(r.a[0]), nullptr);
    return r.d[0];
}
extern "C" uint32_t lib_sl_draw(RegFrame &r)
{
    slider_draw(ptr<Slider>(r.a[0]), static_cast<uint16_t>(r.d[0]));
    return r.d[0] = 0;
}

extern "C" uint32_t lib_dia_no_mkey(RegFrame &r)
{
    dia_no_mkey();
    return r.d[0];
}
extern "C" uint32_t lib_dia_clear_key(RegFrame &r)
{
    dia_clear_key();
    return r.d[0];
}
extern "C" uint32_t lib_dia_wait_mul(RegFrame &r)
{
    dia_wait_mul();
    return r.d[0];
}

// The routine records of Dia_ClearVar (dialog_glue.s)
extern "C" void lib_dia_close_all() { dia_close_all(); }

#if !defined(__mc68000__)
#include "runtime/bindings/lib_dialog.inc"
#endif
