// hal/system.h on another host: AMOS's memory below 4GB (it keeps its
// addresses in 32 bits), one task, no devices or interrupts yet.

#include "hal/system.h"
#include "host/host.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include <exec/tasks.h>
#include <exec/libraries.h>
#include <dos/dosextens.h>

namespace {

// The one task: a process started from a CLI
Process the_process = [] {
    Process p = {};
    p.pr_CLI = 1;
    return p;
}();

// The libraries of the system: a base each (room for the library's
// structure: GfxBase...), known by its name
struct HostLibrary {
    const char *name;
    Library *base;
};
HostLibrary libraries[] = {
    {"dos.library", nullptr}, {"intuition.library", nullptr}, {"graphics.library", nullptr},
    {"layers.library", nullptr}, {"diskfont.library", nullptr}, {"icon.library", nullptr},
    {"mathffp.library", nullptr}, {"mathtrans.library", nullptr}, {"mathieeedoubbas.library", nullptr},
    {"mathieeedoubtrans.library", nullptr}, {"mathieeesingbas.library", nullptr},
    {"mathieeesingtrans.library", nullptr}, {"rexxsyslib.library", nullptr}, {"asl.library", nullptr},
    {"workbench.library", nullptr}, {"keymap.library", nullptr}, {"console.device", nullptr},
};

} // namespace

namespace amos::host {

// The name of a library of the system (null: not one)
const char *library_name(uint32_t base)
{
    for (auto &l : libraries)
        if (l.base && address(l.base) == base)
            return l.name;
    return nullptr;
}

} // namespace amos::host

namespace amos::hal {

uint32_t available_memory(uint32_t flags)
{
    if (flags & MemoryChip)
        return 2 * 1024 * 1024;
    return 8 * 1024 * 1024;
}

void forbid() {}
void permit() {}

// The memory: one arena below 4GB, managed as exec's lists: blocks of 8
// bytes, the first one free that fits, and any part of a block can be
// freed (AMOS does). The free chunks are a list sorted by address.
struct Chunk {
    Chunk *next;
    uint64_t size;
};
Chunk *free_list;
uint8_t *arena;
constexpr uint64_t ArenaSize = 512ull << 20;

void arena_start()
{
    if (arena)
        return;
    void *m = mmap(nullptr, ArenaSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT | MAP_NORESERVE,
                   -1, 0);
    if (m == MAP_FAILED) {
        fprintf(stderr, "amos: no memory below 4GB\n");
        __builtin_trap();
    }
    arena = static_cast<uint8_t *>(m);
    free_list = reinterpret_cast<Chunk *>(arena);
    free_list->next = nullptr;
    free_list->size = ArenaSize;
}

uint64_t rounded(uint32_t size) { return (uint64_t(size) + 7) & ~uint64_t(7); }

void *allocate(uint32_t size, uint32_t flags)
{
    if (!size)
        return nullptr;
    arena_start();
    uint64_t n = rounded(size);
    for (Chunk **c = &free_list; *c; c = &(*c)->next) {
        if ((*c)->size < n)
            continue;
        Chunk *found = *c;
        if (found->size == n) {
            *c = found->next;
        } else {
            Chunk *rest = reinterpret_cast<Chunk *>(reinterpret_cast<uint8_t *>(found) + n);
            rest->next = found->next;
            rest->size = found->size - n;
            *c = rest;
        }
        if (flags & 0x10000)
            memset(found, 0, n);
        return found;
    }
    return nullptr;
}

void release(void *memory, uint32_t size)
{
    if (!memory || !size)
        return;
    uint8_t *at = static_cast<uint8_t *>(memory);
    uint64_t n = rounded(size);
    Chunk **c = &free_list;
    while (*c && reinterpret_cast<uint8_t *>(*c) < at)
        c = &(*c)->next;
    Chunk *chunk = reinterpret_cast<Chunk *>(at);
    chunk->size = n;
    chunk->next = *c;
    *c = chunk;
    // merged with the next and the previous ones
    if (chunk->next && at + chunk->size == reinterpret_cast<uint8_t *>(chunk->next)) {
        chunk->size += chunk->next->size;
        chunk->next = chunk->next->next;
    }
    for (Chunk *p = free_list; p && p != chunk; p = p->next)
        if (p->next == chunk && reinterpret_cast<uint8_t *>(p) + p->size == at) {
            p->size += chunk->size;
            p->next = chunk->next;
            break;
        }
}

void *allocate_at(uint32_t, void *) { return nullptr; }

Task *find_task(const char *name) { return name ? nullptr : &the_process.pr_Task; }
void signal(Task *, uint32_t) {}
static uint32_t the_requester_window;
uint32_t requester_window() { return the_requester_window; }
void set_requester_window(uint32_t window) { the_requester_window = window; }

Library *open_library(const char *name, uint32_t version)
{
    for (auto &l : libraries)
        if (!strcmp(l.name, name)) {
            if (!l.base) {
                l.base = static_cast<Library *>(allocate(4096, 0x10000));
                if (!l.base)
                    return nullptr;
                l.base->lib_Version = 40;
                l.base->lib_Node.ln_Name = const_cast<char *>(l.name);
            }
            if (version > l.base->lib_Version)
                return nullptr;
            return l.base;
        }
    return nullptr;
}
void close_library(Library *) {}
uint16_t os_version() { return 40; }
uint32_t exec_base() { return 0; }
uint8_t vblank_frequency() { return 50; }
void clear_cache() {}
uint32_t set_function(uint32_t, int16_t, uint32_t) { return 0; }

void add_head(void *, void *) {}
void add_port(MsgPort *) {}
void remove_port(MsgPort *) {}
MsgPort *find_port(const char *) { return nullptr; }
Message *get_message(MsgPort *) { return nullptr; }
Message *wait_port(MsgPort *) { return nullptr; }
void reply_message(Message *) {}
int32_t allocate_signal(int32_t) { return -1; }
void free_signal(int32_t) {}
void add_task(Task *, void *, void *) { host::missing("task", nullptr); }

int32_t open_device(const char *, uint32_t, IORequest *, uint32_t) { return -1; }
void close_device(IORequest *) {}
int32_t do_io(IORequest *) { return -1; }
void send_io(IORequest *) {}
IORequest *check_io(IORequest *io) { return io; }
void abort_io(IORequest *) {}
int32_t wait_io(IORequest *) { return -1; }

Interrupt *set_interrupt(int32_t, Interrupt *) { return nullptr; }

// RawDoFmt: %[-][0][width][.limit][l]d, u, x, s, b (a BSTR), c; the
// arguments are 16-bit words unless l, as exec's
const void *raw_format(const char *format, const void *args, void (*put)(char, void *), void *data)
{
    const uint8_t *a = static_cast<const uint8_t *>(args);
    auto word = [&]() {
        uint16_t v = uint16_t(a[0] << 8 | a[1]);
        a += 2;
        return v;
    };
    auto longword = [&]() {
        uint32_t v = uint32_t(a[0]) << 24 | uint32_t(a[1]) << 16 | uint32_t(a[2]) << 8 | a[3];
        a += 4;
        return v;
    };
    for (const char *p = format;; p++) {
        if (*p != '%') {
            put(*p, data);
            if (!*p)
                break;
            continue;
        }
        p++;
        bool left = *p == '-';
        if (left)
            p++;
        char pad = *p == '0' ? '0' : ' ';
        int width = 0, limit = -1;
        while (*p >= '0' && *p <= '9')
            width = width * 10 + (*p++ - '0');
        if (*p == '.') {
            limit = 0;
            for (p++; *p >= '0' && *p <= '9';)
                limit = limit * 10 + (*p++ - '0');
        }
        bool l = *p == 'l';
        if (l)
            p++;
        char text[64];
        const char *s = text;
        switch (*p) {
        case 'd': {
            int32_t v = l ? int32_t(longword()) : int16_t(word());
            snprintf(text, sizeof text, "%d", v);
            break;
        }
        case 'u':
            snprintf(text, sizeof text, "%u", l ? longword() : word());
            break;
        case 'x':
            snprintf(text, sizeof text, "%x", l ? longword() : word());
            break;
        case 'c':
            text[0] = char(l ? longword() : word());
            text[1] = 0;
            break;
        case 's':
            s = reinterpret_cast<const char *>(uintptr_t(longword()));
            break;
        default:
            text[0] = *p;
            text[1] = 0;
            break;
        }
        int n = int(strlen(s));
        if (limit >= 0 && n > limit)
            n = limit;
        if (!left)
            for (int i = n; i < width; i++)
                put(pad, data);
        for (int i = 0; i < n; i++)
            put(s[i], data);
        if (left)
            for (int i = n; i < width; i++)
                put(' ', data);
        if (!*p)
            break;
    }
    return a;
}

} // namespace amos::hal
