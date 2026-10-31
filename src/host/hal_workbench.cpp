// hal/workbench.h on another host: no Workbench, icons or ARexx

#include "hal/workbench.h"

#include <stdio.h>

#include <devices/inputevent.h>

namespace amos::hal {

void workbench_start(void *, void *) {}
void panic_requester(const char *text) { fprintf(stderr, "AMOS: %s\n", text); }
bool open_workbench() { return true; }
bool close_workbench() { return true; }
::Screen *open_screen(NewScreen *) { return nullptr; }
void close_screen(::Screen *) {}
bool put_icon(const char *, DiskObject *) { return false; }
DiskObject *get_icon(const char *) { return nullptr; }
void free_icon(DiskObject *) {}
// RawKeyConvert with the USA keymap: the character of a raw key, with
// the shifts and Caps Lock; Control makes control characters
int32_t key_to_text(InputEvent *e, char *buffer, int32_t length, void *)
{
    static const uint8_t normal[0x41] = {96, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 45, 61, 92, 0, 48, 113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 0, 49, 50, 51, 97, 115, 100, 102, 103, 104, 106, 107, 108, 59, 39, 0, 0, 52, 53, 54, 60, 122, 120, 99, 118, 98, 110, 109, 44, 46, 47, 0, 46, 55, 56, 57, 32};
    static const uint8_t shifted[0x41] = {126, 33, 64, 35, 36, 37, 94, 38, 42, 40, 41, 95, 43, 124, 0, 48, 81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 123, 125, 0, 49, 50, 51, 65, 83, 68, 70, 71, 72, 74, 75, 76, 58, 34, 0, 0, 52, 53, 54, 62, 90, 88, 67, 86, 66, 78, 77, 60, 62, 63, 0, 46, 55, 56, 57, 32};
    uint16_t code = e->ie_Code;
    if (code & 0x80 || length < 1)
        return 0;
    char c = 0;
    if (code <= 0x40) {
        bool shift = e->ie_Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
        c = char(shift ? shifted[code] : normal[code]);
        if (!shift && e->ie_Qualifier & IEQUALIFIER_CAPSLOCK && c >= 'a' && c <= 'z')
            c = char(c - 32);
    } else {
        switch (code) {
        case 0x41: c = 8; break;
        case 0x42: c = 9; break;
        case 0x43:
        case 0x44: c = 13; break;
        case 0x45: c = 27; break;
        case 0x46: c = 127; break;
        case 0x4A: c = '-'; break;
        case 0x5A: c = '('; break;
        case 0x5B: c = ')'; break;
        case 0x5C: c = '/'; break;
        case 0x5D: c = '*'; break;
        case 0x5E: c = '+'; break;
        }
    }
    if (!c)
        return 0;
    if (e->ie_Qualifier & IEQUALIFIER_CONTROL && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
        c = char(c & 0x1F);
    buffer[0] = c;
    return 1;
}
bool is_rexx_message(Message *) { return false; }
uint8_t *rexx_text(const char *, uint32_t) { return nullptr; }

} // namespace amos::hal
