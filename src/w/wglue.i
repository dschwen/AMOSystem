; ---------------------------------------------------------------------
;	Glue for AMOS.library functions implemented in C++ (src/w)
; ---------------------------------------------------------------------
;
; WEntry cfunction,size: the whole entry of a function. It hands all the
; registers to cfunction(AllRegs *), which changes those the function
; returns, and sets the condition codes with tst.size d0. See _w_go
; (core/call_go.s) for the context.

WEntry		MACRO
		movem.l	d0-d7/a0-a6,-(sp)
		lea	\1,a2
		jsr	_w_go
		movem.l	(sp)+,d0-d7/a0-a6
		tst.\2	d0
		rts
		ENDM

; WEntryA0 cfunction: the same for a function returning an address in a0,
; the condition codes from it
WEntryA0	MACRO
		movem.l	d0-d7/a0-a6,-(sp)
		lea	\1,a2
		jsr	_w_go
		movem.l	(sp)+,d0-d7/a0-a6
		cmp.w	#0,a0
		rts
		ENDM
