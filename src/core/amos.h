// The bridge between the converted C++ code and the assembler.
//
// The glue (src/glue.i, CCall) saves the interpreter registers d6-d7/a3-a6
// in the task's context, ctx(), on every entry into C++ and reloads them on
// the way out, so C++ always sees the current a5 data zone, a3 parameter
// stack and a6 program pointer, and calls back into the assembler run with
// them. A call into the assembler leaves the context as it was: what the
// callee does to those registers does not come back.
//
// The context is per task because the file selector runs Interface code in
// a task of its own (see core/call.s).
//
// Rules for C++ that calls back into the assembler:
// - A library routine may not return: errors reset the stack to the
//   interpreter's (BasSp), and running out of string space restarts the
//   whole instruction. So no objects with destructors may be alive across
//   such a call, and side effects should come after string allocation, as
//   in the assembler.
// - Results come back in registers only. Routines that report through the
//   condition codes need a register to test instead.
#pragma once

#include <stdint.h>

#include "core/layout.h"
#include "lib_routines.h"

namespace amos {

// The interpreter registers, as saved by the glue
struct Ctx {
    uint32_t d6, d7;
    uint8_t *a3, *a4, *a5, *a6;
};

// The block of the contexts (core/call.s): the main task, its context, the
// context of any other task. AMOSPro's, shared with the programs it loads.
extern "C" uint8_t *amos_bridge;

// The context of the running task (ExecBase->ThisTask)
inline Ctx &ctx()
{
    constexpr int ThisTask = 276;
    auto exec = *reinterpret_cast<const uint8_t *const *>(4);
    uint8_t *block = amos_bridge;
    bool main = *reinterpret_cast<const void *const *>(exec + ThisTask) == *reinterpret_cast<void **>(block);
    return *reinterpret_cast<Ctx *>(block + (main ? 4 : 4 + sizeof(Ctx)));
}

// The registers of a call into the assembler: d0-d7/a0-a2. d6/d7 start as
// the interpreter's (Demande, for one, depends on d7); a3-a6 always are.
struct Regs {
    uint32_t d[8] = {0, 0, 0, 0, 0, 0, ctx().d6, ctx().d7};
    uint32_t a[3] = {0, 0, 0};
};

// The registers of a RegRoutine (src/glue.i), changed in place
struct RegFrame {
    uint32_t d[8];      // d0-d7
    uint32_t a[3];      // a0-a2
};

// All the registers, for calls into code that wants its own a3-a6
struct AllRegs {
    uint32_t d[8];
    uint32_t a[7];
};

extern "C" {
void amos_call(Regs *r, const void *fn);
void amos_call_all(AllRegs *r, const void *fn);
void amos_eval(Regs *r, const void *fn);
int32_t amos_call_cc(Regs *r, const void *fn);
[[noreturn]] void amos_goto(const void *fn, uint32_t d0);
}

constexpr int LB_Size = 36;

// The interpreter data, above a5
inline IData &idata() { return *reinterpret_cast<IData *>(ctx().a5); }
// The AMOS.library data, below a5
inline WData &wdata() { return *reinterpret_cast<WData *>(ctx().a5 - sizeof(WData)); }

// Address of a routine of AMOSPro.Lib, as JJsr finds it
inline const void *lib_address(int routine)
{
    auto table = reinterpret_cast<const void *const *>(idata().AdTokens[0] - LB_Size - 4);
    return table[-routine];
}

// JJsr: call a routine of AMOSPro.Lib
inline void lib_call(int routine, Regs &r) { amos_call(&r, lib_address(routine)); }

// JJsr to a routine that answers in the condition codes: -1 "less than",
// 0 "equal", 1 "greater than"
inline int32_t lib_call_cc(int routine, Regs &r) { return amos_call_cc(&r, lib_address(routine)); }

// JJsr to a routine that evaluates parameters from the program: the
// interpreter continues at the a6 (and with the a3) it leaves
inline void lib_eval(int routine, Regs &r) { amos_eval(&r, lib_address(routine)); }

// Rjmp to a routine of AMOSPro.Lib that does not come back (errors)
[[noreturn]] inline void lib_jump(int routine, uint32_t d0) { amos_goto(lib_address(routine), d0); }

// Raise run-time error n (L_GoError)
[[noreturn]] inline void error(uint32_t n) { lib_jump(L_GoError, n); }
// "Illegal function call"
[[noreturn]] inline void fon_call() { error(23); }

// SyCall, EcCall, WiCall: a function of the AMOS.library vector tables
// (amos::sy::X, amos::ec::X, amos::wi::X). Most return an error code in d0.
inline void sy_call(int function, Regs &r) { amos_call(&r, reinterpret_cast<const uint8_t *>(wdata().SyVect) + 4 * function); }
inline void ec_call(int function, Regs &r) { amos_call(&r, reinterpret_cast<const uint8_t *>(wdata().EcVect) + 4 * function); }
inline void wi_call(int function, Regs &r) { amos_call(&r, reinterpret_cast<const uint8_t *>(wdata().WiVect) + 4 * function); }

// Big-endian memory access. The Amiga is big-endian and AMOS data is
// word-aligned, so these are plain loads and stores here; a port to a
// little-endian host only has to change them.
inline uint16_t peek_w(const void *p) { return *static_cast<const uint16_t *>(p); }
inline uint32_t peek_l(const void *p) { return *static_cast<const uint32_t *>(p); }
inline void poke_w(void *p, uint16_t v) { *static_cast<uint16_t *>(p) = v; }
inline void poke_l(void *p, uint32_t v) { *static_cast<uint32_t *>(p) = v; }

template <typename T> inline T *ptr(uint32_t address) { return reinterpret_cast<T *>(address); }
template <typename T> inline uint32_t address(T *p) { return reinterpret_cast<uint32_t>(p); }

} // namespace amos
