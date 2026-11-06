// src_editor_startup_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_startup_glue_s[36] = {
};
}

namespace amos::host {

void relocate_src_editor_startup_glue_s()
{
}

const GlueEntry entries_src_editor_startup_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_cold), nullptr, 0, 4, "ed_cold_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_title), nullptr, 0, 4, "ed_title_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_end), nullptr, 0, 4, "ed_end_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_open_editor_fn), nullptr, 0, 4, "ed_open_editor_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 16), GlueKind::w, AMOS_BIND_W(ed_kill_editor), nullptr, 0, 4, "ed_kill_editor_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 20), GlueKind::w, AMOS_BIND_W(ed_close_editor_fn), nullptr, 0, 4, "ed_close_editor_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 24), GlueKind::w, AMOS_BIND_W(ed_draw_top_fn), nullptr, 0, 4, "ed_draw_top_fn_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 28), GlueKind::w, AMOS_BIND_W(ed_tokens_init), nullptr, 0, 4, "ed_tokens_init_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_startup_glue_s, 32), GlueKind::w, AMOS_BIND_W(ed_tokens_end), nullptr, 0, 4, "ed_tokens_end_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
