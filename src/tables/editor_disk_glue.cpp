// src_editor_disk_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_disk_glue_s[64] = {
};
}

namespace amos::host {

void relocate_src_editor_disk_glue_s()
{
}

const GlueEntry entries_src_editor_disk_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_load_hidden_fn), nullptr, 0, 4, "ed_rload_hidden_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_load_hidden_key), nullptr, 0, 4, "ed_load_hidden_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_load), nullptr, 0, 4, "ed_load_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_reload), nullptr, 0, 4, "ed_reload_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_name_part), nullptr, 0, 4, "ed_name_part_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_merge), nullptr, 0, 4, "ed_merge_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_merge_ascii), nullptr, 0, 4, "ed_merge_ascii_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_rename), nullptr, 0, 4, "ed_rename_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_save_as_name), nullptr, 0, 4, "ed_save_as_name_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_save_as), nullptr, 0, 4, "ed_save_as_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_save), nullptr, 0, 4, "ed_save_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 44), GlueKind::w, AMOS_BIND_W(ed_save_it), nullptr, 0, 4, "ed_save_it_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 48), GlueKind::w, AMOS_BIND_W(ed_save_program), nullptr, 0, 4, "ed_save_program_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 52), GlueKind::w, AMOS_BIND_W(ed_save_program2), nullptr, 0, 4, "ed_save_program2_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 56), GlueKind::w, AMOS_BIND_W(ed_make_bak), nullptr, 0, 4, "ed_make_bak_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_disk_glue_s, 60), GlueKind::w, AMOS_BIND_W(ed_save_icon), nullptr, 0, 4, "ed_save_icon_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
