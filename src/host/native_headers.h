// The host is built with -fpack-struct=2 for the guest's records. The system's own structures keep
// their layout (the kernel and the C library fill them in): on macOS a packed struct stat is
// smaller than the one stat() writes, which overwrites the caller's stack. The headers that define
// them are read here first, with the platform's packing (forced in with -include). Only those the host
// uses with a system call that writes them; the NDK's own timeval must stay out (hal_system.cpp).
#pragma once
#if defined(__APPLE__)
#pragma pack(push, 8)
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#pragma pack(pop)
#endif
