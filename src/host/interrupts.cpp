// The interrupts on another host: the vertical blank's servers run 50
// times a second, between the calls of AMOS's routines (poll(): as an
// interrupt between two instructions of the 68000), and while AMOS waits
// for a frame. Also the entries of AMOS.library's interrupts (+W.s VblIn,
// IoHandler) and the system structures of its data zone (w/w.h).

#include "hal/gfx.h"
#include "core/ioreq.h"
#include "hal/system.h"
#include "runtime/glue.h"
#include "runtime/runtime.h"
#include "w/w.h"

#include <exec/interrupts.h>
#include <exec/io.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace amos;

extern "C" void w_vbl(uint8_t *data);
extern "C" uint16_t host_chip_read(uint32_t offset);

extern "C" uint32_t amos_w_base;      // +W.s W_Base (runtime/hand.cpp)

namespace {

Interrupt *vbl_servers[8];
uint64_t next_frame;            // in microseconds
bool in_interrupt;

constexpr uint64_t Frame = 20000;

uint64_t now()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return uint64_t(t.tv_sec) * 1000000 + uint64_t(t.tv_nsec) / 1000;
}

void run_vbl()
{
    for (Interrupt *is : vbl_servers) {
        if (!is || !is->is_Code)
            continue;
        AllRegs r = {};
        r.a[1] = address(is->is_Data);
        r.a[0] = hal::CustomBase;
        host::glue_call(reinterpret_cast<const void *>(is->is_Code), r);
    }
}

// The input handler's and the VBL's structures, the IO requests (with
// their reply port) of the input and console devices (w/w.h)
Interrupt input_is, vbl_is;
alignas(8) uint8_t input_io[256], console_io[256];


} // namespace

namespace amos::w {
struct Interrupt *input_interrupt() { return &input_is; }
IOReq *input_request() { return reinterpret_cast<IOReq *>(input_io); }
uint8_t *console_request() { return console_io; }
struct Interrupt *vbl_interrupt() { return &vbl_is; }
} // namespace amos::w

namespace amos::host {

void display_save(const char *path);
void window_start();
void window_frame();
void audio_interrupts();
void audio_frame();
void run_tasks();
void task_yield();
bool in_task();

// The interrupts due (between two routines, the clock read every 64th
// time: a routine is far shorter than a frame; in a wait, always)
void poll(bool waiting)
{
    if (in_interrupt)
        return;
    if (host_chip_read(0x01E) & 0x0780)     // INTREQR: the audio's
        audio_interrupts();
    if (!next_frame)
        return;
    static unsigned calls;
    if (!waiting && ++calls & 63)
        return;
    uint64_t t = now();
    if (t < next_frame)
        return;
    in_interrupt = true;
    // (frames missed are not made up for beyond one)
    if (t - next_frame > 4 * Frame)
        next_frame = t;
    next_frame += Frame;
    run_vbl();
    audio_frame();
    window_frame();
    // (debugging: AMOS_SHOT_FRAME=n, the frame n into AMOS_SHOT)
    static int frames;
    static const char *shot = getenv("AMOS_SHOT"), *at = getenv("AMOS_SHOT_FRAME");
    if (shot && at && ++frames == atoi(at))
        display_save(shot);
    in_interrupt = false;
}

// +W.s VblIn: the server of AMOS.library (a1 its data)
void host_VblIn(AllRegs &r)
{
    w_vbl(ptr<uint8_t>(r.a[1]));
    r.a[0] = hal::CustomBase;
    r.d[0] = 0;
}

// +W.s IoHandler: the input handler (a0 the events), a5 the data zone
void host_IoHandler(AllRegs &r)
{
    r.a[5] = amos_w_base;
    host::glue_call(w_input, r);
}

} // namespace amos::host

namespace amos::hal {

void add_vbl_server(Interrupt *server)
{
    for (Interrupt *&s : vbl_servers)
        if (!s) {
            s = server;
            break;
        }
    if (!next_frame) {
        next_frame = now() + Frame;
        host::window_start();
    }
}

void remove_vbl_server(Interrupt *server)
{
    for (Interrupt *&s : vbl_servers)
        if (s == server)
            s = nullptr;
}

// A wait loop: the interrupts due, a little sleep
void idle()
{
    if (host::in_task()) {
        host::task_yield();
        return;
    }
    host::poll(true);
    usleep(500);
    host::run_tasks();
}

// WaitTOF: the next frame (its interrupts)
void wait_frame()
{
    if (host::in_task()) {
        host::task_yield();
        return;
    }
    if (!next_frame) {
        usleep(Frame);
        host::run_tasks();
        return;
    }
    uint64_t t = now();
    if (t < next_frame)
        usleep(useconds_t(next_frame - t));
    host::poll(true);
    host::run_tasks();
}

} // namespace amos::hal
