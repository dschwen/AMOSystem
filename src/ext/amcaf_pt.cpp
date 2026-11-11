// AMCAF's Protracker player (Pt Play...): the commands (routines R0ee to R10c) and the player
// itself (R17d, $8a0c-$9d2c of AMOSPro_AMCAF.Lib V1.50beta4), with its two interrupts: the
// vertical blank's (the raw sound effects' leases, the second mouse) and the player's tick, from a
// timer of the CIAs (the library's default) or the vertical blank (Pt Cia Speed 0). The routines
// are the library's, one for one, with their addresses in the comments; the chips' registers are
// hal/chips.h's (Paula's audio channels), so it is the same on the Amiga and on another host.
//
// The player's block (PtBlock) is what the library keeps at $9bcc..$9d2c (the four voices, the
// samples' addresses, the song's position, the voices' masks); AMCAF Pt Data Base gives its address
// (+318, the library's "a5" of the player).

#include "core/guest_scratch.h"
#include "ext/amcaf.h"
#include "ext/amcaf_pt_tables.h"
#include "ext/third_party.h"
#include "hal/chips.h"
#include "hal/system.h"
#include "lib/banks.h"

using namespace amos;
using namespace amos::amcaf;

namespace {

// ---- the player's block --------------------------------------------------------------------

// A voice of the player: its note (the four bytes of the pattern), its sample and its effects
struct Voice {
    be_u32 cell;            // +0 the note: the period (12 bits) with the sample's high nibble, the
                            //    sample's low nibble with the effect, its parameter
    be_u32 start;           // +4 the sample's address
    be_u16 length;          // +8 its length, in words
    be_u32 loop_start;      // +10 the address of its repeat
    be_u16 loop_length;     // +14 its length
    be_u16 period;          // +16 the voice's period
    uint8_t finetune;       // +18
    uint8_t volume;         // +19
    be_u16 dma;             // +20 the voice's bit of DMACON
    uint8_t slide_down;     // +22 tone portamento: down
    uint8_t slide_speed;    // +23
    be_u16 target;          // +24 its target period
    uint8_t vibrato;        // +26 speed and depth
    uint8_t vibrato_pos;    // +27
    uint8_t tremolo;        // +28
    uint8_t tremolo_pos;    // +29
    uint8_t waves;          // +30 the waveforms: vibrato (low nibble), tremolo (high)
    uint8_t glissando;      // +31 glissando (low nibble), the funk repeat's speed (high)
    uint8_t offset;         // +32 sample offset
    uint8_t loop_row;       // +33 the pattern loop's row
    uint8_t loop_count;     // +34
    uint8_t funk;           // +35
    be_u32 funk_at;         // +36
    be_u16 length_again;    // +40
    be_u16 unused;          // +42
};
static_assert(sizeof(Voice) == 44);

// (the library's a5 is the byte of Block::speed - 14 from "player", $9d0a: the offsets of the fields
// in the comments are from it)
struct PtBlock {
    Voice voice[4];         // -318
    be_u32 sample[31];      // -142 the addresses of the 31 samples' data
    be_u32 module;          // -18 the module
    uint8_t speed;          // -14 ticks to a row
    uint8_t counter;        // -13 the tick of the row
    uint8_t position;       // -12 the song's position
    uint8_t break_row;      // -11 the row the next pattern starts at
    uint8_t jump;           // -10 a jump or a break is wanted
    uint8_t loop;           // -9 a pattern loop is wanted
    uint8_t fine;           // -8 fine slides: 15 for once
    uint8_t delay_asked;    // -7 pattern delay (EEx) + 1
    uint8_t delay;          // -6 the rows it still holds
    uint8_t pad5;           // -5
    be_u16 row;             // -4 the row's offset in the pattern (16 bytes a row)
    be_u16 starting;        // -2 the channels' DMA bits to start
    be_u16 bpm;             // +0
    uint8_t signal;         // +2 the last 8xy parameter, or 255 when the song ended
    uint8_t pad3;           // +3
    be_u16 volume;          // +4 the master volume, 0 to 64
    uint8_t enabled[4];     // +6 the voices the music plays (user mask and raw effects)
    uint8_t user[4];        // +10 Pt Voice's mask, a byte a voice
    uint8_t free[4];        // +14 0 for a voice a raw sound effect has
    be_u16 lease[4];        // +18 the ticks it has it for
    uint8_t vu[4];          // +26 the volume of the last note
    be_u32 extension;       // +30 the extension's data
};
static_assert(sizeof(PtBlock) == 352);
static_assert(offsetof(PtBlock, speed) == 304);

constexpr uint32_t BlockSize = sizeof(PtBlock);

PtBlock &block()
{
    static PtBlock *p = [] {
        PtBlock *b = guest_object<PtBlock>();
        for (int v = 0; v < 4; v++)
            b->voice[v].dma = uint16_t(1 << v);
        b->volume = 64;
        b->bpm = 125;
        for (int v = 0; v < 4; v++) {
            b->user[v] = 0xFF;
            b->enabled[v] = 0xFF;
            b->free[v] = 0xFF;
        }
        return b;
    }();
    return *p;
}

// The extension's data
uint8_t *e(uint32_t at) { return amcaf::state_at(at); }
uint16_t e16(uint32_t at) { return peek_w(e(at)); }
uint32_t e32(uint32_t at) { return peek_l(e(at)); }
void set16(uint32_t at, uint16_t v) { poke_w(e(at), v); }
void set32(uint32_t at, uint32_t v) { poke_l(e(at), v); }

// ---- the chips ------------------------------------------------------------------------------

inline volatile hal::ChipWord &reg(uint32_t offset) { return hal::custom_word(offset); }

constexpr uint32_t DmaCon = 0x96, Audio = 0xA0;

void aud_pointer(int v, uint32_t address)
{
    reg(Audio + 16 * v) = uint16_t(address >> 16);
    reg(Audio + 16 * v + 2) = uint16_t(address);
}
void aud_length(int v, uint16_t words) { reg(Audio + 16 * v + 4) = words; }
void aud_period(int v, uint16_t period) { reg(Audio + 16 * v + 6) = period; }
void aud_volume(int v, uint16_t volume) { reg(Audio + 16 * v + 8) = volume; }
void dma(uint16_t bits) { reg(DmaCon) = bits; }

// $9702, $87e4: the time for DMA to take what it was told: six changes of the beam's horizontal position
void dma_wait()
{
    for (int n = 0; n < 6; n++) {
        uint8_t before = uint8_t(uint16_t(reg(0x06)));
        while (uint8_t(uint16_t(reg(0x06))) == before) {
        }
    }
}

// ---- the lookups ----------------------------------------------------------------------------

const uint16_t *periods(unsigned finetune_stride_bytes, uint8_t finetune)
{
    return pt::Periods + finetune * finetune_stride_bytes / 2;
}

// ---- the player (R17d) ----------------------------------------------------------------------

uint8_t byte_of(uint32_t cell, int n) { return uint8_t(cell >> (24 - 8 * n)); }

// $8fa8: the voice's volume and period back to the chips
void restore(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    if (!p.enabled[v])
        return;
    aud_volume(v, uint16_t(uint32_t(x.volume) * uint16_t(p.volume) >> 6));
    aud_period(v, x.period);
}

// the volume of the voice, for the chips
void write_volume(PtBlock &p, int v)
{
    if (p.enabled[v])
        aud_volume(v, uint16_t(uint32_t(p.voice[v].volume) * uint16_t(p.volume) >> 6));
}

// $96b0: the funk repeat (EFx): the sample's repeat is inverted a byte at a time
void funk_repeat(Voice &x)
{
    uint8_t speed = x.glissando >> 4;
    if (!speed)
        return;
    x.funk = uint8_t(x.funk + pt::FunkTable[speed]);
    if (!(x.funk & 0x80))
        return;
    x.funk = 0;
    uint32_t end = x.loop_start + 2 * uint32_t(uint16_t(x.loop_length));
    uint32_t at = x.funk_at + 1;
    if (!(at < end))
        at = x.loop_start;
    x.funk_at = at;
    ContextCursor c = decode_context_cursor(at);
    host::detail::write_cursor_byte(c, uint8_t(0xFF - host::detail::read_cursor_byte(c)));
}

// $9346: the volume slide (Axy), also what 5xy and 6xy do after theirs
void volume_slide(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t param = byte_of(x.cell, 3);
    uint8_t up = param >> 4;
    if (up) {
        x.volume = uint8_t(x.volume + up);
        if (!(int8_t(x.volume - 64) < 0))
            x.volume = 64;
    } else {
        x.volume = uint8_t(x.volume - (param & 15));
        if (int8_t(x.volume) < 0)
            x.volume = 0;
    }
    write_volume(p, v);
}

// $9352, $9384 (EAx, EBx): fine volume slides, with the amount in d0
void volume_add(PtBlock &p, int v, uint8_t amount)
{
    Voice &x = p.voice[v];
    x.volume = uint8_t(x.volume + amount);
    if (!(int8_t(x.volume - 64) < 0))
        x.volume = 64;
    write_volume(p, v);
}
void volume_subtract(PtBlock &p, int v, uint8_t amount)
{
    Voice &x = p.voice[v];
    x.volume = uint8_t(x.volume - amount);
    if (int8_t(x.volume) < 0)
        x.volume = 0;
    write_volume(p, v);
}

// $903c, $908a: portamento up and down (also E1x, E2x with the slide for once)
void slide_up(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t amount = byte_of(x.cell, 3) & p.fine;
    p.fine = 0xFF;
    x.period = uint16_t(x.period - amount);
    if (int16_t((x.period & 0xFFF) - 113) < 0)
        x.period = uint16_t((x.period & 0xF000) | 113);
    if (p.enabled[v])
        aud_period(v, x.period & 0xFFF);
}
void slide_down(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t amount = byte_of(x.cell, 3) & p.fine;
    p.fine = 0xFF;
    x.period = uint16_t(x.period + amount);
    if (!(int16_t((x.period & 0xFFF) - 856) < 0))
        x.period = uint16_t((x.period & 0xF000) | 856);
    if (p.enabled[v])
        aud_period(v, x.period & 0xFFF);
}

// $8fc6: arpeggio
void arpeggio(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint16_t remainder = p.counter % 3;
    uint16_t period;
    if (remainder == 0) {
        period = x.period;
    } else {
        uint8_t param = byte_of(x.cell, 3);
        unsigned offset = remainder == 2 ? param & 15 : param >> 4;
        const uint16_t *row = periods(72, x.finetune);
        uint32_t current = x.period;
        bool found = false;
        period = 0;
        for (int k = 0; k < 37; k++) {
            period = row[k + offset];
            if (uint16_t(current) >= row[k]) {
                found = true;
                break;
            }
        }
        if (!found)
            return;
    }
    if (p.enabled[v])
        aud_period(v, period);
}

// $90ca: tone portamento's target (3xy, 5xy with a note)
void tone_target(Voice &x)
{
    uint16_t note = x.cell >> 16 & 0xFFF;
    const uint16_t *row = periods(74, x.finetune);
    unsigned d0 = 0;
    for (;;) {
        if (note >= row[d0 / 2])
            break;
        d0 += 2;
        if (d0 >= 74) {
            d0 = 70;
            break;
        }
    }
    if ((x.finetune & 8) && d0)
        d0 -= 2;
    uint16_t target = row[d0 / 2];
    x.target = target;
    x.slide_down = 0;
    int16_t compare = int16_t(target - x.period);
    if (!compare) {
        x.target = 0;
        return;
    }
    if (compare > 0)
        return;
    x.slide_down = 1;
}

// $913a: tone portamento as it goes on (also in 5xy)
void tone_portamento_go(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    if (!x.target)
        return;
    uint8_t speed = x.slide_speed;
    if (!x.slide_down) {
        x.period = uint16_t(x.period + speed);
        if (!(int16_t(x.target - x.period) > 0)) {
            x.period = x.target;
            x.target = 0;
        }
    } else {
        x.period = uint16_t(x.period - speed);
        if (!(int16_t(x.target - x.period) < 0)) {
            x.period = x.target;
            x.target = 0;
        }
    }
    uint16_t period = x.period;
    if (x.glissando & 15) {
        const uint16_t *row = periods(72, x.finetune);
        unsigned d0 = 0;
        for (;;) {
            if (period >= row[d0 / 2])
                break;
            d0 += 2;
            if (d0 >= 72) {
                d0 = 70;
                break;
            }
        }
        period = row[d0 / 2];
    }
    if (p.enabled[v])
        aud_period(v, period);
}

// $912c: tone portamento (3xy): a new speed is kept, and the parameter taken from the note
void tone_portamento(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t speed = byte_of(x.cell, 3);
    if (speed) {
        x.slide_speed = speed;
        x.cell = x.cell & 0xFFFFFF00u;
    }
    tone_portamento_go(p, v);
}

// The wave of vibrato and tremolo: the value for the position (a byte, the bit 7 its sign) and the waveform
uint8_t wave_value(uint8_t position, uint8_t form)
{
    unsigned index = (position >> 2) & 31;
    if (!form)
        return pt::SineTable[index];
    uint8_t ramp = uint8_t(index << 3);
    if (form == 1)
        return int8_t(position) < 0 ? uint8_t(0xFF - ramp) : ramp;
    return 0xFF;
}

// $91ea: the vibrato (4xy) as it goes on
void vibrato_go(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t form = x.waves & 3;
    uint16_t amount = uint16_t(uint16_t(wave_value(x.vibrato_pos, form)) * (x.vibrato & 15)) >> 7;
    uint16_t period = x.period;
    period = int8_t(x.vibrato_pos) < 0 ? uint16_t(period - amount) : uint16_t(period + amount);
    if (p.enabled[v])
        aud_period(v, period);
    x.vibrato_pos = uint8_t(x.vibrato_pos + ((x.vibrato >> 2) & 0x3C));
}

// $91c0: vibrato
void vibrato(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t param = byte_of(x.cell, 3);
    if (param) {
        uint8_t d2 = x.vibrato;
        if (param & 15)
            d2 = uint8_t((d2 & 0xF0) | (param & 15));
        if (param & 0xF0)
            d2 = uint8_t((d2 & 0x0F) | (param & 0xF0));
        x.vibrato = d2;
    }
    vibrato_go(p, v);
}

// $9268: tremolo
void tremolo(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t param = byte_of(x.cell, 3);
    if (param) {
        uint8_t d2 = x.tremolo;
        if (param & 15)
            d2 = uint8_t((d2 & 0xF0) | (param & 15));
        if (param & 0xF0)
            d2 = uint8_t((d2 & 0x0F) | (param & 0xF0));
        x.tremolo = d2;
    }
    uint8_t form = (x.waves >> 4) & 3;
    uint16_t amount = uint16_t(uint16_t(wave_value(x.tremolo_pos, form)) * (x.tremolo & 15)) >> 6;
    int16_t volume = int16_t(x.volume);
    volume = int8_t(x.tremolo_pos) < 0 ? int16_t(volume - amount) : int16_t(volume + amount);
    if (volume < 0)
        volume = 0;
    if (uint16_t(volume) > 64)
        volume = 64;
    if (p.enabled[v])
        aud_volume(v, uint16_t(uint32_t(uint16_t(volume)) * uint16_t(p.volume) >> 6));
    x.tremolo_pos = uint8_t(x.tremolo_pos + ((x.tremolo >> 2) & 0x3C));
}

// $9702: the channel's DMA stopped, its data and length given, started again, then the repeat given
void restart(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    dma(x.dma);
    aud_pointer(v, x.start);
    aud_length(v, x.length);
    dma_wait();
    dma(uint16_t(x.dma | 0x8000));
    dma_wait();
    aud_pointer(v, x.loop_start);
    aud_length(v, x.loop_length);
}

// $8b40: the tempo (Fxx from 32, Pt Cia Speed, the start): the timer's period (the clock / the tempo)
void set_tempo(PtBlock &p, uint32_t tempo);

// $9476: the E effects, a tick at a time
void effect_e(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    uint8_t param = byte_of(x.cell, 3);
    uint8_t sub = param >> 4;
    switch (sub) {
    case 0:                                         // $94fc E0x: the filter
        hal::cia_a_pra() = uint8_t((hal::cia_a_pra() & ~2) | ((param & 1) << 1));
        return;
    case 1:                                         // $902e E1x: a fine slide up
        if (p.counter)
            return;
        p.fine = 15;
        slide_up(p, v);
        return;
    case 2:                                         // $907c E2x: a fine slide down
        if (p.counter)
            return;
        p.fine = 15;
        slide_down(p, v);
        return;
    case 3:                                         // $9516 E3x: glissando
        x.glissando = uint8_t((x.glissando & 0xF0) | (param & 15));
        return;
    case 4:                                         // $952a E4x: the vibrato's waveform
        x.waves = uint8_t((x.waves & 0xF0) | (param & 15));
        return;
    case 5:                                         // $953e E5x: the finetune
        x.finetune = param & 15;
        return;
    case 6:                                         // $954c E6x: the pattern loop
        if (p.counter)
            return;
        if (!(param & 15)) {
            x.loop_row = uint8_t(uint16_t(p.row) >> 4);
            return;
        }
        if (x.loop_count) {
            x.loop_count--;
            if (!x.loop_count)
                return;
        } else {
            x.loop_count = param & 15;
        }
        p.break_row = x.loop_row;
        p.loop = 0xFF;
        return;
    case 7:                                         // $958a E7x: the tremolo's waveform
        x.waves = uint8_t((x.waves & 0x0F) | (param << 4));
        return;
    case 9: {                                       // $959c E9x: the note again every x ticks
        uint8_t every = param & 15;
        if (!every)
            return;
        uint32_t tick = p.counter;
        if (!tick) {
            if (x.cell >> 16 & 0xFFF)
                return;
        }
        if (uint16_t(tick) % every)
            return;
        if (p.enabled[v])
            restart(p, v);
        return;
    }
    case 10:                                        // $9604 EAx: the volume up, once
        if (p.counter)
            return;
        volume_add(p, v, param & 15);
        return;
    case 11:                                        // $961a EBx: the volume down, once
        if (p.counter)
            return;
        volume_subtract(p, v, param & 15);
        return;
    case 12:                                        // $9630 ECx: the note cut after x ticks
        if ((param & 15) != p.counter)
            return;
        x.volume = 0;
        if (p.enabled[v])
            aud_volume(v, 0);
        return;
    case 13:                                        // $9652 EDx: the note delayed x ticks
        if ((param & 15) != p.counter)
            return;
        if (!uint16_t(x.cell >> 16))
            return;
        if (p.enabled[v])
            restart(p, v);
        return;
    case 14:                                        // $9670 EEx: the pattern delayed
        if (p.counter)
            return;
        if (p.delay)
            return;
        p.delay_asked = uint8_t((param & 15) + 1);
        return;
    case 15:                                        // $9692 EFx: the funk repeat
        if (p.counter)
            return;
        x.glissando = uint8_t((x.glissando & 0x0F) | (param << 4));
        if (!(param << 4 & 0xFF))
            return;
        funk_repeat(x);
        return;
    }
}

// $8f3a: a voice's effect on the ticks between the rows
void tick_effect(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    funk_repeat(x);
    if (!(uint16_t(x.cell) & 0xFFF)) {
        restore(p, v);
        return;
    }
    uint8_t command = byte_of(x.cell, 2) & 15;
    switch (command) {
    case 0: arpeggio(p, v); return;
    case 1: slide_up(p, v); return;
    case 2: slide_down(p, v); return;
    case 3: tone_portamento(p, v); return;
    case 4: vibrato(p, v); return;
    case 5:
        tone_portamento_go(p, v);
        volume_slide(p, v);
        return;
    case 6:
        vibrato_go(p, v);
        volume_slide(p, v);
        return;
    case 14: effect_e(p, v); return;
    }
    if (p.enabled[v])
        aud_period(v, x.period);
    if (command == 7)
        tremolo(p, v);
    else if (command == 10)
        volume_slide(p, v);
}

// $9434: a voice's effect on the row's tick
void row_effect(PtBlock &p, int v)
{
    Voice &x = p.voice[v];
    funk_repeat(x);
    uint8_t param = byte_of(x.cell, 3);
    switch (byte_of(x.cell, 2) & 15) {
    case 8:                                         // $94f2 8xy: the signal
        p.signal = param;
        restore(p, v);
        return;
    case 9: {                                       // $931a 9xy: the sample's offset
        if (param)
            x.offset = param;
        uint16_t offset = uint16_t(uint16_t(x.offset) << 7);
        if (int16_t(offset) >= int16_t(uint16_t(x.length))) {
            x.length = 1;
            return;
        }
        x.length = uint16_t(uint16_t(x.length) - offset);
        x.start = x.start + uint16_t(offset * 2);
        return;
    }
    case 11: {                                      // $93a4 Bxy: the song's position
        uint8_t to = uint8_t(param - 1);
        if (!(int8_t(to) >= int8_t(p.position)))
            p.signal = 0xFF;
        p.position = to;
        p.break_row = 0;
        p.jump = 0xFF;
        return;
    }
    case 13: {                                      // $93f2 Dxy: the next pattern, from a row (BCD)
        uint8_t row = uint8_t((param >> 4) * 10 + (param & 15));
        if (row > 63) {
            p.break_row = 0;
            p.jump = 0xFF;
            return;
        }
        p.break_row = row;
        p.jump = 0xFF;
        return;
    }
    case 14: effect_e(p, v); return;
    case 15:                                        // $9416 Fxy: the speed or the tempo
        if (!param)
            return;
        if (param < 32) {
            p.counter = 0;
            p.speed = param;
        } else {
            set_tempo(p, param);
        }
        return;
    case 12: {                                      // $93c2 Cxy: the volume
        if (param > 64)
            param = 64;
        if (!p.enabled[v])
            return;
        x.volume = param;
        uint16_t scaled = uint16_t(uint32_t(param) * uint16_t(p.volume) >> 6);
        aud_volume(v, scaled);
        if (uint16_t(x.cell >> 16) & 0xFFF)
            p.vu[v] = uint8_t(scaled);
        return;
    }
    default:
        restore(p, v);
        return;
    }
}

// $8ca8: the note of a voice on the row
void read_note(PtBlock &p, int v, uint32_t at)
{
    Voice &x = p.voice[v];
    if (!x.cell)
        restore(p, v);
    uint32_t note = peek_l(decode_context_cursor(p.module + 1084 + at));
    x.cell = note;
    uint32_t sample = (byte_of(note, 2) >> 4) | (byte_of(note, 0) & 0xF0);
    if (sample) {
        // (the module's sample header: the length, the finetune, the volume, the repeat)
        uint32_t header = p.module + 12 + 30 * sample;
        ContextCursor h = decode_context_cursor(header);
        x.start = p.sample[sample - 1];
        x.length = peek_w(h);
        x.length_again = peek_w(h);
        x.finetune = host::detail::read_cursor_byte(h + 2);
        x.volume = host::detail::read_cursor_byte(h + 3);
        uint16_t repeat = peek_w(h + 4), repeat_length = peek_w(h + 6);
        if (repeat) {
            uint32_t at_repeat = x.start + uint16_t(repeat + repeat);
            x.loop_start = at_repeat;
            x.funk_at = at_repeat;
            x.length = uint16_t(repeat + repeat_length);
            x.loop_length = repeat_length;
        } else {
            x.loop_start = x.start;
            x.funk_at = x.start;
            x.loop_length = repeat_length;
        }
        if (p.enabled[v]) {
            uint16_t scaled = uint16_t(uint32_t(x.volume) * uint16_t(p.volume) >> 6);
            p.vu[v] = uint8_t(scaled);
            aud_volume(v, scaled);
        }
    }
    // the note
    if (!(uint16_t(x.cell >> 16) & 0xFFF)) {
        row_effect(p, v);
        return;
    }
    uint16_t effect12 = uint16_t(x.cell) & 0xFF0;
    uint8_t command = byte_of(x.cell, 2) & 15;
    if (effect12 == 0xE50) {
        x.finetune = byte_of(x.cell, 3) & 15;      // $953e
    } else if (command == 3 || command == 5) {
        tone_target(x);                             // $90ca
        row_effect(p, v);
        return;
    } else if (command == 9) {
        row_effect(p, v);
    }
    // $8db4: the note's period
    {
        uint16_t note = x.cell >> 16 & 0xFFF;
        unsigned index = 0;
        while (index < 37 && !(note >= pt::Periods[index]))
            index++;
        x.period = periods(72, x.finetune)[index];
    }
    // $8de8
    if ((uint16_t(x.cell) & 0xFF0) == 0xED0) {
        row_effect(p, v);
        return;
    }
    if (p.enabled[v])
        dma(x.dma);
    if (!(x.waves & 4))
        x.vibrato_pos = 0;
    if (!(x.waves & 0x40))
        x.tremolo_pos = 0;
    if (p.enabled[v]) {
        aud_pointer(v, x.start);
        aud_length(v, x.length);
        aud_period(v, x.period);
        p.starting = uint16_t(p.starting | uint16_t(x.dma));
    }
    row_effect(p, v);
}

// $8bca: the effects of the voices
void effects(PtBlock &p)
{
    for (int v = 0; v < 4; v++)
        tick_effect(p, v);
}

// $8ef2 on: the song goes on at the next pattern when this one is done (or a jump or a break asked for it)
void end_of_row(PtBlock &p)
{
    if (!(uint16_t(p.row) >= 1024) && !p.jump)
        return;
    // $8efa
    p.row = uint16_t(p.break_row << 4);
    p.break_row = 0;
    p.jump = 0;
    p.position = uint8_t((p.position + 1) & 127);
    uint8_t length = host::detail::read_cursor_byte(decode_context_cursor(p.module + 950));
    if (p.position >= length) {
        p.position = 0;
        p.signal = 0xFF;
    }
}

// $8eb2: the next row (a pattern delay holds it, a pattern loop sends it back)
void next_row(PtBlock &p)
{
    p.row = uint16_t(uint16_t(p.row) + 16);
    if (p.delay_asked) {
        p.delay = p.delay_asked;
        p.delay_asked = 0;
    }
    if (p.delay) {
        p.delay--;
        if (p.delay)
            p.row = uint16_t(uint16_t(p.row) - 16);
    }
    if (p.loop) {
        p.loop = 0;
        uint8_t row = p.break_row;
        p.break_row = 0;
        p.row = uint16_t(row << 4);
    }
    end_of_row(p);
}

// $8b90: the tick
void tick()
{
    PtBlock &p = block();
    for (int v = 0; v < 4; v++)
        p.enabled[v] = uint8_t(p.user[v] & p.free[v]);
    p.counter++;
    if (p.counter < p.speed) {
        effects(p);
        end_of_row(p);                              // $8f2e: a jump asked for on this row
        return;
    }
    p.counter = 0;
    if (p.delay) {
        effects(p);
        next_row(p);
        return;
    }
    // $8c04: a new row
    p.starting = 0;
    uint8_t order = host::detail::read_cursor_byte(decode_context_cursor(p.module + 952 + p.position));
    uint32_t at = uint32_t(order) << 10;
    at = (at & 0xFFFF0000u) | uint16_t(uint16_t(at) + uint16_t(p.row));
    for (int v = 0; v < 4; v++, at += 4)
        read_note(p, v, at);
    // $8e44: the channels that got a note start, then their repeats
    dma_wait();
    dma(uint16_t(uint16_t(p.starting) | 0x8000));
    dma_wait();
    for (int v = 3; v >= 0; v--)
        if (p.enabled[v]) {
            aud_pointer(v, p.voice[v].loop_start);
            aud_length(v, p.voice[v].loop_length);
        }
    next_row(p);
}

}  // namespace

// ---- the player's calls (R17d's commands 1-5), the interrupts ---------------------------------------

namespace {

constexpr uint32_t ChipClear = 0x10002;     // MEMF_CHIP | MEMF_CLEAR

uint32_t block_address() { return address(&block()) + 318; }      // the library's "a5" of the player

bool pal() { return hal::vblank_frequency() == 50; }

// $8a0c, command 4: the block, told where the extension's data is
PtBlock &install_block()
{
    PtBlock &p = block();
    set32(Player, block_address());
    p.extension = address(e(0));
    return p;
}

// $8b28, command 3: the four channels silent, their DMA off
void stop_audio()
{
    for (int v = 0; v < 4; v++)
        aud_volume(v, 0);
    dma(0x000F);
}

// $8a82, command 1: a module to play (it must say M.K. or M!K!), from a position
void init(uint32_t module, uint8_t start)
{
    uint32_t sign = peek_l(decode_context_cursor(module + 1080));
    if (sign != 0x4D2E4B2E && sign != 0x4D214B21)
        fon_call();                                 // $aa6c
    PtBlock &p = install_block();
    p.module = module;
    // the highest pattern of the order (the library's loop takes two off the count for each new highest)
    {
        ContextCursor order = decode_context_cursor(module + 952);
        uint32_t d1 = 0, d2 = 0;
        int16_t d0 = 127;
        unsigned i = 0;
        for (;;) {
            d2 = d1;
            d0 = int16_t(d0 - 1);
            bool again = false;
            for (;;) {
                d1 = host::detail::read_cursor_byte(order + i++);
                if (int8_t(uint8_t(d1)) > int8_t(uint8_t(d2))) {
                    again = true;
                    break;
                }
                if (d0 == 0)
                    break;
                d0 = int16_t(d0 - 1);
            }
            if (!again)
                break;
        }
        uint32_t patterns = uint8_t(d2 + 1);
        uint32_t at = patterns << 10;
        at += 1084 + module;
        // the 31 samples' data, one after the other
        ContextCursor header = decode_context_cursor(module);
        for (int n = 0; n < 31; n++) {
            p.sample[n] = at;
            at += 2 * uint32_t(peek_w(header + 42));
            header += 30;
        }
    }
    hal::cia_a_pra() = uint8_t(hal::cia_a_pra() | 2);       // the filter off
    p.speed = 6;
    p.bpm = 125;
    p.counter = 0;
    p.position = start;
    for (int v = 0; v < 4; v++)
        p.free[v] = 0xFF;
    for (int v = 0; v < 4; v++)
        p.vu[v] = 0;
    p.signal = 0;
    p.pad3 = 0;
    p.row = 0;
    p.break_row = 0;
    p.jump = 0;
    p.loop = 0;
    p.fine = 0;
    p.delay_asked = 0;
    p.delay = 0;
    p.pad5 = 0;
    // the voices' records, but their DMA bits and the portamento bytes after them
    for (int v = 0; v < 4; v++) {
        Voice &x = p.voice[v];
        uint8_t *bytes = reinterpret_cast<uint8_t *>(&x);
        for (int i = 0; i < 20; i++)
            bytes[i] = 0;
        for (int i = 24; i < 44; i++)
            bytes[i] = 0;
    }
    stop_audio();
}

// The time a CIA timer takes to count its latch + 1: the clock is 709379 Hz (PAL) or 715909 Hz (NTSC)
uint32_t timer_microseconds(uint32_t latch) { return (latch + 1) * 1000 / (pal() ? 709 : 716); }

// $8b40: the tempo. Fxx from 32, Pt Cia Speed and the start. The timer's period is the clock over it
void set_tempo(PtBlock &p, uint32_t value)
{
    int16_t tempo = int16_t(value);
    if (tempo < 32)
        tempo = 32;
    tempo &= 255;
    p.bpm = uint16_t(tempo);
    if (!e16(CiaInstalled))
        return;
    uint32_t clock = e32(CiaConstant);
    uint32_t latch = (clock / uint16_t(tempo)) & 0xFFFF;
    hal::timer_change(timer_microseconds(latch));
}

void tick_interrupt() { tick(); }

// $87fa: the player's tick from the vertical blank
void install_vbl()
{
    if (e16(VblInstalled))
        return;
    ext3::vbl_add(tick_interrupt);
    set16(VblInstalled, 0xFFFF);
}

// $8972
void remove_vbl()
{
    if (!e16(VblInstalled))
        return;
    ext3::vbl_remove(tick_interrupt);
    set16(VblInstalled, 0);
}

// $8850: the player's tick from a timer of the CIAs (the library's default). Where there is none, the
// vertical blank, which is the same at the library's tempo of 125
void install_cia()
{
    if (e16(CiaInstalled))
        return;
    PtBlock &p = block();
    uint32_t clock = pal() ? 1773447 : 1789773;
    set32(CiaConstant, clock);
    uint32_t latch = (clock / uint16_t(p.bpm)) & 0xFFFF;
    if (!hal::timer_start(tick_interrupt, timer_microseconds(latch))) {
        install_vbl();
        return;
    }
    set16(CiaInstalled, 0xFFFF);
}

// $8996
void remove_cia()
{
    if (!e16(CiaInstalled))
        return;
    hal::timer_stop();
    set16(CiaInstalled, 0);
}

// R178 or R179, as Pt Cia Speed says
void install_clock()
{
    if (!e16(VblMode))
        install_cia();
    else
        install_vbl();
}

// $87e4, R177's body: a sample of the memory played on voices (a mask of them), at a frequency, once
// (the time the length takes at that frequency is the voices' lease) or for ever
void raw_play(uint32_t mask, uint32_t data, int32_t length, uint32_t frequency, bool loop);

// R0ef: the voice of a mask the sound effects may take
uint32_t free_voice(uint32_t mask)
{
    PtBlock &p = install_block();
    uint32_t d7 = mask;
    if (!d7)
        return 0;
    d7 &= 15;
    if (d7 == 1 || d7 == 2 || d7 == 4 || d7 == 8)
        return d7;
    uint32_t d3 = 0;
    int count = 0;
    if ((d7 & 8) && p.free[3]) {
        d3 = 8;
        count++;
    }
    if ((d7 & 4) && p.free[2]) {
        d3 = 4;
        count++;
    }
    if ((d7 & 2) && p.free[1]) {
        d3 = 2;
        count++;
    }
    if ((d7 & 1) && p.free[0]) {
        d3 = 1;
        count++;
    }
    if (count == 1)
        return d3;
    if (count == 0) {
        // none is free: the one with the most time left (the leases, as signed words)
        uint32_t bit = 1;
        d3 = 0;
        int16_t best = -1;
        for (int v = 0; v < 4; v++, bit <<= 1) {
            int16_t lease = int16_t(uint16_t(p.lease[v]));
            if (d7 & bit && best < lease) {
                best = lease;
                d3 = bit;
            }
        }
        return d3;
    }
    uint32_t d4 = 0;
    for (int v = 0; v < 4; v++)
        if (p.free[v])
            d4 += 1u << v;
    d4 &= d7;
    if (e32(VblInstalled)) {                        // the player is playing: the voices it has not been given first
        uint32_t d5 = 0;
        for (int v = 0; v < 4; v++)
            if (!p.user[v])
                d5 += 1u << v;
        d5 &= d4;
        if (d5)
            d4 = d5;
        else {
            // the voice with the shortest repeat, then (from where that left off) the shortest sample
            uint32_t bit = 1;
            d3 = 0;
            uint16_t shortest = 0xFFFF;
            int v = 0;
            for (; v < 4; v++, bit <<= 1) {
                uint16_t repeat = p.voice[v].loop_length;
                if (d4 & bit && !(shortest < repeat)) {
                    if (int16_t(repeat) <= 1)
                        break;
                    shortest = repeat;
                    d3 = bit;
                }
            }
            if (v < 4) {
                for (; v < 4; v++, bit <<= 1) {
                    uint16_t length = p.voice[v].length;
                    if (d4 & bit && !(int16_t(p.voice[v].loop_length) > 1) && !(shortest < length)) {
                        shortest = length;
                        d3 = bit;
                    }
                }
            }
            return d3;
        }
    }
    if (d4 & 1)
        return 1;
    if (d4 & 2)
        return 2;
    if (d4 & 4)
        return 4;
    return 8;
}

void raw_play(uint32_t mask, uint32_t data, int32_t length, uint32_t frequency, bool loop)
{
    if (!length)
        return;
    PtBlock &p = block();
    if (int16_t(mask) == 0)
        return;
    if (int16_t(mask) < 0) {
        uint32_t wanted = uint16_t(-int16_t(mask)) & 15;
        mask = free_voice(wanted);
    }
    uint32_t voices = mask & 15;
    for (int v = 0; v < 4; v++)
        if (voices >> v & 1) {
            p.free[v] = 0;
            p.lease[v] = 0xFFFF;
        }
    dma(uint16_t(voices));
    uint32_t started = voices;
    uint32_t words = uint32_t(length) >> 1;
    int16_t rate = int16_t(frequency);
    if (!(rate > 400))
        rate = 400;
    if (!(rate < 30000))
        rate = 30000;
    uint16_t lease = 0xFFFE;
    if (!loop)
        lease = uint16_t(uint16_t(uint32_t(uint16_t(words)) * 100 / uint16_t(rate)) + 1);
    uint16_t period = uint16_t(3579545 / uint16_t(rate));
    uint16_t volume = e16(SampleVolume);
    for (int v = 0; v < 4; v++)
        if (voices >> v & 1) {
            p.lease[v] = lease;
            aud_pointer(v, data);
            aud_length(v, uint16_t(words));
            aud_period(v, period);
            aud_volume(v, volume);
        }
    dma_wait();
    dma(uint16_t(started | 0x8000));
    if (!loop) {
        dma_wait();
        for (int v = 0; v < 4; v++)
            if (started >> v & 1) {
                aud_pointer(v, e32(Silence));
                aud_length(v, 1);
            }
    }
}

// $1d06: the vertical blank's work: the second mouse, the sound effects' leases
void vertical_blank()
{
    // the second mouse's counters (JOY1DAT: the vertical in the high byte, the horizontal in the low)
    uint16_t joy = reg(0x0C);
    int16_t x = int8_t(joy & 0xFF), y = int8_t(joy >> 8);
    int16_t old_x = int16_t(e16(MouseOldX)), old_y = int16_t(e16(MouseOldY));
    set16(MouseOldX, uint16_t(x));
    set16(MouseOldY, uint16_t(y));
    int16_t dx = int16_t(x - old_x), dy = int16_t(y - old_y);
    if (!(dx > -127))
        dx += 256;
    if (!(dx < 127))
        dx -= 256;
    if (!(dy > -127))
        dy += 256;
    if (!(dy < 127))
        dy -= 256;
    int16_t px = int16_t(dx + int16_t(e16(MouseX))), py = int16_t(dy + int16_t(e16(MouseY)));
    uint16_t shift = e16(MouseShift);
    int16_t min_x = int16_t(int16_t(e16(MouseMinX)) << shift), min_y = int16_t(int16_t(e16(MouseMinY)) << shift);
    int16_t max_x = int16_t(int16_t(e16(MouseMaxX)) << shift), max_y = int16_t(int16_t(e16(MouseMaxY)) << shift);
    if (!(px > min_x))
        px = min_x;
    if (!(px < max_x))
        px = max_x;
    if (!(py > min_y))
        py = min_y;
    if (!(py < max_y))
        py = max_y;
    set16(MouseX, uint16_t(px));
    set16(MouseY, uint16_t(py));
    // the leases
    PtBlock &p = block();
    for (int v = 0; v < 4; v++) {
        if (p.free[v])
            continue;
        int16_t lease = int16_t(uint16_t(p.lease[v]));
        if (lease < 0)
            continue;
        p.lease[v] = uint16_t(lease - 1);
        if (lease - 1 != 0)
            continue;
        p.free[v] = 0xFF;
        dma(uint16_t(1 << v));
    }
}

// R10b: Pt Stop
void stop_player()
{
    install_block();
    if (!e16(CiaInstalled) && !e16(VblInstalled))
        return;
    remove_cia();
    remove_vbl();
    stop_audio();
}

// R109: Pt Play, R10a: Pt Continue, R107: Pt Bank: the module address of a bank (or an address)
uint32_t module_of(int32_t value, bool must_be_chip)
{
    uint32_t at = bank_or_address(value);
    // (the library wants the module below 2MB, in chip memory: a bank is checked for being one)
    if (must_be_chip && value >= 0 && value < 1024 && !(bank_flags(decode_context_cursor(at)) & BankChip))
        fon_call();
    return at;
}

// A sample of the bank Pt Sam Bank made: the number (negative: repeated), where its data is, its
// frequency, its length
struct BankSample {
    uint32_t data;
    uint32_t frequency;
    uint32_t length;
    bool loop;
};

BankSample bank_sample(int32_t number, bool with_frequency)
{
    uint32_t bank = e32(SampleBank);
    if (!bank)
        fon_call();
    bool loop = false;
    if (!number)
        fon_call();
    if (number < 0) {
        number = -number;
        loop = true;
    }
    ContextCursor b = decode_context_cursor(bank);
    if (uint16_t(number) > peek_w(b))
        fon_call();
    uint32_t offset = peek_l(b + (uint32_t(uint16_t(number)) * 4 - 2));
    ContextCursor record = b + (offset + 8);
    BankSample s;
    s.loop = loop;
    s.frequency = with_frequency ? peek_w(record) : 0;
    s.length = peek_l(record + 2);
    s.data = encode_context_cursor(record + 6);
    return s;
}

}  // namespace

extern "C" {

void amcaf_default();
void amcaf_end();
void amcaf_bank_check();

// PT PLAY bank [,position] (R108, R109)
void amcaf_pt_play_2(int32_t bank, int32_t position)
{
    stop_player();
    set32(ModuleBank, uint32_t(bank));
    uint32_t module = module_of(bank, true);
    set32(Module, module);
    init(module, uint8_t(position));
    install_clock();
}
void amcaf_pt_play_1(int32_t bank) { amcaf_pt_play_2(bank, 0); }

// PT STOP (R10b)
void amcaf_pt_stop() { stop_player(); }

// PT CONTINUE (R10a): the module again, from where it is
void amcaf_pt_continue()
{
    install_block();
    if (!e32(Module))
        fon_call();
    install_clock();
}

// PT BANK bank (R107): a module made ready, not played
void amcaf_pt_bank(int32_t bank)
{
    uint32_t module = module_of(bank, true);
    set32(Module, module);
    init(module, 0);
}

// PT VOLUME v (R105): the master volume, 0 to 64
void amcaf_pt_volume(int32_t volume)
{
    PtBlock &p = install_block();
    if (volume < 0)
        volume = 0;
    if (volume > 64)
        volume = 64;
    p.volume = uint16_t(volume);
}

// PT VOICE mask (R106): the voices the music may play (the others are silenced)
void amcaf_pt_voice(int32_t mask)
{
    PtBlock &p = install_block();
    for (int v = 0; v < 4; v++)
        p.user[v] = 0xFF;
    for (int v = 0; v < 4; v++)
        if (!(mask >> v & 1)) {
            dma(uint16_t(1 << v));
            aud_volume(v, 0);
            p.user[v] = 0;
        }
}

// =PT VU(voice) (R104): the volume the voice's last note began with, once
int32_t amcaf_pt_vu(int32_t voice)
{
    if (voice < 0 || voice >= 4)
        fon_call();
    PtBlock &p = install_block();
    int32_t v = p.vu[voice];
    p.vu[voice] = 0;
    return v;
}

// =PT SIGNAL (R10c): the last 8xy's parameter (255 when the song ended), once
int32_t amcaf_pt_signal()
{
    PtBlock &p = install_block();
    int32_t v = p.signal;
    p.signal = 0;
    return v;
}

// =PT CPATTERN, =PT CPOS (R0f0, R0f1): where the song is
int32_t amcaf_pt_cpattern() { return block().position; }
int32_t amcaf_pt_cpos() { return uint16_t(block().row) >> 4; }

// =PT CINSTR(voice), =PT CNOTE(voice) (R0f2, R0f3): the sample of a voice's note, its frequency
int32_t amcaf_pt_cinstr(int32_t voice)
{
    if (voice < 0 || voice >= 4)
        fon_call();
    return byte_of(block().voice[voice].cell, 2) >> 4;
}
int32_t amcaf_pt_cnote(int32_t voice)
{
    if (voice < 0 || voice >= 4)
        fon_call();
    uint16_t period = block().voice[voice].period;
    return period ? int32_t(3579545 / period & 0xFFFF) : 0;
}

// =PT DATA BASE (R0fd): the player's block
int32_t amcaf_pt_data_base()
{
    install_block();
    return int32_t(block_address());
}

// PT CIA SPEED tempo (R103): 0 the vertical blank ticks the player, else a CIA timer at the tempo
void amcaf_pt_cia_speed(int32_t tempo)
{
    if (!tempo) {
        set16(VblMode, 0xFFFF);
        if (e16(CiaInstalled)) {
            remove_cia();
            install_vbl();
        }
        return;
    }
    set16(VblMode, 0);
    PtBlock &p = install_block();
    if (e16(VblInstalled)) {
        p.bpm = uint16_t(tempo);
        remove_vbl();
        install_cia();
    }
    block().bpm = uint16_t(tempo);
    set_tempo(block(), uint32_t(tempo));
}

// PT SAM BANK bank (R0f9): the sample bank of Pt Sam Play
void amcaf_pt_sam_bank(int32_t bank) { set32(SampleBank, bank_or_address(bank)); }

// PT SAM VOLUME volume, PT SAM VOLUME voices,volume (R0f4, R0f5)
void amcaf_pt_sam_volume_1(int32_t volume)
{
    if (volume < 0)
        volume = 0;
    if (volume > 64)
        volume = 64;
    set16(SampleVolume, uint16_t(volume));
}
void amcaf_pt_sam_volume_2(int32_t mask, int32_t volume)
{
    PtBlock &p = install_block();
    if (volume < 0)
        volume = 0;
    if (volume > 64)
        volume = 64;
    for (int v = 0; v < 4; v++)
        if ((mask >> v & 1) && !p.free[v])
            aud_volume(v, uint16_t(volume));
}

// PT SAM FREQ voices,frequency (R0f6)
void amcaf_pt_sam_freq(int32_t mask, int32_t frequency)
{
    PtBlock &p = install_block();
    if (frequency < 0)
        frequency = 0;
    int16_t rate = int16_t(frequency);
    if (!(rate > 400))
        rate = 400;
    if (!(rate < 30000))
        rate = 30000;
    uint16_t period = uint16_t(3579545 / uint16_t(rate));
    for (int v = 0; v < 4; v++)
        if ((mask >> v & 1) && !p.free[v])
            aud_period(v, period);
}

// PT SAM STOP voices (R0f7): the sound effects on those voices end, the music may have them
void amcaf_pt_sam_stop(int32_t mask)
{
    PtBlock &p = install_block();
    for (int v = 0; v < 4; v++)
        if (mask >> v & 1) {
            p.free[v] = 0xFF;
            dma(uint16_t(1 << v));
            aud_volume(v, 0);
        }
}

// PT RAW PLAY voices,address,length,frequency (R0f8): a sample of the memory (a negative length repeats it)
void amcaf_pt_raw_play(int32_t mask, int32_t at, int32_t length, int32_t frequency)
{
    install_block();
    // (the library negates another register than the length: it stays negative)
    raw_play(uint32_t(mask), uint32_t(at), length, uint32_t(frequency), length < 0);
}

// =PT FREE VOICE [(mask)] (R0ee, R0ef)
int32_t amcaf_pt_free_voice_0() { return int32_t(free_voice(15)); }
int32_t amcaf_pt_free_voice_1(int32_t mask) { return int32_t(free_voice(uint32_t(mask))); }

// PT SAM PLAY [voices,]sample[,frequency] (R0fa, R0fb, R0fc): a sample of Pt Sam Bank's bank
void amcaf_pt_sam_play_1(int32_t sample)
{
    install_block();
    BankSample s = bank_sample(sample, true);
    raw_play(15, s.data, int32_t(s.length), s.frequency, s.loop);
}
void amcaf_pt_sam_play_2(int32_t mask, int32_t sample)
{
    install_block();
    BankSample s = bank_sample(sample, true);
    raw_play(uint32_t(mask), s.data, int32_t(s.length), s.frequency, s.loop);
}
void amcaf_pt_sam_play_3(int32_t mask, int32_t sample, int32_t frequency)
{
    install_block();
    BankSample s = bank_sample(sample, false);
    raw_play(uint32_t(mask), s.data, int32_t(s.length), uint32_t(frequency), s.loop);
}

// =PT INSTR ADDRESS(n), =PT INSTR LENGTH(n) (R101, R102): a sample of the module
int32_t amcaf_pt_instr_address(int32_t n)
{
    PtBlock &p = install_block();
    if (!e32(Module))
        fon_call();
    if (n <= 0 || n > 31)
        fon_call();
    return int32_t(p.sample[n - 1]);
}
int32_t amcaf_pt_instr_length(int32_t n)
{
    uint32_t module = e32(Module);
    if (!module)
        fon_call();
    if (n <= 0 || n > 31)
        fon_call();
    return int32_t(2 * uint32_t(peek_w(decode_context_cursor(module + 12 + 30 * uint32_t(n)))));
}

// PT INSTR PLAY [voices,]sample[,frequency] (R0fe, R0ff, R100): a sample of the module (15625 Hz by default)
void amcaf_pt_instr_play_3(int32_t mask, int32_t n, int32_t frequency)
{
    bool loop = n < 0;
    if (loop)
        n = -n;
    int32_t at = amcaf_pt_instr_address(n);
    int32_t length = amcaf_pt_instr_length(n);
    raw_play(uint32_t(mask), uint32_t(at), length, uint32_t(frequency), loop);
}
void amcaf_pt_instr_play_2(int32_t mask, int32_t n) { amcaf_pt_instr_play_3(mask, n, 15625); }
void amcaf_pt_instr_play_1(int32_t n) { amcaf_pt_instr_play_3(15, n, 15625); }

// R1e26 (+ the library's start): the extension back to its start, at each Default and at the end
void amcaf_default()
{
    stop_player();
    set16(MouseMinX, 0x80);
    set16(MouseMinY, 0x30);
    set16(MouseMaxX, 0x1BF);
    set16(MouseMaxY, 0x12F);
    set16(MouseX, 0);
    set16(MouseY, 0);
    set16(MouseOldX, uint8_t(reg(0x0C)));
    set16(MouseOldY, uint8_t(uint16_t(reg(0x0C)) >> 8));
    set16(MouseShift, 1);
    set16(VblMode, 0);
    set16(SampleVolume, 64);
    PtBlock &p = install_block();
    for (int v = 0; v < 4; v++) {
        p.enabled[v] = 0xFF;
        p.user[v] = 0xFF;
        p.free[v] = 0xFF;
        p.vu[v] = 0;
    }
    p.bpm = 125;
    p.volume = 64;
    p.signal = 0;
}

void amcaf_end()
{
    amcaf_default();
    ext3::vbl_remove(vertical_blank);
}

// $1f48: AMOS says the banks have changed: the module must still be where it was
void amcaf_bank_check()
{
    if (!e32(VblInstalled))
        return;
    ContextCursor bank = bank_find(e32(ModuleBank));
    if (bank && encode_context_cursor(bank) == e32(Module))
        return;
    stop_player();
}

// The cold start's part of the player
void amcaf_start_player()
{
    set32(Silence, address(hal::allocate(4, ChipClear)));
    amcaf_default();
    ext3::vbl_add(vertical_blank);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_pt.inc"
#endif
