; ---------------------------------------------------------------------
;	Calls from C++ into the assembler (see core/amos.h)
; ---------------------------------------------------------------------
;
; The interpreter registers d6-d7/a3-a6 at the last entry into C++ are
; kept per task: the file selector runs Interface code in a second task,
; at the same time as the main one. The contexts are a block: the main
; task (set by +B.s), its context, the context of any other task.
; amos_bridge points to the block: AMOSPro's own, which it passes in
; T_CBridge to the programs it loads (AMOS.library...), so that all share
; it.

		Include	"glue.i"

		SECTION	.text,CODE

; void amos_call(Regs *r, const void *fn)
;
; Calls fn with d0-d7/a0-a2 from *r and the interpreter registers a3-a6
; from the context, then stores d0-d7/a0-a2 back into *r. The context is
; restored afterwards: the callee may leave other values in d6-d7/a3-a6
; (an extension's bank routine loads its own base into a3), and C++ it
; calls in turn saves its registers there.
		xdef	_amos_call
_amos_call
		movem.l	d2-d7/a2-a6,-(sp)
		GetCtx	a0
		movem.l	(a0),d2-d7		keep the context
		movem.l	d2-d7,-(sp)
		move.l	4+44+24(sp),a0		r
		move.l	8+44+24(sp),a1		fn
		move.l	a0,-(sp)
		pea	.Back(pc)
		move.l	a1,-(sp)
		GetCtx	a1
		movem.l	8(a1),a3-a6
		movem.l	(a0),d0-d7/a0-a2
		rts				to fn, which returns to .Back
.Back		move.l	a0,-(sp)
		move.l	4(sp),a0
		movem.l	d0-d7,(a0)
		move.l	(sp)+,8*4(a0)
		movem.l	a1-a2,9*4(a0)
		addq.l	#4,sp
		movem.l	(sp)+,d2-d7
		GetCtx	a0
		movem.l	d2-d7,(a0)
		movem.l	(sp)+,d2-d7/a2-a6
		rts

; int32_t amos_call_cc(Regs *r, const void *fn)
;
; amos_call for a routine that answers in the condition codes (a float
; comparison): returns -1 if they say "less than" (N xor V), 0 "equal"
; (Z), else 1.
		xdef	_amos_call_cc
_amos_call_cc
		movem.l	d2-d7/a2-a6,-(sp)
		GetCtx	a0
		movem.l	(a0),d2-d7		keep the context
		movem.l	d2-d7,-(sp)
		move.l	4+44+24(sp),a0		r
		move.l	8+44+24(sp),a1		fn
		move.l	a0,-(sp)
		pea	.Back(pc)
		move.l	a1,-(sp)
		GetCtx	a1
		movem.l	8(a1),a3-a6
		movem.l	(a0),d0-d7/a0-a2
		rts				to fn, which returns to .Back
.Back		beq.s	.Eq
		blt.s	.Lt
		pea	1.w
		bra.s	.Store
.Lt		pea	-1.w
		bra.s	.Store
.Eq		clr.l	-(sp)
.Store		move.l	a0,-(sp)
		move.l	8(sp),a0
		movem.l	d0-d7,(a0)
		move.l	(sp)+,8*4(a0)
		movem.l	a1-a2,9*4(a0)
		move.l	(sp)+,d0
		addq.l	#4,sp
		movem.l	(sp)+,d2-d7
		GetCtx	a0
		movem.l	d2-d7,(a0)
		movem.l	(sp)+,d2-d7/a2-a6
		rts

; void amos_eval(Regs *r, const void *fn)
;
; amos_call for a routine that reads parameters from the program: a3 and
; a6 keep the values it leaves (the parameter stack and the program
; pointer past the parameters), the rest of the context is restored.
		xdef	_amos_eval
_amos_eval
		movem.l	d2-d7/a2-a6,-(sp)
		GetCtx	a0
		movem.l	(a0),d2-d7
		movem.l	d2-d7,-(sp)
		move.l	4+44+24(sp),a0		r
		move.l	8+44+24(sp),a1		fn
		move.l	a0,-(sp)
		pea	.Back(pc)
		move.l	a1,-(sp)
		GetCtx	a1
		movem.l	8(a1),a3-a6
		movem.l	(a0),d0-d7/a0-a2
		rts				to fn, which returns to .Back
.Back		move.l	a0,-(sp)
		move.l	4(sp),a0
		movem.l	d0-d7,(a0)
		move.l	(sp)+,8*4(a0)
		movem.l	a1-a2,9*4(a0)
		addq.l	#4,sp
		movem.l	(sp)+,d2-d7
		GetCtx	a0
		movem.l	d2-d7,(a0)
		move.l	a3,8(a0)
		move.l	a6,20(a0)
		movem.l	(sp)+,d2-d7/a2-a6
		rts

; void amos_call_all(AllRegs *r, const void *fn)
;
; Calls fn with all of d0-d7/a0-a6 from *r (dialog callbacks get the
; dialog's a3, a4 and a6), stores them back. The context is restored.
		xdef	_amos_call_all
_amos_call_all
		movem.l	d2-d7/a2-a6,-(sp)
		GetCtx	a0
		movem.l	(a0),d2-d7
		movem.l	d2-d7,-(sp)
		move.l	4+44+24(sp),a0		r
		move.l	8+44+24(sp),a1		fn
		move.l	a0,-(sp)
		pea	.Back(pc)
		move.l	a1,-(sp)
		movem.l	(a0),d0-d7/a0-a6
		rts				to fn, which returns to .Back
.Back		move.l	a0,-(sp)
		move.l	4(sp),a0
		movem.l	d0-d7,(a0)
		move.l	(sp)+,8*4(a0)
		movem.l	a1-a6,9*4(a0)
		addq.l	#4,sp
		movem.l	(sp)+,d2-d7
		GetCtx	a0
		movem.l	d2-d7,(a0)
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
		GetCtx	a1
		movem.l	(a1),d6-d7/a3-a6
		jmp	(a0)

		SECTION	.bss,BSS

; The block: the main task, then the contexts (d6, d7, a3-a6) of the main
; task and of the other one
		xdef	_amos_task
_amos_task	ds.l	1
		ds.l	6
		ds.l	6

		SECTION	.data,DATA
		xdef	_amos_bridge
_amos_bridge	dc.l	_amos_task
