; ---------------------------------------------------------------------
;	Start and end of AMOS.library: w/startup.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_ice_start,_w_sy_ice_end,_w_sy_start_all,_w_sy_end_all

		SECTION	.text,CODE

		xdef	_w_ice_start,_w_ice_end,_w_start_all,_w_end_all
_w_ice_start	WEntry	_w_sy_ice_start,l
_w_ice_end	WEntry	_w_sy_ice_end,l
_w_start_all	WEntry	_w_sy_start_all,l
_w_end_all	WEntry	_w_sy_end_all,l
