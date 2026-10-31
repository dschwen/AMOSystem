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
    {host_obj_src_w_bobs_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_set), nullptr, 0, 2, "w_bob_set"},
    {host_obj_src_w_bobs_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_off), nullptr, 0, 4, "w_bob_off"},
    {host_obj_src_w_bobs_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bobs_off), nullptr, 0, 4, "w_bobs_off"},
    {host_obj_src_w_bobs_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_update), nullptr, 0, 4, "w_bob_update"},
    {host_obj_src_w_bobs_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_show), nullptr, 0, 4, "w_bob_show"},
    {host_obj_src_w_bobs_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_erase), nullptr, 0, 4, "w_bob_erase"},
    {host_obj_src_w_bobs_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_limit), nullptr, 0, 4, "w_bob_limit"},
    {host_obj_src_w_bobs_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&w_sy_get_bob), nullptr, 0, 4, "w_get_bob"},
    {host_obj_src_w_bobs_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&w_sy_mask), nullptr, 0, 4, "w_mask"},
    {host_obj_src_w_bobs_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&w_sy_hot_spot), nullptr, 0, 4, "w_hot_spot"},
    {host_obj_src_w_bobs_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_xy), nullptr, 0, 4, "w_bob_xy"},
    {host_obj_src_w_bobs_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bob_put), nullptr, 0, 4, "w_bob_put"},
    {host_obj_src_w_bobs_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&w_sy_paste), nullptr, 0, 4, "w_paste"},
    {host_obj_src_w_bobs_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&w_sy_priority), nullptr, 0, 4, "w_priority"},
    {host_obj_src_w_bobs_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&w_sy_bobs_screen_off), nullptr, 0, 4, "w_bobs_screen_off"},
    {host_obj_src_w_bobs_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&w_ec_block_get), nullptr, 0, 4, "w_block_get"},
    {host_obj_src_w_bobs_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&w_ec_block_delete), nullptr, 0, 4, "w_block_delete"},
    {host_obj_src_w_bobs_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&w_ec_blocks_delete), nullptr, 0, 4, "w_blocks_delete"},
    {host_obj_src_w_bobs_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&w_ec_block_put), nullptr, 0, 4, "w_block_put"},
    {host_obj_src_w_bobs_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&w_ec_block_flip), nullptr, 0, 4, "w_block_flip"},
    {host_obj_src_w_bobs_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&w_ec_flip), nullptr, 0, 4, "w_flip"},
    {host_obj_src_w_bobs_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cblock_get), nullptr, 0, 2, "w_cblock_get"},
    {host_obj_src_w_bobs_glue_s + 88, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cblock_put), nullptr, 0, 4, "w_cblock_put"},
    {host_obj_src_w_bobs_glue_s + 92, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cblock_delete), nullptr, 0, 4, "w_cblock_delete"},
    {host_obj_src_w_bobs_glue_s + 96, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cblocks_delete), nullptr, 0, 4, "w_cblocks_delete"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
