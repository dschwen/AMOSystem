// hal/system.h on another host: AMOS's memory below 4GB (it keeps its
// addresses in 32 bits), one task, no devices or interrupts yet.

#include "hal/system.h"
#include "runtime/runtime.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

extern "C" char *getenv(const char *name) noexcept;    // (stdlib.h clashes with dos/dosextens.h)

#include <exec/tasks.h>
#include <exec/libraries.h>
#include <dos/dosextens.h>
#include <exec/io.h>

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

void input_handler(Interrupt *is);

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

// The memory: one arena below 4GB, managed as exec's lists: blocks of 16
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

// (in 16 bytes: a free chunk holds its header)
uint64_t rounded(uint32_t size) { return (uint64_t(size) + 15) & ~uint64_t(15); }

// (debugging: AMOS_MEMCHECK, the blocks given, sorted, for the frees)
bool memcheck() { static bool check = getenv("AMOS_MEMCHECK"); return check; }
struct Block {
    uint8_t *at;
    uint64_t size;
};
Block *blocks;
int block_count;
constexpr int MaxBlocks = 1 << 20;

int block_index(uint8_t *at)   // the first block at or after at
{
    int lo = 0, hi = block_count;
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (blocks[mid].at < at)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

void block_add(uint8_t *at, uint64_t size)
{
    if (!blocks)
        blocks = static_cast<Block *>(mmap(nullptr, sizeof(Block) * MaxBlocks, PROT_READ | PROT_WRITE,
                                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    int i = block_index(at);
    memmove(blocks + i + 1, blocks + i, sizeof(Block) * size_t(block_count - i));
    blocks[i] = {at, size};
    block_count++;
}

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
        if (memcheck())
            block_add(reinterpret_cast<uint8_t *>(found), n);
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
    // (debugging: AMOS_MEMCHECK: a free outside one block given)
    if (memcheck()) {
        int i = block_index(at + 1) - 1;
        if (i < 0 || at + n > blocks[i].at + blocks[i].size) {
            fprintf(stderr, "amos: memory %p+%u freed, not in one block\n", memory, size);
            __builtin_trap();
        }
        // the block shrinks, or splits
        Block b = blocks[i];
        memmove(blocks + i, blocks + i + 1, sizeof(Block) * size_t(block_count - i - 1));
        block_count--;
        if (at > b.at)
            block_add(b.at, uint64_t(at - b.at));
        if (at + n < b.at + b.size)
            block_add(at + n, uint64_t(b.at + b.size - (at + n)));
    }
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

// The devices: input.device (AMOS.library's input handler) and
// console.device (its keymap: hal::key_to_text) open; the others not
int32_t open_device(const char *name, uint32_t, IORequest *io, uint32_t)
{
    if (strcmp(name, "input.device") && strcmp(name, "console.device"))
        return -1;
    io->io_Error = 0;
    return 0;
}
void close_device(IORequest *) {}
int32_t do_io(IORequest *io)
{
    switch (io->io_Command) {
    case 9:     // IND_ADDHANDLER
        host::input_handler(static_cast<Interrupt *>(reinterpret_cast<IOStdReq *>(io)->io_Data));
        return 0;
    case 10:    // IND_REMHANDLER
        host::input_handler(nullptr);
        return 0;
    case 11:    // IND_WRITEEVENT: the screen blankers' fake events (none here)
        return 0;
    }
    return -1;
}
void send_io(IORequest *) {}
IORequest *check_io(IORequest *io) { return io; }
void abort_io(IORequest *) {}
int32_t wait_io(IORequest *) { return -1; }

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
