// Sprites and bobs (lib/sprites.cpp)
#pragma once

#include "core/amos.h"

namespace amos {

// AdBob / AdIcon: the entry of image n of the sprite / icon bank (bits
// 14-15, the flips, left out); errors if there is none. *count gets the
// number of images.
uint8_t *image_entry(uint32_t n, bool icon, uint16_t *count = nullptr);

// Paste: image n (its bank entry) at x,y of the current screen
void paste(uint32_t x, uint32_t y, uint32_t n, uint8_t *entry);

} // namespace amos
