// The other tasks of AMOS (the file selector's dialog) on another host:
// coroutines on stacks below 4GB, each with a context of its own
// (core/amos.h ctx()). A task runs while the program waits (the idle loop,
// the wait for a frame, a delay: host::run_tasks) and gives the processor
// back where it waits itself (host::task_yield); there is no pre-emption.

#include "hal/system.h"
#include "runtime/runtime.h"

#include <exec/tasks.h>

#include <ucontext.h>

using namespace amos;

namespace amos::host {

namespace {

struct HostTask {
    ucontext_t context, caller;
    Ctx registers;              // the task's interpreter registers
    void (*entry)();
    bool done;
};

constexpr int MaxTasks = 8;
constexpr uint32_t StackSize = 256 * 1024;
HostTask tasks[MaxTasks];
int task_count;
HostTask *running;              // the task that runs, or null: the program

void trampoline()
{
    running->entry();
    running->done = true;
    swapcontext(&running->context, &running->caller);
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
        swapcontext(&t.caller, &t.context);
        running = nullptr;
        t.registers = ctx();
        ctx() = saved;
    }
}

// A task waits: the program goes on
void task_yield()
{
    if (running)
        swapcontext(&running->context, &running->caller);
}

bool in_task() { return running != nullptr; }

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
    void *stack = hal::allocate(host::StackSize, 0);
    if (!stack)
        return;
    getcontext(&t.context);
    t.context.uc_stack.ss_sp = stack;
    t.context.uc_stack.ss_size = host::StackSize;
    t.context.uc_link = nullptr;
    makecontext(&t.context, host::trampoline, 0);
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
