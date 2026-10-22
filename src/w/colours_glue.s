; ---------------------------------------------------------------------
;	Colours of the screen table (EcCall): w/colours.cpp
; ---------------------------------------------------------------------
;
; +W.s jumps here from the routines of its table (EcSCol...).

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_set_colour,_w_ec_get_colour,_w_ec_set_palette,_w_ec_colour_back
		XREF	_w_ec_flash_off,_w_ec_flash,_w_ec_shift,_w_ec_shift_off
		XREF	_w_ec_fade,_w_ec_fade_off,_w_ec_fade_stop

		SECTION	.text,CODE

		xdef	_w_set_colour,_w_get_colour,_w_set_palette,_w_colour_back
		xdef	_w_flash_off,_w_flash,_w_shift,_w_shift_off
		xdef	_w_fade,_w_fade_off,_w_fade_stop
_w_set_colour	WEntry	_w_ec_set_colour,l
_w_get_colour	WEntry	_w_ec_get_colour,l
_w_set_palette	WEntry	_w_ec_set_palette,l
_w_colour_back	WEntry	_w_ec_colour_back,l
_w_flash_off	WEntry	_w_ec_flash_off,l
_w_flash	WEntry	_w_ec_flash,w
_w_shift	WEntry	_w_ec_shift,l
_w_shift_off	WEntry	_w_ec_shift_off,l
_w_fade		WEntry	_w_ec_fade,l
_w_fade_off	WEntry	_w_ec_fade_off,l
_w_fade_stop	WEntry	_w_ec_fade_stop,l
