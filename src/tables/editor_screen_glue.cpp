// src_editor_screen_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_screen_glue_s[40] = {
};
}

namespace amos::host {

void relocate_src_editor_screen_glue_s()
{
}

const GlueEntry entries_src_editor_screen_glue_s[] = {
    {host_obj_src_editor_screen_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_memory_draw), nullptr, 0, 4, "ed_memory_draw_entry"},
    {host_obj_src_editor_screen_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_memory_draw_title), nullptr, 0, 4, "ed_memory_draw_title_entry"},
    {host_obj_src_editor_screen_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_memory_show), nullptr, 0, 4, "ed_memory_show_entry"},
    {host_obj_src_editor_screen_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_unpack_fn), nullptr, 0, 4, "ed_unpack_entry"},
    {host_obj_src_editor_screen_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_unpack_title), nullptr, 0, 4, "ed_unpack_title_entry"},
    {host_obj_src_editor_screen_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_hide_fn), nullptr, 0, 4, "ed_hide_entry"},
    {host_obj_src_editor_screen_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_appear_fn), nullptr, 0, 4, "ed_appear_entry"},
    {host_obj_src_editor_screen_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_redraw), nullptr, 0, 4, "ed_redraw_entry"},
    {host_obj_src_editor_screen_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_limit_mouse), nullptr, 0, 4, "ed_limit_mouse_entry"},
    {host_obj_src_editor_screen_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_recop_fn), nullptr, 0, 4, "ed_recop"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
