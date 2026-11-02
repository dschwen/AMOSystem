// Native Windows bring-up gate. No SDL, NDK, or packed interpreter types.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "host/windows_memory.h"

int native_probe();
int backend_probe(void *main_fiber);

namespace {
using namespace amos::host::windows;
constexpr uintptr_t Limit = uintptr_t(1) << 32;
int failures;
int global;
void *caller;
void *task;
int phase;
bool low_fiber_stack = true;

void check(bool ok, const char *name)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", name);
    if (!ok)
        failures++;
}

bool low_range(const void *p, size_t size)
{
    uintptr_t at = reinterpret_cast<uintptr_t>(p);
    return at && at < Limit && size <= Limit - at;
}

void address(const char *name, const void *p)
{
    printf("INFO %s=%p (%s 4GB)\n", name, p, low_range(p, 1) ? "below" : "above");
}

VOID WINAPI fiber(void *)
{
    volatile uint64_t preserved = 0x123456789abcdef0ull;
    alignas(16) jmp_buf recovery;
    address("fiber stack local", const_cast<const uint64_t *>(&preserved));
    address("fiber setjmp buffer", &recovery);
    low_fiber_stack = low_range(const_cast<const uint64_t *>(&preserved), sizeof preserved) &&
                      low_range(&recovery, sizeof recovery);
    // Exercise repeated same-thread yields and error recovery on that stack.
    for (int i = 0; i < 3; i++) {
        int returned = setjmp(recovery);
        if (!returned)
            longjmp(recovery, 7);
        check(returned == 7, "setjmp/longjmp on fiber");
        check(preserved == 0x123456789abcdef0ull, "fiber stack local survives recovery");
        phase++;
        SwitchToFiber(caller);
    }
    phase++;
    SwitchToFiber(caller);
    // Returning from a Windows fiber would terminate its thread.
    abort();
}

void memory_probe()
{
    constexpr size_t Size = 512ull << 20;
    void *arena = memory_allocate(Size, true);
    address("512MB arena", arena);
    check(low_range(arena, Size), "entire arena fits below 4GB");
    if (arena) {
        auto *bytes = static_cast<volatile unsigned char *>(arena);
        check(bytes[0] == 0 && bytes[Size - 1] == 0, "mapping is zero initialized");
        bytes[0] = 0x12;
        bytes[Size - 1] = 0x34;
        check(bytes[0] == 0x12 && bytes[Size - 1] == 0x34, "arena endpoints writable");
        check(memory_allocate(Size, true, arena) == nullptr &&
              bytes[0] == 0x12 && bytes[Size - 1] == 0x34,
              "occupied arena is not replaced or modified");
    } else {
        printf("INFO arena allocation Windows error=%lu\n", static_cast<unsigned long>(memory_error()));
    }
    void *stack = memory_allocate(8ull << 20, true);
    check(low_range(stack, 8ull << 20), "8MB candidate interpreter stack below 4GB");
    if (stack)
        check(memory_release(stack), "release candidate stack");

    void *requested = reinterpret_cast<void *>(uintptr_t(1) << 20);
    void *chip = memory_allocate(3ull << 20, true, requested);
    if (chip) {
        check(chip == requested, "exact chip mapping at 1MB");
        check(memory_allocate(3ull << 20, true, requested) == nullptr,
              "occupied exact mapping is not replaced");
        check(memory_release(chip), "release chip mapping");
    } else {
        // The existing allocator explicitly supports chip memory in the arena.
        puts("INFO 1MB chip address occupied; arena fallback required");
    }
    check(memory_allocate(8192, true, reinterpret_cast<void *>(Limit - 4096)) == nullptr,
          "reject range crossing 4GB");
    check(memory_allocate(0, true) == nullptr, "reject zero size");
    check(memory_allocate(SIZE_MAX, true) == nullptr, "reject oversized arena");
    if (arena)
        check(memory_release(arena), "release arena");
}
}

int main()
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    puts("AMOS Windows x86-64 address/context probe");
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    printf("INFO page=%lu allocation-granularity=%lu pointer=%zu jmp_buf=%zu alignment=%zu\n",
           static_cast<unsigned long>(info.dwPageSize),
           static_cast<unsigned long>(info.dwAllocationGranularity),
           sizeof(void *), sizeof(jmp_buf), alignof(jmp_buf));
    address("executable", GetModuleHandleW(nullptr));
    address("global", &global);
    address("main stack", &info);
    check(low_range(GetModuleHandleW(nullptr), 1) && low_range(&global, sizeof global),
          "executable and global data below 4GB");
    memory_probe();
    failures += native_probe();

    caller = ConvertThreadToFiberEx(nullptr, FIBER_FLAG_FLOAT_SWITCH);
    check(caller != nullptr, "convert main thread to fiber");
    if (caller) {
        failures += backend_probe(caller);
        task = CreateFiberEx(256 * 1024, 256 * 1024, FIBER_FLAG_FLOAT_SWITCH, fiber, nullptr);
        check(task != nullptr, "create cooperative context");
        if (task) {
            for (int i = 0; i < 4; i++) {
                SwitchToFiber(task);
                check(phase == i + 1, "cooperative context yields to main thread");
            }
            check(low_fiber_stack, "system fiber stack fits AMOS 32-bit address model");
            if (!low_fiber_stack)
                puts("INFO native fibers require replacement with caller-supplied low stacks or guest addresses");
            DeleteFiber(task);
        }
        check(ConvertFiberToThread() != 0, "restore main thread");
    }
    printf("RESULT %d failure(s); no Windows host support claim until this gate and ABI tests pass\n", failures);
    return failures ? 1 : 0;
}
