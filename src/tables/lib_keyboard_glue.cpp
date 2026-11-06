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
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 0, lib_fn_inkey);      // src/lib/keyboard_glue.s:8
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 20, lib_fn_scancode);      // src/lib/keyboard_glue.s:9
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 40, lib_fn_scanshift);      // src/lib/keyboard_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 60, lib_fn_key_state);      // src/lib/keyboard_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 80, lib_fn_key_shift);      // src/lib/keyboard_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 100, lib_fn_joy);      // src/lib/keyboard_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 120, lib_fn_jup);      // src/lib/keyboard_glue.s:14
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 140, lib_fn_jdown);      // src/lib/keyboard_glue.s:15
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 160, lib_fn_jleft);      // src/lib/keyboard_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 180, lib_fn_jright);      // src/lib/keyboard_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 200, lib_fn_fire);      // src/lib/keyboard_glue.s:18
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 220, lib_in_put_key);      // src/lib/keyboard_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 240, lib_in_clear_key);      // src/lib/keyboard_glue.s:20
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 260, lib_in_key);      // src/lib/keyboard_glue.s:21
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 280, lib_fn_key);      // src/lib/keyboard_glue.s:22
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 300, lib_fn_scan1);      // src/lib/keyboard_glue.s:23
    AMOS_LIBRARY_METADATA(host_obj_src_lib_keyboard_glue_s + 320, lib_fn_scan2);      // src/lib/keyboard_glue.s:24
}

const GlueEntry entries_src_lib_keyboard_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 16), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_inkey), nullptr, 0, 2, "FnInkey"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 36), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_scancode), nullptr, 0, 0, "FnScancode"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 56), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_scanshift), nullptr, 0, 0, "FnScanshift"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 76), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_key_state), nullptr, 1, 0, "FnKeyState"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 96), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_key_shift), nullptr, 0, 0, "FnKeyShift"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 116), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_joy), nullptr, 1, 0, "FnJoy"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 136), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_jup), nullptr, 1, 0, "FnJup"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 156), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_jdown), nullptr, 1, 0, "FnJdown"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 176), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_jleft), nullptr, 1, 0, "FnJleft"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 196), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_jright), nullptr, 1, 0, "FnJright"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 216), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_fire), nullptr, 1, 0, "FnFire"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 236), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_put_key), nullptr, 1, 3, "InPutKey"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 256), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_clear_key), nullptr, 0, 3, "InClearKey"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 276), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_key), nullptr, 2, 3, "InKeyD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 296), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_key), nullptr, 1, 2, "FnKeyD"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 316), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_scan1), nullptr, 1, 2, "FnScan1"},
    {AMOS_ENTRY_AT(host_obj_src_lib_keyboard_glue_s, 336), GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_scan2), nullptr, 2, 2, "FnScan2"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
