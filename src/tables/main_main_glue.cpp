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
    put(host_obj_src_main_main_glue_s + 24, 4, (uint32_t(uintptr_t(&main_in_monitor))));      // src/main/main_glue.s:16
    put(host_obj_src_main_main_glue_s + 44, 4, (uint32_t(uintptr_t(&main_in_kill_editor))));      // src/main/main_glue.s:17
    put(host_obj_src_main_main_glue_s + 64, 4, (uint32_t(uintptr_t(&main_in_close_editor))));      // src/main/main_glue.s:18
}

const GlueEntry entries_src_main_main_glue_s[] = {
    {host_obj_src_main_main_glue_s + 0, GlueKind::reg, reinterpret_cast<const void *>(&main_edit_load), nullptr, 0, 0, "Edit_Load"},
    {host_obj_src_main_main_glue_s + 4, GlueKind::reg, reinterpret_cast<const void *>(&main_edit_free), nullptr, 0, 0, "Edit_Free"},
    {host_obj_src_main_main_glue_s + 8, GlueKind::reg, reinterpret_cast<const void *>(&main_mon_load), nullptr, 0, 0, "Mon_Load"},
    {host_obj_src_main_main_glue_s + 12, GlueKind::reg, reinterpret_cast<const void *>(&main_mon_free), nullptr, 0, 0, "Mon_Free"},
    {host_obj_src_main_main_glue_s + 16, GlueKind::reg, reinterpret_cast<const void *>(&main_mem_maximum), nullptr, 0, 0, "MemMaximum"},
    {host_obj_src_main_main_glue_s + 20, GlueKind::reg, reinterpret_cast<const void *>(&main_mem_del_banks), nullptr, 0, 0, "MemDelBanks"},
    {host_obj_src_main_main_glue_s + 40, GlueKind::libcall, reinterpret_cast<const void *>(&main_in_monitor), nullptr, 0, 3, "InMonitor"},
    {host_obj_src_main_main_glue_s + 60, GlueKind::libcall, reinterpret_cast<const void *>(&main_in_kill_editor), nullptr, 0, 3, "InKillEditor"},
    {host_obj_src_main_main_glue_s + 80, GlueKind::libcall, reinterpret_cast<const void *>(&main_in_close_editor), nullptr, 0, 3, "InCloseEditor"},
    {host_obj_src_main_main_glue_s + 84, GlueKind::reg, reinterpret_cast<const void *>(&main_run_error_run_only_reg), nullptr, 0, 0, "main_run_error_run_only"},
    {host_obj_src_main_main_glue_s + 88, GlueKind::reg, reinterpret_cast<const void *>(&main_run_error_no_editor_reg), nullptr, 0, 0, "main_run_error_no_editor"},
    {host_obj_src_main_main_glue_s + 92, GlueKind::reg, reinterpret_cast<const void *>(&main_ram_fast), nullptr, 0, 0, "RamFast"},
    {host_obj_src_main_main_glue_s + 96, GlueKind::reg, reinterpret_cast<const void *>(&main_ram_chip), nullptr, 0, 0, "RamChip"},
    {host_obj_src_main_main_glue_s + 100, GlueKind::reg, reinterpret_cast<const void *>(&main_ram_fast2), nullptr, 0, 0, "RamFast2"},
    {host_obj_src_main_main_glue_s + 104, GlueKind::reg, reinterpret_cast<const void *>(&main_ram_chip2), nullptr, 0, 0, "RamChip2"},
    {host_obj_src_main_main_glue_s + 108, GlueKind::reg, reinterpret_cast<const void *>(&main_ram_free), nullptr, 0, 0, "RamFree"},
    {host_obj_src_main_main_glue_s + 112, GlueKind::reg, reinterpret_cast<const void *>(&main_res_temp_buffer), nullptr, 0, 0, "ResTempBuffer"},
    {host_obj_src_main_main_glue_s + 116, GlueKind::reg, reinterpret_cast<const void *>(&main_math_close), nullptr, 0, 0, "Math_Close"},
    {host_obj_src_main_main_glue_s + 120, GlueKind::reg, reinterpret_cast<const void *>(&main_sys_wait_mul), nullptr, 0, 0, "Sys_WaitMul"},
    {host_obj_src_main_main_glue_s + 124, GlueKind::reg, reinterpret_cast<const void *>(&main_def_get_message), nullptr, 0, 0, "Def_GetMessage"},
    {host_obj_src_main_main_glue_s + 128, GlueKind::reg, reinterpret_cast<const void *>(&main_sys_get_message), nullptr, 0, 0, "Sys_GetMessage"},
    {host_obj_src_main_main_glue_s + 132, GlueKind::reg, reinterpret_cast<const void *>(&main_get_message), nullptr, 0, 0, "GetMessage"},
    {host_obj_src_main_main_glue_s + 136, GlueKind::reg, reinterpret_cast<const void *>(&main_sys_add_path), nullptr, 0, 0, "Sys_AddPath"},
    {host_obj_src_main_main_glue_s + 140, GlueKind::reg, reinterpret_cast<const void *>(&main_sys_uncode), nullptr, 0, 0, "Sys_UnCode"},
    {host_obj_src_main_main_glue_s + 144, GlueKind::reg, reinterpret_cast<const void *>(&main_sys_clear_cache), nullptr, 0, 0, "Sys_ClearCache"},
    {host_obj_src_main_main_glue_s + 148, GlueKind::reg, reinterpret_cast<const void *>(&main_w_option), nullptr, 0, 0, "WOption"},
    {host_obj_src_main_main_glue_s + 152, GlueKind::reg, reinterpret_cast<const void *>(&main_recop), nullptr, 0, 0, "ReCop"},
    {host_obj_src_main_main_glue_s + 156, GlueKind::reg, reinterpret_cast<const void *>(&main_ask_dir), nullptr, 0, 0, "AskDir"},
    {host_obj_src_main_main_glue_s + 160, GlueKind::reg, reinterpret_cast<const void *>(&main_ask_dir2), nullptr, 0, 0, "AskDir2"},
    {host_obj_src_main_main_glue_s + 168, GlueKind::record, reinterpret_cast<const void *>(&main_free_temp_buffer), nullptr, 0, 0, "main_temp_on_clear"},
    {host_obj_src_main_main_glue_s + 176, GlueKind::record, reinterpret_cast<const void *>(&main_free_temp_buffer), nullptr, 0, 0, "main_temp_on_error"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
