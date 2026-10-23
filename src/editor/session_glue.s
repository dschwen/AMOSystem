; ---------------------------------------------------------------------
;	Quit, the session: editor/session.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_quit,_ed_do_quit_fn,_ed_warm_start

		SECTION	.text,CODE

		xdef	_ed_quit_entry
_ed_quit_entry WEntry _ed_quit,l
		xdef	_ed_do_quit_entry
_ed_do_quit_entry WEntry _ed_do_quit_fn,l
		xdef	_ed_warm_start_entry
_ed_warm_start_entry WEntry _ed_warm_start,l
