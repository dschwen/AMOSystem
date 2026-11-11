// The AMCAF extension's data (the state the library keeps in its 9,142 bytes at ExtAdr), as far as the
// commands in C++ use it. Offsets are the library's own.
#pragma once

#include "core/amos.h"

namespace amos::amcaf {

enum : uint32_t {
    MouseShift = 750,           // word: the second mouse's scale (a shift)
    MouseOldX = 752,            // words: the counters of JOY1DAT as they were
    MouseOldY = 754,
    MouseX = 756,               // words: its position
    MouseY = 758,
    MouseMinX = 760,            // words: the limits (before the shift)
    MouseMinY = 762,
    MouseMaxX = 764,
    MouseMaxY = 766,
    VblMode = 662,              // word: 0 the player ticks from a CIA timer, -1 from the vertical blank
    VblInstalled = 696,         // word: the player's vertical blank routine is in (-1)
    CiaInstalled = 698,         // word: its CIA timer is in (-1)
    Module = 700,               // long: the module (Pt Play, Pt Bank)
    ModuleBank = 704,           // long: the number it was given as
    SampleBank = 708,           // long: Pt Sam Bank's sample bank
    CiaConstant = 712,          // long: the clock the timer's period is made from (the tempo divides it)
    Player = 716,               // long: the player's block (Pt Data Base)
    SampleVolume = 720,         // word: the volume of Pt Sam Play and Pt Raw Play (64)
    Notation = 722,             // word: Red Val and the others' colours, 4 (12 bits) or 8 (24 bits)
    AudioLocked = 1168,         // word: Audio Lock done (-1)
    Silence = 9138,             // long: four bytes of chip memory of zeros: what a sample effect ends on
};

uint8_t *state();
uint8_t *state_at(uint32_t at);

} // namespace amos::amcaf
