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
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 0), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_continue"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 4), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_jump"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 8), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_menu_return"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 12), GlueKind::reg, AMOS_BIND_REG(interp_error_reg), nullptr, 0, 0, "Error"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 16), GlueKind::reg, AMOS_BIND_REG(interp_error_ext_reg), nullptr, 0, 0, "ErrorExt"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 20), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_next_float"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 28), GlueKind::record, AMOS_BIND_CALL(interp_omn_eff), nullptr, 0, 0, "interp_omn_record"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 32), GlueKind::test, AMOS_BIND_CALL(interp_test_pasaut), nullptr, 0, 0, "Test_PaSaut"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 36), GlueKind::test, AMOS_BIND_CALL(interp_test_normal), nullptr, 0, 0, "Test_Normal"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 40), GlueKind::test, AMOS_BIND_CALL(interp_test_force), nullptr, 0, 0, "Test_Force"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 48), GlueKind::call, AMOS_BIND_CALL(interp_in_lab), nullptr, 0, 0, "InLab"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 56), GlueKind::call, AMOS_BIND_CALL(interp_in_system), nullptr, 0, 0, "InSystem"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 64), GlueKind::call, AMOS_BIND_CALL(interp_in_edit), nullptr, 0, 0, "InEdit"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 72), GlueKind::call, AMOS_BIND_CALL(interp_in_direct), nullptr, 0, 0, "InDirect"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 80), GlueKind::call, AMOS_BIND_CALL(interp_in_break_on), nullptr, 0, 0, "InBreakOn"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 88), GlueKind::call, AMOS_BIND_CALL(interp_in_break_off), nullptr, 0, 0, "InBreakOff"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 96), GlueKind::call, AMOS_BIND_CALL(interp_in_on_break), nullptr, 0, 0, "InOnBreak"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 104), GlueKind::call, AMOS_BIND_CALL(interp_in_on_error), nullptr, 0, 0, "InOnError"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 112), GlueKind::call, AMOS_BIND_CALL(interp_in_resume_label), nullptr, 0, 0, "InResumeLabel"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 120), GlueKind::call, AMOS_BIND_CALL(interp_in_resume), nullptr, 0, 0, "InResume"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 128), GlueKind::call, AMOS_BIND_CALL(interp_in_resume_next), nullptr, 0, 0, "InResumeNext"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 136), GlueKind::call, AMOS_BIND_CALL(interp_in_trap), nullptr, 0, 0, "InTrap"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 144), GlueKind::call, AMOS_BIND_CALL(interp_in_every), nullptr, 0, 0, "InEvery"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 152), GlueKind::call, AMOS_BIND_CALL(interp_in_every_off), nullptr, 0, 0, "InEveryOff"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 160), GlueKind::call, AMOS_BIND_CALL(interp_in_every_on), nullptr, 0, 0, "InEveryOn"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 168), GlueKind::call, AMOS_BIND_CALL(interp_in_for), nullptr, 0, 0, "InFor"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 176), GlueKind::call, AMOS_BIND_CALL(interp_in_next), nullptr, 0, 0, "InNext"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 184), GlueKind::call, AMOS_BIND_CALL(interp_in_repeat), nullptr, 0, 0, "InRepeat"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 192), GlueKind::call, AMOS_BIND_CALL(interp_in_until), nullptr, 0, 0, "InUntil"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 200), GlueKind::call, AMOS_BIND_CALL(interp_in_while), nullptr, 0, 0, "InWhile"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 208), GlueKind::call, AMOS_BIND_CALL(interp_in_wend), nullptr, 0, 0, "InWend"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 216), GlueKind::call, AMOS_BIND_CALL(interp_in_do), nullptr, 0, 0, "InDo"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 224), GlueKind::call, AMOS_BIND_CALL(interp_in_loop), nullptr, 0, 0, "InLoop"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 232), GlueKind::call, AMOS_BIND_CALL(interp_in_exit), nullptr, 0, 0, "InExit"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 240), GlueKind::call, AMOS_BIND_CALL(interp_in_exit_if), nullptr, 0, 0, "InExitIf"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 248), GlueKind::call, AMOS_BIND_CALL(interp_in_if), nullptr, 0, 0, "InIf"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 256), GlueKind::call, AMOS_BIND_CALL(interp_in_else), nullptr, 0, 0, "InElse"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 264), GlueKind::call, AMOS_BIND_CALL(interp_in_else_if), nullptr, 0, 0, "InElseIf"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 272), GlueKind::call, AMOS_BIND_CALL(interp_in_gosub), nullptr, 0, 0, "InGosub"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 280), GlueKind::call, AMOS_BIND_CALL(interp_in_return), nullptr, 0, 0, "InReturn"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 288), GlueKind::call, AMOS_BIND_CALL(interp_in_pop), nullptr, 0, 0, "InPop"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 296), GlueKind::call, AMOS_BIND_CALL(interp_in_procedure), nullptr, 0, 0, "InProcedure"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 304), GlueKind::call, AMOS_BIND_CALL(interp_in_proc), nullptr, 0, 0, "InProc"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 308), GlueKind::call, AMOS_BIND_CALL(interp_call_proc), nullptr, 0, 0, "CallProc"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 316), GlueKind::call, AMOS_BIND_CALL(interp_in_end_proc), nullptr, 0, 0, "InEndProc"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 324), GlueKind::call, AMOS_BIND_CALL(interp_in_pop_proc), nullptr, 0, 0, "InPopProc"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 328), GlueKind::call, AMOS_BIND_CALL(interp_goto2), nullptr, 0, 0, "Goto2"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 336), GlueKind::call, AMOS_BIND_CALL(interp_in_goto), nullptr, 0, 0, "InGoto"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 344), GlueKind::call, AMOS_BIND_CALL(interp_in_on), nullptr, 0, 0, "InOn"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 352), GlueKind::call, AMOS_BIND_CALL(interp_in_restore), nullptr, 0, 0, "InRestore"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 360), GlueKind::call, AMOS_BIND_CALL(interp_in_apml), nullptr, 0, 0, "In_apml_"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 368), GlueKind::call, AMOS_BIND_CALL(interp_in_on_menu), nullptr, 0, 0, "InOnMenu"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 376), GlueKind::eval, AMOS_BIND_EVAL(interp_fn_err_trap), nullptr, 0, 0, "FnErrTrap"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 380), GlueKind::reg, AMOS_BIND_REG(interp_menu_procedure_reg), nullptr, 0, 0, "MenuProcedure"},
    {AMOS_ENTRY_AT(host_obj_src_interp_control_glue_s, 384), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_restart"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
