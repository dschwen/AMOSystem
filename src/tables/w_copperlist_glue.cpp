// src_w_copperlist_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_copperlist_glue_s[8] = {
};
}

namespace amos::host {

void relocate_src_w_copperlist_glue_s()
{
}

const GlueEntry entries_src_w_copperlist_glue_s[] = {
    {host_obj_src_w_copperlist_glue_s + 0, GlueKind::w, AMOS_BIND_W(w_ec_copper_make), nullptr, 0, 4, "w_copper_make"},
    {host_obj_src_w_copperlist_glue_s + 4, GlueKind::w, AMOS_BIND_W(w_ec_copper_force), nullptr, 0, 4, "w_copper_force"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
