// hal/system.h on the Amiga: exec.library

#include "hal/system.h"
#include "core/ioreq.h"

#include "core/amiga.h"

#include <hardware/intbits.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>

namespace amos::hal {

uint32_t available_memory(uint32_t flags) { return AvailMem(flags); }
void forbid() { Forbid(); }
void permit() { Permit(); }

void *allocate(uint32_t size, uint32_t flags) { return AllocMem(size, flags); }
void release(void *memory, uint32_t size) { FreeMem(memory, size); }

Task *find_task(const char *name) { return FindTask(name); }
void signal(Task *task, uint32_t signals) { Signal(task, signals); }
uint32_t exec_base() { return *reinterpret_cast<const uint32_t *>(4); }
uint32_t requester_window() { return uint32_t(reinterpret_cast<Process *>(FindTask(nullptr))->pr_WindowPtr); }
void set_requester_window(uint32_t window) { reinterpret_cast<Process *>(FindTask(nullptr))->pr_WindowPtr = reinterpret_cast<APTR>(window); }

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

// (amos::IOReq: the NDK's IORequest, its layout on the Amiga)
static IORequest *rq(IOReq *io) { return reinterpret_cast<IORequest *>(io); }
int32_t open_device(const char *name, uint32_t unit, IOReq *io, uint32_t flags) { return OpenDevice(name, unit, rq(io), flags); }
void close_device(IOReq *io) { CloseDevice(rq(io)); }
int32_t do_io(IOReq *io) { return DoIO(rq(io)); }
void send_io(IOReq *io) { SendIO(rq(io)); }
IOReq *check_io(IOReq *io) { return reinterpret_cast<IOReq *>(CheckIO(rq(io))); }
void abort_io(IOReq *io) { AbortIO(rq(io)); }
int32_t wait_io(IOReq *io) { return WaitIO(rq(io)); }

void add_vbl_server(Interrupt *server) { AddIntServer(INTB_VERTB, server); }
void remove_vbl_server(Interrupt *server) { RemIntServer(INTB_VERTB, server); }
Interrupt *set_interrupt(int32_t n, Interrupt *handler) { return SetIntVector(n, handler); }

} // namespace amos::hal

// RawDoFmt's output routine: d0 the character, a3 the data (the C++
// function and its data)
namespace {
struct FormatOutput {
    void (*put)(char, void *);
    void *data;
};
}
extern "C" void hal_format_put_char();
extern "C" void hal_format_put_char_c(uint32_t c, FormatOutput *o) { o->put(char(c), o->data); }
asm(R"(
        .text
        .globl  _hal_format_put_char
_hal_format_put_char:
        movem.l d0-d1/a0-a1,-(sp)
        move.l  a3,-(sp)
        and.l   #0xFF,d0
        move.l  d0,-(sp)
        jsr     _hal_format_put_char_c
        addq.l  #8,sp
        movem.l (sp)+,d0-d1/a0-a1
        rts
)");

const void *amos::hal::raw_format(const char *format, const void *args, void (*put)(char, void *), void *data)
{
    FormatOutput o = {put, data};
    // (The NDK's inline RawDoFmt does not compile as C++)
    register const char *a0 asm("a0") = format;
    register const void *a1 asm("a1") = args;
    register void (*a2)() asm("a2") = hal_format_put_char;
    register FormatOutput *a3 asm("a3") = &o;
    register struct Library *a6 asm("a6") = EXEC_BASE_NAME;
    register const void *next asm("d0");
    asm volatile("jsr %%a6@(-522:W)" : "=r"(next), "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3), "r"(a6)
                 : "d1", "cc", "memory");
    return next;
}
