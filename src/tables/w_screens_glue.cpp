// src_w_screens_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_screens_glue_s[88] = {
};
}

namespace amos::host {

void relocate_src_w_screens_glue_s()
{
}

const GlueEntry entries_src_w_screens_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_ec_open), nullptr, 0, 4, "w_sc_open"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_ec_close), nullptr, 0, 4, "w_sc_close"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_ec_close_range), nullptr, 0, 4, "w_sc_close_range"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_ec_to_front), nullptr, 0, 4, "w_sc_to_front"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_ec_to_back), nullptr, 0, 4, "w_sc_to_back"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_ec_view), nullptr, 0, 4, "w_sc_view"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_ec_offset), nullptr, 0, 4, "w_sc_offset"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_ec_hide), nullptr, 0, 4, "w_sc_hide"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 32), GlueKind::w, AMOS_BIND_W(w_ec_clone), nullptr, 0, 4, "w_sc_clone"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 36), GlueKind::w, AMOS_BIND_W(w_ec_double), nullptr, 0, 4, "w_sc_double"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 40), GlueKind::w, AMOS_BIND_W(w_ec_swap), nullptr, 0, 4, "w_sc_swap"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 44), GlueKind::w, AMOS_BIND_W(w_ec_swap_all), nullptr, 0, 4, "w_sc_swap_all"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 48), GlueKind::w, AMOS_BIND_W(w_ec_dual), nullptr, 0, 4, "w_sc_dual"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 52), GlueKind::w, AMOS_BIND_W(w_ec_dual_priority), nullptr, 0, 4, "w_sc_dual_priority"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 56), GlueKind::w, AMOS_BIND_W(w_ec_activate), nullptr, 0, 4, "w_sc_activate"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 60), GlueKind::w, AMOS_BIND_W(w_ec_current), nullptr, 0, 4, "w_sc_current"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 64), GlueKind::w, AMOS_BIND_W(w_ec_address), nullptr, 0, 4, "w_sc_address"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 68), GlueKind::w, AMOS_BIND_W(w_ec_free_number), nullptr, 0, 4, "w_sc_free_number"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 72), GlueKind::w, AMOS_BIND_W(w_ec_max_raw), nullptr, 0, 4, "w_sc_max_raw"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 76), GlueKind::w, AMOS_BIND_W(w_ec_ntsc), nullptr, 0, 4, "w_sc_ntsc"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 80), GlueKind::w, AMOS_BIND_W(w_ec_reset), nullptr, 0, 4, "w_sc_reset"},
    {AMOS_ENTRY_AT(host_obj_src_w_screens_glue_s, 84), GlueKind::w, AMOS_BIND_W(w_ec_end), nullptr, 0, 4, "w_sc_end"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
