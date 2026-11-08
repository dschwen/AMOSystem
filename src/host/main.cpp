// AMOS Professional on another host: the start (+B.s Cold_Start). AMOS
// keeps its addresses in 32 bits: its memory, and the stack it runs on,
// are below 4GB. The command line is the CLI's: [-c config] [-w]
// [program [its command line]].

#if defined(__APPLE__) && !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 700
#endif

#include "runtime/runtime.h"
#include "runtime/glue.h"
#include "hal/system.h"
#include "main/main.h"

#include <errno.h>
#if !defined(_WIN32)
#pragma pack(push,16)
#include <pthread.h>
#if defined(__APPLE__)
#include <pthread/qos.h>
#endif
#pragma pack(pop)
#else
#include "host/windows_platform.h"
#endif
#include <signal.h>
#if !defined(_WIN32)
#include <execinfo.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <mach/mach.h>
#endif
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "host/cxx.h"
#include "host/devices.h"
#include <sys/stat.h>
#if defined(__APPLE__)
#pragma pack(push,16)
#include <ucontext.h>
#pragma pack(pop)
#include <mach-o/dyld.h>
#endif
#if !defined(_WIN32)
#include "host/installed_state.h"
#endif

#include "host/windows_crt.h"

#if defined(_WIN32)
#define setenv amos::host::windows::environment_set
#define realpath(path, ignored) amos::host::windows::canonical_path(path)
#endif

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

#if defined(__APPLE__) || defined(_WIN32)
void amos_main_context()
{
    host::base_run(start);
}
#else
void *amos_thread(void *)
{
    host::base_run(start);
}
#endif

} // namespace

namespace amos::host {
void display_save(const char *path);
bool startup_document(unsigned milliseconds, char *document, size_t size);   // sdl.cpp: the program Finder opened us with
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

#if !defined(_WIN32)
// AMOS_CRASH_TRACE=1: a fatal signal writes the addresses of the stack (and where the executable
// is loaded, for atos/addr2line), where a debugger is not at hand (a CI runner)
void crash_trace(int signal, siginfo_t *info, void *context)
{
    char text[160];
    // (a trace that hangs is cut short by the alarm: the second time in is only the way out)
    static volatile sig_atomic_t entered;
    if (entered) _exit(128 + signal);
    entered = 1;
    alarm(5);
    int n = snprintf(text, sizeof text, "amos: signal %d at %p\n", signal, info ? info->si_addr : nullptr);
    if (n > 0) (void)!write(2, text, size_t(n));
#if defined(__APPLE__)
    n = snprintf(text, sizeof text, "amos: loaded at %p\n", static_cast<const void *>(_dyld_get_image_header(0)));
    if (n > 0) (void)!write(2, text, size_t(n));
#endif
#if defined(__APPLE__) && defined(__aarch64__)
    // (backtrace() does not go through the signal frame: the pc, and the frame records from the fp.
    // Each value is written as it is read, with no stdio: the last line shows how far this got)
    {
        auto put = [](const char *label, unsigned long long value) {
            char line[48];
            int k = 0;
            for (const char *c = "amos: "; *c; c++) line[k++] = *c;
            for (const char *c = label; *c; c++) line[k++] = *c;
            line[k++] = ' ';
            line[k++] = '0';
            line[k++] = 'x';
            for (int shift = 60; shift >= 0; shift -= 4) line[k++] = "0123456789abcdef"[(value >> shift) & 15];
            line[k++] = '\n';
            (void)!write(2, line, size_t(k));
        };
        put("ctx", (unsigned long long)(uintptr_t)context);
        // (the host is built packed, the system's ucontext_t is not: the registers are read at their
        // places in the arm64 macOS frame: uc_mcontext at 48, in it x0.. at 16, fp 248, lr 256, sp 264, pc 272)
        auto word = [](uintptr_t at) {
            unsigned long long v = 0;
            memcpy(&v, reinterpret_cast<const void *>(at), sizeof v);
            return v;
        };
        const uintptr_t mc = uintptr_t(word(uintptr_t(context) + 48));
        put("mcontext", mc);
        put("pc", word(mc + 272));
        put("lr", word(mc + 256));
        put("sp", word(mc + 264));
        put("x0", word(mc + 16));
        put("x1", word(mc + 24));
        uintptr_t fp = uintptr_t(word(mc + 248));
        for (int i = 0; i < 40 && fp && !(fp & 7); i++) {
            uintptr_t record[2];
            vm_size_t got = 0;
            // (read safely: a bad frame pointer must not fault in here)
            if (vm_read_overwrite(mach_task_self(), fp, sizeof record, reinterpret_cast<vm_address_t>(record), &got) != KERN_SUCCESS ||
                got != sizeof record)
                break;
            put("frame", record[1]);
            fp = record[0];
        }
    }
#else
    (void)context;
#endif
#if !defined(__APPLE__)
    void *frames[48];
    int count = backtrace(frames, 48);
    backtrace_symbols_fd(frames, count, 2);
#endif
    _exit(128 + signal);
}
void crash_trace_start()
{
    if (!getenv("AMOS_CRASH_TRACE")) return;
    struct sigaction action = {};
    action.sa_sigaction = crash_trace;
    action.sa_flags = SA_SIGINFO;
    for (int signal : {SIGSEGV, SIGBUS, SIGILL, SIGABRT, SIGTRAP, SIGFPE, SIGALRM})
        sigaction(signal, &action, nullptr);
    int seconds = atoi(getenv("AMOS_CRASH_TRACE"));
    alarm(seconds > 1 ? unsigned(seconds) : 60); // (a program that hangs says where, instead of the job timing out)
}
#endif

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
#if defined(_WIN32)
    char *resolved = host::windows::executable_path();
    ssize_t n = resolved ? ssize_t(strlen(resolved)) : -1;
    if (n >= ssize_t(sizeof exe)) n = -1;
    if (n > 0) memcpy(exe, resolved, size_t(n));
    free(resolved);
#elif defined(__APPLE__)
    uint32_t size = sizeof exe;
    ssize_t n = _NSGetExecutablePath(exe, &size) == 0 ? ssize_t(strlen(exe)) : -1;
#else
    ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
#endif
    if (n > 0) {
        exe[n] = 0;
        std::string dir = exe;
        dir.erase(dir.rfind('/'));
#if defined(__APPLE__)
        const char *locations[] = {"/../Resources/AMOS", "/../share/amos/AMOS", "/../dist/AMOS", "/AMOS", "/../../AMOS"};
#elif defined(_WIN32)
        const char *locations[] = {"/AMOS", "/../share/amos/AMOS", "/../dist/AMOS", "/../../AMOS"};
#else
        const char *locations[] = {"/../share/amos/AMOS", "/../dist/AMOS", "/AMOS", "/../../AMOS"};
#endif
        for (const char *relative : locations) {
            std::string path = dir + relative;
            if (is_directory(path + "/APSystem"))
                return path;
        }
    }
    return "";
}

} // namespace

#if defined(_WIN32)
extern "C" int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
#if defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__)) && !defined(AMOS_GUEST_BIASED)
    // Cross-linking is useful before the guest-address port, but never allow
    // an ordinary arm64 image to truncate its function/table pointers and crash.
    if (reinterpret_cast<uintptr_t>(&find_home) > UINT32_MAX ||
        reinterpret_cast<uintptr_t>(command) > UINT32_MAX) {
        fprintf(stderr, "amos: experimental arm64 image exceeds the 32-bit address range; native execution requires the guest-address port. Run the standalone macOS probes first.\n");
        return 1;
    }
#endif
#if defined(_WIN32) && !defined(AMOS_GUEST_BIASED)
    if (!host::windows::image_below_4gb()) {
        fprintf(stderr, "amos: Windows executable exceeds 32-bit address range\n");
        return 1;
    }
#endif
#if !defined(_WIN32)
    crash_trace_start();
#if defined(__APPLE__)
    // (AMOS keeps time in frames of 20 ms; macOS lets the timers of a process it thinks is in the
    // background run late, by more than a frame, and the frames missed are not made up: the interpreter
    // runs at the user's interactive priority, as a game does)
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
#endif
#endif
    // where AMOS was started: the volume START:, for the programs named on the command line (the
    // devices directory too, if it has one there); AMOS itself runs in its tree
    char *start = getcwd(nullptr, 0);
#if defined(_WIN32)
    if (start) for (char *p = start; *p; p++) if (*p == '\\') *p = '/';
#endif
    std::string home = find_home();
#if defined(_WIN32)
    if (!home.empty()) {
        char *working = host::windows::prepare_installed_state(home.c_str());
        if (!working) {
            fprintf(stderr, "amos: could not prepare per-user configuration\n");
            free(start);
            return 1;
        }
        home = working;
        free(working);
    }
#endif
#if defined(__APPLE__)
    // Bundled defaults remain immutable, including when launched from a DMG.
    if (!(getenv("AMOS_HOME") && *getenv("AMOS_HOME")) &&
        home.ends_with("/Resources/AMOS")) {
        char *working = host::prepare_macos_state(home.c_str());
        if (!working) { fprintf(stderr, "amos: could not prepare per-user macOS resources\n"); free(start); return 1; }
        home = working; free(working);
    }
#endif
#if !defined(_WIN32) && !defined(__APPLE__)
    // An installed package (/usr/share/amos/AMOS) is read-only: run in a per-user copy of it.
    // A tree that can be written to (a build, ~/.local, AMOS_HOME) runs where it is.
    if (!home.empty() && !(getenv("AMOS_HOME") && *getenv("AMOS_HOME")) && access(home.c_str(), W_OK) != 0) {
        char *working = host::prepare_linux_state(home.c_str());
        if (!working) { fprintf(stderr, "amos: could not prepare the per-user AMOS tree\n"); free(start); return 1; }
        home = working; free(working);
    }
#endif
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
#if defined(_WIN32)
    for (char &c : self) {
        if (c == '\\') c = '/';
        if (c >= 'A' && c <= 'Z') c = char(c + ('a' - 'A'));
    }
    if (self.size() > 4 && self.substr(self.size() - 4) == ".exe") self.resize(self.size() - 4);
#endif
    if (self.substr(self.rfind('/') + 1) == "amos-player") {
        memcpy(p, "-r ", 3);
        p += 3;
    }
    std::string open_document;
    bool first = true;
    std::vector<std::string> args(argv + 1, argv + argc);
#if defined(__APPLE__)
    {
        // Finder starts an application with no arguments and tells it its document by an event
        // (the arguments of a double click are none, or a process serial number of old systems)
        bool plain = true;
        for (const std::string &a : args)
            plain = plain && a.compare(0, 5, "-psn_") == 0;
        if (plain && self.find(".app/Contents/MacOS/") != std::string::npos) {
            char document[4096];
            if (host::startup_document(250, document, sizeof document))
                args = {"--open-document", document};
        }
    }
#endif
    const int count = int(args.size());
    for (int i = 0; i < count; i++) {
        std::string arg = args[i];
#if defined(_WIN32)
        for (char &c : arg) if (c == '\\') c = '/';
#endif
        if (arg == "--open-document") {
            if (!open_document.empty() || i + 1 >= count) {
                fprintf(stderr, "amos: --open-document requires exactly one file\n");
                return 1;
            }
            std::string requested = args[++i];
#if defined(_WIN32)
            for (char &c : requested) if (c == '\\') c = '/';
#endif
            bool absolute = !requested.empty() && requested[0] == '/';
#if defined(_WIN32)
            absolute = absolute || (requested.size() > 2 && requested[1] == ':' && requested[2] == '/');
#endif
            if (!absolute) requested = start_dir + "/" + requested;
            char *path = realpath(requested.c_str(), nullptr);
            if (!path) { perror("amos: open document"); return 1; }
            std::string native = path;
            free(path);
            const size_t slash = native.rfind('/');
            if (slash == std::string::npos || native.size() - slash - 1 > sizeof(Program::Prg_NamePrg) - sizeof("DOCUMENT:")) {
                fprintf(stderr, "amos: document filename exceeds AMOS's 128-byte program-name layout\n");
                return 1;
            }
            host::devices::option("assign", ("DOCUMENT=" + native.substr(0, slash + 1)).c_str());
            host::devices::set_program(native.c_str());
            open_document = "DOCUMENT:" + native.substr(slash + 1);
            continue;
        }
        // the host's own options: --df0 path[=label][,ro], --dh1..., --assign NAME=path,
        // --library path, --devices path, --config file (docs/DEVICES.md)
        if (arg.compare(0, 2, "--") == 0) {
            std::string name = arg.substr(2), value;
            size_t eq = name.find('=');
            if (eq != std::string::npos) {
                value = name.substr(eq + 1);
                name.resize(eq);
            } else if (name == "fullscreen") {
                value = "yes";              // (a flag: --fullscreen, or --fullscreen=no)
            } else if (i + 1 < count) {
                value = args[++i];
            }
            if (!host::devices::option(name.c_str(), value.c_str())) {
                fprintf(stderr, "amos: unknown option %s\n", arg.c_str());
                return 1;
            }
            continue;
        }
        bool absolute_host = arg[0] == '/';
#if defined(_WIN32)
        absolute_host = absolute_host || (arg.size() > 2 && arg[1] == ':' && arg[2] == '/');
#endif
        if (arg[0] != '-' && (arg.find(':') == std::string::npos || absolute_host)) {
            std::string absolute = absolute_host ? arg : start_dir + "/" + arg;
            struct stat st;
            if (stat(absolute.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                size_t slash = absolute.rfind('/');
                // (its profile, and its folder a disk of its own name)
                if (char *real = realpath(absolute.c_str(), nullptr)) {
                    std::string program = real;
                    free(real);
                    host::devices::set_program(program.c_str());
                    setenv("AMOS_PROGRAM_DIR", program.substr(0, program.rfind('/')).c_str(), 1);
                }
                if (absolute_host) {
                    start_dir = absolute.substr(0, slash ? slash : 1);
                    arg = absolute.substr(slash + 1);
                }
                arg = "START:" + arg;
            }
        }
        size_t n = arg.size();
        if (p + n + 2 >= command + sizeof command)
            break;
        if (!first)
            *p++ = ' ';
        first = false;
        memcpy(p, arg.data(), n);
        p += n;
    }
    if (!open_document.empty()) {
        if (!first || self.substr(self.rfind('/') + 1) == "amos-player") {
            fprintf(stderr, "amos: --open-document cannot be combined with program execution\n");
            return 1;
        }
        boot::open_document_name = open_document.c_str();
    }
    setenv("AMOS_START_DIR", start_dir.c_str(), 1);
    *p++ = '\n';
    command_length = uint32_t(p - command);

    host::glue_start();
    // (address 4: ExecBase)
    host::put(amos_low_page + 4, 4, hal::exec_base());

    // AMOS's stack, below 4GB
#if defined(_WIN32)
    if (!host::windows::run_main(amos_main_context)) {
        fprintf(stderr, "amos: could not start Windows interpreter fiber\n");
        return 1;
    }
#else
    constexpr size_t StackSize = 8 * 1024 * 1024;
    void *stack = hal::allocate(StackSize, 0);
    if (!stack) {
        fprintf(stderr, "amos: no memory for the stack\n");
        return 1;
    }
#if defined(__APPLE__)
    // Cocoa requires SDL video on the main thread. Switch only its stack:
    // AMOS still needs stack addresses representable in 32 bits.
    ucontext_t caller, interpreter;
    if (getcontext(&interpreter) != 0) {
        perror("amos: getcontext");
        return 1;
    }
    interpreter.uc_stack.ss_sp = stack;
    interpreter.uc_stack.ss_size = StackSize;
    interpreter.uc_stack.ss_flags = 0;
    interpreter.uc_link = &caller;
    makecontext(&interpreter, amos_main_context, 0);
    if (swapcontext(&caller, &interpreter) != 0) {
        perror("amos: swapcontext");
        return 1;
    }
#else
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
#endif
#endif
    return 0;
}
