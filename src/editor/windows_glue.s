; ---------------------------------------------------------------------
;	The editor's windows: editor/windows.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_top_up
		XREF	_ed_top_down
		XREF	_ed_bottom_up
		XREF	_ed_bottom_down
		XREF	_ed_drag_top
		XREF	_ed_drag_bottom
		XREF	_ed_link_cursor
		XREF	_ed_unlink_scroll
		XREF	_ed_follow_scroll
		XREF	_ed_split
		XREF	_ed_update_split
		XREF	_ed_open_shown_window
		XREF	_ed_open_window
		XREF	_ed_hide_window
		XREF	_ed_button_close
		XREF	_ed_button_hide
		XREF	_ed_button_size
		XREF	_ed_close_window_quit
		XREF	_ed_close_window
		XREF	_ed_new_hidden
		XREF	_ed_new_all_hidden
		XREF	_ed_edit_hidden
		XREF	_ed_get_hidden
		XREF	_ed_delete_window
		XREF	_ed_delete_windows
		XREF	_ed_draw_windows
		XREF	_ed_erase_windows
		XREF	_ed_delete_all_zones
		XREF	_ed_empty_next
		XREF	_ed_next_window
		XREF	_ed_previous_window
		XREF	_ed_flip_size
		XREF	_ed_window_number
		XREF	_ed_shrink_all
		XREF	_ed_max_size
		XREF	_ed_activate

		SECTION	.text,CODE

		xdef	_ed_top_up_entry
_ed_top_up_entry WEntry _ed_top_up,l
		xdef	_ed_top_down_entry
_ed_top_down_entry WEntry _ed_top_down,l
		xdef	_ed_bottom_up_entry
_ed_bottom_up_entry WEntry _ed_bottom_up,l
		xdef	_ed_bottom_down_entry
_ed_bottom_down_entry WEntry _ed_bottom_down,l
		xdef	_ed_drag_top_entry
_ed_drag_top_entry WEntry _ed_drag_top,l
		xdef	_ed_drag_bottom_entry
_ed_drag_bottom_entry WEntry _ed_drag_bottom,l
		xdef	_ed_link_cursor_entry
_ed_link_cursor_entry WEntry _ed_link_cursor,l
		xdef	_ed_unlink_scroll_entry
_ed_unlink_scroll_entry WEntry _ed_unlink_scroll,l
		xdef	_ed_follow_scroll_entry
_ed_follow_scroll_entry WEntry _ed_follow_scroll,l
		xdef	_ed_split_entry
_ed_split_entry WEntry _ed_split,l
		xdef	_ed_update_split_entry
_ed_update_split_entry WEntry _ed_update_split,l
		xdef	_ed_open_shown_window_entry
_ed_open_shown_window_entry WEntry _ed_open_shown_window,l
		xdef	_ed_open_window_entry
_ed_open_window_entry WEntryF _ed_open_window
		xdef	_ed_hide_window_entry
_ed_hide_window_entry WEntry _ed_hide_window,l
		xdef	_ed_button_close_entry
_ed_button_close_entry WEntry _ed_button_close,l
		xdef	_ed_button_hide_entry
_ed_button_hide_entry WEntry _ed_button_hide,l
		xdef	_ed_button_size_entry
_ed_button_size_entry WEntry _ed_button_size,l
		xdef	_ed_close_window_quit_entry
_ed_close_window_quit_entry WEntry _ed_close_window_quit,l
		xdef	_ed_close_window_entry
_ed_close_window_entry WEntry _ed_close_window,l
		xdef	_ed_new_hidden_entry
_ed_new_hidden_entry WEntry _ed_new_hidden,l
		xdef	_ed_new_all_hidden_entry
_ed_new_all_hidden_entry WEntry _ed_new_all_hidden,l
		xdef	_ed_edit_hidden_entry
_ed_edit_hidden_entry WEntry _ed_edit_hidden,l
		xdef	_ed_get_hidden_entry
_ed_get_hidden_entry WEntry _ed_get_hidden,l
		xdef	_ed_delete_window_entry
_ed_delete_window_entry WEntry _ed_delete_window,l
		xdef	_ed_delete_windows_entry
_ed_delete_windows_entry WEntry _ed_delete_windows,l
		xdef	_ed_draw_windows_entry
_ed_draw_windows_entry WEntry _ed_draw_windows,l
		xdef	_ed_erase_windows_entry
_ed_erase_windows_entry WEntry _ed_erase_windows,l
		xdef	_ed_delete_all_zones_entry
_ed_delete_all_zones_entry WEntry _ed_delete_all_zones,l
		xdef	_ed_empty_next_entry
_ed_empty_next_entry WEntry _ed_empty_next,l
		xdef	_ed_next_window_entry
_ed_next_window_entry WEntry _ed_next_window,l
		xdef	_ed_previous_window_entry
_ed_previous_window_entry WEntry _ed_previous_window,l
		xdef	_ed_flip_size_entry
_ed_flip_size_entry WEntry _ed_flip_size,l
		xdef	_ed_window_number_entry
_ed_window_number_entry WEntryF _ed_window_number
		xdef	_ed_shrink_all_entry
_ed_shrink_all_entry WEntry _ed_shrink_all,l
		xdef	_ed_max_size_entry
_ed_max_size_entry WEntry _ed_max_size,l
		xdef	_ed_activate_entry
_ed_activate_entry WEntry _ed_activate,l
