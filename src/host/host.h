// The runtime of AMOS on another host (Phase 7): what the assembler and
// the Amiga's start did.
#pragma once

#include "core/amos.h"

extern "C" uint8_t *amos_bridge;

namespace amos::host {

// Stops: a routine (at address) the host has no C++ for
[[noreturn]] void missing(const char *what, const void *address);

} // namespace amos::host
