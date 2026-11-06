#include "memory/guest_segments.h"
#include "runtime/detail/identity_segments.h"
#include "memory/guest_memory.h"
#include "abi/guest_address_codec.h"
#include <assert.h>
using namespace amos::guest;
using abi::AddressCodec;
int main() {
    Memory memory;
    assert(memory.reserve() == Fault::none);
    const auto base = AddressCodec::decode<Byte>(0x12340000);
    assert(memory.map_ram(base.address(), 128) == Fault::none);
    SegmentBindings bindings;
    assert(bindings.bind(SegmentId(1), GuestSpan<Byte>(base, 128)) == Fault::none);
    assert(bindings.bind(SegmentId(1), GuestSpan<Byte>(base, 128)) == Fault::invalid_range);
    assert(bindings.bind(SegmentId(2), GuestSpan<Byte>(base + 1, 8)) == Fault::invalid_range);
    assert(bindings.bind(SegmentId(128), GuestSpan<Byte>(base + 128, 8)) == Fault::invalid_range);
    constexpr GuestSymbol<Byte> bank(SegmentId(1), 0);
    const auto payload = bindings.resolve(bank.try_offset(GuestByteDelta(10)).value);
    assert(payload && payload.value == base + 10);
    assert(memory.store_be32(payload.value, 0x01020304) == Fault::none);
    assert(memory.load_be32(bindings.resolve(GuestSymbol<const Byte>(SegmentId(1), 10)).value).value == 0x01020304);
    assert(bindings.resolve(GuestSymbol<Byte>(SegmentId(1), 128)).value == base + 128);
    assert(bindings.resolve(GuestSymbol<Byte>(SegmentId(1), 129)).fault == Fault::invalid_range);
    assert(bindings.resolve(GuestSymbol<Byte>(SegmentId(2), 0)).fault == Fault::unmapped);
    auto entry = bindings.resolve(EntrySymbol<void()>(SegmentId(1), 124));
    assert(entry && entry.value.address() == (base + 124).address());
    assert(bindings.resolve(EntrySymbol<void()>(SegmentId(1), 125)).fault == Fault::invalid_range);
    auto distance = bindings.difference(GuestSymbol<const Byte>(SegmentId(1), 32), GuestSymbol<const Byte>(SegmentId(1), 8));
    assert(distance && distance.value == 24);
    Byte native[8]{};
    assert(reinterpret_cast<uintptr_t>(native) > UINT32_MAX);
    assert(detail::IdentitySegments::bind(bindings, SegmentId(2), HostSpan<Byte>(native, 8)) == Fault::overflow);
    const auto last = AddressCodec::decode<Byte>(UINT32_MAX);
    assert(bindings.bind(SegmentId(2), GuestSpan<Byte>(last, 1)) == Fault::none);
    assert(bindings.resolve(GuestSymbol<Byte>(SegmentId(2), 1)).fault == Fault::overflow);
}
