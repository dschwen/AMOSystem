; ---------------------------------------------------------------------
;	Rainbows of the screen table (EcCall): w/rainbow.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_rain_set,_w_ec_rain_do,_w_ec_rain_var,_w_ec_rain_del,_w_ec_rain_hide

		SECTION	.text,CODE

		xdef	_w_rain_set,_w_rain_do,_w_rain_var,_w_rain_del,_w_rain_hide
_w_rain_set	WEntry	_w_ec_rain_set,l
_w_rain_do	WEntry	_w_ec_rain_do,l
_w_rain_var	WEntry	_w_ec_rain_var,l
_w_rain_del	WEntry	_w_ec_rain_del,l
_w_rain_hide	WEntry	_w_ec_rain_hide,l
