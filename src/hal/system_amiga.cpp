// hal/system.h on the Amiga: exec.library

#include "hal/system.h"

#include "core/amiga.h"

namespace amos::hal {

uint32_t available_memory(uint32_t flags) { return AvailMem(flags); }
void forbid() { Forbid(); }
void permit() { Permit(); }

} // namespace amos::hal
