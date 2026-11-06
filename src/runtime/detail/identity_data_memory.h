// Private transition backend for scalar reads from legacy identity data backing.
// Ordinary guest APIs carry GuestPtr and never receive these native pointers.
#pragma once
#include "core/amos.h"
namespace amos::host::detail {
class IdentityDataMemory {
public:
    guest::Result<guest::Byte> load_byte(guest::GuestPtr<const guest::Byte> at) const {
        const auto word = guest::abi::AddressCodec::encode(at.address());
#if !defined(__mc68000__)
        if (!word) return {{}, guest::Fault::null_pointer};
#endif
        return {*ptr<const uint8_t>(word), guest::Fault::none};
    }
    guest::Fault store_byte(guest::GuestPtr<guest::Byte> at, guest::Byte value) const {
        const auto word = guest::abi::AddressCodec::encode(at.address());
#if !defined(__mc68000__)
        if (!word) return guest::Fault::null_pointer;
#endif
        *ptr<uint8_t>(word) = value;
        return guest::Fault::none;
    }
    guest::Fault store_be32(guest::GuestPtr<guest::Byte> at, uint32_t value) const {
        const auto word = guest::abi::AddressCodec::encode(at.address());
#if !defined(__mc68000__)
        if (!word) return guest::Fault::null_pointer;
        if (word > UINT32_MAX-3) return guest::Fault::overflow;
#endif
        poke_l(ptr<void>(word),value);
        return guest::Fault::none;
    }
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
inline void require_guest(guest::Fault fault) {
#if !defined(__mc68000__)
    if (fault != guest::Fault::none) guest::pointer_fault(fault);
#else
    (void)fault;
#endif
}
inline void write_cursor_byte(ContextCursor& cursor, uint8_t value) {
    const auto destination = guest_cursor(cursor++);
    require_guest(IdentityDataMemory{}.store_byte(destination,value));
}
}
