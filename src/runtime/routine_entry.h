#pragma once
#include "core/guest_symbol.h"
namespace amos { struct AllRegs; }
namespace amos::host {
using RoutineSignature = int32_t(AllRegs&);
using RoutineEntry = guest::GuestEntry<RoutineSignature>;
}
