// src_w_input_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_input_glue_s[12] = {
};
}

namespace amos::host {

void relocate_src_w_input_glue_s()
{
}

const GlueEntry entries_src_w_input_glue_s[] = {
    {host_obj_src_w_input_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&w_io_handler), nullptr, 0, 4, "w_input"},
    {host_obj_src_w_input_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&w_sy_send_fake_event), nullptr, 0, 4, "w_send_fake_event"},
    {host_obj_src_w_input_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&w_sy_test_cyclic), nullptr, 0, 4, "w_test_cyclic"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
