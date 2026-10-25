// The Music extension (extensions/+Music.s, slot 1): the music player, the
// sound effects and their waves and envelopes, the samples, the tracker
// and MED players, the narrator. Its data zone is music_data
// (music_glue.s, laid out by extensions/+Music_Data.s), shared with the
// interrupts: everything is accessed through volatile references, in the
// order of the original.
#pragma once

#include "core/amos.h"
#include "hal/chips.h"

extern "C" amos::MusicData music_data;

namespace amos::music {

constexpr uint32_t ExtNb = 1 - 1;
constexpr uint16_t LNoise = 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2;   // a wave, the noise

// x - 1 stored, and returned
template <class T> T decrement(volatile T &x)
{
    T v = T(x - 1);
    x = v;
    return v;
}

inline volatile MusicData &md() { return music_data; }
inline uint32_t mb() { return address(&music_data); }

inline volatile MusEnvelope &envelope(uint32_t v)
{
    return *reinterpret_cast<volatile MusEnvelope *>(md().EnvBase + v * MusEnvelope::EnvLong);
}
inline volatile SampleInt &sample_int(uint32_t v)
{
    return *reinterpret_cast<volatile SampleInt *>(md().Sami_int + v * SampleInt::Sami_intl);
}
inline volatile MusVoice &voice_at(uint32_t at) { return *ptr<volatile MusVoice>(at); }
inline volatile MusTable &table_at(uint32_t at) { return *ptr<volatile MusTable>(at); }
inline volatile MusWave &wave_at(uint32_t at) { return *ptr<volatile MusWave>(at); }

// ---- the hardware ----------------------------------------------------------

constexpr uint32_t CustomAddress = hal::CustomBase;
constexpr uint32_t DmaCon = 0x96, IntEna = 0x9A, IntReq = 0x9C, IntEnaR = 0x1C, VHPosR = 0x06;
constexpr uint32_t Audio = 0xA0;    // the registers of voice 0, 16 bytes a voice
inline volatile uint16_t &w16(uint32_t at) { return *ptr<volatile uint16_t>(at); }
inline volatile uint32_t &w32(uint32_t at) { return *ptr<volatile uint32_t>(at); }
inline uint8_t peek_b(const void *p) { return *static_cast<const uint8_t *>(p); }
inline volatile uint8_t &w8(uint32_t at) { return *ptr<volatile uint8_t>(at); }
inline volatile uint16_t &custom(uint32_t r) { return hal::custom_word(r); }
inline volatile uint8_t &cia_pra() { return hal::cia_a_pra(); }
// An audio channel's (or the dummy registers of a voice taken from the
// music): +0 address, +4 length, +6 period, +8 volume, +10 data
inline uint32_t audio(uint32_t v) { return CustomAddress + Audio + 16 * v; }

// ---- music_play.cpp: the interrupt side -----------------------------------

void env_next(uint32_t v, volatile MusEnvelope &e, uint16_t &on, uint16_t &ended);  // MuIntE
void stop_sound();          // MOff
void dma_wait();            // DmaWait
void sami_stop();           // Sami_stop

} // namespace amos::music

extern "C" {
void ext_music_vbl();                               // MusInt (VblRout)
void ext_music_sami(amos::SampleInt *s);            // Sami_handler (music_glue.s)
}
