// Amiga system libraries for the C++ code. The code runs without a C
// library startup, so the library bases are read where AMOS keeps them.
#pragma once

#include <stdint.h>

#if defined(__mc68000__)
#define EXEC_BASE_NAME (*reinterpret_cast<struct Library **>(4))
#include <proto/exec.h>
#else
// Another host: the system's structures only (its services are src/hal's)
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <exec/ports.h>
#include <exec/libraries.h>
#endif

namespace amos {

// AllocMem flags, as in +Equ.s
constexpr uint32_t MemPublic = 0x01;
constexpr uint32_t MemChip = 0x02;
constexpr uint32_t MemFast = 0x04;
constexpr uint32_t MemClear = 0x10000;

} // namespace amos
