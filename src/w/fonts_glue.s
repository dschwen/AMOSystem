; ---------------------------------------------------------------------
;	Fonts and menu state of the screen table (EcCall): w/fonts.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_get_fonts,_w_ec_free_fonts,_w_ec_get_font,_w_ec_set_font
		XREF	_w_ec_close_font,_w_ec_menu_start,_w_ec_menu_stop

		SECTION	.text,CODE

		xdef	_w_get_fonts,_w_free_fonts,_w_get_font,_w_set_font
		xdef	_w_close_font,_w_menu_start,_w_menu_stop
_w_get_fonts	WEntry	_w_ec_get_fonts,w
_w_free_fonts	WEntry	_w_ec_free_fonts,l
_w_get_font	WEntry	_w_ec_get_font,l
_w_set_font	WEntry	_w_ec_set_font,l
_w_close_font	WEntry	_w_ec_close_font,l
_w_menu_start	WEntry	_w_ec_menu_start,l
_w_menu_stop	WEntry	_w_ec_menu_stop,l
