; ---------------------------------------------------------------------
;	Single precision floats <-> text: lib/floatasc.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; AscToFloat(text): the C convention of the original (its parameter on
; the stack, the float in d0)
		CRoutine AscToFloat,_lib_asc_to_float

		RegRoutine FloatToAsc,_lib_float_to_asc
