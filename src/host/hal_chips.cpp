// hal/chips.h on another host: the custom chips' registers
// (host/include/hardware/custom.h) and CIA A's port A. The registers the
// display, the sprites and the audio read keep what was written; the
// set/clear registers (DMACON, INTENA, INTREQ, ADKCON) change their read
// side; a write to BLTSIZE does the blit, one to DMACON starts or stops
// the audio's voices (host/audio.cpp). The beam's position comes from
// the time (a PAL frame of 312 lines each 20ms).

#include "hal/chips.h"
#include "runtime/runtime.h"

#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include "host/windows_platform.h"
#endif

namespace amos::host {
void audio_dma(uint16_t before, uint16_t after);
}

namespace {

volatile struct Custom registers;
volatile uint8_t cia_a = 0xFF;      // no button pressed
uint16_t potinp = 0x5500;           // POTINP: the ports' pins 5 and 9 high (no button)

uint16_t word(uint32_t offset) { return reinterpret_cast<volatile ChipReg *>(reinterpret_cast<volatile uint8_t *>(&registers) + offset)->raw; }
void set_word(uint32_t offset, uint16_t v)
{
    reinterpret_cast<volatile ChipReg *>(reinterpret_cast<volatile uint8_t *>(&registers) + offset)->raw = v;
}

// SETCLR: bit 15 set the other bits, else clear them
void set_clear(uint32_t read, uint16_t v)
{
    uint16_t old = word(read);
    set_word(read, v & 0x8000 ? uint16_t(old | (v & 0x7FFF)) : uint16_t(old & ~v));
}

uint32_t pointer(uint32_t offset) { return uint32_t(word(offset)) << 16 | word(offset + 2); }

// ---- the blitter -----------------------------------------------------------

uint16_t read_word(uint32_t at) { return amos::peek_w(amos::ptr<void>(at & ~1u)); }
void write_word(uint32_t at, uint16_t v) { amos::poke_w(amos::ptr<void>(at & ~1u), v); }

uint16_t minterm(uint8_t m, uint16_t a, uint16_t b, uint16_t c)
{
    uint16_t d = 0;
    if (m & 0x01) d |= ~a & ~b & ~c;
    if (m & 0x02) d |= ~a & ~b & c;
    if (m & 0x04) d |= ~a & b & ~c;
    if (m & 0x08) d |= ~a & b & c;
    if (m & 0x10) d |= a & ~b & ~c;
    if (m & 0x20) d |= a & ~b & c;
    if (m & 0x40) d |= a & b & ~c;
    if (m & 0x80) d |= a & b & c;
    return d;
}

// The blitter's line mode (BLTCON1 bit 0): a pixel at a time, from the start bit of A's shift in the word of
// the C/D pointers. A's pointer is the error: while its sign is clear the line steps in the minor axis too
// (A's modulo is added), else only in the major one (B's modulo). SUD: x is the major axis; AUL: the major
// step is left or up; SUL: the minor step is left or up. SING: only the first pixel of a row is drawn.
void line(uint16_t size)
{
    uint16_t con0 = word(0x40), con1 = word(0x42);
    uint16_t first_mask = word(0x44);
    uint32_t cpt = pointer(0x48), dpt = pointer(0x54);
    int16_t error = int16_t(word(0x52) & 0xFFFE);
    int16_t amod = int16_t(word(0x64)), bmod = int16_t(word(0x62)), cmod = int16_t(word(0x60)),
            dmod = int16_t(word(0x66));
    uint16_t bdat = word(0x72), adat = word(0x74);
    int length = size >> 6;
    if (!length)
        length = 1024;
    int ashift = con0 >> 12, bshift = con1 >> 12;
    uint8_t m = uint8_t(con0);
    bool sign = con1 & 0x40, single = con1 & 0x02, sud = con1 & 0x10, sul = con1 & 0x08, aul = con1 & 0x04;
    bool dot = false, zero = true;
    auto right = [&] {
        if (++ashift == 16) {
            ashift = 0;
            cpt += 2;
            dpt += 2;
        }
    };
    auto left = [&] {
        if (--ashift < 0) {
            ashift = 15;
            cpt -= 2;
            dpt -= 2;
        }
    };
    auto down = [&] {
        cpt += cmod;
        dpt += dmod;
        dot = false;
    };
    auto up = [&] {
        cpt -= cmod;
        dpt -= dmod;
        dot = false;
    };
    for (int i = 0; i < length; i++) {
        uint16_t a = uint16_t((adat & first_mask) >> ashift);
        uint16_t b = (bdat >> bshift & 1) ? 0xFFFF : 0;
        bshift = (bshift - 1) & 15;
        uint16_t d = minterm(m, a, b, read_word(cpt));
        if (!single || !dot) {
            write_word(dpt, d);
            dot = true;
            if (d)
                zero = false;
        }
        if (sud) {
            aul ? left() : right();
            if (!sign)
                sul ? up() : down();
        } else {
            aul ? up() : down();
            if (!sign)
                sul ? left() : right();
        }
        error = int16_t(error + (sign ? bmod : amod));
        sign = error < 0;
    }
    set_word(0x48, uint16_t(cpt >> 16));
    set_word(0x4A, uint16_t(cpt));
    set_word(0x54, uint16_t(dpt >> 16));
    set_word(0x56, uint16_t(dpt));
    set_word(0x52, uint16_t(error));
    set_word(0x40, uint16_t((con0 & 0x0FFF) | ashift << 12));
    set_word(0x42, uint16_t((con1 & ~0x40 & 0x0FFF) | bshift << 12 | (sign ? 0x40 : 0)));
    uint16_t dma = word(0x02) & ~0x6000;
    if (zero)
        dma |= 0x2000;
    set_word(0x02, dma);
}

void blit(uint16_t size)
{
    if (word(0x42) & 1) {
        line(size);
        return;
    }
    uint16_t con0 = word(0x40), con1 = word(0x42);
    uint16_t first_mask = word(0x44), last_mask = word(0x46);
    uint32_t cpt = pointer(0x48), bpt = pointer(0x4C), apt = pointer(0x50), dpt = pointer(0x54);
    int16_t cmod = int16_t(word(0x60)), bmod = int16_t(word(0x62)), amod = int16_t(word(0x64)),
            dmod = int16_t(word(0x66));
    uint16_t cdat = word(0x70), bdat = word(0x72), adat = word(0x74);
    int width = size & 0x3F, height = size >> 6;
    if (!width)
        width = 64;
    if (!height)
        height = 1024;
    bool use_a = con0 & 0x0800, use_b = con0 & 0x0400, use_c = con0 & 0x0200, use_d = con0 & 0x0100;
    int ashift = con0 >> 12, bshift = con1 >> 12;
    uint8_t m = uint8_t(con0);
    bool desc = con1 & 0x0002;
    bool fill = con1 & 0x0018, exclusive = con1 & 0x0010;
    int step = desc ? -2 : 2;
    bool zero = true;
    uint16_t prev_a = 0, prev_b = 0;
    for (int y = 0; y < height; y++) {
        bool carry = con1 & 0x0004;
        for (int x = 0; x < width; x++) {
            uint16_t a = adat;
            if (use_a) {
                a = read_word(apt);
                apt += step;
            }
            if (x == 0)
                a &= first_mask;
            if (x == width - 1)
                a &= last_mask;
            uint16_t b = bdat;
            if (use_b) {
                b = read_word(bpt);
                bpt += step;
            }
            uint16_t c = cdat;
            if (use_c) {
                c = read_word(cpt);
                cpt += step;
            }
            uint16_t as, bs;
            if (desc) {
                as = uint16_t((uint32_t(a) << 16 | prev_a) >> (16 - ashift));
                bs = uint16_t((uint32_t(b) << 16 | prev_b) >> (16 - bshift));
            } else {
                as = uint16_t((uint32_t(prev_a) << 16 | a) >> ashift);
                bs = uint16_t((uint32_t(prev_b) << 16 | b) >> bshift);
            }
            prev_a = a;
            prev_b = b;
            uint16_t d = minterm(m, as, bs, c);
            if (fill) {
                // area fill, from the right: inclusive the edges kept,
                // exclusive the left ones dropped
                uint16_t out = 0;
                for (int bit = 0; bit < 16; bit++) {
                    bool in = d & (1 << bit);
                    bool o;
                    if (exclusive) {
                        carry ^= in;
                        o = carry;
                    } else {
                        o = in || carry;
                        carry ^= in;
                    }
                    if (o)
                        out |= uint16_t(1 << bit);
                }
                d = out;
            }
            if (d)
                zero = false;
            if (use_d) {
                write_word(dpt, d);
                dpt += step;
            }
        }
        if (desc) {
            apt -= amod;
            bpt -= bmod;
            cpt -= cmod;
            dpt -= dmod;
        } else {
            apt += amod;
            bpt += bmod;
            cpt += cmod;
            dpt += dmod;
        }
    }
    // the pointers past the blit, BZERO, not busy
    set_word(0x48, uint16_t(cpt >> 16));
    set_word(0x4A, uint16_t(cpt));
    set_word(0x4C, uint16_t(bpt >> 16));
    set_word(0x4E, uint16_t(bpt));
    set_word(0x50, uint16_t(apt >> 16));
    set_word(0x52, uint16_t(apt));
    set_word(0x54, uint16_t(dpt >> 16));
    set_word(0x56, uint16_t(dpt));
    uint16_t dma = word(0x02) & ~0x6000;
    if (zero)
        dma |= 0x2000;
    set_word(0x02, dma);
}

// The beam: the line and the position in it
uint32_t beam()
{
#if defined(_WIN32)
    uint64_t us = amos::host::windows::monotonic_microseconds();
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t us = uint64_t(t.tv_sec) * 1000000 + uint64_t(t.tv_nsec) / 1000;
#endif
    uint32_t in_frame = uint32_t(us % 20000);
    uint32_t line = in_frame * 312 / 20000;
    uint32_t h = (in_frame * 312 % 20000) * 227 / 20000;
    return line << 8 | h;
}

} // namespace

extern "C" {

uint32_t host_chip_offset(const volatile void *reg)
{
    return uint32_t(static_cast<const volatile uint8_t *>(reg) - reinterpret_cast<const volatile uint8_t *>(&registers));
}

uint16_t host_chip_read(uint32_t offset)
{
    switch (offset) {
    case 0x004: {           // VPOSR: the long frame, the high bit of the line
        uint32_t b = beam();
        return uint16_t(0x8000 | (b >> 16 & 1));
    }
    case 0x006:             // VHPOSR
        return uint16_t(beam());
    case 0x07C:             // DENISEID: ECS
        return 0x00FC;
    case 0x016:             // POTINP: the right button (port 0's pin 9)
        return potinp;
    }
    return word(offset);
}

void host_chip_write(uint32_t offset, uint16_t v)
{
    switch (offset) {
    case 0x096: {           // DMACON (the audio's voices started, stopped)
        uint16_t before = word(0x002);
        set_clear(0x002, v);
        amos::host::audio_dma(before, word(0x002));
        return;
    }
    case 0x09A:             // INTENA
        set_clear(0x01C, v);
        return;
    case 0x09C:             // INTREQ
        set_clear(0x01E, v);
        return;
    case 0x09E:             // ADKCON
        set_clear(0x010, v);
        return;
    }
    set_word(offset, v);
    if (offset == 0x058)    // BLTSIZE
        blit(v);
}

} // extern "C"

namespace amos::host {

namespace {
bool mouse_left, port_fire[2];

// CIA A's port A: /FIR0 (bit 6) and /FIR1 (bit 7), low while pressed. Port 0's is the mouse's left
// button too.
void fire_buttons()
{
    uint8_t v = uint8_t(cia_a | 0xC0);
    if (mouse_left || port_fire[0])
        v &= uint8_t(~0x40);
    if (port_fire[1])
        v &= uint8_t(~0x80);
    cia_a = v;
}
} // namespace

// The mouse's buttons, as the ports show them: the left one is port 0's
// fire button (CIA A's /FIR0), the right one its pin 9 (POTINP's DATLY)
void mouse_buttons(bool left, bool right)
{
    mouse_left = left;
    fire_buttons();
    potinp = right ? uint16_t(potinp & ~0x0400) : uint16_t(potinp | 0x0400);
}

// A joystick in port 0 or 1 (the Amiga's ports 1 and 2): directions (bit 0 up, 1 down, 2 left, 3 right)
// into JOY0DAT / JOY1DAT as the hardware shows them (right: bit 1, left: bit 9, down: bit 1 xor bit 0,
// up: bit 9 xor bit 8), the fire button into CIA A
void joystick_port(int port, unsigned directions, bool fire)
{
    if (port < 0 || port > 1)
        return;
    unsigned up = directions & 1, down = directions >> 1 & 1, left = directions >> 2 & 1, right = directions >> 3 & 1;
    set_word(port ? 0x00C : 0x00A, uint16_t(left << 9 | (up ^ left) << 8 | right << 1 | (down ^ right)));
    port_fire[port] = fire;
    fire_buttons();
}

} // namespace amos::host

namespace amos::hal {

// (the registers as the system leaves them: DMA on for the copper, the
// bitplanes, the blitter, the sprites and the disk)
volatile struct Custom &custom()
{
    static bool started;
    if (!started) {
        started = true;
        set_word(0x002, 0x03F0);        // DMACONR
    }
    return registers;
}
volatile uint8_t &cia_a_pra() { return cia_a; }
// nothing on the parallel port: the lines are pulled up
volatile uint8_t &parallel_data() { static volatile uint8_t v = 0xFF; return v; }
volatile uint8_t &parallel_control() { static volatile uint8_t v = 0xFF; return v; }

} // namespace amos::hal
