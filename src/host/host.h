// The runtime of AMOS on another host (Phase 7): what the assembler and
// the Amiga's start did.
#pragma once

#include "core/amos.h"

extern uint8_t amos_task[];

namespace amos::host {

// Stops: a routine (at address) the host has no C++ for
[[noreturn]] void missing(const char *what, const void *address);

// The name of a library of the system at base (null: none)
const char *library_name(uint32_t base);

} // namespace amos::host
