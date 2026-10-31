; ---------------------------------------------------------------------
;	The editor's screens: editor/screen.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_memory_draw
		XREF	_ed_memory_draw_title
		XREF	_ed_memory_show
		XREF	_ed_unpack_fn
		XREF	_ed_unpack_title
		XREF	_ed_hide_fn
		XREF	_ed_appear_fn
		XREF	_ed_redraw
		XREF	_ed_limit_mouse

		SECTION	.text,CODE

		xdef	_ed_memory_draw_entry
_ed_memory_draw_entry WEntry _ed_memory_draw,l
		xdef	_ed_memory_draw_title_entry
_ed_memory_draw_title_entry WEntry _ed_memory_draw_title,l
		xdef	_ed_memory_show_entry
_ed_memory_show_entry WEntry _ed_memory_show,l
		xdef	_ed_unpack_entry
_ed_unpack_entry WEntry _ed_unpack_fn,l
		xdef	_ed_unpack_title_entry
_ed_unpack_title_entry WEntry _ed_unpack_title,l
		xdef	_ed_hide_entry
_ed_hide_entry WEntry _ed_hide_fn,l
		xdef	_ed_appear_entry
_ed_appear_entry WEntry _ed_appear_fn,l
		xdef	_ed_redraw_entry
_ed_redraw_entry WEntry _ed_redraw,l
		xdef	_ed_limit_mouse_entry
_ed_limit_mouse_entry WEntry _ed_limit_mouse,l

; EdReCop
		XREF	_ed_recop_fn
		xdef	_ed_recop
_ed_recop WEntry _ed_recop_fn,l
