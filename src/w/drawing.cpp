// Drawing functions of AMOS.library's screen table (EcCall): Cls of a
// rectangle, the clip rectangle, Set Pattern, Paint and the sliders. Also
// the drawing state of the current screen saved and restored around
// menus and sliders.

#include "core/amiga.h"
#include "w/w.h"

#include <graphics/rastport.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::w;

namespace {

struct RastPort *rastport() { return ptr<struct RastPort>(wdata().RastPort); }

// The blitter mask of the first word of a line from pixel n (MCls)
uint16_t left_mask(int n) { return 0xFFFF >> n; }

// ---- CLS ----------------------------------------------------------------------

// ClsR: x1..x2, y1 and height lines in colour
void cls_blit(Screen *s, uint16_t colour, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t height)
{
    auto &c = chips();
    hal::own_blitter();
    hal::wait_blit();
    c.bltafwm = left_mask(x1 & 15);
    uint16_t last = 0;
    uint16_t last_mask = 0xFFFF;
    if (x2 & 15) {
        last = 1;
        last_mask = ~left_mask(x2 & 15);
    }
    c.bltalwm = last_mask;
    uint16_t words = (x2 >> 4) - (x1 >> 4) + last;
    uint16_t size = uint16_t(height << 6) | words;
    uint32_t offset = uint32_t(y1) * s->EcTLigne + uint32_t(int16_t((x1 >> 4) << 1));
    uint16_t modulo = s->EcTLigne - (words << 1);
    c.bltcmod = modulo;
    c.bltdmod = modulo;
    c.bltcon0 = 0x03CA;
    c.bltcon1 = 0;
    c.bltadat = 0xFFFF;
    for (int p = 0; p < s->EcNPlan; p++, colour >>= 1) {
        c.bltbdat = colour & 1 ? 0xFFFF : 0;
        uint8_t *a = ptr<uint8_t>(s->EcCurrent[p]) + offset;
        c.bltcpt = a;
        c.bltdpt = a;
        c.bltsize = size;
        hal::wait_blit();
    }
    hal::disown_blitter();
}

uint16_t clamp(uint16_t v, uint16_t max)
{
    if (int16_t(v) < 0)
        return 0;
    return v > max ? max : v;
}

// EcCls: d1 the colour, d2/d3 to d4/d5, clipped to the screen
void cls(AllRegs *r)
{
    Screen *s = current_screen();
    uint16_t x1 = clamp(r->d[2], s->EcTx), y1 = clamp(r->d[3], s->EcTy);
    uint16_t x2 = clamp(r->d[4], s->EcTx), y2 = clamp(r->d[5], s->EcTy);
    r->d[0] = 0;
    if (x2 <= x1 || y2 <= y1)
        return;
    uint16_t colour = r->d[1];
    if (!s->EcAuto) {
        cls_blit(s, colour, x1, y1, x2, y2 - y1);
        return;
    }
    call(w_tabk1);
    cls_blit(s, colour, x1, y1, x2, y2 - y1);
    call(w_tabk2);
    cls_blit(s, colour, x1, y1, x2, y2 - y1);
    call(w_tabk3);
}

// ---- CLIP ---------------------------------------------------------------------

// TSClip: d1 < 0 no clipping, else d2/d3 to d4/d5 (EntNul: unchanged)
void set_clip(AllRegs *r)
{
    Screen *s = current_screen();
    Regs c;
    if (int16_t(r->d[1]) < 0) {
        c.d[0] = 0;
        c.d[1] = 0;
        c.d[2] = s->EcTx;
        c.d[3] = s->EcTy;
    } else {
        int32_t x1 = r->d[2] == EntNul ? s->EcClipX0 : r->d[2];
        int32_t y1 = r->d[3] == EntNul ? s->EcClipY0 : r->d[3];
        int32_t x2 = r->d[4] == EntNul ? s->EcClipX1 : r->d[4];
        int32_t y2 = r->d[5] == EntNul ? s->EcClipY1 : r->d[5];
        if (x1 < 0 || y1 < 0 || uint32_t(x2) > s->EcTx || uint32_t(y2) > s->EcTy || x2 <= x1 || y2 <= y1) {
            r->d[0] = 1;
            return;
        }
        c.d[0] = uint16_t(x1);
        c.d[1] = uint16_t(y1);
        c.d[2] = uint16_t(x2);
        c.d[3] = uint16_t(y2);
    }
    set_clip_rect(c.d[0], c.d[1], c.d[2], c.d[3]);
    r->d[0] = 0;
}

// ---- SET PATTERN ----------------------------------------------------------------

// SoMouse: skip n images of the mouse bank, false at its end
bool skip_images(const uint8_t *&image, uint16_t n)
{
    do {
        int16_t width = peek_w(image);
        if (width < 0)
            return false;
        uint32_t words = uint32_t(uint16_t(width)) * peek_w(image + 2) * peek_w(image + 4);
        image += 6 + 4 + int16_t(words << 1);
    } while (n-- != 0);
    return true;
}

// SPat: pattern d1: 0 none, n > 0 image n+3 of the mouse bank, n < 0
// image -n of the sprite bank. The height used is the largest power of
// two up to 128 in the image's; images of several planes give
// multi-coloured patterns.
uint32_t set_pattern(int16_t n)
{
    clear_pattern();
    if (!n)
        return 0;
    const uint8_t *image;
    if (n > 0) {
        image = ptr<uint8_t>(wdata().MouBank);
        if (!skip_images(image, 4 - 1))
            return 1;
        if (--n && !skip_images(image, n - 1))
            return 1;
    } else {
        const uint8_t *bank = ptr<uint8_t>(wdata().SprBank);
        if (!bank)
            return 0;
        uint16_t number = -n;
        if (number > peek_w(bank))
            return 0;
        uint32_t address = peek_l(bank + 2 + (number - 1) * 8);
        if (!address)
            return 0;
        image = ptr<uint8_t>(address);
    }
    uint16_t width = peek_w(image), height = peek_w(image + 2), planes = peek_w(image + 4);
    image += 10;
    uint16_t lines = 1;
    uint8_t shift = 0;
    while (lines != height) {
        if (lines > height) {
            if (!--shift)
                return 1;
            lines >>= 1;
            break;
        }
        lines <<= 1;
        if (++shift >= 8)
            return 1;
    }
    if (planes != 1)
        shift = -shift;
    uint16_t bytes = uint16_t(lines << 1) * planes;
    uint32_t pattern = chip_mm(bytes);
    if (!pattern)
        return 1;
    Screen *s = current_screen();
    s->EcPat = pattern;
    s->EcPatL = bytes;
    *reinterpret_cast<uint8_t *>(&s->EcPatY) = shift;
    struct RastPort *rp = rastport();
    rp->AreaPtrn = ptr<UWORD>(pattern);
    rp->AreaPtSz = shift;
    // The first word of each line of each plane
    be_u16 *to = ptr<be_u16>(pattern);
    uint16_t line = width << 1;
    uint16_t plane = uint16_t(line * height);
    for (uint16_t p = 0; p < planes; p++, image += int16_t(plane)) {
        const uint8_t *from = image;
        for (uint16_t y = 0; y < lines; y++, from += int16_t(line))
            *to++ = peek_w(from);
    }
    return 0;
}

// ---- PAINT ----------------------------------------------------------------------

// The fill works in the TmpRas, on the rectangle of words around the clip
// rectangle: walls are 1 bits, the filled pixels become 1 too. The seeds
// waiting to be filled are a stack of words (x, y with three flags on top)
// in the 1024-byte buffer, which overflows into up to 16 buffers of 2048
// bytes, allocated as needed.
constexpr uint32_t ExtraSize = 2048;

struct Fill {
    Screen *s;
    uint8_t *tmp;
    uint32_t start;             // the rectangle in a bitplane
    uint16_t modulo;            // of the screen around it
    uint16_t size;              // blitter size of the rectangle
    uint16_t colour;            // the colour being filled
    // The stack of seeds
    be_u32 *table;            // the extra buffers, ending with -1
    be_u32 *next;             // next entry of the table to use
    uint8_t *top;
    uint8_t *limit;

    // PMask: TmpRas = combination of the planes with the TmpRas: first
    // (bit 0 of colour clear) or first_set (set) for the first plane, then
    // other / other_set
    void mask(uint8_t first, uint8_t first_set, uint8_t other, uint8_t other_set)
    {
        auto &c = chips();
        hal::own_blitter();
        hal::wait_blit();
        uint16_t bits = colour;
        for (int p = 0; p < s->EcNPlan; p++, bits >>= 1) {
            c.bltafwm = 0xFFFF;
            c.bltalwm = 0xFFFF;
            c.bltapt = ptr<uint8_t>(s->EcCurrent[p]) + start;
            c.bltamod = modulo;
            c.bltbpt = tmp;
            c.bltbmod = 0;
            c.bltdpt = tmp;
            c.bltdmod = 0;
            c.bltcon0 = 0x0D00 | (bits & 1 ? first_set : first);
            c.bltcon1 = 0;
            c.bltsize = size;
            first = other;
            first_set = other_set;
            hal::wait_blit();
        }
        hal::disown_blitter();
    }

    // PntNBuf: the next buffer of the stack. False if there is none.
    bool more()
    {
        uint32_t buffer = *next;
        if (int32_t(buffer) < 0)
            return false;
        if (!buffer) {
            buffer = fast_mm2(ExtraSize);
            if (!buffer)
                return false;
        }
        *next++ = buffer;
        uint8_t *b = ptr<uint8_t>(buffer);
        poke_l(b, address(limit));
        poke_l(b + 4, address(top));
        poke_w(b + 8, 0xFFFE);
        poke_w(b + 10, 0);
        top = b + 12;
        limit = top + ExtraSize - 16;
        return true;
    }

    bool push(int32_t x, int32_t y, uint16_t flags)
    {
        poke_w(top, uint16_t(x));
        poke_w(top + 2, uint16_t(y) | flags);
        top += 4;
        return top < limit || more();
    }

    // The scan line fill from x,y. False if out of memory.
    bool fill(int32_t x, int32_t y, uint16_t width, uint16_t height, uint16_t first_bit, uint16_t line)
    {
        // Flags: bit 15 the line above needs a seed, 14 the line below,
        // 13 go left first
        constexpr uint16_t Up = 1 << 15, Down = 1 << 14, Left = 1 << 13;
        uint16_t flags = Left;
        auto outside_x = [&](int32_t v) { return uint32_t(v) >= uint32_t(int16_t(width)); };
        auto outside_y = [&](int32_t v) { return uint32_t(v) >= uint32_t(int16_t(height)); };
        for (;;) {
            // Pnt1: a seed
            uint16_t position = uint16_t(x) + first_bit;
            uint8_t *a = tmp + uint32_t(uint16_t(y)) * line + (position >> 3);
            uint16_t bit = 7 - (position & 7);
            flags |= Up | Down;
            bool left = flags & Left;
            flags |= Left;
            if (left) {
                // To the left end of the run
                do {
                    x--;
                    if (++bit >= 8) {
                        bit = 0;
                        a--;
                    }
                } while (!(*a >> bit & 1) && !outside_x(x) && !outside_y(y));
                x++;
                if (bit-- == 0) {
                    bit = 7;
                    a++;
                }
            }
            for (;;) {
                // Pnt5: look up
                y--;
                a -= line;
                if ((*a >> bit & 1) || outside_y(y)) {
                    flags |= Up;
                } else if (flags & Up) {
                    flags &= ~Up;
                    if (!push(x, y, flags))
                        return false;
                }
                // The pixel itself
                y++;
                a += line;
                *a |= 1 << bit;
                // Look down
                y++;
                if ((a[line] >> bit & 1) || outside_y(y)) {
                    flags |= Down;
                } else if (flags & Down) {
                    flags &= ~Down;
                    if (!push(x, y, flags))
                        return false;
                }
                // One to the right
                flags &= ~Left;
                y--;
                x++;
                if (bit-- == 0) {
                    bit = 7;
                    a++;
                    // A whole word at once, where the lines above and below
                    // are as the flags say all along
                    if (!(address(a) & 1) && uint16_t(x + 15) < width && y && uint16_t(y + 1) < height) {
                        uint16_t above = flags & Up ? 0xFFFF : 0, below = flags & Down ? 0xFFFF : 0;
                        while (!peek_w(a) && peek_w(a + line) == below && peek_w(a - line) == above) {
                            poke_w(a, 0xFFFF);
                            a += 2;
                            x += 16;
                            if (outside_x(x))
                                break;
                        }
                    }
                }
                if ((*a >> bit & 1) || outside_x(x) || outside_y(y))
                    break;
            }
            // Pnt11: the next seed
            for (;;) {
                top -= 4;
                uint16_t word = peek_w(top + 2);
                flags = word & 0xE000;
                y = word & 0x1FFF;
                x = int16_t(peek_w(top));
                if (x >= 0)
                    break;
                // The start of a buffer: back to the previous one
                // (As the assembler: the limit from the second long, the top
                // from the first; when a buffer fills, they are equal)
                next--;
                limit = ptr<uint8_t>(peek_l(top - 4));
                top = ptr<uint8_t>(peek_l(top - 8));
                if (int32_t(address(top)) < 0)
                    return true;
            }
        }
    }

    // The filled pixels in the ink (and pattern) of the RastPort
    void draw()
    {
        auto &c = chips();
        hal::own_blitter();
        hal::wait_blit();
        c.bltafwm = 0xFFFF;
        c.bltalwm = 0xFFFF;
        c.bltcmod = modulo;
        c.bltdmod = modulo;
        c.bltamod = 0;
        c.bltcon0 = 0x0BCA;
        c.bltcon1 = 0;
        struct RastPort *rp = rastport();
        uint16_t fg = rp->FgPen, bg = rp->BgPen;
        uint16_t line = (size & 0x3F) | 0x40;
        // The pattern: its words, for each plane if multi-coloured
        static const be_u16 solid(0xFFFF);
        const be_u16 *pattern = &solid, *end = &solid;
        uint32_t plane_step = 0;
        if (rp->AreaPtrn) {
            pattern = reinterpret_cast<const be_u16 *>(rp->AreaPtrn);
            int8_t sz = rp->AreaPtSz;
            uint8_t n = sz < 0 ? -sz : sz;
            uint16_t bytes = 1;
            do
                bytes <<= 1;
            while (--n);
            bytes <<= 1;
            end = reinterpret_cast<const be_u16 *>(reinterpret_cast<const uint8_t *>(pattern) + int16_t(bytes));
            if (sz < 0)
                plane_step = bytes;
        }
        for (int p = 0; p < s->EcNPlan; p++) {
            hal::wait_blit();
            const be_u16 *w = pattern;
            uint8_t *a = ptr<uint8_t>(s->EcCurrent[p]) + start;
            c.bltcpt = a;
            c.bltdpt = a;
            c.bltapt = tmp;
            uint16_t n = size >> 6;
            do {
                uint16_t bits = *w++;
                uint16_t v = (fg & 1 ? bits : 0) | (bg & 1 ? uint16_t(~bits) : 0);
                hal::wait_blit();
                c.bltbdat = v;
                c.bltsize = line;
                if (w >= end)
                    w = pattern;
            } while (--n);
            pattern = reinterpret_cast<const be_u16 *>(reinterpret_cast<const uint8_t *>(pattern) + plane_step);
            end = reinterpret_cast<const be_u16 *>(reinterpret_cast<const uint8_t *>(end) + plane_step);
            fg >>= 1;
            bg >>= 1;
        }
        hal::disown_blitter();
    }
};

// TPaint: fill from d1/d2 the area of colour d3 (the colour at that
// point), d5: a 1024-byte buffer. (d4, the mode, is not used.) The
// rectangle and TmpRas offsets are 32 bits: the assembler kept the start
// in a word and added in 16 bits, wrong on screens of more than 64 KB a
// bitplane.
void paint(AllRegs *r)
{
    Screen *s = current_screen();
    int32_t x = int16_t(r->d[1]), y = int16_t(r->d[2]);
    if (uint32_t(x) < s->EcClipX0 || uint32_t(x) >= s->EcClipX1 || uint32_t(y) < s->EcClipY0
        || uint32_t(y) >= s->EcClipY1)
        return;
    struct TmpRas *tmp_ras = rastport()->TmpRas;
    if (!tmp_ras)
        return;
    Fill f;
    f.s = s;
    f.colour = r->d[3];
    uint16_t x0 = s->EcClipX0 & 0xFFF0;
    uint16_t words = uint16_t(((s->EcClipX1 + 15) & 0xFFF0) - x0) >> 4;
    uint16_t height = s->EcClipY1 - s->EcClipY0;
    f.size = uint16_t(height << 6) | words;
    uint16_t line = words << 1;
    f.modulo = s->EcTLigne - line;
    f.start = uint32_t(s->EcClipY0) * s->EcTLigne + (x0 >> 3);
    if (uint32_t(height) * line > uint32_t(tmp_ras->Size))
        return;
    f.tmp = reinterpret_cast<uint8_t *>(tmp_ras->RasPtr);

    // The walls: the pixels of other colours
    f.mask(0xF0, 0x0F, 0xFC, 0xCF);

    // The stack in the buffer: the table of extra buffers, then the bottom
    // marker
    uint8_t *buffer = ptr<uint8_t>(r->d[5]);
    f.table = reinterpret_cast<be_u32 *>(buffer);
    f.next = f.table;
    for (int i = 0; i < 16; i++)
        f.table[i] = 0;
    poke_l(buffer + 64, 0xFFFFFFFF);
    poke_l(buffer + 68, 0xFFFFFFFF);
    poke_w(buffer + 72, 0xFFFF);
    poke_w(buffer + 74, 0);
    f.top = buffer + 76;
    f.limit = buffer + 1024 - 16 * 4 - 16;

    uint16_t first_bit = s->EcClipX0 & 15;
    bool ok = f.fill(x - s->EcClipX0, y - s->EcClipY0, s->EcClipX1 - s->EcClipX0, height, first_bit, line);
    if (ok) {
        // Keep the filled pixels only, not the walls
        f.mask(0x0C, 0xC0, 0x0C, 0xC0);
        f.draw();
    }
    for (be_u32 *t = f.table; int32_t(*t) > 0; t++)
        free_mm(*t, ExtraSize);
}

// ---- SLIDERS --------------------------------------------------------------------

// SliPour: the position (d6) and size (d7) of the knob of a slider of
// d0 pixels, total d3, position d4, size d5. The size is at least 4; a
// knob reaching the end is put exactly at the end. The assembler left
// the caller's d6/d7 in d4/d5, and a flag in bit 31 of d7.
void slider_knob(AllRegs *r)
{
    uint16_t length = r->d[0];
    uint16_t total = r->d[3], position = r->d[4], size = r->d[5];
    uint32_t d7 = r->d[7];
    if (size >= total) {
        if (!total) {
            total = 1;
            r->d[3] = 1;
        }
        size = total;
    }
    d7 &= ~0x80000000u;
    if (uint16_t(size + position) >= total)
        d7 |= 0x80000000u;
    uint32_t scaled = uint32_t(length) << 16;
    uint16_t start, knob;
    if (scaled / total <= 0xFFFF) {
        // In 1/65536
        uint32_t q = scaled / total;
        start = uint32_t(position) * uint16_t(q) >> 16;
        uint32_t k = uint32_t(uint16_t(q)) * size;
        if (uint16_t(k) >= 0x8000)
            k += 0x10000;
        knob = k >> 16;
    } else if ((uint32_t(length) << 8) / total <= 0xFFFF) {
        // In 1/256
        uint32_t q = (uint32_t(length) << 8) / total;
        start = uint32_t(position) * uint16_t(q) >> 8;
        uint32_t k = uint32_t(uint16_t(q)) * size;
        if (uint8_t(k) >= 0x80)
            k += 0x100;
        knob = k >> 8;
    } else {
        uint16_t q = length / total;
        start = uint32_t(position) * q;
        knob = uint32_t(q) * size;
    }
    if (knob < 4)
        knob = 4;
    uint16_t from = start;
    if (from >= length)
        from = length - knob;
    uint16_t to = from + knob;
    if (to > length) {
        from = length - knob;
        to = length;
    }
    to -= from;
    r->d[4] = r->d[6];
    r->d[5] = r->d[7];
    r->d[6] = d7 & 0x80000000u ? uint16_t(length - to) : from;
    r->d[7] = (d7 & 0xFFFF0000) | to;
    r->d[0] = 0;
}

// SliPut: the inks and pattern of one part. (The assembler set the A pen
// to the outline colour C, not to ink A; kept.)
void slider_inks(uint8_t ink_b, uint8_t ink_c, uint16_t pattern)
{
    struct RastPort *rp = rastport();
    rp->AOlPen = ink_c;
    hal::set_b_pen(rp, ink_b);
    hal::set_a_pen(rp, ink_c);
    rp->LinePtrn = 0xFFFF;
    rp->Flags |= AREAOUTLINE;
    set_pattern(pattern);
}

// SliDess
void slider_bar(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (x2 > x1 && y2 > y1)
        hal::rect_fill(rastport(), x1, y1, x2, y2);
}

// SliHor / SliVer: d1/d2 the size, d3 total, d4 position, d5 size of
// the knob, d6/d7 the corner. Error 1 if the position is past the total.
void slider(AllRegs *r, bool vertical)
{
    Screen *s = current_screen();
    push_state();
    uint16_t width = r->d[1], height = r->d[2];
    if (uint16_t(r->d[4]) > uint16_t(r->d[3])) {
        pull_state();
        r->d[0] = 1;
        return;
    }
    AllRegs k = *r;
    k.d[0] = vertical ? height : width;
    slider_knob(&k);
    uint16_t x1 = r->d[6], y1 = r->d[7];
    uint16_t x2 = x1 + width, y2 = y1 + height;
    uint16_t start = k.d[6], knob = k.d[7];
    auto frame = [&] { slider_inks(s->EcFInkB, s->EcFInkC, s->EcFPat); };
    auto inside = [&] { slider_inks(s->EcIInkB, s->EcIInkC, s->EcIPat); };
    if (vertical) {
        uint16_t a = y1 + start, b = a + knob;
        frame();
        slider_bar(x1, y1, x2, a);
        inside();
        slider_bar(x1, a, x2, b);
        frame();
        slider_bar(x1, b, x2, y2);
    } else {
        uint16_t a = x1 + start, b = a + knob;
        frame();
        slider_bar(x1, y1, a, y2);
        inside();
        slider_bar(a, y1, b, y2);
        frame();
        slider_bar(b, y1, x2, y2);
    }
    pull_state();
    r->d[0] = 0;
}

// SliSet: inks a/b/c and pattern of the frame (d0-d3) and of the knob
// (d4-d7); EntNul keeps one
void slider_set(AllRegs *r)
{
    Screen *s = current_screen();
    uint8_t *inks[] = {&s->EcFInkA, &s->EcFInkB, &s->EcFInkC, nullptr,
                       &s->EcIInkA, &s->EcIInkB, &s->EcIInkC, nullptr};
    for (int i = 0; i < 8; i++) {
        if (r->d[i] == EntNul)
            continue;
        if (inks[i])
            *inks[i] = r->d[i];
        else
            (i == 3 ? s->EcFPat : s->EcIPat) = r->d[i];
    }
    r->d[0] = 0;
}

// WScCpy: the fast Screen Copy, with the A and D channels only, when the
// minterm is $CC and x1 (d0), x2 (d2) and the width (d4) are multiples of
// 16. a0/a1: the bitmaps, d1/d3 the lines, d5 the height. d7: 0 done,
// -1 not done. Overlapping copies go backwards when needed.
uint32_t screen_copy(const AllRegs *r)
{
    uint16_t x1 = r->d[0], y1 = r->d[1], x2 = r->d[2], y2 = r->d[3], width = r->d[4], height = r->d[5];
    if (uint8_t(r->d[6]) != 0xCC || (x1 & 15) || (x2 & 15) || (width & 15))
        return 0xFFFFFFFF;
    uint16_t con1 = 0;
    if (!(int16_t(y2) < int16_t(y1) || (y2 == y1 && int16_t(x2) < int16_t(x1)))) {
        con1 = 2;               // descending: from the last word
        x1 += width - 16;
        y1 += height - 1;
        x2 += width - 16;
        y2 += height - 1;
    }
    const struct BitMap *from = ptr<struct BitMap>(r->a[0]), *to = ptr<struct BitMap>(r->a[1]);
    uint16_t bytes = uint16_t(width >> 4) << 1;
    uint32_t start1 = uint32_t(uint16_t(x1 >> 4) << 1) + uint32_t(from->BytesPerRow) * y1;
    uint32_t start2 = uint32_t(uint16_t(x2 >> 4) << 1) + uint32_t(to->BytesPerRow) * y2;
    uint16_t size = uint16_t(height << 6) + (bytes >> 1);
    auto &c = chips();
    hal::own_blitter();
    hal::wait_blit();
    c.bltcon0 = 0x09F0;
    c.bltcon1 = con1;
    c.bltbdat = 0xFFFF;
    c.bltcdat = 0xFFFF;
    c.bltalwm = 0xFFFF;
    c.bltafwm = 0xFFFF;
    c.bltamod = from->BytesPerRow - bytes;
    c.bltdmod = to->BytesPerRow - bytes;
    int planes = from->Depth < to->Depth ? from->Depth : to->Depth;
    for (int p = 0; p < planes; p++) {
        uint8_t *a = from->Planes[p] + start1, *d = to->Planes[p] + start2;
        hal::wait_blit();
        c.bltapt = a;
        c.bltcpt = d;
        c.bltdpt = d;
        c.bltsize = size;
    }
    hal::wait_blit();
    hal::disown_blitter();
    return 0;
}

} // namespace

namespace amos::w {

// Ec_SetClip: the clip rectangle of the current screen, through a clip
// region of its layer, which the first call installs
void set_clip_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    Screen *s = current_screen();
    if (!s->Ec_Region) {
        if (x0 == s->EcClipX0 && y0 == s->EcClipY0 && x1 == s->EcClipX1 && y1 == s->EcClipY1)
            return;
        struct Region *region = hal::new_region();
        s->Ec_Region = address(region);
        if (!region)
            return;
        struct Rectangle rect = {int16_t(x0), int16_t(y0), int16_t(x1 - 1), int16_t(y1 - 1)};
        if (!hal::or_rect_region(region, &rect))
            return;
        hal::install_clip_region(ptr<struct Layer>(s->Ec_Layer), region);
    }
    s->EcClipX0 = x0;
    s->EcClipY0 = y0;
    s->EcClipX1 = x1;
    s->EcClipY1 = y1;
    // the bounds of the layer's ClipRect
    be_u16 *bounds = ptr<be_u16>(peek_l(ptr<uint8_t>(s->Ec_Layer) + 8) + 16);
    bounds[0] = x0;
    bounds[1] = y0;
    bounds[2] = x1 - 1;
    bounds[3] = y1 - 1;
}

// T_EcSave: pens, draw mode, flags, line pattern, position, area pattern,
// the font fields of the RastPort, then the clip rectangle
struct Saved {
    uint8_t fg, bg, outline, mode;
    be_u16 flags, line, x, y;
    be_u32 pattern;
    uint8_t pattern_size, pad;
    uint8_t font[14];
    be_u16 clip[4];
};
static_assert(sizeof(Saved) == 40);

// Ec_Push: the drawing state of the current screen into T_EcSave (not
// while a menu has it saved)
void push_state()
{
    WData &d = wdata();
    struct RastPort *rp = rastport();
    if (d.PaPeek || !rp)
        return;
    auto &v = *reinterpret_cast<Saved *>(d.EcSave);
    v.fg = rp->FgPen;
    v.bg = rp->BgPen;
    v.outline = rp->AOlPen;
    v.mode = rp->DrawMode;
    v.flags = rp->Flags;
    v.line = rp->LinePtrn;
    v.x = rp->cp_x;
    v.y = rp->cp_y;
    v.pattern = address(rp->AreaPtrn);
    v.pattern_size = rp->AreaPtSz;
    copy(v.font, &rp->Font, sizeof v.font);
    Screen *s = current_screen();
    v.clip[0] = s->EcClipX0;
    v.clip[1] = s->EcClipY0;
    v.clip[2] = s->EcClipX1;
    v.clip[3] = s->EcClipY1;
}

// Ec_Pull: back from T_EcSave
void pull_state()
{
    WData &d = wdata();
    struct RastPort *rp = rastport();
    if (d.PaPeek || !rp)
        return;
    const auto &v = *reinterpret_cast<const Saved *>(d.EcSave);
    hal::set_a_pen(rp, v.fg);
    hal::set_b_pen(rp, v.bg);
    rp->AOlPen = v.outline;
    hal::set_draw_mode(rp, v.mode);
    rp->Flags = v.flags;
    rp->LinePtrn = v.line;
    rp->cp_x = v.x;
    rp->cp_y = v.y;
    rp->AreaPtrn = ptr<UWORD>(v.pattern);
    rp->AreaPtSz = v.pattern_size;
    copy(&rp->Font, v.font, sizeof v.font);
    set_clip_rect(v.clip[0], v.clip[1], v.clip[2], v.clip[3]);
}

// EffPat: no more pattern on the current screen
void clear_pattern()
{
    WData &d = wdata();
    Screen *s = current_screen();
    if (uint32_t pattern = s->EcPat) {
        uint32_t size = int16_t(s->EcPatL);
        s->EcPat = 0;
        s->EcPatL = 0;
        s->EcPatY = 0;
        free_mm(pattern, size);
    }
    struct RastPort *rp = ptr<struct RastPort>(d.RastPort);
    rp->AreaPtrn = nullptr;
    rp->AreaPtSz = 0;
}

} // namespace amos::w

extern "C" {
void w_ec_cls(AllRegs *r) { cls(r); }
void w_ec_set_clip_fn(AllRegs *r) { set_clip(r); }
void w_ec_pattern(AllRegs *r) { r->d[0] = set_pattern(r->d[1]); }
void w_ec_paint(AllRegs *r) { paint(r); }
void w_ec_slider_h(AllRegs *r) { slider(r, false); }
void w_ec_slider_v(AllRegs *r) { slider(r, true); }
void w_ec_slider_set(AllRegs *r) { slider_set(r); }
void w_ec_slider_knob(AllRegs *r) { slider_knob(r); }
void w_ec_screen_copy(AllRegs *r) { r->d[7] = screen_copy(r); }
}
