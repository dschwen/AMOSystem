#pragma once
#include "core/guest_symbol.h"
#pragma pack(push,8)
namespace amos::guest {
namespace detail { struct IdentitySegments; }
class alignas(8) SegmentBindings {
    struct Mapping { GuestSpan<Byte> extent{}; bool present = false; } mappings_[128]{};
    friend struct detail::IdentitySegments;
    // Trusted transition adapter for current low-address production backing.
    // Unlike truncation, this rejects every native extent above the guest range.
    Fault bind_identity(SegmentId, HostSpan<Byte>);
public:
    Fault bind(SegmentId, GuestSpan<Byte>);
    Result<GuestSpan<Byte>> extent(SegmentId) const;
    template<class T> Result<GuestPtr<T>> resolve(GuestSymbol<T> symbol) const {
        auto mapping = extent(symbol.segment());
        if (!mapping) return {{}, mapping.fault};
        if (symbol.byte_offset() > mapping.value.size()) return {{}, Fault::invalid_range};
        auto next = mapping.value.data().try_offset_bytes(GuestByteDelta(symbol.byte_offset()));
        if (!next) return {{}, next.fault};
        // Symbol typing never changes the numerical guest address. Actual
        // object extent, alignment and permissions are checked by Memory views.
        return {GuestPtr<T>(next.value.bits_), Fault::none};
    }
    template<class Signature> Result<GuestEntry<Signature>> resolve(EntrySymbol<Signature> symbol) const {
        auto mapping = extent(symbol.segment());
        if (!mapping) return {{}, mapping.fault};
        if (symbol.byte_offset() > mapping.value.size() || mapping.value.size() - symbol.byte_offset() < 4)
            return {{}, Fault::invalid_range};
        auto data = resolve(GuestSymbol<Byte>(symbol.segment(), symbol.byte_offset()));
        if (!data) return {{}, data.fault};
        return {GuestEntry<Signature>(data.value.address()), Fault::none};
    }
    Result<int64_t> difference(GuestSymbol<const Byte>, GuestSymbol<const Byte>) const;
};
}
#pragma pack(pop)
