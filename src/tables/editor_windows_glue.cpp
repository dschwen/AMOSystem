// src_editor_windows_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_windows_glue_s[144] = {
};
}

namespace amos::host {

void relocate_src_editor_windows_glue_s()
{
}

const GlueEntry entries_src_editor_windows_glue_s[] = {
    {host_obj_src_editor_windows_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_top_up), nullptr, 0, 4, "ed_top_up_entry"},
    {host_obj_src_editor_windows_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_top_down), nullptr, 0, 4, "ed_top_down_entry"},
    {host_obj_src_editor_windows_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_bottom_up), nullptr, 0, 4, "ed_bottom_up_entry"},
    {host_obj_src_editor_windows_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_bottom_down), nullptr, 0, 4, "ed_bottom_down_entry"},
    {host_obj_src_editor_windows_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_drag_top), nullptr, 0, 4, "ed_drag_top_entry"},
    {host_obj_src_editor_windows_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_drag_bottom), nullptr, 0, 4, "ed_drag_bottom_entry"},
    {host_obj_src_editor_windows_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_link_cursor), nullptr, 0, 4, "ed_link_cursor_entry"},
    {host_obj_src_editor_windows_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_unlink_scroll), nullptr, 0, 4, "ed_unlink_scroll_entry"},
    {host_obj_src_editor_windows_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_follow_scroll), nullptr, 0, 4, "ed_follow_scroll_entry"},
    {host_obj_src_editor_windows_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_split), nullptr, 0, 4, "ed_split_entry"},
    {host_obj_src_editor_windows_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_update_split), nullptr, 0, 4, "ed_update_split_entry"},
    {host_obj_src_editor_windows_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_open_shown_window), nullptr, 0, 4, "ed_open_shown_window_entry"},
    {host_obj_src_editor_windows_glue_s + 48, GlueKind::w_f, reinterpret_cast<const void *>(&ed_open_window), nullptr, 0, 0, "ed_open_window_entry"},
    {host_obj_src_editor_windows_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ed_hide_window), nullptr, 0, 4, "ed_hide_window_entry"},
    {host_obj_src_editor_windows_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ed_button_close), nullptr, 0, 4, "ed_button_close_entry"},
    {host_obj_src_editor_windows_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ed_button_hide), nullptr, 0, 4, "ed_button_hide_entry"},
    {host_obj_src_editor_windows_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ed_button_size), nullptr, 0, 4, "ed_button_size_entry"},
    {host_obj_src_editor_windows_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&ed_close_window_quit), nullptr, 0, 4, "ed_close_window_quit_entry"},
    {host_obj_src_editor_windows_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&ed_close_window), nullptr, 0, 4, "ed_close_window_entry"},
    {host_obj_src_editor_windows_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&ed_new_hidden), nullptr, 0, 4, "ed_new_hidden_entry"},
    {host_obj_src_editor_windows_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&ed_new_all_hidden), nullptr, 0, 4, "ed_new_all_hidden_entry"},
    {host_obj_src_editor_windows_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&ed_edit_hidden), nullptr, 0, 4, "ed_edit_hidden_entry"},
    {host_obj_src_editor_windows_glue_s + 88, GlueKind::w, reinterpret_cast<const void *>(&ed_get_hidden), nullptr, 0, 4, "ed_get_hidden_entry"},
    {host_obj_src_editor_windows_glue_s + 92, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_window), nullptr, 0, 4, "ed_delete_window_entry"},
    {host_obj_src_editor_windows_glue_s + 96, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_windows), nullptr, 0, 4, "ed_delete_windows_entry"},
    {host_obj_src_editor_windows_glue_s + 100, GlueKind::w, reinterpret_cast<const void *>(&ed_draw_windows), nullptr, 0, 4, "ed_draw_windows_entry"},
    {host_obj_src_editor_windows_glue_s + 104, GlueKind::w, reinterpret_cast<const void *>(&ed_erase_windows), nullptr, 0, 4, "ed_erase_windows_entry"},
    {host_obj_src_editor_windows_glue_s + 108, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_all_zones), nullptr, 0, 4, "ed_delete_all_zones_entry"},
    {host_obj_src_editor_windows_glue_s + 112, GlueKind::w, reinterpret_cast<const void *>(&ed_empty_next), nullptr, 0, 4, "ed_empty_next_entry"},
    {host_obj_src_editor_windows_glue_s + 116, GlueKind::w, reinterpret_cast<const void *>(&ed_next_window), nullptr, 0, 4, "ed_next_window_entry"},
    {host_obj_src_editor_windows_glue_s + 120, GlueKind::w, reinterpret_cast<const void *>(&ed_previous_window), nullptr, 0, 4, "ed_previous_window_entry"},
    {host_obj_src_editor_windows_glue_s + 124, GlueKind::w, reinterpret_cast<const void *>(&ed_flip_size), nullptr, 0, 4, "ed_flip_size_entry"},
    {host_obj_src_editor_windows_glue_s + 128, GlueKind::w_f, reinterpret_cast<const void *>(&ed_window_number), nullptr, 0, 0, "ed_window_number_entry"},
    {host_obj_src_editor_windows_glue_s + 132, GlueKind::w, reinterpret_cast<const void *>(&ed_shrink_all), nullptr, 0, 4, "ed_shrink_all_entry"},
    {host_obj_src_editor_windows_glue_s + 136, GlueKind::w, reinterpret_cast<const void *>(&ed_max_size), nullptr, 0, 4, "ed_max_size_entry"},
    {host_obj_src_editor_windows_glue_s + 140, GlueKind::w, reinterpret_cast<const void *>(&ed_activate), nullptr, 0, 4, "ed_activate_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
