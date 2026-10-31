// src_editor_macros_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_macros_glue_s[88] = {
};
}

namespace amos::host {

void relocate_src_editor_macros_glue_s()
{
}

const GlueEntry entries_src_editor_macros_glue_s[] = {
    {host_obj_src_editor_macros_glue_s + 0, GlueKind::w_a0, reinterpret_cast<const void *>(&ed_mem_reserve), nullptr, 0, 0, "ed_mem_reserve_entry"},
    {host_obj_src_editor_macros_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_mem_free), nullptr, 0, 4, "ed_mem_free_entry"},
    {host_obj_src_editor_macros_glue_s + 8, GlueKind::w_f, reinterpret_cast<const void *>(&ed_list_new), nullptr, 0, 0, "ed_list_new_entry"},
    {host_obj_src_editor_macros_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_list_delete_all), nullptr, 0, 4, "ed_list_delete_all_entry"},
    {host_obj_src_editor_macros_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_list_delete), nullptr, 0, 4, "ed_list_delete_entry"},
    {host_obj_src_editor_macros_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_dialogues_open), nullptr, 0, 4, "ed_dialogues_open_entry"},
    {host_obj_src_editor_macros_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_dialogues_close), nullptr, 0, 4, "ed_dialogues_close_entry"},
    {host_obj_src_editor_macros_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_strings_reset), nullptr, 0, 4, "ed_strings_reset_entry"},
    {host_obj_src_editor_macros_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_string_new), nullptr, 0, 4, "ed_string_new_entry"},
    {host_obj_src_editor_macros_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_string_add), nullptr, 0, 4, "ed_string_add_entry"},
    {host_obj_src_editor_macros_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_dialogue_fn), nullptr, 0, 2, "ed_dialogue_entry"},
    {host_obj_src_editor_macros_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_do_dialogue), nullptr, 0, 2, "ed_do_dialogue_entry"},
    {host_obj_src_editor_macros_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_load_default), nullptr, 0, 4, "ed_macro_load_default_entry"},
    {host_obj_src_editor_macros_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_load_as), nullptr, 0, 4, "ed_macro_load_as_entry"},
    {host_obj_src_editor_macros_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_save_default), nullptr, 0, 4, "ed_macro_save_default_entry"},
    {host_obj_src_editor_macros_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_save_as), nullptr, 0, 4, "ed_macro_save_as_entry"},
    {host_obj_src_editor_macros_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_load), nullptr, 0, 4, "ed_macro_load_entry"},
    {host_obj_src_editor_macros_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_erase), nullptr, 0, 4, "ed_macro_erase_entry"},
    {host_obj_src_editor_macros_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_erase_all), nullptr, 0, 4, "ed_macro_erase_all_entry"},
    {host_obj_src_editor_macros_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_record), nullptr, 0, 4, "ed_macro_record_entry"},
    {host_obj_src_editor_macros_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&ed_macro_stop_fn), nullptr, 0, 4, "ed_macro_stop_entry"},
    {host_obj_src_editor_macros_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&ed_macros_free), nullptr, 0, 4, "ed_macros_free_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
