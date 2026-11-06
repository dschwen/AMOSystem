#pragma once
#include "core/guest_symbol.h"
#include "abi/guest_entry_codec.h"
namespace amos { struct AllRegs; }
namespace amos::host {
using RoutineSignature = int32_t(AllRegs&);
using RoutineEntry = guest::GuestEntry<RoutineSignature>;
// Native Amiga callers retain exactly their original pointer ABI.
#if defined(__mc68000__)
using RoutineArg = const void*;
inline RoutineArg decode_routine(uint32_t word) { return reinterpret_cast<const void*>(uintptr_t(word)); }
inline uint32_t encode_routine(RoutineArg routine) { return uint32_t(reinterpret_cast<uintptr_t>(routine)); }
#else
using RoutineArg = RoutineEntry;
inline RoutineArg decode_routine(uint32_t word) { return guest::abi::EntryCodec::decode<RoutineSignature>(word); }
inline uint32_t encode_routine(RoutineArg routine) { return guest::abi::EntryCodec::encode(routine); }
#endif
}
