// An AMOS program started to edit a program file, and the file the system started AMOS with.
// (System headers only: the NDK's timeval clashes with the C++ library's, and with spawn.h's.)

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
#include <spawn.h>
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
extern char **environ;
#endif

namespace amos::host {

// The AMOS program (the executable that runs) started again to edit a program file: an open
// request of the system (Finder's double click) comes to the running AMOS, which has its editor
// full already, so the file gets an AMOS of its own, as it does when the system starts one
bool open_in_new_instance(const char *path)
{
#if defined(_WIN32)
    (void)path;
    return false;
#else
    char exe[4096];
#if defined(__APPLE__)
    uint32_t size = sizeof exe;
    if (_NSGetExecutablePath(exe, &size) != 0)
        return false;
#else
    ssize_t n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    if (n <= 0)
        return false;
    exe[n] = 0;
#endif
    char option[] = "--open-document";
    char *argv[] = {exe, option, const_cast<char *>(path), nullptr};
    pid_t pid;
    return posix_spawn(&pid, exe, nullptr, nullptr, argv, environ) == 0;
#endif
}

} // namespace amos::host
