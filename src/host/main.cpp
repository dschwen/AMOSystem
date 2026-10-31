// AMOS Professional on another host: the start (+B.s Cold_Start). AMOS
// keeps its addresses in 32 bits: its memory, and the stack it runs on,
// are below 4GB. The command line is the CLI's: [-c config] [-w]
// [program [its command line]].

#include "runtime/runtime.h"
#include "runtime/glue.h"
#include "hal/system.h"

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

using namespace amos;

extern "C" {
// main/boot.cpp
void main_cold_start(char *command, uint32_t length, uint32_t sp);
[[noreturn]] void main_init_fin();
}

namespace amos::host {
[[noreturn]] void base_run(void (*start)(uint32_t base));
}

namespace {

char command[1024];
uint32_t command_length;

void start(uint32_t base)
{
    main_cold_start(command, command_length, base);
    main_init_fin();
}

void *amos_thread(void *)
{
    host::base_run(start);
}

} // namespace

namespace amos::host {
void display_save(const char *path);
}

// The way out (AMOS_SHOT: the last frame into that file)
extern "C" [[noreturn]] void main_get_out(uint32_t)
{
    if (const char *shot = getenv("AMOS_SHOT"); shot && !getenv("AMOS_SHOT_FRAME"))
        host::display_save(shot);
    exit(0);
}

int main(int argc, char **argv)
{
    // the CLI's command line: the arguments, a newline
    char *p = command;
    for (int i = 1; i < argc; i++) {
        size_t n = strlen(argv[i]);
        if (p + n + 2 >= command + sizeof command)
            break;
        if (i > 1)
            *p++ = ' ';
        memcpy(p, argv[i], n);
        p += n;
    }
    *p++ = '\n';
    command_length = uint32_t(p - command);

    amos_bridge = amos_task;
    host::glue_start();

    // AMOS's stack, below 4GB
    constexpr size_t StackSize = 8 * 1024 * 1024;
    void *stack = hal::allocate(StackSize, 0);
    if (!stack) {
        fprintf(stderr, "amos: no memory for the stack\n");
        return 1;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstack(&attr, stack, StackSize);
    pthread_t thread;
    if (pthread_create(&thread, &attr, amos_thread, nullptr)) {
        fprintf(stderr, "amos: no thread\n");
        return 1;
    }
    // (debugging: AMOS_TRAP=seconds stops it there, for a debugger)
    if (const char *trap = getenv("AMOS_TRAP")) {
        sleep(unsigned(atoi(trap)));
        pthread_kill(thread, SIGTRAP);
    }
    pthread_join(thread, nullptr);
    return 0;
}
