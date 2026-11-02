#pragma once
#include <stdint.h>

namespace amos::hal {
enum class ClipboardCommand { None, Copy, Cut, Paste, Ignored };
#if !defined(__m68k__)
// Called on the host video/AMOS thread, outside input interrupt callbacks.
void clipboard_direct_active(bool active);
bool clipboard_read(char *&text);
bool clipboard_write(const char *text);
void clipboard_release(char *text);
void clipboard_message(const char *text);
ClipboardCommand clipboard_command(uint32_t key);
#else
inline void clipboard_direct_active(bool) {}
inline ClipboardCommand clipboard_command(uint32_t) { return ClipboardCommand::None; }
#endif
}
