// src_editor_tokens_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_tokens_glue_s[16] = {
};
}

namespace amos::host {

void relocate_src_editor_tokens_glue_s()
{
}

const GlueEntry entries_src_editor_tokens_glue_s[] = {
    {host_obj_src_editor_tokens_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_tokenise), nullptr, 0, 2, "ed_tokenise_entry"},
    {host_obj_src_editor_tokens_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_detok_editor), nullptr, 0, 4, "ed_detok_entry"},
    {host_obj_src_editor_tokens_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_detok_monitor), nullptr, 0, 4, "ed_mon_detok_entry"},
    {host_obj_src_editor_tokens_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_skip_token), nullptr, 0, 4, "ed_tinst_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
