// src_lib_stubs_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_stubs_glue_s[496] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,0,0,0,0,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,83,112,114,105,116,101,115,32,73,99,111,110,
    115,32,32,32,77,117,115,105,99,32,32,32,65,109,97,108,32,32,32,32,77,101,110,117,
    32,32,32,32,68,97,116,97,32,32,32,32,87,111,114,107,32,32,32,32,65,115,109,32,
    32,32,32,32,73,102,102,32,32,32,32,32,76,111,97,100,105,110,103,33,65,109,66,107,
    65,109,83,112,65,109,66,115,65,109,73,99,0,0,0,0};
}

namespace amos::host {

void relocate_src_lib_stubs_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 108, lib_syntax);      // src/lib/stubs_glue.s:40
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 128, lib_syntax);      // src/lib/stubs_glue.s:41
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 148, lib_in_stop);      // src/lib/stubs_glue.s:42
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 168, lib_fnf_e);      // src/lib/stubs_glue.s:43
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 188, lib_in_error);      // src/lib/stubs_glue.s:44
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 208, lib_fn_true);      // src/lib/stubs_glue.s:47
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 228, lib_fn_one);      // src/lib/stubs_glue.s:48
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 248, lib_fn_false);      // src/lib/stubs_glue.s:49
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 300, lib_fn_else);      // src/lib/stubs_glue.s:54
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 320, lib_fn_null);      // src/lib/stubs_glue.s:55
    AMOS_LIBRARY_METADATA(host_obj_src_lib_stubs_glue_s + 344, lib_in_end);      // src/lib/stubs_glue.s:60
}

const GlueEntry entries_src_lib_stubs_glue_s[] = {
    {host_obj_src_lib_stubs_glue_s + 0, GlueKind::reg, AMOS_BIND_REG(lib_go_error), nullptr, 0, 0, "GoError"},
    {host_obj_src_lib_stubs_glue_s + 4, GlueKind::reg, AMOS_BIND_REG(lib_err_type_mis), nullptr, 0, 0, "TypeMis"},
    {host_obj_src_lib_stubs_glue_s + 8, GlueKind::reg, AMOS_BIND_REG(lib_err_fil_oo), nullptr, 0, 0, "FilOO"},
    {host_obj_src_lib_stubs_glue_s + 12, GlueKind::reg, AMOS_BIND_REG(lib_err_fil_no), nullptr, 0, 0, "FilNO"},
    {host_obj_src_lib_stubs_glue_s + 16, GlueKind::reg, AMOS_BIND_REG(lib_err_fil_tm), nullptr, 0, 0, "FilTM"},
    {host_obj_src_lib_stubs_glue_s + 20, GlueKind::reg, AMOS_BIND_REG(lib_err_eo_fil), nullptr, 0, 0, "EOFil"},
    {host_obj_src_lib_stubs_glue_s + 24, GlueKind::reg, AMOS_BIND_REG(lib_err_iff_for2), nullptr, 0, 0, "IffFor2"},
    {host_obj_src_lib_stubs_glue_s + 28, GlueKind::reg, AMOS_BIND_REG(lib_err_di_form), nullptr, 0, 0, "DiForm"},
    {host_obj_src_lib_stubs_glue_s + 32, GlueKind::reg, AMOS_BIND_REG(lib_err_bk_al_res), nullptr, 0, 0, "BkAlRes"},
    {host_obj_src_lib_stubs_glue_s + 36, GlueKind::reg, AMOS_BIND_REG(lib_err_bk_no_res), nullptr, 0, 0, "BkNoRes"},
    {host_obj_src_lib_stubs_glue_s + 40, GlueKind::reg, AMOS_BIND_REG(lib_err_oo_f_mem), nullptr, 0, 0, "OOfMem"},
    {host_obj_src_lib_stubs_glue_s + 44, GlueKind::reg, AMOS_BIND_REG(lib_err_stoo_long), nullptr, 0, 0, "StooLong"},
    {host_obj_src_lib_stubs_glue_s + 48, GlueKind::reg, AMOS_BIND_REG(lib_err_fon_call), nullptr, 0, 0, "FonCall"},
    {host_obj_src_lib_stubs_glue_s + 52, GlueKind::reg, AMOS_BIND_REG(lib_err_adr_err), nullptr, 0, 0, "AdrErr"},
    {host_obj_src_lib_stubs_glue_s + 56, GlueKind::reg, AMOS_BIND_REG(lib_err_cant_fit), nullptr, 0, 0, "CantFit"},
    {host_obj_src_lib_stubs_glue_s + 60, GlueKind::reg, AMOS_BIND_REG(lib_err_iff_for), nullptr, 0, 0, "IffFor"},
    {host_obj_src_lib_stubs_glue_s + 64, GlueKind::reg, AMOS_BIND_REG(lib_err_iff_cmp), nullptr, 0, 0, "IffCmp"},
    {host_obj_src_lib_stubs_glue_s + 68, GlueKind::reg, AMOS_BIND_REG(lib_err_fft_e), nullptr, 0, 0, "FftE"},
    {host_obj_src_lib_stubs_glue_s + 72, GlueKind::reg, AMOS_BIND_REG(lib_err_sp_err), nullptr, 0, 0, "SpErr"},
    {host_obj_src_lib_stubs_glue_s + 76, GlueKind::reg, AMOS_BIND_REG(lib_err_sc_n_op), nullptr, 0, 0, "ScNOp"},
    {host_obj_src_lib_stubs_glue_s + 80, GlueKind::reg, AMOS_BIND_REG(lib_err_rain_err), nullptr, 0, 0, "RainErr"},
    {host_obj_src_lib_stubs_glue_s + 84, GlueKind::reg, AMOS_BIND_REG(lib_err_ill_sc_n), nullptr, 0, 0, "IllScN"},
    {host_obj_src_lib_stubs_glue_s + 88, GlueKind::reg, AMOS_BIND_REG(lib_err_b_fon_call), nullptr, 0, 0, "BFonCall"},
    {host_obj_src_lib_stubs_glue_s + 92, GlueKind::reg, AMOS_BIND_REG(lib_err_w_fon_call), nullptr, 0, 0, "WFonCall"},
    {host_obj_src_lib_stubs_glue_s + 96, GlueKind::reg, AMOS_BIND_REG(lib_ec_wi_err), nullptr, 0, 0, "EcWiErr"},
    {host_obj_src_lib_stubs_glue_s + 100, GlueKind::reg, AMOS_BIND_REG(lib_cop_err), nullptr, 0, 0, "CopErr"},
    {host_obj_src_lib_stubs_glue_s + 104, GlueKind::reg, AMOS_BIND_REG(lib_ad_b_err), nullptr, 0, 0, "AdBErr"},
    {host_obj_src_lib_stubs_glue_s + 124, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_syntax), nullptr, 0, 3, "Syntax"},
    {host_obj_src_lib_stubs_glue_s + 144, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_syntax), nullptr, 0, 3, "Syntax2"},
    {host_obj_src_lib_stubs_glue_s + 164, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_stop), nullptr, 0, 3, "InStop"},
    {host_obj_src_lib_stubs_glue_s + 184, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fnf_e), nullptr, 0, 3, "FnfE"},
    {host_obj_src_lib_stubs_glue_s + 204, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_error), nullptr, 1, 3, "InError"},
    {host_obj_src_lib_stubs_glue_s + 224, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_true), nullptr, 0, 0, "FnTrue"},
    {host_obj_src_lib_stubs_glue_s + 244, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_one), nullptr, 0, 0, "FnOne"},
    {host_obj_src_lib_stubs_glue_s + 264, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_false), nullptr, 0, 0, "FnFalse"},
    {host_obj_src_lib_stubs_glue_s + 272, GlueKind::nop, nullptr, nullptr, 0, 0, "FnInt"},
    {host_obj_src_lib_stubs_glue_s + 280, GlueKind::nop, nullptr, nullptr, 0, 0, "FnPSel"},
    {host_obj_src_lib_stubs_glue_s + 288, GlueKind::nop, nullptr, nullptr, 0, 0, "InSetBuffer"},
    {host_obj_src_lib_stubs_glue_s + 296, GlueKind::nop, nullptr, nullptr, 0, 0, "InNull"},
    {host_obj_src_lib_stubs_glue_s + 316, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_else), nullptr, 0, 0, "FnElse"},
    {host_obj_src_lib_stubs_glue_s + 336, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_fn_null), nullptr, 0, 0, "FnNull"},
    {host_obj_src_lib_stubs_glue_s + 340, GlueKind::reg, AMOS_BIND_REG(lib_init), nullptr, 0, 0, "Init"},
    {host_obj_src_lib_stubs_glue_s + 360, GlueKind::libcall, AMOS_BIND_LIBRARY(lib_in_end), nullptr, 0, 3, "InEnd"},
    {host_obj_src_lib_stubs_glue_s + 368, GlueKind::call, AMOS_BIND_CALL(lib_in_rem), nullptr, 0, 0, "InRem"},
    {host_obj_src_lib_stubs_glue_s + 372, GlueKind::reg, AMOS_BIND_REG(lib_r_ill_dir), nullptr, 0, 0, "RIllDir"},
    {host_obj_src_lib_stubs_glue_s + 376, GlueKind::reg, AMOS_BIND_REG(lib_save_regs), nullptr, 0, 0, "SaveRegs"},
    {host_obj_src_lib_stubs_glue_s + 380, GlueKind::reg, AMOS_BIND_REG(lib_load_regs), nullptr, 0, 0, "LoadRegs"},
    {host_obj_src_lib_stubs_glue_s + 384, GlueKind::reg, AMOS_BIND_REG(lib_maj_d0), nullptr, 0, 0, "MajD0"},
    {host_obj_src_lib_stubs_glue_s + 388, GlueKind::reg, AMOS_BIND_REG(lib_ad_bob), nullptr, 0, 0, "AdBob"},
    {host_obj_src_lib_stubs_glue_s + 392, GlueKind::reg, AMOS_BIND_REG(lib_ad_icon), nullptr, 0, 0, "AdIcon"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
