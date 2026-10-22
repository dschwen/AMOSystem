; ---------------------------------------------------------------------
;	Screen management of the screen table (EcCall): w/screens.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_open,_w_ec_close,_w_ec_close_range,_w_ec_to_front,_w_ec_to_back
		XREF	_w_ec_view,_w_ec_offset,_w_ec_hide,_w_ec_clone,_w_ec_double,_w_ec_swap
		XREF	_w_ec_swap_all,_w_ec_dual,_w_ec_dual_priority,_w_ec_activate,_w_ec_current
		XREF	_w_ec_address,_w_ec_free_number,_w_ec_max_raw,_w_ec_ntsc

		SECTION	.text,CODE

		xdef	_w_sc_open,_w_sc_close,_w_sc_close_range,_w_sc_to_front,_w_sc_to_back
		xdef	_w_sc_view,_w_sc_offset,_w_sc_hide,_w_sc_clone,_w_sc_double,_w_sc_swap
		xdef	_w_sc_swap_all,_w_sc_dual,_w_sc_dual_priority,_w_sc_activate,_w_sc_current
		xdef	_w_sc_address,_w_sc_free_number,_w_sc_max_raw,_w_sc_ntsc
_w_sc_open	WEntry	_w_ec_open,l
_w_sc_close	WEntry	_w_ec_close,l
_w_sc_close_range WEntry _w_ec_close_range,l
_w_sc_to_front	WEntry	_w_ec_to_front,l
_w_sc_to_back	WEntry	_w_ec_to_back,l
_w_sc_view	WEntry	_w_ec_view,l
_w_sc_offset	WEntry	_w_ec_offset,l
_w_sc_hide	WEntry	_w_ec_hide,l
_w_sc_clone	WEntry	_w_ec_clone,l
_w_sc_double	WEntry	_w_ec_double,l
_w_sc_swap	WEntry	_w_ec_swap,l
_w_sc_swap_all	WEntry	_w_ec_swap_all,l
_w_sc_dual	WEntry	_w_ec_dual,l
_w_sc_dual_priority WEntry _w_ec_dual_priority,l
_w_sc_activate	WEntry	_w_ec_activate,l
_w_sc_current	WEntry	_w_ec_current,l
_w_sc_address	WEntry	_w_ec_address,l
_w_sc_free_number WEntry _w_ec_free_number,l
_w_sc_max_raw	WEntry	_w_ec_max_raw,l
_w_sc_ntsc	WEntry	_w_ec_ntsc,l
