// hal/files.h on another host: POSIX. Files are their descriptors + 1
// (0: none); a lock is the number of its name in a table of names.

#include "hal/files.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dos/dos.h>

namespace {

// dos/dosextens.h's (it clashes with the host's timeval)
constexpr int32_t ST_USERDIR = 2, ST_FILE = -3;

int error;      // the last error, as AmigaDOS's

int32_t dos_error(int e)
{
    switch (e) {
    case ENOENT: return ERROR_OBJECT_NOT_FOUND;
    case EEXIST: return ERROR_OBJECT_EXISTS;
    case EACCES: return ERROR_READ_PROTECTED;
    case ENOSPC: return ERROR_DISK_FULL;
    case ENOTEMPTY: return ERROR_DIRECTORY_NOT_EMPTY;
    default: return ERROR_OBJECT_WRONG_TYPE;
    }
}

// AmigaDOS's names: "volume:dir/file"; here the volume is a directory of
// the current one, and "/" alone goes up
const char *host_name(const char *name)
{
    static char buffer[1024];
    char *to = buffer;
    const char *colon = strchr(name, ':');
    if (colon) {
        size_t n = size_t(colon - name);
        memcpy(to, name, n);
        to += n;
        *to++ = '/';
        name = colon + 1;
    }
    for (; *name && to < buffer + sizeof buffer - 4; name++) {
        if (*name == '/' && (to == buffer || to[-1] == '/')) {
            *to++ = '.';
            *to++ = '.';
        }
        *to++ = *name;
    }
    *to = 0;
    return buffer[0] ? buffer : ".";
}

struct Lock {
    char *name;
    DIR *dir;
};
Lock locks[256];

} // namespace

namespace amos::hal {

void files_start(void *) {}

File open(const char *name, int32_t mode)
{
    int flags = mode == ModeNewFile ? O_RDWR | O_CREAT | O_TRUNC : mode == ModeReadWrite ? O_RDWR | O_CREAT : O_RDONLY;
    int fd = ::open(host_name(name), flags, 0644);
    if (fd < 0) {
        error = dos_error(errno);
        return 0;
    }
    return File(fd + 1);
}

void close(File f)
{
    if (f)
        ::close(int(f) - 1);
}

int32_t read(File f, void *buffer, int32_t length)
{
    ssize_t n = ::read(int(f) - 1, buffer, size_t(length));
    if (n < 0)
        error = dos_error(errno);
    return int32_t(n);
}

int32_t write(File f, const void *buffer, int32_t length)
{
    ssize_t n = ::write(int(f) - 1, buffer, size_t(length));
    if (n < 0)
        error = dos_error(errno);
    return int32_t(n);
}

int32_t seek(File f, int32_t offset, int32_t origin)
{
    int fd = int(f) - 1;
    off_t old = lseek(fd, 0, SEEK_CUR);
    int whence = origin == SeekBeginning ? SEEK_SET : origin == SeekEnd ? SEEK_END : SEEK_CUR;
    if (old < 0 || lseek(fd, offset, whence) < 0) {
        error = dos_error(errno);
        return -1;
    }
    return int32_t(old);
}

bool wait_for_char(File, int32_t) { return false; }
File output() { return 2; }         // stdout
int32_t io_error() { return error; }

DirLock lock(const char *name, int32_t)
{
    const char *h = host_name(name);
    struct stat st;
    if (stat(h, &st) < 0) {
        error = dos_error(errno);
        return 0;
    }
    for (int i = 1; i < 256; i++) {
        if (!locks[i].name) {
            locks[i].name = strdup(h);
            locks[i].dir = nullptr;
            return DirLock(i);
        }
    }
    error = ERROR_NO_FREE_STORE;
    return 0;
}

void unlock(DirLock l)
{
    if (l && l < 256 && locks[l].name) {
        if (locks[l].dir)
            closedir(locks[l].dir);
        free(locks[l].name);
        locks[l] = {};
    }
}

namespace {

void fill(FileInfoBlock *fib, const char *path, const char *name)
{
    struct stat st;
    memset(fib, 0, sizeof *fib);
    if (stat(path, &st) == 0) {
        fib->fib_DirEntryType = S_ISDIR(st.st_mode) ? ST_USERDIR : ST_FILE;
        fib->fib_EntryType = fib->fib_DirEntryType;
        fib->fib_Size = int32_t(st.st_size);
        fib->fib_NumBlocks = int32_t((st.st_size + 511) / 512);
    }
    strncpy(reinterpret_cast<char *>(fib->fib_FileName), name, sizeof fib->fib_FileName - 1);
}

} // namespace

bool examine(DirLock l, void *fib)
{
    if (!l || l >= 256 || !locks[l].name)
        return false;
    const char *name = strrchr(locks[l].name, '/');
    fill(static_cast<FileInfoBlock *>(fib), locks[l].name, name ? name + 1 : locks[l].name);
    if (locks[l].dir)
        closedir(locks[l].dir);
    locks[l].dir = opendir(locks[l].name);
    return true;
}

bool examine_next(DirLock l, void *fib)
{
    if (!l || l >= 256 || !locks[l].dir)
        return false;
    while (struct dirent *e = readdir(locks[l].dir)) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
            continue;
        char path[1200];
        snprintf(path, sizeof path, "%s/%s", locks[l].name, e->d_name);
        fill(static_cast<FileInfoBlock *>(fib), path, e->d_name);
        return true;
    }
    error = ERROR_NO_MORE_ENTRIES;
    return false;
}

DirLock current_dir(DirLock l)
{
    if (l && l < 256 && locks[l].name && chdir(locks[l].name) < 0)
        error = dos_error(errno);
    return 0;
}

DirLock parent_dir(DirLock l)
{
    if (!l || l >= 256 || !locks[l].name)
        return 0;
    char path[1200];
    snprintf(path, sizeof path, "%s/..", locks[l].name);
    char *real = realpath(path, nullptr);
    if (!real || !strcmp(real, "/")) {
        free(real);
        return 0;
    }
    DirLock p = lock(real, LockShared);
    free(real);
    return p;
}

bool info(DirLock, void *) { return false; }
bool assign_lock(const char *, DirLock) { return false; }
bool device_exists(const char *) { return false; }

bool delete_file(const char *name)
{
    const char *h = host_name(name);
    if (::unlink(h) == 0 || ::rmdir(h) == 0)
        return true;
    error = dos_error(errno);
    return false;
}

bool rename(const char *from, const char *to)
{
    char f[1024];
    strncpy(f, host_name(from), sizeof f - 1);
    f[sizeof f - 1] = 0;
    if (::rename(f, host_name(to)) == 0)
        return true;
    error = dos_error(errno);
    return false;
}

DirLock create_dir(const char *name)
{
    if (mkdir(host_name(name), 0755) < 0) {
        error = dos_error(errno);
        return 0;
    }
    return lock(name, LockShared);
}

bool execute(const char *, File, File) { return false; }
void delay(int32_t ticks) { usleep(useconds_t(ticks) * 20000); }

} // namespace amos::hal
