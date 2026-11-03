#pragma once
#include "hal/clipboard.h"
namespace amos::ed {
#if !defined(__m68k__)
// Table coordinates are four bytes followed by four bytes of metadata.
// Slot 1 belongs to the AMOS menu, rebuilt after the static menus.
inline uint8_t clipboard_menu_slot(const uint8_t *table)
{
    bool used[256] = {}; used[1] = true;
    for (const uint8_t *t = table; t[0] || t[1] || t[2] || t[3]; t += 8) used[t[0]] = true;
    for (unsigned n = 2; n < 256; ++n) if (!used[n]) return uint8_t(n);
    return 0;
}
bool system_clipboard(uint32_t key);
void system_clipboard_command(hal::ClipboardCommand command);
void clipboard_restore(uint8_t *block, bool redo);
#else
inline bool system_clipboard(uint32_t) { return false; }
#endif
}
