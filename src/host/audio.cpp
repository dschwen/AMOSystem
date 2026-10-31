// Paula on another host: the four voices of the Amiga's audio played from
// the registers AMOS writes (host/hal_chips.cpp): with its DMA on a voice
// reads AUDxLEN words from AUDxLC, a byte each AUDxPER cycles of the 3.5MHz
// clock, at AUDxVOL; at the start of each block it reloads the registers
// and requests its interrupt. Voices 0 and 3 are on the left, 1 and 2 on
// the right. A frame's sound is made at each frame on AMOS's side (the
// interrupts call AMOS's handlers, as exec would) and queued to the host's
// sound (host/sdl.cpp), and recorded into AMOS_WAV's file if it is set.

#include "hal/chips.h"
#include "hal/system.h"
#include "host/glue.h"
#include "host/host.h"

#include <exec/interrupts.h>

#include <stdio.h>
#include <string.h>

extern "C" char *getenv(const char *name) noexcept;

extern "C" {
uint16_t host_chip_read(uint32_t offset);
void host_chip_write(uint32_t offset, uint16_t v);
}

using namespace amos;

namespace amos::host {

constexpr int Rate = 44100;
constexpr int FrameSamples = Rate / 50;
void audio_queue(const int16_t *samples, int count);

namespace {

constexpr uint32_t Clock = 3546895;         // PAL
constexpr uint32_t Step = Clock * 256 / Rate;   // the cycles of a sample, in 1/256

struct Voice {
    bool on;
    uint32_t at;            // the next word
    uint32_t left;          // words left in the block
    uint16_t word;          // the word playing
    int byte;               // its byte playing (0, 1)
    uint32_t cycles;        // since the byte began, in 1/256
};
Voice voices[4];
Interrupt *vectors[16];     // exec's interrupt vectors (the audio's: 7-10)

uint32_t reg(int v, uint32_t offset) { return 0xA0 + 16 * v + offset; }

// The block: the registers loaded, the interrupt requested
void load(int v)
{
    Voice &voice = voices[v];
    voice.at = uint32_t(host_chip_read(reg(v, 0))) << 16 | host_chip_read(reg(v, 2));
    voice.left = host_chip_read(reg(v, 4));
    if (!voice.left)
        voice.left = 0x10000;
    host_chip_write(0x09C, uint16_t(0x8000 | 0x80 << v));
}

uint16_t fetch(uint32_t at) { return at >= 0x1000 ? peek_w(ptr<void>(at & ~1u)) : 0; }

// The next byte of a voice
void advance(int v)
{
    Voice &voice = voices[v];
    if (voice.byte == 0) {
        voice.byte = 1;
        return;
    }
    voice.byte = 0;
    voice.at += 2;
    if (--voice.left == 0)
        load(v);
    voice.word = fetch(voice.at);
}

int sample(int v)
{
    Voice &voice = voices[v];
    if (!voice.on)
        return 0;
    uint32_t period = host_chip_read(reg(v, 6));
    if (period < 124)
        period = 124;
    voice.cycles += Step;
    while (voice.cycles >= period * 256) {
        voice.cycles -= period * 256;
        advance(v);
    }
    int volume = host_chip_read(reg(v, 8)) & 0x7F;
    if (volume > 64)
        volume = 64;
    int8_t byte = int8_t(voice.byte ? voice.word : voice.word >> 8);
    return byte * volume;
}

bool in_interrupts;

// (AMOS_WAV: the sound recorded, its header kept up to date)
void record(const int16_t *out, int count)
{
    static const char *path = getenv("AMOS_WAV");
    static FILE *f = path ? fopen(path, "wb") : nullptr;
    static uint32_t bytes;
    if (!f)
        return;
    fseek(f, 44 + long(bytes), SEEK_SET);
    fwrite(out, 4, size_t(count), f);
    bytes += uint32_t(count) * 4;
    auto le32 = [](uint8_t *p, uint32_t v) { p[0] = uint8_t(v), p[1] = uint8_t(v >> 8), p[2] = uint8_t(v >> 16), p[3] = uint8_t(v >> 24); };
    uint8_t h[44] = {'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 2, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 16, 0, 'd', 'a', 't', 'a', 0, 0, 0, 0};
    le32(h + 4, 36 + bytes);
    le32(h + 24, Rate);
    le32(h + 28, Rate * 4);
    le32(h + 40, bytes);
    fseek(f, 0, SEEK_SET);
    fwrite(h, 1, sizeof h, f);
    fflush(f);
}

} // namespace

// DMACON written: the voices started load their first block
void audio_dma(uint16_t before, uint16_t after)
{
    for (int v = 0; v < 4; v++) {
        bool was = (before & 0x200) && (before & (1 << v)), is = (after & 0x200) && (after & (1 << v));
        if (is && !was) {
            voices[v] = Voice{};
            voices[v].on = true;
            load(v);
            voices[v].word = fetch(voices[v].at);
        } else if (was && !is) {
            voices[v].on = false;
        }
    }
}

// The audio interrupts requested and enabled: their handlers, each once
void audio_interrupts()
{
    if (in_interrupts)
        return;
    in_interrupts = true;
    uint16_t enabled = host_chip_read(0x01C);
    if (enabled & 0x4000) {
        uint16_t due = host_chip_read(0x01E) & enabled & 0x0780;
        for (int n = 7; n <= 10; n++) {
            Interrupt *is = vectors[n];
            if (!(due & (1 << n)) || !is || !is->is_Code)
                continue;
            AllRegs r = {};
            r.d[1] = due;
            r.a[0] = hal::CustomBase;
            r.a[1] = address(is->is_Data);
            r.a[5] = address(is->is_Code);
            r.a[6] = hal::exec_base();
            glue_call(reinterpret_cast<const void *>(is->is_Code), r);
        }
    }
    in_interrupts = false;
}

// A frame's sound
void audio_frame()
{
    static int16_t out[FrameSamples * 2];
    for (int i = 0; i < FrameSamples; i++) {
        int left = sample(0) + sample(3), right = sample(1) + sample(2);
        out[2 * i] = int16_t(left * 2);
        out[2 * i + 1] = int16_t(right * 2);
        // (a block begun: its interrupt, before the next sample)
        if (host_chip_read(0x01E) & 0x0780)
            audio_interrupts();
    }
    audio_queue(out, FrameSamples);
    record(out, FrameSamples);
}

} // namespace amos::host

namespace amos::hal {

// SetIntVector: the handler of interrupt n, the old one back
Interrupt *set_interrupt(int32_t n, Interrupt *handler)
{
    if (n < 0 || n >= 16)
        return nullptr;
    Interrupt *old = host::vectors[n];
    host::vectors[n] = handler;
    return old;
}

} // namespace amos::hal
