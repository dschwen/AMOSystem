; ---------------------------------------------------------------------
;	The VBL interrupt: w/vbl.cpp
; ---------------------------------------------------------------------

		SECTION	.text,CODE

; void w_int_call(const void *fn, uint8_t *data): fn with a5 = data and
; a6 = the custom chips, as the assembler of the VBL called its routines
		xdef	_w_int_call
_w_int_call	movem.l	d2-d7/a2-a6,-(sp)
		move.l	4+44(sp),a0
		move.l	8+44(sp),a5
		lea	$DFF000,a6
		jsr	(a0)
		movem.l	(sp)+,d2-d7/a2-a6
		rts
