#pragma once
#include "tables/segment_ids.h"
#include "memory/guest_segments.h"
namespace amos::guest { class Memory; }
namespace amos::host {
#if !defined(__mc68000__)
void prepare_static_symbols();
const guest::SegmentBindings& static_symbol_bindings();
template<class T> guest::GuestPtr<T> static_pointer(guest::GuestSymbol<T> symbol) {
    auto pointer = static_symbol_bindings().resolve(symbol);
    if (!pointer) guest::pointer_fault(pointer.fault);
    return pointer.value;
}
uint32_t static_address(tables::Segment, uint32_t offset);
uint32_t static_difference(tables::Segment, uint32_t, tables::Segment, uint32_t);
// Builds an independent image; never aliases or modifies native source arrays.
guest::Fault load_static_image(guest::Memory&, guest::SegmentBindings&,
                               guest::GuestPtr<guest::Byte> fast, guest::GuestPtr<guest::Byte> chip);
#endif
}
#if defined(__mc68000__)
#define AMOS_STATIC_ADDRESS(name, offset) (uint32_t(uintptr_t(name + offset)))
#define AMOS_STATIC_DIFFERENCE(left, lo, right, ro) (AMOS_STATIC_ADDRESS(left, lo) - AMOS_STATIC_ADDRESS(right, ro))
#define AMOS_BRIDGE_METADATA(at) ::amos::host::put(at, 4, (uint32_t(uintptr_t(reinterpret_cast<const uint8_t *>(&amos_bridge)))))
#else
#define AMOS_STATIC_ADDRESS(name, offset) ::amos::host::static_address(::amos::tables::Segment::name, offset)
#define AMOS_STATIC_DIFFERENCE(left, lo, right, ro) ::amos::host::static_difference(::amos::tables::Segment::left, lo, ::amos::tables::Segment::right, ro)
#define AMOS_BRIDGE_METADATA(at) ::amos::host::put(at, 4, 0)
#endif
