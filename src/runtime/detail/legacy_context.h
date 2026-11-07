// Explicit temporary capture from legacy native data cursors to guest values.
// Remove this boundary when live Ctx/register data addresses are migrated.
#pragma once
#include "runtime/guest_context.h"
#include "core/guest_bias.h"
namespace amos::host::detail {
#if defined(__mc68000__)
inline const GuestContext& capture_context(const Ctx& context) { return context; }
#else
inline guest::Result<GuestContext> try_capture_context(const Ctx& context) {
    const uint8_t* native[] = {context.a6};
    for (auto pointer : native)
        if (!bias::contains(reinterpret_cast<uintptr_t>(pointer))) return {{},guest::Fault::overflow};
    return {{context.d6, context.d7,
             context.a3,
             context.a4,
             context.a5,
             guest::abi::AddressCodec::decode<guest::Byte>(bias::to_guest(reinterpret_cast<uintptr_t>(context.a6)))},guest::Fault::none};
}
inline GuestContext capture_context(const Ctx& context) {
    auto snapshot = try_capture_context(context);
    if (!snapshot) guest::pointer_fault(snapshot.fault);
    return snapshot.value;
}
#endif
}
