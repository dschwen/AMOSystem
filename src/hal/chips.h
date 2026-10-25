// The Amiga's custom chips and CIAs as AMOS.library drives them (Phase 7:
// the portability layer). On the Amiga they are the hardware; another
// host gives an emulation of the registers the display, the blitter, the
// sprites, the audio and the input read: its renderer composes each frame
// from the copper lists and bitplanes AMOS builds in its memory, as the
// chips do.
#pragma once

#include <stdint.h>

#include <hardware/custom.h>

namespace amos::hal {

constexpr uint32_t CustomBase = 0xDFF000, CiaABase = 0xBFE001;

#if defined(__mc68000__)

inline volatile struct Custom &custom() { return *reinterpret_cast<volatile struct Custom *>(CustomBase); }
// CIA A's port A (the fire buttons, the filter LED...)
inline volatile uint8_t &cia_a_pra() { return *reinterpret_cast<volatile uint8_t *>(CiaABase); }
#else
volatile struct Custom &custom();
volatile uint8_t &cia_a_pra();
#endif

// The custom registers by their offset (as the 16 bits at $DFF000+offset)
inline volatile uint16_t &custom_word(uint32_t offset)
{
    return *reinterpret_cast<volatile uint16_t *>(reinterpret_cast<volatile uint8_t *>(&custom()) + offset);
}

} // namespace amos::hal
