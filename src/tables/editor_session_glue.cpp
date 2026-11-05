// src_editor_session_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_session_glue_s[12] = {
};
}

namespace amos::host {

void relocate_src_editor_session_glue_s()
{
}

const GlueEntry entries_src_editor_session_glue_s[] = {
    {host_obj_src_editor_session_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_quit), nullptr, 0, 4, "ed_quit_entry"},
    {host_obj_src_editor_session_glue_s + 4, GlueKind::w, AMOS_BIND_W(ed_do_quit_fn), nullptr, 0, 4, "ed_do_quit_entry"},
    {host_obj_src_editor_session_glue_s + 8, GlueKind::w, AMOS_BIND_W(ed_warm_start), nullptr, 0, 4, "ed_warm_start_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
