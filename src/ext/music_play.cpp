// The interrupt side of the Music extension: at each VBL the envelopes,
// the noise, then the AMOS music (or, without one, the tracker module);
// the audio interrupts of the samples. They run with the data zone only
// (no context, no a5).

#include "ext/music.h"

using namespace amos;
using namespace amos::music;

namespace {

// The effect of a voice (VoiEffect)
enum Effect : uint32_t { NoEffect, MuSlide, MuArp, MuPTone, MuVib, MuVSl, NoEffect2 };

// Sinus, the periods (and what followed them, which the arpeggio could
// read past them), as in the extension
const uint8_t Sinus[32] = {
    0x00, 0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1, 0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
    0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5, 0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18,
};
const uint16_t Periods[38 + 32] = {
    0x0358, 0x0328, 0x02fa, 0x02d0, 0x02a6, 0x0280, 0x025c, 0x023a, 0x021a, 0x01fc, 0x01e0,
    0x01c5, 0x01ac, 0x0194, 0x017d, 0x0168, 0x0153, 0x0140, 0x012e, 0x011d, 0x010d, 0x00fe,
    0x00f0, 0x00e2, 0x00d6, 0x00ca, 0x00be, 0x00b4, 0x00aa, 0x00a0, 0x0097, 0x008f, 0x0087,
    0x007f, 0x0078, 0x0071, 0x0000, 0x0000,
    // EnvDef, EnvShoot, EnvBoom, EnvBell
    1, 64, 4, 55, 5, 50, 25, 0, 0, 0, 1, 64, 10, 0, 0, 0, 1, 64, 10, 50, 50, 0, 0, 0, 1, 64, 4, 40, 25, 0, 0, 0,
};

// The tracker's
const uint16_t TrackerPeriods[37 + 15] = {
    0x0358, 0x0328, 0x02fa, 0x02d0, 0x02a6, 0x0280, 0x025c, 0x023a, 0x021a, 0x01fc, 0x01e0,
    0x01c5, 0x01ac, 0x0194, 0x017d, 0x0168, 0x0153, 0x0140, 0x012e, 0x011d, 0x010d, 0x00fe,
    0x00f0, 0x00e2, 0x00d6, 0x00ca, 0x00be, 0x00b4, 0x00aa, 0x00a0, 0x0097, 0x008f, 0x0087,
    0x007f, 0x0078, 0x0071, 0x0000,
    // (the arpeggio of a voice without a period read on, into the
    // player's variables: zeros here)
};
const uint8_t TrackerSinus[32] = {
    0x00, 0x18, 0x31, 0x4a, 0x61, 0x78, 0x8d, 0xa1, 0xb4, 0xc5, 0xd4, 0xe0, 0xeb, 0xf4, 0xfa, 0xfd,
    0xff, 0xfd, 0xfa, 0xf4, 0xeb, 0xe0, 0xd4, 0xc5, 0xb4, 0xa1, 0x8d, 0x78, 0x61, 0x4a, 0x31, 0x18,
};
const uint8_t ArpList[32] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1};

volatile MusVoice &voice(volatile MusTable &t, uint32_t v)
{
    return *reinterpret_cast<volatile MusVoice *>(t.MuVoices + v * MusVoice::VoiLong);
}
volatile uint8_t &count_low(volatile MusVoice &vo) { return reinterpret_cast<volatile uint8_t *>(&vo.VoiCpt)[1]; }
volatile uint8_t &low_byte(volatile uint16_t &w) { return reinterpret_cast<volatile uint8_t *>(&w)[1]; }
volatile uint8_t &high_byte(volatile uint16_t &w) { return reinterpret_cast<volatile uint8_t *>(&w)[0]; }

// The voice's volume from its own and the music's (mulu, lsr.w)
uint16_t scaled(uint16_t volume) { return uint16_t(uint16_t(volume * md().MuVolume) >> 6); }

// ---- the AMOS music -------------------------------------------------------

void no_more(volatile MusVoice &vo) { vo.VoiEffect = NoEffect; }

void effect(volatile MusVoice &vo, uint32_t chip)
{
    switch (vo.VoiEffect) {
    case MuSlide: {
        int16_t value = int16_t(vo.VoiValue);
        if (!value) {
            no_more(vo);
            return;
        }
        uint16_t p = uint16_t(value + vo.VoiNote);
        if (p < 0x71) {
            p = 0x71;
            no_more(vo);
        }
        if (p > 0x358) {
            p = 0x358;
            no_more(vo);
        }
        vo.VoiNote = p;
        w16(chip + 6) = p;
        return;
    }
    case MuArp: {
        uint16_t d0 = low_byte(vo.VoiValue);
        uint8_t d1 = high_byte(vo.VoiValue);
        if (d1 >= 3)
            d1 = 2;
        d1--;
        high_byte(vo.VoiValue) = d1;
        if (!d1) {
            w16(chip + 6) = vo.VoiNote;
            return;
        }
        d0 = int8_t(d1) > 0 ? d0 & 0x0F : uint8_t(d0) >> 4;
        int16_t note = int16_t(vo.VoiNote);
        for (int i = 0; i <= 0x24; i++) {
            uint16_t p = Periods[i + d0];
            if (note >= int16_t(Periods[i])) {
                w16(chip + 6) = p;
                return;
            }
        }
        return;
    }
    case MuPTone: {
        uint16_t step = vo.VoiValue, p = vo.VoiNote, to = vo.VoiPToTo;
        if (p == to) {
            no_more(vo);
        } else if (p < to) {
            p = uint16_t(p + step);
            if (p >= to) {
                p = to;
                no_more(vo);
            }
        } else {
            p = uint16_t(p - step);
            if (p <= to) {
                p = to;
                no_more(vo);
            }
        }
        vo.VoiNote = p;
        w16(chip + 6) = p;
        return;
    }
    case MuVib: {
        uint8_t vib = vo.VoiVib;
        uint16_t d2 = Sinus[(vib >> 2) & 0x1f];
        uint8_t value = low_byte(vo.VoiValue);
        d2 = uint16_t(uint16_t(d2 * (value & 0x0F)) >> 6);
        uint16_t p = vo.VoiNote;
        p = int8_t(vib) < 0 ? uint16_t(p - d2) : uint16_t(p + d2);
        w16(chip + 6) = p;
        vo.VoiVib = uint8_t(vib + ((value >> 2) & 0x3C));
        return;
    }
    case MuVSl: {
        int16_t v = int16_t(vo.VoiDVol + vo.VoiValue);
        if (v < 0)
            v = 0;
        if (uint16_t(v) >= 0x40)
            v = 0x3F;
        vo.VoiDVol = uint16_t(v);
        vo.VoiVol = scaled(uint16_t(v));
        return;
    }
    default:        // NoEffect, NoEffect2
        w16(chip + 6) = vo.VoiNote;
        return;
    }
}

// DoEffects
void effects(volatile MusicData &m, volatile MusTable &t)
{
    for (uint32_t v = 0; v < 4; v++) {
        volatile MusVoice &vo = voice(t, v);
        uint32_t chip = m.MuChip[v];
        effect(vo, chip);
        w16(chip + 8) = vo.VoiVol;
    }
}

// A note: the instrument from its start
void note_start(volatile MusicData &m, volatile MusTable &t, volatile MusVoice &vo, uint32_t v, uint32_t chip, uint16_t &started)
{
    uint32_t inst = vo.VoiInst;
    w32(chip) = m.BankInst + peek_l(ptr<void>(inst));
    w16(chip + 4) = peek_w(ptr<void>(inst + 8));
    started |= uint16_t(1 << v);
    low_byte(t.MuStop) &= uint8_t(~(1 << v));
    low_byte(t.MuStart) |= uint8_t(1 << v);
    m.MuVu[v] = low_byte(vo.VoiVol);
}

// MuStep: the commands of a voice up to its next delay
void step(volatile MusicData &m, volatile MusTable &t, volatile MusVoice &vo, uint32_t v, uint32_t chip, uint16_t &started)
{
    uint32_t a2 = vo.VoiAdr;
    for (;;) {
        uint16_t d0 = peek_w(ptr<void>(a2));
        a2 += 2;
        if (!(d0 & 0x8000)) {
            if (d0 & 0x4000) {
                // a note of the first version: its delay first
                vo.VoiCpt = d0;
                d0 = peek_w(ptr<void>(a2));
                a2 += 2;
                if (d0) {
                    d0 &= 0x0FFF;
                    vo.VoiNote = d0;
                    w16(chip + 6) = d0;
                    note_start(m, t, vo, v, chip, started);
                }
                vo.VoiAdr = a2;
                return;
            }
            d0 &= 0x0FFF;
            note_start(m, t, vo, v, chip, started);
            if (vo.VoiPTone) {
                vo.VoiPTone = 0;
                vo.VoiPToTo = d0;
                vo.VoiEffect = MuPTone;
            } else {
                vo.VoiNote = d0;
                w16(chip + 6) = d0;
            }
            continue;
        }
        uint16_t d1 = d0;
        switch ((d0 & 0x7F00) >> 8) {
        case 17:    // jump to a position of the song
            vo.VoiPat = vo.VoiDPat + int16_t(uint16_t((d1 & 0xFF) << 1));
            [[fallthrough]];
        case 0: {   // end of the pattern: the next one of the song
            vo.VoiCpt = 0;
            vo.VoiRep = 0;
            vo.VoiDeb = 0;
            vo.VoiEffect = NoEffect;
            uint32_t a0 = vo.VoiPat;
            for (;;) {
                uint16_t p = peek_w(ptr<void>(a0));
                a0 += 2;
                if (int16_t(p) < 0) {
                    if (p == 0xFFFF)
                        return;
                    a0 = vo.VoiDPat;
                    continue;
                }
                vo.VoiPat = a0;
                uint32_t patterns = m.BankPat;
                if (p > peek_w(ptr<void>(patterns)))
                    return;
                uint16_t offset = peek_w(ptr<void>(patterns + 2 + int16_t(uint16_t((uint16_t((p << 2) + v)) << 1))));
                if (!offset)
                    return;
                a2 = patterns + offset;
                break;
            }
            continue;
        }
        case 3: {   // volume
            uint16_t vol = d1 & 0xFF;
            if (vol >= 64)
                vol = 63;
            vo.VoiDVol = vol;
            vo.VoiVol = scaled(vol);
            continue;
        }
        case 4:     // no effect
            vo.VoiEffect = NoEffect;
            continue;
        case 5: {   // repeat
            uint16_t n = d1 & 0xFF;
            if (!n) {
                vo.VoiDeb = a2;
            } else if (!vo.VoiRep) {
                vo.VoiRep = n;
            } else if (decrement(vo.VoiRep) && vo.VoiDeb) {
                a2 = vo.VoiDeb;
            }
            continue;
        }
        case 6:     // led on
            cia_pra() &= uint8_t(~2);
            continue;
        case 7:     // led off
            cia_pra() |= 2;
            continue;
        case 8:     // tempo
            t.MuTempo = d1 & 0xFF;
            continue;
        case 9: {   // instrument
            uint32_t inst = m.BankInst + 2 + uint16_t((d1 & 0xFF) << 5);
            vo.VoiInst = inst;
            uint16_t vol = peek_w(ptr<void>(inst + 12));
            if (vol >= 64)
                vol = 63;
            vo.VoiDVol = vol;
            vo.VoiVol = scaled(vol);
            continue;
        }
        case 10:    // arpeggio
            low_byte(vo.VoiValue) = uint8_t(d1);
            vo.VoiEffect = MuArp;
            continue;
        case 11:    // portamento
            vo.VoiPTone = 1;
            vo.VoiValue = d1 & 0xFF;
            vo.VoiEffect = MuPTone;
            continue;
        case 12:    // vibrato
            vo.VoiValue = d1 & 0xFF;
            vo.VoiEffect = MuVib;
            continue;
        case 13: {  // volume slide
            uint16_t x = d1 & 0xFF;
            uint16_t up = x >> 4;
            vo.VoiValue = up ? up : uint16_t(-(x & 0x0F));
            vo.VoiEffect = MuVSl;
            continue;
        }
        case 14:    // slide up
            vo.VoiValue = uint16_t(-(d1 & 0xFF));
            vo.VoiEffect = MuSlide;
            continue;
        case 15:    // slide down
            vo.VoiValue = d1 & 0xFF;
            vo.VoiEffect = MuSlide;
            continue;
        case 16:    // delay
            vo.VoiCpt = d1;
            vo.VoiAdr = a2;
            return;
        default:    // 1, 2 (slides of the first version), free
            continue;
        }
    }
}

// MuEvery: the second part of the instruments started at the last VBL,
// the voices started; the voices given back
void every(volatile MusicData &m, volatile MusTable &t)
{
    uint16_t stop = t.MuStop;
    if (stop) {
        uint32_t inst_base = m.BankInst;
        for (uint32_t v = 0; v < 4; v++) {
            if (!(stop & (1 << v)))
                continue;
            uint32_t chip = m.MuChip[v];
            uint32_t inst = voice(t, v).VoiInst;
            w32(chip) = inst_base + peek_l(ptr<void>(inst + 4));
            w16(chip + 4) = peek_w(ptr<void>(inst + 10));
        }
    }
    uint16_t start = t.MuStart;
    t.MuStop = start;
    t.MuStart = 0;
    w16(CustomAddress + DmaCon) = uint16_t((((stop | start) & m.MuDMAsk)) | 0x8000);
    uint16_t back = m.MuReStart;
    if (!back)
        return;
    uint16_t playing = 0;
    for (uint32_t v = 0; v < 4; v++) {
        if (!(back & (1 << v)))
            continue;
        uint32_t chip = audio(v);
        m.MuChip[v] = chip;
        w16(chip + 4) = 2;
        if (voice(t, v).VoiInst)
            playing |= uint16_t(1 << v);
    }
    m.MuReStart = 0;
    m.MuDMAsk |= back;
    t.MuStop |= playing;
}

// ---- the tracker ---------------------------------------------------------

volatile uint8_t *track_voice(volatile MusicData &m, uint32_t v) { return reinterpret_cast<volatile uint8_t *>(m.mt_voice) + 28 * v; }
volatile uint16_t &tw(volatile uint8_t *vc, int at) { return *reinterpret_cast<volatile uint16_t *>(vc + at); }
volatile uint32_t &tl(volatile uint8_t *vc, int at) { return *reinterpret_cast<volatile uint32_t *>(vc + at); }

void lines(int n)
{
    // n raster lines
    for (int i = 0; i < n; i++) {
        uint8_t line = w8(CustomAddress + VHPosR);
        while (w8(CustomAddress + VHPosR) == line)
            ;
    }
}

void track_normal_period(volatile uint8_t *vc, uint32_t chip) { w16(chip + 6) = tw(vc, 0x10); }

void track_vib2(volatile uint8_t *vc, uint32_t chip)
{
    uint16_t d2 = TrackerSinus[(vc[0x19] >> 2) & 0x1f];
    d2 = uint16_t(uint16_t(d2 * (vc[0x18] & 0x0F)) >> 7);
    uint16_t p = tw(vc, 0x10);
    p = int8_t(vc[0x19]) < 0 ? uint16_t(p - d2) : uint16_t(p + d2);
    w16(chip + 6) = p;
    vc[0x19] = uint8_t(vc[0x19] + ((vc[0x18] >> 2) & 0x3C));
}

void track_port2(volatile uint8_t *vc, uint32_t chip)
{
    if (!tw(vc, 0x16))
        return;
    uint16_t step = vc[0x15];
    if (!vc[0x14]) {
        tw(vc, 0x10) = uint16_t(tw(vc, 0x10) + step);
        if (int16_t(tw(vc, 0x16)) > int16_t(tw(vc, 0x10))) {
            w16(chip + 6) = tw(vc, 0x10);
            return;
        }
    } else {
        tw(vc, 0x10) = uint16_t(tw(vc, 0x10) - step);
        if (int16_t(tw(vc, 0x16)) < int16_t(tw(vc, 0x10))) {
            w16(chip + 6) = tw(vc, 0x10);
            return;
        }
    }
    tw(vc, 0x10) = tw(vc, 0x16);
    tw(vc, 0x16) = 0;
    w16(chip + 6) = tw(vc, 0x10);
}

void track_volslide(volatile uint8_t *vc, uint32_t chip)
{
    uint8_t up = vc[3] >> 4;
    if (up) {
        vc[0x13] = uint8_t(vc[0x13] + up);
        if (int8_t(vc[0x13] - 0x40) >= 0)
            vc[0x13] = 0x40;
    } else {
        vc[0x13] = uint8_t(vc[0x13] - (vc[3] & 0x0F));
        if (int8_t(vc[0x13]) < 0)
            vc[0x13] = 0;
    }
    w16(chip + 8) = tw(vc, 0x12);
}

// mt_com: the effects between the rows
void track_effects(volatile MusicData &m, volatile uint8_t *vc, uint32_t chip)
{
    if (!(tw(vc, 2) & 0x0FFF)) {
        track_normal_period(vc, chip);
        return;
    }
    switch (vc[2] & 0x0F) {
    case 0: {   // arpeggio
        uint8_t a = ArpList[m.mt_counter & 31];
        if (!a) {
            track_normal_period(vc, chip);
            return;
        }
        uint16_t n = a == 2 ? vc[3] & 0x0F : vc[3] >> 4;
        int16_t p = int16_t(tw(vc, 0x10));
        int i = 0;
        while (p < int16_t(TrackerPeriods[i]))
            i++;
        w16(chip + 6) = TrackerPeriods[i + n];
        return;
    }
    case 6:
        track_vib2(vc, chip);
        track_volslide(vc, chip);
        return;
    case 4:
        if (vc[3])
            vc[0x18] = vc[3];
        track_vib2(vc, chip);
        return;
    case 5:
        track_port2(vc, chip);
        track_volslide(vc, chip);
        return;
    case 3:
        if (uint8_t step = vc[3]) {
            vc[0x15] = step;
            vc[3] = 0;
        }
        track_port2(vc, chip);
        return;
    case 1: {
        tw(vc, 0x10) = uint16_t(tw(vc, 0x10) - vc[3]);
        if (int16_t(tw(vc, 0x10) - 0x71) < 0)
            tw(vc, 0x10) = 0x71;
        w16(chip + 6) = tw(vc, 0x10);
        return;
    }
    case 2: {
        tw(vc, 0x10) = uint16_t(tw(vc, 0x10) + vc[3]);
        if (int16_t(tw(vc, 0x10) - 0x358) >= 0)
            tw(vc, 0x10) = 0x358;
        w16(chip + 6) = tw(vc, 0x10);
        return;
    }
    case 0x0A:
        w16(chip + 6) = tw(vc, 0x10);
        track_volslide(vc, chip);
        return;
    default:
        w16(chip + 6) = tw(vc, 0x10);
        return;
    }
}

// mt_com2: the effects of a new row
void track_row_effects(volatile MusicData &m, volatile uint8_t *vc, uint32_t chip)
{
    switch (vc[2] & 0x0F) {
    case 0x0E:      // filter
        cia_pra() = uint8_t((cia_pra() & 0xFD) | ((vc[3] & 1) << 1));
        return;
    case 0x0D:      // pattern break
        m.mt_break = 1;
        return;
    case 0x0B:      // position jump
        w16(CustomAddress + 0x180) = 0xFFF;
        m.mt_break = 1;
        m.mt_songpos = uint8_t(vc[3] - 1);
        return;
    case 0x0C: {    // volume
        if (vc[3] > 0x40)
            vc[3] = 0x40;
        uint8_t v = vc[3];
        vc[0x13] = v;
        w16(chip + 8) = v;
        return;
    }
    case 0x0F: {    // speed
        uint8_t s = vc[3];
        if (s > 0x1F)
            s = 0x1F;
        if (!s)
            s = 1;
        m.mt_speed = s;
        return;
    }
    default:
        return;
    }
}

// mt_playvoice: the row of a voice
void track_play_voice(volatile MusicData &m, uint32_t row, uint32_t at, volatile uint8_t *vc, uint32_t chip, uint32_t v)
{
    tl(vc, 0) = peek_l(ptr<void>(row + at));
    uint8_t instrument = uint8_t((vc[2] >> 4) | (vc[0] & 0xF0));
    if (instrument) {
        uint32_t i4 = uint32_t(instrument) << 2;
        tl(vc, 4) = peek_l(ptr<void>(address(m.mt_samplestarts) - 4 + i4));
        uint32_t header = m.mt_mulu[instrument & 31];
        tw(vc, 8) = peek_w(ptr<void>(header));
        tw(vc, 0x12) = peek_w(ptr<void>(header + 2));
        uint32_t start = tl(vc, 4);
        uint16_t repeat = peek_w(ptr<void>(header + 4));
        if (repeat) {
            start += uint16_t(repeat << 1);
            tl(vc, 0x0A) = start;
            tw(vc, 8) = uint16_t(repeat + peek_w(ptr<void>(header + 6)));
        } else {
            tl(vc, 0x0A) = start;
        }
        tw(vc, 0x0E) = peek_w(ptr<void>(header + 6));
        w16(chip + 8) = tw(vc, 0x12);
        m.MuVu[v] = vc[0x13];
    }
    uint16_t period = tw(vc, 0) & 0x0FFF;
    if (period) {
        if (!tw(vc, 8)) {
            w16(CustomAddress + DmaCon) = tw(vc, 0x1A);
        } else {
            uint8_t effect = vc[2] & 0x0F;
            if (effect == 5 || effect == 3) {
                // the portamento's goal
                uint16_t goal = tw(vc, 0) & 0x0FFF;
                tw(vc, 0x16) = goal;
                uint16_t now = tw(vc, 0x10);
                vc[0x14] = 0;
                if (goal == now) {
                    tw(vc, 0x16) = 0;
                    return;
                }
                if (int16_t(goal) < int16_t(now))
                    vc[0x14] = 1;
            } else {
                tw(vc, 0x10) = period;
                w16(CustomAddress + DmaCon) = tw(vc, 0x1A);
                vc[0x19] = 0;
                w32(chip) = tl(vc, 4);
                w16(chip + 4) = tw(vc, 8);
                w16(chip + 6) = tw(vc, 0x10);
                m.mt_dmacon |= tw(vc, 0x1A);
            }
        }
    }
    track_row_effects(m, vc, chip);
}

// mt_music
void track_music(volatile MusicData &m)
{
    uint32_t data = m.mt_data;
    uint32_t song = data + 0x3B8;
    m.mt_counter = uint8_t(m.mt_counter + 1);
    if (int8_t(m.mt_counter) < int8_t(m.mt_speed)) {
        for (uint32_t v = 0; v < 4; v++)
            track_effects(m, track_voice(m, v), audio(v));
        if (!m.mt_break)
            return;
    } else {
        m.mt_counter = 0;
        m.mt_dmacon = 0;
        uint32_t at = uint16_t(uint16_t(peek_b(ptr<void>(song + m.mt_songpos)) << 10) + m.mt_pattpos);
        for (uint32_t v = 0; v < 4; v++, at += 4)
            track_play_voice(m, data + 0x43C, at, track_voice(m, v), audio(v), v);
        if (m.mt_dmacon)
            m.mt_dmacon |= 0x8000;
        m.mt_pattpos = uint16_t(m.mt_pattpos + 0x10);
        if (m.mt_pattpos != 0x400)
            goto exit;
    }
next:
    m.mt_pattpos = 0;
    m.mt_break = 0;
    m.mt_songpos = uint8_t((m.mt_songpos + 1) & 0x7F);
    if (peek_b(ptr<void>(song - 2)) == m.mt_songpos) {
        if (!m.Track_Loop)
            m.Track_Stop = 1;
        m.mt_songpos = peek_b(ptr<void>(song - 1));
    }
exit:
    if (m.mt_break)
        goto next;
    // the DMA: once the hardware took the new sounds
    if (uint16_t dma = m.mt_dmacon) {
        lines(5);
        w16(CustomAddress + DmaCon) = dma;
    }
}

void tracker(volatile MusicData &m)
{
    if (!m.mt_on)
        return;
    if (m.mt_dmacon) {
        // the repeat parts of the samples started
        for (uint32_t v = 0; v < 4; v++) {
            volatile uint8_t *vc = track_voice(m, v);
            w32(audio(v)) = tl(vc, 0x0A);
            w16(audio(v) + 4) = tw(vc, 0x0E);
        }
    }
    track_music(m);
    if (m.Track_Stop) {
        m.mt_on = 0;
        m.mt_dmacon = 0;
        m.Track_Stop = 0;
        for (uint32_t v = 0; v < 4; v++)
            w16(audio(v) + 8) = 0;
        w16(CustomAddress + DmaCon) = 0xF;
    }
}

} // namespace

namespace amos::music {

// MuIntE: the next part of the envelope of voice v; at its end the voice
// stops (on, ended: the interrupt's masks) and goes back to the music
void env_next(uint32_t v, volatile MusEnvelope &e, uint16_t &on, uint16_t &ended)
{
    uint32_t at = e.EnvAd;
    for (;;) {
        int16_t steps = int16_t(peek_w(ptr<void>(at)));
        at += 2;
        if (!steps) {
            ended |= uint16_t(1 << v);
            on &= uint16_t(~(1 << v));
            md().Noise &= uint16_t(~(1 << v));
            low_byte(md().MuReStart) |= uint8_t(1 << v);
            return;
        }
        if (steps < 0) {
            at = e.EnvDeb;
            continue;
        }
        e.EnvNb = uint16_t(steps);
        uint16_t level = uint16_t(uint16_t(e.EnvDVol * peek_w(ptr<void>(at))) >> 6);
        at += 2;
        uint16_t diff = uint16_t(level - (e.EnvVol >> 16));
        int32_t x = int16_t(diff);
        x = int32_t((uint32_t(x) & 0xFFFF0000u) | uint16_t(diff << 8));
        int32_t q = x / steps;
        if (q >= -32768 && q <= 32767)
            x = int16_t(q);
        e.EnvDelta = uint32_t(int32_t(int16_t(x)) << 8);
        e.EnvVol = e.EnvVol & 0xFFFF0000u;
        e.EnvAd = at;
        return;
    }
}

// MOff: the music's voices silent, the sample interrupts off
void stop_sound()
{
    custom(IntEna) = 0b0000011110000000;
    uint16_t mask = md().MuDMAsk;
    if (!mask)
        return;
    custom(DmaCon) = mask;
    // (the bits tested from 3 down, the registers from voice 0 up)
    for (uint32_t v = 0; v < 4; v++) {
        if (mask & (1 << (3 - v))) {
            w16(audio(v) + 4) = 2;
            w16(audio(v) + 8) = 0;
        }
    }
}

// DmaWait: until the audio DMA took the change
void dma_wait()
{
    lines(5);
    for (int i = 0; i < 9; i++)
        asm volatile("");
}

void sami_stop()
{
    custom(IntEna) = 0b0000011110000000;
    custom(IntReq) = 0b0000011110000000;
}

} // namespace amos::music

extern "C" {

// MusInt, in the VBL server
void ext_music_vbl()
{
    volatile MusicData &m = md();
    if (uint16_t on = m.EnvOn) {
        uint16_t ended = 0;
        for (uint32_t v = 0; v < 4; v++) {
            if (!(on & (1 << v)))
                continue;
            volatile MusEnvelope &e = envelope(v);
            uint32_t vol = e.EnvDelta + e.EnvVol;
            e.EnvVol = vol;
            w16(audio(v) + 8) = uint16_t(vol >> 16);
            if (!decrement(e.EnvNb))
                env_next(v, e, on, ended);
        }
        m.EnvOn = on;
        custom(DmaCon) = ended;
        custom(IntEna) = uint16_t(ended << 7);
        // the noise, 8 new words of it
        if (m.Noise) {
            uint16_t at = m.PNoise, seed = m.BSeed;
            uint32_t wave = m.WaveBase + offsetof(MusWave, WaveDeb);
            for (int i = 0; i < 8; i++) {
                seed = uint16_t((uint32_t(uint16_t(seed + custom(VHPosR))) * 0x3171) >> 8);
                w16(wave + int16_t(at)) = seed;
                at = uint16_t(at - 2);
                if (int16_t(at) < 0)
                    at = LNoise - 2;
            }
            m.PNoise = at;
            m.BSeed = seed;
        }
    }
    if (!m.MuBase) {
        tracker(m);
        return;
    }
    volatile MusTable &t = table_at(m.MuBase);
    every(m, t);
    uint16_t count = uint16_t(t.MuCpt + t.MuTempo);
    t.MuCpt = count;
    if (count < m.TempoBase) {
        effects(m, t);
        return;
    }
    t.MuCpt = uint16_t(t.MuCpt - m.TempoBase);
    // a step of the music
    uint16_t counting = 0, started = 0;
    for (uint32_t v = 0; v < 4; v++) {
        volatile MusVoice &vo = voice(t, v);
        volatile uint8_t &c = count_low(vo);
        if (!c)
            continue;
        counting++;
        if (!decrement(c))
            step(m, t, vo, v, m.MuChip[v], started);
    }
    w16(CustomAddress + DmaCon) = uint16_t(started & m.MuDMAsk);
    if (counting)
        return;
    // the music has ended: the one before it again
    if (!decrement(m.MuNumber)) {
        m.MuBase = 0;
        stop_sound();
        return;
    }
    uint32_t at = mb() + offsetof(MusicData, MuBuffer) + uint32_t(m.MuNumber - 1) * MusTable::MuLong;
    m.MuBase = at;
    m.MuReStart = m.MuDMAsk;
    table_at(at).MuStart = 0;
    table_at(at).MuStop = 0;
}

// Sami_handler: the next part of the sample of voice s
void ext_music_sami(SampleInt *si)
{
    volatile SampleInt &s = *si;
    uint32_t chip = CustomAddress + s.Sami_reg;
    uint32_t pos = s.Sami_pos;
    if (pos >= s.Sami_long) {
        uint32_t old = pos;
        if (uint32_t next = s.Sami_radr) {
            // the other buffer
            s.Sami_radr = 0;
            s.Sami_adr = next;
            s.Sami_long = s.Sami_rlong;
            pos = 0;
        } else if (int32_t(s.Sami_rpos) >= 0) {
            pos = s.Sami_rpos;
        } else {
            // the end: once the last part has played
            s.Sami_pos = s.Sami_pos | 0x80000000u;
            if (int32_t(old) >= 0) {
                custom(IntReq) = s.Sami_bit;
                return;
            }
            w16(chip + 10) = 0;
            custom(DmaCon) = uint16_t(1 << s.Sami_dma);
            if (int16_t(s.Sami_dvol) >= 0)
                low_byte(md().MuReStart) |= uint8_t(1 << s.Sami_dma);
            custom(IntEna) = s.Sami_bit;
            custom(IntReq) = s.Sami_bit;
            return;
        }
    }
    uint32_t end = pos + 1024 * 4;
    if (end > s.Sami_long)
        end = s.Sami_long;
    s.Sami_pos = end;
    w32(chip) = s.Sami_adr + pos;
    w16(chip + 4) = uint16_t((end - pos) >> 1);
    if (int16_t(s.Sami_dvol) >= 0)
        w16(chip + 8) = s.Sami_dvol;
    custom(IntReq) = s.Sami_bit;
}
}
