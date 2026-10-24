; ---------------------------------------------------------------------
;	Maths functions: register glue for lib/maths.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; Integer paths of the maths functions (+ILib.s Par_Math)
		LibCall	FnAbs,_lib_fn_abs,1,Int		=ABS(n)
		LibCall	FnSgn,_lib_fn_sgn,1,Int		=SGN(n)

; Float paths: the float in d3/d4 (from the parameter collector); C gets
; and gives d2-d4. FloatEntry: Lib_Def routines, FloatPar: Lib_Par ones.
FloatBody	MACRO
		lea	-12(sp),sp
		movem.l	d3-d4,4(sp)
		pea	(sp)
		CCall	\1
		addq.l	#4,sp
		movem.l	(sp)+,d2-d4
		rts
		ENDM
FloatEntry	MACRO
		LibRoutine \1
		FloatBody \2
		ENDM
FloatPar	MACRO
		LibEntry \1
		FloatBody \2
		ENDM
		FloatEntry FnSgnF,_lib_fn_sgn_f
		FloatEntry FnStrF,_lib_fn_str_f
		FloatEntry FnAbsF,_lib_fn_abs_f
		FloatEntry FnIntF,_lib_fn_int_f
		FloatPar FnTan,_lib_fn_tan
		FloatPar FnSqr,_lib_fn_sqr
		FloatPar FnLog,_lib_fn_log
		FloatPar FnLn,_lib_fn_ln
		FloatPar FnExp,_lib_fn_exp
		FloatPar FnSin,_lib_fn_sin
		FloatPar FnCos,_lib_fn_cos
		FloatPar FnASin,_lib_fn_asin
		FloatPar FnACos,_lib_fn_acos
		FloatPar FnATan,_lib_fn_atan
		FloatPar FnHSin,_lib_fn_hsin
		FloatPar FnHCos,_lib_fn_hcos
		FloatPar FnHTan,_lib_fn_htan
