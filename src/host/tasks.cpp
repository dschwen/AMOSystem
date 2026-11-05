// The other tasks of AMOS (the file selector's dialog) on another host:
// coroutines on stacks below 4GB, each with a context of its own
// (core/amos.h ctx()). A task runs while the program waits (the idle loop,
// the wait for a frame, a delay: host::run_tasks) and gives the processor
// back where it waits itself (host::task_yield); there is no pre-emption.

// macOS exposes the ucontext routines only when X/Open interfaces are enabled.
// Feature-test macros must precede all headers, including project headers.
#if defined(__APPLE__) && !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif

#include "hal/system.h"
#include "runtime/runtime.h"

#include <exec/tasks.h>

#if defined(_WIN32)
#include "host/windows_platform.h"
#else
#pragma pack(push,16)
#include <ucontext.h>
#pragma pack(pop)
#endif

using namespace amos;

namespace amos::host {

namespace {

#pragma pack(push,16)
struct alignas(16) HostTask {
#if defined(_WIN32)
    void *context, *caller;
#else
    ucontext_t context, caller;
#endif
    Ctx registers;              // the task's interpreter registers
    void (*entry)();
    bool done;
};
#pragma pack(pop)

constexpr int MaxTasks = 8;
constexpr uint32_t StackSize = 256 * 1024;
HostTask tasks[MaxTasks];
int task_count;
HostTask *running;              // the task that runs, or null: the program

void trampoline()
{
    running->entry();
    running->done = true;
#if defined(_WIN32)
    windows::fiber_switch(running->caller);
#else
    swapcontext(&running->context, &running->caller);
#endif
}

} // namespace

// The tasks run, each until it waits
void run_tasks()
{
    if (running)
        return;
    for (int i = 0; i < task_count; i++) {
        HostTask &t = tasks[i];
        if (t.done)
            continue;
        Ctx saved = ctx();
        ctx() = t.registers;
        running = &t;
#if defined(_WIN32)
        t.caller = windows::fiber_current();
        windows::fiber_switch(t.context);
        if (t.done) {
            windows::fiber_delete(t.context);
            t.context = nullptr;
        }
#else
        swapcontext(&t.caller, &t.context);
#endif
        running = nullptr;
        t.registers = ctx();
        ctx() = saved;
    }
}

// A task waits: the program goes on
void task_yield()
{
    if (running) {
#if defined(_WIN32)
        windows::fiber_switch(running->caller);
#else
        swapcontext(&running->context, &running->caller);
#endif
    }
}

bool in_task() { return running != nullptr; }
// Stable coroutine identity, not an address; zero denotes the main task.
uint32_t recovery_task_id() { return running ? uint32_t(running - tasks) + 1 : 0; }

} // namespace amos::host

namespace amos::hal {

// AddTask: its entry is a function of the C++ (the start of the file
// selector's task), on a stack of its own
void add_task(Task *, void *entry, void *)
{
    if (host::task_count >= host::MaxTasks)
        return;
    host::HostTask &t = host::tasks[host::task_count];
    t = {};
    t.entry = reinterpret_cast<void (*)()>(entry);
    t.registers = {};
#if defined(_WIN32)
    t.context = host::windows::fiber_create(host::StackSize, host::trampoline);
    if (!t.context)
        return;
#else
    void *stack = hal::allocate(host::StackSize, 0);
    if (!stack)
        return;
    getcontext(&t.context);
    t.context.uc_stack.ss_sp = stack;
    t.context.uc_stack.ss_size = host::StackSize;
    t.context.uc_link = nullptr;
    makecontext(&t.context, host::trampoline, 0);
#endif
    host::task_count++;
}

} // namespace amos::hal

// The file selector's task (lib/fsel.cpp): a5 its data zone, a context of
// its own
extern "C" {
extern uint32_t lib_fs_task_a5;
void lib_fs_task();
void lib_fs_task_entry()
{
    Ctx &c = ctx();
    c = Ctx{};
    c.a5 = ptr<uint8_t>(lib_fs_task_a5);
    lib_fs_task();
}
}
