// Temporary identity backing for data-zone records. Only this boundary creates
// native record references; replace it with schema views before high backing.
#pragma once
#include "core/context_data.h"
namespace amos::host::detail {
#if defined(__mc68000__)
inline DataZoneAddress capture_data_zone(uint8_t* native) { return native; }
#else
inline guest::Result<DataZoneAddress> try_capture_data_zone(uint8_t* native) {
    const auto word = reinterpret_cast<uintptr_t>(native);
    if (word > UINT32_MAX) return {{}, guest::Fault::overflow};
    return {decode_data_zone(uint32_t(word)), guest::Fault::none};
}
inline DataZoneAddress capture_data_zone(uint8_t* native) {
    auto address = try_capture_data_zone(native);
    if (!address) guest::pointer_fault(address.fault);
    return address.value;
}
template<class Record> Record& identity_record(DataZoneAddress address) {
    const auto word = encode_data_zone(address);
    if (!word) guest::pointer_fault(guest::Fault::null_pointer);
    if (sizeof(Record) > (uint64_t(1) << 32) - word) guest::pointer_fault(guest::Fault::overflow);
    return *reinterpret_cast<Record*>(uintptr_t(word));
}
#endif
}
