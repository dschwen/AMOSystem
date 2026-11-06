// Saved guest register values, independent of native context storage addresses.
#pragma once
#include "core/amos.h"
#pragma pack(push, 8)
namespace amos::host {
#if defined(__mc68000__)
using GuestContext = Ctx;
using ContextAddress = uint8_t*;
inline uint32_t context_address(ContextAddress value) { return address(value); }
#else
struct alignas(uint32_t) GuestContext {
    uint32_t d6 = 0, d7 = 0;
    guest::GuestPtr<guest::Byte> a3{}, a4{}, a5{}, a6{};
};
using ContextAddress = guest::GuestPtr<guest::Byte>;
inline uint32_t context_address(ContextAddress value) { return guest::abi::AddressCodec::encode(value.address()); }
static_assert(sizeof(GuestContext) == 24 && alignof(GuestContext) == alignof(uint32_t));
static_assert(offsetof(GuestContext,a3) == 8 && offsetof(GuestContext,a6) == 20);
#endif
}
#pragma pack(pop)
