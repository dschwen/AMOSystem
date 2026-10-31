// src_editor_menu_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_menu_glue_s[88] = {
};
}

namespace amos::host {

void relocate_src_editor_menu_glue_s()
{
}

const GlueEntry entries_src_editor_menu_glue_s[] = {
    {host_obj_src_editor_menu_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_menu_fn), nullptr, 0, 2, "ed_menu_entry"},
    {host_obj_src_editor_menu_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_menus_end), nullptr, 0, 4, "ed_menus_end_entry"},
    {host_obj_src_editor_menu_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_menu_error_fn), nullptr, 0, 4, "ed_menu_error_entry"},
    {host_obj_src_editor_menu_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_menu_init), nullptr, 0, 2, "ed_menu_init_entry"},
    {host_obj_src_editor_menu_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_previous_hidden), nullptr, 0, 4, "ed_previous_hidden_entry"},
    {host_obj_src_editor_menu_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_next_hidden), nullptr, 0, 4, "ed_next_hidden_entry"},
    {host_obj_src_editor_menu_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_branch_amos_fn), nullptr, 0, 4, "ed_branch_amos_entry"},
    {host_obj_src_editor_menu_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_menu_marks), nullptr, 0, 4, "ed_menu_marks_entry"},
    {host_obj_src_editor_menu_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_flip_keys), nullptr, 0, 4, "ed_flip_keys_entry"},
    {host_obj_src_editor_menu_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_menus_editor), nullptr, 0, 4, "ed_menus_editor_entry"},
    {host_obj_src_editor_menu_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_menus_program), nullptr, 0, 4, "ed_menus_program_entry"},
    {host_obj_src_editor_menu_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_set_autosave), nullptr, 0, 4, "ed_set_autosave_entry"},
    {host_obj_src_editor_menu_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ed_save_all), nullptr, 0, 4, "ed_save_all_entry"},
    {host_obj_src_editor_menu_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ed_user_menu), nullptr, 0, 4, "ed_user_menu_entry"},
    {host_obj_src_editor_menu_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ed_add_user), nullptr, 0, 4, "ed_add_user_entry"},
    {host_obj_src_editor_menu_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_user), nullptr, 0, 4, "ed_delete_user_entry"},
    {host_obj_src_editor_menu_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ed_program_to_menu), nullptr, 0, 4, "ed_program_to_menu_entry"},
    {host_obj_src_editor_menu_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&ed_key_to_menu), nullptr, 0, 4, "ed_key_to_menu_entry"},
    {host_obj_src_editor_menu_glue_s + 72, GlueKind::w, reinterpret_cast<const void *>(&ed_flip_insert), nullptr, 0, 4, "ed_flip_insert_entry"},
    {host_obj_src_editor_menu_glue_s + 76, GlueKind::w, reinterpret_cast<const void *>(&ed_flip_sounds), nullptr, 0, 4, "ed_flip_sounds_entry"},
    {host_obj_src_editor_menu_glue_s + 80, GlueKind::w, reinterpret_cast<const void *>(&ed_insert_button), nullptr, 0, 4, "ed_insert_button_entry"},
    {host_obj_src_editor_menu_glue_s + 84, GlueKind::w, reinterpret_cast<const void *>(&ed_clear_var_fn), nullptr, 0, 4, "ed_clear_var"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
