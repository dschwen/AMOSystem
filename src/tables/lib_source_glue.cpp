// src_lib_source_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_source_glue_s[20] = {
};
}

namespace amos::host {

void relocate_src_lib_source_glue_s()
{
}

const GlueEntry entries_src_lib_source_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_source_glue_s, 0), GlueKind::reg, AMOS_BIND_REG(lib_tk_find_line), nullptr, 0, 0, "tk_find_line"},
    {AMOS_ENTRY_AT(host_obj_src_lib_source_glue_s, 4), GlueKind::reg, AMOS_BIND_REG(lib_tk_find_first), nullptr, 0, 0, "tk_find_first"},
    {AMOS_ENTRY_AT(host_obj_src_lib_source_glue_s, 8), GlueKind::reg, AMOS_BIND_REG(lib_tk_size_line), nullptr, 0, 0, "tk_size_line"},
    {AMOS_ENTRY_AT(host_obj_src_lib_source_glue_s, 12), GlueKind::reg, AMOS_BIND_REG(lib_tk_editable), nullptr, 0, 0, "tk_editable"},
    {AMOS_ENTRY_AT(host_obj_src_lib_source_glue_s, 16), GlueKind::reg, AMOS_BIND_REG(lib_tk_find_address), nullptr, 0, 0, "tk_find_address"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
