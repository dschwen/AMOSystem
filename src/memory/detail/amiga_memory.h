// Trusted Amiga identity adapter. No arena, native base, region lookup or VM.
// Ordinary operations have the same validity/alignment/lifetime preconditions
// as the original native accesses. Explicit try_* pointer checks remain opt-in.
#pragma once
#include "core/guest_types.h"
namespace amos::guest {
class Memory {
public:
    template<class T> requires Sized<T> Result<GuestRef<T>> ref(GuestPtr<T> address);
    Result<Byte> load_byte(GuestPtr<const Byte> address) const {
        return {*reinterpret_cast<const Byte*>(address.bits_), Fault::none};
    }
    Fault store_byte(GuestPtr<Byte> address, Byte value) {
        *reinterpret_cast<Byte*>(address.bits_) = value;
        return Fault::none;
    }
    Result<uint32_t> load_be32(GuestPtr<const Byte> address) const {
        return {*reinterpret_cast<const uint32_t*>(address.bits_), Fault::none};
    }
    Fault store_be32(GuestPtr<Byte> address, uint32_t value) {
        *reinterpret_cast<uint32_t*>(address.bits_) = value;
        return Fault::none;
    }
    template<class T> Result<GuestPtr<T>> load_pointer(GuestPtr<const PointerField<T>> address) const {
        return {GuestPtr<T>(*reinterpret_cast<const uint32_t*>(address.bits_)), Fault::none};
    }
    template<class T> Fault store_pointer(GuestPtr<PointerField<T>> address, GuestPtr<T> value) {
        *reinterpret_cast<uint32_t*>(address.bits_) = value.bits_;
        return Fault::none;
    }
    Fault copy_from_host(GuestSpan<Byte> destination, HostSpan<const Byte> source) {
        __builtin_memcpy(reinterpret_cast<void*>(destination.data().bits_),
                         source.start_.value_, size_t(destination.size()));
        return Fault::none;
    }
    Fault copy_to_host(HostSpan<Byte> destination, GuestSpan<const Byte> source) const {
        __builtin_memcpy(destination.start_.value_, reinterpret_cast<const void*>(source.data().bits_),
                         size_t(source.size()));
        return Fault::none;
    }
    // Physical Amiga memory is managed by Exec, never by a translated VM arena.
    Fault reserve() = delete;
    Fault map_ram(GuestAddress32, uint64_t, Access = Access::read_write) = delete;
};
static_assert(sizeof(GuestPtr<Byte>) == sizeof(Byte*));
static_assert(alignof(GuestPtr<Byte>) == alignof(Byte*));
static_assert(sizeof(HostPtr<Byte>) == sizeof(Byte*));
static_assert(alignof(HostPtr<Byte>) == alignof(Byte*));
static_assert(sizeof(GuestByteDelta) == sizeof(ptrdiff_t));
static_assert(sizeof(GuestDistance<Byte>) == sizeof(ptrdiff_t));
}
