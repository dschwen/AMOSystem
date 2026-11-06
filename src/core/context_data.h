// The live interpreter data-zone cursor is a guest address on translated hosts.
#pragma once
#include "abi/guest_address_codec.h"
namespace amos {
#if defined(__mc68000__)
using DataZoneAddress = uint8_t*;
inline DataZoneAddress decode_data_zone(uint32_t word) { return reinterpret_cast<uint8_t*>(uintptr_t(word)); }
inline uint32_t encode_data_zone(DataZoneAddress address) { return uint32_t(reinterpret_cast<uintptr_t>(address)); }
#else
using DataZoneAddress = guest::GuestPtr<guest::Byte>;
inline DataZoneAddress decode_data_zone(uint32_t word) { return guest::abi::AddressCodec::decode<guest::Byte>(word); }
inline uint32_t encode_data_zone(DataZoneAddress address) { return guest::abi::AddressCodec::encode(address.address()); }
#endif
static_assert(sizeof(DataZoneAddress) == 4);
}
