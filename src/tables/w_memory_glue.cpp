// src_w_memory_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_memory_glue_s[40] = {
};
}

namespace amos::host {

void relocate_src_w_memory_glue_s()
{
}

const GlueEntry entries_src_w_memory_glue_s[] = {
    {host_obj_src_w_memory_glue_s + 0, GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_reserve), nullptr, 0, 0, "w_mem_reserve"},
    {host_obj_src_w_memory_glue_s + 4, GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_fast_clear), nullptr, 0, 0, "w_mem_fast_clear"},
    {host_obj_src_w_memory_glue_s + 8, GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_fast), nullptr, 0, 0, "w_mem_fast"},
    {host_obj_src_w_memory_glue_s + 12, GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_chip_clear), nullptr, 0, 0, "w_mem_chip_clear"},
    {host_obj_src_w_memory_glue_s + 16, GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_chip), nullptr, 0, 0, "w_mem_chip"},
    {host_obj_src_w_memory_glue_s + 20, GlueKind::w, AMOS_BIND_W(w_sy_mem_free), nullptr, 0, 4, "w_mem_free"},
    {host_obj_src_w_memory_glue_s + 24, GlueKind::w, AMOS_BIND_W(w_sy_add_routine), nullptr, 0, 4, "w_add_routine"},
    {host_obj_src_w_memory_glue_s + 28, GlueKind::w, AMOS_BIND_W(w_sy_add_flush_routine), nullptr, 0, 4, "w_add_flush_routine"},
    {host_obj_src_w_memory_glue_s + 32, GlueKind::w, AMOS_BIND_W(w_sy_call_routines), nullptr, 0, 4, "w_call_routines"},
    {host_obj_src_w_memory_glue_s + 36, GlueKind::w, AMOS_BIND_W(w_sy_mem_flush), nullptr, 0, 4, "w_mem_flush"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
