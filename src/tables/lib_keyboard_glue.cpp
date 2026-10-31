// src_lib_keyboard_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_keyboard_glue_s[340] = {
    0,0,0,0,0,0,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,2,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,
    0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_keyboard_glue_s()
{
    put(host_obj_src_lib_keyboard_glue_s + 0, 4, (uint32_t(uintptr_t(&lib_fn_inkey))));      // src/lib/keyboard_glue.s:8
    put(host_obj_src_lib_keyboard_glue_s + 20, 4, (uint32_t(uintptr_t(&lib_fn_scancode))));      // src/lib/keyboard_glue.s:9
    put(host_obj_src_lib_keyboard_glue_s + 40, 4, (uint32_t(uintptr_t(&lib_fn_scanshift))));      // src/lib/keyboard_glue.s:10
    put(host_obj_src_lib_keyboard_glue_s + 60, 4, (uint32_t(uintptr_t(&lib_fn_key_state))));      // src/lib/keyboard_glue.s:11
    put(host_obj_src_lib_keyboard_glue_s + 80, 4, (uint32_t(uintptr_t(&lib_fn_key_shift))));      // src/lib/keyboard_glue.s:12
    put(host_obj_src_lib_keyboard_glue_s + 100, 4, (uint32_t(uintptr_t(&lib_fn_joy))));      // src/lib/keyboard_glue.s:13
    put(host_obj_src_lib_keyboard_glue_s + 120, 4, (uint32_t(uintptr_t(&lib_fn_jup))));      // src/lib/keyboard_glue.s:14
    put(host_obj_src_lib_keyboard_glue_s + 140, 4, (uint32_t(uintptr_t(&lib_fn_jdown))));      // src/lib/keyboard_glue.s:15
    put(host_obj_src_lib_keyboard_glue_s + 160, 4, (uint32_t(uintptr_t(&lib_fn_jleft))));      // src/lib/keyboard_glue.s:16
    put(host_obj_src_lib_keyboard_glue_s + 180, 4, (uint32_t(uintptr_t(&lib_fn_jright))));      // src/lib/keyboard_glue.s:17
    put(host_obj_src_lib_keyboard_glue_s + 200, 4, (uint32_t(uintptr_t(&lib_fn_fire))));      // src/lib/keyboard_glue.s:18
    put(host_obj_src_lib_keyboard_glue_s + 220, 4, (uint32_t(uintptr_t(&lib_in_put_key))));      // src/lib/keyboard_glue.s:19
    put(host_obj_src_lib_keyboard_glue_s + 240, 4, (uint32_t(uintptr_t(&lib_in_clear_key))));      // src/lib/keyboard_glue.s:20
    put(host_obj_src_lib_keyboard_glue_s + 260, 4, (uint32_t(uintptr_t(&lib_in_key))));      // src/lib/keyboard_glue.s:21
    put(host_obj_src_lib_keyboard_glue_s + 280, 4, (uint32_t(uintptr_t(&lib_fn_key))));      // src/lib/keyboard_glue.s:22
    put(host_obj_src_lib_keyboard_glue_s + 300, 4, (uint32_t(uintptr_t(&lib_fn_scan1))));      // src/lib/keyboard_glue.s:23
    put(host_obj_src_lib_keyboard_glue_s + 320, 4, (uint32_t(uintptr_t(&lib_fn_scan2))));      // src/lib/keyboard_glue.s:24
}

const GlueEntry entries_src_lib_keyboard_glue_s[] = {
    {host_obj_src_lib_keyboard_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_inkey), nullptr, 0, 2, "FnInkey"},
    {host_obj_src_lib_keyboard_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_scancode), nullptr, 0, 0, "FnScancode"},
    {host_obj_src_lib_keyboard_glue_s + 56, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_scanshift), nullptr, 0, 0, "FnScanshift"},
    {host_obj_src_lib_keyboard_glue_s + 76, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_key_state), nullptr, 1, 0, "FnKeyState"},
    {host_obj_src_lib_keyboard_glue_s + 96, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_key_shift), nullptr, 0, 0, "FnKeyShift"},
    {host_obj_src_lib_keyboard_glue_s + 116, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_joy), nullptr, 1, 0, "FnJoy"},
    {host_obj_src_lib_keyboard_glue_s + 136, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_jup), nullptr, 1, 0, "FnJup"},
    {host_obj_src_lib_keyboard_glue_s + 156, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_jdown), nullptr, 1, 0, "FnJdown"},
    {host_obj_src_lib_keyboard_glue_s + 176, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_jleft), nullptr, 1, 0, "FnJleft"},
    {host_obj_src_lib_keyboard_glue_s + 196, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_jright), nullptr, 1, 0, "FnJright"},
    {host_obj_src_lib_keyboard_glue_s + 216, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_fire), nullptr, 1, 0, "FnFire"},
    {host_obj_src_lib_keyboard_glue_s + 236, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_put_key), nullptr, 1, 3, "InPutKey"},
    {host_obj_src_lib_keyboard_glue_s + 256, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_clear_key), nullptr, 0, 3, "InClearKey"},
    {host_obj_src_lib_keyboard_glue_s + 276, GlueKind::libcall, reinterpret_cast<const void *>(&lib_in_key), nullptr, 2, 3, "InKeyD"},
    {host_obj_src_lib_keyboard_glue_s + 296, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_key), nullptr, 1, 2, "FnKeyD"},
    {host_obj_src_lib_keyboard_glue_s + 316, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_scan1), nullptr, 1, 2, "FnScan1"},
    {host_obj_src_lib_keyboard_glue_s + 336, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_scan2), nullptr, 2, 2, "FnScan2"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
