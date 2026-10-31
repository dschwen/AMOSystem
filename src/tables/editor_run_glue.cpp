// src_editor_run_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_run_glue_s[92] = {
};
}

namespace amos::host {

void relocate_src_editor_run_glue_s()
{
}

const GlueEntry entries_src_editor_run_glue_s[] = {
    {host_obj_src_editor_run_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_go_monitor), nullptr, 0, 4, "ed_go_monitor_entry"},
    {host_obj_src_editor_run_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_program_command), nullptr, 0, 4, "ed_program_command_entry"},
    {host_obj_src_editor_run_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_program_reload), nullptr, 0, 4, "ed_program_reload_entry"},
    {host_obj_src_editor_run_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_window_of_name), nullptr, 0, 4, "ed_window_of_name_entry"},
    {host_obj_src_editor_run_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_run_hidden), nullptr, 0, 4, "ed_run_hidden_entry"},
    {host_obj_src_editor_run_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_run_hide), nullptr, 0, 4, "ed_run_hide_entry"},
    {host_obj_src_editor_run_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_run_direct), nullptr, 0, 4, "ed_run_direct_entry"},
    {host_obj_src_editor_run_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_run), nullptr, 0, 4, "ed_run_entry"},
    {host_obj_src_editor_run_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_error_direct_run), nullptr, 0, 4, "ed_error_direct_run_entry"},
    {host_obj_src_editor_run_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_error_run_hidden), nullptr, 0, 4, "ed_error_run_hidden_entry"},
    {host_obj_src_editor_run_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_error_test), nullptr, 0, 4, "ed_error_test_entry"},
    {host_obj_src_editor_run_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_error_run), nullptr, 0, 4, "ed_error_run_entry"},
    {host_obj_src_editor_run_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ed_error_edit), nullptr, 0, 4, "ed_error_edit_entry"},
    {host_obj_src_editor_run_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ed_error_line), nullptr, 0, 4, "ed_error_line_entry"},
    {host_obj_src_editor_run_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ed_test), nullptr, 0, 4, "ed_test_entry"},
    {host_obj_src_editor_run_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ed_check_13), nullptr, 0, 4, "ed_check_13_entry"},
    {host_obj_src_editor_run_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ed_indent_program), nullptr, 0, 4, "ed_indent_program_entry"},
    {host_obj_src_editor_run_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&ed_test_program_fn), nullptr, 0, 4, "ed_test_program_fn_entry"},
    {host_obj_src_editor_run_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&ed_test_shown), nullptr, 0, 4, "ed_test_shown_entry"},
    {host_obj_src_editor_run_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&ed_test_end), nullptr, 0, 4, "ed_test_end_entry"},
    {host_obj_src_editor_run_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&ed_test_run), nullptr, 0, 4, "ed_test_run_entry"},
    {host_obj_src_editor_run_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&ed_procedure_machine_code), nullptr, 0, 4, "ed_procedure_machine_code_entry"},
    {host_obj_src_editor_run_glue_s + 88, GlueKind::w, reinterpret_cast<const void *>(&ed_nothing), nullptr, 0, 4, "ed_nothing_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
