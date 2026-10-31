// The interrupts on another host: the vertical blank's servers run 50
// times a second, between the calls of AMOS's routines (poll(): as an
// interrupt between two instructions of the 68000), and while AMOS waits
// for a frame. Also the entries of AMOS.library's interrupts (+W.s VblIn,
// IoHandler) and the system structures of its data zone (w/w.h).

#include "hal/gfx.h"
#include "hal/system.h"
#include "host/glue.h"
#include "host/host.h"
#include "w/w.h"

#include <exec/interrupts.h>
#include <exec/io.h>

#include <time.h>
#include <unistd.h>

using namespace amos;

extern "C" void w_vbl(uint8_t *data);
extern "C" uint8_t w_input[];

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

// The input handler's and the VBL's structures (w/w.h)
Interrupt input_is, vbl_is;
IOStdReq input_io;

uint32_t w_base;        // +W.s W_Base: the data zone, for the input handler

} // namespace

namespace amos::w {
struct Interrupt *input_interrupt() { return &input_is; }
struct IOStdReq *input_request() { return &input_io; }
struct Interrupt *vbl_interrupt() { return &vbl_is; }
} // namespace amos::w

namespace amos::host {

// The interrupts due
void poll()
{
    if (in_interrupt || !next_frame)
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
    r.a[5] = w_base;
    host::glue_call(w_input, r);
}

void set_w_base(uint32_t a5) { w_base = a5; }

} // namespace amos::host

namespace amos::hal {

void add_vbl_server(Interrupt *server)
{
    for (Interrupt *&s : vbl_servers)
        if (!s) {
            s = server;
            break;
        }
    if (!next_frame)
        next_frame = now() + Frame;
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
    host::poll();
    usleep(500);
}

// WaitTOF: the next frame (its interrupts)
void wait_frame()
{
    if (!next_frame) {
        usleep(Frame);
        return;
    }
    uint64_t t = now();
    if (t < next_frame)
        usleep(useconds_t(next_frame - t));
    host::poll();
}

} // namespace amos::hal
