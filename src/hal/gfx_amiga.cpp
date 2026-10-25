// hal/gfx.h on the Amiga: graphics.library and diskfont.library

#include "hal/gfx.h"

#include "core/amos.h"
#include "core/amiga.h"

#define GRAPHICS_BASE_NAME (amos::ptr<struct GfxBase>(amos::wdata().GfxBase))
#include <proto/graphics.h>
#define DISKFONT_BASE_NAME (amos::ptr<struct Library>(amos::wdata().FntBase))
#include <proto/diskfont.h>

namespace amos::hal {

void set_a_pen(RastPort *rp, uint32_t pen) { SetAPen(rp, pen); }
void set_b_pen(RastPort *rp, uint32_t pen) { SetBPen(rp, pen); }
void set_draw_mode(RastPort *rp, uint32_t mode) { SetDrMd(rp, mode); }

void move(RastPort *rp, int32_t x, int32_t y) { Move(rp, x, y); }
void draw(RastPort *rp, int32_t x, int32_t y) { Draw(rp, x, y); }
void poly_draw(RastPort *rp, int32_t count, const int16_t *points) { PolyDraw(rp, count, const_cast<int16_t *>(points)); }
void draw_ellipse(RastPort *rp, int32_t x, int32_t y, int32_t rx, int32_t ry) { DrawEllipse(rp, x, y, rx, ry); }
void rect_fill(RastPort *rp, int32_t x1, int32_t y1, int32_t x2, int32_t y2) { RectFill(rp, x1, y1, x2, y2); }
void write_pixel(RastPort *rp, int32_t x, int32_t y) { WritePixel(rp, x, y); }
int32_t read_pixel(RastPort *rp, int32_t x, int32_t y) { return ReadPixel(rp, x, y); }

void init_area(AreaInfo *area, void *buffer, int32_t vertices) { InitArea(area, buffer, vertices); }
void area_move(RastPort *rp, int32_t x, int32_t y) { AreaMove(rp, x, y); }
void area_draw(RastPort *rp, int32_t x, int32_t y) { AreaDraw(rp, x, y); }
void area_end(RastPort *rp) { AreaEnd(rp); }
void init_tmp_ras(TmpRas *t, void *buffer, int32_t length) { InitTmpRas(t, static_cast<BYTE *>(buffer), length); }

void text(RastPort *rp, const char *s, uint32_t length) { Text(rp, s, length); }
uint32_t text_length(RastPort *rp, const char *s, uint32_t length) { return TextLength(rp, s, length); }
void set_font(RastPort *rp, TextFont *font) { SetFont(rp, font); }
TextFont *open_font(TextAttr *attr, bool disk) { return disk ? OpenDiskFont(attr) : OpenFont(attr); }
void close_font(TextFont *font) { CloseFont(font); }

void init_bitmap(BitMap *bm, int32_t planes, int32_t width, int32_t height) { InitBitMap(bm, planes, width, height); }
int32_t blit_bitmap(BitMap *from, int32_t x1, int32_t y1, BitMap *to, int32_t x2, int32_t y2, int32_t width,
                    int32_t height, uint32_t minterm, uint32_t mask, void *buffer)
{
    return BltBitMap(from, x1, y1, to, x2, y2, width, height, minterm, mask, static_cast<PLANEPTR>(buffer));
}
ColorMap *get_color_map(int32_t entries) { return GetColorMap(entries); }
void free_color_map(ColorMap *map) { FreeColorMap(map); }
void set_rgb4_cm(ColorMap *map, int32_t n, uint32_t r, uint32_t g, uint32_t b) { SetRGB4CM(map, n, r, g, b); }

Region *new_region() { return NewRegion(); }
bool or_rect_region(Region *region, Rectangle *rect) { return OrRectRegion(region, rect); }
void dispose_region(Region *region) { DisposeRegion(region); }

void own_blitter() { OwnBlitter(); }
void disown_blitter() { DisownBlitter(); }
void wait_blit() { WaitBlit(); }
void wait_frame() { WaitTOF(); }
void load_view(View *view) { LoadView(view); }

} // namespace amos::hal
