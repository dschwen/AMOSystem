; ---------------------------------------------------------------------
;	Information dialogues, quit options: editor/info.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_quit_options
		XREF	_ed_about
		XREF	_ed_about_extensions
		XREF	_ed_information

		SECTION	.text,CODE

		xdef	_ed_quit_options_entry
_ed_quit_options_entry WEntry _ed_quit_options,l
		xdef	_ed_about_entry
_ed_about_entry WEntry _ed_about,l
		xdef	_ed_about_extensions_entry
_ed_about_extensions_entry WEntry _ed_about_extensions,l
		xdef	_ed_information_entry
_ed_information_entry WEntry _ed_information,l
