; ---------------------------------------------------------------------
;	The mouse in the editor: editor/mouse.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_mouse

		SECTION	.text,CODE

		xdef	_ed_mouse_entry
_ed_mouse_entry	WEntry	_ed_mouse,l
