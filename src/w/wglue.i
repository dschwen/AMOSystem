; ---------------------------------------------------------------------
;	Glue for AMOS.library functions implemented in C++ (src/w)
; ---------------------------------------------------------------------
;
; WEntry cfunction,size: the whole entry of a function. It hands all the
; registers to cfunction(AllRegs *), which changes those the function
; returns, and sets the condition codes with tst.size d0. See _w_go
; (w/call_w.s) for the context.

WEntry		MACRO
		movem.l	d0-d7/a0-a6,-(sp)
		lea	\1,a2
		jsr	_w_go
		movem.l	(sp)+,d0-d7/a0-a6
		tst.\2	d0
		rts
		ENDM
