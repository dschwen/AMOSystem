// src_lib_maths_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_maths_glue_s[160] = {
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,
    71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,
    71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,
    71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,
    71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_maths_glue_s()
{
    put(host_obj_src_lib_maths_glue_s + 0, 4, (uint32_t(uintptr_t(&lib_fn_abs))));      // src/lib/maths_glue.s:9
    put(host_obj_src_lib_maths_glue_s + 20, 4, (uint32_t(uintptr_t(&lib_fn_sgn))));      // src/lib/maths_glue.s:10
}

const GlueEntry entries_src_lib_maths_glue_s[] = {
    {host_obj_src_lib_maths_glue_s + 16, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_abs), nullptr, 1, 0, "FnAbs"},
    {host_obj_src_lib_maths_glue_s + 36, GlueKind::libcall, reinterpret_cast<const void *>(&lib_fn_sgn), nullptr, 1, 0, "FnSgn"},
    {host_obj_src_lib_maths_glue_s + 40, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_sgn_f), nullptr, 0, 0, "FnSgnF"},
    {host_obj_src_lib_maths_glue_s + 44, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_str_f), nullptr, 0, 0, "FnStrF"},
    {host_obj_src_lib_maths_glue_s + 48, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_abs_f), nullptr, 0, 0, "FnAbsF"},
    {host_obj_src_lib_maths_glue_s + 52, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_int_f), nullptr, 0, 0, "FnIntF"},
    {host_obj_src_lib_maths_glue_s + 60, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_tan), nullptr, 0, 0, "FnTan"},
    {host_obj_src_lib_maths_glue_s + 68, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_sqr), nullptr, 0, 0, "FnSqr"},
    {host_obj_src_lib_maths_glue_s + 76, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_log), nullptr, 0, 0, "FnLog"},
    {host_obj_src_lib_maths_glue_s + 84, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_ln), nullptr, 0, 0, "FnLn"},
    {host_obj_src_lib_maths_glue_s + 92, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_exp), nullptr, 0, 0, "FnExp"},
    {host_obj_src_lib_maths_glue_s + 100, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_sin), nullptr, 0, 0, "FnSin"},
    {host_obj_src_lib_maths_glue_s + 108, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_cos), nullptr, 0, 0, "FnCos"},
    {host_obj_src_lib_maths_glue_s + 116, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_asin), nullptr, 0, 0, "FnASin"},
    {host_obj_src_lib_maths_glue_s + 124, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_acos), nullptr, 0, 0, "FnACos"},
    {host_obj_src_lib_maths_glue_s + 132, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_atan), nullptr, 0, 0, "FnATan"},
    {host_obj_src_lib_maths_glue_s + 140, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_hsin), nullptr, 0, 0, "FnHSin"},
    {host_obj_src_lib_maths_glue_s + 148, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_hcos), nullptr, 0, 0, "FnHCos"},
    {host_obj_src_lib_maths_glue_s + 156, GlueKind::float_, reinterpret_cast<const void *>(&lib_fn_htan), nullptr, 0, 0, "FnHTan"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
