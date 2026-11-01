// The topaz fonts of the host (build/host/topaz.cpp, made by tools/extract_topaz.py from the
// Kickstart, or AROS's topaz 8 from data/fonts/topaz8): used by host/fonts.cpp for AMOS's own
// text and by host/overlay.cpp for the host's.
#pragma once

#include <stdint.h>

namespace amos::host {

struct RomFont {
    uint16_t ysize, xsize, baseline, boldsmear, modulo;
    uint8_t style, flags, lo, hi;
    const uint8_t *data;
    const uint32_t *locs;
};

extern const RomFont rom_fonts[];
extern const int rom_font_count;

} // namespace amos::host
