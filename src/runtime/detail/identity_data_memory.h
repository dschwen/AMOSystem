// Private transition backend for scalar reads from legacy identity data backing.
// Ordinary guest APIs carry GuestPtr and never receive these native pointers.
#pragma once
#include "core/amos.h"
namespace amos::host::detail {
class IdentityDataMemory {
public:
    guest::Result<uint16_t> load_be16(guest::GuestPtr<const guest::Byte> at) const {
        const auto word = guest::abi::AddressCodec::encode(at.address());
#if !defined(__mc68000__)
        if (!word) return {{}, guest::Fault::null_pointer};
        if (word > UINT32_MAX-1) return {{}, guest::Fault::overflow};
#endif
        return {peek_w(ptr<const void>(word)), guest::Fault::none};
    }
    guest::Result<uint32_t> load_be32(guest::GuestPtr<const guest::Byte> at) const {
        const auto word = guest::abi::AddressCodec::encode(at.address());
#if !defined(__mc68000__)
        if (!word) return {{}, guest::Fault::null_pointer};
        if (word > UINT32_MAX-3) return {{}, guest::Fault::overflow};
#endif
        return {peek_l(ptr<const void>(word)), guest::Fault::none};
    }
};
template<class T> inline T require_guest(guest::Result<T> value) {
#if !defined(__mc68000__)
    if (!value) guest::pointer_fault(value.fault);
#endif
    return value.value;
}
inline guest::GuestPtr<const guest::Byte> legacy_table_address(const uint8_t* at) {
    const auto word = reinterpret_cast<uintptr_t>(at);
#if !defined(__mc68000__)
    if (word > UINT32_MAX) guest::pointer_fault(guest::Fault::overflow);
#endif
    return guest::abi::AddressCodec::decode<const guest::Byte>(uint32_t(word));
}
}
