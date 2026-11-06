// The interpreter's registers on another host (one task), and its stop
// on an entry with no C++.

#include "runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>

using namespace amos;

extern "C" uint8_t amos_low_page[4096];
uint8_t amos_low_page[4096];

namespace {
Ctx context;            // the interpreter's registers (one task here)
}

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

namespace amos::host {

// A call to an address that is no entry: there is no machine code here
} // namespace amos::host
namespace amos::hal {
bool library_call(uint32_t at, AllRegs &r);
}
namespace amos::host {

int32_t native_call(const void *at, AllRegs &r)
{
    if (hal::library_call(address(at), r))
        return int32_t(r.d[0]);
    missing("address", at);
}

} // namespace amos::host

// (startup.cpp hands the patched LoadView of the Amiga its flag)
extern "C" {
uint32_t w_amos_here;
}
