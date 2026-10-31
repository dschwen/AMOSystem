// src_editor_blocksave_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_blocksave_glue_s[28] = {
};
}

namespace amos::host {

void relocate_src_editor_blocksave_glue_s()
{
}

const GlueEntry entries_src_editor_blocksave_glue_s[] = {
    {host_obj_src_editor_blocksave_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_block_save), nullptr, 0, 4, "ed_block_save_entry"},
    {host_obj_src_editor_blocksave_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_block_save_ascii), nullptr, 0, 4, "ed_block_save_ascii_entry"},
    {host_obj_src_editor_blocksave_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_print_program), nullptr, 0, 4, "ed_print_program_entry"},
    {host_obj_src_editor_blocksave_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_print_block), nullptr, 0, 4, "ed_print_block_entry"},
    {host_obj_src_editor_blocksave_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_block_limits_2), nullptr, 0, 4, "ed_block_limits_2_entry"},
    {host_obj_src_editor_blocksave_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_printer_open), nullptr, 0, 4, "ed_printer_open_entry"},
    {host_obj_src_editor_blocksave_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_printer_print), nullptr, 0, 4, "ed_printer_print_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
