// Guest addresses of another host. A guest address is a 32-bit offset from where the guest memory
// is mapped: from amos_guest_base (AMOS_GUEST_BIASED: anywhere, above 4GB if it is), or from 0 (the
// identity mapping: the guest's memory is below 4GB). The first 4K of an Amiga is its vectors, which
// programs read (ExecBase at 4: Leek(4)); a page of its own stands for them. 0 stays null.
#pragma once
#include <stdint.h>
#if !defined(__mc68000__)
extern "C" uint8_t amos_low_page[4096];
#if defined(AMOS_GUEST_BIASED)
extern "C" uint8_t *amos_guest_base;
extern "C" uint64_t amos_guest_size;
// A native address that is not guest memory, asked for as a guest address: it gets a guest address
// in a window of the handle region (HandleBase up), the 1MB of native address space around it;
// to_native turns it back (src/host/context.cpp; nullptr: not one). The 4GB of guest addresses
// are reserved, so that any word is a native address that faults when used. Windows keep addresses of native objects equal,
// ordered and incrementable within a MB, as a flat 32-bit address of them was.
extern "C" uint32_t amos_address_escape(uintptr_t p, const char *file, int line);
extern "C" uint8_t *amos_handle_native(uint32_t address);
constexpr uint32_t AmosHandleBase = 0xC0000000u;
#endif
namespace amos::bias {
inline uint8_t *to_native(uint32_t address)
{
    if (address < 4096) return address ? amos_low_page + address : nullptr;
#if defined(AMOS_GUEST_BIASED)
    if (address >= AmosHandleBase)
        if (uint8_t *native = amos_handle_native(address)) return native;
    return amos_guest_base + address;
#else
    return reinterpret_cast<uint8_t *>(uintptr_t(address));
#endif
}
// Is it guest memory (and so has a guest address)?
inline bool contains(uintptr_t at)
{
#if defined(AMOS_GUEST_BIASED)
    return !at || at - reinterpret_cast<uintptr_t>(amos_low_page) < 4096 ||
           at - reinterpret_cast<uintptr_t>(amos_guest_base) < amos_guest_size;
#else
    return at <= UINT32_MAX;
#endif
}
inline uint32_t to_guest(uintptr_t at, const char *file = __builtin_FILE(), int line = __builtin_LINE())
{
#if defined(AMOS_GUEST_BIASED)
    if (!at) return 0;
    auto low = reinterpret_cast<uintptr_t>(amos_low_page);
    if (at - low < 4096) return uint32_t(at - low);
    uint64_t offset = at - reinterpret_cast<uintptr_t>(amos_guest_base);
    if (offset >= amos_guest_size) return amos_address_escape(at, file, line);
    return uint32_t(offset);
#else
    (void)file; (void)line;
    return uint32_t(at);
#endif
}
}
#endif
