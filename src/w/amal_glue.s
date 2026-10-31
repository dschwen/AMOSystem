; ---------------------------------------------------------------------
;	AMAL (SyCall AMALTok... and the animator): w/amal.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go,_w_amal_animate_data
		XREF	_w_sy_amal_tok,_w_sy_amal_cre,_w_sy_amal_mvo,_w_sy_amal_dall,_w_sy_amal_animate
		XREF	_w_sy_amal_reg,_w_sy_amal_clr,_w_sy_amal_frz,_w_sy_amal_ufrz,_w_sy_set_sync,_w_sy_sync
		XREF	_w_sy_play_set,_w_sy_movon,_w_sy_chan_a,_w_sy_chan_m,_w_dad_amal_c,_w_amal_init_c

		SECTION	.text,CODE

		xdef	_w_amal_tok,_w_amal_cre,_w_amal_mvo,_w_amal_dall,_w_amal_animate,_w_amal_reg
		xdef	_w_amal_clr,_w_amal_frz,_w_amal_ufrz,_w_amal_set_sync,_w_amal_sync,_w_amal_play_set
		xdef	_w_amal_movon,_w_amal_chan_a,_w_amal_chan_m
_w_amal_tok	WEntry	_w_sy_amal_tok,w
_w_amal_cre	WEntry	_w_sy_amal_cre,w
_w_amal_mvo	WEntry	_w_sy_amal_mvo,l
_w_amal_dall	WEntry	_w_sy_amal_dall,l
_w_amal_animate	WEntry	_w_sy_amal_animate,l
_w_amal_reg	WEntry	_w_sy_amal_reg,l
_w_amal_clr	WEntry	_w_sy_amal_clr,l
_w_amal_frz	WEntry	_w_sy_amal_frz,l
_w_amal_ufrz	WEntry	_w_sy_amal_ufrz,l
_w_amal_set_sync WEntry	_w_sy_set_sync,l
_w_amal_sync	WEntry	_w_sy_sync,l
_w_amal_play_set WEntry	_w_sy_play_set,l
_w_amal_movon	WEntry	_w_sy_movon,l
_w_amal_chan_a	WEntry	_w_sy_chan_a,l
_w_amal_chan_m	WEntry	_w_sy_chan_m,l

; DAdAMAL: a0 the object (its act address); AMALInit, AMALEnd
		xdef	_w_dad_amal,_w_amal_init,_w_amal_end
_w_dad_amal	WEntry	_w_dad_amal_c,l
_w_amal_init	WEntry	_w_amal_init_c,l
_w_amal_end	WEntry	_w_sy_amal_dall,l

; The animator from the VBL server (w_int_call: a5 the data zone, no
; context)
		xdef	_w_animator
_w_animator	move.l	a5,-(sp)
