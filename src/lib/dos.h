// Files: names, the current file (Handle) and disk errors
#pragma once

#include "core/amos.h"
#include "lib/strings.h"

namespace amos {

constexpr uint32_t ModeOldFile = 1005;  // MODE_OLDFILE
constexpr uint32_t ModeNewFile = 1006;  // MODE_NEWFILE
constexpr int DEBase = 79;              // first disk error number

// AddRoutine: link a routine record (a link, then code) into list, one
// of the Sys_ClearRoutines... lists, unless it is in already
void add_routine(uint8_t *record, uint32_t list);

// Dsk.PathIt: complete the name in Name1 with the current directory
void path_it();
// NomDisc: the AMOS string s as a file name in Name1 (completed with the
// current directory). Returns Name1.
char *name_disc(GuestString s);

// D_Open / D_OpenD1: open the current file (Handle); D_Close closes it
// again, here or when an error stops the program. Returns the handle or 0.
uint32_t file_open(uint32_t mode);
uint32_t file_open(const char *name, uint32_t mode);
void file_close();
// D_Read, D_Write: true if all of it was done
bool file_read(void *buffer, uint32_t length);
bool file_write(const void *buffer, uint32_t length);
// D_Seek: the old position, negative on error
int32_t file_seek(int32_t offset, int32_t mode);

// DiskError: the error of the last DOS call; DiskErr: DOS error number
// code as an AMOS error
[[noreturn]] void disk_error();
[[noreturn]] void disk_err(int32_t code);

} // namespace amos
