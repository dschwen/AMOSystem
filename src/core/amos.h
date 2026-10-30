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

// AMOS.library's vector tables of SyCall and EcCall (+W.s)
extern "C" uint8_t w_sy_in[], w_ec_in[], w_wi_in[];

namespace amos {

// AMOS keeps addresses in 32 bits (another host keeps its memory below 4GB)
template <typename T> inline T *ptr(uint32_t address) { return reinterpret_cast<T *>(uintptr_t(address)); }
template <typename T> inline uint32_t address(T *p) { return uint32_t(reinterpret_cast<uintptr_t>(p)); }

// The interpreter registers, as saved by the glue
struct Ctx {
    uint32_t d6, d7;
    uint8_t *a3, *a4, *a5, *a6;
};

// The block of the contexts (core/call.s): the main task, its context, the
// context of any other task. AMOSPro's, shared with the programs it loads.
extern "C" uint8_t *amos_bridge;

// The context of the running task (ExecBase->ThisTask); on another host
// (src/host) that of its thread
#if defined(__mc68000__)
inline Ctx &ctx()
{
    constexpr int ThisTask = 276;
    auto exec = *reinterpret_cast<const uint8_t *const *>(4);
    uint8_t *block = amos_bridge;
    bool main = *reinterpret_cast<const void *const *>(exec + ThisTask) == *reinterpret_cast<void **>(block);
    return *reinterpret_cast<Ctx *>(block + (main ? 4 : 4 + sizeof(Ctx)));
}
#else
Ctx &ctx();
#endif

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
    auto table = reinterpret_cast<const be_u32 *>(uintptr_t(idata().AdTokens[0] - LB_Size - 4));
    return reinterpret_cast<const void *>(uintptr_t(uint32_t(table[-routine])));
}

// The C++ function of a routine of AMOSPro.Lib whose glue is a RegRoutine
// (build/gen/lib_direct.cpp), or null
using RegFunction = uint32_t (*)(RegFrame &);
extern const RegFunction lib_direct[];
// Its glue (build/gen/lib_direct_entries.s), or 0
extern const uint32_t lib_direct_entry[];

// Calls the assembler at entry with registers r (below)
inline void entry_call(const void *entry, Regs &r);

// JJsr: call a routine of AMOSPro.Lib. A routine in C++ is called
// directly, as through the glue: d6/d7 from r in the context during the
// call, the context as it was afterwards.
inline void lib_call(int routine, Regs &r)
{
    RegFunction f = lib_direct[routine];
    if (f && lib_direct_entry[routine] == address(lib_address(routine))) {
        Ctx &c = ctx();
        Ctx saved = c;
        c.d6 = r.d[6];
        c.d7 = r.d[7];
        f(reinterpret_cast<RegFrame &>(r));
        ctx() = saved;
        return;
    }
    entry_call(lib_address(routine), r);
}

// JJsr to a routine that answers in the condition codes: -1 "less than",
// 0 "equal", 1 "greater than"
inline int32_t lib_call_cc(int routine, Regs &r) { return amos_call_cc(&r, lib_address(routine)); }

// JJsr to a routine that evaluates parameters from the program: the
// interpreter continues at the a6 (and with the a3) it leaves
inline void lib_eval(int routine, Regs &r) { amos_eval(&r, lib_address(routine)); }

// Rjmp to a routine of AMOSPro.Lib that does not come back (errors)
[[noreturn]] inline void lib_jump(int routine, uint32_t d0) { amos_goto(lib_address(routine), d0); }

extern "C" [[noreturn]] void interp_error(uint32_t n, uint32_t first, uint32_t extension, uint32_t messages);

// Stops the machine (the 68000's illegal instruction): a state that cannot be
[[noreturn]] inline void halt()
{
#if defined(__mc68000__)
    asm volatile("illegal");
#endif
    __builtin_trap();
}

// Raise run-time error n (L_GoError, L_Error: interp/control.cpp)
[[noreturn]] inline void error(uint32_t n) { interp_error(n, 19, uint32_t(-1), 0); }
// "Illegal function call"
[[noreturn]] inline void fon_call() { error(23); }
// EcWiErr: an error of the screens and windows (1: out of memory)
[[noreturn]] inline void screen_error(uint32_t e)
{
    error(uint16_t(e) == 1 ? 24 : uint16_t(uint16_t(e) + 45 - 1));
}

// SyCall, EcCall, WiCall: a function of the AMOS.library vector tables
// (amos::sy::X, amos::ec::X, amos::wi::X). Most return an error code in d0.
//
// While the vectors are AMOS.library's own (the monitor patches them), a
// function in C++ is called directly, as through its glue (w/wglue.i,
// _w_go): all the registers, d6/d7 and a3-a6 its context during the call,
// the context as it was afterwards (build/gen/w_direct.cpp).
using WFunction = void (*)(AllRegs *);
extern const WFunction sy_direct[], ec_direct[];
template <typename F> inline void w_direct_call(F f, Regs &r)
{
    Ctx &c = ctx();
    AllRegs a;
    for (int i = 0; i < 8; i++)
        a.d[i] = r.d[i];
    for (int i = 0; i < 3; i++)
        a.a[i] = r.a[i];
    a.a[3] = address(c.a3);
    a.a[4] = address(c.a4);
    a.a[5] = address(c.a5);
    a.a[6] = address(c.a6);
    Ctx saved = c;
    c.d6 = r.d[6];
    c.d7 = r.d[7];
    f(&a);
    ctx() = saved;
    for (int i = 0; i < 8; i++)
        r.d[i] = a.d[i];
    for (int i = 0; i < 3; i++)
        r.a[i] = a.a[i];
}
// A WEntry glue (w/wglue.i) and its C++ function; the function of an
// entry, or null (main/direct.cpp)
struct WEntryPair {
    const void *entry;
    WFunction function;
};
WFunction wentry_function(const void *entry);

// Calls the assembler at entry with registers r; directly the C++ of a
// WEntry glue
inline void entry_call(const void *entry, Regs &r)
{
    if (WFunction f = wentry_function(entry))
        return w_direct_call(f, r);
    amos_call(&r, entry);
}

inline void sy_call(int function, Regs &r)
{
    if (wdata().SyVect == address(::w_sy_in))
        if (WFunction f = sy_direct[function])
            return w_direct_call(f, r);
    amos_call(&r, reinterpret_cast<const uint8_t *>(uintptr_t(uint32_t(wdata().SyVect))) + 4 * function);
}
inline void ec_call(int function, Regs &r)
{
    if (wdata().EcVect == address(::w_ec_in))
        if (WFunction f = ec_direct[function])
            return w_direct_call(f, r);
    amos_call(&r, reinterpret_cast<const uint8_t *>(uintptr_t(uint32_t(wdata().EcVect))) + 4 * function);
}
} // namespace amos
// AMOS.library's text window functions (w/windows.cpp)
extern "C" void w_window(uint32_t function, amos::AllRegs *r);
namespace amos {

inline void wi_call(int function, Regs &r)
{
    if (wdata().WiVect == address(::w_wi_in))
        return w_direct_call([function](AllRegs *a) { ::w_window(uint32_t(function), a); }, r);
    amos_call(&r, reinterpret_cast<const uint8_t *>(uintptr_t(uint32_t(wdata().WiVect))) + 4 * function);
}

// The memory of AMOS.library (w/memory.cpp): SyCall MemReserve (size, the
// AllocMem flags; 0 if none), MemFree
namespace w {
uint32_t reserve(uint32_t size, uint32_t flags);
void release(uint32_t address, uint32_t size);
// The flags of MemFastClear, MemFast, MemChipClear, MemChip
constexpr uint32_t FastClear = 0x10001, Fast = 0x1, ChipClear = 0x10003, Chip = 0x3;
// Routine lists (Sys_ClearRoutines...): AddRoutine (a link, then the code;
// added if in no list), CallRoutines (the list empties)
uint32_t add_routine(uint32_t routine, uint32_t list);
void call_routines(be_u32 *list);
} // namespace w

// The banks (lib/banks.cpp): Bnk.EffAll, Bnk.Change
void bank_erase_all();
void bank_changed();

// Files (lib/dos.cpp): Dsk.PathIt, the name in Name1 with the current
// directory; DiskError, the error of the last DOS call
void path_it();
[[noreturn]] void disk_error();

// AMOSPro's system routines (main/system.cpp), for all its parts
namespace boot {
void wait_vbl_multi();      // Sys_WaitMul: the next frame, the AMOS.library's tests first
const char *sys_message(uint32_t n);        // Sys_GetMessage: message n of the configuration
// GetMessage: message n of a list (d0 as the routine leaves it); Def_GetMessage:
// of the default resource's
const uint8_t *get_message(const uint8_t *list, uint32_t &d0);
const uint8_t *def_message(uint32_t n);
void sys_add_path(const char *name);        // Sys_AddPath: into Name1, with the system's path
uint32_t temp_buffer(uint32_t size);        // ResTempBuffer: its address, 0 (none, freed)
void recop();                               // ReCop: the copper lists computed again
void ask_dir();                             // AskDir: the current directory at Buffer+384
void ask_dir2(uint32_t lock);               // AskDir2: the directory of the lock (unlocked)
} // namespace boot

// Big-endian memory access. The Amiga is big-endian and AMOS data is
// word-aligned, so these are plain loads and stores here; a port to a
// little-endian host only has to change them.
inline uint16_t peek_w(const void *p) { return *static_cast<const be_u16 *>(p); }
inline uint32_t peek_l(const void *p) { return *static_cast<const be_u32 *>(p); }
inline void poke_w(void *p, uint16_t v) { *static_cast<be_u16 *>(p) = v; }
inline void poke_l(void *p, uint32_t v) { *static_cast<be_u32 *>(p) = v; }



} // namespace amos
