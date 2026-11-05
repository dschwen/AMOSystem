// Native metadata only; allocation decisions use logical guest addresses.
#include "memory/guest_allocator.h"
#include "abi/guest_address_codec.h"
#include <stdlib.h>
#include <string.h>
namespace amos::guest {
namespace {
uint64_t word(GuestPtr<const Byte> pointer) { return abi::AddressCodec::encode(pointer.address()); }
GuestPtr<Byte> pointer(uint64_t word) { return abi::AddressCodec::decode<Byte>(uint32_t(word)); }
uint64_t rounded(uint32_t size) { return (uint64_t(size) + 7) & ~uint64_t(7); }
constexpr uint64_t limit = uint64_t(1) << 32;
}
RangeAllocator::~RangeAllocator() { free(ranges_); }
Fault RangeAllocator::capacity(uint32_t count) {
    if (count <= capacity_) return Fault::none;
    if (count > (1u << 20)) return Fault::exhausted;
    const auto next = capacity_ ? capacity_ * 2 : 16;
    auto* resized = static_cast<Range*>(realloc(ranges_, sizeof(Range) * next));
    if (!resized) return Fault::exhausted;
    ranges_ = resized; capacity_ = next;
    return Fault::none;
}
Fault RangeAllocator::initialize(GuestSpan<Byte> extent) {
    if (extent_.data() || !extent.data() || !extent.size() || extent.size() > limit - word(extent.data()))
        return Fault::invalid_range;
    const auto fault = capacity(1);
    if (fault != Fault::none) return fault;
    extent_ = extent; ranges_[0] = {extent.data(), extent.size()}; count_ = 1;
    return Fault::none;
}
bool RangeAllocator::contains(GuestPtr<const Byte> address) const {
    const auto at = word(address), start = word(extent_.data());
    return extent_.data() && at >= start && at - start < extent_.size();
}
uint64_t RangeAllocator::free_bytes() const {
    uint64_t total = 0;
    for (uint32_t i = 0; i < count_; ++i) total += ranges_[i].size;
    return total;
}
Result<GuestSpan<Byte>> RangeAllocator::allocate(uint32_t size) {
    if (!size || !extent_.data()) return {{}, Fault::invalid_range};
    const auto count = rounded(size);
    for (uint32_t i = 0; i < count_; ++i) {
        auto& range = ranges_[i];
        if (range.size < count) continue;
        const auto start = range.at;
        if (range.size == count) {
            memmove(ranges_ + i, ranges_ + i + 1, sizeof(Range) * (count_ - i - 1));
            --count_;
        } else {
            range.at = pointer(word(range.at) + count); range.size -= count;
        }
        return {GuestSpan<Byte>(start, count), Fault::none};
    }
    return {{}, Fault::exhausted};
}
Result<GuestSpan<Byte>> RangeAllocator::allocate_at(uint32_t size, GuestPtr<Byte> address) {
    if (!size || !contains(address)) return {{}, Fault::invalid_range};
    const auto start = word(address), count = rounded(size);
    if (count > limit - start) return {{}, Fault::overflow};
    for (uint32_t i = 0; i < count_; ++i) {
        const auto at = word(ranges_[i].at), length = ranges_[i].size;
        if (start < at || start - at > length || count > length - (start - at)) continue;
        const auto before = start - at, after = length - before - count;
        if (before && after) {
            const auto fault = capacity(count_ + 1);
            if (fault != Fault::none) return {{}, fault};
            memmove(ranges_ + i + 2, ranges_ + i + 1, sizeof(Range) * (count_ - i - 1));
            ranges_[i].size = before;
            ranges_[i + 1] = {pointer(start + count), after}; ++count_;
        } else if (before) ranges_[i].size = before;
        else if (after) ranges_[i] = {pointer(start + count), after};
        else {
            memmove(ranges_ + i, ranges_ + i + 1, sizeof(Range) * (count_ - i - 1)); --count_;
        }
        return {GuestSpan<Byte>(address, count), Fault::none};
    }
    return {{}, Fault::exhausted};
}
Fault RangeAllocator::release(GuestPtr<Byte> address, uint32_t size) {
    if (!size || !contains(address)) return Fault::invalid_range;
    const auto start = word(address), count = rounded(size), end = start + count;
    const auto extent_end = word(extent_.data()) + extent_.size();
    if (count > limit - start || end > extent_end) return Fault::overflow;
    uint32_t i = 0;
    while (i < count_ && word(ranges_[i].at) < start) ++i;
    if (i && word(ranges_[i - 1].at) + ranges_[i - 1].size > start) return Fault::invalid_range;
    if (i < count_ && end > word(ranges_[i].at)) return Fault::invalid_range;
    const bool before = i && word(ranges_[i - 1].at) + ranges_[i - 1].size == start;
    const bool after = i < count_ && end == word(ranges_[i].at);
    if (before && after) {
        ranges_[i - 1].size += count + ranges_[i].size;
        memmove(ranges_ + i, ranges_ + i + 1, sizeof(Range) * (count_ - i - 1)); --count_;
    } else if (before) ranges_[i - 1].size += count;
    else if (after) { ranges_[i].at = address; ranges_[i].size += count; }
    else {
        const auto fault = capacity(count_ + 1);
        if (fault != Fault::none) return fault;
        memmove(ranges_ + i + 1, ranges_ + i, sizeof(Range) * (count_ - i));
        ranges_[i] = {address, count}; ++count_;
    }
    return Fault::none;
}
}
