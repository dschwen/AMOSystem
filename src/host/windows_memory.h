// Windows virtual memory, isolated from the interpreter's packed structures.
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace amos::host::windows {
// All mappings are zero-filled, read/write, reserved and committed.
// A non-null exact address never replaces an existing mapping.
void *memory_allocate(size_t size, bool below_4gb, void *exact = nullptr);
bool memory_release(void *address);
uint32_t memory_error();
}
