// hal/gfx.h on another host: not drawn yet (the renderer of the display
// comes with the backend of the display)

#include "hal/gfx.h"

namespace amos::hal {

void set_a_pen(RastPort *, uint32_t) {}
void set_b_pen(RastPort *, uint32_t) {}
void set_draw_mode(RastPort *, uint32_t) {}
void move(RastPort *, int32_t, int32_t) {}
void draw(RastPort *, int32_t, int32_t) {}
void poly_draw(RastPort *, int32_t, const int16_t *) {}
void draw_ellipse(RastPort *, int32_t, int32_t, int32_t, int32_t) {}
void rect_fill(RastPort *, int32_t, int32_t, int32_t, int32_t) {}
void write_pixel(RastPort *, int32_t, int32_t) {}
int32_t read_pixel(RastPort *, int32_t, int32_t) { return 0; }
void init_area(AreaInfo *, void *, int32_t) {}
void area_move(RastPort *, int32_t, int32_t) {}
void area_draw(RastPort *, int32_t, int32_t) {}
void area_end(RastPort *) {}
void init_tmp_ras(TmpRas *, void *, int32_t) {}
void text(RastPort *, const char *, uint32_t) {}
uint32_t text_length(RastPort *, const char *, uint32_t length) { return length * 8; }
void set_font(RastPort *, TextFont *) {}
TextFont *open_font(TextAttr *, bool) { return nullptr; }
void close_font(TextFont *) {}
void init_bitmap(BitMap *, int32_t, int32_t, int32_t) {}
int32_t blit_bitmap(BitMap *, int32_t, int32_t, BitMap *, int32_t, int32_t, int32_t, int32_t, uint32_t, uint32_t, void *)
{
    return 0;
}
ColorMap *get_color_map(int32_t) { return nullptr; }
void free_color_map(ColorMap *) {}
void set_rgb4_cm(ColorMap *, int32_t, uint32_t, uint32_t, uint32_t) {}
Region *new_region() { return nullptr; }
bool or_rect_region(Region *, Rectangle *) { return false; }
void dispose_region(Region *) {}
Layer_Info *new_layer_info() { return nullptr; }
void dispose_layer_info(Layer_Info *) {}
Layer *create_upfront_layer(Layer_Info *, BitMap *, int32_t, int32_t, int32_t, int32_t, uint32_t, BitMap *)
{
    return nullptr;
}
void delete_layer(Layer *) {}
Region *install_clip_region(Layer *, Region *) { return nullptr; }
int32_t avail_fonts(char *, int32_t, uint32_t) { return 0; }
void own_blitter() {}
void disown_blitter() {}
void wait_blit() {}
void wait_frame() {}
void load_view(View *) {}

} // namespace amos::hal
