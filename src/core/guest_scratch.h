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
inline uint32_t guest_callback(void (*fn)()) { return address(reinterpret_cast<const void *>(fn)); }
inline const void *callback_target(uint32_t word) { return ptr<const void>(word); }

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

// A C++ function the guest holds as a code address (a VBL routine): a marker cell of guest memory
// stands for it, the same one every time; callback_target finds the function again (else the word is
// the address of the assembler's code, as before)
struct CallbackMarkers {
    static constexpr int Count = 32;
    void (*fn[Count])() = {};
    uint8_t *cells = nullptr;
    int count = 0;
};
inline CallbackMarkers &callback_markers()
{
    static CallbackMarkers markers;
    return markers;
}

inline uint32_t guest_callback(void (*fn)())
{
    CallbackMarkers &m = callback_markers();
    for (int i = 0; i < m.count; i++)
        if (m.fn[i] == fn)
            return address(m.cells + 4 * i);
    if (!m.cells)
        m.cells = static_cast<uint8_t *>(hal::allocate(4 * CallbackMarkers::Count, hal::MemoryFast | 0x10000));
    if (!m.cells || m.count == CallbackMarkers::Count)
        return address(reinterpret_cast<const void *>(fn));
    m.fn[m.count] = fn;
    return address(m.cells + 4 * m.count++);
}

inline const void *callback_target(uint32_t word)
{
    CallbackMarkers &m = callback_markers();
    for (int i = 0; i < m.count; i++)
        if (word == address(m.cells + 4 * i))
            return reinterpret_cast<const void *>(m.fn[i]);
    return ptr<const void>(word);
}

#endif

} // namespace amos
