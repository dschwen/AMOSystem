// Windows virtual memory, isolated from the interpreter's packed structures.
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace amos::host::windows {
// All mappings are zero-filled, read/write, reserved and committed.
// A non-null exact address never replaces an existing mapping.
void *memory_allocate(size_t size, bool below_4gb, void *exact = nullptr);
bool memory_release(void *address);
// The guest's memory (AMOS_GUEST_BIASED): a range reserved anywhere, no access until parts of it
// are committed (read/write, zero-filled; the commit charge is only what is committed)
void *memory_reserve(size_t size);
bool memory_commit(void *address, size_t size);
uint32_t memory_error();
}
