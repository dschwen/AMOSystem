; ---------------------------------------------------------------------
;	The display of the editor's window: editor/display.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_show_slider,_ed_cursor_on,_ed_cursor_off,_ed_show_current_line,_ed_centre
		XREF	_ed_goto_x_fn,_ed_goto_y_fn,_ed_set_x,_ed_set_y,_ed_set_xy,_ed_labels_around
		XREF	_ed_place_cursor,_ed_show_column,_ed_new_buffer_all,_ed_new_buffer,_ed_show_all
		XREF	_ed_show_line,_ed_show_line_only

		SECTION	.text,CODE

		xdef	_ed_show_slider_entry,_ed_cursor_on_entry,_ed_cursor_off_entry
		xdef	_ed_show_current_line_entry,_ed_centre_entry,_ed_goto_x_entry,_ed_goto_y_entry
		xdef	_ed_set_x_entry,_ed_set_y_entry,_ed_set_xy_entry,_ed_labels_around_entry
		xdef	_ed_place_cursor_entry,_ed_show_column_entry,_ed_new_buffer_all_entry
		xdef	_ed_new_buffer_entry,_ed_show_all_entry,_ed_show_line_entry,_ed_show_line_only_entry
_ed_show_slider_entry WEntry _ed_show_slider,l
_ed_cursor_on_entry WEntry _ed_cursor_on,l
_ed_cursor_off_entry WEntry _ed_cursor_off,l
_ed_show_current_line_entry WEntry _ed_show_current_line,l
_ed_centre_entry WEntry _ed_centre,l
_ed_goto_x_entry WEntry _ed_goto_x_fn,l
_ed_goto_y_entry WEntry _ed_goto_y_fn,l
_ed_set_x_entry	WEntry	_ed_set_x,l
_ed_set_y_entry	WEntry	_ed_set_y,l
_ed_set_xy_entry WEntry	_ed_set_xy,l
_ed_labels_around_entry WEntry _ed_labels_around,l
_ed_place_cursor_entry WEntry _ed_place_cursor,l
_ed_show_column_entry WEntry _ed_show_column,l
_ed_new_buffer_all_entry WEntry _ed_new_buffer_all,l
_ed_new_buffer_entry WEntry _ed_new_buffer,l
_ed_show_all_entry WEntry _ed_show_all,l
_ed_show_line_entry WEntry _ed_show_line,l
_ed_show_line_only_entry WEntry _ed_show_line_only,l
