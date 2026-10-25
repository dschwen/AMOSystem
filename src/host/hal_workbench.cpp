// hal/workbench.h on another host: no Workbench, icons or ARexx

#include "hal/workbench.h"

#include <stdio.h>

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
int32_t key_to_text(InputEvent *, char *, int32_t, void *) { return 0; }
bool is_rexx_message(Message *) { return false; }
uint8_t *rexx_text(const char *, uint32_t) { return nullptr; }

} // namespace amos::hal
