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
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 0), GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_reserve), nullptr, 0, 0, "w_mem_reserve"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 4), GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_fast_clear), nullptr, 0, 0, "w_mem_fast_clear"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 8), GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_fast), nullptr, 0, 0, "w_mem_fast"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 12), GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_chip_clear), nullptr, 0, 0, "w_mem_chip_clear"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 16), GlueKind::w_a0, AMOS_BIND_W(w_sy_mem_chip), nullptr, 0, 0, "w_mem_chip"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_sy_mem_free), nullptr, 0, 4, "w_mem_free"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_sy_add_routine), nullptr, 0, 4, "w_add_routine"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_sy_add_flush_routine), nullptr, 0, 4, "w_add_flush_routine"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 32), GlueKind::w, AMOS_BIND_W(w_sy_call_routines), nullptr, 0, 4, "w_call_routines"},
    {AMOS_ENTRY_AT(host_obj_src_w_memory_glue_s, 36), GlueKind::w, AMOS_BIND_W(w_sy_mem_flush), nullptr, 0, 4, "w_mem_flush"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
