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
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 0, interp_fn_prg_state);      // src/interp/run_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 20, interp_fn_prg_under);      // src/interp/run_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 40, interp_in_run0);      // src/interp/run_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 60, interp_in_run1);      // src/interp/run_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 80, interp_in_prun);      // src/interp/run_glue.s:14
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 100, interp_in_ask_editor1);      // src/interp/run_glue.s:15
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 120, interp_in_ask_editor2);      // src/interp/run_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 140, interp_in_ask_editor3);      // src/interp/run_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 160, interp_in_call_editor1);      // src/interp/run_glue.s:18
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 180, interp_in_call_editor2);      // src/interp/run_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_interp_run_glue_s + 200, interp_in_call_editor3);      // src/interp/run_glue.s:20
}

const GlueEntry entries_src_interp_run_glue_s[] = {
    {host_obj_src_interp_run_glue_s + 16, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_fn_prg_state), nullptr, 0, 0, "FnPrgState"},
    {host_obj_src_interp_run_glue_s + 36, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_fn_prg_under), nullptr, 0, 0, "FnPrgUnder"},
    {host_obj_src_interp_run_glue_s + 56, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_run0), nullptr, 0, 3, "InRun0"},
    {host_obj_src_interp_run_glue_s + 76, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_run1), nullptr, 1, 3, "InRun1"},
    {host_obj_src_interp_run_glue_s + 96, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_prun), nullptr, 1, 3, "InPRun"},
    {host_obj_src_interp_run_glue_s + 116, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_ask_editor1), nullptr, 1, 3, "InAskEditor1"},
    {host_obj_src_interp_run_glue_s + 136, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_ask_editor2), nullptr, 2, 3, "InAskEditor2"},
    {host_obj_src_interp_run_glue_s + 156, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_ask_editor3), nullptr, 3, 3, "InAskEditor3"},
    {host_obj_src_interp_run_glue_s + 176, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_call_editor1), nullptr, 1, 3, "InCallEditor1"},
    {host_obj_src_interp_run_glue_s + 196, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_call_editor2), nullptr, 2, 3, "InCallEditor2"},
    {host_obj_src_interp_run_glue_s + 216, GlueKind::libcall, AMOS_BIND_LIBRARY(interp_in_call_editor3), nullptr, 3, 3, "InCallEditor3"},
    {host_obj_src_interp_run_glue_s + 220, GlueKind::missing, nullptr, nullptr, 0, 0, "RunII"},
    {host_obj_src_interp_run_glue_s + 224, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_prun_run"},
    {host_obj_src_interp_run_glue_s + 228, GlueKind::missing, nullptr, nullptr, 0, 0, "PRun_Back"},
    {host_obj_src_interp_run_glue_s + 232, GlueKind::hand, AMOS_BIND_HAND(host_interp_prun_errors), nullptr, 0, 0, "interp_prun_errors"},
    {host_obj_src_interp_run_glue_s + 236, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_zap_in"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
