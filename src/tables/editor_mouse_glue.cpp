// src_editor_mouse_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_mouse_glue_s[4] = {
};
}

namespace amos::host {

void relocate_src_editor_mouse_glue_s()
{
}

const GlueEntry entries_src_editor_mouse_glue_s[] = {
    {host_obj_src_editor_mouse_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_mouse), nullptr, 0, 4, "ed_mouse_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
