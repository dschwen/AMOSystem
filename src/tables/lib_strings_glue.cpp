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
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 0, lib_fn_upper);      // src/lib/strings_glue.s:8
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 20, lib_fn_lower);      // src/lib/strings_glue.s:9
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 40, lib_fn_space);      // src/lib/strings_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 60, lib_fn_string);      // src/lib/strings_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 80, lib_fn_free);      // src/lib/strings_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 100, lib_fn_flip);      // src/lib/strings_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 120, lib_fn_len);      // src/lib/strings_glue.s:14
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 140, lib_fn_asc);      // src/lib/strings_glue.s:15
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 160, lib_fn_chr);      // src/lib/strings_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 180, lib_fn_tab);      // src/lib/strings_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 200, lib_fn_cleft);      // src/lib/strings_glue.s:18
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 220, lib_fn_cright);      // src/lib/strings_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 240, lib_fn_cup);      // src/lib/strings_glue.s:20
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 260, lib_fn_cdown);      // src/lib/strings_glue.s:21
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 280, lib_fn_pen);      // src/lib/strings_glue.s:22
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 300, lib_fn_paper);      // src/lib/strings_glue.s:23
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 320, lib_fn_at);      // src/lib/strings_glue.s:24
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 340, lib_fn_cmove);      // src/lib/strings_glue.s:25
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 360, lib_fn_repeat);      // src/lib/strings_glue.s:26
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 380, lib_fn_border);      // src/lib/strings_glue.s:27
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 400, lib_fn_zone);      // src/lib/strings_glue.s:28
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 420, lib_fn_hex1);      // src/lib/strings_glue.s:29
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 440, lib_fn_hex2);      // src/lib/strings_glue.s:30
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 460, lib_fn_bin1);      // src/lib/strings_glue.s:31
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 480, lib_fn_bin2);      // src/lib/strings_glue.s:32
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 500, lib_fn_instr2);      // src/lib/strings_glue.s:33
    AMOS_LIBRARY_METADATA(host_obj_src_lib_strings_glue_s + 520, lib_fn_instr3);      // src/lib/strings_glue.s:34
}

const GlueEntry entries_src_lib_strings_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 16), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_upper), nullptr, 1, 2, "FnUpper"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 36), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_lower), nullptr, 1, 2, "FnLower"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 56), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_space), nullptr, 1, 2, "FnSpace"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 76), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_string), nullptr, 2, 2, "FnString"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 96), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_free), nullptr, 0, 0, "FnFree"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 116), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_flip), nullptr, 1, 2, "FnFlip"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 136), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_len), nullptr, 1, 0, "FnLen"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 156), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_asc), nullptr, 1, 0, "FnAsc"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 176), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_chr), nullptr, 1, 2, "FnChr"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 196), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_tab), nullptr, 0, 2, "FnTab"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 216), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_cleft), nullptr, 0, 2, "FnCleft"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 236), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_cright), nullptr, 0, 2, "FnCright"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 256), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_cup), nullptr, 0, 2, "FnCup"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 276), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_cdown), nullptr, 0, 2, "FnCdown"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 296), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_pen), nullptr, 1, 2, "FnPenD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 316), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_paper), nullptr, 1, 2, "FnPaperD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 336), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_at), nullptr, 2, 2, "FnAt"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 356), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_cmove), nullptr, 2, 2, "FnCMoveD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 376), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_repeat), nullptr, 2, 2, "FnRepeat"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 396), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_border), nullptr, 2, 2, "FnBorderD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 416), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_zone), nullptr, 2, 2, "FnZoneD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 436), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_hex1), nullptr, 1, 2, "FnHex1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 456), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_hex2), nullptr, 2, 2, "FnHex2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 476), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_bin1), nullptr, 1, 2, "FnBin1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 496), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_bin2), nullptr, 2, 2, "FnBin2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 516), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_instr2), nullptr, 2, 0, "FnInstr2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 536), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_instr3), nullptr, 3, 0, "FnInstr3"},
    {AMOS_ENTRY_AT(host_obj_src_lib_strings_glue_s, 540), GlueKind::reg, AMOS_BIND_REG(lib_instr_find_reg), nullptr, 0, 0, "InstrFind"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
