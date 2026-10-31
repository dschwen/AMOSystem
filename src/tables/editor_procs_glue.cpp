// src_editor_procs_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_procs_glue_s[20] = {
};
}

namespace amos::host {

void relocate_src_editor_procs_glue_s()
{
}

const GlueEntry entries_src_editor_procs_glue_s[] = {
    {host_obj_src_editor_procs_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_procs_open), nullptr, 0, 4, "ed_procs_open_entry"},
    {host_obj_src_editor_procs_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_procs_close), nullptr, 0, 4, "ed_procs_close_entry"},
    {host_obj_src_editor_procs_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_procs), nullptr, 0, 4, "ed_procs_entry"},
    {host_obj_src_editor_procs_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_proc_open), nullptr, 0, 4, "ed_proc_open_entry"},
    {host_obj_src_editor_procs_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_clear_error_proc), nullptr, 0, 4, "ed_clear_error_proc_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
