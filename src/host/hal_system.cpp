// hal/system.h on another host: AMOS's memory below 4GB (it keeps its
// addresses in 32 bits), one task, no devices or interrupts yet.

#include "hal/system.h"
#include "runtime/runtime.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>

extern "C" char *getenv(const char *name) noexcept;    // (stdlib.h clashes with dos/dosextens.h)

#include <exec/tasks.h>
#include <exec/libraries.h>
#include <exec/devices.h>
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
    {"exec.library", nullptr}, {"translator.library", nullptr},
};

// A library's base has its jump table (the offsets, a negative number of
// bytes) below it: a call there is made by library_call
constexpr uint32_t JumpTable = 1024;

Library *library_base(HostLibrary &l)
{
    if (!l.base) {
        auto *block = static_cast<uint8_t *>(amos::hal::allocate(4096 + JumpTable, 0x10000));
        if (!block)
            return nullptr;
        l.base = reinterpret_cast<Library *>(block + JumpTable);
        l.base->lib_Version = 40;
        l.base->lib_Node.ln_Name = const_cast<char *>(l.name);
    }
    return l.base;
}

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

// The memory: one arena below 4GB, managed as exec's lists: blocks of 8
// bytes, the first free one that fits, and any part of a block can be
// freed (AMOS does) or taken again (AllocAbs: Bank Shrink). The free ranges
// are a table sorted by address, apart from the memory: what is freed keeps
// its contents (exec only writes the first 8 bytes of it, and AMOS relies on
// the rest of a header it frees and takes again).
struct Range {
    uint8_t *at;
    uint64_t size;
};
constexpr int MaxRanges = 1 << 20;
Range *ranges;
int range_count;
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
    ranges = static_cast<Range *>(mmap(nullptr, sizeof(Range) * MaxRanges, PROT_READ | PROT_WRITE,
                                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0));
    ranges[0] = {arena, ArenaSize};
    range_count = 1;
}

uint64_t rounded(uint32_t size) { return (uint64_t(size) + 7) & ~uint64_t(7); }

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
    for (int i = 0; i < range_count; i++) {
        if (ranges[i].size < n)
            continue;
        uint8_t *found = ranges[i].at;
        if (ranges[i].size == n) {
            memmove(ranges + i, ranges + i + 1, sizeof(Range) * size_t(range_count - i - 1));
            range_count--;
        } else {
            ranges[i].at += n;
            ranges[i].size -= n;
        }
        if (flags & 0x10000)
            memset(found, 0, n);
        if (memcheck())
            block_add(found, n);
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
    // into the table, merged with the ranges before and after
    int i = 0;
    while (i < range_count && ranges[i].at < at)
        i++;
    bool before = i > 0 && ranges[i - 1].at + ranges[i - 1].size == at;
    bool after = i < range_count && at + n == ranges[i].at;
    if (before && after) {
        ranges[i - 1].size += n + ranges[i].size;
        memmove(ranges + i, ranges + i + 1, sizeof(Range) * size_t(range_count - i - 1));
        range_count--;
    } else if (before) {
        ranges[i - 1].size += n;
    } else if (after) {
        ranges[i].at = at;
        ranges[i].size += n;
    } else {
        memmove(ranges + i + 1, ranges + i, sizeof(Range) * size_t(range_count - i));
        ranges[i] = {at, n};
        range_count++;
    }
}

// AllocAbs: the memory at address, if it is free (taken out of its range)
void *allocate_at(uint32_t size, void *where)
{
    if (!size || !where)
        return nullptr;
    arena_start();
    uint8_t *at = static_cast<uint8_t *>(where);
    uint64_t n = rounded(size);
    for (int i = 0; i < range_count; i++) {
        uint8_t *start = ranges[i].at;
        if (at < start || at + n > start + ranges[i].size)
            continue;
        uint64_t before = uint64_t(at - start), after = ranges[i].size - before - n;
        if (before && after) {
            memmove(ranges + i + 1, ranges + i, sizeof(Range) * size_t(range_count - i));
            range_count++;
            ranges[i].size = before;
            ranges[i + 1] = {at + n, after};
        } else if (before) {
            ranges[i].size = before;
        } else if (after) {
            ranges[i] = {at + n, after};
        } else {
            memmove(ranges + i, ranges + i + 1, sizeof(Range) * size_t(range_count - i - 1));
            range_count--;
        }
        if (memcheck())
            block_add(at, n);
        return where;
    }
    return nullptr;
}

Task *find_task(const char *name) { return name ? nullptr : &the_process.pr_Task; }
void signal(Task *, uint32_t) {}
static uint32_t the_requester_window;
uint32_t requester_window() { return the_requester_window; }
void set_requester_window(uint32_t window) { the_requester_window = window; }

Library *open_library(const char *name, uint32_t version)
{
    for (auto &l : libraries)
        if (!strcmp(l.name, name)) {
            if (!library_base(l))
                return nullptr;
            if (version > l.base->lib_Version)
                return nullptr;
            return l.base;
        }
    return nullptr;
}
void close_library(Library *) {}

// A call into a library's jump table (Doscall, Execall, Lib Call): the
// few functions that have an answer here, the others 0 in d0
bool library_call(uint32_t at, AllRegs &r)
{
    for (auto &l : libraries) {
        uint32_t base = address(l.base);
        if (!l.base || at >= base || at + JumpTable < base)
            continue;
        int32_t lvo = int32_t(at - base);
        r.d[0] = 0;
        if (!strcmp(l.name, "exec.library")) {
            if (lvo == -294)                    // FindTask
                r.d[0] = r.a[1] ? 0 : address(find_task(nullptr));
        } else if (!strcmp(l.name, "dos.library")) {
            if (lvo == -198)                    // Delay (ticks of 20ms)
                usleep(r.d[1] * 20000);
        }
        return true;
    }
    return false;
}
uint16_t os_version() { return 40; }
uint32_t exec_base()
{
    for (auto &l : libraries)
        if (!strcmp(l.name, "exec.library"))
            return library_base(l) ? address(l.base) : 0;
    return 0;
}
uint8_t vblank_frequency() { return 50; }
void clear_cache() {}
uint32_t set_function(uint32_t, int16_t, uint32_t) { return 0; }

void add_head(void *, void *) {}

// The public ports of the program (there is no other task to talk to)
namespace {
MsgPort *ports[64];
uint32_t signals_used = 0xFFFF;     // the system's: bits 0-15
} // namespace

void add_port(MsgPort *port)
{
    port->mp_MsgList.lh_Head = reinterpret_cast<Node *>(&port->mp_MsgList.lh_Tail);
    port->mp_MsgList.lh_Tail = nullptr;
    port->mp_MsgList.lh_TailPred = reinterpret_cast<Node *>(&port->mp_MsgList.lh_Head);
    for (MsgPort *&p : ports)
        if (!p) {
            p = port;
            return;
        }
}
void remove_port(MsgPort *port)
{
    for (MsgPort *&p : ports)
        if (p == port)
            p = nullptr;
}
MsgPort *find_port(const char *name)
{
    for (MsgPort *p : ports)
        if (p && p->mp_Node.ln_Name && !strcasecmp(p->mp_Node.ln_Name, name))
            return p;
    return nullptr;
}
Message *get_message(MsgPort *port)
{
    Node *first = port->mp_MsgList.lh_Head;
    if (!first->ln_Succ)
        return nullptr;
    first->ln_Succ->ln_Pred = first->ln_Pred;
    first->ln_Pred->ln_Succ = first->ln_Succ;
    return reinterpret_cast<Message *>(first);
}
Message *wait_port(MsgPort *port)
{
    for (;;) {
        if (Message *m = get_message(port))
            return m;
        idle();
    }
}
void reply_message(Message *) {}
int32_t allocate_signal(int32_t n)
{
    for (int i = n >= 0 ? n : 16; i < 32; i++)
        if (!(signals_used & (1u << i))) {
            signals_used |= 1u << i;
            return i;
        }
    return -1;
}
void free_signal(int32_t n)
{
    if (n >= 16 && n < 32)
        signals_used &= ~(1u << n);
}

// The devices: input.device (AMOS.library's input handler) and
// console.device (its keymap: hal::key_to_text) open; the others not
// The devices there are: input.device (AMOS.library's input handler) and
// console.device (its keymap), and timer.device and serial.device, which
// open and complete what they are asked at once; narrator.device too: the
// host has no speech, what is said is silent
namespace {
constexpr int DeviceCount = 5;
Device devices[DeviceCount];
const char *const device_names[DeviceCount] = {"input.device", "console.device", "timer.device", "serial.device",
                                               "narrator.device"};
} // namespace

int32_t open_device(const char *name, uint32_t, IORequest *io, uint32_t)
{
    for (int i = 0; i < DeviceCount; i++)
        if (!strcmp(name, device_names[i])) {
            devices[i].dd_Library.lib_Node.ln_Name = const_cast<char *>(device_names[i]);
            io->io_Device = &devices[i];
            io->io_Error = 0;
            return 0;
        }
    return -1;
}
void close_device(IORequest *io) { io->io_Device = nullptr; }
// The narrator's write: the text said (the sound queued)
static void narrate(IORequest *io)
{
    auto *request = reinterpret_cast<IOStdReq *>(io);
    if (io->io_Command == 3 && request->io_Data)      // CMD_WRITE
        host::speak(static_cast<const char *>(request->io_Data), uint32_t(request->io_Length));
}

int32_t do_io(IORequest *io)
{
    if (io->io_Device == &devices[4]) {
        narrate(io);
        while (host::speaking())        // (done when it has been heard)
            idle();
        return 0;
    }
    if (io->io_Device != &devices[0] && io->io_Device != &devices[1])
        return 0;
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
// The narrator: a write speaks the text (host/speech.cpp), if there is a voice; a request sent
// is done when the sound has played
void send_io(IORequest *io)
{
    if (io->io_Device == &devices[4])
        narrate(io);
    else
        do_io(io);
}
IORequest *check_io(IORequest *io) { return io->io_Device == &devices[4] && host::speaking() ? nullptr : io; }
void abort_io(IORequest *io)
{
    if (io->io_Device == &devices[4])
        host::speech_stop();
}
int32_t wait_io(IORequest *io)
{
    while (io->io_Device == &devices[4] && host::speaking())
        idle();
    return -1;
}

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
