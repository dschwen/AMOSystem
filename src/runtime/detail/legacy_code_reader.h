// Temporary identity data backend; no native pointer escapes this boundary.
// Replace this backend with production Memory when data backing is translated.
#pragma once
#include "core/amos.h"
#include "runtime/detail/identity_data_memory.h"
#if !defined(__mc68000__)
#include "memory/guest_code.h"
#endif
namespace amos::host::detail {
class LegacyCodeReader {
#if defined(__mc68000__)
    RoutineArg entry_;
#else
    IdentityDataMemory memory_;
    guest::CodeReader<RoutineSignature, IdentityDataMemory> reader_;
    template<class T> static T require(guest::Result<T> value) {
        if (!value) guest::pointer_fault(value.fault);
        return value.value;
    }
#endif
public:
#if defined(__mc68000__)
    explicit LegacyCodeReader(RoutineArg entry) : entry_(entry) {}
    uint16_t word_at(int32_t offset) const { return peek_w(static_cast<const uint8_t*>(entry_) + offset); }
    uint32_t long_at(int32_t offset) const { return peek_l(static_cast<const uint8_t*>(entry_) + offset); }
#else
    explicit LegacyCodeReader(RoutineArg entry) : reader_(memory_, entry) {}
    uint16_t word_at(int32_t offset) const { return require(reader_.word_at(offset)); }
    uint32_t long_at(int32_t offset) const { return require(reader_.long_at(offset)); }
#endif
};
}
