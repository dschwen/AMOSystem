// src_w_copper_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_copper_glue_s[32] = {
};
}

namespace amos::host {

void relocate_src_w_copper_glue_s()
{
}

const GlueEntry entries_src_w_copper_glue_s[] = {
    {host_obj_src_w_copper_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_swap), nullptr, 0, 4, "w_cop_swap"},
    {host_obj_src_w_copper_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_swap_lists), nullptr, 0, 4, "w_cop_swap_lists"},
    {host_obj_src_w_copper_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_reset), nullptr, 0, 4, "w_cop_reset"},
    {host_obj_src_w_copper_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_wait), nullptr, 0, 4, "w_cop_wait"},
    {host_obj_src_w_copper_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_move), nullptr, 0, 4, "w_cop_move"},
    {host_obj_src_w_copper_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_movel), nullptr, 0, 4, "w_cop_movel"},
    {host_obj_src_w_copper_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&w_ec_cop_base), nullptr, 0, 4, "w_cop_base"},
    {host_obj_src_w_copper_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&w_ec_copper_on_off), nullptr, 0, 4, "w_copper_on_off"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
