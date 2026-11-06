// Explicit temporary capture from legacy native data cursors to guest values.
// Remove this boundary when live Ctx/register data addresses are migrated.
#pragma once
#include "runtime/guest_context.h"
namespace amos::host::detail {
#if defined(__mc68000__)
inline const GuestContext& capture_context(const Ctx& context) { return context; }
#else
inline guest::Result<GuestContext> try_capture_context(const Ctx& context) {
    const uint8_t* native[] = {context.a3,context.a4,context.a6};
    for (auto pointer : native)
        if (reinterpret_cast<uintptr_t>(pointer) > UINT32_MAX) return {{},guest::Fault::overflow};
    return {{context.d6, context.d7,
             guest::abi::AddressCodec::decode<guest::Byte>(uint32_t(reinterpret_cast<uintptr_t>(context.a3))),
             guest::abi::AddressCodec::decode<guest::Byte>(uint32_t(reinterpret_cast<uintptr_t>(context.a4))),
             context.a5,
             guest::abi::AddressCodec::decode<guest::Byte>(uint32_t(reinterpret_cast<uintptr_t>(context.a6)))},guest::Fault::none};
}
inline GuestContext capture_context(const Ctx& context) {
    auto snapshot = try_capture_context(context);
    if (!snapshot) guest::pointer_fault(snapshot.fault);
    return snapshot.value;
}
#endif
}
