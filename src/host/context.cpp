// The interpreter's registers on another host (one task), and its stop
// on an entry with no C++.

#include "runtime/runtime.h"
#include "runtime/routine_entry.h"
#include "abi/guest_entry_codec.h"

#include <stdio.h>
#include <stdlib.h>
#include <atomic>

using namespace amos;

extern "C" uint8_t amos_low_page[4096];
#if defined(AMOS_GUEST_BIASED)
extern "C" uint8_t *amos_guest_base = nullptr;
extern "C" uint64_t amos_guest_size = 0;
namespace {
// The handle windows: window k of the region from AmosHandleBase stands for the 1MB of native
// address space region[k] (a multiple of 1MB). Entries are published (count) after they are written.
constexpr uint32_t WindowBits = 20, Windows = (0u - AmosHandleBase) >> WindowBits;
uintptr_t region[Windows];
std::atomic<uint32_t> windows{0};
std::atomic_flag adding = ATOMIC_FLAG_INIT;
}
extern "C" uint8_t *amos_handle_native(uint32_t address)
{
    uint32_t k = (address - AmosHandleBase) >> WindowBits;
    if (k >= windows.load(std::memory_order_acquire)) return nullptr;
    return reinterpret_cast<uint8_t *>(region[k] + (address & ((1u << WindowBits) - 1)));
}
// (AMOS_ESCAPE_LOG=1 lists the places each: the native addresses that are made guest words)
extern "C" uint32_t amos_address_escape(uintptr_t p, const char *file, int line)
{
    static const bool log = getenv("AMOS_ESCAPE_LOG");
    if (log) {
        static const char *seen[1024][2];
        static int lines[1024], count;
        bool fresh = true;
        for (int i = 0; i < count; i++)
            if (lines[i] == line && seen[i][0] == file) fresh = false;
        if (fresh && count < 1024) {
            seen[count][0] = file; lines[count++] = line;
            fprintf(stderr, "amos: ESCAPE %s:%d %#lx\n", file, line, (unsigned long)p);
        }
    }
    const uintptr_t base = p & ~((uintptr_t(1) << WindowBits) - 1);
    for (;;) {
        uint32_t n = windows.load(std::memory_order_acquire);
        for (uint32_t k = 0; k < n; k++)
            if (region[k] == base) return AmosHandleBase + (k << WindowBits) + uint32_t(p - base);
        while (adding.test_and_set(std::memory_order_acquire)) {}
        n = windows.load(std::memory_order_relaxed);
        bool found = false;
        for (uint32_t k = 0; k < n; k++) found |= region[k] == base;
        if (!found) {
            if (n == Windows) { fprintf(stderr, "amos: out of handle windows\n"); __builtin_trap(); }
            region[n] = base;
            windows.store(n + 1, std::memory_order_release);
        }
        adding.clear(std::memory_order_release);
    }
}
#endif
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

[[noreturn]] void missing(const char *what, RoutineEntry at)
{
    fprintf(stderr, "amos: %s guest routine 0x%08x has no C++ on this host\n", what, guest::abi::EntryCodec::encode(at));
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

int32_t native_call(RoutineEntry at, AllRegs &r)
{
    if (hal::library_call(guest::abi::EntryCodec::encode(at), r))
        return int32_t(r.d[0]);
    missing("address", at);
}

} // namespace amos::host

// (startup.cpp hands the patched LoadView of the Amiga its flag)
extern "C" {
uint32_t w_amos_here;
}
