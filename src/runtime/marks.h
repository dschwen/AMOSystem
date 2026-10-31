// The marks of the runtime on another host (host/runtime.cpp): what the
// assembler did with the stack pointer is done with marks set with setjmp.
#pragma once

#include "runtime/glue.h"

#include <setjmp.h>

namespace amos::host {

// A point of the stack to come back to: Run its target there (the stack
// above it given up), or Return the registers to the code that made it
struct Mark {
    jmp_buf jb;
    const void *target;         // the entry to run, then
    const void *then;           // the entry run after it (its return)
    void (*fn)();               // or this C++ (the context its registers)
    void (*regs_fn)(AllRegs &); // or this C++, with all the registers
    AllRegs regs;
};

enum { MarkRun = 1, MarkReturn = 2 };

// The interpreter's registers of the context, and back
inline AllRegs context_regs()
{
    const Ctx &c = ctx();
    AllRegs r = {};
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    r.a[3] = address(c.a3);
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = address(c.a6);
    return r;
}

inline void to_context(const AllRegs &r)
{
    Ctx &c = ctx();
    c.d6 = r.d[6];
    c.d7 = r.d[7];
    c.a3 = ptr<uint8_t>(r.a[3]);
    c.a4 = ptr<uint8_t>(r.a[4]);
    c.a5 = ptr<uint8_t>(r.a[5]);
    c.a6 = ptr<uint8_t>(r.a[6]);
}

// What runs at a mark: its target (then what follows it), or its C++
void run_at(Mark &m);

// The mark m set here: first(m) runs; a jump to the mark runs its target
// there; a return (or the end of what ran) gives the registers back
template <typename F> AllRegs mark_run(Mark &m, F first)
{
    switch (setjmp(m.jb)) {
    case 0:
        first();
        return m.regs;
    case MarkRun:
        run_at(m);
        return m.regs;
    default:
        return m.regs;
    }
}

[[noreturn]] void mark_jump(uint32_t mark, const void *target, const AllRegs &r, const void *then = nullptr);
[[noreturn]] void mark_jump_regs(uint32_t mark, void (*fn)(AllRegs &), const AllRegs &r);
[[noreturn]] void mark_jump_cpp(uint32_t mark, void (*fn)(), const AllRegs &r);
[[noreturn]] void mark_return(uint32_t mark, const AllRegs &r);

} // namespace amos::host
