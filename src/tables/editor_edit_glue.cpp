// src_editor_edit_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_edit_glue_s[32] = {
};
}

namespace amos::host {

void relocate_src_editor_edit_glue_s()
{
}

const GlueEntry entries_src_editor_edit_glue_s[] = {
    {host_obj_src_editor_edit_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_del_li_cu_fn), nullptr, 0, 4, "ed_del_li_cu_entry"},
    {host_obj_src_editor_edit_glue_s + 4, GlueKind::w, AMOS_BIND_W(ed_join_fn), nullptr, 0, 4, "ed_join_entry"},
    {host_obj_src_editor_edit_glue_s + 8, GlueKind::w, AMOS_BIND_W(ed_return_fn), nullptr, 0, 4, "ed_return_entry"},
    {host_obj_src_editor_edit_glue_s + 12, GlueKind::w, AMOS_BIND_W(ed_return_quiet_fn), nullptr, 0, 4, "ed_return_quiet_entry"},
    {host_obj_src_editor_edit_glue_s + 16, GlueKind::w, AMOS_BIND_W(ed_ins_line_fn), nullptr, 0, 4, "ed_ins_line_entry"},
    {host_obj_src_editor_edit_glue_s + 20, GlueKind::w, AMOS_BIND_W(ed_tok_cur_fn), nullptr, 0, 4, "ed_tok_cur_entry"},
    {host_obj_src_editor_edit_glue_s + 24, GlueKind::w, AMOS_BIND_W(ed_tok_stok), nullptr, 0, 4, "ed_tok_stok_entry"},
    {host_obj_src_editor_edit_glue_s + 28, GlueKind::w, AMOS_BIND_W(ed_tok_stok2), nullptr, 0, 4, "ed_tok_stok2_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
