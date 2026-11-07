// The host is built with -fpack-struct=2 for the guest's records. The system's own structures keep
// their layout (the kernel and the C library fill them in): on macOS a packed struct stat is
// smaller than the one stat() writes, which overwrites the caller's stack. The headers that define
// them are read here first, with the platform's packing (forced in with -include).
#pragma once
#if defined(__APPLE__)
#pragma pack(push, 8)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/statvfs.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <ucontext.h>
#include <pthread.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach-o/dyld.h>
#pragma pack(pop)
#endif
