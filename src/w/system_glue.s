; ---------------------------------------------------------------------
;	AMOS / Workbench, joysticks, requesters (SyCall): w/system.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_amos_wb,_w_sy_joy,_w_sy_request_on_off

		SECTION	.text,CODE

		xdef	_w_tamoswb,_w_joy,_w_request_on_off
_w_tamoswb	WEntry	_w_sy_amos_wb,l
_w_joy		WEntry	_w_sy_joy,l
_w_request_on_off WEntry _w_sy_request_on_off,l
