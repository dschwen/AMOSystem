// src_editor_misc_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_misc_glue_s[48] = {
};
}

namespace amos::host {

void relocate_src_editor_misc_glue_s()
{
}

const GlueEntry entries_src_editor_misc_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_set_tab), nullptr, 0, 4, "ed_set_tab_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_slider_update), nullptr, 0, 4, "ed_slider_update_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_slider_draw), nullptr, 0, 4, "ed_slider_draw_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_close_name), nullptr, 0, 4, "ed_close_name_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_enlarge_fn), nullptr, 0, 4, "ed_enlarge_fn_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_button_draw), nullptr, 0, 4, "ed_button_draw_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_button_draw_title), nullptr, 0, 4, "ed_button_draw_title_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_disk_error), nullptr, 0, 4, "ed_disk_error_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_load_error), nullptr, 0, 4, "ed_load_error_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_print_error_fn), nullptr, 0, 4, "ed_print_error_fn_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_close_fn), nullptr, 0, 4, "ed_close_fn_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_misc_glue_s, 44), GlueKind::w, AMOS_BIND_W(ed_workbench), nullptr, 0, 4, "ed_workbench_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
