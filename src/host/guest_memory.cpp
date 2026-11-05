// Trusted native mapping boundary: compile without guest structure packing.
#include "memory/guest_memory.h"
#include <string.h>
#include <stdlib.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif
namespace amos::guest {
namespace { constexpr uint64_t window_size = uint64_t(1) << 32; }
[[noreturn]] void pointer_fault(Fault) { abort(); }

Memory::~Memory() {
    if (!base_) return;
#if defined(_WIN32)
    VirtualFree(base_, 0, MEM_RELEASE);
#else
    munmap(base_, size_t(window_size));
#endif
}
Fault Memory::reserve() {
    if (base_) return Fault::invalid_range;
    if (sizeof(size_t) < 8) return Fault::platform;
#if defined(_WIN32)
    SYSTEM_INFO info; GetSystemInfo(&info); page_size_ = info.dwPageSize;
    void* native = VirtualAlloc(nullptr, size_t(window_size), MEM_RESERVE, PAGE_NOACCESS);
#else
    const long page = sysconf(_SC_PAGESIZE);
    if (page <= 0) return Fault::platform;
    page_size_ = uint64_t(page);
    void* native = mmap(nullptr, size_t(window_size), PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (native == MAP_FAILED) return Fault::platform;
#endif
    if (!native) return Fault::platform;
    // Also proves this testbed cannot accidentally depend on a low native base.
    if (reinterpret_cast<uintptr_t>(native) <= UINT32_MAX ||
        reinterpret_cast<uintptr_t>(native) > UINTPTR_MAX - window_size) {
#if defined(_WIN32)
        VirtualFree(native, 0, MEM_RELEASE);
#else
        munmap(native, size_t(window_size));
#endif
        return Fault::platform;
    }
    base_ = static_cast<Byte*>(native);
    return Fault::none;
}
Fault Memory::map_ram(GuestAddress32 address, uint64_t count, Access access) {
    const uint32_t start = address.bits_;
    if (!base_ || !start || !count || count > window_size - start) return Fault::invalid_range;
    const uint64_t end = uint64_t(start) + count;
    for (unsigned i = 0; i < region_count_; ++i)
        if (start < regions_[i].end && end > regions_[i].start) return Fault::invalid_range;
    if (region_count_ == 128) return Fault::exhausted;
    const uint64_t page_start = start - start % page_size_;
    const uint64_t page_end = ((end - 1) / page_size_ + 1) * page_size_;
#if defined(_WIN32)
    if (!VirtualAlloc(base_ + page_start, size_t(page_end - page_start), MEM_COMMIT, PAGE_READWRITE))
        return Fault::platform;
#else
    if (mprotect(base_ + page_start, size_t(page_end - page_start), PROT_READ | PROT_WRITE))
        return Fault::platform;
#endif
    regions_[region_count_++] = {start, end, access};
    return Fault::none;
}
Fault Memory::check(uint32_t start, uint64_t count, bool write) const {
    if (!base_) return Fault::unmapped;
    if (!count) return Fault::none;
    if (!start) return Fault::null_pointer;
    if (count > window_size - start) return Fault::overflow;
    const uint64_t end = uint64_t(start) + count;
    uint64_t cursor = start;
    while (cursor < end) {
        bool found = false;
        for (unsigned i = 0; i < region_count_; ++i) {
            const auto& r = regions_[i];
            if (cursor >= r.start && cursor < r.end) {
                if (write && r.access != Access::read_write) return Fault::permission;
                cursor = r.end < end ? r.end : end;
                found = true;
                break;
            }
        }
        if (!found) return Fault::unmapped;
    }
    return Fault::none;
}
Fault Memory::copy_from_host(GuestSpan<Byte> destination, HostSpan<const Byte> source) {
    if (destination.size() != source.size()) return Fault::invalid_range;
    auto fault = check(destination.data().bits_, destination.size(), true);
    if (fault != Fault::none) return fault;
    if (!destination.size()) return Fault::none;
    if (!source.start_.value_) return Fault::null_pointer;
    memcpy(base_ + destination.data().bits_, source.start_.value_, size_t(destination.size()));
    return Fault::none;
}
Fault Memory::copy_to_host(HostSpan<Byte> destination, GuestSpan<const Byte> source) const {
    if (destination.size() != source.size()) return Fault::invalid_range;
    auto fault = check(source.data().bits_, source.size(), false);
    if (fault != Fault::none) return fault;
    if (!source.size()) return Fault::none;
    if (!destination.start_.value_) return Fault::null_pointer;
    memcpy(destination.start_.value_, base_ + source.data().bits_, size_t(source.size()));
    return Fault::none;
}
Result<Byte> Memory::load_byte(GuestPtr<const Byte> address) const {
    const auto f = check(address.bits_, 1, false);
    return f == Fault::none ? Result<Byte>{base_[address.bits_], f} : Result<Byte>{{}, f};
}
Fault Memory::store_byte(GuestPtr<Byte> address, Byte value) {
    const auto f = check(address.bits_, 1, true);
    if (f == Fault::none) base_[address.bits_] = value;
    return f;
}
Result<uint32_t> Memory::load_be32(GuestPtr<const Byte> address) const {
    const auto f = check(address.bits_, 4, false);
    if (f != Fault::none) return {{}, f};
    const Byte* p = base_ + address.bits_;
    return {(uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | p[3], f};
}
Fault Memory::store_be32(GuestPtr<Byte> address, uint32_t value) {
    const auto f = check(address.bits_, 4, true);
    if (f != Fault::none) return f;
    Byte* p = base_ + address.bits_;
    p[0] = Byte(value >> 24); p[1] = Byte(value >> 16); p[2] = Byte(value >> 8); p[3] = Byte(value);
    return Fault::none;
}
}
