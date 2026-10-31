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
    {host_obj_src_editor_loop_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_loop_fn), nullptr, 0, 4, "ed_loop_entry"},
    {host_obj_src_editor_loop_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_function_call_fn), nullptr, 0, 4, "ed_function_call_entry"},
    {host_obj_src_editor_loop_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_in), nullptr, 0, 4, "ed_zap_in_entry"},
    {host_obj_src_editor_loop_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_prepare), nullptr, 0, 4, "ed_zap_prepare_entry"},
    {host_obj_src_editor_loop_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_out_fn), nullptr, 0, 4, "ed_zap_out_entry"},
    {host_obj_src_editor_loop_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_result), nullptr, 0, 4, "ed_zap_result_entry"},
    {host_obj_src_editor_loop_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_function), nullptr, 0, 4, "ed_zap_function_entry"},
    {host_obj_src_editor_loop_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_new_config), nullptr, 0, 4, "ed_zap_new_config_entry"},
    {host_obj_src_editor_loop_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_new_line), nullptr, 0, 4, "ed_zap_new_line_entry"},
    {host_obj_src_editor_loop_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_zap_new_line_tok), nullptr, 0, 4, "ed_zap_new_line_tok_entry"},
    {host_obj_src_editor_loop_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_go_help), nullptr, 0, 4, "ed_go_help_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
