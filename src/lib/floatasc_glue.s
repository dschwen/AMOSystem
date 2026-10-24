; ---------------------------------------------------------------------
;	Single precision floats <-> text: lib/floatasc.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		RegRoutine FloatToAsc,_lib_float_to_asc

; AscToFloat(text): the C convention of the original (its parameter on
; the stack, the float in d0)
		XREF	_lib_asc_to_float
		LibRoutine AscToFloat
		move.l	4(sp),-(sp)
		CCall	_lib_asc_to_float
		addq.l	#4,sp
		rts
