// The host's own picture over AMOS's: messages, the hint for a disk AMOS asks
// for, the Devices panel (F12).

#include <stdio.h>

namespace amos::host {

// What happened to the disks, for the user: on stderr
void message(const char *text) { fprintf(stderr, "amos: %s\n", text); }

// The disk AMOS is waiting for (null: none)
void hint(const char *) {}

} // namespace amos::host
