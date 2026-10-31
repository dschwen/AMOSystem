// src_editor_program_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_program_glue_s[40] = {
};
}

namespace amos::host {

void relocate_src_editor_program_glue_s()
{
}

const GlueEntry entries_src_editor_program_glue_s[] = {
    {host_obj_src_editor_program_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_new_fn), nullptr, 0, 4, "ed_new_entry"},
    {host_obj_src_editor_program_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_new2_fn), nullptr, 0, 4, "ed_new2_entry"},
    {host_obj_src_editor_program_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_window_new), nullptr, 0, 4, "ed_window_new_entry"},
    {host_obj_src_editor_program_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_save_over_fn), nullptr, 0, 4, "ed_save_over_entry"},
    {host_obj_src_editor_program_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_saved_fn), nullptr, 0, 4, "ed_saved_entry"},
    {host_obj_src_editor_program_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_set_buffer), nullptr, 0, 4, "ed_set_buffer_entry"},
    {host_obj_src_editor_program_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_set_buffer_from), nullptr, 0, 4, "ed_set_buffer_from_entry"},
    {host_obj_src_editor_program_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_get_place_fn), nullptr, 0, 4, "ed_get_place_entry"},
    {host_obj_src_editor_program_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_file_selector_fn), nullptr, 0, 2, "ed_file_selector_entry"},
    {host_obj_src_editor_program_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_open_load), nullptr, 0, 4, "ed_open_load_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
