// Dialog/file-selector local recovery. Capture MUST remain in the live caller;
// never wrap setjmp in a function that returns before the matching restore.
#pragma once
#if (defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__))) || defined(AMOS_STANDARD_LOCAL_JUMPS)
#define AMOS_NATIVE_LOCAL_JUMPS 1
#pragma pack(push,16)
#include <setjmp.h>
namespace amos::local_jump {
struct alignas(16) Buffer { jmp_buf storage; };
}
#pragma pack(pop)
#define AMOS_LOCAL_SETJMP(buffer) setjmp((buffer).storage)
namespace amos::local_jump {
[[gnu::noinline,noreturn]] inline void restore(Buffer *buffer) { longjmp(buffer->storage,1); }
}
#else
#pragma pack(push,16)
namespace amos::local_jump {
struct alignas(void *) Buffer { void *storage[5]; };
}
#pragma pack(pop)
#define AMOS_LOCAL_SETJMP(buffer) __builtin_setjmp((buffer).storage)
namespace amos::local_jump {
[[gnu::noinline,noreturn]] inline void restore(Buffer *buffer) { __builtin_longjmp(buffer->storage,1); }
}
#endif
