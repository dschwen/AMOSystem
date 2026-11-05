// src_monitor_info_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_monitor_info_glue_s[88] = {
};
}

namespace amos::host {

void relocate_src_monitor_info_glue_s()
{
}

const GlueEntry entries_src_monitor_info_glue_s[] = {
    {host_obj_src_monitor_info_glue_s + 0, GlueKind::w, AMOS_BIND_W(mon_info_centre), nullptr, 0, 4, "mon_info_centre_entry"},
    {host_obj_src_monitor_info_glue_s + 4, GlueKind::w, AMOS_BIND_W(mon_info_click), nullptr, 0, 4, "mon_info_click_entry"},
    {host_obj_src_monitor_info_glue_s + 8, GlueKind::w, AMOS_BIND_W(mon_info_show_line), nullptr, 0, 4, "mon_info_show_line_entry"},
    {host_obj_src_monitor_info_glue_s + 12, GlueKind::w, AMOS_BIND_W(mon_info_count_lines), nullptr, 0, 4, "mon_info_count_lines_entry"},
    {host_obj_src_monitor_info_glue_s + 16, GlueKind::w, AMOS_BIND_W(mon_info_show_all), nullptr, 0, 4, "mon_info_show_all_entry"},
    {host_obj_src_monitor_info_glue_s + 20, GlueKind::w, AMOS_BIND_W(mon_info_begin), nullptr, 0, 4, "mon_info_begin_entry"},
    {host_obj_src_monitor_info_glue_s + 24, GlueKind::w, AMOS_BIND_W(mon_info_print), nullptr, 0, 4, "mon_info_print_entry"},
    {host_obj_src_monitor_info_glue_s + 28, GlueKind::w, AMOS_BIND_W(mon_info_end), nullptr, 0, 4, "mon_info_end_entry"},
    {host_obj_src_monitor_info_glue_s + 32, GlueKind::w, AMOS_BIND_W(mon_info_delete), nullptr, 0, 4, "mon_info_delete_entry"},
    {host_obj_src_monitor_info_glue_s + 36, GlueKind::w, AMOS_BIND_W(mon_info_insert), nullptr, 0, 4, "mon_info_insert_entry"},
    {host_obj_src_monitor_info_glue_s + 40, GlueKind::w, AMOS_BIND_W(mon_info_slider_click), nullptr, 0, 4, "mon_info_slider_click_entry"},
    {host_obj_src_monitor_info_glue_s + 44, GlueKind::w, AMOS_BIND_W(mon_info_slider), nullptr, 0, 4, "mon_info_slider_entry"},
    {host_obj_src_monitor_info_glue_s + 48, GlueKind::w, AMOS_BIND_W(mon_alert), nullptr, 0, 4, "mon_alert_entry"},
    {host_obj_src_monitor_info_glue_s + 52, GlueKind::w, AMOS_BIND_W(mon_alert_delete), nullptr, 0, 4, "mon_alert_delete_entry"},
    {host_obj_src_monitor_info_glue_s + 56, GlueKind::w, AMOS_BIND_W(mon_delete_show), nullptr, 0, 4, "mon_delete_show_entry"},
    {host_obj_src_monitor_info_glue_s + 60, GlueKind::w, AMOS_BIND_W(mon_activate_info), nullptr, 0, 4, "mon_activate_info_entry"},
    {host_obj_src_monitor_info_glue_s + 64, GlueKind::w, AMOS_BIND_W(mon_activate_text), nullptr, 0, 4, "mon_activate_text_entry"},
    {host_obj_src_monitor_info_glue_s + 68, GlueKind::w, AMOS_BIND_W(mon_activate), nullptr, 0, 4, "mon_activate_entry"},
    {host_obj_src_monitor_info_glue_s + 72, GlueKind::w, AMOS_BIND_W(mon_message), nullptr, 0, 4, "mon_message_entry"},
    {host_obj_src_monitor_info_glue_s + 76, GlueKind::w, AMOS_BIND_W(mon_wait_release), nullptr, 0, 4, "mon_wait_release_entry"},
    {host_obj_src_monitor_info_glue_s + 80, GlueKind::w, AMOS_BIND_W(mon_banks_monitor), nullptr, 0, 4, "mon_banks_monitor_entry"},
    {host_obj_src_monitor_info_glue_s + 84, GlueKind::w, AMOS_BIND_W(mon_banks_program), nullptr, 0, 4, "mon_banks_program_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
