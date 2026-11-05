// src_editor_direct_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_direct_glue_s[52] = {
};
}

namespace amos::host {

void relocate_src_editor_direct_glue_s()
{
}

const GlueEntry entries_src_editor_direct_glue_s[] = {
    {host_obj_src_editor_direct_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_escape), nullptr, 0, 4, "ed_escape_entry"},
    {host_obj_src_editor_direct_glue_s + 4, GlueKind::w, AMOS_BIND_W(ed_escape_loop), nullptr, 0, 4, "ed_escape_loop_entry"},
    {host_obj_src_editor_direct_glue_s + 8, GlueKind::w, AMOS_BIND_W(ed_escape_leave), nullptr, 0, 4, "ed_escape_leave_entry"},
    {host_obj_src_editor_direct_glue_s + 12, GlueKind::w, AMOS_BIND_W(ed_escape_forget), nullptr, 0, 4, "ed_escape_forget_entry"},
    {host_obj_src_editor_direct_glue_s + 16, GlueKind::w, AMOS_BIND_W(ed_escape_forget_all), nullptr, 0, 4, "ed_escape_forget_all_entry"},
    {host_obj_src_editor_direct_glue_s + 20, GlueKind::w, AMOS_BIND_W(ed_error_direct), nullptr, 0, 4, "ed_error_direct_entry"},
    {host_obj_src_editor_direct_glue_s + 24, GlueKind::w, AMOS_BIND_W(ed_escape_appear), nullptr, 0, 4, "ed_escape_appear_entry"},
    {host_obj_src_editor_direct_glue_s + 28, GlueKind::w, AMOS_BIND_W(ed_escape_max_mouse), nullptr, 0, 4, "ed_escape_max_mouse_entry"},
    {host_obj_src_editor_direct_glue_s + 32, GlueKind::w, AMOS_BIND_W(ed_escape_hide), nullptr, 0, 4, "ed_escape_hide_entry"},
    {host_obj_src_editor_direct_glue_s + 36, GlueKind::w, AMOS_BIND_W(ed_escape_display), nullptr, 0, 4, "ed_escape_display_entry"},
    {host_obj_src_editor_direct_glue_s + 40, GlueKind::w, AMOS_BIND_W(ed_escape_display_d0), nullptr, 0, 4, "ed_escape_display_d0_entry"},
    {host_obj_src_editor_direct_glue_s + 44, GlueKind::w, AMOS_BIND_W(ed_get_error), nullptr, 0, 4, "ed_get_error_entry"},
    {host_obj_src_editor_direct_glue_s + 48, GlueKind::w, AMOS_BIND_W(ed_escape_output), nullptr, 0, 4, "ed_escape_output_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
