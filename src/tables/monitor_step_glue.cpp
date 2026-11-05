// src_monitor_step_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_monitor_step_glue_s[8] = {
};
}

namespace amos::host {

void relocate_src_monitor_step_glue_s()
{
}

const GlueEntry entries_src_monitor_step_glue_s[] = {
    {host_obj_src_monitor_step_glue_s + 0, GlueKind::w, AMOS_BIND_W(mon_step_show), nullptr, 0, 4, "mon_step_show_entry"},
    {host_obj_src_monitor_step_glue_s + 4, GlueKind::w, AMOS_BIND_W(mon_test_error), nullptr, 0, 4, "mon_test_error_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
