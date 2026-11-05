// Explicit untyped ABI boundary, not a native-pointer converter.
#pragma once
#include "core/guest_types.h"
namespace amos::guest::abi {
struct AddressCodec {
    template<class T> static constexpr GuestPtr<T> decode(uint32_t word) {
        return GuestPtr<T>(word);
    }
    static constexpr GuestAddress32 decode_address(uint32_t word) { return GuestAddress32(word); }
    static constexpr uint32_t encode(GuestAddress32 address) { return address.bits_; }
};
}
