// The drawing of the host system (Phase 7: the portability layer):
// graphics.library's RastPort operations, as AMOS uses them, its fonts,
// the blitter's ownership and the display's frames. The structures are
// graphics.library's (a host provides its own); hal/gfx_amiga.cpp is
// graphics.library and diskfont.library.
#pragma once

#include <stdint.h>

struct RastPort;
struct TextFont;
struct TextAttr;
struct BitMap;
struct AreaInfo;
struct TmpRas;
struct ColorMap;
struct View;
struct Region;
struct Rectangle;

namespace amos::hal {

// The pens and the drawing mode (JAM1, JAM2, COMPLEMENT, INVERSVID)
void set_a_pen(RastPort *rp, uint32_t pen);
void set_b_pen(RastPort *rp, uint32_t pen);
void set_draw_mode(RastPort *rp, uint32_t mode);

void move(RastPort *rp, int32_t x, int32_t y);
void draw(RastPort *rp, int32_t x, int32_t y);
void poly_draw(RastPort *rp, int32_t count, const int16_t *points);
void draw_ellipse(RastPort *rp, int32_t x, int32_t y, int32_t rx, int32_t ry);
void rect_fill(RastPort *rp, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
void write_pixel(RastPort *rp, int32_t x, int32_t y);
int32_t read_pixel(RastPort *rp, int32_t x, int32_t y);

// Filled polygons: their vertices buffer and the work raster
void init_area(AreaInfo *area, void *buffer, int32_t vertices);
void area_move(RastPort *rp, int32_t x, int32_t y);
void area_draw(RastPort *rp, int32_t x, int32_t y);
void area_end(RastPort *rp);
void init_tmp_ras(TmpRas *t, void *buffer, int32_t length);

// Text and fonts (disk: from diskfont.library)
void text(RastPort *rp, const char *s, uint32_t length);
uint32_t text_length(RastPort *rp, const char *s, uint32_t length);
void set_font(RastPort *rp, TextFont *font);
TextFont *open_font(TextAttr *attr, bool disk);
void close_font(TextFont *font);

// Bitmaps
void init_bitmap(BitMap *bm, int32_t planes, int32_t width, int32_t height);
int32_t blit_bitmap(BitMap *from, int32_t x1, int32_t y1, BitMap *to, int32_t x2, int32_t y2, int32_t width,
                    int32_t height, uint32_t minterm, uint32_t mask, void *buffer);
ColorMap *get_color_map(int32_t entries);
void free_color_map(ColorMap *map);
void set_rgb4_cm(ColorMap *map, int32_t n, uint32_t r, uint32_t g, uint32_t b);

// Clipping regions
Region *new_region();
bool or_rect_region(Region *region, Rectangle *rect);
void dispose_region(Region *region);

// The blitter, the display
void own_blitter();
void disown_blitter();
void wait_blit();
void wait_frame();          // WaitTOF
void load_view(View *view);

} // namespace amos::hal
