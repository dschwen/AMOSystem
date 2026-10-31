// The display on another host: a frame composed as the custom chips do,
// from what AMOS leaves in their registers (host/hal_chips.cpp) and in its
// memory. The copper runs its list (COP1LC) over the frame, with the beam:
// its moves, waits (8 bits of the line compared: the $FFDF wait of the
// lines after 255) and skips; a move takes effect on its line if it comes
// before the bitplanes start, else on the next one. Then each line: the
// display window (DIWSTRT/DIWSTOP), the fetch (DDFSTRT/DDFSTOP, the
// modulos), the delays of BPLCON1, lowres or hires, dual playfield, extra
// half-brite and HAM, the 8 sprites with their DMA (attached ones too) and
// their priority (BPLCON2), the 32 colours. The frame is the PAL display
// in hires pixels: 768 by 288 lines (the lines of the beam 26 to 313).

#include "hal/chips.h"
#include "host/host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace amos;

namespace amos::host {

constexpr int FrameWidth = 768, FrameHeight = 288;
constexpr int FirstLine = 26;                   // the beam's line of the frame's first
constexpr int FirstX = 0x5C;                    // the lores position of its left edge

namespace {

constexpr int Lines = 312, LineCC = 227;

uint16_t chip_w(uint32_t at) { return peek_w(ptr<void>(at & ~1u)); }

// The registers as the frame goes: the copper's writes, line by line
struct State {
    uint16_t reg[256];          // by offset / 2
    uint16_t &operator[](uint32_t offset) { return reg[offset >> 1]; }
};

struct Write {
    int line;                   // the line it takes effect on
    uint16_t offset, value;
};
Write writes[8192];
int write_count;

void copper(State &start)
{
    write_count = 0;
    uint32_t pc = host_chip_read(0x80) << 16 | host_chip_read(0x82);
    int line = 0, cc = 0;
    auto advance = [&](int n) {
        cc += n;
        while (cc >= LineCC) {
            cc -= LineCC;
            line++;
        }
    };
    static bool dump = getenv("AMOS_COPPER");
    for (int count = 0; pc && count < 20000 && line < Lines; count++) {
        uint16_t w1 = chip_w(pc), w2 = chip_w(pc + 2);
        if (dump)
            fprintf(stderr, "%3d %3d  %04x %04x\n", line, cc, w1, w2);
        pc += 4;
        if (!(w1 & 1)) {
            // MOVE
            uint16_t reg = w1 & 0x1FE;
            if (reg == 0x88) {          // COPJMP1
                pc = start[0x80] << 16 | start[0x82];
            } else if (reg == 0x8A) {   // COPJMP2
                pc = start[0x84] << 16 | start[0x86];
            } else if (write_count < 8192) {
                // (before the bitplanes' start: this line; else the next)
                writes[write_count++] = {cc < 0x30 ? line : line + 1, reg, w2};
                if (reg >= 0x80 && reg <= 0x86)
                    start[reg] = w2;
            }
            advance(4);
            continue;
        }
        // WAIT / SKIP: the beam against VP/HP with the masks
        int vp = w1 >> 8, hp = w1 & 0xFE, ve = (w2 >> 8 & 0x7F) | 0x80, he = w2 & 0xFE;
        auto reached = [&](int l, int c) {
            int v = l & 0xFF & ve, h = c & he;
            return v > (vp & ve) || (v == (vp & ve) && h >= (hp & he));
        };
        if (w2 & 1) {
            // SKIP: the next instruction left out if the beam is there
            if (reached(line, cc))
                pc += 4;
            advance(4);
            continue;
        }
        if (w1 == 0xFFFF && w2 == 0xFFFE)
            break;
        // to the first position that satisfies it (at most the frame's end)
        int guard = 0;
        while (!reached(line, cc) && line < Lines && guard++ < Lines * LineCC) {
            if (((line & 0xFF & ve) < (vp & ve)) || ((line & 0xFF & ve) == (vp & ve))) {
                advance(1);
            } else {
                // past it in this line's 8 bits: the next line
                advance(LineCC - cc);
            }
        }
        // (a wait for the line's end: the next instruction is on the next
        // line, as the copper's fetches take it there: the $FFDF wait)
        if ((hp & he) >= 0xDE)
            advance(LineCC - cc);
        else
            advance(4);
    }
}

uint32_t rgb(uint16_t c)
{
    uint32_t r = (c >> 8 & 15) * 17, g = (c >> 4 & 15) * 17, b = (c & 15) * 17;
    return r << 16 | g << 8 | b;
}

// The sprites of a line: each one's state of DMA
struct Sprite {
    uint32_t at;                // its list
    int vstart, vstop, hstart;
    bool attached, armed, shown;
    uint16_t data, datb;
};

void sprite_control(Sprite &s, uint16_t pos, uint16_t ctl)
{
    s.vstart = (pos >> 8) | (ctl & 4) << 6;
    s.vstop = (ctl >> 8) | (ctl & 2) << 7;
    s.hstart = (pos & 0xFF) << 1 | (ctl & 1);
    s.attached = ctl & 0x80;
}

} // namespace

// The frame into rgb (FrameWidth by FrameHeight, 0x00RRGGBB)
void display_frame(uint32_t *out)
{
    State st;
    for (uint32_t o = 0; o < 0x200; o += 2)
        st[o] = host_chip_read(o);
    State copper_start = st;
    copper(copper_start);
    uint16_t dma = host_chip_read(0x02);
    bool on = (dma & 0x0300) == 0x0300;      // DMAEN, BPLEN
    uint32_t planes[6];
    for (int p = 0; p < 6; p++)
        planes[p] = uint32_t(st[0xE0 + 4 * p]) << 16 | st[0xE2 + 4 * p];
    Sprite sprites[8] = {};
    for (int n = 0; n < 8; n++) {
        sprites[n].at = uint32_t(st[0x120 + 4 * n]) << 16 | st[0x122 + 4 * n];
        sprites[n].vstart = sprites[n].vstop = -1;
    }
    bool vdisplay = false;
    int w = 0;
    for (int line = 0; line < Lines; line++) {
        // the writes for this line
        for (; w < write_count && writes[w].line <= line; w++) {
            uint16_t o = writes[w].offset, v = writes[w].value;
            if (o == 0x96) {            // DMACON through the copper
                host_chip_write(o, v);
                dma = host_chip_read(0x02);
                on = (dma & 0x0300) == 0x0300;
                continue;
            }
            st[o] = v;
            if (o >= 0xE0 && o < 0xF8) {
                int p = (o - 0xE0) >> 2;
                planes[p] = uint32_t(st[0xE0 + 4 * p]) << 16 | st[0xE2 + 4 * p];
            }
            if (o >= 0x120 && o < 0x140) {
                int n = (o - 0x120) >> 2;
                sprites[n].at = uint32_t(st[0x120 + 4 * n]) << 16 | st[0x122 + 4 * n];
                sprites[n].vstart = -1;
            }
            if (o >= 0x140 && o < 0x180 && (o & 7) == 0) {
                // SPRxPOS/CTL written by the CPU or the copper
                int n = (o - 0x140) >> 3;
                sprite_control(sprites[n], st[0x140 + 8 * n], st[0x142 + 8 * n]);
            }
        }
        uint16_t diwstrt = st[0x8E], diwstop = st[0x90];
        int vstart = diwstrt >> 8, vstop = diwstop >> 8;
        if (!(vstop & 0x80))
            vstop |= 0x100;
        int hstart = diwstrt & 0xFF, hstop = (diwstop & 0xFF) | 0x100;
        if (line == vstart)
            vdisplay = true;
        if (line == vstop)
            vdisplay = false;
        uint16_t con0 = st[0x100], con1 = st[0x102], con2 = st[0x104];
        bool hires = con0 & 0x8000, ham = con0 & 0x0800, dual = con0 & 0x0400;
        int depth = con0 >> 12 & 7;
        if (depth > 6)
            depth = 6;
        bool ehb = depth == 6 && !ham && !dual;
        uint32_t colours[64];
        for (int c = 0; c < 32; c++) {
            colours[c] = rgb(st[0x180 + 2 * c]);
            colours[c + 32] = rgb(uint16_t(st[0x180 + 2 * c] >> 1 & 0x777));
        }
        int y = line - FirstLine;
        uint32_t *row = y >= 0 && y < FrameHeight ? out + y * FrameWidth : nullptr;
        // the line's pixels (hires positions of the frame): the background
        uint8_t pf[FrameWidth];
        memset(pf, 0, sizeof pf);
        bool fetched = on && vdisplay && depth;
        int ddfstrt = st[0x92] & 0xFC, ddfstop = st[0x94] & 0xFC;
        int words = hires ? (ddfstop - ddfstrt) / 4 + 2 : (ddfstop - ddfstrt) / 8 + 1;
        if (words < 0 || words > 64)
            words = 0;
        if (fetched && words) {
            // the first pixel's lores position, the delays
            int first = hires ? ddfstrt * 2 + 9 : ddfstrt * 2 + 17;
            int delay1 = con1 & 15, delay2 = con1 >> 4 & 15;
            int pixels = words * 16;
            for (int i = 0; i < pixels; i++) {
                int value = 0;
                for (int p = 0; p < depth; p++) {
                    int delay = (p & 1) ? delay2 : delay1;
                    int k = i - (hires ? delay * 2 : delay);
                    if (k < 0)
                        continue;
                    uint16_t word = chip_w(planes[p] + uint32_t(k >> 4) * 2);
                    if (word & (0x8000 >> (k & 15)))
                        value |= 1 << p;
                }
                // to the frame's hires positions
                int x = hires ? (first - FirstX) * 2 + i : (first - FirstX) * 2 + 2 * i;
                for (int r = 0; r < (hires ? 1 : 2); r++)
                    if (x + r >= 0 && x + r < FrameWidth)
                        pf[x + r] = uint8_t(value);
            }
            for (int p = 0; p < depth; p++) {
                int16_t mod = int16_t(p & 1 ? st[0x10A] : st[0x108]);
                planes[p] += uint32_t(words * 2 + mod);
            }
        }
        // the sprites of the line
        int16_t sprite_colour[FrameWidth];
        uint8_t sprite_pair[FrameWidth];
        for (int i = 0; i < FrameWidth; i++)
            sprite_colour[i] = -1;
        if (dma & 0x0220 && (dma & 0x0220) == 0x0220) {
            for (int n = 0; n < 8; n++) {
                Sprite &s = sprites[n];
                s.shown = false;
                if (!s.at)
                    continue;
                if (s.vstart < 0) {
                    // the control words at the start of the list
                    sprite_control(s, chip_w(s.at), chip_w(s.at + 2));
                    s.at += 4;
                    s.armed = true;
                }
                if (line < s.vstart || line >= s.vstop || !s.armed)
                    continue;
                s.data = chip_w(s.at);
                s.datb = chip_w(s.at + 2);
                s.at += 4;
                s.shown = true;
                if (line == s.vstop - 1) {
                    // the next control words: the sprite again further down, or the end
                    uint16_t pos = chip_w(s.at), ctl = chip_w(s.at + 2);
                    s.at += 4;
                    if (!pos && !ctl)
                        s.armed = false;
                    else
                        sprite_control(s, pos, ctl);
                }
            }
            for (int n = 0; n < 8; n++) {
                Sprite &s = sprites[n];
                if (!s.shown)
                    continue;
                bool odd = n & 1;
                if (odd && s.attached)
                    continue;               // with its even partner
                for (int i = 0; i < 16; i++) {
                    int bits = (s.data >> (15 - i) & 1) | (s.datb >> (15 - i) & 1) << 1;
                    int colour;
                    if (!odd && sprites[n + 1].attached && sprites[n + 1].shown) {
                        const Sprite &o = sprites[n + 1];
                        bits |= (o.data >> (15 - i) & 1) << 2 | (o.datb >> (15 - i) & 1) << 3;
                        if (!bits)
                            continue;
                        colour = 16 + bits;
                    } else {
                        if (!bits)
                            continue;
                        colour = 16 + (n >> 1) * 4 + bits;
                    }
                    int x = (s.hstart + 1 + i - FirstX) * 2;
                    for (int r = 0; r < 2; r++)
                        if (x + r >= 0 && x + r < FrameWidth && sprite_colour[x + r] < 0) {
                            sprite_colour[x + r] = int16_t(colour);
                            sprite_pair[x + r] = uint8_t(n >> 1);
                        }
                }
            }
        }
        if (!row)
            continue;
        // the pixels: the window, the playfields, the sprites in front of them
        uint32_t ham_colour = colours[0];
        int pf1p = con2 & 7, pf2p = con2 >> 3 & 7;
        bool pf2_first = con2 & 0x40;
        for (int x = 0; x < FrameWidth; x++) {
            int lores = FirstX + x / 2;
            bool window = vdisplay && lores >= hstart && lores < hstop;
            uint32_t c = colours[0];
            int playfield = 0;      // 0 none, 1 or 2
            if (window && fetched) {
                int v = pf[x];
                if (ham) {
                    int ctrl = v >> 4 & 3, data = v & 15;
                    uint32_t r = ham_colour >> 16 & 0xFF, g = ham_colour >> 8 & 0xFF, b = ham_colour & 0xFF;
                    if (ctrl == 0)
                        ham_colour = colours[data];
                    else if (ctrl == 1)
                        ham_colour = r << 16 | g << 8 | data * 17;
                    else if (ctrl == 2)
                        ham_colour = uint32_t(data * 17) << 16 | g << 8 | b;
                    else
                        ham_colour = r << 16 | uint32_t(data * 17) << 8 | b;
                    c = ham_colour;
                    playfield = v ? 1 : 0;
                } else if (dual) {
                    int p1 = (v & 1) | (v >> 1 & 2) | (v >> 2 & 4), p2 = (v >> 1 & 1) | (v >> 2 & 2) | (v >> 3 & 4);
                    if (pf2_first ? p2 : !p1 && p2) {
                        c = colours[8 + p2];
                        playfield = 2;
                    } else if (p1) {
                        c = colours[p1];
                        playfield = 1;
                    }
                } else {
                    c = colours[ehb ? v : v & 31];
                    playfield = v ? 1 : 0;
                }
            }
            if (window && sprite_colour[x] >= 0) {
                int pair = sprite_pair[x];
                int priority = playfield == 2 ? pf2p : pf1p;
                if (!playfield || pair < priority)
                    c = colours[sprite_colour[x]];
            }
            row[x] = window ? c : colours[0];
        }
    }
}

} // namespace amos::host

namespace amos::host {

// The frame as a PPM file (AMOS_SHOT: when AMOS ends)
void display_save(const char *path)
{
    static uint32_t frame[FrameWidth * FrameHeight];
    display_frame(frame);
    FILE *f = fopen(path, "wb");
    if (!f)
        return;
    fprintf(f, "P6\n%d %d\n255\n", FrameWidth, FrameHeight);
    for (uint32_t p : frame) {
        uint8_t c[3] = {uint8_t(p >> 16), uint8_t(p >> 8), uint8_t(p)};
        fwrite(c, 1, 3, f);
    }
    fclose(f);
}

} // namespace amos::host
