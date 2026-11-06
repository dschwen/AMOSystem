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
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_ec_cls), nullptr, 0, 4, "w_cls"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_ec_set_clip_fn), nullptr, 0, 4, "w_set_clip"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_ec_pattern), nullptr, 0, 4, "w_pattern"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_ec_paint), nullptr, 0, 4, "w_paint"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_ec_slider_h), nullptr, 0, 2, "w_slider_h"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_ec_slider_v), nullptr, 0, 2, "w_slider_v"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_ec_slider_set), nullptr, 0, 4, "w_slider_set"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_ec_slider_knob), nullptr, 0, 4, "w_slider_knob"},
    {AMOS_ENTRY_AT(host_obj_src_w_drawing_glue_s, 32), GlueKind::w, AMOS_BIND_W(w_ec_screen_copy), nullptr, 0, 4, "w_screen_copy"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
