// Services of +W.s for the C++ parts of AMOS.library

#include "core/amiga.h"
#include "w/w.h"
#include "hal/system.h"

namespace amos::w {

// FastMm... (w/memory.cpp)
uint32_t fast_mm(uint32_t size) { return reserve(size, MemClear | MemPublic); }
uint32_t fast_mm2(uint32_t size) { return reserve(size, MemPublic); }
uint32_t chip_mm(uint32_t size) { return reserve(size, MemClear | MemPublic | MemChip); }
uint32_t chip_mm2(uint32_t size) { return reserve(size, MemPublic | MemChip); }
void free_mm(uint32_t address, uint32_t size) { release(address, size); }

uint32_t call(host::RoutineArg routine)
{
    Regs r;
    amos_call(&r, routine);
    return r.d[0];
}

void copy(void *to, const void *from, uint32_t n)
{
    auto t = static_cast<uint8_t *>(to);
    auto f = static_cast<const uint8_t *>(from);
    while (n--)
        *t++ = *f++;
}

// WVbl: until the next VBL (the interrupt counts them)
void wait_vbl()
{
    const volatile be_u32 &count = wdata().VblCount;
    uint32_t now = count;
    while (count == now)
        hal::idle();
}

} // namespace amos::w
