// Opaque recovery tokens. Architectural words decode only at this ABI boundary.
#pragma once
#include "core/amos.h"
#include "core/mark_token.h"
namespace amos::host {
struct Mark;
#if defined(__mc68000__)
inline uint32_t mark_word(Mark& mark) { return address(&mark); }
#else
uint32_t mark_word(Mark&);
void mark_enter(Mark&);
void mark_leave(Mark&);
// Null for an inactive/stale/wrong-task token; never dereferences guest memory.
Mark* mark_lookup(MarkHandle);
// Discard active marks above the destination before the native stack unwinds.
Mark& mark_unwind(MarkHandle);
#endif
inline MarkHandle mark_token(Mark& mark) { return decode_mark(mark_word(mark)); }
[[noreturn]] void mark_jump(MarkHandle, const void* target, const AllRegs&, const void* then = nullptr);
[[noreturn]] void mark_jump_regs(MarkHandle, void (*)(AllRegs&), const AllRegs&);
[[noreturn]] void mark_jump_cpp(MarkHandle, void (*)(), const AllRegs&);
[[noreturn]] void mark_return(MarkHandle, const AllRegs&);
}
