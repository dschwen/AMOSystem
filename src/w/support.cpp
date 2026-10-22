// Services of +W.s for the C++ parts of AMOS.library

#include "w/w.h"

namespace amos::w {

static uint32_t memory(uint8_t *routine, uint32_t size)
{
    Regs r;
    r.d[0] = size;
    amos_call(&r, routine);
    return r.d[0];
}

uint32_t fast_mm(uint32_t size) { return memory(w_fast_mm, size); }
uint32_t fast_mm2(uint32_t size) { return memory(w_fast_mm2, size); }
uint32_t chip_mm(uint32_t size) { return memory(w_chip_mm, size); }

void free_mm(uint32_t address, uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[1] = address;
    amos_call(&r, w_free_mm);
}

uint32_t call(uint8_t *routine)
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

} // namespace amos::w
