// src_lib_system_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_system_glue_s[368] = {
    0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,4,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_system_glue_s()
{
    put(host_obj_src_lib_system_glue_s + 0, 4, (uint32_t(uintptr_t(&lib_fn_errn))));      // src/lib/system_glue.s:8
    put(host_obj_src_lib_system_glue_s + 20, 4, (uint32_t(uintptr_t(&lib_fn_err))));      // src/lib/system_glue.s:9
    put(host_obj_src_lib_system_glue_s + 40, 4, (uint32_t(uintptr_t(&lib_fn_str_int))));      // src/lib/system_glue.s:10
    put(host_obj_src_lib_system_glue_s + 60, 4, (uint32_t(uintptr_t(&lib_in_radian))));      // src/lib/system_glue.s:11
    put(host_obj_src_lib_system_glue_s + 80, 4, (uint32_t(uintptr_t(&lib_in_degree))));      // src/lib/system_glue.s:12
    put(host_obj_src_lib_system_glue_s + 100, 4, (uint32_t(uintptr_t(&lib_in_fix))));      // src/lib/system_glue.s:13
    put(host_obj_src_lib_system_glue_s + 120, 4, (uint32_t(uintptr_t(&lib_fn_rnd))));      // src/lib/system_glue.s:14
    put(host_obj_src_lib_system_glue_s + 144, 4, (uint32_t(uintptr_t(&lib_in_randomize))));      // src/lib/system_glue.s:16
    put(host_obj_src_lib_system_glue_s + 164, 4, (uint32_t(uintptr_t(&lib_in_timer))));      // src/lib/system_glue.s:17
    put(host_obj_src_lib_system_glue_s + 184, 4, (uint32_t(uintptr_t(&lib_fn_timer))));      // src/lib/system_glue.s:18
    put(host_obj_src_lib_system_glue_s + 204, 4, (uint32_t(uintptr_t(&lib_fn_pi))));      // src/lib/system_glue.s:19
    put(host_obj_src_lib_system_glue_s + 224, 4, (uint32_t(uintptr_t(&lib_in_wait))));      // src/lib/system_glue.s:20
    put(host_obj_src_lib_system_glue_s + 248, 4, (uint32_t(uintptr_t(&lib_in_multi_wait))));      // src/lib/system_glue.s:22
    put(host_obj_src_lib_system_glue_s + 268, 4, (uint32_t(uintptr_t(&lib_in_wait_vbl))));      // src/lib/system_glue.s:23
    put(host_obj_src_lib_system_glue_s + 288, 4, (uint32_t(uintptr_t(&lib_in_wait_key))));      // src/lib/system_glue.s:24
    put(host_obj_src_lib_system_glue_s + 308, 4, (uint32_t(uintptr_t(&lib_in_key_speed))));      // src/lib/system_glue.s:25
    put(host_obj_src_lib_system_glue_s + 328, 4, (uint32_t(uintptr_t(&lib_fn_param_string))));      // src/lib/system_glue.s:26
    put(host_obj_src_lib_system_glue_s + 348, 4, (uint32_t(uintptr_t(&lib_fn_param_int))));      // src/lib/system_glue.s:27
}

const GlueEntry entries_src_lib_system_glue_s[] = {
    {host_obj_src_lib_system_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_errn), nullptr, 0, 0, "FnErrn"},
    {host_obj_src_lib_system_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_err), nullptr, 1, 2, "FnErrD"},
    {host_obj_src_lib_system_glue_s + 56, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_str_int), nullptr, 1, 2, "FnStrE"},
    {host_obj_src_lib_system_glue_s + 76, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_radian), nullptr, 0, 3, "InRadian"},
    {host_obj_src_lib_system_glue_s + 96, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_degree), nullptr, 0, 3, "InDegree"},
    {host_obj_src_lib_system_glue_s + 116, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_fix), nullptr, 1, 3, "InFix"},
    {host_obj_src_lib_system_glue_s + 136, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_rnd), nullptr, 1, 0, "FnRnd"},
    {host_obj_src_lib_system_glue_s + 140, GlueKind::reg, reinterpret_cast<const void *>(&lib_mulu32), nullptr, 0, 0, "Mulu32"},
    {host_obj_src_lib_system_glue_s + 160, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_randomize), nullptr, 1, 3, "InRandom"},
    {host_obj_src_lib_system_glue_s + 180, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_timer), nullptr, 1, 3, "InTimer"},
    {host_obj_src_lib_system_glue_s + 200, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_timer), nullptr, 0, 0, "FnTimer"},
    {host_obj_src_lib_system_glue_s + 220, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_pi), nullptr, 0, 4, "FnPi"},
    {host_obj_src_lib_system_glue_s + 240, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_wait), nullptr, 1, 3, "InWait"},
    {host_obj_src_lib_system_glue_s + 244, GlueKind::reg, reinterpret_cast<const void *>(&lib_wait_rout), nullptr, 0, 0, "WaitRout"},
    {host_obj_src_lib_system_glue_s + 264, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_multi_wait), nullptr, 0, 3, "InMWait"},
    {host_obj_src_lib_system_glue_s + 284, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_wait_vbl), nullptr, 0, 3, "InWtVbl"},
    {host_obj_src_lib_system_glue_s + 304, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_wait_key), nullptr, 0, 3, "InWtKy"},
    {host_obj_src_lib_system_glue_s + 324, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_key_speed), nullptr, 2, 3, "InKeySpeed"},
    {host_obj_src_lib_system_glue_s + 344, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_param_string), nullptr, 0, 2, "FnParamS"},
    {host_obj_src_lib_system_glue_s + 364, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_param_int), nullptr, 0, 0, "FnParamE"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
