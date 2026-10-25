// The instructions of the Music extension, and its start (MusInit, from
// the extension's cold start), its DEFAULT and END routines and its check
// of the banks. See music.h; the interrupt side is music_play.cpp.

#include "core/amiga.h"
#include "ext/music.h"
#include "lib/banks.h"
#include "lib/strings.h"

#include <exec/interrupts.h>
#include <exec/io.h>

#include <dos/dos.h>

#include "hal/files.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::music;

extern "C" {
// music_glue.s: the routines handed to AMOS; a library function with d0,
// d1, a0, a1 from r (d0 back)
extern uint8_t music_default[], music_end[], music_bank_check[], music_sami_entry[];
uint32_t music_lvo(uint32_t *r, uint32_t base, int32_t lvo);
// lib/devices.cpp: amiga.lib
struct MsgPort *lib_create_port(const char *name, int32_t priority);
void lib_delete_port(struct MsgPort *port);
struct IORequest *lib_create_ext_io(struct MsgPort *port, int32_t size);
void lib_delete_ext_io(struct IORequest *io);
}

namespace {

constexpr uint32_t EntNul = 0x80000000u;
constexpr uint16_t Sami_lplay = 1024 * 4;
constexpr int DEBase = 79;

// The music's errors, in the main list
enum : uint32_t {
    WNDef = 178, SNDef, BNSam, STSho, W0Res, MnRes, MNDef, NoNar, NotTracker,
    NoMedLib = 187, NoMedPlayer, NotMed, NotMedBank = 11 + 178,
};

[[noreturn]] void ifonc() { error(23); }
[[noreturn]] void out_of_memory() { error(24); }
[[noreturn]] void music_disk_error() { error(DEBase + 15); }

// The default envelopes, after the periods in the extension
const uint16_t EnvDef[] = {1, 64, 4, 55, 5, 50, 25, 0, 0, 0};
const uint16_t EnvShoot[] = {1, 64, 10, 0, 0, 0};
const uint16_t EnvBoom[] = {1, 64, 10, 50, 50, 0, 0, 0};
const uint16_t EnvBell[] = {1, 64, 4, 40, 25, 0, 0, 0};

// Octaves: offset of the wave's part, its length in words
const uint16_t TFreq[] = {0, 256 / 2, 0, 256 / 2, 256, 128 / 2, 384, 64 / 2, 448, 32 / 2,
                          480, 16 / 2, 496, 8 / 2, 504, 4 / 2, 504, 4 / 2};
// Notes, in Hz
const uint16_t TNotes[] = {
    00,   00,   00,   33,   35,   37,   39,   41,   44,   46,   49,   52,   55,   58,   62,   65,   69,   73,
    78,   82,   87,   92,   98,   104,  110,  117,  123,  131,  139,  147,  156,  165,  175,  185,  196,  208,
    220,  233,  247,  262,  277,  294,  311,  330,  349,  370,  392,  415,  440,  466,  494,  523,  554,  587,
    622,  659,  698,  740,  784,  830,  880,  932,  988,  1046, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661,
    1760, 1865, 1986, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3952, 4186, 4435, 4699,
    4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372,
};

uint16_t bit(uint32_t v) { return uint16_t(1 << v); }

// divu: remainder and quotient, n unchanged if the quotient overflows
uint32_t divu(uint32_t n, uint16_t d)
{
    if (!d || n / d > 0xFFFF)
        return n;
    return (n % d) << 16 | (n / d);
}

// A sample's frequency for a note (index in TNotes)
uint32_t note_frequency(uint16_t frequency, uint16_t note) { return divu(uint32_t(frequency) * TNotes[note], 440) & 0xFFFF; }
volatile uint8_t &low_byte(volatile uint16_t &w) { return reinterpret_cast<volatile uint8_t *>(&w)[1]; }

uint32_t lvo(uint32_t base, int32_t offset, uint32_t d0 = 0, uint32_t d1 = 0, uint32_t a0 = 0, uint32_t a1 = 0)
{
    uint32_t r[4] = {d0, d1, a0, a1};
    return music_lvo(r, base, offset);
}

// ---- voices, envelopes -------------------------------------------------

// StopDma: the voices not in keep stopped, their interrupts too
void stop_dma(uint16_t keep)
{
    uint16_t stop = keep ^ 0x0F;
    custom(DmaCon) = stop;
    custom(IntEna) = uint16_t(stop << 7);
    dma_wait();
}

// VOnOf: the voices of on for the music, the others taken from it
void voices(uint16_t on)
{
    volatile MusicData &m = md();
    uint32_t base = m.MuBase;
    if (!base)
        return;
    m.MuBase = 0;
    uint16_t had = m.MuDMAsk;
    m.MuDMAsk = on;
    uint32_t dummy = m.WaveBase + offsetof(MusWave, WaveDeb);
    uint16_t stop = 0;
    volatile MusTable &t = table_at(base);
    for (uint32_t v = 0; v < 4; v++) {
        if (!(on & bit(v))) {
            if (had & bit(v)) {
                // its registers now somewhere in memory
                stop |= bit(v);
                m.MuChip[v] = dummy;
                low_byte(t.MuStart) &= uint8_t(~bit(v));
                low_byte(t.MuStop) &= uint8_t(~bit(v));
            }
        } else if (!(had & bit(v))) {
            low_byte(m.MuReStart) |= uint8_t(bit(v));
        }
    }
    m.MuBase = base;
    custom(DmaCon) = stop;
}

// EnvOff: the envelopes of the voices of mask stopped
void envelopes_off(uint16_t mask)
{
    volatile MusicData &m = md();
    uint16_t on = m.EnvOn;
    m.EnvOn = 0;
    uint16_t stopped = 0;
    for (uint32_t v = 0; v < 4; v++) {
        if ((mask & bit(v)) && (on & bit(v))) {
            on &= uint16_t(~bit(v));
            stopped |= bit(v);
            w16(audio(v) + 4) = 2;
            w16(audio(v) + 8) = 0;
        }
    }
    m.EnvOn = on;
    m.MuReStart = stopped;
}

// An envelope started on voice v
void envelope_start(uint32_t v, uint32_t table)
{
    volatile MusEnvelope &e = envelope(v);
    e.EnvAd = table;
    e.EnvDeb = table;
    e.EnvVol = e.EnvVol & 0x0000FFFFu;
    uint16_t on = 0, ended = 0;
    env_next(v, e, on, ended);
}

// Vol: the volume of the voices of mask
void volume(uint32_t level, uint16_t mask)
{
    if (level >= 64)
        ifonc();
    for (uint32_t v = 0; v < 4; v++) {
        if (!(mask & bit(v)))
            continue;
        envelope(v).EnvDVol = uint16_t(level);
        volatile SampleInt &s = sample_int(v);
        if (int16_t(s.Sami_dvol) >= 0)
            s.Sami_dvol = uint16_t(level);
    }
}

// MVol: the music's volume
void music_volume(uint32_t level)
{
    volatile MusicData &m = md();
    level &= 63;
    m.MuVolume = uint16_t(level);
    uint32_t base = m.MuBase;
    if (!base)
        return;
    m.MuBase = 0;
    uint32_t t = mb() + offsetof(MusicData, MuBuffer);
    uint16_t n = m.MuNumber;
    do {
        for (uint32_t v = 0; v < 4; v++) {
            volatile MusVoice &vo = voice_at(t + v * MusVoice::VoiLong);
            vo.VoiVol = uint16_t(uint16_t(vo.VoiDVol * level) >> 6);
        }
        t += MusTable::MuLong;
    } while (decrement(n));
    m.MuBase = base;
}

// MuInit
void music_init()
{
    volatile MusicData &m = md();
    m.MuBase = 0;
    m.MuNumber = 0;
    for (uint32_t v = 0; v < 4; v++)
        m.MuChip[v] = audio(v);
    m.MuDMAsk = 0x000F;
    m.MuReStart = 0;
    stop_sound();
}

// ---- waves -----------------------------------------------------------------

// WaveAd: the wave n; at: what links to it (or to the end of the list)
struct Found {
    uint32_t wave, link;
};
Found find_wave(uint16_t n)
{
    uint32_t link = 0, at = mb() + offsetof(MusicData, WaveBase);
    for (uint32_t w = peek_l(ptr<void>(at)); w; w = peek_l(ptr<void>(at))) {
        link = at;
        at = w;
        if (wave_at(at).WaveNb == n)
            return {at, link};
    }
    return {0, at};
}

void delete_wave(const Found &f)
{
    poke_l(ptr<void>(f.link), wave_at(f.wave).WaveNext);
    Regs r;
    r.d[0] = MusWave::WaveLong;
    r.a[1] = f.wave;
    w::release(r.a[1], r.d[0]);
}

// NoWave
void default_waves()
{
    for (int v = 0; v < 4; v++)
        md().Waves[v] = 1;
}

// RazWave
void delete_waves()
{
    envelopes_off(0x0F);
    uint32_t head = mb() + offsetof(MusicData, WaveBase);
    while (uint32_t w = peek_l(ptr<void>(head)))
        delete_wave({w, head});
    default_waves();
}

// NeWave: wave n from 256 bytes at data, and its halves; false if out of
// memory
bool new_wave(uint16_t n, const uint8_t *data)
{
    Found f;
    while ((f = find_wave(n)).wave) {
        envelopes_off(0x0F);
        delete_wave(f);
    }
    Regs r;
    r.d[0] = MusWave::WaveLong;
    r.a[0] = w::reserve(r.d[0], w::ChipClear);
    uint32_t w = r.a[0];
    if (!w)
        return false;
    poke_l(ptr<void>(f.link), w);
    volatile MusWave &wave = wave_at(w);
    wave.WaveNb = n;
    // the default envelope
    for (int i = 0; i < 10; i += 2) {
        wave.WaveEnv[i] = EnvDef[i];
        wave.WaveEnv[i + 1] = EnvDef[i + 1];
        if (!EnvDef[i] && !EnvDef[i + 1])
            break;
    }
    volatile int8_t *to = reinterpret_cast<volatile int8_t *>(wave.WaveDeb);
    for (int i = 0; i < 256; i++)
        to[i] = int8_t(data[i]);
    volatile int8_t *from = to;
    to += 256;
    for (int count = 128; count >= 4; count >>= 1) {
        volatile int8_t *start = to;
        for (int i = 0; i < count; i++) {
            int16_t a = from[2 * i], b = from[2 * i + 1];
            to[i] = int8_t((a + b) >> 1);
        }
        from = start;
        to += count;
    }
    return true;
}

// ---- samples -----------------------------------------------------------

struct Sample {
    uint32_t start, length, frequency;
};

// GetSam: sample n of the sample bank
Sample sample(uint32_t n)
{
    int32_t number = int16_t(md().SamBank);
    if (!number)
        ifonc();
    uint8_t *bank = bank_find(uint32_t(number));
    if (!bank || peek_l(bank - 8) != 0x53616D70)       // "Samp"
        error(BNSam);
    if (!n)
        ifonc();
    if (uint16_t(n) > peek_w(bank))
        error(SNDef);
    uint32_t offset = peek_l(bank + 2 - 4 + uint16_t(n << 2));
    if (!offset)
        error(SNDef);
    uint8_t *s = bank + offset;
    return {address(s + 14), peek_l(s + 10), peek_w(s + 8)};
}

// Div32: d6 / d3, by the original's division (a 1 for the first 16
// subtractions only)
uint32_t div32(uint32_t n, uint32_t d)
{
    uint32_t r = 0, q = 0;
    uint16_t ones = 0xFFFF;
    bool x = false;
    for (int i = 0; i < 32; i++) {
        x = n & 0x80000000u;
        n <<= 1;
        bool out = r & 0x80000000u;
        r = (r << 1) | x;
        x = out;
        if (r >= d) {
            r -= d;
            x = ones & 1;
            ones >>= 1;
        }
        q = (q << 1) | x;
    }
    return q;
}

// SPl0: sample (or noise) on voice v; envelope: 0, or an envelope's
// address (bit 0 set: the sound loops). started, playing: GoSam's masks.
void play_sample(uint32_t v, uint32_t start, uint32_t length, uint32_t frequency, uint32_t envelope_flag,
                 uint16_t &started, uint16_t &playing)
{
    volatile MusicData &m = md();
    custom(DmaCon) = bit(v);
    volatile SampleInt &s = sample_int(v);
    custom(IntEna) = s.Sami_bit;
    volatile MusEnvelope &e = envelope(v);
    uint32_t chip = audio(v);
    w16(chip + 4) = 1;
    s.Sami_adr = start;
    s.Sami_long = length;
    s.Sami_pos = 0;
    s.Sami_rpos = 0;
    if (!(low_byte(m.SamLoops) & bit(v)))
        s.Sami_rpos = uint32_t(-1);
    s.Sami_radr = 0;
    uint32_t period = div32(m.MusClock, frequency);
    if (period < 124)
        period = 124;
    w16(chip + 6) = uint16_t(period);
    playing &= uint16_t(~bit(v));
    s.Sami_dvol = e.EnvDVol;
    if (envelope_flag) {
        s.Sami_rpos = 0;
        if (!(envelope_flag & 1))
            s.Sami_rpos = uint32_t(-1);
        envelope_flag &= ~1u;
        playing |= bit(v);
        s.Sami_dvol = 0xFFFF;
        envelope_start(v, envelope_flag);
    }
    m.Sami_bits |= s.Sami_bit;
    // the first part now
    ext_music_sami(const_cast<SampleInt *>(&s));
    started |= bit(v);
}

// GoSam: a sample on the voices of mask
void go_sample(uint16_t mask, uint32_t start, uint32_t length, uint32_t frequency)
{
    volatile MusicData &m = md();
    uint16_t keep = mask ^ 0x0F;
    stop_dma(keep);
    voices(keep);
    uint16_t playing = m.EnvOn;
    m.EnvOn = 0;
    m.Sami_bits = 0x8000;
    uint16_t started = 0;
    for (int v = 3; v >= 0; v--) {
        if (mask & bit(v)) {
            m.Noise &= uint16_t(~bit(v));
            play_sample(v, start, length, frequency, 0, started, playing);
        }
    }
    dma_wait();
    custom(DmaCon) = uint16_t(started | 0x8000);
    custom(IntEna) = m.Sami_bits;
    m.EnvOn = playing;
}

// ---- sounds ---------------------------------------------------------------

// VPlay: note on voice v, its wave (forced: >= 0) or sample
void play_voice(uint32_t v, uint16_t note, int16_t forced, uint32_t envelope_table, uint16_t &started, uint16_t &playing)
{
    volatile MusicData &m = md();
    custom(DmaCon) = bit(v);
    custom(IntEna) = uint16_t(bit(v) << 7);
    m.Noise &= uint16_t(~bit(v));
    if (!note) {
        custom(DmaCon) = bit(v);
        playing &= uint16_t(~bit(v));
        return;
    }
    note = uint16_t(note + 3);
    int16_t wave = forced;
    if (forced < 0) {
        wave = int16_t(m.Waves[v]);
        if (wave < 0) {
            // a sample
            Sample s = sample(uint16_t(-wave));
            uint32_t frequency = note_frequency(uint16_t(s.frequency), uint16_t(note - 1));
            play_sample(v, s.start, s.length, frequency, 0, started, playing);
            return;
        }
    }
    if (!wave) {
        // the noise
        m.Noise |= bit(v);
        uint32_t w = m.WaveBase;
        uint32_t env = envelope_table ? envelope_table : (w + offsetof(MusWave, WaveEnv)) | 1;
        uint32_t frequency = note_frequency(2000, uint16_t(note - 1));
        play_sample(v, w + offsetof(MusWave, WaveDeb), LNoise, frequency, env, started, playing);
        return;
    }
    Found f = find_wave(uint16_t(wave));
    if (!f.wave)
        error(WNDef);
    uint32_t chip = audio(v);
    uint32_t data = f.wave + offsetof(MusWave, WaveDeb);
    uint16_t n = uint16_t(note - 1);
    const uint16_t *octave = TFreq + 2 * (n / 12);
    w32(chip) = data + octave[0];
    uint16_t length = octave[1];
    w16(chip + 4) = length;
    // (divu: the low word of the divisor; a quotient too big leaves the
    // clock, whose low word is the period)
    uint32_t period = divu(m.MusClock, uint16_t(uint16_t(length << 1) * TNotes[n]));
    if (uint16_t(period) < 124)
        period = 124;
    w16(chip + 6) = uint16_t(period);
    envelope_start(v, envelope_table ? envelope_table : f.wave + offsetof(MusWave, WaveEnv));
    started |= bit(v);
    playing |= bit(v);
}

// GoShot: note on the voices of mask, then wait
void go_shot(uint16_t mask, uint16_t note, uint32_t wait, int16_t forced, uint32_t envelope_table)
{
    volatile MusicData &m = md();
    uint16_t playing = m.EnvOn;
    m.EnvOn = 0;
    m.Sami_bits = 0x8000;
    uint16_t started = 0;
    for (int v = 3; v >= 0; v--)
        if (mask & bit(v))
            play_voice(v, note, forced, envelope_table, started, playing);
    dma_wait();
    custom(DmaCon) = uint16_t(started | 0x8000);
    custom(IntEna) = m.Sami_bits;
    m.EnvOn = playing;
    if (wait) {
        Regs r;
        r.d[3] = wait;
        lib_call(L_WaitRout, r);
    }
}

// GoBel
void go_bell(uint16_t mask, uint32_t note, uint32_t wait, int16_t forced, uint32_t envelope_table)
{
    if (note > 96)
        ifonc();
    uint16_t keep = mask ^ 0x0F;
    stop_dma(keep);
    voices(keep);
    go_shot(mask, uint16_t(note), wait, forced, envelope_table);
}

// Shout: noise down the four voices
void shout(uint16_t note, const uint16_t *env)
{
    stop_dma(0);
    voices(0);
    for (uint16_t mask = 0b1000; mask; mask >>= 1, note++)
        go_shot(mask, note, 0, 0, address(env));
}

// ---- the narrator ------------------------------------------------------

struct IOStdReq *write_io() { return ptr<struct IOStdReq>(md().WriteIo); }
uint8_t *read_io() { return const_cast<uint8_t *>(md().ReadIo); }

// Sami_install / Sami_remove: the audio interrupts are the samples'
void samples_install()
{
    volatile MusicData &m = md();
    if (m.Sami_flag)
        return;
    m.Sami_oldena = custom(IntEnaR) & 0b0000011110000000;
    sami_stop();
    for (uint32_t v = 0; v < 4; v++) {
        volatile SampleInt &s = sample_int(v);
        auto is = const_cast<struct Interrupt *>(reinterpret_cast<volatile struct Interrupt *>(s.Sami_is));
        is->is_Data = is;
        is->is_Code = ptr<void (void)>(m.Sami_handad);
        is->is_Node.ln_Type = 2;
        is->is_Node.ln_Pri = 0;
        s.Sami_old = address(hal::set_interrupt(int32_t(7 + v), is));
    }
    m.Sami_flag = uint16_t(m.Sami_flag - 1);
}

void samples_remove()
{
    volatile MusicData &m = md();
    if (!m.Sami_flag)
        return;
    sami_stop();
    for (uint32_t v = 0; v < 4; v++)
        hal::set_interrupt(int32_t(7 + v), ptr<struct Interrupt>(sample_int(v).Sami_old));
    custom(DmaCon) = 0x000F;
    // (without the "set" bit, as the original: it clears them)
    custom(IntEna) = m.Sami_oldena;
    m.Sami_flag = 0;
}

// NarInit: the narrator's defaults
void narrator_defaults()
{
    if (struct IOStdReq *io = write_io()) {
        uint8_t *b = reinterpret_cast<uint8_t *>(io);
        poke_w(b + 48, 150);
        poke_w(b + 50, 110);
        poke_w(b + 52, 0);
        poke_w(b + 54, 0);
        poke_l(b + 56, address(md().Amaps));
        poke_w(b + 60, 4);
        poke_w(b + 62, 63);
        poke_w(b + 64, 22200);
    }
}

// TALK STOP: a speech in multitask mode stopped
void talk_stop()
{
    struct IOStdReq *io = write_io();
    if (!io)
        return;
    uint8_t *b = reinterpret_cast<uint8_t *>(io);
    if (!b[66])
        return;
    b[66] = 0;
    if (!hal::check_io(reinterpret_cast<struct IORequest *>(io)))
        hal::abort_io(reinterpret_cast<struct IORequest *>(io));
    hal::wait_io(reinterpret_cast<struct IORequest *>(io));
    samples_install();
}

// OpNar: the narrator device and the translator open
void narrator_open()
{
    volatile MusicData &m = md();
    if (m.TranBase) {
        talk_stop();
        return;
    }
    m.WritePort = address(lib_create_port(nullptr, 0));
    m.WriteIo = address(lib_create_ext_io(ptr<struct MsgPort>(m.WritePort), 88));
    if (hal::open_device(const_cast<char *>("narrator.device"), 0, ptr<struct IORequest>(m.WriteIo), 0))
        error(NoNar);
    m.ReadPort = address(lib_create_port(nullptr, 0));
    uint8_t *r = read_io();
    const uint8_t *w = ptr<uint8_t>(m.WriteIo);
    for (int i = 0; i < 88; i++)
        r[i] = w[i];
    poke_l(r + 14, m.ReadPort);
    narrator_defaults();
    m.TranBase = address(hal::open_library(const_cast<char *>("translator.library"), 0));
    if (!m.TranBase)
        error(NoNar);
    hal::delay(50);
    m.Sami_flag = 0;
    samples_install();
}

// Say
void say(const uint8_t *text, uint32_t multi)
{
    volatile MusicData &m = md();
    stop_dma(0);
    voices(0);
    m.EnvOn = 0;
    m.Noise = 0;
    uint16_t length = str_len(text);
    const uint8_t *s = str_chars(text);
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    uint32_t count;
    if (*s == '~' && length < 1024 && int16_t(length - 2) >= 0) {
        // phonemes
        s++;
        for (int i = 0; i < length - 1; i++)
            buffer[i] = s[i];
        uint8_t *end = buffer + length - 1;
        *end++ = 'Q';
        *end++ = '#';
        *end++ = 'U';
        *end++ = 0;
        *end = 0;
        count = uint16_t(length + 4);
    } else {
        if (*s == '~')
            s++;
        for (int i = 0; i < 255 * 4; i++)
            buffer[i] = 0;
        if (uint16_t(lvo(m.TranBase, -30, length, 1024, address(s), address(buffer)))) {
            voices(0x0F);
            return;
        }
        count = 1024;
    }
    samples_remove();
    struct IOStdReq *io = write_io();
    io->io_Command = CMD_WRITE;
    io->io_Length = count;
    io->io_Data = buffer;
    uint8_t *b = reinterpret_cast<uint8_t *>(io);
    if (!uint16_t(multi)) {
        b[66] = 0;
        hal::do_io(reinterpret_cast<struct IORequest *>(io));
        samples_install();
        voices(0x0F);
    } else {
        b[66] = 1;      // and the mouths
        hal::send_io(reinterpret_cast<struct IORequest *>(io));
    }
}

// ---- trackers ----------------------------------------------------------

// TRACK STOP
void track_stop()
{
    volatile MusicData &m = md();
    if (!m.mt_on)
        return;
    m.mt_on = 0;
    m.mt_dmacon = 0;
    m.Track_Stop = 0;
    for (uint32_t v = 0; v < 4; v++)
        w16(audio(v) + 8) = 0;
    custom(DmaCon) = 0x000F;
}

void samples_stop(uint32_t mask)
{
    mask &= 0x0F;
    custom(DmaCon) = uint16_t(mask);
    custom(IntEna) = uint16_t(mask << 7);
}

void med_stop()
{
    volatile MusicData &m = md();
    if (!m.Med_On)
        return;
    m.Med_On = 0;
    lvo(m.Med_Base, -54);
}

void med_close()
{
    volatile MusicData &m = md();
    med_stop();
    m.Med_Midi = 0;
    m.Med_Adr = 0;
    if (uint32_t base = m.Med_Base) {
        lvo(base, -36);
        m.Med_Base = 0;
        hal::close_library(ptr<struct Library>(base));
    }
}

void med_open()
{
    volatile MusicData &m = md();
    if (m.Med_Base)
        return;
    m.Med_Base = address(hal::open_library(const_cast<char *>("medplayer.library"), 2));
    if (!m.Med_Base)
        error(NoMedLib);
    if (lvo(m.Med_Base, -30, m.Med_Midi)) {
        med_close();
        error(NoMedPlayer);
    }
}

// A file into Name1 with its path
void file_name(const uint8_t *name)
{
    uint16_t n = uint16_t(str_len(name) - 1);
    if (n >= 128)
        ifonc();
    char *to = ptr<char>(idata().Name1);
    const uint8_t *from = str_chars(name);
    for (uint16_t i = 0; i <= n; i++)
        to[i] = char(from[i]);
    to[n + 1] = 0;
    path_it();
}

// A file loaded into a new bank (Track Load, Med Load): its address
uint8_t *load_bank(const uint8_t *name, uint32_t number, uint16_t flags, uint32_t extra, const char *bank_name)
{
    file_name(name);
    BPTR file = hal::open(ptr<char>(idata().Name1), MODE_OLDFILE);
    if (!file)
        music_disk_error();
    int32_t size = hal::seek(file, hal::seek(file, 0, OFFSET_END), OFFSET_BEGINNING);
    uint8_t *bank = bank_reserve(number, flags, uint32_t(size) + extra, bank_name);
    if (!bank) {
        hal::close(file);
        out_of_memory();
    }
    int32_t read = hal::read(file, bank, size);
    hal::close(file);
    if (read != size)
        ifonc();
    return bank;
}

// A file of the program (Sload, Ssave), open and not a directory one
BPTR program_file(uint32_t n)
{
    if (n >= 10)
        ifonc();
    if (!n)
        ifonc();
    uint8_t *f = idata().Fichiers + 10 * (n - 1);
    BPTR handle = BPTR(peek_l(f));
    if (!handle || (f[4] & 4))
        ifonc();
    return handle;
}

} // namespace

extern "C" {

// ---- the extension's routines ------------------------------------------

// MusInit, from the cold start: the extension's routines and data
void ext_music_init()
{
    IData &d = idata();
    volatile MusicData &m = md();
    d.ExtAdr[ExtNb * 4] = mb();
    d.ExtAdr[ExtNb * 4 + 1] = address(music_default);
    d.ExtAdr[ExtNb * 4 + 2] = address(music_end);
    d.ExtAdr[ExtNb * 4 + 3] = address(music_bank_check);
    d.VblRout[0] = address(ext_music_vbl);
    m.MusClock = 3546895;
    m.TempoBase = 100;
    Regs r;
    ec_call(ec::NTSC, r);
    if (uint16_t(r.d[1])) {
        m.TempoBase = 120;
        m.MusClock = 3579545;
    }
    m.Sami_handad = address(music_sami_entry);
    // the data of the extension's data zone
    for (uint32_t v = 0; v < 4; v++) {
        volatile SampleInt &s = sample_int(v);
        s.Sami_bit = uint16_t(0b10000000 << v);
        s.Sami_dma = uint16_t(v);
        s.Sami_reg = uint16_t(Audio + 16 * v);
    }
    m.FoEnd = 0x8000;
    m.Amaps[0] = 3;
    m.Amaps[1] = 5;
    m.Amaps[2] = 10;
    m.Amaps[3] = 12;
    m.Med_Bank = 7;
    m.Track_Bank = 6;
    m.mt_speed = 6;
    for (uint32_t v = 0; v < 4; v++)
        m.mt_voice[14 * v + 13] = bit(v);
}

// MusDef, at each Default (Run...)
void ext_music_default()
{
    volatile MusicData &m = md();
    talk_stop();
    narrator_defaults();
    track_stop();
    m.Track_Loop = 0;
    med_close();
    m.Med_Midi = 0;
    custom(DmaCon) = 0x000F;
    samples_install();
    delete_waves();
    // a square wave, and the noise
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    for (int i = 0; i < 128; i++) {
        buffer[128 + i] = uint8_t(-127);
        buffer[i] = 127;
    }
    new_wave(0, buffer);
    new_wave(1, buffer);
    uint16_t seed = m.BSeed;
    uint32_t to = m.WaveBase + offsetof(MusWave, WaveDeb);
    for (int i = 0; i < LNoise / 2; i++, to += 2) {
        seed = uint16_t((uint32_t(uint16_t(seed + custom(VHPosR))) * 0x3171) >> 8);
        w16(to) = seed;
    }
    m.BSeed = seed;
    volume(56, 0x0F);
    music_volume(56);
    m.SamBank = 5;
    for (uint32_t v = 0; v < 4; v++) {
        low_byte(m.SamLoops) &= uint8_t(~bit(v));
        sample_int(v).Sami_rpos = uint32_t(-1);
    }
    music_init();
}

// MusEnd, at the end of AMOS
void ext_music_end()
{
    volatile MusicData &m = md();
    talk_stop();
    if (struct IOStdReq *io = write_io()) {
        hal::close_device(reinterpret_cast<struct IORequest *>(io));
        lib_delete_ext_io(reinterpret_cast<struct IORequest *>(io));
        lib_delete_port(ptr<struct MsgPort>(m.WritePort));
        lib_delete_port(ptr<struct MsgPort>(m.ReadPort));
    }
    if (m.TranBase)
        hal::close_library(ptr<struct Library>(m.TranBase));
    track_stop();
    med_close();
    idata().VblRout[0] = 0;
    samples_remove();
    stop_sound();
    envelopes_off(0x0F);
    delete_waves();
    custom(DmaCon) = 0x000F;
    for (uint32_t v = 0; v < 4; v++)
        w16(audio(v) + 8) = 0;
}

// BkCheck, after banks changed: the musics whose bank went stopped
void ext_music_bank_check()
{
    volatile MusicData &m = md();
    // the tracker's
    if (m.mt_on) {
        uint8_t *data = ptr<uint8_t>(m.mt_data);
        if (peek_l(data - 4) != 0x6B657220 || peek_l(data - 8) != 0x54726163)     // "Trac" "ker "
            track_stop();
    }
    // MED's (the original compared the name's second half with its first,
    // and so stopped the music at any change of the banks)
    if (m.Med_On && m.Med_Adr) {
        uint8_t *module = ptr<uint8_t>(m.Med_Adr);
        if (peek_l(module - 4) != 0x20202020 || peek_l(module - 8) != 0x4D656420) {   // "Med " "    "
            med_stop();
            m.Med_Adr = 0;
        }
    }
    // the AMOS music's: bank 3
    uint32_t old = m.MusBank;
    uint8_t *bank = bank_find(3);
    if (!bank || peek_l(bank - 8) != 0x4D757369) {   // "Musi"
        if (old) {
            music_init();
            m.MusBank = 0;
        }
        return;
    }
    uint32_t sum = peek_l(bank) + peek_l(bank + 4) + peek_l(bank + 8) + peek_l(bank + 12);
    if (address(bank) == old && sum == m.MusCheck)
        return;
    m.MusBank = address(bank);
    m.MusCheck = sum;
    music_init();
    m.BankInst = address(bank) + peek_l(bank);
    m.BankSong = address(bank) + peek_l(bank + 4);
    m.BankPat = address(bank) + peek_l(bank + 8);
}

// GoSam with registers (the editor's sounds): d1 voices, a1 sample, d2
// length, d3 frequency
void ext_music_go_sam(uint32_t voices, uint32_t length, uint32_t frequency, uint32_t start)
{
    go_sample(uint16_t(voices), start, length, frequency);
}

// ---- instructions ------------------------------------------------------

// BELL [note]
void ext_bell0() { go_bell(0x0F, 70, 0, 1, address(EnvBell)); }
void ext_bell1(uint32_t note) { go_bell(0x0F, note, 0, 1, address(EnvBell)); }
// BOOM, SHOOT
void ext_boom() { shout(36, EnvBoom); }
void ext_shoot() { shout(60, EnvShoot); }

// PLAY [voices,]note,wait
void ext_play2(uint32_t note, int32_t wait)
{
    if (wait < 0)
        ifonc();
    go_bell(0x0F, note, uint32_t(wait), -1, 0);
}
void ext_play3(uint32_t mask, uint32_t note, int32_t wait)
{
    if (wait < 0)
        ifonc();
    go_bell(uint16_t(mask), note, uint32_t(wait), -1, 0);
}

// PLAY OFF [voices]
void ext_play_off0() { envelopes_off(0x0F); }
void ext_play_off1(uint32_t mask) { envelopes_off(uint16_t(mask)); }

// VOLUME [voices,]level
void ext_volume1(uint32_t level)
{
    volume(level, 0x0F);
    music_volume(level);
}
void ext_volume2(uint32_t mask, uint32_t level) { volume(level, uint16_t(mask)); }

// MVOLUME level
void ext_mvolume(uint32_t level)
{
    if (level >= 64)
        ifonc();
    music_volume(level);
}

// VOICE mask
void ext_voice(uint32_t mask) { voices(uint16_t(mask & 0x0F)); }

// MUSIC n
void ext_music(uint32_t n)
{
    volatile MusicData &m = md();
    if (!n)
        ifonc();
    if (!m.MusBank)
        error(MnRes);
    uint32_t song = m.BankSong;
    if (uint16_t(n) > peek_w(ptr<void>(song)))
        error(MNDef);
    song += peek_l(ptr<void>(song + 2 - 4 + uint16_t(n << 2)));
    if (m.MuNumber >= 3)
        return;
    m.MuBase = 0;
    uint16_t number = m.MuNumber;
    m.MuNumber = uint16_t(number + 1);
    uint32_t t = mb() + offsetof(MusicData, MuBuffer) + uint32_t(number) * MusTable::MuLong;
    volatile MusTable &table = table_at(t);
    for (int i = 0; i < MusVoice::VoiLong * 4; i++)
        table.MuVoices[i] = 0;
    table.MuStop = 0;
    table.MuStart = 0;
    table.MuCpt = m.TempoBase;
    table.MuTempo = 17;
    for (uint32_t v = 0; v < 4; v++) {
        volatile MusVoice &vo = voice_at(t + v * MusVoice::VoiLong);
        vo.VoiCpt = 1;
        vo.VoiAdr = mb() + offsetof(MusicData, FoEnd);
        uint32_t pattern = song + int16_t(peek_w(ptr<void>(song + 2 * v)));
        vo.VoiPat = pattern;
        vo.VoiDPat = pattern;
        vo.VoiEffect = 6;       // NoEffect2 (music_play.cpp)
    }
    // no more samples
    custom(IntEna) = 0b0000011110000000;
    m.MuBase = t;
}

// MUSIC OFF, MUSIC STOP
void ext_music_off()
{
    volatile MusicData &m = md();
    m.MuBase = 0;
    m.MuNumber = 0;
    stop_sound();
}

void ext_music_stop()
{
    volatile MusicData &m = md();
    uint32_t base = m.MuBase;
    if (!base)
        return;
    // (the original cleared the high word only, the time to stop the voices)
    w16(address(&m.MuBase)) = 0;
    for (uint32_t v = 0; v < 4; v++)
        voice_at(base + v * MusVoice::VoiLong).VoiCpt = 0;
    m.MuBase = base;
}

// TEMPO t
void ext_tempo(uint32_t tempo)
{
    if (tempo > 100)
        ifonc();
    if (uint32_t base = md().MuBase)
        table_at(base).MuTempo = uint16_t(tempo);
}

// =VUMETER(voice)
uint32_t ext_fn_vumeter(uint32_t v)
{
    if (v >= 4)
        ifonc();
    uint8_t vu = md().MuVu[v];
    md().MuVu[v] = 0;
    return vu;
}

// =MUBASE
uint32_t ext_fn_mubase() { return mb(); }

// LED ON, LED OFF
void ext_led_on() { cia_pra() &= uint8_t(~2); }
void ext_led_off() { cia_pra() |= 2; }

// ---- waves, envelopes ----------------------------------------------------

// NOISE TO voices, WAVE n TO voices, SAMPLE n TO voices
static void waves_of(uint32_t mask, uint16_t wave)
{
    for (uint32_t v = 0; v < 4; v++)
        if (mask & bit(v))
            md().Waves[v] = wave;
}

void ext_noise_to(uint32_t mask) { waves_of(mask, 0); }

void ext_wave(int32_t n, uint32_t mask)
{
    if (n < 0)
        ifonc();
    if (!find_wave(uint16_t(n)).wave)
        error(WNDef);
    waves_of(mask, uint16_t(n));
}

void ext_sample_to(uint32_t n, uint32_t mask)
{
    sample(n);
    waves_of(mask, uint16_t(-int16_t(n)));
}

// SET WAVE n,a$
void ext_set_wave(uint32_t n, const uint8_t *s)
{
    if (str_len(s) < 256)
        error(STSho);
    if (!n)
        ifonc();
    if (!new_wave(uint16_t(n), str_chars(s)))
        out_of_memory();
}

// DEL WAVE n
void ext_del_wave(int32_t n)
{
    if (n < 0)
        ifonc();
    if (n == 0 || n == 1)
        error(W0Res);
    envelopes_off(0x0F);
    Found f = find_wave(uint16_t(n));
    if (!f.wave)
        error(WNDef);
    delete_wave(f);
    default_waves();
}

// SET ENVEL wave,phase TO duration,volume
void ext_set_envel(int32_t n, int32_t phase, uint32_t duration, uint32_t vol)
{
    if (vol >= 64)
        ifonc();
    if (phase < 0 || phase >= 7)
        ifonc();
    if (n < 0)
        ifonc();
    if (!uint16_t(phase) && !uint16_t(duration))
        ifonc();
    Found f = find_wave(uint16_t(n));
    if (!f.wave)
        error(WNDef);
    volatile MusWave &w = wave_at(f.wave);
    w.WaveEnv[2 * phase] = uint16_t(duration);
    w.WaveEnv[2 * phase + 1] = uint16_t(vol);
    w.WaveEnv[2 * phase + 2] = 0;
}

// ---- samples -------------------------------------------------------------

// SAM BANK n
void ext_sam_bank(uint32_t n)
{
    if (!n || n > 16)
        ifonc();
    md().SamBank = uint16_t(n);
}

// SAM LOOP ON/OFF [voices]
static void sam_loops(uint32_t rpos, uint32_t mask)
{
    volatile MusicData &m = md();
    for (uint32_t v = 0; v < 4; v++) {
        if (!(mask & bit(v)))
            continue;
        low_byte(m.SamLoops) &= uint8_t(~bit(v));
        sample_int(v).Sami_rpos = rpos;
        if (!rpos)
            low_byte(m.SamLoops) |= uint8_t(bit(v));
    }
}
void ext_sam_loop_on0() { sam_loops(0, 0x0F); }
void ext_sam_loop_on1(uint32_t mask) { sam_loops(0, mask); }
void ext_sam_loop_off0() { sam_loops(uint32_t(-1), 0x0F); }
void ext_sam_loop_off1(uint32_t mask) { sam_loops(uint32_t(-1), mask); }

// SAM PLAY [voices,]n[,frequency]
void ext_sam_play1(uint32_t n)
{
    Sample s = sample(n);
    go_sample(0x0F, s.start, s.length, s.frequency);
}
void ext_sam_play2(uint32_t mask, uint32_t n)
{
    Sample s = sample(n);
    go_sample(uint16_t(mask), s.start, s.length, s.frequency);
}
void ext_sam_play3(uint32_t mask, uint32_t n, int32_t frequency)
{
    Sample s = sample(n);
    if (frequency <= 500)
        ifonc();
    go_sample(uint16_t(mask), s.start, s.length, uint32_t(frequency));
}

// SAM RAW voices,address,length,frequency
void ext_sam_raw(uint32_t mask, uint32_t start, int32_t length, int32_t frequency)
{
    if (frequency <= 500)
        ifonc();
    if (length <= 256)
        ifonc();
    go_sample(uint16_t(mask), start, uint32_t(length), uint32_t(frequency));
}

// =SAM SWAPPED(voice)
int32_t ext_fn_sam_swapped(uint32_t v)
{
    if (v > 3)
        ifonc();
    if (!((custom(IntEnaR) >> 7) & bit(v)))
        return 1;
    volatile SampleInt &s = sample_int(v);
    if (s.Sami_radr || s.Sami_pos == Sami_lplay)
        return 0;
    return -1;
}

// SAM SWAP voices TO address,length
void ext_sam_swap(uint32_t mask, uint32_t at, int32_t length)
{
    if (length < 0)
        ifonc();
    uint32_t start = bank_or_address(int32_t(at));
    for (uint32_t v = 0; v < 4; v++) {
        if (!(mask & bit(v)))
            continue;
        volatile SampleInt &s = sample_int(v);
        s.Sami_radr = start;
        s.Sami_rlong = uint32_t(length);
    }
}

// SAM STOP [voices]
void ext_sam_stop0() { samples_stop(0x0F); }
void ext_sam_stop1(uint32_t mask) { samples_stop(mask); }

// SLOAD file TO address,length
void ext_sload(uint32_t file, uint32_t at, int32_t length)
{
    if (length < 0)
        ifonc();
    uint32_t to = bank_or_address(int32_t(at));
    BPTR handle = program_file(file);
    if (hal::read(handle, ptr<void>(to), length) < 0)
        music_disk_error();
}

// SSAVE file,start TO end
void ext_ssave(uint32_t file, uint32_t start, uint32_t end)
{
    int32_t length = int32_t(end - start);
    if (length <= 0)
        ifonc();
    BPTR handle = program_file(file);
    if (hal::write(handle, ptr<void>(start), length) < 0)
        music_disk_error();
}

// ---- the narrator ----------------------------------------------------------

void ext_say1(const uint8_t *s)
{
    narrator_open();
    say(s, 0);
}
void ext_say2(const uint8_t *s, uint32_t multi)
{
    narrator_open();
    say(s, multi);
}

// SET TALK sex,mode,pitch,rate
void ext_set_talk(uint32_t sex, uint32_t mode, uint32_t pitch, uint32_t rate)
{
    narrator_open();
    uint8_t *b = reinterpret_cast<uint8_t *>(write_io());
    if (rate != EntNul) {
        if (uint16_t(rate) < 40 || uint16_t(rate) > 400)
            ifonc();
        poke_w(b + 48, uint16_t(rate));
    }
    if (pitch != EntNul) {
        if (uint16_t(pitch) < 65 || uint16_t(pitch) > 320)
            ifonc();
        poke_w(b + 50, uint16_t(pitch));
    }
    if (mode != EntNul)
        poke_w(b + 52, mode & 1);
    if (sex != EntNul)
        poke_w(b + 54, sex & 1);
}

// TALK MISC volume,frequency
void ext_talk_misc(uint32_t vol, uint32_t frequency)
{
    narrator_open();
    uint8_t *b = reinterpret_cast<uint8_t *>(write_io());
    if (frequency != EntNul) {
        if (uint16_t(frequency) < 5000 || uint16_t(frequency) > 25000)
            ifonc();
        poke_w(b + 64, uint16_t(frequency));
    }
    if (vol != EntNul) {
        if (int32_t(vol) < 0 || uint16_t(vol) > 64)
            ifonc();
        poke_w(b + 62, uint16_t(vol));
    }
}

void ext_talk_stop() { talk_stop(); }

// MOUTH READ, =MOUTH WIDTH, =MOUTH HEIGHT
void ext_mouth_read()
{
    volatile MusicData &m = md();
    uint8_t *r = read_io();
    struct IOStdReq *w = write_io();
    if (m.TranBase && w && reinterpret_cast<uint8_t *>(w)[66]) {
        poke_w(r + 28, CMD_READ);
        hal::do_io(reinterpret_cast<struct IORequest *>(r));
        if (int8_t(r[0x1F]) != -8)      // ND_NoWrite
            return;
    }
    poke_w(r + 88, 0xFFFF);
}
int32_t ext_fn_mouth_width() { return int8_t(read_io()[88]); }
int32_t ext_fn_mouth_height() { return int8_t(read_io()[89]); }

// ---- the tracker -----------------------------------------------------------

// TRACK LOAD "name",bank
void ext_track_load(const uint8_t *name, int32_t bank)
{
    volatile MusicData &m = md();
    if (bank >= 0x10000)
        ifonc();
    if (uint16_t(bank) == m.Track_Bank && m.mt_on)
        track_stop();
    m.Track_Bank = uint16_t(bank);
    load_bank(name, uint32_t(bank), BankData | BankChip, 32, "Tracker ");
}

void ext_track_stop() { track_stop(); }
void ext_track_loop_on() { md().Track_Loop = 1; }
void ext_track_loop_off() { md().Track_Loop = 0; }

// TRACK PLAY [bank[,pattern]] (the pattern is not used)
void ext_track_play2(uint32_t bank, uint32_t)
{
    volatile MusicData &m = md();
    if (bank == EntNul)
        bank = m.Track_Bank;
    uint8_t *data = ptr<uint8_t>(bank_or_address(int32_t(bank)));
    if (peek_l(data - 8) != 0x54726163 || peek_l(data - 4) != 0x6B657220)
        error(NotTracker);
    samples_stop(0x0F);
    track_stop();
    m.mt_data = address(data);
    for (uint32_t i = 0; i < 32; i++)
        m.mt_mulu[i] = address(data) + 0x0C + 0x1E * i;
    // the highest pattern, then the samples after the patterns
    int8_t highest = 0;
    for (int i = 0; i < 128; i++)
        if (int8_t(data[0x3B8 + i]) > highest)
            highest = int8_t(data[0x3B8 + i]);
    uint8_t *sample = data + 0x3B8 + 128 + 4 + (uint32_t(int32_t(highest) + 1) << 10);
    uint8_t *header = data + 0x2A;
    for (int i = 0; i < 31; i++, header += 0x1E) {
        poke_l(sample, 0);
        m.mt_samplestarts[i] = address(sample);
        header[2] = 0;
        sample += uint32_t(peek_w(header)) << 1;
    }
    m.mt_speed = 6;
    for (uint32_t v = 0; v < 4; v++)
        w16(audio(v) + 8) = 0;
    m.mt_songpos = 0;
    m.mt_counter = 0;
    m.mt_pattpos = 0;
    m.mt_on = 1;
}
void ext_track_play1(uint32_t bank) { ext_track_play2(bank, EntNul); }
void ext_track_play0() { ext_track_play2(EntNul, EntNul); }

// ---- MED -----------------------------------------------------------------

// MED LOAD "name",bank
void ext_med_load(const uint8_t *name, int32_t bank)
{
    volatile MusicData &m = md();
    med_open();
    if (bank >= 0x10000)
        ifonc();
    if (uint16_t(bank) == m.Med_Bank)
        med_stop();
    m.Med_Bank = uint16_t(bank);
    uint8_t *module = load_bank(name, uint32_t(int16_t(m.Med_Bank)), BankChip, 8, "Med     ");
    uint32_t id = peek_l(module);
    if (id == 0x4D4D4430 || id == 0x4D4D4431) {     // MMD0, MMD1
        lvo(m.Med_Base, -102, 0, 0, address(module));
        return;
    }
    bank_erase(uint32_t(int16_t(m.Med_Bank)));
    error(NotMed);
}

void ext_med_stop() { med_stop(); }

// MED PLAY [bank[,song]]
void ext_med_play2(uint32_t bank, uint32_t song)
{
    volatile MusicData &m = md();
    med_open();
    med_stop();
    m.Med_Adr = 0;
    if (bank == EntNul)
        bank = m.Med_Bank;
    uint8_t *module = ptr<uint8_t>(bank_or_address(int32_t(bank)));
    if (peek_l(module - 8) != 0x4D656420 || peek_l(module - 4) != 0x20202020)
        error(NotMedBank);
    samples_stop(0x0F);
    track_stop();
    med_stop();
    lvo(m.Med_Base, -96, song == EntNul ? 0 : song);
    lvo(m.Med_Base, -42, 0, 0, address(module));
    m.Med_Adr = address(module);
    m.Med_On = 1;
}
void ext_med_play1(uint32_t bank) { ext_med_play2(bank, EntNul); }
void ext_med_play0() { ext_med_play2(EntNul, EntNul); }

void ext_med_cont()
{
    volatile MusicData &m = md();
    if (!m.Med_Adr || m.Med_On)
        return;
    m.Med_On = 1;
    lvo(m.Med_Base, -48, 0, 0, m.Med_Adr);
}

void ext_med_midi_on() { md().Med_Midi = 1; }
}
