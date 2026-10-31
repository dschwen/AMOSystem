// src_verif_program_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_verif_program_glue_s[72] = {
};
}

namespace amos::host {

void relocate_src_verif_program_glue_s()
{
}

const GlueEntry entries_src_verif_program_glue_s[] = {
    {host_obj_src_verif_program_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_run_it), nullptr, 0, 4, "ver_prg_run_it_entry"},
    {host_obj_src_verif_program_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_test_it), nullptr, 0, 4, "ver_prg_test_it_entry"},
    {host_obj_src_verif_program_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_push), nullptr, 0, 4, "ver_prg_push_entry"},
    {host_obj_src_verif_program_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_pull), nullptr, 0, 4, "ver_prg_pull_entry"},
    {host_obj_src_verif_program_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_data_save), nullptr, 0, 4, "ver_prg_data_save_entry"},
    {host_obj_src_verif_program_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_data_load), nullptr, 0, 4, "ver_prg_data_load_entry"},
    {host_obj_src_verif_program_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_data_new), nullptr, 0, 4, "ver_prg_data_new_entry"},
    {host_obj_src_verif_program_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_new_structure), nullptr, 0, 4, "ver_prg_new_structure_entry"},
    {host_obj_src_verif_program_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_del_structure), nullptr, 0, 4, "ver_prg_del_structure_entry"},
    {host_obj_src_verif_program_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_set_banks), nullptr, 0, 4, "ver_prg_set_banks_entry"},
    {host_obj_src_verif_program_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_reset_banks), nullptr, 0, 4, "ver_prg_reset_banks_entry"},
    {host_obj_src_verif_program_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_new), nullptr, 0, 4, "ver_prg_new_entry"},
    {host_obj_src_verif_program_glue_s + 48, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_chg_ttexte), nullptr, 0, 4, "ver_prg_chg_ttexte_entry"},
    {host_obj_src_verif_program_glue_s + 52, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_load), nullptr, 0, 4, "ver_prg_load_entry"},
    {host_obj_src_verif_program_glue_s + 56, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_cpt_lines), nullptr, 0, 4, "ver_prg_cpt_lines_entry"},
    {host_obj_src_verif_program_glue_s + 60, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_acc_adr), nullptr, 0, 4, "ver_prg_acc_adr_entry"},
    {host_obj_src_verif_program_glue_s + 64, GlueKind::w, reinterpret_cast<const void *>(&ver_prg_save), nullptr, 0, 4, "ver_prg_save_entry"},
    {host_obj_src_verif_program_glue_s + 68, GlueKind::w_f, reinterpret_cast<const void *>(&ver_prg_deja_runned), nullptr, 0, 0, "ver_prg_deja_runned_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
