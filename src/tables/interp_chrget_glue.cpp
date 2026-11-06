// src_interp_chrget_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_interp_chrget_glue_s[24] = {
};
}

namespace amos::host {

void relocate_src_interp_chrget_glue_s()
{
}

const GlueEntry entries_src_interp_chrget_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_interp_chrget_glue_s, 0), GlueKind::hand, AMOS_BIND_HAND(host_New_ChrGet), nullptr, 0, 0, "New_ChrGet"},
    {AMOS_ENTRY_AT(host_obj_src_interp_chrget_glue_s, 4), GlueKind::hand, AMOS_BIND_HAND(host_interp_iret), nullptr, 0, 0, "interp_iret"},
    {AMOS_ENTRY_AT(host_obj_src_interp_chrget_glue_s, 8), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_exec_value"},
    {AMOS_ENTRY_AT(host_obj_src_interp_chrget_glue_s, 12), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_step_instruction"},
    {AMOS_ENTRY_AT(host_obj_src_interp_chrget_glue_s, 16), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_exec"},
    {AMOS_ENTRY_AT(host_obj_src_interp_chrget_glue_s, 20), GlueKind::reg, AMOS_BIND_REG(interp_set_chr_patch), nullptr, 0, 0, "SetChrPatch"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
