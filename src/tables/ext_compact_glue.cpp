// src_ext_compact_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_ext_compact_glue_s[152] = {
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,6,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,6,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0};
}

namespace amos::host {

void relocate_src_ext_compact_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 0, ext_pack2);      // src/ext/compact_glue.s:8
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 20, ext_pack6);      // src/ext/compact_glue.s:9
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 40, ext_spack2);      // src/ext/compact_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 60, ext_spack6);      // src/ext/compact_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 80, ext_unpack1);      // src/ext/compact_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 100, ext_unpack2);      // src/ext/compact_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_ext_compact_glue_s + 120, ext_unpack3);      // src/ext/compact_glue.s:14
}

const GlueEntry entries_src_ext_compact_glue_s[] = {
    {host_obj_src_ext_compact_glue_s + 16, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_pack2), nullptr, 2, 3, "Ext_Compact_InPack2"},
    {host_obj_src_ext_compact_glue_s + 36, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_pack6), nullptr, 6, 3, "Ext_Compact_InPack6"},
    {host_obj_src_ext_compact_glue_s + 56, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_spack2), nullptr, 2, 3, "Ext_Compact_InSPack2"},
    {host_obj_src_ext_compact_glue_s + 76, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_spack6), nullptr, 6, 3, "Ext_Compact_InSPack6"},
    {host_obj_src_ext_compact_glue_s + 96, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_unpack1), nullptr, 1, 3, "Ext_Compact_InUnpack1"},
    {host_obj_src_ext_compact_glue_s + 116, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_unpack2), nullptr, 2, 3, "Ext_Compact_InUnpack2"},
    {host_obj_src_ext_compact_glue_s + 136, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_unpack3), nullptr, 3, 3, "Ext_Compact_InUnpack3"},
    {host_obj_src_ext_compact_glue_s + 140, GlueKind::reg, AMOS_BIND_REG(ext_compact_cold), nullptr, 0, 0, "Ext_Compact_Compact_Cold"},
    {host_obj_src_ext_compact_glue_s + 144, GlueKind::reg, AMOS_BIND_REG(ext_compact_custom), nullptr, 0, 0, "Ext_Compact_Custom"},
    {host_obj_src_ext_compact_glue_s + 148, GlueKind::reg, AMOS_BIND_REG(ext_compact_custom2), nullptr, 0, 0, "Ext_Compact_Custom2"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
