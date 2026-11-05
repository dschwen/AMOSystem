// src_editor_cursor_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_cursor_glue_s[120] = {
};
}

namespace amos::host {

void relocate_src_editor_cursor_glue_s()
{
}

const GlueEntry entries_src_editor_cursor_glue_s[] = {
    {host_obj_src_editor_cursor_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_cursor_up), nullptr, 0, 4, "ed_cursor_up_entry"},
    {host_obj_src_editor_cursor_glue_s + 4, GlueKind::w, AMOS_BIND_W(ed_cursor_up_t), nullptr, 0, 4, "ed_cursor_up_t_entry"},
    {host_obj_src_editor_cursor_glue_s + 8, GlueKind::w, AMOS_BIND_W(ed_cursor_down), nullptr, 0, 4, "ed_cursor_down_entry"},
    {host_obj_src_editor_cursor_glue_s + 12, GlueKind::w, AMOS_BIND_W(ed_cursor_down_t), nullptr, 0, 4, "ed_cursor_down_t_entry"},
    {host_obj_src_editor_cursor_glue_s + 16, GlueKind::w, AMOS_BIND_W(ed_cursor_left), nullptr, 0, 4, "ed_cursor_left_entry"},
    {host_obj_src_editor_cursor_glue_s + 20, GlueKind::w, AMOS_BIND_W(ed_cursor_right), nullptr, 0, 4, "ed_cursor_right_entry"},
    {host_obj_src_editor_cursor_glue_s + 24, GlueKind::w, AMOS_BIND_W(ed_page_top), nullptr, 0, 4, "ed_page_top_entry"},
    {host_obj_src_editor_cursor_glue_s + 28, GlueKind::w, AMOS_BIND_W(ed_page_bottom), nullptr, 0, 4, "ed_page_bottom_entry"},
    {host_obj_src_editor_cursor_glue_s + 32, GlueKind::w, AMOS_BIND_W(ed_word_left_key), nullptr, 0, 4, "ed_word_left_key_entry"},
    {host_obj_src_editor_cursor_glue_s + 36, GlueKind::w, AMOS_BIND_W(ed_word_right_key), nullptr, 0, 4, "ed_word_right_key_entry"},
    {host_obj_src_editor_cursor_glue_s + 40, GlueKind::w, AMOS_BIND_W(ed_line_start), nullptr, 0, 4, "ed_line_start_entry"},
    {host_obj_src_editor_cursor_glue_s + 44, GlueKind::w, AMOS_BIND_W(ed_line_end), nullptr, 0, 4, "ed_line_end_entry"},
    {host_obj_src_editor_cursor_glue_s + 48, GlueKind::w, AMOS_BIND_W(ed_page_up), nullptr, 0, 4, "ed_page_up_entry"},
    {host_obj_src_editor_cursor_glue_s + 52, GlueKind::w, AMOS_BIND_W(ed_page_down), nullptr, 0, 4, "ed_page_down_entry"},
    {host_obj_src_editor_cursor_glue_s + 56, GlueKind::w, AMOS_BIND_W(ed_scroll_up_key), nullptr, 0, 4, "ed_scroll_up_key_entry"},
    {host_obj_src_editor_cursor_glue_s + 60, GlueKind::w, AMOS_BIND_W(ed_scroll_down_key), nullptr, 0, 4, "ed_scroll_down_key_entry"},
    {host_obj_src_editor_cursor_glue_s + 64, GlueKind::w, AMOS_BIND_W(ed_scroll_left_key), nullptr, 0, 4, "ed_scroll_left_key_entry"},
    {host_obj_src_editor_cursor_glue_s + 68, GlueKind::w, AMOS_BIND_W(ed_scroll_right_key), nullptr, 0, 4, "ed_scroll_right_key_entry"},
    {host_obj_src_editor_cursor_glue_s + 72, GlueKind::w, AMOS_BIND_W(ed_text_top), nullptr, 0, 4, "ed_text_top_entry"},
    {host_obj_src_editor_cursor_glue_s + 76, GlueKind::w, AMOS_BIND_W(ed_text_top_t), nullptr, 0, 4, "ed_text_top_t_entry"},
    {host_obj_src_editor_cursor_glue_s + 80, GlueKind::w, AMOS_BIND_W(ed_text_bottom), nullptr, 0, 4, "ed_text_bottom_entry"},
    {host_obj_src_editor_cursor_glue_s + 84, GlueKind::w, AMOS_BIND_W(ed_text_bottom_t), nullptr, 0, 4, "ed_text_bottom_t_entry"},
    {host_obj_src_editor_cursor_glue_s + 88, GlueKind::w, AMOS_BIND_W(ed_label_previous), nullptr, 0, 4, "ed_label_previous_entry"},
    {host_obj_src_editor_cursor_glue_s + 92, GlueKind::w, AMOS_BIND_W(ed_label_next), nullptr, 0, 4, "ed_label_next_entry"},
    {host_obj_src_editor_cursor_glue_s + 96, GlueKind::w, AMOS_BIND_W(ed_buf_untok_fn), nullptr, 0, 4, "ed_buf_untok_entry"},
    {host_obj_src_editor_cursor_glue_s + 100, GlueKind::w, AMOS_BIND_W(ed_untok), nullptr, 0, 4, "ed_untok_entry"},
    {host_obj_src_editor_cursor_glue_s + 104, GlueKind::w, AMOS_BIND_W(ed_scroll_right), nullptr, 0, 4, "ed_cgo_entry"},
    {host_obj_src_editor_cursor_glue_s + 108, GlueKind::w, AMOS_BIND_W(ed_scroll_left), nullptr, 0, 4, "ed_cdr_entry"},
    {host_obj_src_editor_cursor_glue_s + 112, GlueKind::w, AMOS_BIND_W(ed_window_up), nullptr, 0, 4, "ed_shaut2_entry"},
    {host_obj_src_editor_cursor_glue_s + 116, GlueKind::w, AMOS_BIND_W(ed_window_down), nullptr, 0, 4, "ed_sbas2_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
