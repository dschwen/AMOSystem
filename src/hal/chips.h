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
// The parallel port: its data lines (CIA A's port B) and its control lines (CIA B's port A: busy, paper
// out, select)
inline volatile uint8_t &parallel_data() { return *reinterpret_cast<volatile uint8_t *>(0xBFE101); }
inline volatile uint8_t &parallel_control() { return *reinterpret_cast<volatile uint8_t *>(0xBFD000); }
#else
volatile struct Custom &custom();
volatile uint8_t &cia_a_pra();
volatile uint8_t &parallel_data();
volatile uint8_t &parallel_control();
#endif

// The blitter started on the registers set (BLTSIZE written with size: its
// height << 6 | its width in words)
inline void blit(uint16_t size) { custom().bltsize = size; }

// A register of 16 bits (another host's emulates the chips: host/include/
// hardware/custom.h)
#if defined(__mc68000__)
using ChipWord = uint16_t;
#else
using ChipWord = ChipReg;
#endif

// The custom registers by their offset (as the 16 bits at $DFF000+offset)
inline volatile ChipWord &custom_word(uint32_t offset)
{
    return *reinterpret_cast<volatile ChipWord *>(reinterpret_cast<volatile uint8_t *>(&custom()) + offset);
}

} // namespace amos::hal
