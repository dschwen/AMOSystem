; ---------------------------------------------------------------------
;	Autoback (EcCall AutoBack1-4): w/autoback.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_autoback1,_w_ec_autoback2,_w_ec_autoback3,_w_ec_autoback4

		SECTION	.text,CODE

		xdef	_w_tabk1,_w_tabk2,_w_tabk3,_w_tabk4
_w_tabk1	WEntry	_w_ec_autoback1,l
_w_tabk2	WEntry	_w_ec_autoback2,l
_w_tabk3	WEntry	_w_ec_autoback3,l
_w_tabk4	WEntry	_w_ec_autoback4,l
