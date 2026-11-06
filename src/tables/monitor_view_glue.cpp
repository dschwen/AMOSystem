// src_monitor_view_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_monitor_view_glue_s[112] = {
};
}

namespace amos::host {

void relocate_src_monitor_view_glue_s()
{
}

const GlueEntry entries_src_monitor_view_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 0), GlueKind::w, AMOS_BIND_W(mon_view_show), nullptr, 0, 4, "mon_view_show_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 4), GlueKind::w, AMOS_BIND_W(mon_view_next), nullptr, 0, 4, "mon_view_next_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 8), GlueKind::w, AMOS_BIND_W(mon_view_clear), nullptr, 0, 4, "mon_view_clear_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 12), GlueKind::w, AMOS_BIND_W(mon_view_info), nullptr, 0, 4, "mon_view_info_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 16), GlueKind::w, AMOS_BIND_W(mon_view_sizes), nullptr, 0, 4, "mon_view_sizes_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 20), GlueKind::w, AMOS_BIND_W(mon_unpack), nullptr, 0, 4, "mon_unpack_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 24), GlueKind::w, AMOS_BIND_W(mon_button), nullptr, 0, 4, "mon_button_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 28), GlueKind::w, AMOS_BIND_W(mon_all_buttons), nullptr, 0, 4, "mon_all_buttons_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 32), GlueKind::w, AMOS_BIND_W(mon_stop_button), nullptr, 0, 4, "mon_stop_button_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 36), GlueKind::w, AMOS_BIND_W(mon_show), nullptr, 0, 4, "mon_show_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 40), GlueKind::w, AMOS_BIND_W(mon_hide), nullptr, 0, 4, "mon_hide_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 44), GlueKind::w, AMOS_BIND_W(mon_mouse_free), nullptr, 0, 4, "mon_mouse_free_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 48), GlueKind::w, AMOS_BIND_W(mon_recop), nullptr, 0, 4, "mon_recop_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 52), GlueKind::w, AMOS_BIND_W(mon_move_up), nullptr, 0, 4, "mon_move_up_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 56), GlueKind::w, AMOS_BIND_W(mon_move_down), nullptr, 0, 4, "mon_move_down_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 60), GlueKind::w, AMOS_BIND_W(mon_move_left), nullptr, 0, 4, "mon_move_left_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 64), GlueKind::w, AMOS_BIND_W(mon_move_right), nullptr, 0, 4, "mon_move_right_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 68), GlueKind::w, AMOS_BIND_W(mon_change_screen), nullptr, 0, 4, "mon_change_screen_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 72), GlueKind::w, AMOS_BIND_W(mon_see), nullptr, 0, 4, "mon_see_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 76), GlueKind::w, AMOS_BIND_W(mon_to_front), nullptr, 0, 4, "mon_to_front_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 80), GlueKind::w, AMOS_BIND_W(mon_to_back), nullptr, 0, 4, "mon_to_back_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 84), GlueKind::w, AMOS_BIND_W(mon_screen_copy), nullptr, 0, 4, "mon_screen_copy_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 88), GlueKind::w, AMOS_BIND_W(mon_tests_patch), nullptr, 0, 4, "mon_tests_patch_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 92), GlueKind::w, AMOS_BIND_W(mon_set_help), nullptr, 0, 4, "mon_set_help_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 96), GlueKind::w, AMOS_BIND_W(mon_set_break), nullptr, 0, 4, "mon_set_break_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 100), GlueKind::w, AMOS_BIND_W(mon_set_evaluate), nullptr, 0, 4, "mon_set_evaluate_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 104), GlueKind::w, AMOS_BIND_W(mon_mode_buttons), nullptr, 0, 4, "mon_mode_buttons_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 108), GlueKind::w, AMOS_BIND_W(mon_tested), nullptr, 0, 4, "mon_tested_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
