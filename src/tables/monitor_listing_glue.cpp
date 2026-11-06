// src_monitor_listing_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_monitor_listing_glue_s[60] = {
};
}

namespace amos::host {

void relocate_src_monitor_listing_glue_s()
{
}

const GlueEntry entries_src_monitor_listing_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 0), GlueKind::w, AMOS_BIND_W(mon_centre_click), nullptr, 0, 4, "mon_centre_click_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 4), GlueKind::w, AMOS_BIND_W(mon_slider_horizontal_click), nullptr, 0, 4, "mon_slider_horizontal_click_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 8), GlueKind::w, AMOS_BIND_W(mon_slider_vertical_click), nullptr, 0, 4, "mon_slider_vertical_click_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 12), GlueKind::w, AMOS_BIND_W(mon_listing_centre), nullptr, 0, 4, "mon_listing_centre_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 16), GlueKind::w, AMOS_BIND_W(mon_listing_show), nullptr, 0, 4, "mon_listing_show_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 20), GlueKind::w, AMOS_BIND_W(mon_listing_new), nullptr, 0, 4, "mon_listing_new_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 24), GlueKind::w, AMOS_BIND_W(mon_listing_untokenise), nullptr, 0, 4, "mon_listing_untokenise_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 28), GlueKind::w, AMOS_BIND_W(mon_separation_drag), nullptr, 0, 4, "mon_separation_drag_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 32), GlueKind::w, AMOS_BIND_W(mon_listing_screen), nullptr, 0, 4, "mon_listing_screen_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 36), GlueKind::w, AMOS_BIND_W(mon_listing_size), nullptr, 0, 4, "mon_listing_size_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 40), GlueKind::w, AMOS_BIND_W(mon_listing_click), nullptr, 0, 4, "mon_listing_click_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 44), GlueKind::w, AMOS_BIND_W(mon_choose), nullptr, 0, 4, "mon_choose_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 48), GlueKind::w, AMOS_BIND_W(mon_mouse_position), nullptr, 0, 4, "mon_mouse_position_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 52), GlueKind::w, AMOS_BIND_W(mon_split), nullptr, 0, 4, "mon_split_entry"},
    {AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 56), GlueKind::w, AMOS_BIND_W(mon_no_init), nullptr, 0, 4, "mon_no_init_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
