// hal/files.h on another host: POSIX. Files are their descriptors + 1
// (0: none); a lock is the number of its name in a table of names.

#include "hal/files.h"
#include "hal/system.h"
#include "host/devices.h"
#include "runtime/runtime.h"

extern "C" void w_easy_request(amos::AllRegs *r);
extern "C" uint32_t amos_w_base;

namespace amos::host {
bool window_shown();
bool drops_pending();
void disk_event(bool inserted);
void hint(const char *volume);
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

int last_error;      // the last error, as AmigaDOS's

int32_t dos_error(int e)
{
    switch (e) {
    case ENOENT: return ERROR_OBJECT_NOT_FOUND;
    case EEXIST: return ERROR_OBJECT_EXISTS;
    case EBUSY: return ERROR_OBJECT_IN_USE;
    case EACCES: return ERROR_READ_PROTECTED;
    case ENOSPC: return ERROR_DISK_FULL;
    case EROFS: return ERROR_WRITE_PROTECTED;
    case ENOTEMPTY: return ERROR_DIRECTORY_NOT_EMPTY;
    case EISDIR: return ERROR_OBJECT_WRONG_TYPE;
    default: return ERROR_OBJECT_WRONG_TYPE;
    }
}

// The disks, the units and the assigns are host/devices.cpp's
const char *assigned(const char *volume, size_t n)
{
    amos::host::devices::Status status;
    return amos::host::devices::resolve(volume, n, status);
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

// A volume not mounted: AmigaDOS's requester "Please insert volume" (the
// AMOS requester, as AMOS patches EasyRequest), unless the process has
// none (pr_WindowPtr -1). True to try again (Retry), false: not mounted
bool insert_volume(const char *volume, size_t n)
{
    if (amos::hal::requester_window() == 0xFFFFFFFFu || !amos_w_base ||
        !(amos::host::window_shown() || amos::host::drops_pending()))
        return false;
    // (AMOS's requester opens over the current screen)
    const auto *w = reinterpret_cast<const amos::WData *>(amos::ptr<uint8_t>(amos_w_base) - sizeof(amos::WData));
    if (!w->EcCourant)
        return false;
    // (the structures in AMOS's memory, below 4GB)
    // (intuition.h's EasyStruct: its header clashes with the host's timeval)
    struct EasyStruct {
        ULONG es_StructSize, es_Flags;
        UBYTE *es_Title, *es_TextFormat, *es_GadgetFormat;
    };
    struct Request {
        EasyStruct easy;
        char name[64];
        uint8_t args[4], idcmp[4];
    };
    auto *q = static_cast<Request *>(amos::hal::allocate(sizeof(Request), 0x10000));
    if (!q)
        return false;
    snprintf(q->name, sizeof q->name, "%.*s", int(n < 63 ? n : 63), volume);
    q->easy.es_StructSize = sizeof(EasyStruct);
    q->easy.es_TextFormat = reinterpret_cast<UBYTE *>(const_cast<char *>("Please insert volume\n%s\nin any drive"));
    q->easy.es_GadgetFormat = reinterpret_cast<UBYTE *>(const_cast<char *>("Retry|Cancel"));
    amos::poke_l(q->args, amos::address(q->name));
    // the requester ends by itself when a disk is put in (DISKINSERTED): no disk is in the
    // drive now, as AMOS.library sees it
    amos::poke_l(q->idcmp, 0x8000);
    amos::host::disk_event(false);
    amos::host::hint(q->name);
    amos::AllRegs r = {};
    r.a[1] = amos::address(&q->easy);
    r.a[2] = amos::address(q->idcmp);
    r.a[3] = amos::address(q->args);
    // (AMOS.library's context: a5 its data)
    amos::Ctx saved = amos::ctx();
    amos::ctx().a5 = amos::ptr<uint8_t>(amos_w_base);
    w_easy_request(&r);
    amos::ctx() = saved;
    amos::host::hint(nullptr);
    amos::hal::release(q, sizeof(Request));
    return r.d[0] != 0;
}

// host_name's volume is not there (cancelled: ERROR_DEVICE_NOT_MOUNTED), or its
// unit is empty (ERROR_NO_DISK); 0 if it is
int name_error;

// AmigaDOS's names: "volume:dir/file" (the volume an assign, or a
// directory of the current one), "/" alone up a directory; each part
// found whatever its case. A volume that is not there: its requester, and
// name_error if it is cancelled
const char *host_name(const char *name)
{
    name_error = 0;
    static char buffer[1024];
    char *to = buffer;
    *to = 0;
    const char *colon = strchr(name, ':');
    if (colon) {
        size_t n = size_t(colon - name);
        // NIL:, the device that takes everything and gives nothing
        if (n == 3 && !strncasecmp(name, "NIL", 3))
            return "/dev/null";
        amos::host::devices::Status status;
        if (const char *path = amos::host::devices::resolve(name, n, status)) {
            snprintf(buffer, sizeof buffer, "%s", path);
        } else if (status == amos::host::devices::Status::no_disk) {
            name_error = ERROR_NO_DISK;
            snprintf(buffer, sizeof buffer, "%.*s", int(n), name);
        } else if (n) {
            // a directory of the current one, else of the root (the names
            // of the locks begin with the root's directory: "home:...")
            char entry[256];
            for (;;) {
                if (find_entry("", name, n, entry, sizeof entry))
                    snprintf(buffer, sizeof buffer, "%s", entry);
                else if (find_entry("/", name, n, entry, sizeof entry))
                    snprintf(buffer, sizeof buffer, "/%s", entry);
                else if (const char *path = assigned(name, n))
                    snprintf(buffer, sizeof buffer, "%s", path);    // (mounted meanwhile)
                else if (insert_volume(name, n))
                    continue;
                else {
                    name_error = ERROR_DEVICE_NOT_MOUNTED;
                    snprintf(buffer, sizeof buffer, "%.*s", int(n), name);
                }
                break;
            }
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

// A lock, and the entries of its directory that ExNext gives (Examine reads
// them)
struct Lock {
    char *name;
    char **entries;
    int count, next;
};
Lock locks[256];

void free_entries(Lock &l)
{
    for (int i = 0; i < l.count; i++)
        free(l.entries[i]);
    free(l.entries);
    l.entries = nullptr;
    l.count = l.next = 0;
}

// The entries of a directory, the newest first (as the RAM disk of the
// Amiga lists them; its disks list them in their hash order): by the time
// they were made (else changed), then by their inode (the file systems give
// them in order, mostly, where the clock is too coarse), then by name
void read_entries(Lock &l)
{
    free_entries(l);
    DIR *d = opendir(l.name);
    if (!d)
        return;
    struct Entry {
        char *name;
        int64_t sec, nsec;
        uint64_t inode;
    };
    Entry *list = nullptr;
    int n = 0, room = 0;
    while (struct dirent *e = readdir(d)) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, ".."))
            continue;
        if (n == room) {
            room = room ? 2 * room : 32;
            list = static_cast<Entry *>(realloc(list, sizeof(Entry) * size_t(room)));
        }
        Entry &x = list[n++];
        x = {strdup(e->d_name), 0, 0, e->d_ino};
        struct statx st;
        if (statx(dirfd(d), e->d_name, AT_SYMLINK_NOFOLLOW, STATX_BTIME | STATX_MTIME, &st) == 0) {
            const struct statx_timestamp &t = st.stx_mask & STATX_BTIME ? st.stx_btime : st.stx_mtime;
            x.sec = t.tv_sec;
            x.nsec = t.tv_nsec;
        }
    }
    closedir(d);
    qsort(list, size_t(n), sizeof(Entry), [](const void *a, const void *b) {
        const Entry &x = *static_cast<const Entry *>(a), &y = *static_cast<const Entry *>(b);
        if (x.sec != y.sec)
            return x.sec > y.sec ? -1 : 1;
        if (x.nsec != y.nsec)
            return x.nsec > y.nsec ? -1 : 1;
        if (x.inode != y.inode)
            return x.inode > y.inode ? -1 : 1;
        return strcmp(x.name, y.name);
    });
    l.entries = static_cast<char **>(malloc(sizeof(char *) * size_t(n ? n : 1)));
    for (int i = 0; i < n; i++)
        l.entries[i] = list[i].name;
    l.count = n;
    free(list);
}

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
    if (name_error) {
        last_error = name_error;
        return 0;
    }
    struct stat st;
    if (stat(h, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            // (a directory is not a file: "object wrong type")
            last_error = ERROR_OBJECT_WRONG_TYPE;
            return 0;
        }
        for (const Open &o : opened)
            if (o.used && o.dev == st.st_dev && o.ino == st.st_ino && (o.exclusive || mode == ModeNewFile)) {
                last_error = ERROR_OBJECT_IN_USE;
                return 0;
            }
    }
    // (an old file can be written too; read-only if it cannot, or if its disk is write-protected)
    bool protect = amos::host::devices::read_only(h);
    if (protect && mode != ModeOldFile) {
        last_error = ERROR_WRITE_PROTECTED;
        return 0;
    }
    int flags = mode == ModeNewFile ? O_RDWR | O_CREAT | O_TRUNC : mode == ModeReadWrite ? O_RDWR | O_CREAT : O_RDWR;
    int fd = protect ? ::open(h, O_RDONLY) : ::open(h, flags, 0644);
    if (fd < 0 && mode == ModeOldFile && (errno == EACCES || errno == EROFS))
        fd = ::open(h, O_RDONLY);
    if (fd < 0) {
        last_error = dos_error(errno);
        return 0;
    }
    if (fd < 1024 && fstat(fd, &st) == 0)
        opened[fd] = {st.st_dev, st.st_ino, mode == ModeNewFile, true};
    last_error = 0;
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
    last_error = n < 0 ? dos_error(errno) : 0;
    return int32_t(n);
}

int32_t write(File f, const void *buffer, int32_t length)
{
    ssize_t n = ::write(int(f) - 1, buffer, size_t(length));
    // (a file of a write-protected disk is open for reading only)
    last_error = n < 0 ? (errno == EBADF ? ERROR_WRITE_PROTECTED : dos_error(errno)) : 0;
    return int32_t(n);
}

int32_t seek(File f, int32_t offset, int32_t origin)
{
    int fd = int(f) - 1;
    off_t old = lseek(fd, 0, SEEK_CUR);
    int whence = origin == SeekBeginning ? SEEK_SET : origin == SeekEnd ? SEEK_END : SEEK_CUR;
    if (old < 0 || lseek(fd, offset, whence) < 0) {
        last_error = dos_error(errno);
        return -1;
    }
    return int32_t(old);
}

bool wait_for_char(File, int32_t) { return false; }
File output() { return 2; }         // stdout
int32_t io_error() { return last_error; }

} // namespace amos::hal

namespace {

// A lock on a path of the host (its real path)
amos::hal::DirLock lock_path(const char *h)
{
    char *real = realpath(h, nullptr);
    if (!real) {
        last_error = dos_error(errno);
        return 0;
    }
    for (int i = 1; i < 256; i++) {
        if (!locks[i].name) {
            locks[i] = {real, nullptr, 0, 0};
            last_error = 0;
            return amos::hal::DirLock(i);
        }
    }
    free(real);
    last_error = ERROR_NO_FREE_STORE;
    return 0;
}

// The volumes Info gave (InfoData's id_VolumeNode: 1 + the index)
char *volumes[64];
int volume_count;

// The volume a directory is the top of (RAM: is "Ram Disk", as on the Amiga), or null
const char *volume_of(const char *real) { return amos::host::devices::volume_of(real); }

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
    const char *h = host_name(name);
    if (name_error) {
        last_error = name_error;
        return 0;
    }
    return lock_path(h);
}

void unlock(DirLock l)
{
    if (l && l < 256 && locks[l].name) {
        free_entries(locks[l]);
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
    last_error = 0;
    read_entries(locks[l]);
    return true;
}

bool examine_next(DirLock l, void *fib)
{
    if (!l || l >= 256 || !locks[l].name)
        return false;
    Lock &k = locks[l];
    while (k.next < k.count) {
        const char *entry = k.entries[k.next++];
        char path[1200];
        snprintf(path, sizeof path, "%s/%s", k.name, entry);
        struct stat st;
        if (lstat(path, &st) < 0)
            continue;           // (gone since)
        fill(static_cast<FileInfoBlock *>(fib), path, entry);
        return true;
    }
    last_error = ERROR_NO_MORE_ENTRIES;
    return false;
}

DirLock current_dir(DirLock l)
{
    if (l && l < 256 && locks[l].name && chdir(locks[l].name) < 0)
        last_error = dos_error(errno);
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
        last_error = dos_error(errno);
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
    last_error = 0;
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
    size_t n = strlen(name);
    if (n && name[n - 1] == ':')
        n--;
    if (!l)
        return amos::host::devices::assign(name, n, nullptr);
    if (l >= 256 || !locks[l].name)
        return false;
    bool done = amos::host::devices::assign(name, n, locks[l].name);
    unlock(l);
    return done;
}
// A device: a unit (even without a disk), an assign or a volume ("name:")
bool device_exists(const char *name)
{
    size_t n = strlen(name);
    if (n && name[n - 1] == ':')
        n--;
    return n && amos::host::devices::exists(name, n);
}

// The names for Dev First$ and the file selector: the units, the volumes, the assigns
void each_device(void (*f)(const char *name, bool device, void *data), void *data)
{
    amos::host::devices::each(f, data);
}

bool delete_file(const char *name)
{
    const char *h = host_name(name);
    if (name_error) {
        last_error = name_error;
        return false;
    }
    if (amos::host::devices::read_only(h)) {
        last_error = ERROR_WRITE_PROTECTED;
        return false;
    }
    if (::unlink(h) == 0 || ::rmdir(h) == 0)
        return true;
    last_error = dos_error(errno);
    return false;
}

bool rename(const char *from, const char *to)
{
    char f[1024];
    strncpy(f, host_name(from), sizeof f - 1);
    f[sizeof f - 1] = 0;
    const char *t = name_error ? nullptr : host_name(to);
    if (name_error) {
        last_error = name_error;
        return false;
    }
    if (amos::host::devices::read_only(f) || amos::host::devices::read_only(t)) {
        last_error = ERROR_WRITE_PROTECTED;
        return false;
    }
    if (::rename(f, t) == 0)
        return true;
    last_error = dos_error(errno);
    return false;
}

DirLock create_dir(const char *name)
{
    const char *h = host_name(name);
    if (name_error) {
        last_error = name_error;
        return 0;
    }
    if (amos::host::devices::read_only(h)) {
        last_error = ERROR_WRITE_PROTECTED;
        return 0;
    }
    if (mkdir(h, 0755) < 0) {
        last_error = dos_error(errno);
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
