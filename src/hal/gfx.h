// The drawing of the host system (Phase 7: the portability layer):
// graphics.library's RastPort operations, as AMOS uses them, its fonts,
// the blitter's ownership and the display's frames. The structures are
// graphics.library's (a host provides its own); hal/gfx_amiga.cpp is
// graphics.library and diskfont.library.
#pragma once

#include <stdint.h>

namespace amos {
struct AllRegs;
}

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
struct Layer;
struct Layer_Info;

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

// Layers (layers.library): a screen's, which its clipping region needs
Layer_Info *new_layer_info();
void dispose_layer_info(Layer_Info *info);
Layer *create_upfront_layer(Layer_Info *info, BitMap *bm, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                            uint32_t flags, BitMap *super);
void delete_layer(Layer *layer);
Region *install_clip_region(Layer *layer, Region *region);

// The fonts there are (AvailFonts: into buffer; the bytes missing)
int32_t avail_fonts(char *buffer, int32_t size, uint32_t types);

// A function of graphics.library by its offset, with the registers of its
// call (a1 the RastPort, the others the function's: WritePixel, Draw,
// DrawEllipse, PolyDraw, Text, RectFill); a6 graphics.library
void gfx_call(int16_t lvo, amos::AllRegs &r);

// The system's view: its copper list, its BPLCON0 bits
uint32_t system_copper();
uint16_t system_bplcon0();

// The blitter, the display
void own_blitter();
void disown_blitter();
void wait_blit();
void wait_frame();          // WaitTOF
void load_view(View *view);

} // namespace amos::hal
