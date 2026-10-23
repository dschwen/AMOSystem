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
