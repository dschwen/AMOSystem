// src_w_amal_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_amal_glue_s[76] = {
};
}

namespace amos::host {

void relocate_src_w_amal_glue_s()
{
}

const GlueEntry entries_src_w_amal_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_sy_amal_tok), nullptr, 0, 2, "w_amal_tok"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_sy_amal_cre), nullptr, 0, 2, "w_amal_cre"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_sy_amal_mvo), nullptr, 0, 4, "w_amal_mvo"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_sy_amal_dall), nullptr, 0, 4, "w_amal_dall"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_sy_amal_animate), nullptr, 0, 4, "w_amal_animate"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_sy_amal_reg), nullptr, 0, 4, "w_amal_reg"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_sy_amal_clr), nullptr, 0, 4, "w_amal_clr"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_sy_amal_frz), nullptr, 0, 4, "w_amal_frz"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 32), GlueKind::w, AMOS_BIND_W(w_sy_amal_ufrz), nullptr, 0, 4, "w_amal_ufrz"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 36), GlueKind::w, AMOS_BIND_W(w_sy_set_sync), nullptr, 0, 4, "w_amal_set_sync"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 40), GlueKind::w, AMOS_BIND_W(w_sy_sync), nullptr, 0, 4, "w_amal_sync"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 44), GlueKind::w, AMOS_BIND_W(w_sy_play_set), nullptr, 0, 4, "w_amal_play_set"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 48), GlueKind::w, AMOS_BIND_W(w_sy_movon), nullptr, 0, 4, "w_amal_movon"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 52), GlueKind::w, AMOS_BIND_W(w_sy_chan_a), nullptr, 0, 4, "w_amal_chan_a"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 56), GlueKind::w, AMOS_BIND_W(w_sy_chan_m), nullptr, 0, 4, "w_amal_chan_m"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 60), GlueKind::w, AMOS_BIND_W(w_dad_amal_c), nullptr, 0, 4, "w_dad_amal"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 64), GlueKind::w, AMOS_BIND_W(w_amal_init_c), nullptr, 0, 4, "w_amal_init"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 68), GlueKind::w, AMOS_BIND_W(w_sy_amal_dall), nullptr, 0, 4, "w_amal_end"},
    {AMOS_ENTRY_AT(host_obj_src_w_amal_glue_s, 72), GlueKind::hand, AMOS_BIND_HAND(host_w_animator), nullptr, 0, 0, "w_animator"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
