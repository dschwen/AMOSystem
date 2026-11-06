// src_editor_loop_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_loop_glue_s[44] = {
};
}

namespace amos::host {

void relocate_src_editor_loop_glue_s()
{
}

const GlueEntry entries_src_editor_loop_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_loop_fn), nullptr, 0, 4, "ed_loop_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_function_call_fn), nullptr, 0, 4, "ed_function_call_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_zap_in), nullptr, 0, 4, "ed_zap_in_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_zap_prepare), nullptr, 0, 4, "ed_zap_prepare_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_zap_out_fn), nullptr, 0, 4, "ed_zap_out_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_zap_result), nullptr, 0, 4, "ed_zap_result_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_zap_function), nullptr, 0, 4, "ed_zap_function_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_zap_new_config), nullptr, 0, 4, "ed_zap_new_config_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_zap_new_line), nullptr, 0, 4, "ed_zap_new_line_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_zap_new_line_tok), nullptr, 0, 4, "ed_zap_new_line_tok_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_loop_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_go_help), nullptr, 0, 4, "ed_go_help_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
