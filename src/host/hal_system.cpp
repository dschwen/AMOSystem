// hal/system.h on another host: AMOS's memory below 4GB (it keeps its
// addresses in 32 bits), one task, no devices or interrupts yet.

#include "hal/system.h"
#include "host/host.h"

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include <exec/tasks.h>

namespace {

Task the_task;

} // namespace

namespace amos::hal {

uint32_t available_memory(uint32_t flags)
{
    if (flags & MemoryChip)
        return 2 * 1024 * 1024;
    return 8 * 1024 * 1024;
}

void forbid() {}
void permit() {}

// Each block mapped on its own, below 4GB; cleared as mmap's are
void *allocate(uint32_t size, uint32_t)
{
    if (!size)
        return nullptr;
    void *m = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT, -1, 0);
    return m == MAP_FAILED ? nullptr : m;
}

void release(void *memory, uint32_t size)
{
    if (memory)
        munmap(memory, size);
}

void *allocate_at(uint32_t, void *) { return nullptr; }

Task *find_task(const char *name) { return name ? nullptr : &the_task; }
void signal(Task *, uint32_t) {}
static uint32_t the_requester_window;
uint32_t requester_window() { return the_requester_window; }
void set_requester_window(uint32_t window) { the_requester_window = window; }

Library *open_library(const char *, uint32_t) { return nullptr; }
void close_library(Library *) {}
uint16_t os_version() { return 40; }
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

void add_vbl_server(Interrupt *) {}
void remove_vbl_server(Interrupt *) {}
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
