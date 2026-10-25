// The host system's services AMOS uses besides its files (Phase 7: the
// portability layer): the memory there is, and the sections no other task
// may interrupt. hal/system_amiga.cpp is exec.library.
#pragma once

#include <stdint.h>

struct Task;
struct Library;
struct Interrupt;
struct MsgPort;
struct Message;
struct IORequest;

namespace amos::hal {

// The memory free of the kinds of flags (exec's MEMF_: public, chip,
// fast; total for all of it)
constexpr uint32_t MemoryPublic = 0x1, MemoryChip = 0x2, MemoryFast = 0x4, MemoryTotal = 0x80000;
uint32_t available_memory(uint32_t flags);

// No other task runs between forbid() and permit()
void forbid();
void permit();

// Memory of the system (exec's AllocMem: flags as above, 0x10000 cleared)
void *allocate(uint32_t size, uint32_t flags);
void release(void *memory, uint32_t size);

// Tasks: by name (null: the running one), signals
Task *find_task(const char *name);
void signal(Task *task, uint32_t signals);

// Libraries of the system (0: none), its version, its caches
Library *open_library(const char *name, uint32_t version);
void close_library(Library *library);
uint16_t os_version();                          // exec's
uint8_t vblank_frequency();                     // 50 (PAL) or 60
void clear_cache();
// SetFunction: library's function at offset becomes function; the old one
uint32_t set_function(uint32_t library, int16_t offset, uint32_t function);

// Memory at a given address (AllocAbs)
void *allocate_at(uint32_t size, void *address);

// Exec's lists
void add_head(void *list, void *node);

// Message ports, signals
void add_port(MsgPort *port);
void remove_port(MsgPort *port);
MsgPort *find_port(const char *name);
Message *get_message(MsgPort *port);
Message *wait_port(MsgPort *port);
void reply_message(Message *message);
int32_t allocate_signal(int32_t n);             // -1 for any
void free_signal(int32_t n);
// A task of its own: started with its stack and entry set up (AddTask)
void add_task(Task *task, void *entry, void *final_pc);

// Devices (their units and IO requests, as exec's)
int32_t open_device(const char *name, uint32_t unit, IORequest *io, uint32_t flags);
void close_device(IORequest *io);
int32_t do_io(IORequest *io);
void send_io(IORequest *io);
IORequest *check_io(IORequest *io);          // the request if done, or null
void abort_io(IORequest *io);
int32_t wait_io(IORequest *io);

// Interrupts: a server of the vertical blank; the handler of interrupt n
// (the old one)
void add_vbl_server(Interrupt *server);
void remove_vbl_server(Interrupt *server);
Interrupt *set_interrupt(int32_t n, Interrupt *handler);

} // namespace amos::hal
