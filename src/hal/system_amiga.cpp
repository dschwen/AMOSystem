// hal/system.h on the Amiga: exec.library

#include "hal/system.h"

#include "core/amiga.h"

#include <hardware/intbits.h>
#include <exec/execbase.h>

namespace amos::hal {

uint32_t available_memory(uint32_t flags) { return AvailMem(flags); }
void forbid() { Forbid(); }
void permit() { Permit(); }

void *allocate(uint32_t size, uint32_t flags) { return AllocMem(size, flags); }
void release(void *memory, uint32_t size) { FreeMem(memory, size); }

Task *find_task(const char *name) { return FindTask(name); }
void signal(Task *task, uint32_t signals) { Signal(task, signals); }

Library *open_library(const char *name, uint32_t version) { return OpenLibrary(name, version); }
void close_library(Library *library) { CloseLibrary(library); }
uint16_t os_version() { return EXEC_BASE_NAME->lib_Version; }
uint8_t vblank_frequency() { return reinterpret_cast<struct ExecBase *>(EXEC_BASE_NAME)->VBlankFrequency; }
void clear_cache() { CacheClearU(); }

// (the NDK's inline of SetFunction does not compile as C++)
uint32_t set_function(uint32_t library, int16_t offset, uint32_t function)
{
    register uint32_t d0 __asm("d0") = function;
    register uint32_t a1 __asm("a1") = library;
    register int32_t a0 __asm("a0") = offset;
    register void *a6 __asm("a6") = EXEC_BASE_NAME;
    __asm volatile("jsr %%a6@(-420:W)" : "+d"(d0), "+a"(a1), "+a"(a0) : "a"(a6) : "d1", "cc", "memory");
    return d0;
}

void *allocate_at(uint32_t size, void *address) { return AllocAbs(size, address); }

void add_head(void *list, void *node) { AddHead(static_cast<struct List *>(list), static_cast<struct Node *>(node)); }

void add_port(MsgPort *port) { AddPort(port); }
void remove_port(MsgPort *port) { RemPort(port); }
MsgPort *find_port(const char *name) { return FindPort(name); }
Message *get_message(MsgPort *port) { return GetMsg(port); }
Message *wait_port(MsgPort *port) { return WaitPort(port); }
void reply_message(Message *message) { ReplyMsg(message); }
int32_t allocate_signal(int32_t n) { return AllocSignal(n); }
void free_signal(int32_t n) { FreeSignal(n); }
void add_task(Task *task, void *entry, void *final_pc) { AddTask(task, entry, final_pc); }

int32_t open_device(const char *name, uint32_t unit, IORequest *io, uint32_t flags) { return OpenDevice(name, unit, io, flags); }
void close_device(IORequest *io) { CloseDevice(io); }
int32_t do_io(IORequest *io) { return DoIO(io); }
void send_io(IORequest *io) { SendIO(io); }
IORequest *check_io(IORequest *io) { return CheckIO(io); }
void abort_io(IORequest *io) { AbortIO(io); }
int32_t wait_io(IORequest *io) { return WaitIO(io); }

void add_vbl_server(Interrupt *server) { AddIntServer(INTB_VERTB, server); }
void remove_vbl_server(Interrupt *server) { RemIntServer(INTB_VERTB, server); }
Interrupt *set_interrupt(int32_t n, Interrupt *handler) { return SetIntVector(n, handler); }

} // namespace amos::hal
