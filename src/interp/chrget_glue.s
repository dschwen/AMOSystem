; ---------------------------------------------------------------------
;	The ChrGet: interp/chrget.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; New_ChrGet: a program starts, the stack at BasSp
Ch_BasSp	equ	1060			IData offsets (core/layout.h)
Ch_AdTokens	equ	32

		LibRoutine New_ChrGet
		move.l	Ch_BasSp(a5),sp
		move.l	Ch_AdTokens(a5),a4
		CCall	_interp_new_chrget

; Prg_InsRet: back into the loop after an instruction, at a6
		xdef	_interp_iret
_interp_iret
		CCall	_interp_chrget_continue

; void interp_exec(const void *routine, uint32_t d0, uint32_t d1)
;
; An instruction: its routine called with d0 (the token), d1 (its
; entry) and the interpreter registers of the context, which get them
; back
		xdef	_interp_exec
_interp_exec
		movem.l	d2-d7/a2-a6,-(sp)
		move.l	4+44(sp),a0
		move.l	8+44(sp),d0
		move.l	12+44(sp),d1
		GetCtx	a1
		movem.l	(a1),d6-d7/a3-a6
		jsr	(a0)
		GetCtx	a1
		movem.l	d6-d7/a3-a6,(a1)
		movem.l	(sp)+,d2-d7/a2-a6
		rts

		RegRoutine SetChrPatch,_interp_set_chr_patch
