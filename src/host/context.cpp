// The interpreter's registers on another host (one task), and its stop
// on an entry with no C++.

#include "runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>

using namespace amos;

namespace {
Ctx context;            // the interpreter's registers (one task here)
}

uint8_t *amos_bridge;
uint8_t amos_task[52];

namespace amos {

Ctx &ctx() { return context; }

} // namespace amos

namespace amos::host {

[[noreturn]] void missing(const char *what, const void *address)
{
    fprintf(stderr, "amos: %s %p has no C++ on this host\n", what, address);
    exit(70);
}

} // namespace amos::host
