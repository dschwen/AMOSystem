#include "abi/guest_address_codec.h"
#include "memory/guest_view.h"
#include <assert.h>
#include <limits.h>
using namespace amos::guest;
using abi::AddressCodec;
struct GuestRecord { uint64_t native_only_not_the_wire_size; };
template<> struct amos::guest::Layout<GuestRecord> { static constexpr uint32_t size = 6; };
// Schema-only record: no native struct is ever overlaid on RAM.
struct Node;
template<> struct amos::guest::Layout<Node> { static constexpr uint32_t size = 8; };
using Next = Field<Node, PointerField<Node>, 0>;
using Text = Field<Node, PointerField<Byte>, 4>;
struct Unsized {};
static_assert(sizeof(GuestRecord) == 8);
static_assert(sizeof(GuestPtr<Byte>) == 4);
static_assert(alignof(Memory) == 8);
static_assert(sizeof(HostSpan<Byte>) == 16);
constexpr auto c = AddressCodec::decode<Byte>(0x12340000);
static_assert(c.try_advance(10).value.address() == AddressCodec::decode_address(0x1234000a));
static_assert(c.try_advance(INT64_MIN).fault == Fault::overflow);
static_assert(c.try_advance(INT64_MAX).fault == Fault::overflow);
static_assert(c.try_advance(UINT64_MAX).fault == Fault::overflow);
static_assert(GuestPtr<Byte>{}.try_advance(0).fault == Fault::null_pointer);
static_assert(AddressCodec::decode<Byte>(1).try_advance(-1).fault == Fault::null_pointer);

// Linked to a native-packed backend from a -fpack-struct=2 translation unit.
int main() {
    Memory ram;
    assert(ram.reserve() == Fault::none);
    assert(ram.reserve() == Fault::invalid_range);
    assert(ram.host_page_size() >= 4096);
    auto bank = AddressCodec::decode<Byte>(0x01000003); // deliberately unaligned
    assert(ram.map_ram(bank.address(), 32) == Fault::none);
    auto payload = bank + 10;
    assert((payload - bank).elements() == 10);
    assert((bank - payload).elements() == -10);
    assert(payload > bank && bank <= payload && bank != payload);
    GuestPtr<const Byte> const_payload(payload);
    assert(const_payload == payload && (const_payload - bank).elements() == 10);
    assert(10 + bank == payload);
    auto cursor = bank; cursor += 10; assert(cursor++ == payload);
    assert(--cursor == payload); cursor -= 10; assert(cursor == bank);
    assert(ram.store_be32(payload, 0x1234abcd) == Fault::none);
    assert(ram.load_be32(payload).value == 0x1234abcd);
    assert(ram.load_byte(payload).value == 0x12);
    PointerField<Byte> saved;
    saved.store(payload);
    assert(saved.load() == payload);
    assert(AddressCodec::decode<Byte>(AddressCodec::encode(saved.load().address())) == payload);
    assert(ram.load_be32(saved.load()).value == 0x1234abcd);
    const Byte source[] = {3, 7, 11, 19};
    Byte destination[4] = {};
    assert(ram.copy_from_host(GuestSpan<Byte>(payload, 4), HostSpan<const Byte>(source, 4)) == Fault::none);
    assert(ram.copy_to_host(HostSpan<Byte>(destination, 4), GuestSpan<const Byte>(payload, 4)) == Fault::none);
    for (unsigned i = 0; i < 4; ++i) assert(destination[i] == source[i]);
    assert(ram.copy_from_host(GuestSpan<Byte>(payload, 3), HostSpan<const Byte>(source, 4)) == Fault::invalid_range);
    assert(ram.copy_from_host(GuestSpan<Byte>(payload, 4), HostSpan<const Byte>(nullptr, 4)) == Fault::null_pointer);
    assert(ram.load_byte(bank + 31));
    assert(ram.load_byte(bank + 32).fault == Fault::unmapped); // one past
    assert(ram.store_byte(bank - 1, 3) == Fault::unmapped); // committed host page, guest hole
    assert(ram.map_ram(bank.address(), 1) == Fault::invalid_range);
    auto span = GuestSpan<Byte>(bank, 32);
    assert(span.subspan(10, 4).value.data() == payload);
    assert(span.subspan(32, 0));
    assert(span.subspan(32, 1).fault == Fault::invalid_range);
    assert(span.subspan(UINT64_MAX, 1).fault == Fault::invalid_range);

    // Typed arithmetic uses the six-byte guest schema, not eight-byte native sizeof.
    auto records = AddressCodec::decode<GuestRecord>(0x2000000);
    assert(AddressCodec::encode((records + 2).address()) == 0x200000c);
    assert((records + 2 - records).elements() == 2);
    assert(records.try_advance(INT64_MAX).fault == Fault::overflow);
    assert(records.try_advance(INT64_MIN).fault == Fault::overflow);
    assert((records.offset_bytes(GuestByteDelta(1))).try_distance(records).fault == Fault::misaligned);

    // Adjacent logical regions and host page crossings resolve the same addresses.
    auto edge = AddressCodec::decode<Byte>(0x3000000 + uint32_t(ram.host_page_size()) - 2);
    assert(ram.map_ram(edge.address(), 2) == Fault::none);
    assert(ram.map_ram((edge + 2).address(), 2) == Fault::none);
    assert(ram.store_be32(edge, 0x10203040) == Fault::none);
    assert(ram.load_be32(edge).value == 0x10203040);
    auto ro = edge + 4;
    assert(ram.map_ram(ro.address(), 2, Access::read) == Fault::none);
    assert(ram.store_byte(ro, 1) == Fault::permission);
    assert(ram.store_be32(edge + 2, 0xaabbccdd) == Fault::permission);
    assert(ram.load_be32(edge).value == 0x10203040); // full validation before mutation
    assert(ram.store_be32(edge + 3, 0) == Fault::permission);
    assert(ram.load_byte(GuestPtr<Byte>{}).fault == Fault::null_pointer);
    auto last = AddressCodec::decode<Byte>(UINT32_MAX);
    assert(last.try_advance(1).fault == Fault::overflow);
    assert(ram.map_ram(last.address(), 1) == Fault::none);
    assert(ram.store_byte(last, 99) == Fault::none);
    assert(ram.load_byte(last).value == 99);
    assert(ram.load_be32(last).fault == Fault::overflow);
    assert(ram.map_ram(last.address(), UINT64_MAX) == Fault::invalid_range);
    assert(ram.copy_to_host(HostSpan<Byte>(nullptr, 0), GuestSpan<const Byte>()) == Fault::none);

    // Linked-list/string view: typed fields remain live guest references.
    const auto first = AddressCodec::decode<Node>(0x4000000);
    const auto second = first + 1;
    assert(ram.map_ram(first.address(), 16) == Fault::none);
    auto first_view = ram.ref(first); assert(first_view);
    auto second_view = ram.ref(second); assert(second_view);
    auto next_field = first_view.value.field<Next>(); assert(next_field);
    assert(next_field.value.store(second) == Fault::none);
    assert(next_field.value.load().value == second);
    auto text_field = second_view.value.field<Text>(); assert(text_field);
    assert(text_field.value.store(payload) == Fault::none);
    auto followed = ram.ref(next_field.value.load().value); assert(followed);
    auto text = followed.value.field<Text>().value.load(); assert(text);
    assert(ram.load_byte(text.value).value == source[0]);
    GuestPtr<const Node> const_first(first);
    assert(ram.ref(const_first).value.field<Next>().value.load().value == second);
    assert(ram.ref(AddressCodec::decode<Node>(0x5000000)).fault == Fault::unmapped);
    auto scalar = AddressCodec::decode<uint32_t>(0x4000001); // odd, endian-safe
    assert(ram.ref(scalar).value.store(0x12345678) == Fault::none);
    assert(ram.ref(scalar).value.load().value == 0x12345678);

    // Independent arithmetic identities over a large reproducible address sample.
    uint32_t state = 42;
    for (unsigned i = 0; i < 10000; ++i) {
        state = state * 1664525u + 1013904223u;
        const uint32_t bits = (state & 0x7fffffffu) + 0x10000u;
        auto p = AddressCodec::decode<Byte>(bits);
        const int64_t delta = int64_t((state >> 16) & 0x7fff) - 0x4000;
        auto q = p + delta;
        assert(AddressCodec::encode(q.address()) == uint32_t(int64_t(bits) + delta));
        assert((q - p).elements() == delta);
        assert(q - delta == p);
    }
}
