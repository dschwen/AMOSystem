// Four-byte guest code-address fields; no implicit integer or pointer conversion.
#pragma once
#include "core/endian.h"
#include "runtime/routine_entry.h"
namespace amos::wire {
class RoutineField {
    be_u32 word_;
public:
    host::RoutineArg load() const { return host::decode_routine(uint32_t(word_)); }
    void store(host::RoutineArg entry) { word_ = host::encode_routine(entry); }
};
static_assert(sizeof(RoutineField) == 4 && alignof(RoutineField) == alignof(be_u32));
}

namespace amos::guest {
template<> struct Layout<wire::RoutineField> { static constexpr uint32_t size = 4; };
}
