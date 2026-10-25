// AMOS Professional on another host: the start (main/boot.cpp's on the
// Amiga). Not running yet: the display, the input and the audio backends
// and the byte order of AMOS's data are still to come.

#include "host/host.h"

#include <stdio.h>

int main(int, char **)
{
    fprintf(stderr, "AMOS: this host's build is not complete yet\n");
    return 1;
}
