// The runtime of AMOS on another host (Phase 7): what the assembler and
// the Amiga's start did.
#pragma once

#include "core/amos.h"
#include "runtime/routine_entry.h"

#if defined(__mc68000__)
extern "C" uint8_t amos_task[];
#endif

namespace amos::host {

// Stops: a routine (at address) the host has no C++ for
[[noreturn]] void missing(const char *what, const void *address);
#if !defined(__mc68000__)
[[noreturn]] void missing(const char *what, RoutineEntry address);
#endif

// The narrator's voice (host/speech.cpp)
bool speak(const char *text, uint32_t length);
bool speaking();
void speech_stop();

// The other tasks (host/tasks.cpp): they run while the program waits
void run_tasks();
void task_yield();
bool in_task();

// The name of a library of the system at base (null: none)
const char *library_name(uint32_t base);

} // namespace amos::host
