// src_main_main_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_main_main_glue_s[180] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0};
}

namespace amos::host {

void relocate_src_main_main_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_main_main_glue_s + 24, main_in_monitor);      // src/main/main_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_main_main_glue_s + 44, main_in_kill_editor);      // src/main/main_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_main_main_glue_s + 64, main_in_close_editor);      // src/main/main_glue.s:18
}

const GlueEntry entries_src_main_main_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 0), GlueKind::reg, AMOS_BIND_REG(main_edit_load), nullptr, 0, 0, "Edit_Load"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 4), GlueKind::reg, AMOS_BIND_REG(main_edit_free), nullptr, 0, 0, "Edit_Free"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 8), GlueKind::reg, AMOS_BIND_REG(main_mon_load), nullptr, 0, 0, "Mon_Load"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 12), GlueKind::reg, AMOS_BIND_REG(main_mon_free), nullptr, 0, 0, "Mon_Free"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 16), GlueKind::reg, AMOS_BIND_REG(main_mem_maximum), nullptr, 0, 0, "MemMaximum"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 20), GlueKind::reg, AMOS_BIND_REG(main_mem_del_banks), nullptr, 0, 0, "MemDelBanks"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 40), GlueKind::libcall, AMOS_BIND_LIBRARY(main_in_monitor), nullptr, 0, 3, "InMonitor"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 60), GlueKind::libcall, AMOS_BIND_LIBRARY(main_in_kill_editor), nullptr, 0, 3, "InKillEditor"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 80), GlueKind::libcall, AMOS_BIND_LIBRARY(main_in_close_editor), nullptr, 0, 3, "InCloseEditor"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 84), GlueKind::reg, AMOS_BIND_REG(main_run_error_run_only_reg), nullptr, 0, 0, "main_run_error_run_only"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 88), GlueKind::reg, AMOS_BIND_REG(main_run_error_no_editor_reg), nullptr, 0, 0, "main_run_error_no_editor"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 92), GlueKind::reg, AMOS_BIND_REG(main_ram_fast), nullptr, 0, 0, "RamFast"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 96), GlueKind::reg, AMOS_BIND_REG(main_ram_chip), nullptr, 0, 0, "RamChip"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 100), GlueKind::reg, AMOS_BIND_REG(main_ram_fast2), nullptr, 0, 0, "RamFast2"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 104), GlueKind::reg, AMOS_BIND_REG(main_ram_chip2), nullptr, 0, 0, "RamChip2"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 108), GlueKind::reg, AMOS_BIND_REG(main_ram_free), nullptr, 0, 0, "RamFree"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 112), GlueKind::reg, AMOS_BIND_REG(main_res_temp_buffer), nullptr, 0, 0, "ResTempBuffer"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 116), GlueKind::reg, AMOS_BIND_REG(main_math_close), nullptr, 0, 0, "Math_Close"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 120), GlueKind::reg, AMOS_BIND_REG(main_sys_wait_mul), nullptr, 0, 0, "Sys_WaitMul"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 124), GlueKind::reg, AMOS_BIND_REG(main_def_get_message), nullptr, 0, 0, "Def_GetMessage"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 128), GlueKind::reg, AMOS_BIND_REG(main_sys_get_message), nullptr, 0, 0, "Sys_GetMessage"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 132), GlueKind::reg, AMOS_BIND_REG(main_get_message), nullptr, 0, 0, "GetMessage"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 136), GlueKind::reg, AMOS_BIND_REG(main_sys_add_path), nullptr, 0, 0, "Sys_AddPath"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 140), GlueKind::reg, AMOS_BIND_REG(main_sys_uncode), nullptr, 0, 0, "Sys_UnCode"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 144), GlueKind::reg, AMOS_BIND_REG(main_sys_clear_cache), nullptr, 0, 0, "Sys_ClearCache"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 148), GlueKind::reg, AMOS_BIND_REG(main_w_option), nullptr, 0, 0, "WOption"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 152), GlueKind::reg, AMOS_BIND_REG(main_recop), nullptr, 0, 0, "ReCop"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 156), GlueKind::reg, AMOS_BIND_REG(main_ask_dir), nullptr, 0, 0, "AskDir"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 160), GlueKind::reg, AMOS_BIND_REG(main_ask_dir2), nullptr, 0, 0, "AskDir2"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 168), GlueKind::record, AMOS_BIND_CALL(main_free_temp_buffer), nullptr, 0, 0, "main_temp_on_clear"},
    {AMOS_ENTRY_AT(host_obj_src_main_main_glue_s, 176), GlueKind::record, AMOS_BIND_CALL(main_free_temp_buffer), nullptr, 0, 0, "main_temp_on_error"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
