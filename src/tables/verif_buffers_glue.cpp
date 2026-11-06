// src_verif_buffers_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_verif_buffers_glue_s[40] = {
};
}

namespace amos::host {

void relocate_src_verif_buffers_glue_s()
{
}

const GlueEntry entries_src_verif_buffers_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 0), GlueKind::w, AMOS_BIND_W(ver_clear_variables), nullptr, 0, 4, "ver_clear_variables_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 4), GlueKind::w, AMOS_BIND_W(ver_clear_buffers), nullptr, 0, 4, "ver_clear_buffers_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 8), GlueKind::w, AMOS_BIND_W(ver_stack_reserve), nullptr, 0, 4, "ver_stack_reserve_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 12), GlueKind::w, AMOS_BIND_W(ver_direct_variables), nullptr, 0, 4, "ver_direct_variables_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 16), GlueKind::w, AMOS_BIND_W(ver_resize_variables), nullptr, 0, 4, "ver_resize_variables_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 20), GlueKind::w, AMOS_BIND_W(ver_resize_names), nullptr, 0, 4, "ver_resize_names_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 24), GlueKind::w, AMOS_BIND_W(ver_includes_clear), nullptr, 0, 4, "ver_includes_clear_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 28), GlueKind::w, AMOS_BIND_W(ver_includes_address), nullptr, 0, 4, "ver_includes_address_entry"},
    {AMOS_ENTRY_AT(host_obj_src_verif_buffers_glue_s, 36), GlueKind::w, AMOS_BIND_W(ver_equ_free), nullptr, 0, 4, "ver_equ_free_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
