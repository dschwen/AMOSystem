; ---------------------------------------------------------------------
;	Direct mode: editor/direct.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_escape
		XREF	_ed_escape_loop
		XREF	_ed_escape_leave
		XREF	_ed_escape_forget
		XREF	_ed_escape_forget_all
		XREF	_ed_error_direct
		XREF	_ed_escape_appear
		XREF	_ed_escape_max_mouse
		XREF	_ed_escape_hide
		XREF	_ed_escape_display
		XREF	_ed_escape_display_d0
		XREF	_ed_get_error

		SECTION	.text,CODE

		xdef	_ed_escape_entry
_ed_escape_entry WEntry _ed_escape,l
		xdef	_ed_escape_loop_entry
_ed_escape_loop_entry WEntry _ed_escape_loop,l
		xdef	_ed_escape_leave_entry
_ed_escape_leave_entry WEntry _ed_escape_leave,l
		xdef	_ed_escape_forget_entry
_ed_escape_forget_entry WEntry _ed_escape_forget,l
		xdef	_ed_escape_forget_all_entry
_ed_escape_forget_all_entry WEntry _ed_escape_forget_all,l
		xdef	_ed_error_direct_entry
_ed_error_direct_entry WEntry _ed_error_direct,l
		xdef	_ed_escape_appear_entry
_ed_escape_appear_entry WEntry _ed_escape_appear,l
		xdef	_ed_escape_max_mouse_entry
_ed_escape_max_mouse_entry WEntry _ed_escape_max_mouse,l
		xdef	_ed_escape_hide_entry
_ed_escape_hide_entry WEntry _ed_escape_hide,l
		xdef	_ed_escape_display_entry
_ed_escape_display_entry WEntry _ed_escape_display,l
		xdef	_ed_escape_display_d0_entry
_ed_escape_display_d0_entry WEntry _ed_escape_display_d0,l
		xdef	_ed_get_error_entry
_ed_get_error_entry WEntry _ed_get_error,l
