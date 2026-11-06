// src_editor_display_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_display_glue_s[72] = {
};
}

namespace amos::host {

void relocate_src_editor_display_glue_s()
{
}

const GlueEntry entries_src_editor_display_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_show_slider), nullptr, 0, 4, "ed_show_slider_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_cursor_on), nullptr, 0, 4, "ed_cursor_on_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_cursor_off), nullptr, 0, 4, "ed_cursor_off_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_show_current_line), nullptr, 0, 4, "ed_show_current_line_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_centre), nullptr, 0, 4, "ed_centre_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_goto_x_fn), nullptr, 0, 4, "ed_goto_x_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_goto_y_fn), nullptr, 0, 4, "ed_goto_y_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_set_x), nullptr, 0, 4, "ed_set_x_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_set_y), nullptr, 0, 4, "ed_set_y_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_set_xy), nullptr, 0, 4, "ed_set_xy_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_labels_around), nullptr, 0, 4, "ed_labels_around_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 44), GlueKind::w, AMOS_BIND_W(ed_place_cursor), nullptr, 0, 4, "ed_place_cursor_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 48), GlueKind::w, AMOS_BIND_W(ed_show_column), nullptr, 0, 4, "ed_show_column_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 52), GlueKind::w, AMOS_BIND_W(ed_new_buffer_all), nullptr, 0, 4, "ed_new_buffer_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 56), GlueKind::w, AMOS_BIND_W(ed_new_buffer), nullptr, 0, 4, "ed_new_buffer_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 60), GlueKind::w, AMOS_BIND_W(ed_show_all), nullptr, 0, 4, "ed_show_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 64), GlueKind::w, AMOS_BIND_W(ed_show_line), nullptr, 0, 4, "ed_show_line_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_display_glue_s, 68), GlueKind::w, AMOS_BIND_W(ed_show_line_only), nullptr, 0, 4, "ed_show_line_only_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
