// src_lib_amal_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_amal_glue_s[820] = {
    0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,3,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,3,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,4,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_amal_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 0, lib_fn_amalerr);      // src/lib/amal_glue.s:8
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 20, lib_in_freeze);      // src/lib/amal_glue.s:9
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 40, lib_in_unfreeze);      // src/lib/amal_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 60, lib_in_synchro_on);      // src/lib/amal_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 80, lib_in_synchro_off);      // src/lib/amal_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 100, lib_in_synchro);      // src/lib/amal_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 120, lib_in_amal_on0);      // src/lib/amal_glue.s:14
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 140, lib_in_amal_off0);      // src/lib/amal_glue.s:15
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 160, lib_in_amal_freeze0);      // src/lib/amal_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 180, lib_in_move_on0);      // src/lib/amal_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 200, lib_in_anim_on0);      // src/lib/amal_glue.s:18
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 220, lib_in_move_off0);      // src/lib/amal_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 240, lib_in_anim_off0);      // src/lib/amal_glue.s:20
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 260, lib_in_move_freeze0);      // src/lib/amal_glue.s:21
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 280, lib_in_anim_freeze0);      // src/lib/amal_glue.s:22
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 300, lib_in_amal_on1);      // src/lib/amal_glue.s:23
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 320, lib_in_amal_off1);      // src/lib/amal_glue.s:24
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 340, lib_in_amal_freeze1);      // src/lib/amal_glue.s:25
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 360, lib_in_move_on1);      // src/lib/amal_glue.s:26
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 380, lib_in_anim_on1);      // src/lib/amal_glue.s:27
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 400, lib_in_move_off1);      // src/lib/amal_glue.s:28
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 420, lib_in_anim_off1);      // src/lib/amal_glue.s:29
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 440, lib_in_move_freeze1);      // src/lib/amal_glue.s:30
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 460, lib_in_anim_freeze1);      // src/lib/amal_glue.s:31
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 480, lib_in_move_x3);      // src/lib/amal_glue.s:32
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 500, lib_in_move_y3);      // src/lib/amal_glue.s:33
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 520, lib_in_anim3);      // src/lib/amal_glue.s:34
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 540, lib_in_amal3);      // src/lib/amal_glue.s:35
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 560, lib_in_move_x2);      // src/lib/amal_glue.s:36
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 580, lib_in_move_y2);      // src/lib/amal_glue.s:37
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 600, lib_in_amal2);      // src/lib/amal_glue.s:38
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 620, lib_in_anim2);      // src/lib/amal_glue.s:39
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 640, lib_fn_movon);      // src/lib/amal_glue.s:40
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 660, lib_fn_chanan);      // src/lib/amal_glue.s:41
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 680, lib_fn_chanmv);      // src/lib/amal_glue.s:42
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 700, lib_in_amreg1);      // src/lib/amal_glue.s:43
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 720, lib_fn_amreg1);      // src/lib/amal_glue.s:44
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 740, lib_in_amreg2);      // src/lib/amal_glue.s:45
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 760, lib_fn_amreg2);      // src/lib/amal_glue.s:46
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 780, lib_in_amplay2);      // src/lib/amal_glue.s:47
    AMOS_LIBRARY_METADATA(host_obj_src_lib_amal_glue_s + 800, lib_in_amplay4);      // src/lib/amal_glue.s:48
}

const GlueEntry entries_src_lib_amal_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 16), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_amalerr), nullptr, 0, 0, "FnAmalerr"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 36), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_freeze), nullptr, 0, 3, "InFreeze"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 56), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_unfreeze), nullptr, 0, 3, "InUnFreeze"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 76), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_synchro_on), nullptr, 0, 3, "InSynchroOn"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 96), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_synchro_off), nullptr, 0, 3, "InSynchroOff"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 116), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_synchro), nullptr, 0, 3, "InSynchro"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 136), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal_on0), nullptr, 0, 3, "InAmalOn0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 156), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal_off0), nullptr, 0, 3, "InAmalOff0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 176), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal_freeze0), nullptr, 0, 3, "InAmalFreeze0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 196), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_on0), nullptr, 0, 3, "InMoveOn0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 216), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim_on0), nullptr, 0, 3, "InAnimOn0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 236), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_off0), nullptr, 0, 3, "InMoveOff0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 256), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim_off0), nullptr, 0, 3, "InAnimOff0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 276), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_freeze0), nullptr, 0, 3, "InMoveFreeze0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 296), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim_freeze0), nullptr, 0, 3, "InAnimFreeze0"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 316), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal_on1), nullptr, 1, 3, "InAmalOn1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 336), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal_off1), nullptr, 1, 3, "InAmalOff1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 356), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal_freeze1), nullptr, 1, 3, "InAmalFreeze1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 376), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_on1), nullptr, 1, 3, "InMoveOn1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 396), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim_on1), nullptr, 1, 3, "InAnimOn1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 416), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_off1), nullptr, 1, 3, "InMoveOff1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 436), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim_off1), nullptr, 1, 3, "InAnimOff1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 456), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_freeze1), nullptr, 1, 3, "InMoveFreeze1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 476), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim_freeze1), nullptr, 1, 3, "InAnimFreeze1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 496), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_x3), nullptr, 3, 3, "InMoveX3"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 516), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_y3), nullptr, 3, 3, "InMoveY3"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 536), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim3), nullptr, 3, 3, "InAnim3"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 556), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal3), nullptr, 3, 3, "InAmal3"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 576), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_x2), nullptr, 2, 3, "InMoveX2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 596), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_move_y2), nullptr, 2, 3, "InMoveY2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 616), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amal2), nullptr, 2, 3, "InAmal2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 636), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_anim2), nullptr, 2, 3, "InAnim2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 656), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_movon), nullptr, 1, 0, "FnMovon"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 676), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_chanan), nullptr, 1, 0, "FnChanAn"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 696), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_chanmv), nullptr, 1, 0, "FnChanMv"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 716), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amreg1), nullptr, 2, 3, "InAmreg1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 736), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_amreg1), nullptr, 1, 0, "FnAmreg1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 756), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amreg2), nullptr, 3, 3, "InAmreg2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 776), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_amreg2), nullptr, 2, 0, "FnAmreg2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 796), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amplay2), nullptr, 2, 3, "InAmPlay2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_amal_glue_s, 816), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_amplay4), nullptr, 4, 3, "InAmPlay4"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
