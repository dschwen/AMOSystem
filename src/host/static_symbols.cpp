// Native source storage and architectural guest placement are separate domains.
#include "runtime/static_symbols.h"
#include "runtime/detail/identity_segments.h"
#include "tables/tables.h"
#include "hal/system.h"
#include "core/amos.h"
#include <string.h>
#include "memory/guest_memory.h"
#include "abi/guest_address_codec.h"
#include <stdio.h>
#include <stdlib.h>
#pragma pack(push,8)
namespace amos::host {
namespace {
using namespace guest;
struct Source { tables::Segment id; HostSpan<Byte> bytes; bool chip; };
enum class Kind { scalar, absolute, difference };
struct Relocation {
    tables::Segment source;
    uint32_t offset;
    uint8_t width;
    Kind kind;
    tables::Segment target;
    uint32_t target_offset;
    tables::Segment anchor;
    uint32_t anchor_offset;
    uint32_t scalar;
};
#include "tables/generated/static_segments.inc"
SegmentBindings native_bindings;
bool ready;
SegmentId id(tables::Segment segment) { return SegmentId(uint16_t(segment)); }
GuestSymbol<const Byte> symbol(tables::Segment segment, uint32_t offset) {
    return GuestSymbol<const Byte>(id(segment), offset);
}
[[noreturn]] void invalid_static_symbol() {
    fprintf(stderr, "amos: invalid or unrepresentable static guest symbol\n");
    abort();
}
Result<uint32_t> relocated_value(const SegmentBindings& bindings, const Relocation& relocation) {
    if (relocation.kind == Kind::scalar) return {relocation.scalar, Fault::none};
    if (relocation.kind == Kind::absolute) {
        auto address = bindings.resolve(symbol(relocation.target, relocation.target_offset));
        if (!address) return {{}, address.fault};
        return {abi::AddressCodec::encode(address.value.address()), Fault::none};
    }
    auto delta = bindings.difference(symbol(relocation.target, relocation.target_offset),
                                     symbol(relocation.anchor, relocation.anchor_offset));
    if (!delta) return {{}, delta.fault};
    if (delta.value < INT32_MIN || delta.value > INT32_MAX) return {{}, Fault::overflow};
    if (relocation.width == 2 && (delta.value < INT16_MIN || delta.value > INT16_MAX))
        return {{}, Fault::overflow};
    return {uint32_t(delta.value), Fault::none};
}
}
#if defined(AMOS_GUEST_BIASED)
// The tables' data is loaded into the guest's memory (chip segments to chip memory), with its
// relocations: the native arrays are the image's source and are not used again.
void prepare_static_symbols() {
    if (ready) return;
    SegmentBindings plan;
    for (const auto& source : sources) {
        const uint64_t size = source.bytes.size();
        uint8_t *memory = static_cast<uint8_t *>(hal::allocate(uint32_t(size ? size : 1), source.chip ? hal::MemoryChip : 0));
        if (!memory) invalid_static_symbol();
        memcpy(memory, source.bytes.data().unsafe_native(), size_t(size));
        const auto start = abi::AddressCodec::decode<Byte>(address(memory));
        if (plan.bind(id(source.id), GuestSpan<Byte>(start, size)) != Fault::none) invalid_static_symbol();
    }
    for (const auto& relocation : relocations) {
        auto destination = plan.resolve(GuestSymbol<Byte>(id(relocation.source), relocation.offset));
        auto value = relocated_value(plan, relocation);
        if (!destination || !value) invalid_static_symbol();
        auto *at = ptr<uint8_t>(abi::AddressCodec::encode(destination.value.address()));
        for (uint8_t i = 0; i < relocation.width; ++i)
            at[i] = uint8_t(value.value >> (8 * (relocation.width - 1 - i)));
    }
    native_bindings = plan;
    ready = true;
}
#else
void prepare_static_symbols() {
    if (ready) return;
    for (const auto& source : sources) {
        // Only this transition adapter may inspect a native source address.
        // Array length and identity remain explicit in the HostSpan descriptor.
        if (guest::detail::IdentitySegments::bind(native_bindings, id(source.id), source.bytes) != Fault::none) invalid_static_symbol();
    }
    ready = true;
}
#endif
const SegmentBindings& static_symbol_bindings() {
    if (!ready) prepare_static_symbols();
    return native_bindings;
}
namespace detail {
uint8_t* native_callback_marker(guest::EntrySymbol<RoutineSignature> symbol) {
    const auto entry = static_entry(symbol);
    return ptr<uint8_t>(guest::abi::EntryCodec::encode(entry));
}
}
uint32_t static_address(tables::Segment segment, uint32_t offset) {
    if (!ready) prepare_static_symbols();
    auto pointer = native_bindings.resolve(symbol(segment, offset));
    if (!pointer) invalid_static_symbol();
    return abi::AddressCodec::encode(pointer.value.address());
}
uint32_t static_difference(tables::Segment left, uint32_t lo, tables::Segment right, uint32_t ro) {
    if (!ready) invalid_static_symbol();
    auto delta = native_bindings.difference(symbol(left, lo), symbol(right, ro));
    if (!delta || delta.value < INT32_MIN || delta.value > INT32_MAX) invalid_static_symbol();
    return uint32_t(delta.value);
}
Fault load_static_image(Memory& memory, SegmentBindings& output, GuestPtr<Byte> fast, GuestPtr<Byte> chip) {
    SegmentBindings plan;
    uint64_t fast_cursor = abi::AddressCodec::encode(fast.address());
    uint64_t chip_cursor = abi::AddressCodec::encode(chip.address());
    if (!fast_cursor || !chip_cursor) return Fault::null_pointer;
    for (const auto& source : sources) {
        auto& cursor = source.chip ? chip_cursor : fast_cursor;
        cursor = (cursor + 3) & ~uint64_t(3);
        if (cursor > UINT32_MAX || source.bytes.size() > (uint64_t(1) << 32) - cursor) return Fault::overflow;
        if (source.chip && cursor + source.bytes.size() > (uint64_t(1) << 24)) return Fault::invalid_range;
        auto start = abi::AddressCodec::decode<Byte>(uint32_t(cursor));
        const auto fault = plan.bind(id(source.id), GuestSpan<Byte>(start, source.bytes.size()));
        if (fault != Fault::none) return fault;
        cursor += source.bytes.size();
    }
    // Validate every symbol and relocation before touching destination memory.
    for (const auto& relocation : relocations) {
        auto extent = plan.extent(id(relocation.source));
        if (!extent || relocation.offset > extent.value.size() || relocation.width > extent.value.size() - relocation.offset)
            return Fault::invalid_range;
        auto value = relocated_value(plan, relocation);
        if (!value) return value.fault;
    }
    for (const auto& source : sources) {
        const auto destination = plan.extent(id(source.id)).value;
        if (!destination.size()) continue;
        auto fault = memory.map_ram(destination.data().address(), destination.size());
        if (fault != Fault::none) return fault;
        fault = memory.copy_from_host(destination, source.bytes);
        if (fault != Fault::none) return fault;
    }
    for (const auto& relocation : relocations) {
        auto destination = plan.resolve(GuestSymbol<Byte>(id(relocation.source), relocation.offset)).value;
        const auto value = relocated_value(plan, relocation).value;
        for (uint8_t i = 0; i < relocation.width; ++i) {
            const auto byte = Byte(value >> (8 * (relocation.width - 1 - i)));
            const auto fault = memory.store_byte(destination + i, byte);
            if (fault != Fault::none) return fault;
        }
    }
    output = plan;
    return Fault::none;
}
}
#pragma pack(pop)
