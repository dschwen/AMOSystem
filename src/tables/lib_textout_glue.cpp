// src_lib_textout_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_lib_textout_glue_s[60] = {
};
}

namespace amos::host {

void relocate_src_lib_textout_glue_s()
{
}

const GlueEntry entries_src_lib_textout_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 0), GlueKind::reg, AMOS_BIND_REG(lib_ch_ver_buf), nullptr, 0, 0, "ChVerBuf"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 4), GlueKind::reg, AMOS_BIND_REG(lib_ch_ver_buf2), nullptr, 0, 0, "ChVerBuf2"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 8), GlueKind::reg, AMOS_BIND_REG(lib_str2_chaine), nullptr, 0, 0, "Str2Chaine"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 12), GlueKind::reg, AMOS_BIND_REG(lib_mes2_chaine), nullptr, 0, 0, "Mes2Chaine"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 16), GlueKind::reg, AMOS_BIND_REG(lib_mc_suit), nullptr, 0, 0, "MCSuit"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 20), GlueKind::reg, AMOS_BIND_REG(lib_set_hi_chaine), nullptr, 0, 0, "Set_HiChaine"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 24), GlueKind::reg, AMOS_BIND_REG(lib_ret_ch_vide), nullptr, 0, 0, "Ret_ChVide"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 28), GlueKind::reg, AMOS_BIND_REG(lib_a0_to_chaine), nullptr, 0, 0, "A0ToChaine"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 32), GlueKind::reg, AMOS_BIND_REG(lib_fin_bin), nullptr, 0, 0, "FinBin"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 36), GlueKind::reg, AMOS_BIND_REG(lib_imp_chaine), nullptr, 0, 0, "ImpChaine"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 40), GlueKind::reg, AMOS_BIND_REG(lib_prt_open), nullptr, 0, 0, "PRT_Open"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 44), GlueKind::reg, AMOS_BIND_REG(lib_prt_close), nullptr, 0, 0, "PRT_Close"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 48), GlueKind::reg, AMOS_BIND_REG(lib_prt_print), nullptr, 0, 0, "PRT_Print"},
    {AMOS_ENTRY_AT(host_obj_src_lib_textout_glue_s, 56), GlueKind::record, AMOS_BIND_CALL(lib_prt_close_fn), nullptr, 0, 0, "lib_prt_close_record"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
