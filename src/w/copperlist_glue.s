; ---------------------------------------------------------------------
;	The copper list builder (EcCall CopMake, CopForce): w/copperlist.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_copper_make,_w_ec_copper_force

		SECTION	.text,CODE

		xdef	_w_copper_make,_w_copper_force
_w_copper_make	WEntry	_w_ec_copper_make,l
_w_copper_force	WEntry	_w_ec_copper_force,l
