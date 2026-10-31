// hal/files.h on another host: POSIX. Files are their descriptors + 1
// (0: none); a lock is the number of its name in a table of names.

#include "hal/files.h"

namespace amos::host {
void run_tasks();
void task_yield();
bool in_task();
}

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
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
    case EBUSY: return ERROR_OBJECT_IN_USE;
    case EACCES: return ERROR_READ_PROTECTED;
    case ENOSPC: return ERROR_DISK_FULL;
    case ENOTEMPTY: return ERROR_DIRECTORY_NOT_EMPTY;
    case EISDIR: return ERROR_OBJECT_WRONG_TYPE;
    default: return ERROR_OBJECT_WRONG_TYPE;
    }
}

// The volumes and assigns: "NAME=directory" pairs of AMOS_ASSIGNS
// (separated by ';'), and those made by assign_lock
struct Assign {
    char name[32];
    char *path;
    bool volume;        // a directory of the devices directory
};
Assign assigns[64];
int assign_count;

void scan_devices();

void read_assigns()
{
    static bool done;
    if (done)
        return;
    done = true;
    // the start directory a volume (AMOSPRO:, unless an assign is it), so
    // that the paths stay as short as an Amiga's
    char *cwd = getcwd(nullptr, 0);
    bool covered = false;
    const char *env = getenv("AMOS_ASSIGNS");
    while (env && *env && assign_count < 64) {
        const char *eq = strchr(env, '='), *end = strchr(env, ';');
        if (!end)
            end = env + strlen(env);
        if (eq && eq < end && size_t(eq - env) < sizeof assigns[0].name) {
            Assign &a = assigns[assign_count++];
            a.volume = false;
            memcpy(a.name, env, size_t(eq - env));
            a.name[eq - env] = 0;
            a.path = strndup(eq + 1, size_t(end - eq - 1));
        }
        env = *end ? end + 1 : end;
    }
    for (int i = 0; i < assign_count && cwd; i++) {
        char *a = realpath(assigns[i].path, nullptr);
        if (a && !strcmp(a, cwd))
            covered = true;
        free(a);
    }
    if (cwd && !covered && assign_count < 64) {
        Assign &a = assigns[assign_count++];
        a.volume = false;
        strcpy(a.name, "AMOSPRO");
        a.path = cwd;
        cwd = nullptr;
    }
    free(cwd);
    scan_devices();
}

// The devices directory: each directory in it is a volume, the directories
// and files below it those of the volume (AMOS_DEVICES, or "devices" in
// the directory AMOS starts in). A volume made there appears at once. It
// comes with RAM: (and a first disk, Work:) when it is new.
char *devices_root;

void devices_open()
{
    if (devices_root)
        return;
    const char *env = getenv("AMOS_DEVICES");
    char path[1024];
    if (env && *env) {
        snprintf(path, sizeof path, "%s", env);
    } else {
        char *cwd = getcwd(nullptr, 0);
        snprintf(path, sizeof path, "%s/devices", cwd ? cwd : ".");
        free(cwd);
    }
    bool fresh = mkdir(path, 0777) == 0;
    devices_root = realpath(path, nullptr);
    if (!devices_root)
        return;
    char volume[1100];
    snprintf(volume, sizeof volume, "%s/RAM", devices_root);
    mkdir(volume, 0777);
    if (fresh) {
        snprintf(volume, sizeof volume, "%s/Work", devices_root);
        mkdir(volume, 0777);
    }
}

// The directories of the devices directory that are not assigns yet
void scan_devices()
{
    devices_open();
    if (!devices_root)
        return;
    DIR *d = opendir(devices_root);
    if (!d)
        return;
    while (struct dirent *e = readdir(d)) {
        if (e->d_name[0] == '.' || strlen(e->d_name) >= sizeof assigns[0].name || assign_count >= 64)
            continue;
        char path[1100];
        snprintf(path, sizeof path, "%s/%s", devices_root, e->d_name);
        struct stat st;
        if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode))
            continue;
        bool known = false;
        for (int i = 0; i < assign_count; i++)
            if (!strcasecmp(assigns[i].name, e->d_name))
                known = true;
        if (known)
            continue;
        Assign &a = assigns[assign_count++];
        strcpy(a.name, e->d_name);
        a.path = strdup(path);
        a.volume = true;
    }
    closedir(d);
}

const char *assigned(const char *volume, size_t n)
{
    read_assigns();
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < assign_count; i++)
            if (strlen(assigns[i].name) == n && !strncasecmp(assigns[i].name, volume, n))
                return assigns[i].path;
        // (a volume made since: in the devices directory)
        if (!pass)
            scan_devices();
    }
    // RAM:'s volume, "Ram Disk"
    if (n == 8 && !strncasecmp(volume, "Ram Disk", 8))
        return assigned("RAM", 3);
    return nullptr;
}

// The entry of directory dir named name, whatever its case (AmigaDOS's
// names are not case-sensitive): into to, false if there is none
bool find_entry(const char *dir, const char *name, size_t n, char *to, size_t size)
{
    DIR *d = opendir(*dir ? dir : ".");
    if (!d)
        return false;
    bool found = false;
    while (struct dirent *e = readdir(d)) {
        if (strlen(e->d_name) == n && !strncasecmp(e->d_name, name, n)) {
            snprintf(to, size, "%s", e->d_name);
            found = true;
            break;
        }
    }
    closedir(d);
    return found;
}

// AmigaDOS's names: "volume:dir/file" (the volume an assign, or a
// directory of the current one), "/" alone up a directory; each part
// found whatever its case
const char *host_name(const char *name)
{
    static char buffer[1024];
    char *to = buffer;
    *to = 0;
    const char *colon = strchr(name, ':');
    if (colon) {
        size_t n = size_t(colon - name);
        if (const char *path = assigned(name, n)) {
            snprintf(buffer, sizeof buffer, "%s", path);
        } else if (n) {
            // a directory of the current one, else of the root (the names
            // of the locks begin with the root's directory: "home:...")
            char entry[256];
            if (find_entry("", name, n, entry, sizeof entry))
                snprintf(buffer, sizeof buffer, "%s", entry);
            else if (find_entry("/", name, n, entry, sizeof entry))
                snprintf(buffer, sizeof buffer, "/%s", entry);
            else
                snprintf(buffer, sizeof buffer, "%.*s", int(n), name);
        } else {
            snprintf(buffer, sizeof buffer, "/");
        }
        to = buffer + strlen(buffer);
        name = colon + 1;
    }
    // the parts
    while (*name) {
        const char *end = strchr(name, '/');
        size_t n = end ? size_t(end - name) : strlen(name);
        if (to > buffer && to[-1] != '/' && to < buffer + sizeof buffer - 2)
            *to++ = '/';
        if (!n) {
            // "/": the parent
            if (to + 3 < buffer + sizeof buffer) {
                *to++ = '.';
                *to++ = '.';
            }
        } else {
            *to = 0;
            char entry[256];
            if (find_entry(buffer, name, n, entry, sizeof entry))
                snprintf(to, size_t(buffer + sizeof buffer - to), "%s", entry);
            else
                snprintf(to, size_t(buffer + sizeof buffer - to), "%.*s", int(n), name);
            to += strlen(to);
        }
        name += n;
        if (*name == '/')
            name++;
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

// The files open, as AmigaDOS locks them: a new file exclusively, an old
// one shared
struct Open {
    dev_t dev;
    ino_t ino;
    bool exclusive, used;
};
Open opened[1024];

File open(const char *name, int32_t mode)
{
    const char *h = host_name(name);
    struct stat st;
    if (stat(h, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            // (a directory is not a file: "object wrong type")
            error = ERROR_OBJECT_WRONG_TYPE;
            return 0;
        }
        for (const Open &o : opened)
            if (o.used && o.dev == st.st_dev && o.ino == st.st_ino && (o.exclusive || mode == ModeNewFile)) {
                error = ERROR_OBJECT_IN_USE;
                return 0;
            }
    }
    // (an old file can be written too; read-only if it cannot)
    int flags = mode == ModeNewFile ? O_RDWR | O_CREAT | O_TRUNC : mode == ModeReadWrite ? O_RDWR | O_CREAT : O_RDWR;
    int fd = ::open(h, flags, 0644);
    if (fd < 0 && mode == ModeOldFile && (errno == EACCES || errno == EROFS))
        fd = ::open(h, O_RDONLY);
    if (fd < 0) {
        error = dos_error(errno);
        return 0;
    }
    if (fd < 1024 && fstat(fd, &st) == 0)
        opened[fd] = {st.st_dev, st.st_ino, mode == ModeNewFile, true};
    error = 0;
    return File(fd + 1);
}

void close(File f)
{
    if (!f)
        return;
    int fd = int(f) - 1;
    if (fd < 1024)
        opened[fd].used = false;
    ::close(fd);
}

int32_t read(File f, void *buffer, int32_t length)
{
    ssize_t n = ::read(int(f) - 1, buffer, size_t(length));
    error = n < 0 ? dos_error(errno) : 0;
    return int32_t(n);
}

int32_t write(File f, const void *buffer, int32_t length)
{
    ssize_t n = ::write(int(f) - 1, buffer, size_t(length));
    error = n < 0 ? dos_error(errno) : 0;
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

} // namespace amos::hal

namespace {

// A lock on a path of the host (its real path)
amos::hal::DirLock lock_path(const char *h)
{
    char *real = realpath(h, nullptr);
    if (!real) {
        error = dos_error(errno);
        return 0;
    }
    for (int i = 1; i < 256; i++) {
        if (!locks[i].name) {
            locks[i].name = real;
            locks[i].dir = nullptr;
            error = 0;
            return amos::hal::DirLock(i);
        }
    }
    free(real);
    error = ERROR_NO_FREE_STORE;
    return 0;
}

// The volumes Info gave (InfoData's id_VolumeNode: 1 + the index)
char *volumes[64];
int volume_count;

// The volume a directory is the top of: the assign of that directory
// (RAM: is "Ram Disk", as on the Amiga), or null
const char *volume_of(const char *real)
{
    read_assigns();
    for (int i = 0; i < assign_count; i++) {
        char *a = realpath(assigns[i].path, nullptr);
        bool same = a && !strcmp(a, real);
        free(a);
        if (same)
            return strcasecmp(assigns[i].name, "RAM") ? assigns[i].name : "Ram Disk";
    }
    return nullptr;
}

} // namespace

namespace amos::hal {

// (the host's commands: "c:assign", built in, as a lock on its own
// executable when there is no C:)
bool builtin_command(const char *name)
{
    return !strcasecmp(name, "c:assign") && !assigned("C", 1);
}

DirLock lock(const char *name, int32_t)
{
    if (builtin_command(name))
        return lock_path("/proc/self/exe");
    return lock_path(host_name(name));
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

// A FileInfoBlock as dos.library fills it: big-endian (AMOS reads it in
// its memory), the date of the file in days, minutes and ticks since 1978
void put_l(uint8_t *at, uint32_t v)
{
    at[0] = uint8_t(v >> 24);
    at[1] = uint8_t(v >> 16);
    at[2] = uint8_t(v >> 8);
    at[3] = uint8_t(v);
}

void fill(FileInfoBlock *fib, const char *path, const char *name)
{
    struct stat st;
    uint8_t *f = reinterpret_cast<uint8_t *>(fib);
    memset(f, 0, sizeof *fib);
    if (stat(path, &st) == 0) {
        int32_t type = S_ISDIR(st.st_mode) ? ST_USERDIR : ST_FILE;
        put_l(f + 4, uint32_t(type));                               // fib_DirEntryType
        put_l(f + 120, uint32_t(type));                             // fib_EntryType
        put_l(f + 124, uint32_t(st.st_size));                       // fib_Size
        put_l(f + 128, uint32_t((st.st_size + 511) / 512));         // fib_NumBlocks
        int64_t t = int64_t(st.st_mtime) - 252460800;               // since 1.1.1978
        if (t < 0)
            t = 0;
        put_l(f + 132, uint32_t(t / 86400));                        // ds_Days
        put_l(f + 136, uint32_t(t % 86400 / 60));                   // ds_Minute
        put_l(f + 140, uint32_t(t % 60 * 50));                      // ds_Tick
    }
    strncpy(reinterpret_cast<char *>(f + 8), name, 107);            // fib_FileName
}

} // namespace

bool examine(DirLock l, void *fib)
{
    if (!l || l >= 256 || !locks[l].name)
        return false;
    const char *name = volume_of(locks[l].name);
    if (!name) {
        name = strrchr(locks[l].name, '/');
        name = name ? name + 1 : locks[l].name;
    }
    fill(static_cast<FileInfoBlock *>(fib), locks[l].name, name);
    error = 0;
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
    // (the top of a volume has no parent)
    if (volume_of(locks[l].name) || !strcmp(locks[l].name, "/"))
        return 0;
    char path[1200];
    snprintf(path, sizeof path, "%s/..", locks[l].name);
    char *real = realpath(path, nullptr);
    if (!real || !strcmp(real, "/")) {
        free(real);
        return 0;
    }
    DirLock p = lock_path(real);
    free(real);
    return p;
}

// Info: the disk of the lock (the host's file system), its volume
bool info(DirLock l, void *info_data)
{
    if (!l || l >= 256 || !locks[l].name)
        return false;
    struct statvfs fs;
    if (statvfs(locks[l].name, &fs) < 0) {
        error = dos_error(errno);
        return false;
    }
    InfoData *id = static_cast<InfoData *>(info_data);
    memset(id, 0, sizeof *id);
    uint64_t block = 512;
    uint64_t total = uint64_t(fs.f_blocks) * fs.f_frsize / block, avail = uint64_t(fs.f_bavail) * fs.f_frsize / block;
    // (at most 2GB free, the bytes that AMOS's 32 bits count)
    if (total > 0x7FFFFFFF)
        total = 0x7FFFFFFF;
    if (avail > total)
        avail = total;
    if (avail > 0x7FFFFFFF / block)
        avail = 0x7FFFFFFF / block;
    id->id_DiskState = ID_VALIDATED;
    id->id_NumBlocks = int32_t(total);
    id->id_NumBlocksUsed = int32_t(total - avail);
    id->id_BytesPerBlock = int32_t(block);
    id->id_DiskType = ID_DOS_DISK;
    // the volume: the top directory above the lock that is an assign
    char path[1024];
    snprintf(path, sizeof path, "%s", locks[l].name);
    for (;;) {
        if (const char *v = volume_of(path)) {
            for (int i = 0; i < volume_count; i++)
                if (!strcmp(volumes[i], v))
                    id->id_VolumeNode = i + 1;
            if (!id->id_VolumeNode && volume_count < 64) {
                volumes[volume_count++] = strdup(v);
                id->id_VolumeNode = volume_count;
            }
            // (RAM: is full, as the Amiga's)
            if (!strcmp(v, "Ram Disk"))
                id->id_NumBlocksUsed = id->id_NumBlocks;
            break;
        }
        char *slash = strrchr(path, '/');
        if (!slash || slash == path)
            break;
        *slash = 0;
    }
    error = 0;
    return true;
}

bool volume_name(const void *info_data, char *name, int size)
{
    BPTR node = static_cast<const InfoData *>(info_data)->id_VolumeNode;
    if (node < 1 || node > volume_count)
        return false;
    snprintf(name, size_t(size), "%s", volumes[node - 1]);
    return true;
}
// AssignLock: name (without its colon) the directory of the lock, which
// it takes; no lock: the assign goes
bool assign_lock(const char *name, DirLock l)
{
    read_assigns();
    size_t n = strlen(name);
    if (n && name[n - 1] == ':')
        n--;
    if (!n || n >= sizeof assigns[0].name)
        return false;
    int i = 0;
    while (i < assign_count && !(strlen(assigns[i].name) == n && !strncasecmp(assigns[i].name, name, n)))
        i++;
    if (!l) {
        if (i == assign_count)
            return false;
        free(assigns[i].path);
        assigns[i] = assigns[--assign_count];
        return true;
    }
    if (l >= 256 || !locks[l].name)
        return false;
    if (i == assign_count) {
        if (assign_count == 64)
            return false;
        assign_count++;
        assigns[i].volume = false;
        memcpy(assigns[i].name, name, n);
        assigns[i].name[n] = 0;
    } else {
        free(assigns[i].path);
    }
    char *real = realpath(locks[l].name, nullptr);
    assigns[i].path = real ? real : strdup(locks[l].name);
    unlock(l);
    return true;
}
// A device: here an assign or a volume ("name:")
bool device_exists(const char *name)
{
    size_t n = strlen(name);
    if (n && name[n - 1] == ':')
        n--;
    return n && assigned(name, n);
}

// The volumes: the assigns and the directories of the devices directory
void each_device(void (*f)(const char *name, bool device, void *data), void *data)
{
    read_assigns();
    scan_devices();
    for (int i = 0; i < assign_count; i++) {
        f(assigns[i].name, assigns[i].volume, data);
        // (RAM: is a device whose volume is "Ram Disk")
        if (!strcasecmp(assigns[i].name, "RAM"))
            f("Ram Disk", false, data);
    }
}

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

// Execute: the host's commands (c:assign NAME: "directory")
bool execute(const char *command, File, File)
{
    // Wait n (seconds): the command word at the end of its path
    {
        const char *word = command, *end = command;
        while (*end && *end != ' ')
            end++;
        for (const char *p = command; p < end; p++)
            if (*p == '/' || *p == ':')
                word = p + 1;
        if (end - word == 4 && !strncasecmp(word, "wait", 4)) {
            double seconds = atof(end);
            if (seconds > 0)
                usleep(useconds_t(seconds * 1e6));
            return true;
        }
    }
    if (strncasecmp(command, "c:assign ", 9))
        return false;
    const char *p = command + 9;
    char name[64];
    int n = 0;
    while (*p && *p != ' ' && n < 63)
        name[n++] = *p++;
    name[n] = 0;
    while (*p == ' ')
        p++;
    char path[512];
    n = 0;
    if (*p == '"')
        p++;
    while (*p && *p != '"' && n < 511)
        path[n++] = *p++;
    path[n] = 0;
    DirLock l = lock(path, LockShared);
    return l && assign_lock(name, l);
}
void delay(int32_t ticks)
{
    for (; ticks > 0; ticks--) {
        if (host::in_task())
            host::task_yield();
        else {
            usleep(20000);
            host::run_tasks();
        }
    }
}

} // namespace amos::hal
