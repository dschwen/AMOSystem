// hal/gfx.h on another host: graphics.library's drawing, in C++, on the
// planar bitmaps of AMOS's screens; layers with a clipping region; the
// fonts (host/fonts.cpp). The drawing follows graphics.library: the pens
// and draw modes, the write mask, the line and area patterns, the pen
// position. The structures are the NDK's, in memory below 4GB (AMOS keeps
// their addresses).

#include "hal/gfx.h"
#include "hal/system.h"
#include "host/host.h"

#include <graphics/clip.h>
#include <graphics/gfx.h>
#include <graphics/layers.h>
#include <graphics/rastport.h>
#include <graphics/regions.h>
#include <graphics/text.h>
#include <diskfont/diskfont.h>
#include <graphics/view.h>

#include <stdlib.h>
#include <string.h>

using namespace amos;

namespace amos::host {
TextFont *font_open(const char *name, uint16_t size);
void font_close(TextFont *font);
}

namespace {

template <typename T> T *low_new()
{
    return static_cast<T *>(hal::allocate(sizeof(T), 0x10000));
}
template <typename T> void low_delete(T *p) { hal::release(p, sizeof(T)); }

// ---- clipping ----------------------------------------------------------------

struct Clip {
    int x0, y0, x1, y1;     // inclusive
};

// The rectangles a RastPort draws into (at most 64), in bitmap coordinates;
// the origin of its coordinates
int clips(const RastPort *rp, Clip *out, int &ox, int &oy)
{
    const BitMap *bm = rp->BitMap;
    ox = oy = 0;
    Clip all = {0, 0, bm->BytesPerRow * 8 - 1, bm->Rows - 1};
    const Layer *l = rp->Layer;
    if (!l) {
        out[0] = all;
        return 1;
    }
    ox = l->bounds.MinX;
    oy = l->bounds.MinY;
    Clip lb = {l->bounds.MinX, l->bounds.MinY, l->bounds.MaxX, l->bounds.MaxY};
    auto meet = [](Clip a, Clip b) {
        return Clip{a.x0 > b.x0 ? a.x0 : b.x0, a.y0 > b.y0 ? a.y0 : b.y0, a.x1 < b.x1 ? a.x1 : b.x1,
                    a.y1 < b.y1 ? a.y1 : b.y1};
    };
    lb = meet(lb, all);
    if (const ClipRect *cr = l->ClipRect)
        lb = meet(lb, Clip{cr->bounds.MinX, cr->bounds.MinY, cr->bounds.MaxX, cr->bounds.MaxY});
    if (!l->ClipRegion) {
        out[0] = lb;
        return 1;
    }
    int n = 0;
    const Region *r = l->ClipRegion;
    for (const RegionRectangle *rr = r->RegionRectangle; rr && n < 64; rr = rr->Next) {
        Clip c = {r->bounds.MinX + rr->bounds.MinX + ox, r->bounds.MinY + rr->bounds.MinY + oy,
                  r->bounds.MinX + rr->bounds.MaxX + ox, r->bounds.MinY + rr->bounds.MaxY + oy};
        c = meet(c, lb);
        if (c.x0 <= c.x1 && c.y0 <= c.y1)
            out[n++] = c;
    }
    return n;
}

// ---- pixels ------------------------------------------------------------------

struct Painter {
    RastPort *rp;
    Clip clip[64];
    int n, ox, oy;

    explicit Painter(RastPort *r) : rp(r) { n = clips(r, clip, ox, oy); }

    bool inside(int x, int y) const
    {
        for (int i = 0; i < n; i++)
            if (x >= clip[i].x0 && x <= clip[i].x1 && y >= clip[i].y0 && y <= clip[i].y1)
                return true;
        return false;
    }

    // a pixel of colour pen (bitmap coordinates), the mask and the mode
    // (complement: the planes of the mask inverted)
    void put(int x, int y, uint32_t pen, bool complement) const
    {
        if (!inside(x, y))
            return;
        const BitMap *bm = rp->BitMap;
        uint8_t bit = uint8_t(0x80 >> (x & 7));
        uint32_t at = uint32_t(y) * bm->BytesPerRow + uint32_t(x >> 3);
        for (int p = 0; p < bm->Depth; p++) {
            if (!(rp->Mask & (1 << p)) || !bm->Planes[p])
                continue;
            uint8_t &b = bm->Planes[p][at];
            if (complement)
                b ^= bit;
            else if (pen & (1 << p))
                b |= bit;
            else
                b &= uint8_t(~bit);
        }
    }

    int get(int x, int y) const
    {
        const BitMap *bm = rp->BitMap;
        if (x < 0 || y < 0 || x >= bm->BytesPerRow * 8 || y >= bm->Rows)
            return -1;
        uint8_t bit = uint8_t(0x80 >> (x & 7));
        uint32_t at = uint32_t(y) * bm->BytesPerRow + uint32_t(x >> 3);
        int pen = 0;
        for (int p = 0; p < bm->Depth; p++)
            if (bm->Planes[p] && (bm->Planes[p][at] & bit))
                pen |= 1 << p;
        return pen;
    }

    // a pixel drawn with the RastPort's mode: on (a set bit of a pattern or
    // a glyph) or off
    void dot(int x, int y, bool on) const
    {
        uint8_t mode = uint8_t(rp->DrawMode);
        if (mode & INVERSVID)
            on = !on;
        if (mode & COMPLEMENT) {
            if (on)
                put(x, y, 0, true);
            return;
        }
        if (on)
            put(x, y, uint8_t(rp->FgPen), false);
        else if (mode & JAM2)
            put(x, y, uint8_t(rp->BgPen), false);
    }
};

// The line from (x0,y0) to (x1,y1) (RastPort coordinates), its pattern
void line(RastPort *rp, int x0, int y0, int x1, int y1)
{
    Painter p(rp);
    int dx = abs(x1 - x0), dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int e = dx + dy;
    uint16_t pattern = rp->LinePtrn;
    int count = rp->linpatcnt & 15;
    for (;;) {
        bool on = pattern & (0x8000 >> count);
        count = (count + 1) & 15;
        p.dot(x0 + p.ox, y0 + p.oy, on);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * e;
        if (e2 >= dy) {
            e += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            e += dx;
            y0 += sy;
        }
    }
    rp->linpatcnt = int8_t(count);
}

// A horizontal span filled with the area pattern
void span(const Painter &p, int x0, int x1, int y)
{
    RastPort *rp = p.rp;
    if (x0 > x1) {
        int t = x0;
        x0 = x1;
        x1 = t;
    }
    const UWORD *pattern = rp->AreaPtrn;
    int size = rp->AreaPtSz;
    bool multi = size < 0;
    int rows = 1 << (size < 0 ? -size : size);
    for (int x = x0; x <= x1; x++) {
        int bx = x + p.ox, by = y + p.oy;
        if (!pattern) {
            p.dot(bx, by, true);
            continue;
        }
        // (the patterns are in AMOS's memory: big-endian)
        const uint8_t *row = reinterpret_cast<const uint8_t *>(pattern) + 2 * ((by & (rows - 1)));
        uint16_t w = uint16_t(row[0] << 8 | row[1]);
        bool on = w & (0x8000 >> (bx & 15));
        if (!multi) {
            p.dot(bx, by, on);
            continue;
        }
        // a pattern of colours: a pattern per plane
        int pen = 0;
        const BitMap *bm = rp->BitMap;
        for (int pl = 0; pl < bm->Depth; pl++) {
            const uint8_t *r = reinterpret_cast<const uint8_t *>(pattern) + 2 * (pl * rows + (by & (rows - 1)));
            if (uint16_t(r[0] << 8 | r[1]) & (0x8000 >> (bx & 15)))
                pen |= 1 << pl;
        }
        p.put(bx, by, uint32_t(pen), (rp->DrawMode & COMPLEMENT) != 0);
    }
}

void fill_rect(RastPort *rp, int x0, int y0, int x1, int y1)
{
    Painter p(rp);
    for (int y = y0; y <= y1; y++)
        span(p, x0, x1, y);
}

// A polygon filled (even-odd, as the blitter's area fill)
void fill_polygon(RastPort *rp, const int *xs, const int *ys, int n)
{
    if (n < 3)
        return;
    int ymin = ys[0], ymax = ys[0];
    for (int i = 1; i < n; i++) {
        if (ys[i] < ymin)
            ymin = ys[i];
        if (ys[i] > ymax)
            ymax = ys[i];
    }
    Painter p(rp);
    int *cross = static_cast<int *>(malloc(sizeof(int) * size_t(n)));
    for (int y = ymin; y <= ymax; y++) {
        int c = 0;
        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            int ya = ys[i], yb = ys[j];
            if (ya == yb)
                continue;
            int xa = xs[i], xb = xs[j];
            if (ya > yb) {
                int t = ya;
                ya = yb;
                yb = t;
                t = xa;
                xa = xb;
                xb = t;
            }
            if (y < ya || y >= yb)
                continue;
            cross[c++] = xa + (y - ya) * (xb - xa) / (yb - ya);
        }
        for (int i = 1; i < c; i++)
            for (int k = i; k > 0 && cross[k - 1] > cross[k]; k--) {
                int t = cross[k];
                cross[k] = cross[k - 1];
                cross[k - 1] = t;
            }
        for (int i = 0; i + 1 < c; i += 2)
            span(p, cross[i], cross[i + 1], y);
        // the bottom vertices (the edges stop above them)
        for (int i = 0; i < n; i++) {
            int prev = ys[(i + n - 1) % n], next = ys[(i + 1) % n];
            if (ys[i] == y && prev < y && next < y)
                span(p, xs[i], xs[i], y);
        }
    }
    free(cross);
}

// ---- text ----------------------------------------------------------------------

// The glyph of character c: its bits' offset and width, its spacing and kerning
struct Glyph {
    int offset, width, space, kern;
};

Glyph glyph(const TextFont *f, uint8_t c)
{
    if (c < f->tf_LoChar || c > f->tf_HiChar)
        c = uint8_t(f->tf_HiChar + 1);          // the "unknown" glyph after the last
    int i = c - f->tf_LoChar;
    const ULONG *loc = static_cast<const ULONG *>(f->tf_CharLoc);
    Glyph g = {int(loc[i] >> 16), int(loc[i] & 0xFFFF), f->tf_XSize, 0};
    if (f->tf_CharSpace)
        g.space = static_cast<const WORD *>(f->tf_CharSpace)[i];
    if (f->tf_CharKern)
        g.kern = static_cast<const WORD *>(f->tf_CharKern)[i];
    return g;
}

void draw_text(RastPort *rp, const uint8_t *s, uint32_t n)
{
    const TextFont *f = rp->Font;
    if (!f)
        return;
    Painter p(rp);
    int x = rp->cp_x, top = rp->cp_y - f->tf_Baseline;
    const uint8_t *data = static_cast<const uint8_t *>(f->tf_CharData);
    bool bold = rp->AlgoStyle & FSF_BOLD, underlined = rp->AlgoStyle & FSF_UNDERLINED;
    for (uint32_t i = 0; i < n; i++) {
        Glyph g = glyph(f, s[i]);
        int gx = x + g.kern;
        int advance = g.space + rp->TxSpacing;
        int cell = advance > g.width ? advance : g.width;
        for (int row = 0; row < f->tf_YSize; row++) {
            const uint8_t *bits = data + row * f->tf_Modulo;
            bool prev = false;
            for (int col = 0; col < cell; col++) {
                bool on = false;
                if (col < g.width) {
                    int b = g.offset + col;
                    on = bits[b >> 3] & (0x80 >> (b & 7));
                }
                bool shown = on || (bold && prev);
                prev = on;
                if (underlined && row == f->tf_Baseline + 1)
                    shown = true;
                // JAM2 fills the cell, JAM1 only the glyph
                if (shown || (rp->DrawMode & (JAM2 | INVERSVID)))
                    p.dot(gx + col + p.ox, top + row + p.oy, shown);
            }
        }
        x += advance;
    }
    rp->cp_x = int16_t(x);
}

} // namespace

namespace amos::hal {

void set_a_pen(RastPort *rp, uint32_t pen) { rp->FgPen = int8_t(pen); }
void set_b_pen(RastPort *rp, uint32_t pen) { rp->BgPen = int8_t(pen); }
void set_draw_mode(RastPort *rp, uint32_t mode) { rp->DrawMode = int8_t(mode); }

void move(RastPort *rp, int32_t x, int32_t y)
{
    rp->cp_x = int16_t(x);
    rp->cp_y = int16_t(y);
    rp->linpatcnt = 0;
}

void draw(RastPort *rp, int32_t x, int32_t y)
{
    line(rp, rp->cp_x, rp->cp_y, x, y);
    rp->cp_x = int16_t(x);
    rp->cp_y = int16_t(y);
}

// (points: words of AMOS's memory, big-endian)
void poly_draw(RastPort *rp, int32_t count, const int16_t *points)
{
    const uint8_t *p = reinterpret_cast<const uint8_t *>(points);
    for (int32_t i = 0; i < count; i++, p += 4)
        draw(rp, int16_t(p[0] << 8 | p[1]), int16_t(p[2] << 8 | p[3]));
}

void draw_ellipse(RastPort *rp, int32_t cx, int32_t cy, int32_t a, int32_t b)
{
    Painter p(rp);
    auto plot4 = [&](int x, int y) {
        p.dot(cx + x + p.ox, cy + y + p.oy, true);
        if (x)
            p.dot(cx - x + p.ox, cy + y + p.oy, true);
        if (y) {
            p.dot(cx + x + p.ox, cy - y + p.oy, true);
            if (x)
                p.dot(cx - x + p.ox, cy - y + p.oy, true);
        }
    };
    if (a <= 0 || b <= 0) {
        line(rp, cx - a, cy - b, cx + a, cy + b);
        return;
    }
    // midpoint, region 1 then 2
    int64_t a2 = int64_t(a) * a, b2 = int64_t(b) * b;
    int64_t x = 0, y = b;
    int64_t d = b2 - a2 * b + a2 / 4;
    while (b2 * x <= a2 * y) {
        plot4(int(x), int(y));
        if (d < 0) {
            d += b2 * (2 * x + 3);
        } else {
            d += b2 * (2 * x + 3) + a2 * (-2 * y + 2);
            y--;
        }
        x++;
    }
    d = b2 * (2 * x + 1) * (2 * x + 1) / 4 + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        plot4(int(x), int(y));
        if (d > 0) {
            d += a2 * (-2 * y + 3);
        } else {
            d += b2 * (2 * x + 2) + a2 * (-2 * y + 3);
            x++;
        }
        y--;
    }
}

void rect_fill(RastPort *rp, int32_t x1, int32_t y1, int32_t x2, int32_t y2) { fill_rect(rp, x1, y1, x2, y2); }

void write_pixel(RastPort *rp, int32_t x, int32_t y)
{
    Painter p(rp);
    p.dot(x + p.ox, y + p.oy, true);
}

int32_t read_pixel(RastPort *rp, int32_t x, int32_t y)
{
    Painter p(rp);
    return p.get(x + p.ox, y + p.oy);
}

// Filled polygons: the vertices into the AreaInfo's table (native words)
void init_area(AreaInfo *area, void *buffer, int32_t vertices)
{
    area->VctrTbl = area->VctrPtr = static_cast<WORD *>(buffer);
    area->FlagTbl = area->FlagPtr = reinterpret_cast<BYTE *>(static_cast<WORD *>(buffer) + 2 * vertices);
    area->Count = 0;
    area->MaxCount = int16_t(vertices);
    area->FirstX = area->FirstY = 0;
}

void area_move(RastPort *rp, int32_t x, int32_t y)
{
    AreaInfo *a = rp->AreaInfo;
    if (!a || a->Count >= a->MaxCount)
        return;
    a->VctrTbl[2 * a->Count] = int16_t(x);
    a->VctrTbl[2 * a->Count + 1] = int16_t(y);
    a->FlagTbl[a->Count] = 1;           // a polygon starts
    a->Count++;
    a->FirstX = int16_t(x);
    a->FirstY = int16_t(y);
}

void area_draw(RastPort *rp, int32_t x, int32_t y)
{
    AreaInfo *a = rp->AreaInfo;
    if (!a || a->Count >= a->MaxCount)
        return;
    a->VctrTbl[2 * a->Count] = int16_t(x);
    a->VctrTbl[2 * a->Count + 1] = int16_t(y);
    a->FlagTbl[a->Count] = 0;
    a->Count++;
}

void area_end(RastPort *rp)
{
    AreaInfo *a = rp->AreaInfo;
    if (!a)
        return;
    // each polygon from its AreaMove; then the outline if asked
    int start = 0;
    while (start < a->Count) {
        int end = start + 1;
        while (end < a->Count && !a->FlagTbl[end])
            end++;
        int n = end - start;
        int *xs = static_cast<int *>(malloc(sizeof(int) * size_t(n))), *ys = static_cast<int *>(malloc(sizeof(int) * size_t(n)));
        for (int i = 0; i < n; i++) {
            xs[i] = a->VctrTbl[2 * (start + i)];
            ys[i] = a->VctrTbl[2 * (start + i) + 1];
        }
        fill_polygon(rp, xs, ys, n);
        if (rp->Flags & AREAOUTLINE) {
            int8_t pen = rp->FgPen;
            rp->FgPen = rp->AOlPen;
            uint16_t pattern = rp->LinePtrn;
            rp->LinePtrn = 0xFFFF;
            for (int i = 0; i < n; i++)
                line(rp, xs[i], ys[i], xs[(i + 1) % n], ys[(i + 1) % n]);
            rp->LinePtrn = pattern;
            rp->FgPen = pen;
        }
        free(xs);
        free(ys);
        start = end;
    }
    a->Count = 0;
    a->VctrPtr = a->VctrTbl;
    a->FlagPtr = a->FlagTbl;
}

void init_tmp_ras(TmpRas *t, void *buffer, int32_t length)
{
    t->RasPtr = static_cast<BYTE *>(buffer);
    t->Size = length;
}

void text(RastPort *rp, const char *s, uint32_t length) { draw_text(rp, reinterpret_cast<const uint8_t *>(s), length); }

uint32_t text_length(RastPort *rp, const char *s, uint32_t length)
{
    const TextFont *f = rp->Font;
    if (!f)
        return 0;
    uint32_t w = 0;
    for (uint32_t i = 0; i < length; i++)
        w += uint32_t(glyph(f, uint8_t(s[i])).space + rp->TxSpacing);
    return w;
}

void set_font(RastPort *rp, TextFont *font)
{
    if (!font)
        return;
    rp->Font = font;
    rp->TxHeight = font->tf_YSize;
    rp->TxWidth = font->tf_XSize;
    rp->TxBaseline = font->tf_Baseline;
}

TextFont *open_font(TextAttr *attr, bool)
{
    return host::font_open(reinterpret_cast<const char *>(attr->ta_Name), attr->ta_YSize);
}

void close_font(TextFont *font) { host::font_close(font); }

void init_bitmap(BitMap *bm, int32_t planes, int32_t width, int32_t height)
{
    memset(bm, 0, sizeof *bm);
    bm->BytesPerRow = uint16_t(((width + 15) >> 4) << 1);
    bm->Rows = uint16_t(height);
    bm->Depth = uint8_t(planes);
}

// BltBitMap: the rectangle with the minterm, for the planes of mask
int32_t blit_bitmap(BitMap *from, int32_t x1, int32_t y1, BitMap *to, int32_t x2, int32_t y2, int32_t width,
                    int32_t height, uint32_t minterm, uint32_t mask, void *)
{
    if (width <= 0 || height <= 0)
        return 0;
    int planes = from->Depth < to->Depth ? from->Depth : to->Depth;
    int n = 0;
    // overlapping: from the end
    bool back = from == to && (y2 > y1 || (y2 == y1 && x2 > x1));
    for (int p = 0; p < planes; p++) {
        if (!(mask & (1 << p)) || !from->Planes[p] || !to->Planes[p])
            continue;
        n++;
        for (int j = 0; j < height; j++) {
            int y = back ? height - 1 - j : j;
            int sy = y1 + y, dy = y2 + y;
            if (sy < 0 || dy < 0 || sy >= from->Rows || dy >= to->Rows)
                continue;
            for (int i = 0; i < width; i++) {
                int x = back ? width - 1 - i : i;
                int sx = x1 + x, dx = x2 + x;
                if (sx < 0 || dx < 0 || sx >= from->BytesPerRow * 8 || dx >= to->BytesPerRow * 8)
                    continue;
                bool b = from->Planes[p][sy * from->BytesPerRow + (sx >> 3)] & (0x80 >> (sx & 7));
                uint8_t &d = to->Planes[p][dy * to->BytesPerRow + (dx >> 3)];
                bool c = d & (0x80 >> (dx & 7));
                // the minterm's ABC: A is all ones (the mask), B the source, C the destination
                int index = 4 | (b ? 2 : 0) | (c ? 1 : 0);
                bool r = (minterm >> 4) & (1 << index);
                if (r)
                    d |= uint8_t(0x80 >> (dx & 7));
                else
                    d &= uint8_t(~(0x80 >> (dx & 7)));
            }
        }
    }
    return n;
}

ColorMap *get_color_map(int32_t entries)
{
    ColorMap *cm = low_new<ColorMap>();
    if (cm) {
        cm->Count = uint16_t(entries);
        cm->ColorTable = hal::allocate(uint32_t(entries) * 2, 0x10000);
    }
    return cm;
}

void free_color_map(ColorMap *map)
{
    if (!map)
        return;
    hal::release(map->ColorTable, uint32_t(map->Count) * 2);
    low_delete(map);
}

void set_rgb4_cm(ColorMap *map, int32_t n, uint32_t r, uint32_t g, uint32_t b)
{
    if (map && n >= 0 && n < map->Count)
        static_cast<UWORD *>(map->ColorTable)[n] = uint16_t((r & 15) << 8 | (g & 15) << 4 | (b & 15));
}

Region *new_region() { return low_new<Region>(); }

bool or_rect_region(Region *region, Rectangle *rect)
{
    RegionRectangle *rr = low_new<RegionRectangle>();
    if (!rr)
        return false;
    // the region's rectangles relative to its bounds' corner (0, 0 here)
    rr->bounds = *rect;
    rr->Next = region->RegionRectangle;
    if (rr->Next)
        rr->Next->Prev = rr;
    region->RegionRectangle = rr;
    if (!rr->Next) {
        region->bounds = *rect;
    } else {
        if (rect->MinX < region->bounds.MinX)
            region->bounds.MinX = rect->MinX;
        if (rect->MinY < region->bounds.MinY)
            region->bounds.MinY = rect->MinY;
        if (rect->MaxX > region->bounds.MaxX)
            region->bounds.MaxX = rect->MaxX;
        if (rect->MaxY > region->bounds.MaxY)
            region->bounds.MaxY = rect->MaxY;
    }
    return true;
}

void dispose_region(Region *region)
{
    if (!region)
        return;
    for (RegionRectangle *rr = region->RegionRectangle; rr;) {
        RegionRectangle *next = rr->Next;
        low_delete(rr);
        rr = next;
    }
    low_delete(region);
}

Layer_Info *new_layer_info() { return low_new<Layer_Info>(); }
void dispose_layer_info(Layer_Info *info) { low_delete(info); }

Layer *create_upfront_layer(Layer_Info *info, BitMap *bm, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                            uint32_t flags, BitMap *super)
{
    Layer *l = low_new<Layer>();
    RastPort *rp = low_new<RastPort>();
    if (!l || !rp)
        return nullptr;
    l->bounds = {int16_t(x0), int16_t(y0), int16_t(x1), int16_t(y1)};
    l->Flags = uint16_t(flags);
    l->SuperBitMap = super;
    l->LayerInfo = info;
    l->rp = rp;
    l->Width = int16_t(x1 - x0 + 1);
    // one ClipRect: the whole layer (AMOS sets its bounds for its clipping)
    ClipRect *cr = low_new<ClipRect>();
    if (!cr)
        return nullptr;
    cr->bounds = l->bounds;
    l->ClipRect = cr;
    l->Height = int16_t(y1 - y0 + 1);
    // InitRastPort's values
    rp->Layer = l;
    rp->BitMap = bm;
    rp->Mask = 0xFF;
    rp->FgPen = -1;
    rp->AOlPen = -1;
    rp->DrawMode = JAM2;
    rp->LinePtrn = 0xFFFF;
    rp->Flags = FRST_DOT;
    return l;
}

void delete_layer(Layer *layer)
{
    if (!layer)
        return;
    low_delete(layer->ClipRect);
    low_delete(layer->rp);
    low_delete(layer);
}

// The new region; the old one
Region *install_clip_region(Layer *layer, Region *region)
{
    Region *old = layer->ClipRegion;
    layer->ClipRegion = region;
    return old;
}

// AvailFonts: the fonts of the ROM (topaz 8 and 9, in memory), as the
// Amiga's list (big-endian): their number, then for each its type and
// its TextAttr (name, height, style, flags); the bytes missing
int32_t avail_fonts(char *buffer, int32_t size, uint32_t types)
{
    static const char topaz[] = "topaz.font";
    int n = types & AFF_MEMORY ? 2 : 0;
    int32_t need = 2 + 10 * n;
    if (size < need)
        return need - size;
    uint8_t *p = reinterpret_cast<uint8_t *>(buffer);
    poke_w(p, uint16_t(n));
    for (int i = 0; i < n; i++) {
        uint8_t *e = p + 2 + 10 * i;
        poke_w(e, AFF_MEMORY);
        poke_l(e + 2, address(topaz));
        poke_w(e + 6, uint16_t(8 + i));
        e[8] = 0;
        e[9] = FPF_ROMFONT;
    }
    return 0;
}
void own_blitter() {}
void disown_blitter() {}
void wait_blit() {}
void load_view(View *) {}

// GfxFunc's functions: the registers of graphics.library's calls
void gfx_call(int16_t lvo, AllRegs &r)
{
    RastPort *rp = ptr<RastPort>(r.a[1]);
    int32_t d0 = int32_t(r.d[0]), d1 = int32_t(r.d[1]), d2 = int32_t(r.d[2]), d3 = int32_t(r.d[3]);
    switch (lvo) {
    case -60:   // Text(rp, a0 string, d0 count)
        text(rp, ptr<char>(r.a[0]), uint32_t(uint16_t(d0)));
        return;
    case -180:  // DrawEllipse(rp, d0 cx, d1 cy, d2 a, d3 b)
        draw_ellipse(rp, int16_t(d0), int16_t(d1), int16_t(d2), int16_t(d3));
        return;
    case -240:  // Move
        move(rp, int16_t(d0), int16_t(d1));
        return;
    case -246:  // Draw(rp, d0, d1)
        draw(rp, int16_t(d0), int16_t(d1));
        return;
    case -306:  // RectFill(rp, d0..d3)
        rect_fill(rp, int16_t(d0), int16_t(d1), int16_t(d2), int16_t(d3));
        return;
    case -324:  // WritePixel(rp, d0, d1)
        write_pixel(rp, int16_t(d0), int16_t(d1));
        r.d[0] = 0;
        return;
    case -336:  // PolyDraw(rp, d0 count, a0 points)
        poly_draw(rp, int16_t(d0), ptr<int16_t>(r.a[0]));
        return;
    }
    host::missing("graphics.library function", reinterpret_cast<const void *>(intptr_t(lvo)));
}

uint32_t system_copper() { return 0; }
uint16_t system_bplcon0() { return 0x0200; }

} // namespace amos::hal
