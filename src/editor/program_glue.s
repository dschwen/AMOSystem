; ---------------------------------------------------------------------
;	The program as a whole: editor/program.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_new_fn
		XREF	_ed_new2_fn
		XREF	_ed_window_new
		XREF	_ed_save_over_fn
		XREF	_ed_saved_fn
		XREF	_ed_set_buffer
		XREF	_ed_set_buffer_from
		XREF	_ed_get_place_fn
		XREF	_ed_file_selector_fn
		XREF	_ed_open_load

		SECTION	.text,CODE

		xdef	_ed_new_entry
_ed_new_entry WEntry _ed_new_fn,l
		xdef	_ed_new2_entry
_ed_new2_entry WEntry _ed_new2_fn,l
		xdef	_ed_window_new_entry
_ed_window_new_entry WEntry _ed_window_new,l
		xdef	_ed_save_over_entry
_ed_save_over_entry WEntry _ed_save_over_fn,l
		xdef	_ed_saved_entry
_ed_saved_entry WEntry _ed_saved_fn,l
		xdef	_ed_set_buffer_entry
_ed_set_buffer_entry WEntry _ed_set_buffer,l
		xdef	_ed_set_buffer_from_entry
_ed_set_buffer_from_entry WEntry _ed_set_buffer_from,l
		xdef	_ed_get_place_entry
_ed_get_place_entry WEntry _ed_get_place_fn,l
		xdef	_ed_file_selector_entry
_ed_file_selector_entry WEntry _ed_file_selector_fn,w
		xdef	_ed_open_load_entry
_ed_open_load_entry WEntry _ed_open_load,l
