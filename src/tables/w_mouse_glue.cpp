// src_w_mouse_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_mouse_glue_s[96] = {
};
}

namespace amos::host {

void relocate_src_w_mouse_glue_s()
{
}

const GlueEntry entries_src_w_mouse_glue_s[] = {
    {host_obj_src_w_mouse_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_key), nullptr, 0, 4, "w_mouse_key"},
    {host_obj_src_w_mouse_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_click), nullptr, 0, 4, "w_mouse_click"},
    {host_obj_src_w_mouse_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_change), nullptr, 0, 4, "w_mouse_change"},
    {host_obj_src_w_mouse_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_store), nullptr, 0, 4, "w_mouse_store"},
    {host_obj_src_w_mouse_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_recall), nullptr, 0, 4, "w_mouse_recall"},
    {host_obj_src_w_mouse_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_hide), nullptr, 0, 4, "w_mouse_hide"},
    {host_obj_src_w_mouse_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_show), nullptr, 0, 4, "w_mouse_show"},
    {host_obj_src_w_mouse_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_xy), nullptr, 0, 4, "w_mouse_xy"},
    {host_obj_src_w_mouse_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&w_sy_xy_screen), nullptr, 0, 4, "w_xy_screen"},
    {host_obj_src_w_mouse_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&w_sy_xy_hard), nullptr, 0, 4, "w_xy_hard"},
    {host_obj_src_w_mouse_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&w_sy_xy_window), nullptr, 0, 4, "w_xy_window"},
    {host_obj_src_w_mouse_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&w_sy_screen_at), nullptr, 0, 4, "w_screen_at"},
    {host_obj_src_w_mouse_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_screen), nullptr, 0, 4, "w_mouse_screen"},
    {host_obj_src_w_mouse_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_set), nullptr, 0, 4, "w_mouse_set"},
    {host_obj_src_w_mouse_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_limit), nullptr, 0, 4, "w_mouse_limit"},
    {host_obj_src_w_mouse_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mouse_limit_screen), nullptr, 0, 4, "w_mouse_limit_screen"},
    {host_obj_src_w_mouse_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&w_sy_zones_reserve), nullptr, 0, 4, "w_zones_reserve"},
    {host_obj_src_w_mouse_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&w_sy_zone_reset), nullptr, 0, 4, "w_zone_reset"},
    {host_obj_src_w_mouse_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&w_sy_zone_set), nullptr, 0, 4, "w_sy_set_z"},
    {host_obj_src_w_mouse_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&w_sy_zone_graphic), nullptr, 0, 4, "w_zone_graphic"},
    {host_obj_src_w_mouse_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&w_sy_zone_hard), nullptr, 0, 4, "w_zone_hard"},
    {host_obj_src_w_mouse_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&w_sy_zone_mouse), nullptr, 0, 4, "w_zone_mouse"},
    {host_obj_src_w_mouse_glue_s + 88, GlueKind::w, reinterpret_cast<const void *>(&w_sy_hardcol_set), nullptr, 0, 4, "w_hardcol_set"},
    {host_obj_src_w_mouse_glue_s + 92, GlueKind::w, reinterpret_cast<const void *>(&w_sy_hardcol_get), nullptr, 0, 4, "w_hardcol_get"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
