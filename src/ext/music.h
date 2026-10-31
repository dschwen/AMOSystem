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
template <class T> native_t<T> decrement(volatile T &x)
{
    native_t<T> v = native_t<T>(x - 1);
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
// Words and longs of memory or of the chips (the voices' registers are
// either: those of an audio channel, or the dummies of a voice taken from
// the music)
#if defined(__mc68000__)
inline volatile be_u16 &w16(uint32_t at) { return *ptr<volatile be_u16>(at); }
inline volatile be_u32 &w32(uint32_t at) { return *ptr<volatile be_u32>(at); }
inline volatile uint8_t &w8(uint32_t at) { return *ptr<volatile uint8_t>(at); }
#else
// (another host: the chips' registers are emulated, not memory)
inline bool is_chip(uint32_t at) { return at - CustomAddress < 0x200; }
struct W16 {
    uint32_t at;
    operator uint16_t() const { return is_chip(at) ? uint16_t(hal::custom_word(at - CustomAddress)) : *ptr<volatile be_u16>(at); }
    void operator=(uint16_t v) const
    {
        if (is_chip(at))
            hal::custom_word(at - CustomAddress) = v;
        else
            *ptr<volatile be_u16>(at) = v;
    }
    void operator+=(int v) const { *this = uint16_t(*this + v); }
    void operator-=(int v) const { *this = uint16_t(*this - v); }
    void operator|=(int v) const { *this = uint16_t(*this | v); }
    void operator&=(int v) const { *this = uint16_t(*this & v); }
};
struct W32 {
    uint32_t at;
    operator uint32_t() const { return uint32_t(W16{at}) << 16 | W16{at + 2}; }
    void operator=(uint32_t v) const
    {
        W16{at} = uint16_t(v >> 16);
        W16{at + 2} = uint16_t(v);
    }
    void operator+=(uint32_t v) const { *this = uint32_t(*this + v); }
    void operator-=(uint32_t v) const { *this = uint32_t(*this - v); }
};
struct W8 {
    uint32_t at;
    operator uint8_t() const { return uint8_t(is_chip(at) ? (at & 1 ? W16{at & ~1u} & 0xFF : W16{at} >> 8) : *ptr<volatile uint8_t>(at)); }
    void operator=(uint8_t v) const
    {
        if (!is_chip(at)) {
            *ptr<volatile uint8_t>(at) = v;
            return;
        }
        uint16_t w = W16{at & ~1u};
        W16{at & ~1u} = at & 1 ? uint16_t((w & 0xFF00) | v) : uint16_t((w & 0xFF) | v << 8);
    }
};
inline W16 w16(uint32_t at) { return {at}; }
inline W32 w32(uint32_t at) { return {at}; }
inline W8 w8(uint32_t at) { return {at}; }
#endif
inline uint8_t peek_b(const void *p) { return *static_cast<const uint8_t *>(p); }
inline volatile hal::ChipWord &custom(uint32_t r) { return hal::custom_word(r); }
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
