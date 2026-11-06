// The marks of the runtime on another host (host/runtime.cpp): what the
// assembler did with the stack pointer is done with marks set with setjmp.
#pragma once

#include "runtime/glue.h"
#include "runtime/mark_handle.h"

#if !defined(__mc68000__)
#pragma pack(push,16)
#endif
#include <setjmp.h>
#if !defined(__mc68000__)
#pragma pack(pop)
#endif

namespace amos::host {

// A point of the stack to come back to: Run its target there (the stack
// above it given up), or Return the registers to the code that made it
#if !defined(__mc68000__)
#pragma pack(push,16)
struct alignas(16) Mark {
#else
struct Mark {
#endif
    jmp_buf jb;
#if defined(__mc68000__)
    const void *target;
    const void *then;
#else
    RoutineEntry target;
    RoutineEntry then;
#endif
    void (*fn)();               // or this C++ (the context its registers)
    void (*regs_fn)(AllRegs &); // or this C++, with all the registers
    AllRegs regs;
#if !defined(__mc68000__)
    uint64_t context_watermark;
    uint32_t recovery_token = 0; // Native-only registry metadata, never an address.
#endif
};

#if !defined(__mc68000__)
#pragma pack(pop)
static_assert(alignof(Mark) >= 16 && offsetof(Mark, jb) == 0);
static_assert(offsetof(Mark, context_watermark) % alignof(uint64_t) == 0);
#endif

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
#if !defined(__mc68000__)
    mark_enter(m);
#endif
    switch (setjmp(m.jb)) {
    case 0:
        first();
        break;
    case MarkRun:
        run_at(m);
        break;
    default:
        break;
    }
    const AllRegs result = m.regs;
#if !defined(__mc68000__)
    mark_leave(m);
#endif
    return result;
}


} // namespace amos::host
