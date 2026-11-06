// Temporary bootstrap privilege. Ordinary guest code must not include this.
#pragma once
#include "memory/guest_segments.h"
namespace amos::guest::detail {
struct IdentitySegments {
    static Fault bind(SegmentBindings& bindings, SegmentId id, HostSpan<Byte> source) {
        return bindings.bind_identity(id, source);
    }
};
}
