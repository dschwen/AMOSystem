// Compile twice with identical C entry signatures. The wrapper build must have
// identical .text bytes and relocations to the raw-pointer baseline.
#include <stdint.h>
#include <stddef.h>
inline void* operator new(size_t, void* storage) noexcept { return storage; }
#if defined(AMOS_TEST_WRAPPED)
#include "abi/guest_address_codec.h"
#include "core/mark_token.h"
#include "core/context_token.h"
#include "memory/guest_view.h"
using namespace amos::guest;
using abi::AddressCodec;
struct Node;
template<> struct amos::guest::Layout<Node> { static constexpr uint32_t size = 8; };
using Next = Field<Node, PointerField<Node>, 4>;
static_assert(sizeof(GuestRef<Node>) == sizeof(void*));
static_assert(alignof(GuestRef<Node>) == alignof(void*));
#endif
extern "C" {
uint32_t byte_add(uint32_t base, int32_t offset) {
#if defined(AMOS_TEST_WRAPPED)
    return AddressCodec::encode((AddressCodec::decode<Byte>(base) + offset).address());
#else
    return uint32_t(reinterpret_cast<uint8_t*>(base) + offset);
#endif
}
uint32_t word_subtract(uint32_t base, int32_t offset) {
#if defined(AMOS_TEST_WRAPPED)
    return AddressCodec::encode((AddressCodec::decode<uint32_t>(base) - offset).address());
#else
    return uint32_t(reinterpret_cast<uint32_t*>(base) - offset);
#endif
}
int32_t word_distance(uint32_t end, uint32_t start) {
#if defined(AMOS_TEST_WRAPPED)
    return (AddressCodec::decode<uint32_t>(end) - AddressCodec::decode<uint32_t>(start)).elements();
#else
    return reinterpret_cast<uint32_t*>(end) - reinterpret_cast<uint32_t*>(start);
#endif
}
uint32_t byte_offset(uint32_t base, int32_t offset) {
#if defined(AMOS_TEST_WRAPPED)
    return AddressCodec::encode(AddressCodec::decode<uint32_t>(base).offset_bytes(GuestByteDelta(offset)).address());
#else
    return uint32_t(reinterpret_cast<uint8_t*>(base) + offset);
#endif
}
bool pointer_less(uint32_t a, uint32_t b) {
#if defined(AMOS_TEST_WRAPPED)
    return AddressCodec::decode<Byte>(a) < AddressCodec::decode<Byte>(b);
#else
    return reinterpret_cast<uint8_t*>(a) < reinterpret_cast<uint8_t*>(b);
#endif
}
bool host_nonnull(uint32_t base) {
#if defined(AMOS_TEST_WRAPPED)
    return bool(HostPtr<Byte>(reinterpret_cast<Byte*>(base)));
#else
    return reinterpret_cast<uint8_t*>(base) != nullptr;
#endif
}
uint32_t load_long(uint32_t base) {
#if defined(AMOS_TEST_WRAPPED)
    return Memory{}.load_be32(AddressCodec::decode<Byte>(base)).value;
#else
    return *reinterpret_cast<const uint32_t*>(base);
#endif
}
void store_long(uint32_t base, uint32_t value) {
#if defined(AMOS_TEST_WRAPPED)
    Memory{}.store_be32(AddressCodec::decode<Byte>(base), value);
#else
    *reinterpret_cast<uint32_t*>(base) = value;
#endif
}
uint8_t load_bank_byte(uint32_t base, int32_t offset) {
#if defined(AMOS_TEST_WRAPPED)
    return Memory{}.load_byte(AddressCodec::decode<Byte>(base) + offset).value;
#else
    return reinterpret_cast<const uint8_t*>(base)[offset];
#endif
}
uint32_t load_next(uint32_t base) {
#if defined(AMOS_TEST_WRAPPED)
    auto next = Memory{}.ref(AddressCodec::decode<Node>(base)).value.field<Next>().value.load().value;
    return AddressCodec::encode(next.address());
#else
    return reinterpret_cast<const uint32_t*>(base)[1];
#endif
}
void store_next(uint32_t base, uint32_t next) {
#if defined(AMOS_TEST_WRAPPED)
    Memory{}.ref(AddressCodec::decode<Node>(base)).value.field<Next>().value.store(AddressCodec::decode<Node>(next));
#else
    reinterpret_cast<uint32_t*>(base)[1] = next;
#endif
}
uint32_t load_wire(const void* cell) {
#if defined(AMOS_TEST_WRAPPED)
    return AddressCodec::encode(static_cast<const PointerField<Byte>*>(cell)->load().address());
#else
    return *static_cast<const uint32_t*>(cell);
#endif
}
void store_wire(void* cell, uint32_t value) {
#if defined(AMOS_TEST_WRAPPED)
    static_cast<PointerField<Byte>*>(cell)->store(AddressCodec::decode<Byte>(value));
#else
    *static_cast<uint32_t*>(cell) = value;
#endif
}
void copy_to_guest(uint32_t destination, const uint8_t* source, uint32_t count) {
#if defined(AMOS_TEST_WRAPPED)
    Memory{}.copy_from_host(GuestSpan<Byte>(AddressCodec::decode<Byte>(destination), count), HostSpan<const Byte>(source, count));
#else
    __builtin_memcpy(reinterpret_cast<void*>(destination), source, count);
#endif
}
}

// Test real function boundaries, not just wrappers optimized inside a veneer.
extern "C" {
#if defined(AMOS_TEST_WRAPPED)
GuestPtr<Byte> advance_guest(GuestPtr<Byte> p, int32_t n) { return p + n; }
HostPtr<Byte> forward_host(HostPtr<Byte> p) { return p; }
GuestPtr<Byte> opaque_guest(GuestPtr<Byte>, int32_t);
#else
uint8_t* advance_guest(uint8_t* p, int32_t n) { return p + n; }
uint8_t* forward_host(uint8_t* p) { return p; }
uint8_t* opaque_guest(uint8_t*, int32_t);
#endif
#if defined(AMOS_TEST_WRAPPED)
GuestPtr<Byte> call_opaque(GuestPtr<Byte> base, int32_t offset) {
#else
uint8_t* call_opaque(uint8_t* base, int32_t offset) {
#endif
    return opaque_guest(base, offset);
}

}

extern "C" uint32_t slice_bank(uint32_t base, uint32_t size, uint32_t offset, uint32_t count) {
#if defined(AMOS_TEST_WRAPPED)
    auto slice = GuestSpan<Byte>(AddressCodec::decode<Byte>(base), size).subspan(offset, count).value;
    return AddressCodec::encode(slice.data().address());
#else
    (void)size; (void)count;
    return uint32_t(reinterpret_cast<uint8_t*>(base) + offset);
#endif
}

extern "C" uint32_t load_recovery(const void* cell) {
#if defined(AMOS_TEST_WRAPPED)
    return amos::host::encode_mark(static_cast<const amos::wire::MarkField*>(cell)->load());
#else
    return *static_cast<const uint32_t*>(cell);
#endif
}
extern "C" void store_recovery(void* cell, uint32_t value) {
#if defined(AMOS_TEST_WRAPPED)
    static_cast<amos::wire::MarkField*>(cell)->store(amos::host::decode_mark(value));
#else
    *static_cast<uint32_t*>(cell) = value;
#endif
}

// Default initialization must not add hidden null stores; value initialization
// explicitly requests null and must match a raw pointer's single zero store.
extern "C" void default_guest_pointer(void* storage) {
#if defined(AMOS_TEST_WRAPPED)
    ::new (storage) GuestPtr<Byte>;
#else
    ::new (storage) uint8_t*;
#endif
}
extern "C" void default_host_pointer(void* storage) {
#if defined(AMOS_TEST_WRAPPED)
    ::new (storage) HostPtr<Byte>;
#else
    ::new (storage) uint8_t*;
#endif
}
extern "C" void default_wire_pointer(void* storage) {
#if defined(AMOS_TEST_WRAPPED)
    ::new (storage) PointerField<Byte>;
#else
    ::new (storage) uint32_t;
#endif
}
extern "C" void default_recovery(void* storage) {
#if defined(AMOS_TEST_WRAPPED)
    ::new (storage) amos::wire::MarkField;
#else
    ::new (storage) uint32_t;
#endif
}
extern "C" void null_guest_pointer(void* storage) {
#if defined(AMOS_TEST_WRAPPED)
    ::new (storage) GuestPtr<Byte>{};
#else
    using NativePointer = uint8_t*;
    ::new (storage) NativePointer{};
#endif
}

extern "C" uint32_t load_context(const void* cell) {
#if defined(AMOS_TEST_WRAPPED)
    return amos::host::encode_context(static_cast<const amos::wire::ContextField*>(cell)->load());
#else
    return *static_cast<const uint32_t*>(cell);
#endif
}
extern "C" void store_context(void* cell, uint32_t value) {
#if defined(AMOS_TEST_WRAPPED)
    static_cast<amos::wire::ContextField*>(cell)->store(amos::host::decode_context(value));
#else
    *static_cast<uint32_t*>(cell) = value;
#endif
}

#if defined(AMOS_TEST_WRAPPED)
extern "C" uint32_t module_base(GuestPtr<const Byte> module) {
    return AddressCodec::encode(module.address());
}
#else
extern "C" uint32_t module_base(const uint8_t* module) {
    return uint32_t(module);
}
#endif

extern "C" uint32_t sum_bytes(uint32_t base, uint32_t count) {
    uint32_t sum = 0;
#if defined(AMOS_TEST_WRAPPED)
    auto cursor = AddressCodec::decode<Byte>(base);
    const auto end = cursor + count;
    for (; cursor != end; ++cursor) sum += Memory{}.load_byte(cursor).value;
#else
    auto cursor = reinterpret_cast<const uint8_t*>(base);
    const auto end = cursor + count;
    for (; cursor != end; ++cursor) sum += *cursor;
#endif
    return sum;
}
