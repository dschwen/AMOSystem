; ---------------------------------------------------------------------
;	Drawing functions of the screen table (EcCall): w/drawing.cpp
; ---------------------------------------------------------------------
;
; +W.s jumps here from the routines of its table (EcCls...).

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_ec_cls,_w_ec_set_clip_fn,_w_ec_pattern,_w_ec_paint
		XREF	_w_ec_slider_h,_w_ec_slider_v,_w_ec_slider_set,_w_ec_slider_knob,_w_ec_screen_copy

		SECTION	.text,CODE

		xdef	_w_cls,_w_set_clip,_w_pattern,_w_paint
		xdef	_w_slider_h,_w_slider_v,_w_slider_set,_w_slider_knob,_w_screen_copy
_w_cls		WEntry	_w_ec_cls,l
_w_set_clip	WEntry	_w_ec_set_clip_fn,l
_w_pattern	WEntry	_w_ec_pattern,l
_w_paint	WEntry	_w_ec_paint,l
_w_slider_h	WEntry	_w_ec_slider_h,w
_w_slider_v	WEntry	_w_ec_slider_v,w
_w_slider_set	WEntry	_w_ec_slider_set,l
_w_slider_knob	WEntry	_w_ec_slider_knob,l
_w_screen_copy	WEntry	_w_ec_screen_copy,l
