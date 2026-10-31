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
    put(host_obj_src_lib_amal_glue_s + 0, 4, (uint32_t(uintptr_t(&lib_fn_amalerr))));      // src/lib/amal_glue.s:8
    put(host_obj_src_lib_amal_glue_s + 20, 4, (uint32_t(uintptr_t(&lib_in_freeze))));      // src/lib/amal_glue.s:9
    put(host_obj_src_lib_amal_glue_s + 40, 4, (uint32_t(uintptr_t(&lib_in_unfreeze))));      // src/lib/amal_glue.s:10
    put(host_obj_src_lib_amal_glue_s + 60, 4, (uint32_t(uintptr_t(&lib_in_synchro_on))));      // src/lib/amal_glue.s:11
    put(host_obj_src_lib_amal_glue_s + 80, 4, (uint32_t(uintptr_t(&lib_in_synchro_off))));      // src/lib/amal_glue.s:12
    put(host_obj_src_lib_amal_glue_s + 100, 4, (uint32_t(uintptr_t(&lib_in_synchro))));      // src/lib/amal_glue.s:13
    put(host_obj_src_lib_amal_glue_s + 120, 4, (uint32_t(uintptr_t(&lib_in_amal_on0))));      // src/lib/amal_glue.s:14
    put(host_obj_src_lib_amal_glue_s + 140, 4, (uint32_t(uintptr_t(&lib_in_amal_off0))));      // src/lib/amal_glue.s:15
    put(host_obj_src_lib_amal_glue_s + 160, 4, (uint32_t(uintptr_t(&lib_in_amal_freeze0))));      // src/lib/amal_glue.s:16
    put(host_obj_src_lib_amal_glue_s + 180, 4, (uint32_t(uintptr_t(&lib_in_move_on0))));      // src/lib/amal_glue.s:17
    put(host_obj_src_lib_amal_glue_s + 200, 4, (uint32_t(uintptr_t(&lib_in_anim_on0))));      // src/lib/amal_glue.s:18
    put(host_obj_src_lib_amal_glue_s + 220, 4, (uint32_t(uintptr_t(&lib_in_move_off0))));      // src/lib/amal_glue.s:19
    put(host_obj_src_lib_amal_glue_s + 240, 4, (uint32_t(uintptr_t(&lib_in_anim_off0))));      // src/lib/amal_glue.s:20
    put(host_obj_src_lib_amal_glue_s + 260, 4, (uint32_t(uintptr_t(&lib_in_move_freeze0))));      // src/lib/amal_glue.s:21
    put(host_obj_src_lib_amal_glue_s + 280, 4, (uint32_t(uintptr_t(&lib_in_anim_freeze0))));      // src/lib/amal_glue.s:22
    put(host_obj_src_lib_amal_glue_s + 300, 4, (uint32_t(uintptr_t(&lib_in_amal_on1))));      // src/lib/amal_glue.s:23
    put(host_obj_src_lib_amal_glue_s + 320, 4, (uint32_t(uintptr_t(&lib_in_amal_off1))));      // src/lib/amal_glue.s:24
    put(host_obj_src_lib_amal_glue_s + 340, 4, (uint32_t(uintptr_t(&lib_in_amal_freeze1))));      // src/lib/amal_glue.s:25
    put(host_obj_src_lib_amal_glue_s + 360, 4, (uint32_t(uintptr_t(&lib_in_move_on1))));      // src/lib/amal_glue.s:26
    put(host_obj_src_lib_amal_glue_s + 380, 4, (uint32_t(uintptr_t(&lib_in_anim_on1))));      // src/lib/amal_glue.s:27
    put(host_obj_src_lib_amal_glue_s + 400, 4, (uint32_t(uintptr_t(&lib_in_move_off1))));      // src/lib/amal_glue.s:28
    put(host_obj_src_lib_amal_glue_s + 420, 4, (uint32_t(uintptr_t(&lib_in_anim_off1))));      // src/lib/amal_glue.s:29
    put(host_obj_src_lib_amal_glue_s + 440, 4, (uint32_t(uintptr_t(&lib_in_move_freeze1))));      // src/lib/amal_glue.s:30
    put(host_obj_src_lib_amal_glue_s + 460, 4, (uint32_t(uintptr_t(&lib_in_anim_freeze1))));      // src/lib/amal_glue.s:31
    put(host_obj_src_lib_amal_glue_s + 480, 4, (uint32_t(uintptr_t(&lib_in_move_x3))));      // src/lib/amal_glue.s:32
    put(host_obj_src_lib_amal_glue_s + 500, 4, (uint32_t(uintptr_t(&lib_in_move_y3))));      // src/lib/amal_glue.s:33
    put(host_obj_src_lib_amal_glue_s + 520, 4, (uint32_t(uintptr_t(&lib_in_anim3))));      // src/lib/amal_glue.s:34
    put(host_obj_src_lib_amal_glue_s + 540, 4, (uint32_t(uintptr_t(&lib_in_amal3))));      // src/lib/amal_glue.s:35
    put(host_obj_src_lib_amal_glue_s + 560, 4, (uint32_t(uintptr_t(&lib_in_move_x2))));      // src/lib/amal_glue.s:36
    put(host_obj_src_lib_amal_glue_s + 580, 4, (uint32_t(uintptr_t(&lib_in_move_y2))));      // src/lib/amal_glue.s:37
    put(host_obj_src_lib_amal_glue_s + 600, 4, (uint32_t(uintptr_t(&lib_in_amal2))));      // src/lib/amal_glue.s:38
    put(host_obj_src_lib_amal_glue_s + 620, 4, (uint32_t(uintptr_t(&lib_in_anim2))));      // src/lib/amal_glue.s:39
    put(host_obj_src_lib_amal_glue_s + 640, 4, (uint32_t(uintptr_t(&lib_fn_movon))));      // src/lib/amal_glue.s:40
    put(host_obj_src_lib_amal_glue_s + 660, 4, (uint32_t(uintptr_t(&lib_fn_chanan))));      // src/lib/amal_glue.s:41
    put(host_obj_src_lib_amal_glue_s + 680, 4, (uint32_t(uintptr_t(&lib_fn_chanmv))));      // src/lib/amal_glue.s:42
    put(host_obj_src_lib_amal_glue_s + 700, 4, (uint32_t(uintptr_t(&lib_in_amreg1))));      // src/lib/amal_glue.s:43
    put(host_obj_src_lib_amal_glue_s + 720, 4, (uint32_t(uintptr_t(&lib_fn_amreg1))));      // src/lib/amal_glue.s:44
    put(host_obj_src_lib_amal_glue_s + 740, 4, (uint32_t(uintptr_t(&lib_in_amreg2))));      // src/lib/amal_glue.s:45
    put(host_obj_src_lib_amal_glue_s + 760, 4, (uint32_t(uintptr_t(&lib_fn_amreg2))));      // src/lib/amal_glue.s:46
    put(host_obj_src_lib_amal_glue_s + 780, 4, (uint32_t(uintptr_t(&lib_in_amplay2))));      // src/lib/amal_glue.s:47
    put(host_obj_src_lib_amal_glue_s + 800, 4, (uint32_t(uintptr_t(&lib_in_amplay4))));      // src/lib/amal_glue.s:48
}

const GlueEntry entries_src_lib_amal_glue_s[] = {
    {host_obj_src_lib_amal_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_amalerr), nullptr, 0, 0, "FnAmalerr"},
    {host_obj_src_lib_amal_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_freeze), nullptr, 0, 3, "InFreeze"},
    {host_obj_src_lib_amal_glue_s + 56, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_unfreeze), nullptr, 0, 3, "InUnFreeze"},
    {host_obj_src_lib_amal_glue_s + 76, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_synchro_on), nullptr, 0, 3, "InSynchroOn"},
    {host_obj_src_lib_amal_glue_s + 96, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_synchro_off), nullptr, 0, 3, "InSynchroOff"},
    {host_obj_src_lib_amal_glue_s + 116, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_synchro), nullptr, 0, 3, "InSynchro"},
    {host_obj_src_lib_amal_glue_s + 136, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal_on0), nullptr, 0, 3, "InAmalOn0"},
    {host_obj_src_lib_amal_glue_s + 156, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal_off0), nullptr, 0, 3, "InAmalOff0"},
    {host_obj_src_lib_amal_glue_s + 176, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal_freeze0), nullptr, 0, 3, "InAmalFreeze0"},
    {host_obj_src_lib_amal_glue_s + 196, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_on0), nullptr, 0, 3, "InMoveOn0"},
    {host_obj_src_lib_amal_glue_s + 216, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim_on0), nullptr, 0, 3, "InAnimOn0"},
    {host_obj_src_lib_amal_glue_s + 236, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_off0), nullptr, 0, 3, "InMoveOff0"},
    {host_obj_src_lib_amal_glue_s + 256, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim_off0), nullptr, 0, 3, "InAnimOff0"},
    {host_obj_src_lib_amal_glue_s + 276, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_freeze0), nullptr, 0, 3, "InMoveFreeze0"},
    {host_obj_src_lib_amal_glue_s + 296, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim_freeze0), nullptr, 0, 3, "InAnimFreeze0"},
    {host_obj_src_lib_amal_glue_s + 316, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal_on1), nullptr, 1, 3, "InAmalOn1"},
    {host_obj_src_lib_amal_glue_s + 336, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal_off1), nullptr, 1, 3, "InAmalOff1"},
    {host_obj_src_lib_amal_glue_s + 356, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal_freeze1), nullptr, 1, 3, "InAmalFreeze1"},
    {host_obj_src_lib_amal_glue_s + 376, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_on1), nullptr, 1, 3, "InMoveOn1"},
    {host_obj_src_lib_amal_glue_s + 396, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim_on1), nullptr, 1, 3, "InAnimOn1"},
    {host_obj_src_lib_amal_glue_s + 416, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_off1), nullptr, 1, 3, "InMoveOff1"},
    {host_obj_src_lib_amal_glue_s + 436, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim_off1), nullptr, 1, 3, "InAnimOff1"},
    {host_obj_src_lib_amal_glue_s + 456, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_freeze1), nullptr, 1, 3, "InMoveFreeze1"},
    {host_obj_src_lib_amal_glue_s + 476, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim_freeze1), nullptr, 1, 3, "InAnimFreeze1"},
    {host_obj_src_lib_amal_glue_s + 496, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_x3), nullptr, 3, 3, "InMoveX3"},
    {host_obj_src_lib_amal_glue_s + 516, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_y3), nullptr, 3, 3, "InMoveY3"},
    {host_obj_src_lib_amal_glue_s + 536, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim3), nullptr, 3, 3, "InAnim3"},
    {host_obj_src_lib_amal_glue_s + 556, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal3), nullptr, 3, 3, "InAmal3"},
    {host_obj_src_lib_amal_glue_s + 576, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_x2), nullptr, 2, 3, "InMoveX2"},
    {host_obj_src_lib_amal_glue_s + 596, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_move_y2), nullptr, 2, 3, "InMoveY2"},
    {host_obj_src_lib_amal_glue_s + 616, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amal2), nullptr, 2, 3, "InAmal2"},
    {host_obj_src_lib_amal_glue_s + 636, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_anim2), nullptr, 2, 3, "InAnim2"},
    {host_obj_src_lib_amal_glue_s + 656, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_movon), nullptr, 1, 0, "FnMovon"},
    {host_obj_src_lib_amal_glue_s + 676, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_chanan), nullptr, 1, 0, "FnChanAn"},
    {host_obj_src_lib_amal_glue_s + 696, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_chanmv), nullptr, 1, 0, "FnChanMv"},
    {host_obj_src_lib_amal_glue_s + 716, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amreg1), nullptr, 2, 3, "InAmreg1"},
    {host_obj_src_lib_amal_glue_s + 736, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_amreg1), nullptr, 1, 0, "FnAmreg1"},
    {host_obj_src_lib_amal_glue_s + 756, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amreg2), nullptr, 3, 3, "InAmreg2"},
    {host_obj_src_lib_amal_glue_s + 776, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_amreg2), nullptr, 2, 0, "FnAmreg2"},
    {host_obj_src_lib_amal_glue_s + 796, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amplay2), nullptr, 2, 3, "InAmPlay2"},
    {host_obj_src_lib_amal_glue_s + 816, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_amplay4), nullptr, 4, 3, "InAmPlay4"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
