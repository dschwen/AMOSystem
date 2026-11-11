// AMCAF: the joysticks on the parallel port (R00b-R010) and the second mouse's position (R0a4-R0aa). The
// registers are the chips' (hal/chips.h).

#include "ext/amcaf.h"
#include "hal/chips.h"
#include "lib/banks.h"

using namespace amos;

namespace {

uint16_t mouse_w(uint32_t at) { return peek_w(amcaf::state_at(at)); }
void mouse_set(uint32_t at, uint16_t v) { poke_w(amcaf::state_at(at), v); }

// the direction or fire of a joystick (port 0 or 1) is -1 when the line's bit is clear; another port is
// an error
int32_t line_low(uint8_t value, int32_t port, int bit0, int bit1)
{
    if (port != 0 && port != 1)
        fon_call();
    return value & (1 << (port ? bit1 : bit0)) ? 0 : -1;
}

} // namespace

extern "C" {

// =PJOY(port): the directions from the parallel port's data lines (port 0: the low nibble, port 1: the
// high one, as NEG of the byte), 16 added when the fire line is clear
int32_t amcaf_pjoy(int32_t port)
{
    if (port != 0 && port != 1)
        fon_call();
    uint8_t negated = uint8_t(-hal::parallel_data());
    uint8_t bits;
    if (port == 0) {
        bits = negated & 15;
        if (!(hal::parallel_control() & 4))
            bits += 16;
    } else {
        bits = uint8_t(negated >> 4);
        if (!(hal::parallel_control() & 1))
            bits += 16;
    }
    return bits;
}

int32_t amcaf_pjleft(int32_t port) { return line_low(hal::parallel_data(), port, 2, 6); }
int32_t amcaf_pjright(int32_t port) { return line_low(hal::parallel_data(), port, 3, 7); }
int32_t amcaf_pjup(int32_t port) { return line_low(hal::parallel_data(), port, 0, 4); }
int32_t amcaf_pjdown(int32_t port) { return line_low(hal::parallel_data(), port, 1, 5); }
int32_t amcaf_pfire(int32_t port) { return line_low(hal::parallel_control(), port, 2, 0); }

// =X SMOUSE, =Y SMOUSE: the position, with the scale taken off
int32_t amcaf_x_smouse() { return int16_t(mouse_w(amcaf::MouseX)) >> (mouse_w(amcaf::MouseShift) & 15); }
int32_t amcaf_y_smouse() { return int16_t(mouse_w(amcaf::MouseY)) >> (mouse_w(amcaf::MouseShift) & 15); }

// SMOUSE X n, SMOUSE Y n
void amcaf_smouse_x(int32_t n) { mouse_set(amcaf::MouseX, uint16_t(n << (mouse_w(amcaf::MouseShift) & 15))); }
void amcaf_smouse_y(int32_t n) { mouse_set(amcaf::MouseY, uint16_t(n << (mouse_w(amcaf::MouseShift) & 15))); }

// SMOUSE SPEED n: the scale (a shift) changed, the position kept
void amcaf_smouse_speed(int32_t n)
{
    int old = mouse_w(amcaf::MouseShift) & 15, now = n & 15;
    mouse_set(amcaf::MouseX, uint16_t(int16_t(mouse_w(amcaf::MouseX)) >> old << now));
    mouse_set(amcaf::MouseY, uint16_t(int16_t(mouse_w(amcaf::MouseY)) >> old << now));
    mouse_set(amcaf::MouseShift, uint16_t(n));
}

// LIMIT SMOUSE: the limits of the current screen's window
void amcaf_limit_smouse_0()
{
    const Screen *s = ptr<Screen>(idata().ScOnAd);
    uint16_t x = s->EcWX & 1023, y = s->EcWY & 1023;
    mouse_set(amcaf::MouseMinX, x);
    mouse_set(amcaf::MouseMinY, y);
    mouse_set(amcaf::MouseMaxX, uint16_t(s->EcTx + x - 1));
    mouse_set(amcaf::MouseMaxY, uint16_t(s->EcTy + y - 1));
}

// LIMIT SMOUSE x1,y1 TO x2,y2
void amcaf_limit_smouse_4(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    mouse_set(amcaf::MouseMinX, uint16_t(x1));
    mouse_set(amcaf::MouseMinY, uint16_t(y1));
    mouse_set(amcaf::MouseMaxX, uint16_t(x2));
    mouse_set(amcaf::MouseMaxY, uint16_t(y2));
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_joy.inc"
#endif
