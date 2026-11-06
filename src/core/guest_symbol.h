// Link-time guest symbols. They contain segment-relative offsets, not pointers.
#pragma once
#include "core/guest_types.h"
#pragma pack(push,8)
namespace amos::guest {
template<Integer N> constexpr uint32_t symbol_offset(N offset) {
    auto value = signed_offset(offset);
    const int64_t number = value.value;
    if (!value || number < 0 || number > UINT32_MAX) pointer_fault(Fault::overflow);
    return uint32_t(number);
}
class SegmentId {
    uint16_t index_;
public:
    constexpr SegmentId() = default;
    template<Integer N> explicit constexpr SegmentId(N index) : index_(0) {
        const auto value = symbol_offset(index);
        if (value > UINT16_MAX) pointer_fault(Fault::overflow);
        index_ = uint16_t(value);
    }
    constexpr uint16_t index() const { return index_; }
    friend constexpr bool operator==(SegmentId, SegmentId) = default;
};
template<class T> class GuestSymbol {
    SegmentId segment_;
    uint32_t offset_;
    template<class> friend class GuestSymbol;
public:
    constexpr GuestSymbol() = default;
    template<Integer N> explicit constexpr GuestSymbol(SegmentId segment, N offset) : segment_(segment), offset_(symbol_offset(offset)) {}
    template<class U> requires const_compatible<U, T>
    constexpr GuestSymbol(GuestSymbol<U> value) : segment_(value.segment_), offset_(value.offset_) {}
    constexpr SegmentId segment() const { return segment_; }
    constexpr uint32_t byte_offset() const { return offset_; }
    constexpr Result<GuestSymbol> try_offset(GuestByteDelta delta) const {
        const int64_t count = delta.bytes();
        if (count < -int64_t(offset_) || count > int64_t(UINT32_MAX) - offset_)
            return {{}, Fault::overflow};
        return {GuestSymbol(segment_, uint32_t(int64_t(offset_) + count)), Fault::none};
    }
};
class SegmentBindings;
template<class Signature> class EntrySymbol {
    SegmentId segment_;
    uint32_t offset_;
public:
    constexpr EntrySymbol() = default;
    template<Integer N> explicit constexpr EntrySymbol(SegmentId segment, N offset) : segment_(segment), offset_(symbol_offset(offset)) {}
    constexpr SegmentId segment() const { return segment_; }
    constexpr uint32_t byte_offset() const { return offset_; }
};
template<class Signature> class alignas(pointer_alignment) GuestEntry {
    GuestAddress32 address_;
    explicit constexpr GuestEntry(GuestAddress32 address) : address_(address) {}
    friend class SegmentBindings;
public:
    constexpr GuestEntry() = default;
    explicit constexpr operator bool() const { return bool(address_); }
    constexpr GuestAddress32 address() const { return address_; }
    friend constexpr bool operator==(GuestEntry, GuestEntry) = default;
};
static_assert(sizeof(GuestEntry<void()>) == 4);
}
#pragma pack(pop)
