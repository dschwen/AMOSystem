// src_lib_convert_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_convert_glue_s[16] = {
};
}

namespace amos::host {

void relocate_src_lib_convert_glue_s()
{
}

const GlueEntry entries_src_lib_convert_glue_s[] = {
    {host_obj_src_lib_convert_glue_s + 0, GlueKind::reg, AMOS_BIND_REG(lib_long_to_hex_reg), nullptr, 0, 0, "LongToHex"},
    {host_obj_src_lib_convert_glue_s + 4, GlueKind::reg, AMOS_BIND_REG(lib_long_to_bin_reg), nullptr, 0, 0, "LongToBin"},
    {host_obj_src_lib_convert_glue_s + 8, GlueKind::reg, AMOS_BIND_REG(lib_long_to_asc), nullptr, 0, 0, "LongToAsc"},
    {host_obj_src_lib_convert_glue_s + 12, GlueKind::reg, AMOS_BIND_REG(lib_long_to_dec), nullptr, 0, 0, "LongToDec"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
