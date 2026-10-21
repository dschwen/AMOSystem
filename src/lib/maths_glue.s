; ---------------------------------------------------------------------
;	Maths functions: register glue for lib/maths.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; Integer paths of the maths functions (+ILib.s Par_Math)
		LibCall	FnAbs,_lib_fn_abs,1,Int		=ABS(n)
		LibCall	FnSgn,_lib_fn_sgn,1,Int		=SGN(n)
