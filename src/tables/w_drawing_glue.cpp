// src_w_drawing_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_drawing_glue_s[36] = {
};
}

namespace amos::host {

void relocate_src_w_drawing_glue_s()
{
}

const GlueEntry entries_src_w_drawing_glue_s[] = {
    {host_obj_src_w_drawing_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cls), nullptr, 0, 4, "w_cls"},
    {host_obj_src_w_drawing_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&w_ec_set_clip_fn), nullptr, 0, 4, "w_set_clip"},
    {host_obj_src_w_drawing_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&w_ec_pattern), nullptr, 0, 4, "w_pattern"},
    {host_obj_src_w_drawing_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&w_ec_paint), nullptr, 0, 4, "w_paint"},
    {host_obj_src_w_drawing_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&w_ec_slider_h), nullptr, 0, 2, "w_slider_h"},
    {host_obj_src_w_drawing_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&w_ec_slider_v), nullptr, 0, 2, "w_slider_v"},
    {host_obj_src_w_drawing_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&w_ec_slider_set), nullptr, 0, 4, "w_slider_set"},
    {host_obj_src_w_drawing_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&w_ec_slider_knob), nullptr, 0, 4, "w_slider_knob"},
    {host_obj_src_w_drawing_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&w_ec_screen_copy), nullptr, 0, 4, "w_screen_copy"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
