// src_verif_main_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_verif_main_glue_s[20] = {
};
}

namespace amos::host {

void relocate_src_verif_main_glue_s()
{
}

const GlueEntry entries_src_verif_main_glue_s[] = {
    {host_obj_src_verif_main_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ver_test), nullptr, 0, 4, "ver_test_entry"},
    {host_obj_src_verif_main_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ver_direct_test), nullptr, 0, 4, "ver_direct_test_entry"},
    {host_obj_src_verif_main_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ver_program_test), nullptr, 0, 4, "ver_program_test_entry"},
    {host_obj_src_verif_main_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ver_free_tables), nullptr, 0, 4, "ver_free_tables_entry"},
    {host_obj_src_verif_main_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ver_tables_run), nullptr, 0, 4, "ver_tables_run_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
