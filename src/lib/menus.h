// The menu engine of AMOSPro.Lib (lib/menus.cpp), for the editor
#pragma once

#include "core/amos.h"

namespace amos::menu {

// MnFlag: the flags are its high byte; the low byte is set by Menu Called
enum : uint8_t {
    MnFlat = 1 << 0,    // first item of a branch: MnPrev is the parent
    MnFixed = 1 << 1,   // position set by the program or dragged
    MnSep = 1 << 2,     // starts a new box (Menu Separate)
    MnBar = 1 << 3,     // the next item goes below (else to the right)
    MnOff = 1 << 4,     // inactive
    MnTotal = 1 << 5,   // the box is as wide as the screen
    MnTBouge = 1 << 6,  // the whole branch can be dragged
    MnBouge = 1 << 7,   // the item can be dragged in its box
};

inline uint8_t &flags(MenuItem *m) { return reinterpret_cast<uint8_t *>(&m->MnFlag)[0]; }

// MnFind / MnIns: the item at coords (the deepest first), depth of them
#if defined(__mc68000__)
using Coordinates = const be_u32*;
inline Coordinates coordinates(ContextCursor cursor) { return reinterpret_cast<const be_u32*>(cursor); }
#else
using Coordinates = guest::GuestPtr<const guest::Byte>;
inline Coordinates coordinates(ContextCursor cursor) { return guest_cursor(cursor); }
#endif
MenuItem *find(Coordinates coords, uint16_t depth, uint32_t *last = nullptr);
MenuItem *insert(Coordinates coords, uint16_t depth);
// MnEff: item first and what it opens (with siblings, the items after it
// too); MnRaz: the whole menu
void erase(uint32_t first, uint16_t siblings);
void erase_all();
// MnObjet: the object of a string (a length word, the text); 0 out of
// memory, -1 a bad string
uint32_t compile_object(const uint8_t *s);

} // namespace amos::menu
