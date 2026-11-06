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
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 0), GlueKind::missing, nullptr, nullptr, 0, 0, "interp_lvo"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 4), GlueKind::reg, AMOS_BIND_REG(interp_int_to_fl1), nullptr, 0, 0, "IntToFl1"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 8), GlueKind::reg, AMOS_BIND_REG(interp_dint_to_fl1), nullptr, 0, 0, "DIntToFl1"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 12), GlueKind::reg, AMOS_BIND_REG(interp_int_to_fl2), nullptr, 0, 0, "IntToFl2"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 16), GlueKind::reg, AMOS_BIND_REG(interp_dint_to_fl2), nullptr, 0, 0, "DIntToFl2"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 20), GlueKind::reg, AMOS_BIND_REG(interp_fl_to_int1), nullptr, 0, 0, "FlToInt1"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 24), GlueKind::reg, AMOS_BIND_REG(interp_dfl_to_int1), nullptr, 0, 0, "DFlToInt1"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 28), GlueKind::reg, AMOS_BIND_REG(interp_fl_to_int2), nullptr, 0, 0, "FlToInt2"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 32), GlueKind::reg, AMOS_BIND_REG(interp_dfl_to_int2), nullptr, 0, 0, "DFlToInt2"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 36), GlueKind::reg, AMOS_BIND_REG(interp_math_fonction), nullptr, 0, 0, "Math_Fonction"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 40), GlueKind::reg, AMOS_BIND_REG(interp_dmath_fonction), nullptr, 0, 0, "DMath_Fonction"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 44), GlueKind::reg, AMOS_BIND_REG(interp_float_compare), nullptr, 0, 0, "Float_Compare"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 48), GlueKind::reg, AMOS_BIND_REG(interp_dfloat_compare), nullptr, 0, 0, "DFloat_Compare"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 52), GlueKind::reg, AMOS_BIND_REG(interp_float_operation), nullptr, 0, 0, "Float_Operation"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 56), GlueKind::reg, AMOS_BIND_REG(interp_dfloat_operation), nullptr, 0, 0, "DFloat_Operation"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 60), GlueKind::reg, AMOS_BIND_REG(interp_float_test), nullptr, 0, 0, "Float_Test"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 64), GlueKind::reg, AMOS_BIND_REG(interp_float_testf), nullptr, 0, 0, "Float_TestF"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 68), GlueKind::reg, AMOS_BIND_REG(interp_math_operation), nullptr, 0, 0, "Math_Operation"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 72), GlueKind::reg, AMOS_BIND_REG(interp_dmath_operation), nullptr, 0, 0, "DMath_Operation"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 76), GlueKind::reg, AMOS_BIND_REG(interp_float_fonction), nullptr, 0, 0, "Float_Fonction"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 80), GlueKind::reg, AMOS_BIND_REG(interp_dfloat_fonction), nullptr, 0, 0, "DFloat_Fonction"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 84), GlueKind::reg, AMOS_BIND_REG(interp_fl_pos), nullptr, 0, 0, "FlPos"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 88), GlueKind::reg, AMOS_BIND_REG(interp_fl_pos_d), nullptr, 0, 0, "FlPosD"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 92), GlueKind::reg, AMOS_BIND_REG(interp_aangle), nullptr, 0, 0, "AAngle"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 96), GlueKind::reg, AMOS_BIND_REG(interp_aangle_d), nullptr, 0, 0, "AAngleD"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 100), GlueKind::reg, AMOS_BIND_REG(interp_ffangle), nullptr, 0, 0, "FFAngle"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 104), GlueKind::reg, AMOS_BIND_REG(interp_fangle_d), nullptr, 0, 0, "FAngleD"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 108), GlueKind::reg, AMOS_BIND_REG(interp_fn_param_f), nullptr, 0, 0, "FnParamF"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 112), GlueKind::reg, AMOS_BIND_REG(interp_fn_param_d), nullptr, 0, 0, "FnParamD"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 116), GlueKind::reg, AMOS_BIND_REG(interp_demande), nullptr, 0, 0, "Demande"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 120), GlueKind::reg, AMOS_BIND_REG(interp_ddemande), nullptr, 0, 0, "DDemande"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 124), GlueKind::reg, AMOS_BIND_REG(interp_menage), nullptr, 0, 0, "Menage"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 128), GlueKind::reg, AMOS_BIND_REG(interp_val_rout), nullptr, 0, 0, "ValRout"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 132), GlueKind::reg, AMOS_BIND_REG(interp_defrun1), nullptr, 0, 0, "DefRun1"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 136), GlueKind::reg, AMOS_BIND_REG(interp_defrun2), nullptr, 0, 0, "DefRun2"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 140), GlueKind::reg, AMOS_BIND_REG(interp_defrun_acc), nullptr, 0, 0, "DefRunAcc"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 144), GlueKind::reg, AMOS_BIND_REG(interp_defrun_extensions), nullptr, 0, 0, "DefRunExtensions"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 148), GlueKind::call, AMOS_BIND_CALL(interp_open_math_libraries), nullptr, 0, 0, "Open_MathLibraries"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 152), GlueKind::reg, AMOS_BIND_REG(interp_ascii2float), nullptr, 0, 0, "Ascii2Float"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 156), GlueKind::reg, AMOS_BIND_REG(interp_ascii2float_d), nullptr, 0, 0, "Ascii2FloatD"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 160), GlueKind::reg, AMOS_BIND_REG(interp_float2ascii), nullptr, 0, 0, "Float2Ascii"},
    {AMOS_ENTRY_AT(host_obj_src_interp_float_glue_s, 164), GlueKind::reg, AMOS_BIND_REG(interp_float2ascii_d), nullptr, 0, 0, "Float2AsciiD"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
