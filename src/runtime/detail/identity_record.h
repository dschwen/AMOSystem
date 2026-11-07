// Temporary identity backing for legacy guest records. Only this boundary creates
// native record references; replace it with schema views before high backing.
#pragma once
#include "core/context_data.h"
#include "core/guest_bias.h"
namespace amos::host::detail {
#if defined(__mc68000__)
inline ContextCursor capture_cursor(uint8_t* native) { return native; }
#else
inline guest::Result<ContextCursor> try_capture_cursor(uint8_t* native) {
#if defined(AMOS_GUEST_BIASED)
    return {decode_context_cursor(bias::to_guest(reinterpret_cast<uintptr_t>(native))), guest::Fault::none};
#endif
    const auto word = reinterpret_cast<uintptr_t>(native);
    if (word > UINT32_MAX) return {{}, guest::Fault::overflow};
    return {decode_context_cursor(uint32_t(word)), guest::Fault::none};
}
inline ContextCursor capture_cursor(uint8_t* native) {
    auto address = try_capture_cursor(native);
    if (!address) guest::pointer_fault(address.fault);
    return address.value;
}
#endif
template<class Record> Record& identity_record(ContextCursor address) {
#if defined(__mc68000__)
    return *reinterpret_cast<Record*>(address);
#else
    const auto word = encode_context_cursor(address);
    if (!word) guest::pointer_fault(guest::Fault::null_pointer);
    if (sizeof(Record) > (uint64_t(1) << 32) - word) guest::pointer_fault(guest::Fault::overflow);
#if defined(AMOS_GUEST_BIASED)
    return *reinterpret_cast<Record*>(bias::to_native(word));
#endif
    return *reinterpret_cast<Record*>(uintptr_t(word));
#endif
}
}
