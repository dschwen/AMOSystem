// AMCAF: bits, banks, the beam, the second mouse's buttons (R019/R01a, R0c0/R0c1, R0ab, R0ce/R0cf,
// R0db, R017). The registers are the chips' (hal/chips.h: the hardware on the Amiga, its emulation on
// another host).

#include "ext/amcaf.h"
#include "hal/chips.h"
#include "hal/system.h"
#include "lib/banks.h"

using namespace amos;

namespace {

// Bank Permanent / Temporary: the flag that Erase Temp leaves alone (R019, R01a)
void bank_data_flag(int32_t bank, bool on)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    uint16_t flags = bank_flags(b);
    poke_w(b - 12, on ? uint16_t(flags | BankData) : uint16_t(flags & ~BankData));
}

// The beam's line: VPOSR's bit 0 (line bit 8) and VHPOSR's high byte (R0ce's d0 after its LSR)
int16_t beam_line()
{
    volatile struct Custom &c = hal::custom();
    uint16_t vposr = c.vposr, vhposr = c.vhposr;
    return int16_t(uint16_t((vposr & 0xFF) << 8 | vhposr >> 8));
}

} // namespace

extern "C" {

// BANK PERMANENT bank
void amcaf_bank_permanent(int32_t bank) { bank_data_flag(bank, true); }

// BANK TEMPORARY bank
void amcaf_bank_temporary(int32_t bank) { bank_data_flag(bank, false); }

// =ODD(n): -1 if bit 0 is set
int32_t amcaf_odd(int32_t n) { return -(n & 1); }

// =EVEN(n): -1 if bit 0 is clear
int32_t amcaf_even(int32_t n) { return n & 1 ? 0 : -1; }

// RASTER WAIT y (R0ce): until the beam is at line y or below, or has gone round to the top of the next
// frame (a line that is never reached does not wait for ever); line 0 does not count
void amcaf_raster_wait_1(int32_t y)
{
    int16_t line = 0;
    for (;;) {
        int16_t before = line;
        line = beam_line();
        if (line >= before && line < int16_t(y)) {
            hal::idle();
            continue;
        }
        if (line == 0)
            continue;
        return;
    }
}

// RASTER WAIT x,y (R0cf): the same, then until the beam's horizontal byte (VHPOSR's low byte) is at
// x / 2 (a signed byte compare). The test for line 0 that follows reads the line's high byte with that
// byte: it repeats only when both are 0
void amcaf_raster_wait_2(int32_t x, int32_t y)
{
    int8_t column = int8_t(uint32_t(x) >> 1);
    int16_t line = 0;
    for (;;) {
        int16_t before = line;
        line = beam_line();
        if (line >= before) {
            if (line < int16_t(y)) {
                hal::idle();
                continue;
            }
            int8_t h;
            while ((h = int8_t(uint16_t(hal::custom().vhposr) & 0xFF)) < column)
                hal::idle();
            line = int16_t((uint16_t(line) & 0xFF00) | uint8_t(h));
        }
        if (line == 0)
            continue;
        return;
    }
}

// FLUSH LIBS: the libraries that nothing uses are given up by the system, by asking for all of its memory
// (nothing to flush on another host)
void amcaf_flush_libs() {}

// AUDIO LOCK: the audio channels taken from the system (audio.device), to be played by AMCAF's own
// routines. Nothing to take on another host: the sound is AMOS's alone there
void amcaf_audio_lock() { poke_w(amcaf::state_at(amcaf::AudioLocked), 0xFFFF); }

// =SMOUSE KEY: the buttons of the mouse in the second port: 1 left (CIA A /FIR1), 2 right (POTGOR's
// DATRY), 4 middle (DATRX)
int32_t amcaf_smouse_key()
{
    volatile struct Custom &c = hal::custom();
    c.potgo = 0xF000;
    int32_t keys = 0;
    if (!(hal::cia_a_pra() & 0x80))
        keys += 1;
    uint16_t pins = c.potinp;
    if (!(pins & 0x4000))
        keys += 2;
    if (!(pins & 0x1000))
        keys += 4;
    return keys;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_misc.inc"
#endif
