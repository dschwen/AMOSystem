// src_lib_doubleasc_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_doubleasc_glue_s[8] = {
};
}

namespace amos::host {

void relocate_src_lib_doubleasc_glue_s()
{
}

const GlueEntry entries_src_lib_doubleasc_glue_s[] = {
    {host_obj_src_lib_doubleasc_glue_s + 0, GlueKind::c_routine, reinterpret_cast<const void *>(&lib_double_to_asc), nullptr, 0, 0, "DoubleToAsc"},
    {host_obj_src_lib_doubleasc_glue_s + 4, GlueKind::c_routine, reinterpret_cast<const void *>(&lib_asc_to_double), nullptr, 0, 0, "AscToDouble"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
