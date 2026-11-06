// src_lib_floatasc_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_floatasc_glue_s[8] = {
};
}

namespace amos::host {

void relocate_src_lib_floatasc_glue_s()
{
}

const GlueEntry entries_src_lib_floatasc_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_floatasc_glue_s, 0), GlueKind::c_routine, AMOS_BIND_UNSUPPORTED(lib_asc_to_float), nullptr, 0, 0, "AscToFloat"},
    {AMOS_ENTRY_AT(host_obj_src_lib_floatasc_glue_s, 4), GlueKind::reg, AMOS_BIND_REG(lib_float_to_asc), nullptr, 0, 0, "FloatToAsc"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
