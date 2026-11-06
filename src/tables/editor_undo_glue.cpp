// src_editor_undo_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_undo_glue_s[56] = {
};
}

namespace amos::host {

void relocate_src_editor_undo_glue_s()
{
}

const GlueEntry entries_src_editor_undo_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 0), GlueKind::w_f, AMOS_BIND_W_RESULT(ed_undo_start), nullptr, 0, 0, "ed_undo_start_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 4), GlueKind::w_f, AMOS_BIND_W_RESULT(ed_undo_line), nullptr, 0, 0, "ed_undo_line_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_undo_advance), nullptr, 0, 4, "ed_undo_advance_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_undo_clear), nullptr, 0, 4, "ed_undo_clear_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_undo_previous), nullptr, 0, 4, "ed_undo_previous_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_undo_next), nullptr, 0, 4, "ed_undo_next_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_undo_create), nullptr, 0, 4, "ed_undo_create_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_undo_reset), nullptr, 0, 4, "ed_undo_reset_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_undo_free), nullptr, 0, 4, "ed_undo_free_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 36), GlueKind::w, AMOS_BIND_W(ed_undo_create_all), nullptr, 0, 4, "ed_undo_create_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 40), GlueKind::w, AMOS_BIND_W(ed_undo_reset_all), nullptr, 0, 4, "ed_undo_reset_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 44), GlueKind::w, AMOS_BIND_W(ed_undo_free_all), nullptr, 0, 4, "ed_undo_free_all_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 48), GlueKind::w, AMOS_BIND_W(ed_undo), nullptr, 0, 4, "ed_undo_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_undo_glue_s, 52), GlueKind::w, AMOS_BIND_W(ed_redo), nullptr, 0, 4, "ed_redo_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
