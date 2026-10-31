// src_interp_control_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_interp_control_glue_s[388] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,0,0,0,0,
    71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,0};
}

namespace amos::host {

void relocate_src_interp_control_glue_s()
{
}

const GlueEntry entries_src_interp_control_glue_s[] = {
    {host_obj_src_interp_control_glue_s + 0, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_continue"},
    {host_obj_src_interp_control_glue_s + 4, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_jump"},
    {host_obj_src_interp_control_glue_s + 8, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_menu_return"},
    {host_obj_src_interp_control_glue_s + 12, GlueKind::reg, reinterpret_cast<const void *>(&interp_error_reg), nullptr, 0, 0, "Error"},
    {host_obj_src_interp_control_glue_s + 16, GlueKind::reg, reinterpret_cast<const void *>(&interp_error_ext_reg), nullptr, 0, 0, "ErrorExt"},
    {host_obj_src_interp_control_glue_s + 20, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_next_float"},
    {host_obj_src_interp_control_glue_s + 28, GlueKind::record, reinterpret_cast<const void *>(&interp_omn_eff), nullptr, 0, 0, "interp_omn_record"},
    {host_obj_src_interp_control_glue_s + 32, GlueKind::test, reinterpret_cast<const void *>(&interp_test_pasaut), nullptr, 0, 0, "Test_PaSaut"},
    {host_obj_src_interp_control_glue_s + 36, GlueKind::test, reinterpret_cast<const void *>(&interp_test_normal), nullptr, 0, 0, "Test_Normal"},
    {host_obj_src_interp_control_glue_s + 40, GlueKind::test, reinterpret_cast<const void *>(&interp_test_force), nullptr, 0, 0, "Test_Force"},
    {host_obj_src_interp_control_glue_s + 48, GlueKind::call, reinterpret_cast<const void *>(&interp_in_lab), nullptr, 0, 0, "InLab"},
    {host_obj_src_interp_control_glue_s + 56, GlueKind::call, reinterpret_cast<const void *>(&interp_in_system), nullptr, 0, 0, "InSystem"},
    {host_obj_src_interp_control_glue_s + 64, GlueKind::call, reinterpret_cast<const void *>(&interp_in_edit), nullptr, 0, 0, "InEdit"},
    {host_obj_src_interp_control_glue_s + 72, GlueKind::call, reinterpret_cast<const void *>(&interp_in_direct), nullptr, 0, 0, "InDirect"},
    {host_obj_src_interp_control_glue_s + 80, GlueKind::call, reinterpret_cast<const void *>(&interp_in_break_on), nullptr, 0, 0, "InBreakOn"},
    {host_obj_src_interp_control_glue_s + 88, GlueKind::call, reinterpret_cast<const void *>(&interp_in_break_off), nullptr, 0, 0, "InBreakOff"},
    {host_obj_src_interp_control_glue_s + 96, GlueKind::call, reinterpret_cast<const void *>(&interp_in_on_break), nullptr, 0, 0, "InOnBreak"},
    {host_obj_src_interp_control_glue_s + 104, GlueKind::call, reinterpret_cast<const void *>(&interp_in_on_error), nullptr, 0, 0, "InOnError"},
    {host_obj_src_interp_control_glue_s + 112, GlueKind::call, reinterpret_cast<const void *>(&interp_in_resume_label), nullptr, 0, 0, "InResumeLabel"},
    {host_obj_src_interp_control_glue_s + 120, GlueKind::call, reinterpret_cast<const void *>(&interp_in_resume), nullptr, 0, 0, "InResume"},
    {host_obj_src_interp_control_glue_s + 128, GlueKind::call, reinterpret_cast<const void *>(&interp_in_resume_next), nullptr, 0, 0, "InResumeNext"},
    {host_obj_src_interp_control_glue_s + 136, GlueKind::call, reinterpret_cast<const void *>(&interp_in_trap), nullptr, 0, 0, "InTrap"},
    {host_obj_src_interp_control_glue_s + 144, GlueKind::call, reinterpret_cast<const void *>(&interp_in_every), nullptr, 0, 0, "InEvery"},
    {host_obj_src_interp_control_glue_s + 152, GlueKind::call, reinterpret_cast<const void *>(&interp_in_every_off), nullptr, 0, 0, "InEveryOff"},
    {host_obj_src_interp_control_glue_s + 160, GlueKind::call, reinterpret_cast<const void *>(&interp_in_every_on), nullptr, 0, 0, "InEveryOn"},
    {host_obj_src_interp_control_glue_s + 168, GlueKind::call, reinterpret_cast<const void *>(&interp_in_for), nullptr, 0, 0, "InFor"},
    {host_obj_src_interp_control_glue_s + 176, GlueKind::call, reinterpret_cast<const void *>(&interp_in_next), nullptr, 0, 0, "InNext"},
    {host_obj_src_interp_control_glue_s + 184, GlueKind::call, reinterpret_cast<const void *>(&interp_in_repeat), nullptr, 0, 0, "InRepeat"},
    {host_obj_src_interp_control_glue_s + 192, GlueKind::call, reinterpret_cast<const void *>(&interp_in_until), nullptr, 0, 0, "InUntil"},
    {host_obj_src_interp_control_glue_s + 200, GlueKind::call, reinterpret_cast<const void *>(&interp_in_while), nullptr, 0, 0, "InWhile"},
    {host_obj_src_interp_control_glue_s + 208, GlueKind::call, reinterpret_cast<const void *>(&interp_in_wend), nullptr, 0, 0, "InWend"},
    {host_obj_src_interp_control_glue_s + 216, GlueKind::call, reinterpret_cast<const void *>(&interp_in_do), nullptr, 0, 0, "InDo"},
    {host_obj_src_interp_control_glue_s + 224, GlueKind::call, reinterpret_cast<const void *>(&interp_in_loop), nullptr, 0, 0, "InLoop"},
    {host_obj_src_interp_control_glue_s + 232, GlueKind::call, reinterpret_cast<const void *>(&interp_in_exit), nullptr, 0, 0, "InExit"},
    {host_obj_src_interp_control_glue_s + 240, GlueKind::call, reinterpret_cast<const void *>(&interp_in_exit_if), nullptr, 0, 0, "InExitIf"},
    {host_obj_src_interp_control_glue_s + 248, GlueKind::call, reinterpret_cast<const void *>(&interp_in_if), nullptr, 0, 0, "InIf"},
    {host_obj_src_interp_control_glue_s + 256, GlueKind::call, reinterpret_cast<const void *>(&interp_in_else), nullptr, 0, 0, "InElse"},
    {host_obj_src_interp_control_glue_s + 264, GlueKind::call, reinterpret_cast<const void *>(&interp_in_else_if), nullptr, 0, 0, "InElseIf"},
    {host_obj_src_interp_control_glue_s + 272, GlueKind::call, reinterpret_cast<const void *>(&interp_in_gosub), nullptr, 0, 0, "InGosub"},
    {host_obj_src_interp_control_glue_s + 280, GlueKind::call, reinterpret_cast<const void *>(&interp_in_return), nullptr, 0, 0, "InReturn"},
    {host_obj_src_interp_control_glue_s + 288, GlueKind::call, reinterpret_cast<const void *>(&interp_in_pop), nullptr, 0, 0, "InPop"},
    {host_obj_src_interp_control_glue_s + 296, GlueKind::call, reinterpret_cast<const void *>(&interp_in_procedure), nullptr, 0, 0, "InProcedure"},
    {host_obj_src_interp_control_glue_s + 304, GlueKind::call, reinterpret_cast<const void *>(&interp_in_proc), nullptr, 0, 0, "InProc"},
    {host_obj_src_interp_control_glue_s + 308, GlueKind::call, reinterpret_cast<const void *>(&interp_call_proc), nullptr, 0, 0, "CallProc"},
    {host_obj_src_interp_control_glue_s + 316, GlueKind::call, reinterpret_cast<const void *>(&interp_in_end_proc), nullptr, 0, 0, "InEndProc"},
    {host_obj_src_interp_control_glue_s + 324, GlueKind::call, reinterpret_cast<const void *>(&interp_in_pop_proc), nullptr, 0, 0, "InPopProc"},
    {host_obj_src_interp_control_glue_s + 328, GlueKind::call, reinterpret_cast<const void *>(&interp_goto2), nullptr, 0, 0, "Goto2"},
    {host_obj_src_interp_control_glue_s + 336, GlueKind::call, reinterpret_cast<const void *>(&interp_in_goto), nullptr, 0, 0, "InGoto"},
    {host_obj_src_interp_control_glue_s + 344, GlueKind::call, reinterpret_cast<const void *>(&interp_in_on), nullptr, 0, 0, "InOn"},
    {host_obj_src_interp_control_glue_s + 352, GlueKind::call, reinterpret_cast<const void *>(&interp_in_restore), nullptr, 0, 0, "InRestore"},
    {host_obj_src_interp_control_glue_s + 360, GlueKind::call, reinterpret_cast<const void *>(&interp_in_apml), nullptr, 0, 0, "In_apml_"},
    {host_obj_src_interp_control_glue_s + 368, GlueKind::call, reinterpret_cast<const void *>(&interp_in_on_menu), nullptr, 0, 0, "InOnMenu"},
    {host_obj_src_interp_control_glue_s + 376, GlueKind::eval, reinterpret_cast<const void *>(&interp_fn_err_trap), nullptr, 0, 0, "FnErrTrap"},
    {host_obj_src_interp_control_glue_s + 380, GlueKind::reg, reinterpret_cast<const void *>(&interp_menu_procedure_reg), nullptr, 0, 0, "MenuProcedure"},
    {host_obj_src_interp_control_glue_s + 384, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_restart"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
