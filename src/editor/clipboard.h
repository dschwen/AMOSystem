#pragma once
#include "hal/clipboard.h"
namespace amos::ed {
#if !defined(__m68k__)
bool system_clipboard(uint32_t key);
void system_clipboard_command(hal::ClipboardCommand command);
void clipboard_restore(uint8_t *block, bool redo);
#else
inline bool system_clipboard(uint32_t) { return false; }
#endif
}
