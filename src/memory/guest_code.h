// Read a guest code entry's scalar metadata through its memory backend.
// This is an explicit code/data boundary and never exposes a native pointer.
#pragma once
#include "memory/guest_memory.h"
#include "abi/guest_entry_codec.h"
namespace amos::guest {
template<class Signature, class Reader = Memory> class CodeReader {
    const Reader& memory_;
    GuestEntry<Signature> entry_;
    Result<GuestPtr<const Byte>> location(int32_t offset) const {
        auto base = abi::AddressCodec::decode<const Byte>(abi::EntryCodec::encode(entry_));
#if defined(__mc68000__)
        return {base.offset_bytes(GuestByteDelta(offset)), Fault::none};
#else
        return base.try_offset_bytes(GuestByteDelta(offset));
#endif
    }
public:
    CodeReader(const Reader& memory, GuestEntry<Signature> entry) : memory_(memory), entry_(entry) {}
    Result<uint16_t> word_at(int32_t offset) const {
        auto at = location(offset);
        if (!at) return {{}, at.fault};
        return memory_.load_be16(at.value);
    }
    Result<uint32_t> long_at(int32_t offset) const {
        auto at = location(offset);
        if (!at) return {{}, at.fault};
        return memory_.load_be32(at.value);
    }
};
}
