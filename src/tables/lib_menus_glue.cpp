// src_lib_menus_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_menus_glue_s[628] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_menus_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 40, lib_in_on_menu_on);      // src/lib/menus_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 60, lib_in_on_menu_on);      // src/lib/menus_glue.s:20
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 80, lib_in_on_menu_del);      // src/lib/menus_glue.s:21
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 100, lib_in_menu_to_bank);      // src/lib/menus_glue.s:22
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 120, lib_in_bank_to_menu);      // src/lib/menus_glue.s:23
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 140, lib_in_menu_on);      // src/lib/menus_glue.s:24
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 160, lib_in_menu_off);      // src/lib/menus_glue.s:25
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 180, lib_in_menu_mouse_on);      // src/lib/menus_glue.s:26
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 200, lib_in_menu_mouse_off);      // src/lib/menus_glue.s:27
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 220, lib_in_menu_base);      // src/lib/menus_glue.s:28
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 240, lib_fn_choice0);      // src/lib/menus_glue.s:29
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 260, lib_fn_choice1);      // src/lib/menus_glue.s:30
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 280, lib_in_menu_calc);      // src/lib/menus_glue.s:31
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 300, lib_fn_xmenu);      // src/lib/menus_glue.s:33
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 320, lib_fn_ymenu);      // src/lib/menus_glue.s:34
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 340, lib_in_menu_bar);      // src/lib/menus_glue.s:35
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 360, lib_in_menu_line);      // src/lib/menus_glue.s:36
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 380, lib_in_menu_tline);      // src/lib/menus_glue.s:37
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 400, lib_in_menu_movable);      // src/lib/menus_glue.s:38
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 420, lib_in_menu_static);      // src/lib/menus_glue.s:39
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 440, lib_in_menu_item_movable);      // src/lib/menus_glue.s:40
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 460, lib_in_menu_item_static);      // src/lib/menus_glue.s:41
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 480, lib_in_menu_active);      // src/lib/menus_glue.s:42
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 500, lib_in_menu_inactive);      // src/lib/menus_glue.s:43
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 520, lib_in_menu_separate);      // src/lib/menus_glue.s:44
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 540, lib_in_menu_link);      // src/lib/menus_glue.s:45
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 560, lib_in_menu_called);      // src/lib/menus_glue.s:46
    AMOS_LIBRARY_METADATA(host_obj_src_lib_menus_glue_s + 580, lib_in_menu_once);      // src/lib/menus_glue.s:47
}

const GlueEntry entries_src_lib_menus_glue_s[] = {
    {host_obj_src_lib_menus_glue_s + 0, GlueKind::reg, AMOS_BIND_REG(lib_omn_eff), nullptr, 0, 0, "OMnEff"},
    {host_obj_src_lib_menus_glue_s + 4, GlueKind::reg, AMOS_BIND_REG(lib_mn_gere), nullptr, 0, 0, "MnGere"},
    {host_obj_src_lib_menus_glue_s + 8, GlueKind::reg, AMOS_BIND_REG(lib_mn_find), nullptr, 0, 0, "MnFind"},
    {host_obj_src_lib_menus_glue_s + 12, GlueKind::reg, AMOS_BIND_REG(lib_mn_ins), nullptr, 0, 0, "MnIns"},
    {host_obj_src_lib_menus_glue_s + 16, GlueKind::reg, AMOS_BIND_REG(lib_menu_reset), nullptr, 0, 0, "MenuReset"},
    {host_obj_src_lib_menus_glue_s + 20, GlueKind::reg, AMOS_BIND_REG(lib_mn_raz), nullptr, 0, 0, "MnRaz"},
    {host_obj_src_lib_menus_glue_s + 24, GlueKind::reg, AMOS_BIND_REG(lib_mn_clear_var), nullptr, 0, 0, "MnClearVar"},
    {host_obj_src_lib_menus_glue_s + 28, GlueKind::reg, AMOS_BIND_REG(lib_mn_eff), nullptr, 0, 0, "MnEff"},
    {host_obj_src_lib_menus_glue_s + 32, GlueKind::reg, AMOS_BIND_REG(lib_mn_objet), nullptr, 0, 0, "MnObjet"},
    {host_obj_src_lib_menus_glue_s + 36, GlueKind::reg, AMOS_BIND_REG(lib_menu_key_explore), nullptr, 0, 0, "MenuKeyExplore"},
    {host_obj_src_lib_menus_glue_s + 56, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_on_menu_on), nullptr, 0, 3, "InOnMenuOn"},
    {host_obj_src_lib_menus_glue_s + 76, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_on_menu_on), nullptr, 0, 3, "InOnMenuOff"},
    {host_obj_src_lib_menus_glue_s + 96, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_on_menu_del), nullptr, 0, 3, "InOnMenuDel"},
    {host_obj_src_lib_menus_glue_s + 116, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_to_bank), nullptr, 1, 3, "InMenuToBank"},
    {host_obj_src_lib_menus_glue_s + 136, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_bank_to_menu), nullptr, 1, 3, "InBankToMenu"},
    {host_obj_src_lib_menus_glue_s + 156, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_on), nullptr, 0, 3, "InMenuOn"},
    {host_obj_src_lib_menus_glue_s + 176, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_off), nullptr, 0, 3, "InMenuOff"},
    {host_obj_src_lib_menus_glue_s + 196, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_mouse_on), nullptr, 0, 3, "InMenuMouseOn"},
    {host_obj_src_lib_menus_glue_s + 216, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_mouse_off), nullptr, 0, 3, "InMenuMouseOff"},
    {host_obj_src_lib_menus_glue_s + 236, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_base), nullptr, 2, 3, "InMenuBase"},
    {host_obj_src_lib_menus_glue_s + 256, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_choice0), nullptr, 0, 0, "FnChoice0"},
    {host_obj_src_lib_menus_glue_s + 276, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_choice1), nullptr, 1, 0, "FnChoice1"},
    {host_obj_src_lib_menus_glue_s + 296, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_calc), nullptr, 0, 3, "InMenuCalc"},
    {host_obj_src_lib_menus_glue_s + 316, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_xmenu), nullptr, 0, 0, "FnXMenu"},
    {host_obj_src_lib_menus_glue_s + 336, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_ymenu), nullptr, 0, 0, "FnYMenu"},
    {host_obj_src_lib_menus_glue_s + 356, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_bar), nullptr, 0, 3, "InMenuBar"},
    {host_obj_src_lib_menus_glue_s + 376, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_line), nullptr, 0, 3, "InMenuLine"},
    {host_obj_src_lib_menus_glue_s + 396, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_tline), nullptr, 0, 3, "InMenuTline"},
    {host_obj_src_lib_menus_glue_s + 416, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_movable), nullptr, 0, 3, "InMenuMovable"},
    {host_obj_src_lib_menus_glue_s + 436, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_static), nullptr, 0, 3, "InMenuStatic"},
    {host_obj_src_lib_menus_glue_s + 456, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_item_movable), nullptr, 0, 3, "InMenuItemMovable"},
    {host_obj_src_lib_menus_glue_s + 476, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_item_static), nullptr, 0, 3, "InMenuItemStatic"},
    {host_obj_src_lib_menus_glue_s + 496, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_active), nullptr, 0, 3, "InMenuActive"},
    {host_obj_src_lib_menus_glue_s + 516, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_inactive), nullptr, 0, 3, "InMenuInactive"},
    {host_obj_src_lib_menus_glue_s + 536, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_separate), nullptr, 0, 3, "InMenuSeparate"},
    {host_obj_src_lib_menus_glue_s + 556, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_link), nullptr, 0, 3, "InMenuLink"},
    {host_obj_src_lib_menus_glue_s + 576, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_called), nullptr, 0, 3, "InMenuCalled"},
    {host_obj_src_lib_menus_glue_s + 596, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_menu_once), nullptr, 0, 3, "InMenuOnce"},
    {host_obj_src_lib_menus_glue_s + 604, GlueKind::record, AMOS_BIND_CALL(lib_menu_clear), nullptr, 0, 0, "lib_menu_on_clear"},
    {host_obj_src_lib_menus_glue_s + 612, GlueKind::record, AMOS_BIND_CALL(lib_menu_clear), nullptr, 0, 0, "lib_menu_on_default"},
    {host_obj_src_lib_menus_glue_s + 620, GlueKind::record, AMOS_BIND_CALL(lib_menu_error), nullptr, 0, 0, "lib_menu_on_error"},
    {host_obj_src_lib_menus_glue_s + 624, GlueKind::missing, nullptr, nullptr, 0, 0, "mn_call_procedure"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
