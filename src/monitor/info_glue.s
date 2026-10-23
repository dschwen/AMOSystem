; ---------------------------------------------------------------------
;	The monitor's information window, messages: monitor/info.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_info_centre
		XREF	_mon_info_click
		XREF	_mon_info_show_line
		XREF	_mon_info_count_lines
		XREF	_mon_info_show_all
		XREF	_mon_info_begin
		XREF	_mon_info_print
		XREF	_mon_info_end
		XREF	_mon_info_delete
		XREF	_mon_info_insert
		XREF	_mon_info_slider_click
		XREF	_mon_info_slider
		XREF	_mon_alert
		XREF	_mon_alert_delete
		XREF	_mon_delete_show
		XREF	_mon_activate_info
		XREF	_mon_activate_text
		XREF	_mon_activate
		XREF	_mon_message
		XREF	_mon_wait_release
		XREF	_mon_banks_monitor
		XREF	_mon_banks_program

		SECTION	.text,CODE

		xdef	_mon_info_centre_entry
_mon_info_centre_entry WEntry _mon_info_centre,l
		xdef	_mon_info_click_entry
_mon_info_click_entry WEntry _mon_info_click,l
		xdef	_mon_info_show_line_entry
_mon_info_show_line_entry WEntry _mon_info_show_line,l
		xdef	_mon_info_count_lines_entry
_mon_info_count_lines_entry WEntry _mon_info_count_lines,l
		xdef	_mon_info_show_all_entry
_mon_info_show_all_entry WEntry _mon_info_show_all,l
		xdef	_mon_info_begin_entry
_mon_info_begin_entry WEntry _mon_info_begin,l
		xdef	_mon_info_print_entry
_mon_info_print_entry WEntry _mon_info_print,l
		xdef	_mon_info_end_entry
_mon_info_end_entry WEntry _mon_info_end,l
		xdef	_mon_info_delete_entry
_mon_info_delete_entry WEntry _mon_info_delete,l
		xdef	_mon_info_insert_entry
_mon_info_insert_entry WEntry _mon_info_insert,l
		xdef	_mon_info_slider_click_entry
_mon_info_slider_click_entry WEntry _mon_info_slider_click,l
		xdef	_mon_info_slider_entry
_mon_info_slider_entry WEntry _mon_info_slider,l
		xdef	_mon_alert_entry
_mon_alert_entry WEntry _mon_alert,l
		xdef	_mon_alert_delete_entry
_mon_alert_delete_entry WEntry _mon_alert_delete,l
		xdef	_mon_delete_show_entry
_mon_delete_show_entry WEntry _mon_delete_show,l
		xdef	_mon_activate_info_entry
_mon_activate_info_entry WEntry _mon_activate_info,l
		xdef	_mon_activate_text_entry
_mon_activate_text_entry WEntry _mon_activate_text,l
		xdef	_mon_activate_entry
_mon_activate_entry WEntry _mon_activate,l
		xdef	_mon_message_entry
_mon_message_entry WEntry _mon_message,l
		xdef	_mon_wait_release_entry
_mon_wait_release_entry WEntry _mon_wait_release,l
		xdef	_mon_banks_monitor_entry
_mon_banks_monitor_entry WEntry _mon_banks_monitor,l
		xdef	_mon_banks_program_entry
_mon_banks_program_entry WEntry _mon_banks_program,l
