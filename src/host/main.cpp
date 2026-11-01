// AMOS Professional on another host: the start (+B.s Cold_Start). AMOS
// keeps its addresses in 32 bits: its memory, and the stack it runs on,
// are below 4GB. The command line is the CLI's: [-c config] [-w]
// [program [its command line]].

#include "runtime/runtime.h"
#include "runtime/glue.h"
#include "hal/system.h"

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>

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

namespace amos::boot {
int exit_status();
}

// The way out (AMOS_SHOT: the last frame into that file); the status 1 if
// AMOS stopped with a message (an error of a program run only, say)
extern "C" [[noreturn]] void main_get_out(uint32_t)
{
    if (const char *shot = getenv("AMOS_SHOT"); shot && !getenv("AMOS_SHOT_FRAME"))
        host::display_save(shot);
    exit(amos::boot::exit_status());
}

namespace {

bool is_directory(const std::string &path)
{
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

// The AMOS tree (APSystem, the examples...): AMOS_HOME, else where the program was installed
// (../share/amos/AMOS) or built (../dist/AMOS next to build/host), found from the program itself,
// so that it does not matter where it is started
std::string find_home()
{
    if (const char *home = getenv("AMOS_HOME"); home && *home)
        return home;
    char exe[4096];
    ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    if (n > 0) {
        exe[n] = 0;
        std::string dir = exe;
        dir.erase(dir.rfind('/'));
        for (const char *relative : {"/../share/amos/AMOS", "/../dist/AMOS", "/AMOS"}) {
            std::string path = dir + relative;
            if (is_directory(path + "/APSystem"))
                return path;
        }
    }
    return "";
}

} // namespace

int main(int argc, char **argv)
{
    // where AMOS was started: the volume START:, for the programs named on the command line (the
    // devices directory too, if it has one there); AMOS itself runs in its tree
    char *start = getcwd(nullptr, 0);
    std::string home = find_home();
    if (home.empty()) {
        fprintf(stderr, "amos: no AMOS tree (APSystem...): install it, or set AMOS_HOME\n");
        return 1;
    }
    if (chdir(home.c_str()) != 0) {
        fprintf(stderr, "amos: %s: %s\n", home.c_str(), strerror(errno));
        return 1;
    }
    setenv("AMOS_HOME", home.c_str(), 0);
    std::string start_dir = start ? start : ".";
    free(start);

    // the CLI's command line: the arguments, a newline. A program of the host's file system
    // (prog.AMOS, ../x/prog.AMOS, /home/me/prog.AMOS) is START:prog.AMOS
    char *p = command;
    // amos-player: AMOSPro's -r, the program run only (no editor, no
    // direct mode: AMOS stops when it does)
    std::string self = argv[0];
    if (self.substr(self.rfind('/') + 1) == "amos-player") {
        memcpy(p, "-r ", 3);
        p += 3;
    }
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] != '-' && arg.find(':') == std::string::npos) {
            std::string absolute = arg[0] == '/' ? arg : start_dir + "/" + arg;
            struct stat st;
            if (stat(absolute.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                size_t slash = absolute.rfind('/');
                if (arg[0] == '/') {
                    start_dir = absolute.substr(0, slash ? slash : 1);
                    arg = absolute.substr(slash + 1);
                }
                arg = "START:" + arg;
            }
        }
        size_t n = arg.size();
        if (p + n + 2 >= command + sizeof command)
            break;
        if (i > 1)
            *p++ = ' ';
        memcpy(p, arg.data(), n);
        p += n;
    }
    setenv("AMOS_START_DIR", start_dir.c_str(), 1);
    *p++ = '\n';
    command_length = uint32_t(p - command);

    amos_bridge = amos_task;
    host::glue_start();
    // (address 4: ExecBase)
    host::put(amos_low_page + 4, 4, hal::exec_base());

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
