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
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 0), GlueKind::w_a0, AMOS_BIND_W(ed_mem_reserve), nullptr, 0, 0, "ed_mem_reserve_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_mem_free), nullptr, 0, 4, "ed_mem_free_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 8), GlueKind::w_f, AMOS_BIND_W_RESULT(ed_list_new), nullptr, 0, 0, "ed_list_new_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_list_delete_all), nullptr, 0, 4, "ed_list_delete_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_list_delete), nullptr, 0, 4, "ed_list_delete_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_dialogues_open), nullptr, 0, 4, "ed_dialogues_open_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_dialogues_close), nullptr, 0, 4, "ed_dialogues_close_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_strings_reset), nullptr, 0, 4, "ed_strings_reset_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_string_new), nullptr, 0, 4, "ed_string_new_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_string_add), nullptr, 0, 4, "ed_string_add_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_dialogue_fn), nullptr, 0, 2, "ed_dialogue_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 44), GlueKind::w, AMOS_BIND_W(ed_do_dialogue), nullptr, 0, 2, "ed_do_dialogue_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 48), GlueKind::w, AMOS_BIND_W(ed_macro_load_default), nullptr, 0, 4, "ed_macro_load_default_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 52), GlueKind::w, AMOS_BIND_W(ed_macro_load_as), nullptr, 0, 4, "ed_macro_load_as_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 56), GlueKind::w, AMOS_BIND_W(ed_macro_save_default), nullptr, 0, 4, "ed_macro_save_default_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 60), GlueKind::w, AMOS_BIND_W(ed_macro_save_as), nullptr, 0, 4, "ed_macro_save_as_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 64), GlueKind::w, AMOS_BIND_W(ed_macro_load), nullptr, 0, 4, "ed_macro_load_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 68), GlueKind::w, AMOS_BIND_W(ed_macro_erase), nullptr, 0, 4, "ed_macro_erase_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 72), GlueKind::w, AMOS_BIND_W(ed_macro_erase_all), nullptr, 0, 4, "ed_macro_erase_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 76), GlueKind::w, AMOS_BIND_W(ed_macro_record), nullptr, 0, 4, "ed_macro_record_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 80), GlueKind::w, AMOS_BIND_W(ed_macro_stop_fn), nullptr, 0, 4, "ed_macro_stop_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_macros_glue_s, 84), GlueKind::w, AMOS_BIND_W(ed_macros_free), nullptr, 0, 4, "ed_macros_free_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
