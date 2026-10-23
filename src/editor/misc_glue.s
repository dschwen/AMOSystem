; ---------------------------------------------------------------------
;	The rest of the editor: editor/misc.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_set_tab
		XREF	_ed_slider_update
		XREF	_ed_slider_draw
		XREF	_ed_close_name
		XREF	_ed_enlarge_fn
		XREF	_ed_button_draw
		XREF	_ed_button_draw_title
		XREF	_ed_disk_error
		XREF	_ed_load_error
		XREF	_ed_print_error_fn
		XREF	_ed_close_fn
		XREF	_ed_workbench

		SECTION	.text,CODE

		xdef	_ed_set_tab_entry
_ed_set_tab_entry WEntry _ed_set_tab,l
		xdef	_ed_slider_update_entry
_ed_slider_update_entry WEntry _ed_slider_update,l
		xdef	_ed_slider_draw_entry
_ed_slider_draw_entry WEntry _ed_slider_draw,l
		xdef	_ed_close_name_entry
_ed_close_name_entry WEntry _ed_close_name,l
		xdef	_ed_enlarge_fn_entry
_ed_enlarge_fn_entry WEntry _ed_enlarge_fn,l
		xdef	_ed_button_draw_entry
_ed_button_draw_entry WEntry _ed_button_draw,l
		xdef	_ed_button_draw_title_entry
_ed_button_draw_title_entry WEntry _ed_button_draw_title,l
		xdef	_ed_disk_error_entry
_ed_disk_error_entry WEntry _ed_disk_error,l
		xdef	_ed_load_error_entry
_ed_load_error_entry WEntry _ed_load_error,l
		xdef	_ed_print_error_fn_entry
_ed_print_error_fn_entry WEntry _ed_print_error_fn,l
		xdef	_ed_close_fn_entry
_ed_close_fn_entry WEntry _ed_close_fn,l
		xdef	_ed_workbench_entry
_ed_workbench_entry WEntry _ed_workbench,l
