// src_monitor_shell_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_monitor_shell_glue_s[44] = {
};
}

namespace amos::host {

void relocate_src_monitor_shell_glue_s()
{
}

const GlueEntry entries_src_monitor_shell_glue_s[] = {
    {host_obj_src_monitor_shell_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&mon_tests), nullptr, 0, 4, "mon_tests_entry"},
    {host_obj_src_monitor_shell_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&mon_reloop), nullptr, 0, 4, "mon_reloop_entry"},
    {host_obj_src_monitor_shell_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&mon_step0), nullptr, 0, 4, "mon_step0_entry"},
    {host_obj_src_monitor_shell_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&mon_step1), nullptr, 0, 4, "mon_step1_entry"},
    {host_obj_src_monitor_shell_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&mon_step2), nullptr, 0, 4, "mon_step2_entry"},
    {host_obj_src_monitor_shell_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&mon_step3), nullptr, 0, 4, "mon_step3_entry"},
    {host_obj_src_monitor_shell_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&mon_initialise), nullptr, 0, 4, "mon_initialise_entry"},
    {host_obj_src_monitor_shell_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&mon_reinit), nullptr, 0, 4, "mon_reinit_entry"},
    {host_obj_src_monitor_shell_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&mon_quit), nullptr, 0, 4, "mon_quit_entry"},
    {host_obj_src_monitor_shell_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&mon_split_token), nullptr, 0, 4, "mon_split_hook"},
    {host_obj_src_monitor_shell_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&mon_nothing), nullptr, 0, 4, "mon_nothing_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
