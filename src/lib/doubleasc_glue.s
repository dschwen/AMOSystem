; ---------------------------------------------------------------------
;	Double precision floats <-> text: lib/doubleasc.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; The C convention of the original: the parameters on the stack, the
; result in d0:d1. DoubleToAsc(hi, lo, buffer, digits, mode) keeps all
; the registers but a0.
		XREF	_lib_double_to_asc
		LibRoutine DoubleToAsc
		movem.l	d0-d1/a1,-(sp)
		REPT	5
		move.l	32(sp),-(sp)
		ENDR
		CCall	_lib_double_to_asc
		lea	20(sp),sp
		movem.l	(sp)+,d0-d1/a1
		rts

		XREF	_lib_asc_to_double
		LibRoutine AscToDouble
		move.l	4(sp),-(sp)
		CCall	_lib_asc_to_double
		addq.l	#4,sp
		rts
