// src_lib_strings_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_strings_glue_s[544] = {
    0,0,0,0,0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,2,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,2,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,2,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,2,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,3,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_strings_glue_s()
{
    put(host_obj_src_lib_strings_glue_s + 0, 4, (uint32_t(uintptr_t(&lib_fn_upper))));      // src/lib/strings_glue.s:8
    put(host_obj_src_lib_strings_glue_s + 20, 4, (uint32_t(uintptr_t(&lib_fn_lower))));      // src/lib/strings_glue.s:9
    put(host_obj_src_lib_strings_glue_s + 40, 4, (uint32_t(uintptr_t(&lib_fn_space))));      // src/lib/strings_glue.s:10
    put(host_obj_src_lib_strings_glue_s + 60, 4, (uint32_t(uintptr_t(&lib_fn_string))));      // src/lib/strings_glue.s:11
    put(host_obj_src_lib_strings_glue_s + 80, 4, (uint32_t(uintptr_t(&lib_fn_free))));      // src/lib/strings_glue.s:12
    put(host_obj_src_lib_strings_glue_s + 100, 4, (uint32_t(uintptr_t(&lib_fn_flip))));      // src/lib/strings_glue.s:13
    put(host_obj_src_lib_strings_glue_s + 120, 4, (uint32_t(uintptr_t(&lib_fn_len))));      // src/lib/strings_glue.s:14
    put(host_obj_src_lib_strings_glue_s + 140, 4, (uint32_t(uintptr_t(&lib_fn_asc))));      // src/lib/strings_glue.s:15
    put(host_obj_src_lib_strings_glue_s + 160, 4, (uint32_t(uintptr_t(&lib_fn_chr))));      // src/lib/strings_glue.s:16
    put(host_obj_src_lib_strings_glue_s + 180, 4, (uint32_t(uintptr_t(&lib_fn_tab))));      // src/lib/strings_glue.s:17
    put(host_obj_src_lib_strings_glue_s + 200, 4, (uint32_t(uintptr_t(&lib_fn_cleft))));      // src/lib/strings_glue.s:18
    put(host_obj_src_lib_strings_glue_s + 220, 4, (uint32_t(uintptr_t(&lib_fn_cright))));      // src/lib/strings_glue.s:19
    put(host_obj_src_lib_strings_glue_s + 240, 4, (uint32_t(uintptr_t(&lib_fn_cup))));      // src/lib/strings_glue.s:20
    put(host_obj_src_lib_strings_glue_s + 260, 4, (uint32_t(uintptr_t(&lib_fn_cdown))));      // src/lib/strings_glue.s:21
    put(host_obj_src_lib_strings_glue_s + 280, 4, (uint32_t(uintptr_t(&lib_fn_pen))));      // src/lib/strings_glue.s:22
    put(host_obj_src_lib_strings_glue_s + 300, 4, (uint32_t(uintptr_t(&lib_fn_paper))));      // src/lib/strings_glue.s:23
    put(host_obj_src_lib_strings_glue_s + 320, 4, (uint32_t(uintptr_t(&lib_fn_at))));      // src/lib/strings_glue.s:24
    put(host_obj_src_lib_strings_glue_s + 340, 4, (uint32_t(uintptr_t(&lib_fn_cmove))));      // src/lib/strings_glue.s:25
    put(host_obj_src_lib_strings_glue_s + 360, 4, (uint32_t(uintptr_t(&lib_fn_repeat))));      // src/lib/strings_glue.s:26
    put(host_obj_src_lib_strings_glue_s + 380, 4, (uint32_t(uintptr_t(&lib_fn_border))));      // src/lib/strings_glue.s:27
    put(host_obj_src_lib_strings_glue_s + 400, 4, (uint32_t(uintptr_t(&lib_fn_zone))));      // src/lib/strings_glue.s:28
    put(host_obj_src_lib_strings_glue_s + 420, 4, (uint32_t(uintptr_t(&lib_fn_hex1))));      // src/lib/strings_glue.s:29
    put(host_obj_src_lib_strings_glue_s + 440, 4, (uint32_t(uintptr_t(&lib_fn_hex2))));      // src/lib/strings_glue.s:30
    put(host_obj_src_lib_strings_glue_s + 460, 4, (uint32_t(uintptr_t(&lib_fn_bin1))));      // src/lib/strings_glue.s:31
    put(host_obj_src_lib_strings_glue_s + 480, 4, (uint32_t(uintptr_t(&lib_fn_bin2))));      // src/lib/strings_glue.s:32
    put(host_obj_src_lib_strings_glue_s + 500, 4, (uint32_t(uintptr_t(&lib_fn_instr2))));      // src/lib/strings_glue.s:33
    put(host_obj_src_lib_strings_glue_s + 520, 4, (uint32_t(uintptr_t(&lib_fn_instr3))));      // src/lib/strings_glue.s:34
}

const GlueEntry entries_src_lib_strings_glue_s[] = {
    {host_obj_src_lib_strings_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_upper), nullptr, 1, 2, "FnUpper"},
    {host_obj_src_lib_strings_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_lower), nullptr, 1, 2, "FnLower"},
    {host_obj_src_lib_strings_glue_s + 56, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_space), nullptr, 1, 2, "FnSpace"},
    {host_obj_src_lib_strings_glue_s + 76, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_string), nullptr, 2, 2, "FnString"},
    {host_obj_src_lib_strings_glue_s + 96, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_free), nullptr, 0, 0, "FnFree"},
    {host_obj_src_lib_strings_glue_s + 116, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_flip), nullptr, 1, 2, "FnFlip"},
    {host_obj_src_lib_strings_glue_s + 136, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_len), nullptr, 1, 0, "FnLen"},
    {host_obj_src_lib_strings_glue_s + 156, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_asc), nullptr, 1, 0, "FnAsc"},
    {host_obj_src_lib_strings_glue_s + 176, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_chr), nullptr, 1, 2, "FnChr"},
    {host_obj_src_lib_strings_glue_s + 196, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_tab), nullptr, 0, 2, "FnTab"},
    {host_obj_src_lib_strings_glue_s + 216, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_cleft), nullptr, 0, 2, "FnCleft"},
    {host_obj_src_lib_strings_glue_s + 236, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_cright), nullptr, 0, 2, "FnCright"},
    {host_obj_src_lib_strings_glue_s + 256, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_cup), nullptr, 0, 2, "FnCup"},
    {host_obj_src_lib_strings_glue_s + 276, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_cdown), nullptr, 0, 2, "FnCdown"},
    {host_obj_src_lib_strings_glue_s + 296, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_pen), nullptr, 1, 2, "FnPenD"},
    {host_obj_src_lib_strings_glue_s + 316, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_paper), nullptr, 1, 2, "FnPaperD"},
    {host_obj_src_lib_strings_glue_s + 336, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_at), nullptr, 2, 2, "FnAt"},
    {host_obj_src_lib_strings_glue_s + 356, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_cmove), nullptr, 2, 2, "FnCMoveD"},
    {host_obj_src_lib_strings_glue_s + 376, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_repeat), nullptr, 2, 2, "FnRepeat"},
    {host_obj_src_lib_strings_glue_s + 396, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_border), nullptr, 2, 2, "FnBorderD"},
    {host_obj_src_lib_strings_glue_s + 416, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_zone), nullptr, 2, 2, "FnZoneD"},
    {host_obj_src_lib_strings_glue_s + 436, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_hex1), nullptr, 1, 2, "FnHex1"},
    {host_obj_src_lib_strings_glue_s + 456, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_hex2), nullptr, 2, 2, "FnHex2"},
    {host_obj_src_lib_strings_glue_s + 476, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_bin1), nullptr, 1, 2, "FnBin1"},
    {host_obj_src_lib_strings_glue_s + 496, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_bin2), nullptr, 2, 2, "FnBin2"},
    {host_obj_src_lib_strings_glue_s + 516, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_instr2), nullptr, 2, 0, "FnInstr2"},
    {host_obj_src_lib_strings_glue_s + 536, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_instr3), nullptr, 3, 0, "FnInstr3"},
    {host_obj_src_lib_strings_glue_s + 540, GlueKind::reg, reinterpret_cast<const void *>(&lib_instr_find_reg), nullptr, 0, 0, "InstrFind"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
