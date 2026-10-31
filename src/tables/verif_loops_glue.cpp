// src_verif_loops_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_verif_loops_glue_s[72] = {
};
}

namespace amos::host {

void relocate_src_verif_loops_glue_s()
{
}

const GlueEntry entries_src_verif_loops_glue_s[] = {
    {host_obj_src_verif_loops_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ver_for_two_c), nullptr, 0, 4, "ver_for_two"},
    {host_obj_src_verif_loops_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ver_repeat_two_c), nullptr, 0, 4, "ver_repeat_two"},
    {host_obj_src_verif_loops_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ver_while_two_c), nullptr, 0, 4, "ver_while_two"},
    {host_obj_src_verif_loops_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ver_do_two_c), nullptr, 0, 4, "ver_do_two"},
    {host_obj_src_verif_loops_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ver_exit_two_c), nullptr, 0, 4, "ver_exit_two"},
    {host_obj_src_verif_loops_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ver_if_two_c), nullptr, 0, 4, "ver_if_two"},
    {host_obj_src_verif_loops_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ver_if_then_two_c), nullptr, 0, 4, "ver_if_then_two"},
    {host_obj_src_verif_loops_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ver_else_if_two_c), nullptr, 0, 4, "ver_else_if_two"},
    {host_obj_src_verif_loops_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ver_else_two_c), nullptr, 0, 4, "ver_else_two"},
    {host_obj_src_verif_loops_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ver_else_then_two_c), nullptr, 0, 4, "ver_else_then_two"},
    {host_obj_src_verif_loops_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ver_goto_two_c), nullptr, 0, 4, "ver_goto_two"},
    {host_obj_src_verif_loops_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ver_on_goto_two_c), nullptr, 0, 4, "ver_on_goto_two"},
    {host_obj_src_verif_loops_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ver_next_alone_c), nullptr, 0, 4, "ver_next_alone"},
    {host_obj_src_verif_loops_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ver_until_alone_c), nullptr, 0, 4, "ver_until_alone"},
    {host_obj_src_verif_loops_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ver_wend_alone_c), nullptr, 0, 4, "ver_wend_alone"},
    {host_obj_src_verif_loops_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ver_loop_alone_c), nullptr, 0, 4, "ver_loop_alone"},
    {host_obj_src_verif_loops_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ver_else_alone_c), nullptr, 0, 4, "ver_else_alone"},
    {host_obj_src_verif_loops_glue_s + 68, GlueKind::w, reinterpret_cast<const void *>(&ver_end_if_alone_c), nullptr, 0, 4, "ver_end_if_alone"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
