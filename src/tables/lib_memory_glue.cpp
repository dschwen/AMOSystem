// src_lib_memory_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_memory_glue_s[324] = {
    0,0,0,0,0,0,0,0,0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,3,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,3,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_memory_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 4, lib_in_copy);      // src/lib/memory_glue.s:9
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 24, lib_in_fill);      // src/lib/memory_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 44, lib_fn_hunt);      // src/lib/memory_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 64, lib_in_areg);      // src/lib/memory_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 84, lib_fn_areg);      // src/lib/memory_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 104, lib_in_dreg);      // src/lib/memory_glue.s:14
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 124, lib_fn_dreg);      // src/lib/memory_glue.s:15
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 144, lib_in_poke);      // src/lib/memory_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 164, lib_in_doke);      // src/lib/memory_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 184, lib_in_loke);      // src/lib/memory_glue.s:18
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 204, lib_fn_peek);      // src/lib/memory_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 224, lib_fn_deek);      // src/lib/memory_glue.s:20
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 244, lib_fn_leek);      // src/lib/memory_glue.s:21
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 264, lib_in_poke_string);      // src/lib/memory_glue.s:22
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 284, lib_fn_peek_string2);      // src/lib/memory_glue.s:23
    AMOS_LIBRARY_METADATA(host_obj_src_lib_memory_glue_s + 304, lib_fn_peek_string3);      // src/lib/memory_glue.s:24
}

const GlueEntry entries_src_lib_memory_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 0), GlueKind::reg, AMOS_BIND_REG(lib_trans_mem), nullptr, 0, 0, "TransMem"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 20), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_copy), nullptr, 3, 3, "InCopy"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 40), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_fill), nullptr, 3, 3, "InFill"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 60), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_hunt), nullptr, 3, 0, "FnHunt"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 80), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_areg), nullptr, 2, 3, "InAReg"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 100), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_areg), nullptr, 1, 0, "FnAReg"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 120), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_dreg), nullptr, 2, 3, "InDReg"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 140), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_dreg), nullptr, 1, 0, "FnDReg"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 160), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_poke), nullptr, 2, 3, "InPoke"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 180), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_doke), nullptr, 2, 3, "InDoke"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 200), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_loke), nullptr, 2, 3, "InLoke"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 220), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_peek), nullptr, 1, 0, "FnPeek"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 240), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_deek), nullptr, 1, 0, "FnDeek"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 260), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_leek), nullptr, 1, 0, "FnLeek"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 280), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_poke_string), nullptr, 2, 3, "InPokeD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 300), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_peek_string2), nullptr, 2, 2, "FnPeekD2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_memory_glue_s, 320), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_peek_string3), nullptr, 3, 2, "FnPeekD3"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
