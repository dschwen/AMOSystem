// Finite adapter for identity-model callers. No general native/guest conversion.
#pragma once
#include "runtime/glue.h"
#include "abi/guest_entry_codec.h"
namespace amos::host::detail {
inline RoutineEntry legacy_routine_entry(const void* at) {
    const auto word = reinterpret_cast<uintptr_t>(at);
    if (word > UINT32_MAX) guest::pointer_fault(guest::Fault::overflow);
    return guest::abi::EntryCodec::decode<RoutineSignature>(uint32_t(word));
}
}

namespace amos::host {
#if defined(__mc68000__)
inline const GlueEntry* legacy_glue_entry(const void* at) { return glue_entry(at); }
inline int32_t legacy_glue_call(const void* at, AllRegs& r) { return glue_call(at,r); }
#else
const GlueEntry* legacy_glue_entry(const void* at);
int32_t legacy_glue_call(const void* at, AllRegs& r);
#endif
}
