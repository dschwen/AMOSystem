// The AMCAF extension's data (the state the library keeps in its 9,142 bytes at ExtAdr), as far as the
// commands in C++ use it. Offsets are the library's own.
#pragma once

#include "core/amos.h"

namespace amos::amcaf {

enum : uint32_t {
    AudioLocked = 1168,         // word: Audio Lock done (-1)
};

uint8_t *state();
uint8_t *state_at(uint32_t at);

} // namespace amos::amcaf
