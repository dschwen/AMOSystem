; ---------------------------------------------------------------------
;	Messages, alerts, warnings, status lines: editor/status.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_get_systeme_fn
		XREF	_ed_get_message_fn
		XREF	_ed_get_message_a0
		XREF	_ed_get_next_message
		XREF	_ed_status_y
		XREF	_ed_status_x
		XREF	_ed_status_free
		XREF	_ed_status_order
		XREF	_ed_status_insert
		XREF	_ed_status_name
		XREF	_ed_name_to_buffer
		XREF	_ed_recall_alert
		XREF	_ed_alert_fn
		XREF	_ed_zap_alert
		XREF	_ed_alert_clean
		XREF	_ed_alert_rts
		XREF	_ed_warn_name
		XREF	_ed_warn_message
		XREF	_ed_warn_fn
		XREF	_ed_warning_end
		XREF	_ed_all_warnings_end
		XREF	_ed_status_line
		XREF	_ed_status_line_d0
		XREF	_ed_status_text

		SECTION	.text,CODE

		xdef	_ed_get_systeme_entry
_ed_get_systeme_entry WEntry _ed_get_systeme_fn,b
		xdef	_ed_get_message_entry
_ed_get_message_entry WEntry _ed_get_message_fn,b
		xdef	_ed_get_message_a0_entry
_ed_get_message_a0_entry WEntry _ed_get_message_a0,b
		xdef	_ed_get_next_message_entry
_ed_get_next_message_entry WEntry _ed_get_next_message,b
		xdef	_ed_status_y_entry
_ed_status_y_entry WEntry _ed_status_y,l
		xdef	_ed_status_x_entry
_ed_status_x_entry WEntry _ed_status_x,l
		xdef	_ed_status_free_entry
_ed_status_free_entry WEntry _ed_status_free,l
		xdef	_ed_status_order_entry
_ed_status_order_entry WEntry _ed_status_order,l
		xdef	_ed_status_insert_entry
_ed_status_insert_entry WEntry _ed_status_insert,l
		xdef	_ed_status_name_entry
_ed_status_name_entry WEntry _ed_status_name,l
		xdef	_ed_name_to_buffer_entry
_ed_name_to_buffer_entry WEntry _ed_name_to_buffer,l
		xdef	_ed_recall_alert_entry
_ed_recall_alert_entry WEntry _ed_recall_alert,l
		xdef	_ed_alert_entry
_ed_alert_entry WEntry _ed_alert_fn,l
		xdef	_ed_zap_alert_entry
_ed_zap_alert_entry WEntry _ed_zap_alert,l
		xdef	_ed_alert_clean_entry
_ed_alert_clean_entry WEntry _ed_alert_clean,l
		xdef	_ed_alert_rts_entry
_ed_alert_rts_entry WEntry _ed_alert_rts,l
		xdef	_ed_warn_name_entry
_ed_warn_name_entry WEntry _ed_warn_name,l
		xdef	_ed_warn_message_entry
_ed_warn_message_entry WEntry _ed_warn_message,l
		xdef	_ed_warn_entry
_ed_warn_entry WEntry _ed_warn_fn,l
		xdef	_ed_warning_end_entry
_ed_warning_end_entry WEntry _ed_warning_end,w
		xdef	_ed_all_warnings_end_entry
_ed_all_warnings_end_entry WEntry _ed_all_warnings_end,l
		xdef	_ed_status_line_entry
_ed_status_line_entry WEntry _ed_status_line,l
		xdef	_ed_status_line_d0_entry
_ed_status_line_d0_entry WEntry _ed_status_line_d0,l
		xdef	_ed_status_text_entry
_ed_status_text_entry WEntry _ed_status_text,l

; The alerts of the editor's messages (+Edit.s Ed_NotEdit...)
		XREF	_ed_not_edit_fn
		xdef	_ed_not_edit
_ed_not_edit WEntry _ed_not_edit_fn,l
		XREF	_ed_line_too_long
		xdef	_ed_l_to_long
_ed_l_to_long WEntry _ed_line_too_long,l
		XREF	_ed_top_of_text_fn
		xdef	_ed_top_of_text
_ed_top_of_text WEntry _ed_top_of_text_fn,l
		XREF	_ed_bottom_of_text_fn
		xdef	_ed_bottom_of_text
_ed_bottom_of_text WEntry _ed_bottom_of_text_fn,l
		XREF	_ed_no_mark_fn
		xdef	_ed_no_mark
_ed_no_mark WEntry _ed_no_mark_fn,l
		XREF	_ed_out_of_buffer
		xdef	_ed_oof_buf
_ed_oof_buf WEntry _ed_out_of_buffer,l
		XREF	_ed_fold_error
		xdef	_ed_fo_e
_ed_fo_e WEntry _ed_fold_error,l
		XREF	_ed_alert_100_fn
		xdef	_ed_alert_100
_ed_alert_100 WEntry _ed_alert_100_fn,l
		XREF	_ed_out_of_memory_fn
		xdef	_ed_out_of_memory
_ed_out_of_memory WEntry _ed_out_of_memory_fn,l
		XREF	_ed_no_found_fn
		xdef	_ed_no_found
_ed_no_found WEntry _ed_no_found_fn,l
		XREF	_ed_not_done_fn
		xdef	_ed_not_done
_ed_not_done WEntry _ed_not_done_fn,l
		XREF	_ed_not_done2_fn
		xdef	_ed_not_done2
_ed_not_done2 WEntry _ed_not_done2_fn,l
		XREF	_ed_not_amos_fn
		xdef	_ed_not_amos
_ed_not_amos WEntry _ed_not_amos_fn,l
		XREF	_ed_block_what_fn
		xdef	_ed_block_what
_ed_block_what WEntry _ed_block_what_fn,l
		XREF	_ed_too_many_windows_fn
		xdef	_ed_too_many_windows
_ed_too_many_windows WEntry _ed_too_many_windows_fn,l
		XREF	_ed_no_hide_fn
		xdef	_ed_no_hide
_ed_no_hide WEntry _ed_no_hide_fn,l
		XREF	_ed_already_runned_fn
		xdef	_ed_already_runned
_ed_already_runned WEntry _ed_already_runned_fn,l
		XREF	_ed_no_undo_fn
		xdef	_ed_no_undo
_ed_no_undo WEntry _ed_no_undo_fn,l
		XREF	_ed_no_redo_fn
		xdef	_ed_no_redo
_ed_no_redo WEntry _ed_no_redo_fn,l
		XREF	_ed_pro_alert_fn
		xdef	_ed_pro_alert
_ed_pro_alert WEntry _ed_pro_alert_fn,l
