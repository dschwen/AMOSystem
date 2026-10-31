// The assembler's entries on another host: build/host/host_glue.cpp
// (tools/host_glue.py) lays out the assembler's tables and data and puts
// a slot of 4 bytes where each entry was; glue.cpp does what the glue
// macro of the entry did (src/glue.i, src/w/wglue.i) when it is called.
#pragma once

#include "core/amos.h"

// The data of a section of chip memory (the Amiga's)
#if defined(__mc68000__)
#define AMOS_CHIP __attribute__((section(".datachip")))
#else
#define AMOS_CHIP
#endif

namespace amos::host {

enum class GlueKind : uint8_t {
    libcall,        // LibCall: fn(p1..pn) from d3 and the a3 stack; a2 the result
    nop,            // LibNop: nothing
    reg,            // RegRoutine: uint32_t fn(RegFrame &)
    call,           // InCall, RoutineCall: fn()
    test,           // TestCall: fn(), the registers kept
    eval,           // FnCall, EvalRoutine: fn(EvalOut *): d0-d4/a0
    eval_d3,        // FnCallD3: the same, d3 its parameter
    record,         // RoutineRecord: fn(), the registers kept
    c_routine,      // CRoutine: parameters on the stack (none here)
    float_,         // FloatPar, FloatEntry: fn(d2-d4)
    w,              // WEntry: fn(AllRegs *), the context the caller's; a2 the size tested
    w_a0,           // WEntryA0: the same, a0 tested
    w_f,            // WEntryF: the same, the function's result tested
    window,         // WiEntry: w_window(a1, AllRegs *)
    alias,          // jmp/bra target
    stack,          // StackEntry: the stack back at BasSp, then target
    lib_jump,       // JJmp: the routine a1 of AMOSPro.Lib
    hand,           // assembler done by src/host: host_<name>(AllRegs &)
    missing,        // assembler with no C++ here
};
struct GlueEntry {
    const uint8_t *at;
    GlueKind kind;
    const void *fn;
    const void *target;
    int32_t a1, a2;
    const char *name;
};

extern const GlueEntry glue_entries[];
extern const int glue_entry_count;
void glue_relocate();

// The data of the assembler: n bytes (1, 2 or 4), big-endian
inline void put(uint8_t *at, int n, uint32_t v)
{
    for (int i = n - 1; i >= 0; i--, v >>= 8)
        at[i] = uint8_t(v);
}

// The tables ready (at the start)
void glue_start();
// The entry at an address, or null
const GlueEntry *glue_entry(const void *at);
// The entry called with all the registers; its condition codes (the value
// they were set from: negative, zero or positive)
int32_t glue_call(const void *at, AllRegs &r);

} // namespace amos::host
