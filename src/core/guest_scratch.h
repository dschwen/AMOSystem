// Native text and constants the guest reads. On the Amiga they are guest memory already; elsewhere
// a native object has no guest address, so what the guest is to read is given some.
#pragma once
#include "core/amos.h"
#include "hal/system.h"
#include <string.h>

namespace amos {

// An object of the system (a task, a device, an IO request...) in guest memory, zeroed, for the life
// of the program: its address is the guest's
template <typename T> inline T *guest_object(size_t count = 1)
{
    return static_cast<T *>(hal::allocate(uint32_t(sizeof(T) * count), hal::MemoryFast | 0x10000));
}

#if defined(__mc68000__) || !defined(AMOS_GUEST_BIASED)

inline uint32_t guest_text(const void *text) { return address(text); }
inline uint32_t guest_constant(const void *data, size_t) { return address(data); }

#else

// A text for a call that reads it at once (wi::Print...): the address of text when it is guest memory,
// else of a copy in a ring of guest scratch memory
inline uint32_t guest_text(const void *text_)
{
    const char *text = static_cast<const char *>(text_);
    if (bias::contains(reinterpret_cast<uintptr_t>(text)))
        return address(text);
    constexpr uint32_t Size = 16384;
    static uint8_t *ring;
    static uint32_t at;
    if (!ring)
        ring = static_cast<uint8_t *>(hal::allocate(Size, hal::MemoryFast | 0x10000));
    size_t n = strlen(text) + 1;
    if (n > Size)
        n = Size;
    if (at + n > Size)
        at = 0;
    uint8_t *to = ring + at;
    memcpy(to, text, n);
    to[n - 1] = 0;
    at += uint32_t((n + 1) & ~size_t(1));
    return address(to);
}

// Constant data the guest reads later (a table, a message, an envelope): one copy in guest memory
// for the life of the program, found again by where it came from
inline uint32_t guest_constant(const void *data, size_t size)
{
    if (bias::contains(reinterpret_cast<uintptr_t>(data)))
        return address(data);
    struct Copy { const void *from; uint32_t to; };
    static Copy copies[128];
    static int count;
    for (int i = 0; i < count; i++)
        if (copies[i].from == data)
            return copies[i].to;
    void *to = hal::allocate(uint32_t(size), hal::MemoryFast);
    if (!to || count == 128)
        return address(data);
    memcpy(to, data, size);
    copies[count++] = {data, address(to)};
    return copies[count - 1].to;
}

#endif

} // namespace amos
