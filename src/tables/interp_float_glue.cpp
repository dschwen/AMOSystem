// src_interp_float_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_interp_float_glue_s[168] = {
};
}

namespace amos::host {

void relocate_src_interp_float_glue_s()
{
}

const GlueEntry entries_src_interp_float_glue_s[] = {
    {host_obj_src_interp_float_glue_s + 0, GlueKind::missing, nullptr, nullptr, 0, 0, "interp_lvo"},
    {host_obj_src_interp_float_glue_s + 4, GlueKind::reg, reinterpret_cast<const void *>(&interp_int_to_fl1), nullptr, 0, 0, "IntToFl1"},
    {host_obj_src_interp_float_glue_s + 8, GlueKind::reg, reinterpret_cast<const void *>(&interp_dint_to_fl1), nullptr, 0, 0, "DIntToFl1"},
    {host_obj_src_interp_float_glue_s + 12, GlueKind::reg, reinterpret_cast<const void *>(&interp_int_to_fl2), nullptr, 0, 0, "IntToFl2"},
    {host_obj_src_interp_float_glue_s + 16, GlueKind::reg, reinterpret_cast<const void *>(&interp_dint_to_fl2), nullptr, 0, 0, "DIntToFl2"},
    {host_obj_src_interp_float_glue_s + 20, GlueKind::reg, reinterpret_cast<const void *>(&interp_fl_to_int1), nullptr, 0, 0, "FlToInt1"},
    {host_obj_src_interp_float_glue_s + 24, GlueKind::reg, reinterpret_cast<const void *>(&interp_dfl_to_int1), nullptr, 0, 0, "DFlToInt1"},
    {host_obj_src_interp_float_glue_s + 28, GlueKind::reg, reinterpret_cast<const void *>(&interp_fl_to_int2), nullptr, 0, 0, "FlToInt2"},
    {host_obj_src_interp_float_glue_s + 32, GlueKind::reg, reinterpret_cast<const void *>(&interp_dfl_to_int2), nullptr, 0, 0, "DFlToInt2"},
    {host_obj_src_interp_float_glue_s + 36, GlueKind::reg, reinterpret_cast<const void *>(&interp_math_fonction), nullptr, 0, 0, "Math_Fonction"},
    {host_obj_src_interp_float_glue_s + 40, GlueKind::reg, reinterpret_cast<const void *>(&interp_dmath_fonction), nullptr, 0, 0, "DMath_Fonction"},
    {host_obj_src_interp_float_glue_s + 44, GlueKind::reg, reinterpret_cast<const void *>(&interp_float_compare), nullptr, 0, 0, "Float_Compare"},
    {host_obj_src_interp_float_glue_s + 48, GlueKind::reg, reinterpret_cast<const void *>(&interp_dfloat_compare), nullptr, 0, 0, "DFloat_Compare"},
    {host_obj_src_interp_float_glue_s + 52, GlueKind::reg, reinterpret_cast<const void *>(&interp_float_operation), nullptr, 0, 0, "Float_Operation"},
    {host_obj_src_interp_float_glue_s + 56, GlueKind::reg, reinterpret_cast<const void *>(&interp_dfloat_operation), nullptr, 0, 0, "DFloat_Operation"},
    {host_obj_src_interp_float_glue_s + 60, GlueKind::reg, reinterpret_cast<const void *>(&interp_float_test), nullptr, 0, 0, "Float_Test"},
    {host_obj_src_interp_float_glue_s + 64, GlueKind::reg, reinterpret_cast<const void *>(&interp_float_testf), nullptr, 0, 0, "Float_TestF"},
    {host_obj_src_interp_float_glue_s + 68, GlueKind::reg, reinterpret_cast<const void *>(&interp_math_operation), nullptr, 0, 0, "Math_Operation"},
    {host_obj_src_interp_float_glue_s + 72, GlueKind::reg, reinterpret_cast<const void *>(&interp_dmath_operation), nullptr, 0, 0, "DMath_Operation"},
    {host_obj_src_interp_float_glue_s + 76, GlueKind::reg, reinterpret_cast<const void *>(&interp_float_fonction), nullptr, 0, 0, "Float_Fonction"},
    {host_obj_src_interp_float_glue_s + 80, GlueKind::reg, reinterpret_cast<const void *>(&interp_dfloat_fonction), nullptr, 0, 0, "DFloat_Fonction"},
    {host_obj_src_interp_float_glue_s + 84, GlueKind::reg, reinterpret_cast<const void *>(&interp_fl_pos), nullptr, 0, 0, "FlPos"},
    {host_obj_src_interp_float_glue_s + 88, GlueKind::reg, reinterpret_cast<const void *>(&interp_fl_pos_d), nullptr, 0, 0, "FlPosD"},
    {host_obj_src_interp_float_glue_s + 92, GlueKind::reg, reinterpret_cast<const void *>(&interp_aangle), nullptr, 0, 0, "AAngle"},
    {host_obj_src_interp_float_glue_s + 96, GlueKind::reg, reinterpret_cast<const void *>(&interp_aangle_d), nullptr, 0, 0, "AAngleD"},
    {host_obj_src_interp_float_glue_s + 100, GlueKind::reg, reinterpret_cast<const void *>(&interp_ffangle), nullptr, 0, 0, "FFAngle"},
    {host_obj_src_interp_float_glue_s + 104, GlueKind::reg, reinterpret_cast<const void *>(&interp_fangle_d), nullptr, 0, 0, "FAngleD"},
    {host_obj_src_interp_float_glue_s + 108, GlueKind::reg, reinterpret_cast<const void *>(&interp_fn_param_f), nullptr, 0, 0, "FnParamF"},
    {host_obj_src_interp_float_glue_s + 112, GlueKind::reg, reinterpret_cast<const void *>(&interp_fn_param_d), nullptr, 0, 0, "FnParamD"},
    {host_obj_src_interp_float_glue_s + 116, GlueKind::reg, reinterpret_cast<const void *>(&interp_demande), nullptr, 0, 0, "Demande"},
    {host_obj_src_interp_float_glue_s + 120, GlueKind::reg, reinterpret_cast<const void *>(&interp_ddemande), nullptr, 0, 0, "DDemande"},
    {host_obj_src_interp_float_glue_s + 124, GlueKind::reg, reinterpret_cast<const void *>(&interp_menage), nullptr, 0, 0, "Menage"},
    {host_obj_src_interp_float_glue_s + 128, GlueKind::reg, reinterpret_cast<const void *>(&interp_val_rout), nullptr, 0, 0, "ValRout"},
    {host_obj_src_interp_float_glue_s + 132, GlueKind::reg, reinterpret_cast<const void *>(&interp_defrun1), nullptr, 0, 0, "DefRun1"},
    {host_obj_src_interp_float_glue_s + 136, GlueKind::reg, reinterpret_cast<const void *>(&interp_defrun2), nullptr, 0, 0, "DefRun2"},
    {host_obj_src_interp_float_glue_s + 140, GlueKind::reg, reinterpret_cast<const void *>(&interp_defrun_acc), nullptr, 0, 0, "DefRunAcc"},
    {host_obj_src_interp_float_glue_s + 144, GlueKind::reg, reinterpret_cast<const void *>(&interp_defrun_extensions), nullptr, 0, 0, "DefRunExtensions"},
    {host_obj_src_interp_float_glue_s + 148, GlueKind::call, reinterpret_cast<const void *>(&interp_open_math_libraries), nullptr, 0, 0, "Open_MathLibraries"},
    {host_obj_src_interp_float_glue_s + 152, GlueKind::reg, reinterpret_cast<const void *>(&interp_ascii2float), nullptr, 0, 0, "Ascii2Float"},
    {host_obj_src_interp_float_glue_s + 156, GlueKind::reg, reinterpret_cast<const void *>(&interp_ascii2float_d), nullptr, 0, 0, "Ascii2FloatD"},
    {host_obj_src_interp_float_glue_s + 160, GlueKind::reg, reinterpret_cast<const void *>(&interp_float2ascii), nullptr, 0, 0, "Float2Ascii"},
    {host_obj_src_interp_float_glue_s + 164, GlueKind::reg, reinterpret_cast<const void *>(&interp_float2ascii_d), nullptr, 0, 0, "Float2AsciiD"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
