// Amiga system libraries for the C++ code. The code runs without a C
// library startup, so the library bases are read where AMOS keeps them.
#pragma once

#define EXEC_BASE_NAME (*reinterpret_cast<struct Library **>(4))
#include <proto/exec.h>

namespace amos {

// AllocMem flags, as in +Equ.s
constexpr uint32_t MemPublic = 0x01;
constexpr uint32_t MemChip = 0x02;
constexpr uint32_t MemFast = 0x04;
constexpr uint32_t MemClear = 0x10000;

} // namespace amos
