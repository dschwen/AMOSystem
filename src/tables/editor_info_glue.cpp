// src_editor_info_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_info_glue_s[16] = {
};
}

namespace amos::host {

void relocate_src_editor_info_glue_s()
{
}

const GlueEntry entries_src_editor_info_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_editor_info_glue_s, 0), GlueKind::w, AMOS_BIND_W(ed_quit_options), nullptr, 0, 4, "ed_quit_options_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_info_glue_s, 4), GlueKind::w, AMOS_BIND_W(ed_about), nullptr, 0, 4, "ed_about_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_info_glue_s, 8), GlueKind::w, AMOS_BIND_W(ed_about_extensions), nullptr, 0, 4, "ed_about_extensions_entry"},
    {AMOS_ENTRY_AT(host_obj_src_editor_info_glue_s, 12), GlueKind::w, AMOS_BIND_W(ed_information), nullptr, 0, 4, "ed_information_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
