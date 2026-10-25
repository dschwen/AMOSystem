// The host system's services AMOS uses besides its files (Phase 7: the
// portability layer): the memory there is, and the sections no other task
// may interrupt. hal/system_amiga.cpp is exec.library.
#pragma once

#include <stdint.h>

namespace amos::hal {

// The memory free of the kinds of flags (exec's MEMF_: public, chip,
// fast; total for all of it)
constexpr uint32_t MemoryPublic = 0x1, MemoryChip = 0x2, MemoryFast = 0x4, MemoryTotal = 0x80000;
uint32_t available_memory(uint32_t flags);

// No other task runs between forbid() and permit()
void forbid();
void permit();

} // namespace amos::hal
