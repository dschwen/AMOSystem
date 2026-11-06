// src_w_bobs_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_bobs_glue_s[100] = {
};
}

namespace amos::host {

void relocate_src_w_bobs_glue_s()
{
}

const GlueEntry entries_src_w_bobs_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_sy_bob_set), nullptr, 0, 2, "w_bob_set"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_sy_bob_off), nullptr, 0, 4, "w_bob_off"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_sy_bobs_off), nullptr, 0, 4, "w_bobs_off"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_sy_bob_update), nullptr, 0, 4, "w_bob_update"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_sy_bob_show), nullptr, 0, 4, "w_bob_show"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_sy_bob_erase), nullptr, 0, 4, "w_bob_erase"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_sy_bob_limit), nullptr, 0, 4, "w_bob_limit"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_sy_get_bob), nullptr, 0, 4, "w_get_bob"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 32), GlueKind::w, AMOS_BIND_W(w_sy_mask), nullptr, 0, 4, "w_mask"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 36), GlueKind::w, AMOS_BIND_W(w_sy_hot_spot), nullptr, 0, 4, "w_hot_spot"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 40), GlueKind::w, AMOS_BIND_W(w_sy_bob_xy), nullptr, 0, 4, "w_bob_xy"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 44), GlueKind::w, AMOS_BIND_W(w_sy_bob_put), nullptr, 0, 4, "w_bob_put"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 48), GlueKind::w, AMOS_BIND_W(w_sy_paste), nullptr, 0, 4, "w_paste"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 52), GlueKind::w, AMOS_BIND_W(w_sy_priority), nullptr, 0, 4, "w_priority"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 56), GlueKind::w, AMOS_BIND_W(w_sy_bobs_screen_off), nullptr, 0, 4, "w_bobs_screen_off"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 60), GlueKind::w, AMOS_BIND_W(w_ec_block_get), nullptr, 0, 4, "w_block_get"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 64), GlueKind::w, AMOS_BIND_W(w_ec_block_delete), nullptr, 0, 4, "w_block_delete"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 68), GlueKind::w, AMOS_BIND_W(w_ec_blocks_delete), nullptr, 0, 4, "w_blocks_delete"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 72), GlueKind::w, AMOS_BIND_W(w_ec_block_put), nullptr, 0, 4, "w_block_put"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 76), GlueKind::w, AMOS_BIND_W(w_ec_block_flip), nullptr, 0, 4, "w_block_flip"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 80), GlueKind::w, AMOS_BIND_W(w_ec_flip), nullptr, 0, 4, "w_flip"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 84), GlueKind::w, AMOS_BIND_W(w_ec_cblock_get), nullptr, 0, 2, "w_cblock_get"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 88), GlueKind::w, AMOS_BIND_W(w_ec_cblock_put), nullptr, 0, 4, "w_cblock_put"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 92), GlueKind::w, AMOS_BIND_W(w_ec_cblock_delete), nullptr, 0, 4, "w_cblock_delete"},
    {AMOS_ENTRY_AT(host_obj_src_w_bobs_glue_s, 96), GlueKind::w, AMOS_BIND_W(w_ec_cblocks_delete), nullptr, 0, 4, "w_cblocks_delete"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
