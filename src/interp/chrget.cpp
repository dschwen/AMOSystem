// The ChrGet (+ILib.s New_ChrGet): the start of a program and the loop
// that runs its instructions. Each instruction's routine (with the call to
// its parameter collector the loader put before it) is called through
// interp_exec (chrget_glue.s) with the interpreter registers of the
// context. Prg_InsRet, where instructions that jump go back to, is
// interp_iret: it enters the loop again at a6.

#include "interp/interp.h"
#include "verif/verif.h"

using namespace amos;

extern "C" {
// chrget_glue.s
void interp_exec(const void *routine, uint32_t d0, uint32_t d1);
extern uint8_t interp_iret[];
// float.cpp
void interp_open_math_libraries();
// control.cpp
[[noreturn]] void interp_error(uint32_t n, uint32_t first, uint32_t extension, uint32_t messages);
}

namespace {

constexpr uint32_t NbEnd = 10;

// The routine of token t: the instruction's, or the routine the monitor
// put in its place (SetChrPatch)
const void *routine(const uint8_t *a4, uint16_t t, uint16_t patch)
{
    int32_t entry = patch ? -int32_t(patch) * 4 - 4 : int16_t(peek_w(a4 + int16_t(t)));
    return ptr<void>(peek_l(a4 - LB_Size + entry));
}

// The instructions and functions called here directly: a LibCall entry
// (src/glue.i, tagged before it) with integer parameters, or none. The
// routine of one with parameters starts with the jsr d16(a4) to the
// collector the loader put there, then the entry.
constexpr uint32_t Tag = 0x4C43616C;        // "LCal"
constexpr uint32_t GetP = 0x47657450;       // "GetP"
constexpr uint16_t JsrA4 = 0x4EAC;          // jsr d16(a4)
constexpr int Tokens = 0x4000;              // the token offsets cached
constexpr uint8_t LCNone = 3, LCDouble = 4;

struct Native {
    const void *routine;    // the table's entry it was found for
    const void *fn;         // the C function, or 0: the routine is called
    uint8_t params, result;
};
Native instructions[Tokens / 2], functions[Tokens / 2];

// The integer collectors: Parameters' entries 1 to 5 (Ent_1 to Ent_4,
// Ent_N; a function's 2 bytes before, past its opening parenthesis)
bool integer_collector(const uint8_t *a4, const uint8_t *collector, uint16_t t, bool function, uint8_t &params)
{
    const uint8_t *par = static_cast<const uint8_t *>(lib_address(L_Parameters));
    for (int k = 1; k <= 5; k++) {
        if (par + int16_t(peek_w(par + 2 * k)) - (function ? 2 : 0) == collector) {
            params = k < 5 ? uint8_t(k) : uint8_t(peek_w(a4 + int16_t(t) + 2));
            return true;
        }
    }
    return false;
}

Native find_native(const uint8_t *a4, uint16_t t, const void *routine, bool function)
{
    Native n = {routine, nullptr, 0, 0};
    const uint8_t *body = static_cast<const uint8_t *>(routine);
    uint8_t params = 0;
    if (peek_w(body) == JsrA4) {
        if (!integer_collector(a4, a4 + int16_t(peek_w(body + 2)), t, function, params))
            return n;
        body += 4;
    } else if (peek_l(body - 4) != GetP) {
        return n;
    }
    if (peek_l(body - 8) != Tag || peek_w(body - 12) != params)
        return n;
    n.result = uint8_t(peek_w(body - 10));
    if (function == (n.result == LCNone))
        return n;
    n.fn = ptr<void>(peek_l(body - 16));
    n.params = params;
    return n;
}

// The parameters (commas between them), integers (floats made integers),
// then the C function. Its result as 64 bits: d0 (the high half) and d1,
// as gcc returns them, for the functions of 32 and of 64 bits.
uint64_t call_native(const Native &n)
{
    uint32_t p[10];
    for (int i = 0; i < n.params; i++) {
        if (i)
            interp::pc() += 2;
        interp::Value v = interp::evaluate();
        if (interp::kind(v) == interp::TFloat)
            v = interp::to_integer(v);
        p[i] = v.v1;
    }
    using U = uint32_t;
    using R = uint64_t;
    switch (n.params) {
    case 0:
        return reinterpret_cast<R (*)()>(n.fn)();
    case 1:
        return reinterpret_cast<R (*)(U)>(n.fn)(p[0]);
    case 2:
        return reinterpret_cast<R (*)(U, U)>(n.fn)(p[0], p[1]);
    case 3:
        return reinterpret_cast<R (*)(U, U, U)>(n.fn)(p[0], p[1], p[2]);
    case 4:
        return reinterpret_cast<R (*)(U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3]);
    case 5:
        return reinterpret_cast<R (*)(U, U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3], p[4]);
    case 6:
        return reinterpret_cast<R (*)(U, U, U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3], p[4], p[5]);
    case 7:
        return reinterpret_cast<R (*)(U, U, U, U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
    case 8:
        return reinterpret_cast<R (*)(U, U, U, U, U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
    case 9:
        return reinterpret_cast<R (*)(U, U, U, U, U, U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
    default:
        return reinterpret_cast<R (*)(U, U, U, U, U, U, U, U, U, U)>(n.fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9]);
    }
}

// The native entry of token t (its routine r), or 0
const Native *native(IData &d, const uint8_t *a4, uint16_t t, const void *r, bool function)
{
    if (t >= Tokens || a4 != ptr<uint8_t>(d.AdTokens[0]))
        return nullptr;
    Native &n = (function ? functions : instructions)[t >> 1];
    if (n.routine != r)
        n = find_native(a4, t, r, function);
    return n.fn ? &n : nullptr;
}

// Instruction t
void instruction(IData &d, Ctx &c, uint16_t t)
{
    uint16_t patch = uint16_t(d.Cur_ChrJump >> 16);
    const void *r = routine(c.a4, t, patch);
    if (const Native *n = patch ? nullptr : native(d, c.a4, t, r, false)) {
        call_native(*n);
        return;
    }
    interp_exec(r, t, peek_w(c.a4 + int16_t(t)));
}

// The program's end
[[noreturn]] void end_reached()
{
    interp_error(NbEnd, 19, uint32_t(-1), 0);
}

// The loop, from the start of a line (at_line) or after an instruction
[[noreturn]] void run(bool at_line)
{
    IData &d = idata();
    Ctx &c = ctx();
    uint16_t t;
    if (!at_line)
        goto iret;
new_line:
    // the line's header
    t = peek_w(c.a6);
    c.a6 += 2;
    if (!t)
        end_reached();
in_line:
    t = peek_w(c.a6);
    c.a6 += 2;
    if (!t)
        goto new_line;
instruction:
    c.d7 = address(c.a6);
    instruction(d, c, t);
iret:
    t = peek_w(c.a6);
    c.a6 += 2;
    if (t)
        goto instruction;
    t = peek_w(c.a6);
    c.a6 += 2;
    if (t)
        goto in_line;
    end_reached();
}

} // namespace

extern "C" {

// New_ChrGet (after the stack at BasSp, a4 = AdTokens): the program at
// Prg_Run started
[[noreturn]] void interp_new_chrget()
{
    IData &d = idata();
    Ctx &c = ctx();
    d.Prg_InsRet = address(interp_iret);
    // the loops' buffer
    verif::stack_reserve();
    c.a3 = ptr<uint8_t>(d.HoLoop);
    d.PLoop = d.HoLoop;
    d.BasA3 = d.HoLoop;
    d.MinLoop = d.BaLoop + 64;
    interp_open_math_libraries();
    wdata().Actualise = 0;
    d.AData = 0;
    d.DefFlag = 1;
    c.a6 = ptr<uint8_t>(d.Prg_Run);
    d.DProc = d.Prg_Run;
    d.PData = d.Prg_Run;
    c.a4 = ptr<uint8_t>(d.AdTokens[0]);
    run(true);
}

// _IRet: after an instruction
[[noreturn]] void interp_chrget_continue() { run(false); }

// SetChrPatch: d0 the routine every instruction goes to (the monitor's),
// 0 for the instructions' own
uint32_t interp_set_chr_patch(RegFrame &r)
{
    idata().Cur_ChrJump = (idata().Cur_ChrJump & 0xFFFF) | (r.d[0] & 0xFFFF) << 16;
    return 0;
}
}

namespace amos::interp {

// Function t (its routine r) called here if it can be (eval.cpp)
bool native_function(uint16_t t, const void *r, Value &v)
{
    const Native *n = native(idata(), ctx().a4, t, r, true);
    if (!n)
        return false;
    // a function with parameters: past its opening parenthesis
    if (n->params)
        pc() += 2;
    uint64_t x = call_native(*n);
    uint32_t hi = uint32_t(x >> 32);
    switch (n->result) {
    case 0:
        v = {TInt, hi, 0};
        break;
    case 1:
        v = {TFloat, hi, 0};
        break;
    case 2:
        v = {TString, hi, 0};
        break;
    default:
        v = {TFloat, hi, uint32_t(x)};
        break;
    }
    return true;
}

} // namespace amos::interp
