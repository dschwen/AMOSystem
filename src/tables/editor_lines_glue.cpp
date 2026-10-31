// src_editor_lines_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_lines_glue_s[64] = {
};
}

namespace amos::host {

void relocate_src_editor_lines_glue_s()
{
}

const GlueEntry entries_src_editor_lines_glue_s[] = {
    {host_obj_src_editor_lines_glue_s + 0, GlueKind::w_f, reinterpret_cast<const void *>(&ed_current_line), nullptr, 0, 0, "ed_current_line_entry"},
    {host_obj_src_editor_lines_glue_s + 4, GlueKind::w_f, reinterpret_cast<const void *>(&ed_letter), nullptr, 0, 0, "ed_letter_entry"},
    {host_obj_src_editor_lines_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_insert_text), nullptr, 0, 4, "ed_insert_text_entry"},
    {host_obj_src_editor_lines_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_text), nullptr, 0, 4, "ed_delete_text_entry"},
    {host_obj_src_editor_lines_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_word_left), nullptr, 0, 4, "ed_word_left_entry"},
    {host_obj_src_editor_lines_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_word_right), nullptr, 0, 4, "ed_word_right_entry"},
    {host_obj_src_editor_lines_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_type_key), nullptr, 0, 4, "ed_type_key_entry"},
    {host_obj_src_editor_lines_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_key), nullptr, 0, 4, "ed_delete_key_entry"},
    {host_obj_src_editor_lines_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_backspace), nullptr, 0, 4, "ed_backspace_entry"},
    {host_obj_src_editor_lines_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_clear_line), nullptr, 0, 4, "ed_clear_line_entry"},
    {host_obj_src_editor_lines_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_to_end), nullptr, 0, 4, "ed_delete_to_end_entry"},
    {host_obj_src_editor_lines_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_to_start), nullptr, 0, 4, "ed_delete_to_start_entry"},
    {host_obj_src_editor_lines_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_word), nullptr, 0, 4, "ed_delete_word_entry"},
    {host_obj_src_editor_lines_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_word_left), nullptr, 0, 4, "ed_delete_word_left_entry"},
    {host_obj_src_editor_lines_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ed_tab), nullptr, 0, 4, "ed_tab_entry"},
    {host_obj_src_editor_lines_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ed_untab), nullptr, 0, 4, "ed_untab_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
