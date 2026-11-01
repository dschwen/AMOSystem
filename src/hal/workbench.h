// The host's desktop as AMOS meets it (Phase 7: the portability layer):
// the Workbench and Intuition's screens, the icons of the programs, the
// keyboard's conversion to characters, ARexx's messages.
// hal/workbench_amiga.cpp is intuition.library, icon.library,
// console.device and rexxsyslib.library.
#pragma once

#include <stdint.h>

struct Screen;
struct NewScreen;
struct DiskObject;
struct InputEvent;
struct Message;

namespace amos::hal {

// Intuition and icons before AMOS's data exist (the start): their libraries
void workbench_start(void *intuition, void *icon);

// A message in a requester, with " Cancel " (AMOS's panic)
void panic_requester(const char *text);

bool open_workbench();
bool close_workbench();
// AMOS to the front (true) or behind the Workbench: on another host, the
// window takes the mouse pointer, or gives it back
void amos_in_front(bool front);
::Screen *open_screen(NewScreen *ns);
void close_screen(::Screen *s);

bool put_icon(const char *name, DiskObject *icon);         // PutDiskObject
DiskObject *get_icon(const char *name);                    // GetDiskObject
void free_icon(DiskObject *icon);

// The characters of a key (RawKeyConvert, of the console device given)
int32_t key_to_text(InputEvent *event, char *buffer, int32_t length, void *console);

// ARexx: a message of ARexx?, a text for it
bool is_rexx_message(Message *m);
uint8_t *rexx_text(const char *text, uint32_t length);

} // namespace amos::hal
