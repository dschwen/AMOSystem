; ---------------------------------------------------------------------
;	Calls from C++ into the assembler (see core/amos.h)
; ---------------------------------------------------------------------
;
; The interpreter registers d6-d7/a3-a6 at the last entry into C++ are
; kept per task: the file selector runs Interface code in a second task,
; at the same time as the main one. The main task (amos_task, set by +B.s)
; uses amos_ctx, any other amos_ctx2.

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

; The contexts, d6, d7, a3-a6: the main task's and the other one's
		xdef	_amos_ctx
		xdef	_amos_ctx2
		xdef	_amos_task
_amos_ctx	ds.l	6
_amos_ctx2	ds.l	6
_amos_task	ds.l	1
