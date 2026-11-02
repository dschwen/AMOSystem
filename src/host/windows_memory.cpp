// Compile this translation unit with native Windows ABI packing, not AMOS's.
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "host/windows_memory.h"

namespace amos::host::windows {
namespace {
constexpr uintptr_t Limit = uintptr_t(1) << 32;
}

void *memory_allocate(size_t size, bool below_4gb, void *exact)
{
    if (!size) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return nullptr;
    }
    if (exact) {
        uintptr_t at = reinterpret_cast<uintptr_t>(exact);
        if (below_4gb && (at >= Limit || size > Limit - at)) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return nullptr;
        }
        void *p = VirtualAlloc(exact, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (p && p != exact) {
            VirtualFree(p, 0, MEM_RELEASE);
            SetLastError(ERROR_INVALID_ADDRESS);
            return nullptr;
        }
        return p;
    }
    if (!below_4gb)
        return VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // Leave the lowest addresses for chip memory. VirtualAlloc only reserves
    // free regions: an allocation race causes a failed attempt, not replacement.
    SYSTEM_INFO system;
    GetSystemInfo(&system);
    const uintptr_t granularity = system.dwAllocationGranularity;
    const uintptr_t page = system.dwPageSize;
    if (size > Limit - page) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    size = (size + page - 1) & ~(page - 1);
    for (uintptr_t at = 0x10000000; at < Limit && size <= Limit - at;) {
        MEMORY_BASIC_INFORMATION region;
        if (!VirtualQuery(reinterpret_cast<void *>(at), &region, sizeof region))
            break;
        uintptr_t base = reinterpret_cast<uintptr_t>(region.BaseAddress);
        uintptr_t candidate = (at + granularity - 1) & ~(granularity - 1);
        uintptr_t end = base + region.RegionSize;
        if (end < base || end <= at)
            break;
        if (region.State == MEM_FREE && candidate < Limit && size <= Limit - candidate &&
            candidate <= end && size <= end - candidate) {
            if (void *p = VirtualAlloc(reinterpret_cast<void *>(candidate), size,
                                      MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))
                return p;
        }
        at = end;
    }
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return nullptr;
}

bool memory_release(void *address) { return VirtualFree(address, 0, MEM_RELEASE) != 0; }
uint32_t memory_error() { return GetLastError(); }
}
#endif
