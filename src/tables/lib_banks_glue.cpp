// src_lib_banks_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_banks_glue_s[604] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_banks_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 124, lib_in_list_bank);      // src/lib/banks_glue.s:44
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 144, lib_in_erase);      // src/lib/banks_glue.s:45
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 164, lib_in_erase_temp);      // src/lib/banks_glue.s:46
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 184, lib_in_erase_all);      // src/lib/banks_glue.s:47
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 204, lib_in_bank_swap);      // src/lib/banks_glue.s:48
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 224, lib_fn_bstart);      // src/lib/banks_glue.s:49
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 244, lib_fn_blength);      // src/lib/banks_glue.s:50
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 264, lib_in_bgrab);      // src/lib/banks_glue.s:51
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 284, lib_in_bsend);      // src/lib/banks_glue.s:52
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 304, lib_in_ins_sprite);      // src/lib/banks_glue.s:53
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 324, lib_in_ins_icon);      // src/lib/banks_glue.s:54
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 344, lib_in_del_icon1);      // src/lib/banks_glue.s:55
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 364, lib_in_del_icon2);      // src/lib/banks_glue.s:56
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 384, lib_in_del_sprite1);      // src/lib/banks_glue.s:57
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 404, lib_in_del_sprite2);      // src/lib/banks_glue.s:58
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 424, lib_in_res_work);      // src/lib/banks_glue.s:59
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 444, lib_in_res_chip_work);      // src/lib/banks_glue.s:60
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 464, lib_in_res_data);      // src/lib/banks_glue.s:61
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 484, lib_in_res_chip_data);      // src/lib/banks_glue.s:62
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 504, lib_in_bank_schrink);      // src/lib/banks_glue.s:63
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 524, lib_fn_start);      // src/lib/banks_glue.s:64
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 544, lib_fn_length);      // src/lib/banks_glue.s:65
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 564, lib_fn_chip_free);      // src/lib/banks_glue.s:66
    AMOS_LIBRARY_METADATA(host_obj_src_lib_banks_glue_s + 584, lib_fn_fast_free);      // src/lib/banks_glue.s:67
}

const GlueEntry entries_src_lib_banks_glue_s[] = {
    {host_obj_src_lib_banks_glue_s + 0, GlueKind::reg, AMOS_BIND_REG(lib_lst_chip_new), nullptr, 0, 0, "Lst.ChipNew"},
    {host_obj_src_lib_banks_glue_s + 4, GlueKind::reg, AMOS_BIND_REG(lib_lst_new), nullptr, 0, 0, "Lst.New"},
    {host_obj_src_lib_banks_glue_s + 8, GlueKind::reg, AMOS_BIND_REG(lib_lst_cree), nullptr, 0, 0, "Lst.Cree"},
    {host_obj_src_lib_banks_glue_s + 12, GlueKind::reg, AMOS_BIND_REG(lib_lst_del_all), nullptr, 0, 0, "Lst.DelAll"},
    {host_obj_src_lib_banks_glue_s + 16, GlueKind::reg, AMOS_BIND_REG(lib_lst_del), nullptr, 0, 0, "Lst.Del"},
    {host_obj_src_lib_banks_glue_s + 20, GlueKind::reg, AMOS_BIND_REG(lib_lst_insert), nullptr, 0, 0, "Lst.Insert"},
    {host_obj_src_lib_banks_glue_s + 24, GlueKind::reg, AMOS_BIND_REG(lib_lst_remove), nullptr, 0, 0, "Lst.Remove"},
    {host_obj_src_lib_banks_glue_s + 28, GlueKind::reg, AMOS_BIND_REG(lib_bnk_prev_program), nullptr, 0, 0, "Bnk.PrevProgram"},
    {host_obj_src_lib_banks_glue_s + 32, GlueKind::reg, AMOS_BIND_REG(lib_bnk_cur_program), nullptr, 0, 0, "Bnk.CurProgram"},
    {host_obj_src_lib_banks_glue_s + 36, GlueKind::reg, AMOS_BIND_REG(lib_bnk_get_adr), nullptr, 0, 0, "Bnk.GetAdr"},
    {host_obj_src_lib_banks_glue_s + 40, GlueKind::reg, AMOS_BIND_REG(lib_bnk_get_bobs), nullptr, 0, 0, "Bnk.GetBobs"},
    {host_obj_src_lib_banks_glue_s + 44, GlueKind::reg, AMOS_BIND_REG(lib_bnk_get_icons), nullptr, 0, 0, "Bnk.GetIcons"},
    {host_obj_src_lib_banks_glue_s + 48, GlueKind::reg, AMOS_BIND_REG(lib_bnk_eff), nullptr, 0, 0, "Bnk.Eff"},
    {host_obj_src_lib_banks_glue_s + 52, GlueKind::reg, AMOS_BIND_REG(lib_bnk_eff_a0), nullptr, 0, 0, "Bnk.EffA0"},
    {host_obj_src_lib_banks_glue_s + 56, GlueKind::reg, AMOS_BIND_REG(lib_bnk_eff_bob_a0), nullptr, 0, 0, "Bnk.EffBobA0"},
    {host_obj_src_lib_banks_glue_s + 60, GlueKind::reg, AMOS_BIND_REG(lib_bnk_eff_all), nullptr, 0, 0, "Bnk.EffAll"},
    {host_obj_src_lib_banks_glue_s + 64, GlueKind::reg, AMOS_BIND_REG(lib_bnk_eff_temp), nullptr, 0, 0, "Bnk.EffTemp"},
    {host_obj_src_lib_banks_glue_s + 68, GlueKind::reg, AMOS_BIND_REG(lib_bnk_or_adr), nullptr, 0, 0, "Bnk.OrAdr"},
    {host_obj_src_lib_banks_glue_s + 72, GlueKind::reg, AMOS_BIND_REG(lib_bnk_ad_bob), nullptr, 0, 0, "Bnk.AdBob"},
    {host_obj_src_lib_banks_glue_s + 76, GlueKind::reg, AMOS_BIND_REG(lib_bnk_ad_icon), nullptr, 0, 0, "Bnk.AdIcon"},
    {host_obj_src_lib_banks_glue_s + 80, GlueKind::reg, AMOS_BIND_REG(lib_bnk_res_ico), nullptr, 0, 0, "Bnk.ResIco"},
    {host_obj_src_lib_banks_glue_s + 84, GlueKind::reg, AMOS_BIND_REG(lib_bnk_res_bob), nullptr, 0, 0, "Bnk.ResBob"},
    {host_obj_src_lib_banks_glue_s + 88, GlueKind::reg, AMOS_BIND_REG(lib_bnk_ric), nullptr, 0, 0, "Bnk.Ric"},
    {host_obj_src_lib_banks_glue_s + 92, GlueKind::reg, AMOS_BIND_REG(lib_bnk_schrink), nullptr, 0, 0, "Bnk.Schrink"},
    {host_obj_src_lib_banks_glue_s + 96, GlueKind::reg, AMOS_BIND_REG(lib_bnk_ins_bob), nullptr, 0, 0, "Bnk.InsBob"},
    {host_obj_src_lib_banks_glue_s + 100, GlueKind::reg, AMOS_BIND_REG(lib_bnk_del_bob), nullptr, 0, 0, "Bnk.DelBob"},
    {host_obj_src_lib_banks_glue_s + 104, GlueKind::reg, AMOS_BIND_REG(lib_bnk_un_rev), nullptr, 0, 0, "Bnk.UnRev"},
    {host_obj_src_lib_banks_glue_s + 108, GlueKind::reg, AMOS_BIND_REG(lib_bnk_reserve), nullptr, 0, 0, "Bnk.Reserve"},
    {host_obj_src_lib_banks_glue_s + 112, GlueKind::reg, AMOS_BIND_REG(lib_bnk_get_length), nullptr, 0, 0, "Bnk.GetLength"},
    {host_obj_src_lib_banks_glue_s + 116, GlueKind::reg, AMOS_BIND_REG(lib_bnk_length), nullptr, 0, 0, "Bnk.Length"},
    {host_obj_src_lib_banks_glue_s + 120, GlueKind::reg, AMOS_BIND_REG(lib_bnk_change), nullptr, 0, 0, "Bnk.Change"},
    {host_obj_src_lib_banks_glue_s + 140, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_list_bank), nullptr, 0, 3, "InListBank"},
    {host_obj_src_lib_banks_glue_s + 160, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_erase), nullptr, 1, 3, "InErase"},
    {host_obj_src_lib_banks_glue_s + 180, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_erase_temp), nullptr, 0, 3, "InEraseTemp"},
    {host_obj_src_lib_banks_glue_s + 200, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_erase_all), nullptr, 0, 3, "InEraseAll"},
    {host_obj_src_lib_banks_glue_s + 220, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_bank_swap), nullptr, 2, 3, "InBankSwap"},
    {host_obj_src_lib_banks_glue_s + 240, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_bstart), nullptr, 1, 0, "FnBStart"},
    {host_obj_src_lib_banks_glue_s + 260, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_blength), nullptr, 1, 0, "FnBLength"},
    {host_obj_src_lib_banks_glue_s + 280, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_bgrab), nullptr, 1, 3, "InBGrab"},
    {host_obj_src_lib_banks_glue_s + 300, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_bsend), nullptr, 1, 3, "InBSend"},
    {host_obj_src_lib_banks_glue_s + 320, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_ins_sprite), nullptr, 1, 3, "InInsSprite"},
    {host_obj_src_lib_banks_glue_s + 340, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_ins_icon), nullptr, 1, 3, "InInsIcon"},
    {host_obj_src_lib_banks_glue_s + 360, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_del_icon1), nullptr, 1, 3, "InDelIcon1"},
    {host_obj_src_lib_banks_glue_s + 380, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_del_icon2), nullptr, 2, 3, "InDelIcon2"},
    {host_obj_src_lib_banks_glue_s + 400, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_del_sprite1), nullptr, 1, 3, "InDelSprite1"},
    {host_obj_src_lib_banks_glue_s + 420, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_del_sprite2), nullptr, 2, 3, "InDelSprite2"},
    {host_obj_src_lib_banks_glue_s + 440, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_res_work), nullptr, 2, 3, "InResWork"},
    {host_obj_src_lib_banks_glue_s + 460, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_res_chip_work), nullptr, 2, 3, "InResChipWork"},
    {host_obj_src_lib_banks_glue_s + 480, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_res_data), nullptr, 2, 3, "InResData"},
    {host_obj_src_lib_banks_glue_s + 500, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_res_chip_data), nullptr, 2, 3, "InResChipData"},
    {host_obj_src_lib_banks_glue_s + 520, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_bank_schrink), nullptr, 2, 3, "InBankSchrink"},
    {host_obj_src_lib_banks_glue_s + 540, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_start), nullptr, 1, 0, "FnStart"},
    {host_obj_src_lib_banks_glue_s + 560, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_length), nullptr, 1, 0, "FnLength"},
    {host_obj_src_lib_banks_glue_s + 580, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_chip_free), nullptr, 0, 0, "FnChipFree"},
    {host_obj_src_lib_banks_glue_s + 600, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_fast_free), nullptr, 0, 0, "FnFastFree"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
