// AMAL (+W.s): the tokeniser of AMAL programs, the channels and their
// management (SyCall AMALCre, AMALMvO, AMALReg...), and the animator,
// which runs every channel once a VBL (or at each Synchro).
//
// A program is tokenised to words as the original did: an instruction is
// the offset of its routine in the original's table (AmJumps: 0 End, $04
// STOS Anim... $D8 Xor), followed by its data; the labels, jumps and
// loops are offsets in it. The animator reads them in the original's
// order and with its arithmetic. It runs in the VBL interrupt: it uses the
// data zone it is given, never the task's context.

#include "w/w.h"
#include "hal/chips.h"

using namespace amos;
using namespace amos::w;

extern "C" {
// w/collision.cpp
void w_sy_bob_collisions(AllRegs *r);
void w_sy_sprite_collisions(AllRegs *r);
int32_t w_collision_get(uint8_t *data, int32_t n);
}

namespace {

constexpr int NbInterne = 10;
constexpr uint16_t BitEcrans = 12, BitBobs = 13, BitSprites = 14;
constexpr int16_t EcYBase = 0x1000;
constexpr int RainLong = 24, RnActAt = 16;

// A channel (the original's AmXxx offsets), followed by its program
struct Channel {
    be_u32 AmPrev, AmNext;
    be_u16 AmLong, AmNb;          // size of the block; channel * 4 + type
    be_u32 AmPos, AmAuto, AmAct;  // program, autotest, the object's x/y/image
    be_u16 AmBit, AmCpt;          // actualisation bit ($8000: frozen)
    be_u32 AmDeltX, AmDeltY;
    be_u16 AmVirgX, AmVirgY;
    be_u32 AmFin, AmAJsr, AmAAd, AmAALoop;
    be_u16 AmACLoop, AmACpt;
    be_u16 AmIRegs[NbInterne];    // R9...R0
};
static_assert(sizeof(Channel) == 80);
constexpr uint32_t AmStart = sizeof(Channel);
constexpr uint32_t R0 = AmStart - 2, R1 = AmStart - 4;     // offsets of R0, R1

// The animations run after the program (AmAJsr)
enum Anim : uint32_t { NoAnim, DoAni, StAni, StMvX, StMvY };

// The instructions, by their offsets in the original's AmJumps
enum Op : uint16_t {
    AmStop = 0x00, AmAnim = 0x04, AmMvtX = 0x08, AmMvtY = 0x0C, AmWait = 0x10, AmPose = 0x14, AmMove = 0x18,
    AmJump = 0x1C, AmLet = 0x20, AmIf = 0x24, AmFor = 0x28, AmNxt = 0x2C, AmAEg = 0x30, AmXEg = 0x34, AmYEg = 0x38,
    AmREg = 0x3C, AmEgA = 0x40, AmEgX = 0x44, AmEgY = 0x48, AmEgR = 0x4C, AmOn = 0x50, AmXS = 0x54, AmYS = 0x58,
    AmCol = 0x5C, AmAOn = 0x60, AmAOff = 0x64, AmAExit = 0x68, AmDirect = 0x6C, AmChif = 0x70, AmXMou = 0x74,
    AmYMou = 0x78, AmSExp = 0x7C, AmJ0 = 0x80, AmJ1 = 0x84, AmM1 = 0x88, AmM2 = 0x8C, AmEg = 0x90, AmDif = 0x94,
    AmInf = 0x98, AmSup = 0x9C, AmPlus = 0xA0, AmMoins = 0xA4, AmDiv = 0xA8, AmMult = 0xAC, AmOr = 0xB0, AmAnd = 0xB4,
    AmSCol = 0xB8, AmBCol = 0xBC, AmPlay = 0xC0, AmXH = 0xC4, AmYH = 0xC8, AmAni = 0xCC, AmHaz = 0xD0, AmVu = 0xD4,
    AmXor = 0xD8,
};

uint16_t pw(uint32_t at) { return peek_w(ptr<void>(at)); }
int16_t sw16(uint32_t at) { return int16_t(peek_w(ptr<void>(at))); }
void sw(uint32_t at, uint16_t v) { poke_w(ptr<void>(at), v); }
Channel &channel(uint32_t at) { return *ptr<Channel>(at); }
// The high word of a long field (the original wrote words there)
be_u16 &high(be_u32 &l) { return reinterpret_cast<be_u16 *>(&l)[0]; }

// ---- the tokeniser ----------------------------------------------------------

struct Tokeniser {
    const uint8_t *s;           // a0
    uint8_t *labels;            // a1: 26 labels, the jumps (+52), the loops (+156)
    uint32_t buffer, out, end;  // d7, a2, a5
    uint32_t overflow = 0;      // a6
    uint32_t autotest = 0;      // d5: the offset of AU('s word << 16 | 1
    int jumps = 0;              // d6
    uint8_t *loops;             // a3
    uint32_t bank;
    int error = 0;

    bool fail(int e)
    {
        error = e;
        return false;
    }
    void emit(uint16_t w)
    {
        sw(out, w);
        out += 2;
    }
    uint16_t place() const { return uint16_t(out - buffer); }

    // AniChr: the next character, lower case letters and others skipped
    uint8_t chr()
    {
        for (;;) {
            uint8_t c = *s++;
            if (!c || (c >= 33 && c <= 'Z') || c == '|' || c == '!')
                return c;
            if (c == 27)
                s += 2;
        }
    }

    // StChr: for the STOS forms, spaces skipped, in upper case
    uint8_t st_chr()
    {
        uint8_t c;
        do
            c = *s++;
        while (c == ' ');
        if (c >= 'a' && c <= 'z')
            c = uint8_t(c - 32);
        return c;
    }

    // Tohh
    static int hex(uint8_t c)
    {
        c = uint8_t(c - '0');
        if (c < 10)
            return c;
        c = uint8_t(c - ('A' - '0'));
        if (c < 6)
            return c + 10;
        return -1;
    }

    // AniLong: a decimal or hexadecimal number, maybe negative
    bool number(uint32_t &v)
    {
        int sign = 1;
        uint8_t c = chr();
        if (c == '-') {
            sign = 0;
            c = chr();
        }
        uint32_t n;
        if (c == '$') {
            int h = hex(chr());
            if (h < 0)
                return fail(1);
            n = uint32_t(h);
            sign--;
            while ((h = hex(chr())) >= 0)
                n = (n << 4) + uint32_t(h);
        } else {
            uint8_t d = uint8_t(c - '0');
            if (d >= 10)
                return fail(1);
            n = d;
            sign--;
            while ((d = uint8_t(chr() - '0')) < 10)
                n = n * 10 + d;
        }
        s--;
        if (sign > 0)
            return fail(1);
        v = sign ? 0u - n : n;
        return true;
    }

    // AniR, c read: A, X, Y or a register. kind: 0 A, 2 X, 4 Y, 6 R (and
    // its offset); size: 2, 4 with the offset, 0 if none (c given back)
    struct Reg {
        uint16_t kind, offset, size;
    };
    Reg reg(uint8_t c)
    {
        if (c == 'A')
            return {0, 0, 2};
        if (c == 'X')
            return {2, 0, 2};
        if (c == 'Y')
            return {4, 0, 2};
        if (c == 'R') {
            uint8_t d = chr();
            if (d >= '0') {
                d = uint8_t(d - '0');
                if (d < NbInterne)
                    return {6, uint16_t(-(d + 1) * 2), 4};
                if (d >= 'A' - '0') {
                    d = uint8_t(d - ('A' - '0'));
                    if (d <= 26)
                        return {6, uint16_t(d * 2), 4};
                }
            }
        }
        s--;
        return {0, 0, 0};
    }
    Reg reg() { return reg(chr()); }

    void emit_reg(uint16_t op, const Reg &r)
    {
        emit(op);
        if (r.size == 4)
            emit(r.offset);
    }

    // A label's number (times 2) from c
    bool label(uint8_t c, uint16_t &n)
    {
        if (c < 'A' || uint8_t(c - 'A') > 26)
            return fail(1);
        n = uint16_t((c - 'A') << 1);
        return true;
    }

    // AnJmp, AnDirect: a jump to a label, in the list of the jumps
    bool jump(uint16_t op, bool flagged)
    {
        emit(op);
        uint16_t n;
        if (!label(chr(), n))
            return false;
        uint16_t pos = place();
        if (flagged)
            pos |= uint16_t(autotest);
        poke_w(labels + 52 + jumps, pos);
        poke_w(labels + 52 + jumps + 2, n);
        jumps += 4;
        poke_w(labels + 52 + jumps, 0xFFFF);
        emit(0);
        return true;
    }

    bool expect(uint8_t c)
    {
        if (chr() != c)
            return fail(1);
        return true;
    }

    // An operand's arguments: AniRe gives back the character and ends
    bool open()
    {
        if (chr() == '(')
            return true;
        s--;
        return false;
    }
    bool closing(uint8_t c)
    {
        if (chr() == c)
            return true;
        s--;
        return false;
    }

    bool expression();
    bool operand();
    bool tokenise(uint16_t type);
    bool stos(uint16_t type);
    bool pass2();
};

// AniOpe: an operand
bool Tokeniser::operand()
{
    uint8_t c = chr();
    switch (c) {
    case 'K':
        c = chr();
        if (c == '1')
            emit(AmM1);
        else if (c == '2')
            emit(AmM2);
        else
            return fail(1);
        return true;
    case 'J':
        c = chr();
        if (c == '0')
            emit(AmJ0);
        else if (c == '1')
            emit(AmJ1);
        else
            return fail(1);
        return true;
    case 'O':
        emit(AmOn);
        return true;
    case 'S':
    case 'B':
        // SC(n,x,y), BC(n,x,y)
        emit(c == 'S' ? AmSCol : AmBCol);
        if (!closing('C') || !open())
            return true;
        if (!expression())
            return false;
        if (!closing(','))
            return true;
        if (!expression())
            return false;
        if (!closing(','))
            return true;
        if (!expression())
            return false;
        closing(')');
        return true;
    case 'C':
    case 'Z':
    case 'V':
        // C(n), Z(n), V(n)
        emit(c == 'C' ? AmCol : c == 'Z' ? AmHaz : AmVu);
        if (!open())
            return true;
        if (!expression())
            return false;
        closing(')');
        return true;
    default:
        break;
    }
    if (c == '-' || c == '$' || (c >= '0' && c <= '9')) {
        s--;
        uint32_t v;
        if (!number(v))
            return false;
        emit(AmChif);
        emit(uint16_t(v));
        return true;
    }
    Reg r = reg(c);
    if (!r.size)
        return fail(1);
    if (r.kind == 2 || r.kind == 4) {
        // XS, YS, XH, YH (screen,coordinate), XM, YM
        uint8_t next = chr();
        s--;
        if (next == 'M') {
            s++;
            emit(r.kind == 2 ? AmXMou : AmYMou);
            return true;
        }
        if (next == 'S' || next == 'H') {
            s++;
            emit(next == 'S' ? (r.kind == 2 ? AmXS : AmYS) : (r.kind == 2 ? AmXH : AmYH));
            if (!open())
                return true;
            if (!expression())
                return false;
            if (!closing(','))
                return true;
            if (!expression())
                return false;
            closing(')');
            return true;
        }
    }
    emit_reg(uint16_t(AmEgA + r.kind * 2), r);
    return true;
}

// AniExp: operands and operators, then AmSExp
bool Tokeniser::expression()
{
    if (!operand())
        return false;
    for (;;) {
        uint16_t op;
        switch (chr()) {
        case '=':
            op = AmEg;
            break;
        case '<':
            op = AmInf;
            if (*s == '>') {
                s++;
                op = AmDif;
            }
            break;
        case '>':
            op = AmSup;
            break;
        case '+':
            op = AmPlus;
            break;
        case '-':
            op = AmMoins;
            break;
        case '/':
            op = AmDiv;
            break;
        case '*':
            op = AmMult;
            break;
        case '|':
            op = AmOr;
            break;
        case '&':
            op = AmAnd;
            break;
        case '!':
            op = AmXor;
            break;
        default:
            emit(AmSExp);
            s--;
            return true;
        }
        if (!operand())
            return false;
        emit(op);
    }
}

// The instructions (AniLoop)
bool Tokeniser::tokenise(uint16_t type)
{
    if (type)
        return stos(type);
    for (;;) {
        if (out >= end) {
            // too long: on from the start, the size counted
            overflow += out - buffer;
            out = buffer;
        }
        uint8_t c = chr();
        if (!c)
            return pass2();
        uint8_t d = chr();
        if (d == ':') {
            uint16_t n;
            if (!label(c, n))
                return false;
            if (int16_t(peek_w(labels + n)) >= 0)
                return fail(8);
            poke_w(labels + n, uint16_t(place() | uint16_t(autotest)));
            continue;
        }
        s--;
        switch (c) {
        case 'J':
            if (!jump(AmJump, true))
                return false;
            break;
        case 'L': {
            emit(AmLet);
            Reg r = reg();
            if (!r.size)
                return fail(1);
            if (!expect('=') || !expression())
                return false;
            emit_reg(uint16_t(AmAEg + r.kind * 2), r);
            break;
        }
        case 'M':
            if (autotest)
                return fail(9);
            emit(AmMove);
            if (!expression() || !expect(',') || !expression() || !expect(',') || !expression())
                return false;
            break;
        case 'F': {
            if (autotest)
                return fail(9);
            emit(AmFor);
            Reg r = reg();
            if (!r.size || r.kind != 6)
                return fail(1);
            if (!expect('=') || !expression() || !expect('T') || !expression())
                return false;
            poke_l(loops, out);
            poke_w(loops + 4, r.offset);
            loops += 6;
            emit(r.offset);
            emit(0);        // the end value, at run time
            break;
        }
        case 'N': {
            if (autotest)
                return fail(9);
            emit(AmNxt);
            Reg r = reg();
            if (!r.size || r.kind != 6)
                return fail(1);
            loops -= 2;
            if (peek_w(loops) != r.offset)
                return fail(2);
            loops -= 4;
            emit(uint16_t(peek_l(loops) - out));
            break;
        }
        case 'I':
            emit(AmIf);
            if (!expression())
                return false;
            switch (chr()) {
            case 'J':
                if (!jump(AmJump, true))
                    return false;
                break;
            case 'D':
                if (!autotest)
                    return fail(6);
                if (!jump(AmDirect, false))
                    return false;
                break;
            case 'X':
                if (!autotest)
                    return fail(6);
                emit(AmAExit);
                emit(0);
                break;
            default:
                return fail(1);
            }
            break;
        case 'W':
            if (autotest)
                return fail(9);
            emit(AmWait);
            break;
        case 'P':
            if (*s == 'L') {
                // PLay
                s++;
                if (!bank)
                    return fail(10);
                emit(AmPlay);
                if (!expression())
                    return false;
            } else {
                emit(AmPose);
            }
            break;
        case 'E':
            emit(AmStop);
            break;
        case 'A':
            if (*s == 'U') {
                // AU(: the autotest
                s++;
                uint8_t e = chr();
                if (autotest)
                    return fail(5);
                if (e != '(')
                    return fail(1);
                if (chr() == ')') {
                    emit(AmAOff);
                    break;
                }
                s--;
                emit(AmAOn);
                autotest = uint32_t(place()) << 16 | 1;
                emit(0);
                break;
            }
            {
                // Anim n,(image,delay)...
                emit(AmAni);
                if (!expression())
                    return false;
                uint32_t list = out;
                emit(0);
                if (!expect(',') || !expect('('))
                    return false;
                do {
                    if (!expression() || !expect(',') || !expression() || !expect(')'))
                        return false;
                } while (chr() == '(');
                s--;
                emit(0);
                sw(list, uint16_t(out - list));
            }
            break;
        case 'D':
            if (!autotest)
                return fail(6);
            if (!jump(AmDirect, false))
                return false;
            break;
        case 'X':
            if (!autotest)
                return fail(6);
            emit(AmAExit);
            emit(0);
            break;
        case ')': {
            if (!autotest)
                return fail(6);
            emit(AmAExit);
            uint16_t at = uint16_t(autotest >> 16);
            sw(buffer + at, uint16_t(place() - at));
            autotest = 0;
            break;
        }
        default:
            break;
        }
    }
}

// AniStos: Anim n,"(i,d)...", Move X/Y n,"[start](delay,step,count)...[L|E[end]]"
bool Tokeniser::stos(uint16_t type)
{
    if (type == 1) {
        emit(AmAnim);
        if (st_chr() != '(')
            return fail(1);
        for (;;) {
            uint32_t v;
            if (!number(v))
                return false;
            emit(uint16_t(v));
            if (st_chr() != ',')
                return fail(1);
            if (!number(v))
                return false;
            emit(uint16_t(v));
            if (int16_t(v) < 0)
                return fail(1);
            if (st_chr() != ')')
                return fail(1);
            uint8_t c = st_chr();
            if (!c) {
                emit(0xFFFF);
                return pass2();
            }
            if (c == 'L') {
                emit(0xFFFE);
                return pass2();
            }
            if (c != '(')
                return fail(1);
        }
    }
    emit(type == 2 ? AmMvtX : AmMvtY);
    uint32_t head = out;
    emit(0x8000);       // the start
    emit(0);            // loop
    emit(0x8000);       // the end
    uint8_t c = st_chr();
    if (!c)
        return fail(1);
    uint32_t v;
    if (c != '(') {
        s--;
        if (!number(v))
            return false;
        sw(head, uint16_t(v));
        if (st_chr() != '(')
            return fail(1);
    }
    for (;;) {
        if (!number(v))
            return false;
        emit(uint16_t(v));
        if (int16_t(v) <= 0)
            return fail(1);
        if (st_chr() != ',' || !number(v))
            return error ? false : fail(1);
        emit(uint16_t(v));
        if (st_chr() != ',' || !number(v))
            return error ? false : fail(1);
        emit(uint16_t(v));
        if (int16_t(v) < 0)
            return fail(1);
        if (st_chr() != ')')
            return fail(1);
        c = st_chr();
        if (c != '(')
            break;
    }
    emit(0);
    if (!c)
        return pass2();
    if (c == 'L')
        sw(head + 2, 0xFFFF);
    else if (c != 'E')
        return fail(1);
    if (!st_chr())
        return pass2();
    s--;
    if (!number(v))
        return false;
    sw(head + 4, uint16_t(v));
    return pass2();
}

// AnPasse2: the end, the labels into the jumps
bool Tokeniser::pass2()
{
    emit(AmStop);
    for (int j = 0;; j += 4) {
        int16_t pos = int16_t(peek_w(labels + 52 + j));
        if (pos < 0)
            return true;
        uint16_t flag = pos & 1, at = uint16_t(pos & 0xFFFE);
        int16_t target = int16_t(peek_w(labels + peek_w(labels + 52 + j + 2)));
        if (target < 0) {
            s = ptr<uint8_t>(buffer + at);
            return fail(3);
        }
        if ((target & 1) != flag) {
            s = ptr<uint8_t>(buffer + at);
            return fail(4);
        }
        sw(buffer + at, uint16_t((target & 0xFFFE) - at));
    }
}

// TokAMAL: type 0 AMAL, 1 Anim, 2 Move X, 3 Move Y; the program at text,
// tokenised in the buffer, vars its work space. 0 (and its length), an
// error (and where), or -1 (and the length it needs).
int32_t tokenise(WData &d, uint16_t type, const uint8_t *text, uint32_t buffer, uint32_t length, uint8_t *vars,
                 uint32_t bank, uint32_t &result)
{
    d.AmBank = bank;
    Tokeniser t;
    t.s = text;
    t.labels = vars;
    t.buffer = buffer;
    t.out = buffer;
    t.end = buffer + uint16_t(length);
    t.bank = bank;
    for (int i = 0; i < 14; i++)
        poke_l(vars + 4 * i, 0xFFFFFFFF);
    t.loops = vars + 26 * 2 + 26 * 4;
    poke_l(t.loops, 0xFFFFFFFF);
    t.loops += 4;
    if (!t.tokenise(type)) {
        result = address(t.s) - address(text);
        return t.error;
    }
    uint32_t size = t.out - buffer;
    if (t.overflow) {
        result = t.overflow + size;
        return -1;
    }
    result = size;
    return 0;
}

// ---- the channels -------------------------------------------------------------

// DAMAL: channel c out of the list (head: AmDeb's copy) and freed
void remove(uint32_t &head, uint32_t c)
{
    Channel &ch = channel(c);
    uint32_t next = ch.AmNext, prev = ch.AmPrev;
    if (prev)
        channel(prev).AmNext = next;
    else
        head = next;
    if (next)
        channel(next).AmPrev = prev;
    free_mm(c, uint32_t(int32_t(int16_t(ch.AmLong))));
}

// ResAMAL: the block of a channel (size: of the program in the buffer,
// copied; 0: block already the tokeniser's), its object. 0, -1 (out of
// memory), -3 (screen not open), -24 (bob not defined).
int32_t reserve_channel(WData &d, uint32_t &block, uint32_t size, uint32_t buffer, uint16_t id, uint32_t target,
                        uint32_t object)
{
    if (size) {
        uint16_t total = uint16_t(size + AmStart + 4);
        uint32_t b = fast_mm(total);
        if (!b)
            return -1;
        block = b;
        Channel &ch = channel(b);
        ch.AmLong = total;
        ch.AmNb = id;
        ch.AmPos = b + AmStart;
        uint16_t longs = uint16_t(uint16_t(total - (AmStart + 4)) >> 2);
        for (uint32_t i = 0; i <= longs; i++)
            poke_l(ptr<void>(b + AmStart + 4 * i), peek_l(ptr<void>(buffer + 4 * i)));
    } else {
        channel(block).AmNb = id;
        channel(block).AmPos = block + AmStart;
    }
    Channel &ch = channel(block);
    switch (uint16_t(target)) {
    case 6:
        // a rainbow
        ch.AmAct = address(d.RainTable) + uint32_t(uint16_t(object) * RainLong) + RnActAt;
        ch.AmBit = 0x8000 + BitEcrans;
        return 0;
    case 5:
        // an address
        ch.AmAct = object;
        ch.AmBit = uint16_t(0x8000 + ((target >> 16) & 0xFF));
        return 0;
    case 0:
        ch.AmAct = address(&d.HsTAct[uint16_t(object << 3) / 2]);
        ch.AmBit = 0x8000 + BitSprites;
        return 0;
    case 1: {
        amos::Bob *b = nullptr;
        for (uint32_t a = d.BbDeb; a; a = ptr<amos::Bob>(a)->BbNext) {
            if (ptr<amos::Bob>(a)->BbNb == uint16_t(object)) {
                b = ptr<amos::Bob>(a);
                break;
            }
            if (uint16_t(object) < ptr<amos::Bob>(a)->BbNb)
                break;
        }
        if (!b) {
            free_mm(block, ch.AmLong);
            return -24;
        }
        ch.AmAct = address(&b->BbAct);
        ch.AmBit = 0x8000 + BitBobs;
        return 0;
    }
    default: {
        uint32_t s = d.EcAdr[uint16_t(object << 2) / 4];
        if (!s) {
            free_mm(block, ch.AmLong);
            return -3;
        }
        Screen *e = ptr<Screen>(s);
        ch.AmAct = uint16_t(target) == 2 ? address(&e->EcAW) : uint16_t(target) == 3 ? address(&e->EcAWT) : address(&e->EcAV);
        ch.AmBit = 0x8000 + BitEcrans;
        return 0;
    }
    }
}

// CreAMAL: channel n (type) tokenised and put in the list in place of the
// one there. result: the error's place.
int32_t create(WData &d, AllRegs *r)
{
    const uint8_t *text = ptr<uint8_t>(r->a[1]);
    uint32_t buffer = r->a[2], length = r->d[2];
    uint8_t *vars = ptr<uint8_t>(r->d[1]);
    uint16_t type = uint16_t(r->d[3]);
    uint32_t target = r->d[4], object = r->d[5];
    uint16_t number = uint16_t(r->d[6]);
    uint32_t result;
    int32_t e = tokenise(d, type, text, buffer, length, vars, r->d[7], result);
    uint32_t block = 0, size = result;
    if (e > 0) {
        r->a[0] = result;
        return e;
    }
    if (e < 0) {
        // too long for the buffer: in a block of its size
        uint32_t total = result + AmStart + 4;
        block = fast_mm(total);
        if (!block)
            return -1;
        channel(block).AmLong = uint16_t(total);
        tokenise(d, type, text, block + AmStart, total - (AmStart + 4), vars, r->d[7], result);
        size = 0;
    }
    r->a[0] = size;
    d.AmChaine = 0;
    d.AmFreeze = 0;
    uint16_t id = uint16_t((number << 2) + type);
    uint32_t c = d.AmDeb;
    if (!c) {
        int32_t err = reserve_channel(d, block, size, buffer, id, target, object);
        if (err)
            return err;
        d.AmDeb = block;
    } else {
        uint32_t last = 0;
        for (; c; last = c, c = channel(c).AmNext) {
            Channel &old = channel(c);
            if (old.AmNb == id) {
                // in place of the old one
                uint32_t prev = old.AmPrev, next = old.AmNext;
                free_mm(c, old.AmLong);
                int32_t err = reserve_channel(d, block, size, buffer, id, target, object);
                if (err) {
                    // (the original linked the freed block, or the buffer,
                    // into the list)
                    if (prev)
                        channel(prev).AmNext = next;
                    else
                        d.AmDeb = next;
                    if (next)
                        channel(next).AmPrev = prev;
                    return err;
                }
                channel(block).AmPrev = prev;
                channel(block).AmNext = next;
                if (next)
                    channel(next).AmPrev = block;
                if (prev)
                    channel(prev).AmNext = block;
                else
                    d.AmDeb = block;
                d.AmChaine = d.AmDeb;
                return 0;
            }
            if (id < old.AmNb) {
                // before it
                int32_t err = reserve_channel(d, block, size, buffer, id, target, object);
                if (err)
                    return err;
                uint32_t prev = old.AmPrev, after;
                old.AmPrev = block;
                channel(block).AmPrev = prev;
                if (!prev) {
                    after = d.AmDeb;
                    d.AmDeb = block;
                } else {
                    after = channel(prev).AmNext;
                    channel(prev).AmNext = block;
                }
                channel(block).AmNext = after;
                d.AmChaine = d.AmDeb;
                return 0;
            }
        }
        // at the end
        int32_t err = reserve_channel(d, block, size, buffer, id, target, object);
        if (err)
            return err;
        channel(block).AmPrev = last;
        channel(last).AmNext = block;
    }
    d.AmChaine = d.AmDeb;
    return 0;
}

// AdAMAL: the first block of channel n from c on (type: its type)
uint32_t find(uint32_t c, uint16_t n, uint16_t &type)
{
    n = uint16_t(n << 2);
    for (; c; c = channel(c).AmNext) {
        uint16_t nb = channel(c).AmNb & 0xFFFC;
        if (nb == n) {
            type = channel(c).AmNb & 3;
            return c;
        }
        if (n < nb)
            return 0;
    }
    return 0;
}

// OnOfFrz: mode < 0 off, 0 freeze, > 0 on
void on_off(uint32_t &head, uint32_t c, int16_t mode)
{
    if (mode < 0)
        remove(head, c);
    else if (!mode)
        channel(c).AmBit |= 0x8000;
    else
        channel(c).AmBit &= 0x7FFF;
}

// MvOAMAL: channel n (< 0: all) of the types of mask
void move_on_off(WData &d, int16_t n, uint16_t mask, int16_t mode)
{
    uint32_t head = d.AmDeb;
    d.AmFreeze = 0;
    d.AmChaine = 0;
    if (n < 0) {
        for (uint32_t c = head; c;) {
            uint32_t next = channel(c).AmNext;
            if (mask & (1 << (channel(c).AmNb & 3)))
                on_off(head, c, mode);
            c = next;
        }
    } else {
        uint16_t type;
        uint32_t c = find(head, uint16_t(n), type);
        while (c) {
            uint32_t next = channel(c).AmNext;
            if (mask & (1 << type))
                on_off(head, c, mode);
            c = find(next, uint16_t(n), type);
        }
    }
    d.AmDeb = head;
    d.AmChaine = head;
}

// ---- the animator -------------------------------------------------------------

struct Value {
    int16_t result, last;   // d3, and d2: the last operand
};

struct Animator {
    WData &d;
    Channel &ch;
    uint32_t at;                // the channel's block
    uint32_t a3;
    int count;                  // d6: jumps before the next VBL
    bool main;                  // the program, not the autotest
    uint16_t bit;               // d4
    uint16_t &act;              // d5: T_Actualise

    uint16_t word()
    {
        uint16_t w = pw(a3);
        a3 += 2;
        return w;
    }
    uint32_t reg_address(int16_t offset)
    {
        return offset >= 0 ? address(d.AmRegs) + uint32_t(offset) : at + AmStart + int32_t(offset);
    }
    void touched(int flag)
    {
        *ptr<uint8_t>(ch.AmAct) |= uint8_t(1 << flag);
        // (bset on a data register: the bit modulo 32, then a word kept)
        if ((bit & 31) < 16)
            act |= uint16_t(1 << (bit & 31));
    }

    IData &idata_of() { return *reinterpret_cast<IData *>(reinterpret_cast<uint8_t *>(&d) + sizeof(WData)); }

    Value evaluate();
    int16_t operand(uint16_t op);
    bool run();                 // false: the channel's turn is over
    bool step(uint16_t op);
    int16_t screen_coordinate(uint16_t op);
    int16_t collision(bool bobs);
};

// AmEvalue
Value Animator::evaluate()
{
    int16_t last = operand(word()), result = last;
    for (;;) {
        uint16_t op = word();
        if (op == AmSExp)
            return {result, last};
        last = operand(op);
        int16_t r = result, v = last;
        switch (word()) {
        case AmEg:
            r = v == r ? -1 : 0;
            break;
        case AmInf:
            r = r < v ? -1 : 0;
            break;
        case AmSup:
            r = r > v ? -1 : 0;
            break;
        case AmDif:
            r = r != v ? -1 : 0;
            break;
        case AmPlus:
            r = int16_t(r + v);
            break;
        case AmMoins:
            r = int16_t(r - v);
            break;
        case AmDiv:
            if (v) {
                int32_t q = int32_t(r) / v;
                if (q >= -32768 && q <= 32767)
                    r = int16_t(q);
            }
            break;
        case AmMult:
            r = int16_t(int32_t(r) * v);
            break;
        case AmOr:
            r = int16_t(r | v);
            break;
        case AmAnd:
            r = int16_t(r & v);
            break;
        case AmXor:
            r = int16_t(r ^ v);
            break;
        default:
            break;
        }
        result = r;
    }
}

// XS, YS, XH, YH: a coordinate converted on a screen (-1 if not open)
int16_t Animator::screen_coordinate(uint16_t op)
{
    int16_t n = evaluate().result;
    int16_t c = evaluate().result;
    uint32_t s = d.EcAdr[n & 7];
    if (!s)
        return -1;
    const Screen *e = ptr<Screen>(s);
    switch (op) {
    case AmXH:
        if (int16_t(e->EcCon0) < 0)
            c = int16_t(c >> 1);
        return int16_t(c + e->EcWX);
    case AmYH:
        return int16_t(c + e->EcWY - EcYBase);
    case AmXS:
        c = int16_t(c - e->EcWX);
        if (e->EcCon0 & 0x8000)
            c = int16_t(c << 1);
        return int16_t(c + e->EcVX);
    default:
        return int16_t(c + EcYBase - e->EcWY + e->EcVY);
    }
}

// BC(n,first,last), SC(...): with Synchro Off only, in the task
int16_t Animator::collision(bool bobs)
{
    if (!d.SyncOff) {
        // (the original read its arguments as instructions)
        evaluate();
        evaluate();
        evaluate();
        return 0;
    }
    int16_t n = evaluate().result, first = evaluate().result, last = evaluate().result;
    AllRegs r = {};
    r.d[1] = uint32_t(n);
    r.d[2] = uint32_t(first);
    r.d[3] = uint32_t(last);
    if (bobs)
        w_sy_bob_collisions(&r);
    else
        w_sy_sprite_collisions(&r);
    return int16_t(r.d[0]);
}

int16_t Animator::operand(uint16_t op)
{
    switch (op) {
    case AmChif:
        return int16_t(word());
    case AmEgA:
        return sw16(ch.AmAct + 6);
    case AmEgX:
        return sw16(ch.AmAct + 2);
    case AmEgY:
        return sw16(ch.AmAct + 4);
    case AmEgR:
        return sw16(reg_address(int16_t(word())));
    case AmOn:
        return int16_t(ch.AmCpt) > 0 ? -1 : 0;
    case AmXMou:
        return int16_t(d.XMouse);
    case AmYMou:
        return int16_t(d.YMouse);
    case AmJ0:
    case AmJ1: {
        // ClJoy
        int port = op == AmJ1;
        if (!(d.AMOSHere >> 8))
            return 0;
        static const uint8_t directions[16] = {0, 2, 10, 8, 1, 0, 0, 9, 5, 0, 0, 0, 4, 6, 0, 0};
        uint16_t counters = (&chips().joy0dat)[port];
        uint16_t bits = 0;
        if (!(hal::cia_a_pra() & (1 << ((6 + port) & 7))))
            bits |= 1 << 4;
        bits |= directions[(counters >> 6 & 0x0C) | (counters & 3)];
        return int16_t(bits);
    }
    case AmM1:
        return hal::cia_a_pra() & 0x40 ? 0 : -1;
    case AmM2:
        return hal::custom_word(0x016) & 0x0400 ? -1 : 0;
    case AmXS:
    case AmYS:
    case AmXH:
    case AmYH:
        return screen_coordinate(op);
    case AmCol: {
        int16_t n = evaluate().result;
        return int16_t(w_collision_get(reinterpret_cast<uint8_t *>(&d) + sizeof(WData), n));
    }
    case AmHaz: {
        int16_t mask = evaluate().result;
        uint32_t v = uint32_t(d.AmSeed) * 0x3171;
        v = (v & 0xFFFF0000u) | uint16_t(v + hal::custom_word(0x006) + 1);
        d.AmSeed = uint16_t(v);
        return int16_t(uint16_t(v >> 8) & mask);
    }
    case AmVu: {
        uint16_t n = uint16_t(evaluate().last);
        uint32_t music = idata_of().ExtAdr[0];
        if (!music || n >= 4)
            return 0;
        uint8_t *vu = ptr<uint8_t>(music + n);
        uint8_t v = *vu;
        *vu = 0;
        return int16_t(int8_t(v));
    }
    case AmSCol:
        return collision(false);
    case AmBCol:
        return collision(true);
    default:
        return 0;
    }
}

// The next instruction after Move or PLay: at AmFin
bool next_instruction(Animator &a)
{
    a.a3 = a.ch.AmFin;
    return true;
}

// AmMv0: the position with its fractions; the object moved if it changed
// (old: what it is compared with)
void place(Animator &a, uint32_t x, uint32_t y, uint16_t old_x, uint16_t old_y)
{
    Channel &ch = a.ch;
    ch.AmVirgX = uint16_t(x);
    ch.AmVirgY = uint16_t(y);
    if (uint16_t(x >> 16) != old_x) {
        sw(ch.AmAct + 2, uint16_t(x >> 16));
        a.touched(1);
    }
    if (uint16_t(y >> 16) != old_y) {
        sw(ch.AmAct + 4, uint16_t(y >> 16));
        a.touched(2);
    }
}

// The step of a Move: v / steps, 8 bits of fraction (divu: 0 if too big)
int32_t move_delta(int16_t v, uint16_t steps)
{
    bool negative = v < 0;
    uint32_t m = uint32_t(negative ? -int32_t(v) : int32_t(v)) << 8;
    uint32_t q = m / steps;
    uint16_t w = q > 0xFFFF ? 0 : uint16_t(q);
    if (negative)
        w = uint16_t(-w);
    return int32_t(int16_t(w)) << 8;
}

// MOVE dx,dy,steps: a step a VBL
bool move(Animator &a)
{
    Channel &ch = a.ch;
    int16_t c = int16_t(--ch.AmCpt);
    if (!c)
        return next_instruction(a);
    if (c > 0) {
        uint16_t x = pw(ch.AmAct + 2), y = pw(ch.AmAct + 4);
        place(a, (uint32_t(x) << 16 | ch.AmVirgX) + ch.AmDeltX, (uint32_t(y) << 16 | ch.AmVirgY) + ch.AmDeltY, x, y);
        return false;
    }
    // the start: the steps
    ch.AmPos = a.a3 - 2;
    int16_t dx = a.evaluate().result;
    int16_t dy = a.evaluate().result;
    int16_t steps = a.evaluate().result;
    ch.AmFin = a.a3;
    if (steps <= 0)
        steps = 1;
    ch.AmCpt = uint16_t(steps);
    ch.AmDeltX = uint32_t(move_delta(dx, uint16_t(steps)));
    ch.AmDeltY = uint32_t(move_delta(dy, uint16_t(steps)));
    uint32_t x = uint32_t(pw(ch.AmAct + 2)) << 16 | 0x8000, y = uint32_t(pw(ch.AmAct + 4)) << 16 | 0x8000;
    // (compared with $8000, as the original: the first step always moves)
    place(a, ch.AmDeltX + x, ch.AmDeltY + y, 0x8000, 0x8000);
    return false;
}

// One coordinate of PLay: a byte, -128-127 a move, else a wait
void play_axis(Animator &a, be_u32 &pointer, be_u16 &wait, uint16_t at, int flag, int16_t direction)
{
    Channel &ch = a.ch;
    int8_t b = *ptr<int8_t>(pointer);
    if (b < 0) {
        uint16_t w = uint16_t(--wait);
        if (w && int16_t(w) > 0)
            return;
        if (w) {
            uint8_t count = uint8_t(b) & 0x7F;
            if (count) {
                wait = count;
                return;
            }
        }
        if (!direction)
            pointer--;
        else
            pointer++;
        return;
    }
    int16_t step = int16_t(int8_t(uint8_t(b << 1)) >> 1);
    a.touched(flag);
    if (direction) {
        sw(ch.AmAct + at, uint16_t(pw(ch.AmAct + at) + step));
        pointer++;
    } else {
        sw(ch.AmAct + at, uint16_t(pw(ch.AmAct + at) - step));
        pointer--;
    }
}

// PLay n: the movement n of the AMAL bank, R0 its speed, R1 its direction
bool play(Animator &a)
{
    Channel &ch = a.ch;
    int16_t c = int16_t(--ch.AmCpt);
    if (c < 0) {
        ch.AmPos = a.a3 - 2;
        int16_t n = a.evaluate().result;
        ch.AmFin = a.a3;
        uint32_t bank = a.d.AmBank;
        if (!bank || uint16_t(n) > pw(bank + 4))
            return next_instruction(a);
        int16_t n2 = int16_t(uint16_t(n << 1));
        if (!n2)
            return next_instruction(a);
        uint16_t offset = pw(bank + 4 + n2);
        if (!offset)
            return next_instruction(a);
        uint32_t m = bank + 4 + int16_t(uint16_t(offset << 1));
        sw(a.at + R0, pw(m));
        sw(a.at + R1, 1);
        int16_t y = int16_t(pw(m + 2));
        m += 4;
        ch.AmDeltY = m - 4 + 1 + y;
        ch.AmDeltX = m + 1;
        ch.AmVirgX = 0;
        ch.AmVirgY = 0;
    } else if (c) {
        return false;
    }
    int16_t direction = sw16(a.at + R1);
    if (direction < 0 || !*ptr<int8_t>(ch.AmDeltX))
        return next_instruction(a);
    play_axis(a, ch.AmDeltX, ch.AmVirgX, 2, 1, direction);
    if (!*ptr<int8_t>(ch.AmDeltY))
        return next_instruction(a);
    play_axis(a, ch.AmDeltY, ch.AmVirgY, 4, 2, direction);
    ch.AmCpt = pw(a.at + R0);
    return false;
}

// StML: a STOS movement from its start (or again); a1: the coordinate
void stos_move_start(Animator &a, int flag, uint32_t a1)
{
    Channel &ch = a.ch;
    uint16_t v = a.word();
    if (v != 0x8000) {
        sw(a1, v);
        a.touched(flag);
    }
    a.a3 += 2;
    high(ch.AmDeltY) = a.word();
    ch.AmAAd = a.a3;
    high(ch.AmDeltX) = pw(a.a3 + 4);
}

// StM0: the next step of a STOS movement
void stos_move(Animator &a, int flag, uint32_t a1)
{
    Channel &ch = a.ch;
    a.a3 = ch.AmAAd;
    ch.AmACpt = a.word();
    uint16_t v = uint16_t(pw(a1) + a.word());
    sw(a1, v);
    a.touched(flag);
    if (v != high(ch.AmDeltY)) {
        if (--high(ch.AmDeltX))
            return;
        // the next movement
        a.a3 += 2;
        if (pw(a.a3)) {
            ch.AmAAd = a.a3;
            high(ch.AmDeltX) = pw(a.a3 + 4);
            return;
        }
    }
    // the end: again if it loops
    a.a3 = ch.AmAALoop;
    if (!pw(a.a3 + 2)) {
        ch.AmAJsr = NoAnim;
        return;
    }
    stos_move_start(a, flag, a1);
}

// The animations after the program: Anim, the STOS forms
void anim(Animator &a)
{
    Channel &ch = a.ch;
    switch (ch.AmAJsr) {
    case DoAni:
        if (--ch.AmACpt)
            return;
        a.a3 = ch.AmAAd;
        for (;;) {
            if (!pw(a.a3)) {
                if (ch.AmACLoop && !--ch.AmACLoop) {
                    ch.AmAJsr = NoAnim;
                    return;
                }
                a.a3 = ch.AmAALoop;
                continue;
            }
            sw(ch.AmAct + 6, uint16_t(a.evaluate().result));
            a.touched(0);
            ch.AmACpt = uint16_t(a.evaluate().result);
            ch.AmAAd = a.a3;
            return;
        }
    case StAni:
        if (--ch.AmACpt)
            return;
        a.a3 = ch.AmAAd;
        for (;;) {
            int16_t v = int16_t(a.word());
            if (v < 0) {
                if (v == -1) {
                    ch.AmAJsr = NoAnim;
                    return;
                }
                a.a3 = ch.AmAALoop;
                continue;
            }
            sw(ch.AmAct + 6, uint16_t(v));
            a.touched(0);
            ch.AmACpt = a.word();
            if (!ch.AmACpt) {
                ch.AmAJsr = NoAnim;
                return;
            }
            ch.AmAAd = a.a3;
            return;
        }
    case StMvX:
    case StMvY:
        if (--ch.AmACpt)
            return;
        if (ch.AmAJsr == StMvX)
            stos_move(a, 1, ch.AmAct + 2);
        else
            stos_move(a, 2, ch.AmAct + 4);
        return;
    default:
        return;
    }
}

bool Animator::step(uint16_t op)
{
    switch (op) {
    case AmStop:
        ch.AmPos = 0;
        ch.AmAuto = 0;
        return false;
    case AmAnim:
        ch.AmACpt = 1;
        ch.AmAAd = a3;
        ch.AmAALoop = a3;
        ch.AmAJsr = StAni;
        ch.AmPos = 0;
        return false;
    case AmMvtX:
    case AmMvtY:
        ch.AmACpt = 1;
        ch.AmAALoop = a3;
        ch.AmAJsr = op == AmMvtX ? StMvX : StMvY;
        ch.AmPos = 0;
        if (op == AmMvtX)
            stos_move_start(*this, 1, ch.AmAct + 2);
        else
            stos_move_start(*this, 2, ch.AmAct + 4);
        return false;
    case AmWait:
        ch.AmPos = 0;
        return false;
    case AmPose:
        ch.AmPos = a3;
        return false;
    case AmMove:
        return move(*this);
    case AmPlay:
        return play(*this);
    case AmJump:
        a3 += int16_t(pw(a3));
        if (!--count) {
            // enough for this VBL: on from there at the next
            if (main)
                ch.AmPos = a3;
            return false;
        }
        return true;
    case AmLet: {
        int16_t v = evaluate().result;
        uint32_t to;
        switch (word()) {
        case AmAEg:
            touched(0);
            to = ch.AmAct + 6;
            break;
        case AmXEg:
            touched(1);
            to = ch.AmAct + 2;
            break;
        case AmYEg:
            touched(2);
            to = ch.AmAct + 4;
            break;
        default:
            to = reg_address(int16_t(word()));
            break;
        }
        sw(to, uint16_t(v));
        return true;
    }
    case AmIf:
        if (!evaluate().result)
            a3 += 4;
        return true;
    case AmFor: {
        int16_t start = evaluate().result;
        int16_t end = evaluate().result;
        sw(reg_address(int16_t(word())), uint16_t(start));
        sw(a3, uint16_t(end));
        a3 += 2;
        return true;
    }
    case AmNxt: {
        int16_t offset = int16_t(word());
        uint32_t loop = a3 - 2 + offset;
        uint32_t r = reg_address(sw16(loop));
        sw(r, uint16_t(pw(r) + 1));
        int16_t end = sw16(loop + 2);
        if (end >= sw16(r)) {
            ch.AmPos = loop + 4;
            return false;
        }
        return true;
    }
    case AmAOn: {
        int16_t offset = int16_t(word());
        ch.AmAuto = a3;
        a3 = a3 - 2 + offset;
        return true;
    }
    case AmAOff:
        ch.AmAuto = 0;
        return true;
    case AmDirect: {
        int16_t offset = int16_t(word());
        ch.AmPos = a3 - 2 + offset;
        ch.AmCpt = 0;
        return false;
    }
    case AmAni: {
        ch.AmACLoop = uint16_t(evaluate().result);
        ch.AmACpt = 1;
        ch.AmAAd = a3 + 2;
        ch.AmAALoop = a3 + 2;
        ch.AmAJsr = DoAni;
        a3 += int16_t(pw(a3));
        return true;
    }
    default:        // AmAExit, and what is not an instruction
        return false;
    }
}

bool Animator::run()
{
    while (step(word()))
        ;
    return false;
}

// Animeur: every channel not frozen: its autotest, its program, its
// animation
void animate(WData &d)
{
    uint32_t chain = d.AmChaine;
    if (!chain)
        return;
    d.AmChaine = 0;
    uint16_t act = d.Actualise;
    for (uint32_t c = chain; c; c = channel(c).AmNext) {
        Channel &ch = channel(c);
        uint16_t bit = ch.AmBit;
        if (int16_t(bit) < 0)
            continue;
        Animator a{d, ch, c, 0, 0, false, bit, act};
        if (ch.AmAuto) {
            a.a3 = ch.AmAuto;
            a.count = 20;
            a.main = false;
            a.run();
        }
        if (ch.AmPos) {
            a.a3 = ch.AmPos;
            a.count = 10;
            a.main = true;
            a.run();
        }
        if (ch.AmAJsr)
            anim(a);
    }
    d.Actualise = act;
    d.AmChaine = chain;
}

} // namespace

// ---- the entries ---------------------------------------------------------------

extern "C" {

// The animator from the VBL (and Synchro): data, the data zone (a5)
void w_amal_animate_data(uint8_t *data) { animate(*reinterpret_cast<WData *>(data - sizeof(WData))); }

// AMALTok
void w_sy_amal_tok(AllRegs *r)
{
    uint32_t result;
    r->d[0] = uint32_t(tokenise(wdata(), uint16_t(r->d[3]), ptr<uint8_t>(r->a[1]), r->a[2], r->d[2], ptr<uint8_t>(r->d[1]),
                                r->d[7], result));
    r->a[0] = result;
}

// AMALCre: d0 0, an error of the tokeniser (a0 its place), -1 out of
// memory, -3 screen not open, -24 bob not defined
void w_sy_amal_cre(AllRegs *r)
{
    r->d[0] = uint32_t(create(wdata(), r));
}

// AMALMvO: d1 channel (< 0 all), d2 types, d3 -1 off, 0 freeze, 1 on
void w_sy_amal_mvo(AllRegs *r)
{
    move_on_off(wdata(), int16_t(r->d[1]), uint16_t(r->d[2]), int16_t(r->d[3]));
    r->d[0] = 0;
}

// AMALDAll, AMALEnd: all the channels off
void w_sy_amal_dall(AllRegs *r)
{
    move_on_off(wdata(), -1, 0x0F, -1);
    r->d[0] = 0;
}

void w_sy_amal_animate(AllRegs *) { animate(wdata()); }

// AMALReg: d1 channel (< 0: the global ones), d2 type, d3 register. a0
// its address; d0 -1 if there is no such channel.
void w_sy_amal_reg(AllRegs *r)
{
    WData &d = wdata();
    uint16_t n = uint16_t(r->d[3] << 1);
    if (int16_t(r->d[1]) < 0) {
        r->a[0] = address(d.AmRegs) + n;
        r->d[0] = 0;
        return;
    }
    uint16_t id = uint16_t((uint16_t(r->d[1]) << 2) + uint16_t(r->d[2]));
    for (uint32_t c = d.AmDeb; c; c = channel(c).AmNext) {
        if (channel(c).AmNb == id) {
            r->a[0] = c + R0 - n;
            r->d[0] = 0;
            return;
        }
        if (id < channel(c).AmNb)
            break;
    }
    r->d[0] = uint32_t(-1);
}

// AMALClr: all off, the global registers 0
void w_sy_amal_clr(AllRegs *r)
{
    WData &d = wdata();
    move_on_off(d, -1, 0x0F, -1);
    for (be_u16 &reg : d.AmRegs)
        reg = 0;
    r->d[0] = 0;
}

// AMALFrz, AMALUFrz: all frozen / going again
void w_sy_amal_frz(AllRegs *r)
{
    WData &d = wdata();
    if (!d.AmFreeze) {
        d.AmFreeze = d.AmChaine;
        d.AmChaine = 0;
    }
    r->d[0] = 0;
}

void w_sy_amal_ufrz(AllRegs *r)
{
    WData &d = wdata();
    if (!d.AmChaine) {
        if (!d.AmFreeze) {
            r->d[0] = 0;
            return;
        }
        d.AmChaine = d.AmFreeze;
    }
    d.AmFreeze = 0;
    r->d[0] = 0;
}

// SetSync: d1 Synchro Off (not 0) / On; Synchro: a step if off
void w_sy_set_sync(AllRegs *r)
{
    wdata().SyncOff = uint16_t(r->d[1]);
    r->d[0] = 0;
}

void w_sy_sync(AllRegs *)
{
    WData &d = wdata();
    if (d.SyncOff)
        animate(d);
}

// PlaySet: channels d1 to d2: R0 (speed) d3, R1 (direction) d4 (EntNul:
// unchanged)
void w_sy_play_set(AllRegs *r)
{
    WData &d = wdata();
    uint16_t first = uint16_t(r->d[1] << 2), last = uint16_t(r->d[2] << 2);
    for (uint32_t c = d.AmDeb; c; c = channel(c).AmNext) {
        uint16_t nb = channel(c).AmNb;
        if (nb > last)
            break;
        if (nb < first)
            continue;
        if (r->d[3] != EntNul)
            sw(c + R0, uint16_t(r->d[3]));
        if (r->d[4] != EntNul)
            sw(c + R1, uint16_t(r->d[4]));
    }
    r->d[0] = 0;
}

// MovOn: d1 -1 if the Move X/Y of channel d1 (its first block of
// these) runs
void w_sy_movon(AllRegs *r)
{
    WData &d = wdata();
    uint16_t n = uint16_t(r->d[1]), type;
    r->d[1] = 0;
    for (uint32_t c = find(d.AmDeb, n, type); c; c = find(channel(c).AmNext, n, type)) {
        if (type == 2 || type == 3) {
            const Channel &ch = channel(c);
            if (int16_t(ch.AmBit) >= 0 && ch.AmAJsr)
                r->d[1] = uint32_t(-1);
            return;
        }
    }
}

// ChanA, ChanM: d1 -1 if channel d1's animation / program runs
static uint32_t channel_state(uint16_t n, bool program)
{
    uint16_t type;
    uint32_t c = find(wdata().AmDeb, n, type);
    if (!c || int16_t(channel(c).AmBit) < 0)
        return 0;
    return (program ? channel(c).AmPos : channel(c).AmAJsr) ? uint32_t(-1) : 0;
}

void w_sy_chan_a(AllRegs *r) { r->d[1] = channel_state(uint16_t(r->d[1]), false); }
void w_sy_chan_m(AllRegs *r) { r->d[1] = channel_state(uint16_t(r->d[1]), true); }

// DAdAMAL: the channels of the object at a0 off
void w_dad_amal_c(AllRegs *r)
{
    WData &d = wdata();
    uint32_t head = d.AmDeb;
    d.AmChaine = 0;
    d.AmFreeze = 0;
    for (uint32_t c = head; c;) {
        uint32_t next = channel(c).AmNext;
        if (channel(c).AmAct == r->a[0])
            remove(head, c);
        c = next;
    }
    d.AmDeb = head;
    d.AmChaine = head;
}

// AMALInit
void w_amal_init_c(AllRegs *r)
{
    WData &d = wdata();
    d.AmDeb = 0;
    d.AmChaine = 0;
    d.AmSeed = 0x1234;
    w_sy_amal_clr(r);
}
}
