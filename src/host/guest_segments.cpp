#include "memory/guest_segments.h"
#include "abi/guest_address_codec.h"
namespace amos::guest {
Fault SegmentBindings::bind(SegmentId id, GuestSpan<Byte> extent) {
    const auto index = id.index();
    if (!index || index >= 128 || mappings_[index].present || !extent.data()) return Fault::invalid_range;
    const uint64_t start = abi::AddressCodec::encode(extent.data().address());
    if (extent.size() > (uint64_t(1) << 32) - start) return Fault::overflow;
    const uint64_t end = start + extent.size();
    for (const auto& other : mappings_) {
        if (!other.present) continue;
        const uint64_t at = abi::AddressCodec::encode(other.extent.data().address());
        if (start < at + other.extent.size() && at < end) return Fault::invalid_range;
    }
    mappings_[index] = {extent, true};
    return Fault::none;
}
Fault SegmentBindings::bind_identity(SegmentId id, HostSpan<Byte> source) {
    const auto native = reinterpret_cast<uintptr_t>(source.start_.value_);
    if (!native) return Fault::null_pointer;
    if (native > UINT32_MAX || source.size() > (uint64_t(1) << 32) - native) return Fault::overflow;
    return bind(id, GuestSpan<Byte>(abi::AddressCodec::decode<Byte>(uint32_t(native)), source.size()));
}
Result<GuestSpan<Byte>> SegmentBindings::extent(SegmentId id) const {
    const auto index = id.index();
    if (!index || index >= 128 || !mappings_[index].present) return {{}, Fault::unmapped};
    return {mappings_[index].extent, Fault::none};
}
Result<int64_t> SegmentBindings::difference(GuestSymbol<const Byte> left, GuestSymbol<const Byte> right) const {
    auto l = resolve(left); if (!l) return {{}, l.fault};
    auto r = resolve(right); if (!r) return {{}, r.fault};
    auto distance = l.value.try_distance(r.value);
    if (!distance) return {{}, distance.fault};
    return {distance.value.elements(), Fault::none};
}
}
