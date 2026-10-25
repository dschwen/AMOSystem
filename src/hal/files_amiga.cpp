// hal/files.h on the Amiga: dos.library

#include "hal/files.h"

#include "core/amos.h"
#include "core/amiga.h"

namespace {
void *dos_library;      // dos.library before the data zone exists
}
#define DOS_BASE_NAME \
    (dos_library ? static_cast<struct DosLibrary *>(dos_library) : amos::ptr<struct DosLibrary>(amos::idata().DosBase))
#include <proto/dos.h>

namespace amos::hal {

void files_start(void *dos) { dos_library = dos; }

File open(const char *name, int32_t mode) { return File(Open(name, mode)); }
void close(File f) { Close(BPTR(f)); }
int32_t read(File f, void *buffer, int32_t length) { return Read(BPTR(f), buffer, length); }
int32_t write(File f, const void *buffer, int32_t length) { return Write(BPTR(f), const_cast<void *>(buffer), length); }
int32_t seek(File f, int32_t offset, int32_t origin) { return Seek(BPTR(f), offset, origin); }
bool wait_for_char(File f, int32_t microseconds) { return WaitForChar(BPTR(f), microseconds); }
File output() { return File(Output()); }
int32_t io_error() { return IoErr(); }

DirLock lock(const char *name, int32_t mode) { return DirLock(Lock(name, mode)); }
void unlock(DirLock l) { UnLock(BPTR(l)); }
bool examine(DirLock l, void *fib) { return Examine(BPTR(l), static_cast<FileInfoBlock *>(fib)); }
bool examine_next(DirLock l, void *fib) { return ExNext(BPTR(l), static_cast<FileInfoBlock *>(fib)); }
DirLock current_dir(DirLock l) { return DirLock(CurrentDir(BPTR(l))); }
DirLock parent_dir(DirLock l) { return DirLock(ParentDir(BPTR(l))); }
bool info(DirLock l, void *info_data) { return Info(BPTR(l), static_cast<InfoData *>(info_data)); }
bool assign_lock(const char *name, DirLock l) { return AssignLock(name, BPTR(l)); }
bool device_exists(const char *name) { return DeviceProc(name) != nullptr; }

bool delete_file(const char *name) { return DeleteFile(name); }
bool rename(const char *from, const char *to) { return Rename(from, to); }
DirLock create_dir(const char *name) { return DirLock(CreateDir(name)); }
bool execute(const char *command, File input, File output) { return Execute(command, BPTR(input), BPTR(output)); }
void delay(int32_t ticks) { Delay(ticks); }

} // namespace amos::hal
