// Guest-only free-list allocator. It never stores or returns a native pointer.
#pragma once
#include "core/guest_types.h"
#pragma pack(push, 8)
namespace amos::guest {
class alignas(8) RangeAllocator {
    struct Range { GuestPtr<Byte> at; uint64_t size; };
    Range* ranges_ = nullptr; // Native metadata, independent of backing RAM.
    uint32_t count_ = 0, capacity_ = 0;
    GuestSpan<Byte> extent_{};
    Fault capacity(uint32_t count);
public:
    RangeAllocator() = default;
    ~RangeAllocator();
    RangeAllocator(const RangeAllocator&) = delete;
    RangeAllocator& operator=(const RangeAllocator&) = delete;
    Fault initialize(GuestSpan<Byte>);
    Result<GuestSpan<Byte>> allocate(uint32_t size);
    Result<GuestSpan<Byte>> allocate_at(uint32_t size, GuestPtr<Byte>);
    Fault release(GuestPtr<Byte>, uint32_t size);
    bool contains(GuestPtr<const Byte>) const;
    uint64_t free_bytes() const;
};
}
#pragma pack(pop)
