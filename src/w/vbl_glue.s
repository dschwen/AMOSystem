; ---------------------------------------------------------------------
;	The VBL interrupt: w/vbl.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_add_vbl,_w_sy_rem_vbl,_w_sy_wait_vbl

		SECTION	.text,CODE

; Restart VBL, Stop VBL, Wait Vbl (SyCall)
		xdef	_w_add_vbl,_w_rem_vbl,_w_wvbl
_w_add_vbl	WEntry	_w_sy_add_vbl,l
_w_rem_vbl	WEntry	_w_sy_rem_vbl,l
_w_wvbl		WEntry	_w_sy_wait_vbl,l

; void w_int_call(const void *fn, uint8_t *data): fn with a5 = data and
; a6 = the custom chips, as the assembler of the VBL called its routines
		xdef	_w_int_call
_w_int_call	movem.l	d2-d7/a2-a6,-(sp)
