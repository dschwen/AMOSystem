// src_w_vbl_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_vbl_glue_s[16] = {
};
}

namespace amos::host {

void relocate_src_w_vbl_glue_s()
{
}

const GlueEntry entries_src_w_vbl_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_vbl_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_sy_add_vbl), nullptr, 0, 4, "w_add_vbl"},
    {AMOS_ENTRY_AT(host_obj_src_w_vbl_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_sy_rem_vbl), nullptr, 0, 4, "w_rem_vbl"},
    {AMOS_ENTRY_AT(host_obj_src_w_vbl_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_sy_wait_vbl), nullptr, 0, 4, "w_wvbl"},
    {AMOS_ENTRY_AT(host_obj_src_w_vbl_glue_s, 12), GlueKind::missing, nullptr, nullptr, 0, 0, "w_int_call"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
