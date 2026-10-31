// The files of the host system (Phase 7: the portability layer). AMOS
// keeps the values of AmigaDOS: a file and a lock are 32 bits (0: none),
// the modes and origins are dos.library's, the directory entries are
// FileInfoBlocks. hal/files_amiga.cpp is dos.library; another host
// implements the same.
#pragma once

#include <stdint.h>

namespace amos::hal {

// The system's files before AMOS's data exist (the start): its library
void files_start(void *dos);

using File = uint32_t;
using DirLock = uint32_t;

constexpr int32_t ModeReadWrite = 1004, ModeOldFile = 1005, ModeNewFile = 1006;
constexpr int32_t SeekBeginning = -1, SeekCurrent = 0, SeekEnd = 1;
constexpr int32_t LockShared = -2, LockExclusive = -1;

File open(const char *name, int32_t mode);
void close(File f);
int32_t read(File f, void *buffer, int32_t length);
int32_t write(File f, const void *buffer, int32_t length);
int32_t seek(File f, int32_t offset, int32_t origin);   // the old position, -1 on error
bool wait_for_char(File f, int32_t microseconds);
File output();                                          // the console's, or 0
int32_t io_error();                                     // the error of the last call

DirLock lock(const char *name, int32_t mode);
void unlock(DirLock l);
bool examine(DirLock l, void *fib);                     // a FileInfoBlock
bool examine_next(DirLock l, void *fib);
DirLock current_dir(DirLock l);                         // the old one
DirLock parent_dir(DirLock l);
bool info(DirLock l, void *info_data);                  // an InfoData
// The name of the volume of an InfoData (info's), false if none
bool volume_name(const void *info_data, char *name, int size);
bool assign_lock(const char *name, DirLock l);
bool device_exists(const char *name);                   // "name:" a device
// The devices and assigns of DOS (their names, without the colon): f for
// each, device true for a device (not an assign)
void each_device(void (*f)(const char *name, bool device, void *data), void *data);


bool delete_file(const char *name);
bool rename(const char *from, const char *to);
DirLock create_dir(const char *name);
bool execute(const char *command, File input, File output);
void delay(int32_t ticks);                              // in 50ths of a second

} // namespace amos::hal
