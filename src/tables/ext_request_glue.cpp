// src_ext_request_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_ext_request_glue_s[68] = {
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0};
}

namespace amos::host {

void relocate_src_ext_request_glue_s()
{
    put(host_obj_src_ext_request_glue_s + 0, 4, (uint32_t(uintptr_t(&ext_request_wb))));      // src/ext/request_glue.s:8
    put(host_obj_src_ext_request_glue_s + 20, 4, (uint32_t(uintptr_t(&ext_request_on))));      // src/ext/request_glue.s:9
    put(host_obj_src_ext_request_glue_s + 40, 4, (uint32_t(uintptr_t(&ext_request_off))));      // src/ext/request_glue.s:10
}

const GlueEntry entries_src_ext_request_glue_s[] = {
    {host_obj_src_ext_request_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&ext_request_wb), nullptr, 0, 3, "Ext_Request_InRequestWb"},
    {host_obj_src_ext_request_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&ext_request_on), nullptr, 0, 3, "Ext_Request_InRequestOn"},
    {host_obj_src_ext_request_glue_s + 56, GlueKind::libcall, reinterpret_cast<const void *>(&ext_request_off), nullptr, 0, 3, "Ext_Request_InRequestOff"},
    {host_obj_src_ext_request_glue_s + 60, GlueKind::reg, reinterpret_cast<const void *>(&ext_request_cold), nullptr, 0, 0, "Ext_Request_Cold"},
    {host_obj_src_ext_request_glue_s + 64, GlueKind::call, reinterpret_cast<const void *>(&ext_request_warm), nullptr, 0, 0, "ext_request_warm_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
