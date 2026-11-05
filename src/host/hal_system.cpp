// hal/system.h on another host: AMOS's memory below 4GB (it keeps its
// addresses in 32 bits), one task, no devices or interrupts yet.

#include "hal/system.h"
#include "core/ioreq.h"
#include "memory/guest_allocator.h"
#include "abi/guest_address_codec.h"
#include "runtime/runtime.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#if defined(_WIN32)
#include "host/windows_memory.h"
#else
#include <sys/mman.h>
#endif

extern "C" char *getenv(const char *name) noexcept;    // (stdlib.h clashes with dos/dosextens.h)

#include <exec/tasks.h>
#include <exec/libraries.h>
#include <exec/devices.h>
// Keep the NDK's Amiga timeval separate from the host's POSIX type.
#define timeval AmigaTimeval
#include <dos/dosextens.h>
#undef timeval
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
        // (in chip memory, ExecBase as on the Amiga)
        auto *block = static_cast<uint8_t *>(amos::hal::allocate(4096 + JumpTable, 0x10000 | amos::hal::MemoryChip));
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

// The memory: two arenas below 4GB, chip and fast memory as the Amiga's.
// Chip memory is below the program (1MB to 4MB), its addresses 24 bits as
// the Amiga's (Btst(2,4) reads the top byte of ExecBase's address), and a
// chip block is followed by free chip memory, as there (the IFF packer reads
// past the end of a bitplane). Each is managed as exec's
// lists: blocks of 8 bytes, the first free one that fits, and any part of
// a block can be freed (AMOS does) or taken again (AllocAbs: Bank Shrink).
// The free ranges are a table sorted by address, apart from the memory:
// what is freed keeps its contents (exec only writes the first 8 bytes of
// it, and AMOS relies on the rest of a header it frees and takes again).
// Fast memory, when there is none left, comes from the chip arena.
struct alignas(8) Pool {
    guest::RangeAllocator free;
    bool has(const uint8_t* at) const {
        return free.contains(guest::abi::AddressCodec::decode<const guest::Byte>(address(at)));
    }
};
Pool chip_pool, fast_pool;
uint8_t *arena;
constexpr uint64_t ChipAt = 1ull << 20, ChipSize = 3ull << 20, ArenaSize = 512ull << 20;

void pool_start(Pool& pool, uint8_t* start, uint64_t size)
{
    // Temporary legacy backing boundary: logical and native addresses still
    // coincide until raw-pointer interpreter callers have migrated.
    auto at = guest::abi::AddressCodec::decode<guest::Byte>(address(start));
    if (pool.free.initialize(guest::GuestSpan<guest::Byte>(at, size)) != guest::Fault::none) {
        fprintf(stderr, "amos: could not initialize guest allocation ranges\n");
        __builtin_trap();
    }
}

void arena_start()
{
    if (arena)
        return;
#if defined(_WIN32)
    void *m = host::windows::memory_allocate(ArenaSize, true);
    if (!m) {
#else
    void *m = mmap(nullptr, ArenaSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT | MAP_NORESERVE,
                   -1, 0);
    if (m == MAP_FAILED) {
#endif
        fprintf(stderr, "amos: no memory below 4GB\n");
        __builtin_trap();
    }
    arena = static_cast<uint8_t *>(m);
#if defined(_WIN32)
    void *chip = host::windows::memory_allocate(ChipSize, true, reinterpret_cast<void *>(ChipAt));
#else
    int chip_flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;
#if defined(__linux__)
    chip_flags |= MAP_FIXED_NOREPLACE;
#endif
    void *chip = mmap(reinterpret_cast<void *>(ChipAt), ChipSize, PROT_READ | PROT_WRITE,
                      chip_flags, -1, 0);
#endif
    if (chip == reinterpret_cast<void *>(ChipAt)) {
        pool_start(chip_pool, static_cast<uint8_t *>(chip), ChipSize);
        pool_start(fast_pool, arena, ArenaSize);
    } else {
#if !defined(_WIN32)
        if (chip != MAP_FAILED)
            munmap(chip, ChipSize);
#endif
        // (taken: the chip memory at the start of the arena)
        pool_start(chip_pool, arena, ChipSize);
        pool_start(fast_pool, arena + ChipSize, ArenaSize - ChipSize);
    }
}

Pool &pool_of(const uint8_t *at) { return chip_pool.has(at) ? chip_pool : fast_pool; }

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
    if (!blocks) {
#if defined(_WIN32)
        blocks = static_cast<Block *>(host::windows::memory_allocate(sizeof(Block) * MaxBlocks, false));
        if (!blocks) {
            fprintf(stderr, "amos: no memory for allocation tracking (Windows error %u)\n", host::windows::memory_error());
            __builtin_trap();
        }
#else
        blocks = static_cast<Block *>(mmap(nullptr, sizeof(Block) * MaxBlocks, PROT_READ | PROT_WRITE,
                                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
#endif
    }
    int i = block_index(at);
    memmove(blocks + i + 1, blocks + i, sizeof(Block) * size_t(block_count - i));
    blocks[i] = {at, size};
    block_count++;
}

// The first free block of n bytes in a pool
uint8_t* take(Pool& pool, uint32_t size)
{
    auto result = pool.free.allocate(size);
    return result ? ptr<uint8_t>(guest::abi::AddressCodec::encode(result.value.data().address())) : nullptr;
}

void *allocate(uint32_t size, uint32_t flags)
{
    if (!size)
        return nullptr;
    arena_start();
    uint64_t n = rounded(size);
    uint8_t *found = flags & MemoryChip ? take(chip_pool, size) : take(fast_pool, size);
    if (!found && !(flags & (MemoryChip | MemoryFast)))
        found = take(chip_pool, size);
    if (!found)
        return nullptr;
    if (flags & 0x10000)
        memset(found, 0, n);
    if (memcheck())
        block_add(found, n);
    return found;
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
    const auto guest_address = guest::abi::AddressCodec::decode<guest::Byte>(address(at));
    if (pool_of(at).free.release(guest_address, size) != guest::Fault::none) {
        fprintf(stderr, "amos: invalid guest allocation release %p+%u\n", memory, size);
        __builtin_trap();
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
    auto guest_address = guest::abi::AddressCodec::decode<guest::Byte>(address(at));
    const auto result = pool_of(at).free.allocate_at(size, guest_address);
    if (!result) return nullptr;
    if (memcheck()) block_add(at, n);
    return where;
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

// (the requests in the 68000's layout, core/ioreq.h; io_Device the address
// of one of the devices here)
static int device_of(const IOReq *io)
{
    for (int i = 0; i < DeviceCount; i++)
        if (uint32_t(io->io_Device) == address(&devices[i]))
            return i;
    return -1;
}

int32_t open_device(const char *name, uint32_t, IOReq *io, uint32_t)
{
    for (int i = 0; i < DeviceCount; i++)
        if (!strcmp(name, device_names[i])) {
            devices[i].dd_Library.lib_Node.ln_Name = const_cast<char *>(device_names[i]);
            io->io_Device = address(&devices[i]);
            io->io_Error = 0;
            if (i == 3) {
                // serial.device's settings in the request, as Kickstart 3.1's
                // with the default preferences (measured): of the flags asked
                // for, SERF_SHARED stays
                auto *ser = reinterpret_cast<IOExtSer *>(io);
                ser->io_CtlChar = 0x11130000;
                ser->io_RBufLen = 512;
                ser->io_ExtFlags = 0;
                ser->io_Baud = 9600;
                ser->io_BrkTime = 250000;
                for (uint8_t &t : ser->io_TermArray)
                    t = 0;
                ser->io_ReadLen = 8;
                ser->io_WriteLen = 8;
                ser->io_StopBits = 1;
                ser->io_SerFlags &= 0x20;
                ser->io_Status = 0;
            }
            return 0;
        }
    return -1;
}
void close_device(IOReq *io) { io->io_Device = 0; }
// The narrator's write: the text said (the sound queued)
static void narrate(IOReq *io)
{
    if (io->io_Command == 3 && io->io_Data)       // CMD_WRITE
        host::speak(ptr<const char>(io->io_Data), uint32_t(io->io_Length));
}

// serial.device (no port behind it): SDCMD_SETPARAMS checks the settings as
// Kickstart 3.1's does (measured): a buffer under 64 bytes is SerErr_BufErr,
// a speed outside 110-292000 or XON/XOFF with both characters 0
// SerErr_InvParam; the others are done at once
static int32_t serial_io(IOReq *io)
{
    auto *ser = reinterpret_cast<IOExtSer *>(io);
    int8_t error = 0;
    if (uint16_t(io->io_Command) == 11) {       // SDCMD_SETPARAMS
        uint32_t baud = ser->io_Baud;
        if (uint32_t(ser->io_RBufLen) < 64)
            error = 4;
        else if (baud < 110 || baud > 292000 || (!(ser->io_SerFlags & 0x80) && !(uint32_t(ser->io_CtlChar) >> 16)))
            error = 5;
    } else if (uint16_t(io->io_Command) == 3) { // CMD_WRITE
        io->io_Actual = io->io_Length;
    }
    io->io_Error = error;
    return error;
}

int32_t do_io(IOReq *io)
{
    int device = device_of(io);
    if (device == 4) {
        narrate(io);
        while (host::speaking())        // (done when it has been heard)
            idle();
        return 0;
    }
    if (device == 3)
        return serial_io(io);
    if (device != 0 && device != 1)
        return 0;
    switch (uint16_t(io->io_Command)) {
    case 9:     // IND_ADDHANDLER
        host::input_handler(ptr<Interrupt>(io->io_Data));
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
void send_io(IOReq *io)
{
    if (device_of(io) == 4)
        narrate(io);
    else
        do_io(io);
}
IOReq *check_io(IOReq *io) { return device_of(io) == 4 && host::speaking() ? nullptr : io; }
void abort_io(IOReq *io)
{
    if (device_of(io) == 4)
        host::speech_stop();
}
int32_t wait_io(IOReq *io)
{
    while (device_of(io) == 4 && host::speaking())
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
