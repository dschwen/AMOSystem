; ---------------------------------------------------------------------
;	Double precision floats <-> text: lib/doubleasc.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; The C convention of the original: the parameters on the stack, the
; result in d0:d1 (DoubleToAsc(hi, lo, buffer, digits, mode))
		CRoutine DoubleToAsc,_lib_double_to_asc
		CRoutine AscToDouble,_lib_asc_to_double
