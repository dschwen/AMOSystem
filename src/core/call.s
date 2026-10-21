; ---------------------------------------------------------------------
;	Calls from C++ into the assembler (see core/amos.h)
; ---------------------------------------------------------------------

		SECTION	.text,CODE

; void amos_call(Regs *r, const void *fn)
;
; Calls fn with d0-d5/a0-a2 from *r and the interpreter registers
; d6-d7/a3-a6 from amos_ctx, then stores the registers back: d0-d5/a0-a2
; into *r, d6-d7/a3-a6 into amos_ctx.
		xdef	_amos_call
_amos_call
		movem.l	d2-d7/a2-a6,-(sp)
		move.l	4+44(sp),a0		r
		move.l	8+44(sp),a1		fn
		move.l	a0,-(sp)
		pea	.Back(pc)
		move.l	a1,-(sp)
		movem.l	_amos_ctx,d6-d7/a3-a6
		movem.l	(a0),d0-d5/a0-a2
		rts				to fn, which returns to .Back
.Back		movem.l	d6-d7/a3-a6,_amos_ctx
		move.l	a0,-(sp)
		move.l	4(sp),a0
		movem.l	d0-d5,(a0)
		move.l	(sp)+,6*4(a0)
		movem.l	a1-a2,7*4(a0)
		addq.l	#4,sp
		movem.l	(sp)+,d2-d7/a2-a6
		rts

; void amos_goto(const void *fn, uint32_t d0)
;
; Jumps to fn with d0 and the interpreter registers. Never returns: used to
; raise errors, which reset the stack to the interpreter's.
		xdef	_amos_goto
_amos_goto
		move.l	4(sp),a0
		move.l	8(sp),d0
		movem.l	_amos_ctx,d6-d7/a3-a6
		jmp	(a0)

		SECTION	.bss,BSS

; The interpreter registers at the last entry into C++: d6, d7, a3-a6
		xdef	_amos_ctx
_amos_ctx	ds.l	6
