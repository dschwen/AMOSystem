// src_editor_storage_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_storage_glue_s[28] = {
};
}

namespace amos::host {

void relocate_src_editor_storage_glue_s()
{
}

const GlueEntry entries_src_editor_storage_glue_s[] = {
    {host_obj_src_editor_storage_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_store), nullptr, 0, 4, "ed_store_entry"},
    {host_obj_src_editor_storage_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_store_block), nullptr, 0, 4, "ed_store_block_entry"},
    {host_obj_src_editor_storage_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_line), nullptr, 0, 4, "ed_delete_line_entry"},
    {host_obj_src_editor_storage_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_chunk_at), nullptr, 0, 4, "ed_delete_chunk_at_entry"},
    {host_obj_src_editor_storage_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_delete_chunk), nullptr, 0, 4, "ed_delete_chunk_entry"},
    {host_obj_src_editor_storage_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_find_line), nullptr, 0, 2, "ed_find_line_entry"},
    {host_obj_src_editor_storage_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_next_line), nullptr, 0, 2, "ed_next_line_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
