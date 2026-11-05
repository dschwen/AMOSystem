// src_w_system_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_system_glue_s[12] = {
};
}

namespace amos::host {

void relocate_src_w_system_glue_s()
{
}

const GlueEntry entries_src_w_system_glue_s[] = {
    {host_obj_src_w_system_glue_s + 0, GlueKind::w, AMOS_BIND_W(w_sy_amos_wb), nullptr, 0, 4, "w_tamoswb"},
    {host_obj_src_w_system_glue_s + 4, GlueKind::w, AMOS_BIND_W(w_sy_joy), nullptr, 0, 4, "w_joy"},
    {host_obj_src_w_system_glue_s + 8, GlueKind::w, AMOS_BIND_W(w_sy_request_on_off), nullptr, 0, 4, "w_request_on_off"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
