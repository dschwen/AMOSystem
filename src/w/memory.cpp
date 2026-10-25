// The memory of AMOS.library (SyCall MemReserve, MemFree...): AllocMem,
// and when that fails the flush routines (T_MemFlush) are called and it
// tries once more. Lists of routines (WAddRoutine/WCallRoutines): each is
// a long (the next one / 2, bit 31 set while in the list) followed by its
// code.

#include "core/amiga.h"
#include "w/w.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::w;

namespace amos::w {

// WCallRoutines: call the routines of the list (which empties)
void call_routines(uint32_t *list)
{
    uint32_t next = *list;
    if (!next)
        return;
    *list = 0;
    while (next) {
        uint8_t *routine = ptr<uint8_t>(next);
        next = peek_l(routine) << 1;
        poke_l(routine, 0);
        AllRegs r = {};
        for (int i = 0; i < 8; i++)
            r.d[i] = 0;
        r.d[0] = next;
        r.a[0] = address(routine);
        r.a[3] = address(ctx().a3);
        r.a[4] = address(ctx().a4);
        r.a[5] = address(ctx().a5);
        r.a[6] = address(ctx().a6);
        amos_call_all(&r, routine + 4);
    }
}

// WMemReserve: size bytes with the AllocMem flags; 0 if none
uint32_t reserve(uint32_t size, uint32_t flags)
{
    void *m = hal::allocate(size, flags);
    if (!m) {
        call_routines(&wdata().MemFlush);
        m = hal::allocate(size, flags);
    }
    return address(m);
}

void release(uint32_t address, uint32_t size) { hal::release(ptr<void>(address), size); }

// WAddRoutine: the routine into the list, if it is in none; returns its
// link
uint32_t add_routine(uint32_t routine, uint32_t list)
{
    uint8_t *r = ptr<uint8_t>(routine);
    if (int32_t(peek_l(r)) < 0)
        return peek_l(r);
    uint32_t *l = ptr<uint32_t>(list);
    uint32_t link = (*l >> 1) | 0x80000000;
    poke_l(r, link);
    *l = routine;
    return link;
}

} // namespace amos::w

extern "C" {
void w_sy_mem_reserve(AllRegs *r) { r->a[0] = reserve(r->d[0], r->d[1]); }
void w_sy_mem_fast_clear(AllRegs *r) { r->a[0] = reserve(r->d[0], MemPublic | MemClear); }
void w_sy_mem_fast(AllRegs *r) { r->a[0] = reserve(r->d[0], MemPublic); }
void w_sy_mem_chip_clear(AllRegs *r) { r->a[0] = reserve(r->d[0], MemChip | MemPublic | MemClear); }
void w_sy_mem_chip(AllRegs *r) { r->a[0] = reserve(r->d[0], MemChip | MemPublic); }
void w_sy_mem_free(AllRegs *r) { release(r->a[1], r->d[0]); }

// WAddRoutine: routine a1 into the list a2 (if not in one)
void w_sy_add_routine(AllRegs *r)
{
    if (int32_t(peek_l(ptr<void>(r->a[1]))) < 0)
        return;
    r->d[0] = add_routine(r->a[1], r->a[2]);
}

void w_sy_add_flush_routine(AllRegs *r)
{
    AllRegs a = *r;
    a.a[2] = address(&wdata().MemFlush);
    w_sy_add_routine(&a);
    r->d[0] = a.d[0];
}

void w_sy_call_routines(AllRegs *r)
{
    call_routines(ptr<uint32_t>(r->a[1]));
    r->d[0] = 0;
}

void w_sy_mem_flush(AllRegs *r)
{
    call_routines(&wdata().MemFlush);
    r->d[0] = 0;
}
}
