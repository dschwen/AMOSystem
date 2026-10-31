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
    put(host_obj_src_lib_memory_glue_s + 4, 4, (uint32_t(uintptr_t(&lib_in_copy))));      // src/lib/memory_glue.s:9
    put(host_obj_src_lib_memory_glue_s + 24, 4, (uint32_t(uintptr_t(&lib_in_fill))));      // src/lib/memory_glue.s:10
    put(host_obj_src_lib_memory_glue_s + 44, 4, (uint32_t(uintptr_t(&lib_fn_hunt))));      // src/lib/memory_glue.s:11
    put(host_obj_src_lib_memory_glue_s + 64, 4, (uint32_t(uintptr_t(&lib_in_areg))));      // src/lib/memory_glue.s:12
    put(host_obj_src_lib_memory_glue_s + 84, 4, (uint32_t(uintptr_t(&lib_fn_areg))));      // src/lib/memory_glue.s:13
    put(host_obj_src_lib_memory_glue_s + 104, 4, (uint32_t(uintptr_t(&lib_in_dreg))));      // src/lib/memory_glue.s:14
    put(host_obj_src_lib_memory_glue_s + 124, 4, (uint32_t(uintptr_t(&lib_fn_dreg))));      // src/lib/memory_glue.s:15
    put(host_obj_src_lib_memory_glue_s + 144, 4, (uint32_t(uintptr_t(&lib_in_poke))));      // src/lib/memory_glue.s:16
    put(host_obj_src_lib_memory_glue_s + 164, 4, (uint32_t(uintptr_t(&lib_in_doke))));      // src/lib/memory_glue.s:17
    put(host_obj_src_lib_memory_glue_s + 184, 4, (uint32_t(uintptr_t(&lib_in_loke))));      // src/lib/memory_glue.s:18
    put(host_obj_src_lib_memory_glue_s + 204, 4, (uint32_t(uintptr_t(&lib_fn_peek))));      // src/lib/memory_glue.s:19
    put(host_obj_src_lib_memory_glue_s + 224, 4, (uint32_t(uintptr_t(&lib_fn_deek))));      // src/lib/memory_glue.s:20
    put(host_obj_src_lib_memory_glue_s + 244, 4, (uint32_t(uintptr_t(&lib_fn_leek))));      // src/lib/memory_glue.s:21
    put(host_obj_src_lib_memory_glue_s + 264, 4, (uint32_t(uintptr_t(&lib_in_poke_string))));      // src/lib/memory_glue.s:22
    put(host_obj_src_lib_memory_glue_s + 284, 4, (uint32_t(uintptr_t(&lib_fn_peek_string2))));      // src/lib/memory_glue.s:23
    put(host_obj_src_lib_memory_glue_s + 304, 4, (uint32_t(uintptr_t(&lib_fn_peek_string3))));      // src/lib/memory_glue.s:24
}

const GlueEntry entries_src_lib_memory_glue_s[] = {
    {host_obj_src_lib_memory_glue_s + 0, GlueKind::reg, reinterpret_cast<const void *>(&lib_trans_mem), nullptr, 0, 0, "TransMem"},
    {host_obj_src_lib_memory_glue_s + 20, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_copy), nullptr, 3, 3, "InCopy"},
    {host_obj_src_lib_memory_glue_s + 40, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_fill), nullptr, 3, 3, "InFill"},
    {host_obj_src_lib_memory_glue_s + 60, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_hunt), nullptr, 3, 0, "FnHunt"},
    {host_obj_src_lib_memory_glue_s + 80, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_areg), nullptr, 2, 3, "InAReg"},
    {host_obj_src_lib_memory_glue_s + 100, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_areg), nullptr, 1, 0, "FnAReg"},
    {host_obj_src_lib_memory_glue_s + 120, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_dreg), nullptr, 2, 3, "InDReg"},
    {host_obj_src_lib_memory_glue_s + 140, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_dreg), nullptr, 1, 0, "FnDReg"},
    {host_obj_src_lib_memory_glue_s + 160, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_poke), nullptr, 2, 3, "InPoke"},
    {host_obj_src_lib_memory_glue_s + 180, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_doke), nullptr, 2, 3, "InDoke"},
    {host_obj_src_lib_memory_glue_s + 200, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_loke), nullptr, 2, 3, "InLoke"},
    {host_obj_src_lib_memory_glue_s + 220, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_peek), nullptr, 1, 0, "FnPeek"},
    {host_obj_src_lib_memory_glue_s + 240, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_deek), nullptr, 1, 0, "FnDeek"},
    {host_obj_src_lib_memory_glue_s + 260, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_leek), nullptr, 1, 0, "FnLeek"},
    {host_obj_src_lib_memory_glue_s + 280, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_poke_string), nullptr, 2, 3, "InPokeD"},
    {host_obj_src_lib_memory_glue_s + 300, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_peek_string2), nullptr, 2, 2, "FnPeekD2"},
    {host_obj_src_lib_memory_glue_s + 320, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_peek_string3), nullptr, 3, 2, "FnPeekD3"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
