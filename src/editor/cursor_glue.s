; ---------------------------------------------------------------------
;	Cursor movement, the window's lines from the program: editor/cursor.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_cursor_up,_ed_cursor_up_t,_ed_cursor_down,_ed_cursor_down_t,_ed_cursor_left
		XREF	_ed_cursor_right,_ed_page_top,_ed_page_bottom,_ed_word_left_key,_ed_word_right_key
		XREF	_ed_line_start,_ed_line_end,_ed_page_up,_ed_page_down,_ed_scroll_up_key
		XREF	_ed_scroll_down_key,_ed_scroll_left_key,_ed_scroll_right_key,_ed_text_top
		XREF	_ed_text_top_t,_ed_text_bottom,_ed_text_bottom_t,_ed_label_previous,_ed_label_next
		XREF	_ed_buf_untok_fn,_ed_untok,_ed_scroll_right,_ed_scroll_left,_ed_window_up,_ed_window_down

		SECTION	.text,CODE

		xdef	_ed_cursor_up_entry,_ed_cursor_up_t_entry,_ed_cursor_down_entry,_ed_cursor_down_t_entry
		xdef	_ed_cursor_left_entry,_ed_cursor_right_entry,_ed_page_top_entry,_ed_page_bottom_entry
		xdef	_ed_word_left_key_entry,_ed_word_right_key_entry,_ed_line_start_entry,_ed_line_end_entry
		xdef	_ed_page_up_entry,_ed_page_down_entry,_ed_scroll_up_key_entry,_ed_scroll_down_key_entry
		xdef	_ed_scroll_left_key_entry,_ed_scroll_right_key_entry,_ed_text_top_entry,_ed_text_top_t_entry
		xdef	_ed_text_bottom_entry,_ed_text_bottom_t_entry,_ed_label_previous_entry,_ed_label_next_entry
		xdef	_ed_buf_untok_entry,_ed_untok_entry
_ed_cursor_up_entry WEntry _ed_cursor_up,l
_ed_cursor_up_t_entry WEntry _ed_cursor_up_t,l
_ed_cursor_down_entry WEntry _ed_cursor_down,l
_ed_cursor_down_t_entry WEntry _ed_cursor_down_t,l
_ed_cursor_left_entry WEntry _ed_cursor_left,l
_ed_cursor_right_entry WEntry _ed_cursor_right,l
_ed_page_top_entry WEntry _ed_page_top,l
_ed_page_bottom_entry WEntry _ed_page_bottom,l
_ed_word_left_key_entry WEntry _ed_word_left_key,l
_ed_word_right_key_entry WEntry _ed_word_right_key,l
_ed_line_start_entry WEntry _ed_line_start,l
_ed_line_end_entry WEntry _ed_line_end,l
_ed_page_up_entry WEntry _ed_page_up,l
_ed_page_down_entry WEntry _ed_page_down,l
_ed_scroll_up_key_entry WEntry _ed_scroll_up_key,l
_ed_scroll_down_key_entry WEntry _ed_scroll_down_key,l
_ed_scroll_left_key_entry WEntry _ed_scroll_left_key,l
_ed_scroll_right_key_entry WEntry _ed_scroll_right_key,l
_ed_text_top_entry WEntry _ed_text_top,l
_ed_text_top_t_entry WEntry _ed_text_top_t,l
_ed_text_bottom_entry WEntry _ed_text_bottom,l
_ed_text_bottom_t_entry WEntry _ed_text_bottom_t,l
_ed_label_previous_entry WEntry _ed_label_previous,l
_ed_label_next_entry WEntry _ed_label_next,l
_ed_buf_untok_entry WEntry _ed_buf_untok_fn,l
_ed_untok_entry	WEntry	_ed_untok,l
		xdef	_ed_cgo_entry,_ed_cdr_entry,_ed_shaut2_entry,_ed_sbas2_entry
_ed_cgo_entry	WEntry	_ed_scroll_right,l
_ed_cdr_entry	WEntry	_ed_scroll_left,l
_ed_shaut2_entry WEntry	_ed_window_up,l
_ed_sbas2_entry	WEntry	_ed_window_down,l
