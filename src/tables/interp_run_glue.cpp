// src_interp_run_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_interp_run_glue_s[240] = {
    0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,3,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
}

namespace amos::host {

void relocate_src_interp_run_glue_s()
{
    put(host_obj_src_interp_run_glue_s + 0, 4, (uint32_t(uintptr_t(&interp_fn_prg_state))));      // src/interp/run_glue.s:10
    put(host_obj_src_interp_run_glue_s + 20, 4, (uint32_t(uintptr_t(&interp_fn_prg_under))));      // src/interp/run_glue.s:11
    put(host_obj_src_interp_run_glue_s + 40, 4, (uint32_t(uintptr_t(&interp_in_run0))));      // src/interp/run_glue.s:12
    put(host_obj_src_interp_run_glue_s + 60, 4, (uint32_t(uintptr_t(&interp_in_run1))));      // src/interp/run_glue.s:13
    put(host_obj_src_interp_run_glue_s + 80, 4, (uint32_t(uintptr_t(&interp_in_prun))));      // src/interp/run_glue.s:14
    put(host_obj_src_interp_run_glue_s + 100, 4, (uint32_t(uintptr_t(&interp_in_ask_editor1))));      // src/interp/run_glue.s:15
    put(host_obj_src_interp_run_glue_s + 120, 4, (uint32_t(uintptr_t(&interp_in_ask_editor2))));      // src/interp/run_glue.s:16
    put(host_obj_src_interp_run_glue_s + 140, 4, (uint32_t(uintptr_t(&interp_in_ask_editor3))));      // src/interp/run_glue.s:17
    put(host_obj_src_interp_run_glue_s + 160, 4, (uint32_t(uintptr_t(&interp_in_call_editor1))));      // src/interp/run_glue.s:18
    put(host_obj_src_interp_run_glue_s + 180, 4, (uint32_t(uintptr_t(&interp_in_call_editor2))));      // src/interp/run_glue.s:19
    put(host_obj_src_interp_run_glue_s + 200, 4, (uint32_t(uintptr_t(&interp_in_call_editor3))));      // src/interp/run_glue.s:20
}

const GlueEntry entries_src_interp_run_glue_s[] = {
    {host_obj_src_interp_run_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&interp_fn_prg_state), nullptr, 0, 0, "FnPrgState"},
    {host_obj_src_interp_run_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&interp_fn_prg_under), nullptr, 0, 0, "FnPrgUnder"},
    {host_obj_src_interp_run_glue_s + 56, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_run0), nullptr, 0, 3, "InRun0"},
    {host_obj_src_interp_run_glue_s + 76, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_run1), nullptr, 1, 3, "InRun1"},
    {host_obj_src_interp_run_glue_s + 96, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_prun), nullptr, 1, 3, "InPRun"},
    {host_obj_src_interp_run_glue_s + 116, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_ask_editor1), nullptr, 1, 3, "InAskEditor1"},
    {host_obj_src_interp_run_glue_s + 136, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_ask_editor2), nullptr, 2, 3, "InAskEditor2"},
    {host_obj_src_interp_run_glue_s + 156, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_ask_editor3), nullptr, 3, 3, "InAskEditor3"},
    {host_obj_src_interp_run_glue_s + 176, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_call_editor1), nullptr, 1, 3, "InCallEditor1"},
    {host_obj_src_interp_run_glue_s + 196, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_call_editor2), nullptr, 2, 3, "InCallEditor2"},
    {host_obj_src_interp_run_glue_s + 216, GlueKind::libcall, reinterpret_cast<const void *>(&interp_in_call_editor3), nullptr, 3, 3, "InCallEditor3"},
    {host_obj_src_interp_run_glue_s + 220, GlueKind::missing, nullptr, nullptr, 0, 0, "RunII"},
    {host_obj_src_interp_run_glue_s + 224, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_prun_run"},
    {host_obj_src_interp_run_glue_s + 228, GlueKind::missing, nullptr, nullptr, 0, 0, "PRun_Back"},
    {host_obj_src_interp_run_glue_s + 232, GlueKind::hand, reinterpret_cast<const void *>(&host_interp_prun_errors), nullptr, 0, 0, "interp_prun_errors"},
    {host_obj_src_interp_run_glue_s + 236, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_zap_in"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
