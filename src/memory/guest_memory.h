// Isolated translated-memory substrate. Not yet the legacy interpreter allocator.
#pragma once
#include "core/guest_types.h"
namespace amos::guest { enum class Access : uint8_t { read, read_write }; }
#if defined(__mc68000__)
#include "memory/detail/amiga_memory.h"
#else
#pragma pack(push, 8)
namespace amos::guest {
class alignas(8) Memory {
    struct Region { uint32_t start; uint64_t end; Access access; };
    Byte* base_ = nullptr;
    uint64_t page_size_ = 0;
    Region regions_[128]{};
    unsigned region_count_ = 0;
    Fault check(uint32_t start, uint64_t count, bool write) const;
public:
    Memory() = default;
    ~Memory();
    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;
    // Reserve a 4GiB window above 4GiB; no public native address accessor.
    Fault reserve();
    // Logical region bounds are exact, independently of host VM page size.
    Fault map_ram(GuestAddress32 start, uint64_t count, Access access = Access::read_write);
    Fault copy_from_host(GuestSpan<Byte> destination, HostSpan<const Byte> source);
    Fault copy_to_host(HostSpan<Byte> destination, GuestSpan<const Byte> source) const;
    Result<Byte> load_byte(GuestPtr<const Byte>) const;
    Fault store_byte(GuestPtr<Byte>, Byte);
    Result<uint16_t> load_be16(GuestPtr<const Byte>) const;
    Fault store_be16(GuestPtr<Byte>, uint16_t);
    Result<uint32_t> load_be32(GuestPtr<const Byte>) const;
    Fault store_be32(GuestPtr<Byte>, uint32_t);
    template<class T> requires Sized<T>
    Result<GuestRef<T>> ref(GuestPtr<T> address);
    template<class T> Result<GuestPtr<T>> load_pointer(GuestPtr<const PointerField<T>> address) const {
        auto word = load_be32(GuestPtr<const Byte>(address.bits_));
        if (!word) return {{}, word.fault};
        return {GuestPtr<T>(word.value), Fault::none};
    }
    template<class T> Fault store_pointer(GuestPtr<PointerField<T>> address, GuestPtr<T> value) {
        return store_be32(GuestPtr<Byte>(address.bits_), value.bits_);
    }
    uint64_t host_page_size() const { return page_size_; }
};
}
#pragma pack(pop)

#endif
