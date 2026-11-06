// Architectural code addresses have their own wire boundary, separate from data.
#pragma once
#include "core/guest_symbol.h"
#include "abi/guest_address_codec.h"
namespace amos::guest::abi {
struct EntryCodec {
    template<class Signature> static constexpr GuestEntry<Signature> decode(uint32_t word) {
        return GuestEntry<Signature>(AddressCodec::decode_address(word));
    }
    template<class Signature> static constexpr uint32_t encode(GuestEntry<Signature> entry) {
        return AddressCodec::encode(entry.address());
    }
};
}
