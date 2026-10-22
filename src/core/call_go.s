; ---------------------------------------------------------------------
;	Entry of assembler-called functions into C++ (see w/wglue.i): used by
;	AMOS.library and the editor
; ---------------------------------------------------------------------
;
; The registers d6-d7/a3-a6 of the caller become the context of the task
; (see src/glue.i) for the C++ code and its calls back into AMOS.library
; (a5: the data zone). The context is put back afterwards: the caller may
; be C++ (amos_call) or a task that is not the interpreter's.

		Include	"glue.i"

		SECTION	.text,CODE

; a2 = the C function, the registers at 4(sp)
		xdef	_w_go
_w_go		lea	4(sp),a1
		GetCtx	a0
		movem.l	(a0),d1-d6		keep the context
		movem.l	d1-d6,-(sp)
		move.l	24(a1),(a0)+		the caller's d6-d7/a3-a6
		move.l	28(a1),(a0)+
		move.l	44(a1),(a0)+
		move.l	48(a1),(a0)+
		move.l	52(a1),(a0)+
		move.l	56(a1),(a0)
		move.l	a1,-(sp)
		jsr	(a2)
		addq.l	#4,sp
		GetCtx	a0
		movem.l	(sp)+,d1-d6
		movem.l	d1-d6,(a0)
		rts
