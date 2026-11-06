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
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_memory_draw), nullptr, 0, 4, "ed_memory_draw_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_memory_draw_title), nullptr, 0, 4, "ed_memory_draw_title_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_memory_show), nullptr, 0, 4, "ed_memory_show_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_unpack_fn), nullptr, 0, 4, "ed_unpack_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_unpack_title), nullptr, 0, 4, "ed_unpack_title_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_hide_fn), nullptr, 0, 4, "ed_hide_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_appear_fn), nullptr, 0, 4, "ed_appear_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_redraw), nullptr, 0, 4, "ed_redraw_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_limit_mouse), nullptr, 0, 4, "ed_limit_mouse_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_screen_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_recop_fn), nullptr, 0, 4, "ed_recop"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
