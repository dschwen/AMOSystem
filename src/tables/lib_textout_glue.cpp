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
    {host_obj_src_lib_textout_glue_s + 0, GlueKind::reg, reinterpret_cast<const void *>(&lib_ch_ver_buf), nullptr, 0, 0, "ChVerBuf"},
    {host_obj_src_lib_textout_glue_s + 4, GlueKind::reg, reinterpret_cast<const void *>(&lib_ch_ver_buf2), nullptr, 0, 0, "ChVerBuf2"},
    {host_obj_src_lib_textout_glue_s + 8, GlueKind::reg, reinterpret_cast<const void *>(&lib_str2_chaine), nullptr, 0, 0, "Str2Chaine"},
    {host_obj_src_lib_textout_glue_s + 12, GlueKind::reg, reinterpret_cast<const void *>(&lib_mes2_chaine), nullptr, 0, 0, "Mes2Chaine"},
    {host_obj_src_lib_textout_glue_s + 16, GlueKind::reg, reinterpret_cast<const void *>(&lib_mc_suit), nullptr, 0, 0, "MCSuit"},
    {host_obj_src_lib_textout_glue_s + 20, GlueKind::reg, reinterpret_cast<const void *>(&lib_set_hi_chaine), nullptr, 0, 0, "Set_HiChaine"},
    {host_obj_src_lib_textout_glue_s + 24, GlueKind::reg, reinterpret_cast<const void *>(&lib_ret_ch_vide), nullptr, 0, 0, "Ret_ChVide"},
    {host_obj_src_lib_textout_glue_s + 28, GlueKind::reg, reinterpret_cast<const void *>(&lib_a0_to_chaine), nullptr, 0, 0, "A0ToChaine"},
    {host_obj_src_lib_textout_glue_s + 32, GlueKind::reg, reinterpret_cast<const void *>(&lib_fin_bin), nullptr, 0, 0, "FinBin"},
    {host_obj_src_lib_textout_glue_s + 36, GlueKind::reg, reinterpret_cast<const void *>(&lib_imp_chaine), nullptr, 0, 0, "ImpChaine"},
    {host_obj_src_lib_textout_glue_s + 40, GlueKind::reg, reinterpret_cast<const void *>(&lib_prt_open), nullptr, 0, 0, "PRT_Open"},
    {host_obj_src_lib_textout_glue_s + 44, GlueKind::reg, reinterpret_cast<const void *>(&lib_prt_close), nullptr, 0, 0, "PRT_Close"},
    {host_obj_src_lib_textout_glue_s + 48, GlueKind::reg, reinterpret_cast<const void *>(&lib_prt_print), nullptr, 0, 0, "PRT_Print"},
    {host_obj_src_lib_textout_glue_s + 56, GlueKind::record, reinterpret_cast<const void *>(&lib_prt_close_fn), nullptr, 0, 0, "lib_prt_close_record"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
