// src_editor_marks_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_marks_glue_s[100] = {
};
}

namespace amos::host {

void relocate_src_editor_marks_glue_s()
{
}

const GlueEntry entries_src_editor_marks_glue_s[] = {
    {host_obj_src_editor_marks_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_marks_clear), nullptr, 0, 4, "ed_marks_clear_entry"},
    {host_obj_src_editor_marks_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_auto_marks_fn), nullptr, 0, 4, "ed_auto_marks_entry"},
    {host_obj_src_editor_marks_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark), nullptr, 0, 4, "ed_set_mark_entry"},
    {host_obj_src_editor_marks_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark), nullptr, 0, 4, "ed_goto_mark_entry"},
    {host_obj_src_editor_marks_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_marks_to_addresses), nullptr, 0, 4, "ed_marks_to_addresses_entry"},
    {host_obj_src_editor_marks_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_marks_to_numbers), nullptr, 0, 4, "ed_marks_to_numbers_entry"},
    {host_obj_src_editor_marks_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_marks_change_fn), nullptr, 0, 4, "ed_marks_change_entry"},
    {host_obj_src_editor_marks_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_1), nullptr, 0, 4, "ed_set_mark_1_entry"},
    {host_obj_src_editor_marks_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_1), nullptr, 0, 4, "ed_goto_mark_1_entry"},
    {host_obj_src_editor_marks_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_2), nullptr, 0, 4, "ed_set_mark_2_entry"},
    {host_obj_src_editor_marks_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_2), nullptr, 0, 4, "ed_goto_mark_2_entry"},
    {host_obj_src_editor_marks_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_3), nullptr, 0, 4, "ed_set_mark_3_entry"},
    {host_obj_src_editor_marks_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_3), nullptr, 0, 4, "ed_goto_mark_3_entry"},
    {host_obj_src_editor_marks_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_4), nullptr, 0, 4, "ed_set_mark_4_entry"},
    {host_obj_src_editor_marks_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_4), nullptr, 0, 4, "ed_goto_mark_4_entry"},
    {host_obj_src_editor_marks_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_5), nullptr, 0, 4, "ed_set_mark_5_entry"},
    {host_obj_src_editor_marks_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_5), nullptr, 0, 4, "ed_goto_mark_5_entry"},
    {host_obj_src_editor_marks_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_6), nullptr, 0, 4, "ed_set_mark_6_entry"},
    {host_obj_src_editor_marks_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_6), nullptr, 0, 4, "ed_goto_mark_6_entry"},
    {host_obj_src_editor_marks_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_7), nullptr, 0, 4, "ed_set_mark_7_entry"},
    {host_obj_src_editor_marks_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_7), nullptr, 0, 4, "ed_goto_mark_7_entry"},
    {host_obj_src_editor_marks_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_8), nullptr, 0, 4, "ed_set_mark_8_entry"},
    {host_obj_src_editor_marks_glue_s + 88, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_8), nullptr, 0, 4, "ed_goto_mark_8_entry"},
    {host_obj_src_editor_marks_glue_s + 92, GlueKind::w, reinterpret_cast<const void *>(&ed_set_mark_9), nullptr, 0, 4, "ed_set_mark_9_entry"},
    {host_obj_src_editor_marks_glue_s + 96, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_mark_9), nullptr, 0, 4, "ed_goto_mark_9_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
