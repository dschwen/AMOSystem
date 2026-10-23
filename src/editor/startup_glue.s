; ---------------------------------------------------------------------
;	The editor's start and end, its screens: editor/startup.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_cold
		XREF	_ed_title
		XREF	_ed_end
		XREF	_ed_open_editor_fn
		XREF	_ed_kill_editor
		XREF	_ed_close_editor_fn
		XREF	_ed_draw_top_fn
		XREF	_ed_tokens_init
		XREF	_ed_tokens_end

		SECTION	.text,CODE

		xdef	_ed_cold_entry
_ed_cold_entry WEntry _ed_cold,l
		xdef	_ed_title_entry
_ed_title_entry WEntry _ed_title,l
		xdef	_ed_end_entry
_ed_end_entry WEntry _ed_end,l
		xdef	_ed_open_editor_entry
_ed_open_editor_entry WEntry _ed_open_editor_fn,l
		xdef	_ed_kill_editor_entry
_ed_kill_editor_entry WEntry _ed_kill_editor,l
		xdef	_ed_close_editor_entry
_ed_close_editor_entry WEntry _ed_close_editor_fn,l
		xdef	_ed_draw_top_fn_entry
_ed_draw_top_fn_entry WEntry _ed_draw_top_fn,l
		xdef	_ed_tokens_init_entry
_ed_tokens_init_entry WEntry _ed_tokens_init,l
		xdef	_ed_tokens_end_entry
_ed_tokens_end_entry WEntry _ed_tokens_end,l
