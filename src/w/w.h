// Shared by the C++ parts of AMOS.library (src/w): the custom chips and
// the assembler routines of +W.s they still use.
#pragma once

#include "core/amos.h"

#include <hardware/custom.h>

// +W.s, exported under _w_ names
extern "C" uint8_t w_tabk1[], w_tabk2[], w_tabk3[], w_tabk4[], w_fast_mm[], w_fast_mm2[], w_chip_mm[], w_chip_mm2[], w_free_mm[],
    w_sy_set_z[], w_ec_set_clip[], w_eff_pat[], w_ec_push[], w_ec_pull[];

namespace amos::w {

// EcFlags BitDble
constexpr uint16_t DoubleBuffer = 1 << (8 + 5);
constexpr uint32_t EntNul = 0x80000000;

inline volatile struct Custom &chips() { return *reinterpret_cast<volatile struct Custom *>(0xDFF000); }

inline Screen *current_screen() { return ptr<Screen>(wdata().EcCourant); }

// FastMm (cleared), FastMm2, ChipMm (cleared): 0 if no memory
uint32_t fast_mm(uint32_t size);
uint32_t fast_mm2(uint32_t size);
uint32_t chip_mm(uint32_t size);
uint32_t chip_mm2(uint32_t size);     // not cleared
void free_mm(uint32_t address, uint32_t size);

// Calls a +W.s routine with no parameters (TAbk1-4...). Returns its d0.
uint32_t call(uint8_t *routine);

void copy(void *to, const void *from, uint32_t n);

} // namespace amos::w
