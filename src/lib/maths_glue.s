; ---------------------------------------------------------------------
;	Maths functions: register glue for lib/maths.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; =ABS(n), integer path of a maths function (+ILib.s Par_Math)
		LibEntry FnAbs
		move.l	d3,-(sp)
		jsr	_lib_fn_abs
		addq.l	#4,sp
		move.l	d0,d3
		Ret_Int

; =SGN(n), integer path
		LibEntry FnSgn
		move.l	d3,-(sp)
		jsr	_lib_fn_sgn
		addq.l	#4,sp
		move.l	d0,d3
		Ret_Int
