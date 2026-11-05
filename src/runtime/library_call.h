#pragma once
#include <stdint.h>
namespace amos::host {
// One exact signature for native thunks; each thunk calls its real C++ function
// with decoded argument types. No cast based on argument count is permitted.
using LibraryCall = uint64_t (*)(const uint32_t*);
}
