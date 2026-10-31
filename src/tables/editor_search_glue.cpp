// src_editor_search_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_search_glue_s[48] = {
};
}

namespace amos::host {

void relocate_src_editor_search_glue_s()
{
}

const GlueEntry entries_src_editor_search_glue_s[] = {
    {host_obj_src_editor_search_glue_s + 0, GlueKind::w, reinterpret_cast<const void *>(&ed_search), nullptr, 0, 4, "ed_search_entry"},
    {host_obj_src_editor_search_glue_s + 4, GlueKind::w, reinterpret_cast<const void *>(&ed_search_front), nullptr, 0, 4, "ed_search_front_entry"},
    {host_obj_src_editor_search_glue_s + 8, GlueKind::w, reinterpret_cast<const void *>(&ed_search_back), nullptr, 0, 4, "ed_search_back_entry"},
    {host_obj_src_editor_search_glue_s + 12, GlueKind::w, reinterpret_cast<const void *>(&ed_replace_text), nullptr, 0, 4, "ed_replace_text_entry"},
    {host_obj_src_editor_search_glue_s + 16, GlueKind::w, reinterpret_cast<const void *>(&ed_goto_line), nullptr, 0, 4, "ed_goto_line_entry"},
    {host_obj_src_editor_search_glue_s + 20, GlueKind::w, reinterpret_cast<const void *>(&ed_search_dialogue), nullptr, 0, 2, "ed_search_dialogue_entry"},
    {host_obj_src_editor_search_glue_s + 24, GlueKind::w, reinterpret_cast<const void *>(&ed_search_new), nullptr, 0, 4, "ed_search_new_entry"},
    {host_obj_src_editor_search_glue_s + 28, GlueKind::w, reinterpret_cast<const void *>(&ed_search_previous), nullptr, 0, 4, "ed_search_previous_entry"},
    {host_obj_src_editor_search_glue_s + 32, GlueKind::w, reinterpret_cast<const void *>(&ed_search_next), nullptr, 0, 4, "ed_search_next_entry"},
    {host_obj_src_editor_search_glue_s + 36, GlueKind::w, reinterpret_cast<const void *>(&ed_replace_new), nullptr, 0, 4, "ed_replace_new_entry"},
    {host_obj_src_editor_search_glue_s + 40, GlueKind::w, reinterpret_cast<const void *>(&ed_replace_next), nullptr, 0, 4, "ed_replace_next_entry"},
    {host_obj_src_editor_search_glue_s + 44, GlueKind::w, reinterpret_cast<const void *>(&ed_replace_previous), nullptr, 0, 4, "ed_replace_previous_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
