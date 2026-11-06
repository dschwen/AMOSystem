// Shared by the C++ parts of AMOS.library (src/w): the custom chips and
// the assembler routines of +W.s they still use.
#pragma once

#include "core/amos.h"
#include "core/ioreq.h"

#include <hardware/custom.h>
#include "hal/chips.h"

struct Interrupt;

// +W.s, exported under _w_ names


namespace amos::w {

// EcFlags BitDble
constexpr uint16_t DoubleBuffer = 1 << (8 + 5);
constexpr uint32_t EntNul = 0x80000000;

inline volatile struct Custom &chips() { return hal::custom(); }

inline Screen *current_screen() { return ptr<Screen>(wdata().EcCourant); }

// The system's structures in the data zone: the input handler's interrupt
// and its IO request, the VBL's interrupt. (Their room is the Amiga's
// size: another host's, larger, are elsewhere.)
#if defined(__mc68000__)

inline uint8_t *console_request() { return w_con_io; }         // +W.s ConIo
inline struct Interrupt *input_interrupt() { return reinterpret_cast<struct Interrupt *>(wdata().Interrupt); }
inline IOReq *input_request() { return reinterpret_cast<IOReq *>(wdata().IoDevice); }
inline struct Interrupt *vbl_interrupt() { return reinterpret_cast<struct Interrupt *>(wdata().VBL_Is); }
#else
uint8_t *console_request();
struct Interrupt *input_interrupt();
IOReq *input_request();
struct Interrupt *vbl_interrupt();
#endif

// FastMm (cleared), FastMm2, ChipMm (cleared): 0 if no memory
uint32_t fast_mm(uint32_t size);
uint32_t fast_mm2(uint32_t size);
uint32_t chip_mm(uint32_t size);
uint32_t chip_mm2(uint32_t size);     // not cleared
void free_mm(uint32_t address, uint32_t size);

// WMemReserve / WMemFree (w/memory.cpp)
uint32_t reserve(uint32_t size, uint32_t flags);
void release(uint32_t address, uint32_t size);
void call_routines(be_u32 *list);

// Calls a +W.s routine with no parameters (TAbk1-4...). Returns its d0.
uint32_t call(host::RoutineArg routine);

void copy(void *to, const void *from, uint32_t n);

// w/drawing.cpp: the clip rectangle of the current screen, its drawing
// state saved and restored (T_EcSave), no pattern
void set_clip_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void push_state();
void pull_state();
void clear_pattern();

// w/support.cpp: until the next VBL
void wait_vbl();

// The start and end of the parts (w/startup.cpp calls them): sprites,
// bobs, copper lists, screens, the VBL server
bool sprite_buffers(uint16_t lines);
void sprite_buffers_free();
bool sprites_init(uint16_t lines);
void sprites_end();
bool bobs_init(uint16_t n);
void bobs_end();
bool copper_init(uint32_t length);
void copper_end();
bool screens_init();
void screens_end();
void add_vbl();
void rem_vbl();
void vbl_init();
void open_input(uint8_t *io);
void close_input(uint8_t *io);
void open_console();
void close_console();
void clear_keys();

// w/system.cpp: TAMOSWb, 0 the Workbench in front, >0 AMOS, <0 nothing;
// -1 if AMOS is in front, 0 if not
int32_t amos_wb(int32_t to);

// w/keyboard.cpp, w/input.cpp
const uint8_t *put_key(const uint8_t *text);
void send_fake_event();

} // namespace amos::w
