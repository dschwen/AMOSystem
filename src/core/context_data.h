// Live interpreter data cursors retain guest addresses on translated hosts.
#pragma once
#include "abi/guest_address_codec.h"
namespace amos {
#if defined(__mc68000__)
using ContextCursor = uint8_t*;
inline ContextCursor decode_context_cursor(uint32_t word) { return reinterpret_cast<uint8_t*>(uintptr_t(word)); }
inline uint32_t encode_context_cursor(ContextCursor address) { return uint32_t(reinterpret_cast<uintptr_t>(address)); }
#else
using ContextCursor = guest::GuestPtr<guest::Byte>;
inline ContextCursor decode_context_cursor(uint32_t word) { return guest::abi::AddressCodec::decode<guest::Byte>(word); }
inline uint32_t encode_context_cursor(ContextCursor address) { return guest::abi::AddressCodec::encode(address.address()); }
#endif
using DataZoneAddress = ContextCursor;
inline DataZoneAddress decode_data_zone(uint32_t word) { return decode_context_cursor(word); }
inline uint32_t encode_data_zone(DataZoneAddress address) { return encode_context_cursor(address); }
inline guest::GuestPtr<guest::Byte> guest_cursor(ContextCursor address) {
    return guest::abi::AddressCodec::decode<guest::Byte>(encode_context_cursor(address));
}
static_assert(sizeof(ContextCursor) == 4);
}
