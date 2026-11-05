// src_editor_block_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_block_glue_s[64] = {
};
}

namespace amos::host {

void relocate_src_editor_block_glue_s()
{
}

const GlueEntry entries_src_editor_block_glue_s[] = {
    {host_obj_src_editor_block_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_block_show), nullptr, 0, 4, "ed_block_show_entry"},
    {host_obj_src_editor_block_glue_s + 4, GlueKind::w, AMOS_BIND_W(ed_block_on), nullptr, 0, 4, "ed_block_on_entry"},
    {host_obj_src_editor_block_glue_s + 8, GlueKind::w, AMOS_BIND_W(ed_block_cursor), nullptr, 0, 4, "ed_block_cursor_entry"},
    {host_obj_src_editor_block_glue_s + 12, GlueKind::w, AMOS_BIND_W(ed_block_hide), nullptr, 0, 4, "ed_block_hide_entry"},
    {host_obj_src_editor_block_glue_s + 16, GlueKind::w, AMOS_BIND_W(ed_cursor_normal_fn), nullptr, 0, 4, "ed_cursor_normal_entry"},
    {host_obj_src_editor_block_glue_s + 20, GlueKind::w, AMOS_BIND_W(ed_block_all), nullptr, 0, 4, "ed_block_all_entry"},
    {host_obj_src_editor_block_glue_s + 24, GlueKind::w, AMOS_BIND_W(ed_block_store), nullptr, 0, 4, "ed_block_store_entry"},
    {host_obj_src_editor_block_glue_s + 28, GlueKind::w, AMOS_BIND_W(ed_block_forget), nullptr, 0, 4, "ed_block_forget_entry"},
    {host_obj_src_editor_block_glue_s + 32, GlueKind::w, AMOS_BIND_W(ed_block_cut), nullptr, 0, 4, "ed_block_cut_entry"},
    {host_obj_src_editor_block_glue_s + 36, GlueKind::w, AMOS_BIND_W(ed_block_paste), nullptr, 0, 4, "ed_block_paste_entry"},
    {host_obj_src_editor_block_glue_s + 40, GlueKind::w, AMOS_BIND_W(ed_block_free), nullptr, 0, 4, "ed_block_free_entry"},
    {host_obj_src_editor_block_glue_s + 44, GlueKind::w, AMOS_BIND_W(ed_block_free_a0), nullptr, 0, 4, "ed_block_free_a0_entry"},
    {host_obj_src_editor_block_glue_s + 48, GlueKind::w, AMOS_BIND_W(ed_block_limits), nullptr, 0, 4, "ed_block_limits_entry"},
    {host_obj_src_editor_block_glue_s + 52, GlueKind::w, AMOS_BIND_W(ed_block_insert), nullptr, 0, 4, "ed_block_insert_entry"},
    {host_obj_src_editor_block_glue_s + 56, GlueKind::w, AMOS_BIND_W(ed_block_delete), nullptr, 0, 4, "ed_block_delete_entry"},
    {host_obj_src_editor_block_glue_s + 60, GlueKind::w, AMOS_BIND_W(ed_block_copy), nullptr, 0, 4, "ed_block_copy_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
