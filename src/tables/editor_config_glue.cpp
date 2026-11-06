// src_editor_config_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_config_glue_s[88] = {
};
}

namespace amos::host {

void relocate_src_editor_config_glue_s()
{
}

const GlueEntry entries_src_editor_config_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_set_banks_fn), nullptr, 0, 4, "ed_set_banks_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_sound_play), nullptr, 0, 4, "ed_sound_play_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_sounds_changed), nullptr, 0, 4, "ed_sounds_changed_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_sounds_end), nullptr, 0, 4, "ed_sounds_end_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_resource_load), nullptr, 0, 4, "ed_resource_load_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_resource_free), nullptr, 0, 4, "ed_resource_free_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_config_set_palette), nullptr, 0, 4, "ed_config_set_palette_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_config_load), nullptr, 0, 4, "ed_config_load_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_config_save), nullptr, 0, 4, "ed_config_save_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_config_free), nullptr, 0, 4, "ed_config_free_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_config_save_default), nullptr, 0, 4, "ed_config_save_default_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 44), GlueKind::w, AMOS_BIND_W(ed_config_save_default_quiet), nullptr, 0, 4, "ed_config_save_default_quiet_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 48), GlueKind::w, AMOS_BIND_W(ed_config_save_as), nullptr, 0, 4, "ed_config_save_as_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 52), GlueKind::w, AMOS_BIND_W(ed_config_load_default), nullptr, 0, 4, "ed_config_load_default_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 56), GlueKind::w, AMOS_BIND_W(ed_config_load_default_quiet), nullptr, 0, 4, "ed_config_load_default_quiet_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 60), GlueKind::w, AMOS_BIND_W(ed_config_load_as), nullptr, 0, 4, "ed_config_load_as_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 64), GlueKind::w, AMOS_BIND_W(ed_config_redraw), nullptr, 0, 4, "ed_config_redraw_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 68), GlueKind::w, AMOS_BIND_W(ed_config_save_changed), nullptr, 0, 4, "ed_config_save_changed_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 72), GlueKind::w, AMOS_BIND_W(ed_wait_input), nullptr, 0, 4, "ed_wait_input_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 76), GlueKind::w, AMOS_BIND_W(ed_text_change), nullptr, 0, 4, "ed_text_change_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 80), GlueKind::w, AMOS_BIND_W(ed_text_count), nullptr, 0, 4, "ed_text_count_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_config_glue_s, 84), GlueKind::w, AMOS_BIND_W(ed_text_free), nullptr, 0, 4, "ed_text_free_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
