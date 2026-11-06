// src_w_startup_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_startup_glue_s[16] = {
};
}

namespace amos::host {

void relocate_src_w_startup_glue_s()
{
}

const GlueEntry entries_src_w_startup_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_startup_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_sy_ice_start), nullptr, 0, 4, "w_ice_start"},
    {AMOS_ENTRY_AT(host_obj_src_w_startup_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_sy_ice_end), nullptr, 0, 4, "w_ice_end"},
    {AMOS_ENTRY_AT(host_obj_src_w_startup_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_sy_start_all), nullptr, 0, 4, "w_start_all"},
    {AMOS_ENTRY_AT(host_obj_src_w_startup_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_sy_end_all), nullptr, 0, 4, "w_end_all"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
