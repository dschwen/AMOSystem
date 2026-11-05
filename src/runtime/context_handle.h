#pragma once
#include "core/amos.h"
#include "core/context_token.h"
namespace amos::host {
#if defined(__mc68000__)
inline ContextHandle save_context(const Ctx& context) { return decode_context(address(&context)); }
inline const Ctx& saved_context(ContextHandle handle) { return *ptr<const Ctx>(encode_context(handle)); }
inline void release_context(ContextHandle) {}
#else
ContextHandle save_context(const Ctx&);
const Ctx& saved_context(ContextHandle);
void release_context(ContextHandle);
uint64_t context_checkpoint();
void context_unwind(uint64_t checkpoint);
bool context_valid(ContextHandle);
#endif
}
