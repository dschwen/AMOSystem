// The captured saved values of a context: the live registers are guest
// addresses already, so the snapshot is a copy.
#pragma once
#include "runtime/guest_context.h"
namespace amos::host::detail {
#if defined(__mc68000__)
inline const GuestContext& capture_context(const Ctx& context) { return context; }
#else
inline GuestContext capture_context(const Ctx& context)
{
    return {context.d6, context.d7, context.a3, context.a4, context.a5, context.a6};
}
#endif
}
