// src_editor_status_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_editor_status_glue_s[176] = {
};
}

namespace amos::host {

void relocate_src_editor_status_glue_s()
{
}

const GlueEntry entries_src_editor_status_glue_s[] = {
    {host_obj_src_editor_status_glue_s + 0, GlueKind::w, AMOS_BIND_W(ed_get_systeme_fn), nullptr, 0, 1, "ed_get_systeme_entry"},
    {host_obj_src_editor_status_glue_s + 4, GlueKind::w, AMOS_BIND_W(ed_get_message_fn), nullptr, 0, 1, "ed_get_message_entry"},
    {host_obj_src_editor_status_glue_s + 8, GlueKind::w, AMOS_BIND_W(ed_get_message_a0), nullptr, 0, 1, "ed_get_message_a0_entry"},
    {host_obj_src_editor_status_glue_s + 12, GlueKind::w, AMOS_BIND_W(ed_get_next_message), nullptr, 0, 1, "ed_get_next_message_entry"},
    {host_obj_src_editor_status_glue_s + 16, GlueKind::w, AMOS_BIND_W(ed_status_y), nullptr, 0, 4, "ed_status_y_entry"},
    {host_obj_src_editor_status_glue_s + 20, GlueKind::w, AMOS_BIND_W(ed_status_x), nullptr, 0, 4, "ed_status_x_entry"},
    {host_obj_src_editor_status_glue_s + 24, GlueKind::w, AMOS_BIND_W(ed_status_free), nullptr, 0, 4, "ed_status_free_entry"},
    {host_obj_src_editor_status_glue_s + 28, GlueKind::w, AMOS_BIND_W(ed_status_order), nullptr, 0, 4, "ed_status_order_entry"},
    {host_obj_src_editor_status_glue_s + 32, GlueKind::w, AMOS_BIND_W(ed_status_insert), nullptr, 0, 4, "ed_status_insert_entry"},
    {host_obj_src_editor_status_glue_s + 36, GlueKind::w, AMOS_BIND_W(ed_status_name), nullptr, 0, 4, "ed_status_name_entry"},
    {host_obj_src_editor_status_glue_s + 40, GlueKind::w, AMOS_BIND_W(ed_name_to_buffer), nullptr, 0, 4, "ed_name_to_buffer_entry"},
    {host_obj_src_editor_status_glue_s + 44, GlueKind::w, AMOS_BIND_W(ed_recall_alert), nullptr, 0, 4, "ed_recall_alert_entry"},
    {host_obj_src_editor_status_glue_s + 48, GlueKind::w, AMOS_BIND_W(ed_alert_fn), nullptr, 0, 4, "ed_alert_entry"},
    {host_obj_src_editor_status_glue_s + 52, GlueKind::w, AMOS_BIND_W(ed_zap_alert), nullptr, 0, 4, "ed_zap_alert_entry"},
    {host_obj_src_editor_status_glue_s + 56, GlueKind::w, AMOS_BIND_W(ed_alert_clean), nullptr, 0, 4, "ed_alert_clean_entry"},
    {host_obj_src_editor_status_glue_s + 60, GlueKind::w, AMOS_BIND_W(ed_alert_rts), nullptr, 0, 4, "ed_alert_rts_entry"},
    {host_obj_src_editor_status_glue_s + 64, GlueKind::w, AMOS_BIND_W(ed_warn_name), nullptr, 0, 4, "ed_warn_name_entry"},
    {host_obj_src_editor_status_glue_s + 68, GlueKind::w, AMOS_BIND_W(ed_warn_message), nullptr, 0, 4, "ed_warn_message_entry"},
    {host_obj_src_editor_status_glue_s + 72, GlueKind::w, AMOS_BIND_W(ed_warn_fn), nullptr, 0, 4, "ed_warn_entry"},
    {host_obj_src_editor_status_glue_s + 76, GlueKind::w, AMOS_BIND_W(ed_warning_end), nullptr, 0, 2, "ed_warning_end_entry"},
    {host_obj_src_editor_status_glue_s + 80, GlueKind::w, AMOS_BIND_W(ed_all_warnings_end), nullptr, 0, 4, "ed_all_warnings_end_entry"},
    {host_obj_src_editor_status_glue_s + 84, GlueKind::w, AMOS_BIND_W(ed_status_line), nullptr, 0, 4, "ed_status_line_entry"},
    {host_obj_src_editor_status_glue_s + 88, GlueKind::w, AMOS_BIND_W(ed_status_line_d0), nullptr, 0, 4, "ed_status_line_d0_entry"},
    {host_obj_src_editor_status_glue_s + 92, GlueKind::w, AMOS_BIND_W(ed_status_text), nullptr, 0, 4, "ed_status_text_entry"},
    {host_obj_src_editor_status_glue_s + 96, GlueKind::w, AMOS_BIND_W(ed_not_edit_fn), nullptr, 0, 4, "ed_not_edit"},
    {host_obj_src_editor_status_glue_s + 100, GlueKind::w, AMOS_BIND_W(ed_line_too_long), nullptr, 0, 4, "ed_l_to_long"},
    {host_obj_src_editor_status_glue_s + 104, GlueKind::w, AMOS_BIND_W(ed_top_of_text_fn), nullptr, 0, 4, "ed_top_of_text"},
    {host_obj_src_editor_status_glue_s + 108, GlueKind::w, AMOS_BIND_W(ed_bottom_of_text_fn), nullptr, 0, 4, "ed_bottom_of_text"},
    {host_obj_src_editor_status_glue_s + 112, GlueKind::w, AMOS_BIND_W(ed_no_mark_fn), nullptr, 0, 4, "ed_no_mark"},
    {host_obj_src_editor_status_glue_s + 116, GlueKind::w, AMOS_BIND_W(ed_out_of_buffer), nullptr, 0, 4, "ed_oof_buf"},
    {host_obj_src_editor_status_glue_s + 120, GlueKind::w, AMOS_BIND_W(ed_fold_error), nullptr, 0, 4, "ed_fo_e"},
    {host_obj_src_editor_status_glue_s + 124, GlueKind::w, AMOS_BIND_W(ed_alert_100_fn), nullptr, 0, 4, "ed_alert_100"},
    {host_obj_src_editor_status_glue_s + 128, GlueKind::w, AMOS_BIND_W(ed_out_of_memory_fn), nullptr, 0, 4, "ed_out_of_memory"},
    {host_obj_src_editor_status_glue_s + 132, GlueKind::w, AMOS_BIND_W(ed_no_found_fn), nullptr, 0, 4, "ed_no_found"},
    {host_obj_src_editor_status_glue_s + 136, GlueKind::w, AMOS_BIND_W(ed_not_done_fn), nullptr, 0, 4, "ed_not_done"},
    {host_obj_src_editor_status_glue_s + 140, GlueKind::w, AMOS_BIND_W(ed_not_done2_fn), nullptr, 0, 4, "ed_not_done2"},
    {host_obj_src_editor_status_glue_s + 144, GlueKind::w, AMOS_BIND_W(ed_not_amos_fn), nullptr, 0, 4, "ed_not_amos"},
    {host_obj_src_editor_status_glue_s + 148, GlueKind::w, AMOS_BIND_W(ed_block_what_fn), nullptr, 0, 4, "ed_block_what"},
    {host_obj_src_editor_status_glue_s + 152, GlueKind::w, AMOS_BIND_W(ed_too_many_windows_fn), nullptr, 0, 4, "ed_too_many_windows"},
    {host_obj_src_editor_status_glue_s + 156, GlueKind::w, AMOS_BIND_W(ed_no_hide_fn), nullptr, 0, 4, "ed_no_hide"},
    {host_obj_src_editor_status_glue_s + 160, GlueKind::w, AMOS_BIND_W(ed_already_runned_fn), nullptr, 0, 4, "ed_already_runned"},
    {host_obj_src_editor_status_glue_s + 164, GlueKind::w, AMOS_BIND_W(ed_no_undo_fn), nullptr, 0, 4, "ed_no_undo"},
    {host_obj_src_editor_status_glue_s + 168, GlueKind::w, AMOS_BIND_W(ed_no_redo_fn), nullptr, 0, 4, "ed_no_redo"},
    {host_obj_src_editor_status_glue_s + 172, GlueKind::w, AMOS_BIND_W(ed_pro_alert_fn), nullptr, 0, 4, "ed_pro_alert"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
