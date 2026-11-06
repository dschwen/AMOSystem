#include "runtime/static_symbols.h"
#include "runtime/detail/identity_segments.h"
#include "runtime/glue.h"
#include "tables/tables.h"
#include "memory/guest_memory.h"
#include "abi/guest_address_codec.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#if defined(AMOS_STATIC_IMAGE_PROBE)
#include <stdio.h>
#endif
using namespace amos;
using namespace amos::guest;
namespace {
struct Source { tables::Segment id; HostSpan<Byte> bytes; bool chip; };
enum class Kind { scalar, absolute, difference };
struct Relocation {
    tables::Segment source; uint32_t offset; uint8_t width; Kind kind;
    tables::Segment target; uint32_t target_offset;
    tables::Segment anchor; uint32_t anchor_offset; uint32_t scalar;
};
#include "tables/generated/static_segments.inc"
SegmentId id(tables::Segment segment) { return SegmentId(uint16_t(segment)); }
GuestPtr<Byte> address(uint32_t word) { return abi::AddressCodec::decode<Byte>(word); }
GuestPtr<Byte> at(const SegmentBindings& bindings, tables::Segment segment, uint32_t offset = 0) {
    auto p = bindings.resolve(GuestSymbol<Byte>(id(segment), offset)); assert(p); return p.value;
}
uint32_t bits(GuestPtr<Byte> p) { return abi::AddressCodec::encode(p.address()); }
}
int main() {
#if defined(AMOS_NATIVE_STATIC_REFERENCE)
    // Current production identity image is an independent byte-level reference.
    host::prepare_static_symbols();
    host::glue_relocate();
#else
    assert(reinterpret_cast<uintptr_t>(host_obj__B_s) > UINT32_MAX);
#endif
    Memory memory;
    assert(memory.reserve() == Fault::none);
    SegmentBindings bindings;
    assert(host::load_static_image(memory, bindings, address(0x4000000), address(0x100000)) == Fault::none);
    assert(memory.load_be32(at(bindings, tables::Segment::host_obj__Edit_s, 0)).value == 64);
    assert(memory.load_be32(at(bindings, tables::Segment::host_obj__Edit_s, 390)).value ==
           bits(at(bindings, tables::Segment::host_obj_src_editor_cursor_glue_s)));
    assert(memory.load_be32(at(bindings, tables::Segment::host_obj__Edit_s, 60)).value == 0);
    assert(memory.load_be32(at(bindings, tables::Segment::host_obj__Monitor_s, 16)).value == 0);
    assert(memory.load_be32(at(bindings, tables::Segment::host_obj__W_s, 8)).value == sizeof(WData));
    assert(memory.load_be32(at(bindings, tables::Segment::host_obj_src_lib_banks_glue_s, 124)).value == 0);
    assert(bits(at(bindings, tables::Segment::host_obj__W_s_chip)) == 0x100000);
    auto short_delta = at(bindings, tables::Segment::host_obj__Monitor_s, 538);
    assert(memory.load_byte(short_delta).value == 1 && memory.load_byte(short_delta + 1).value == 24);
    for (const auto& source : sources) {
        auto span = bindings.extent(id(source.id)); assert(span && span.value.size() == source.bytes.size());
        assert(bits(span.value.data()) >= (source.chip ? 0x100000u : 0x4000000u));
    }
#if defined(AMOS_NATIVE_STATIC_REFERENCE)
    SegmentBindings identity;
    for (const auto& source : sources) assert(guest::detail::IdentitySegments::bind(identity, id(source.id), source.bytes) == Fault::none);
    // Convert only absolute pointer fields from native placement to guest
    // placement, then compare every byte of every image segment.
    for (const auto& source : sources) {
        Byte expected[32768], actual[32768];
        assert(source.bytes.size() <= sizeof expected);
        const auto native = bits(at(identity, source.id));
        memcpy(expected, reinterpret_cast<const Byte*>(uintptr_t(native)), size_t(source.bytes.size()));
        for (const auto& relocation : relocations) {
            if (relocation.source != source.id || relocation.kind == Kind::scalar) continue;
            uint32_t old = 0;
            for (unsigned i=0; i<relocation.width; ++i) old = (old << 8) | expected[relocation.offset+i];
            uint32_t adjusted = old + bits(at(bindings, relocation.target)) - bits(at(identity, relocation.target));
            if (relocation.kind == Kind::difference)
                adjusted += bits(at(identity, relocation.anchor)) - bits(at(bindings, relocation.anchor));
            for (unsigned i=0; i<relocation.width; ++i)
                expected[relocation.offset+i] = Byte(adjusted >> (8*(relocation.width-1-i)));
        }
        assert(memory.copy_to_host(HostSpan<Byte>(actual, source.bytes.size()),
                                   GuestSpan<const Byte>(at(bindings, source.id), source.bytes.size())) == Fault::none);
        assert(memcmp(expected, actual, size_t(source.bytes.size())) == 0);
    }
#endif
    Memory shifted;
    assert(shifted.reserve() == Fault::none);
    SegmentBindings shifted_bindings;
    assert(host::load_static_image(shifted, shifted_bindings, address(0x80000000), address(0x200000)) == Fault::none);
    assert(shifted.load_be32(at(shifted_bindings, tables::Segment::host_obj__Edit_s, 390)).value ==
           bits(at(shifted_bindings, tables::Segment::host_obj_src_editor_cursor_glue_s)));
    assert(shifted.load_be32(at(shifted_bindings, tables::Segment::host_obj__Edit_s, 0)).value == 64);
    Memory bad;
    assert(bad.reserve() == Fault::none);
    SegmentBindings collision;
    assert(host::load_static_image(bad, collision, address(0x100000), address(0x100000)) == Fault::invalid_range);
#if defined(AMOS_STATIC_IMAGE_PROBE)
    printf("{\"translated_static_image\":true,\"segments\":%zu,\"relocations\":%zu,"
           "\"native_sources_above_4gb\":true,\"page_size\":%llu,\"scope\":\"static image only\"}\n",
           sizeof(sources)/sizeof(sources[0]), sizeof(relocations)/sizeof(relocations[0]),
           static_cast<unsigned long long>(memory.host_page_size()));
#endif
}
