; ---------------------------------------------------------------------
;	User copper list of the screen table (EcCall): w/copper.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_cop_swap,_w_ec_cop_swap_lists,_w_ec_cop_reset,_w_ec_cop_wait
		XREF	_w_ec_cop_move,_w_ec_cop_movel,_w_ec_cop_base
		XREF	_w_ec_copper_on_off

		SECTION	.text,CODE

		xdef	_w_cop_swap,_w_cop_swap_lists,_w_cop_reset,_w_cop_wait
		xdef	_w_cop_move,_w_cop_movel,_w_cop_base
_w_cop_swap	WEntry	_w_ec_cop_swap,l
_w_cop_swap_lists WEntry _w_ec_cop_swap_lists,l
_w_cop_reset	WEntry	_w_ec_cop_reset,l
_w_cop_wait	WEntry	_w_ec_cop_wait,l
_w_cop_move	WEntry	_w_ec_cop_move,l
_w_cop_movel	WEntry	_w_ec_cop_movel,l
_w_cop_base	WEntry	_w_ec_cop_base,l
		xdef	_w_copper_on_off
_w_copper_on_off WEntry	_w_ec_copper_on_off,l
