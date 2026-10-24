; ---------------------------------------------------------------------
;	The float layer: interp/float.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"

		SECTION	.text,CODE

; int32_t interp_lvo(uint32_t *r, uint32_t base, int32_t offset)
;
; The math library function at base + offset with d0-d3 from r[0..3];
; d0/d1 back into r[0..1]. Returns its condition codes: -1 less than,
; 0 equal, 1 greater.
		xdef	_interp_lvo
_interp_lvo
		movem.l	d2-d3/a6,-(sp)
		move.l	16(sp),a0
		move.l	20(sp),a6
		move.l	24(sp),d0
		lea	0(a6,d0.l),a1
		move.l	a0,-(sp)
		movem.l	(a0),d0-d3
		jsr	(a1)
		beq.s	.Eq
		blt.s	.Lt
		moveq	#1,d2
		bra.s	.Out
.Lt		moveq	#-1,d2
		bra.s	.Out
.Eq		moveq	#0,d2
.Out		move.l	(sp)+,a0
		movem.l	d0-d1,(a0)
		move.l	d2,d0
		movem.l	(sp)+,d2-d3/a6
		rts

		RegRoutine IntToFl1,_interp_int_to_fl1
		RegRoutine DIntToFl1,_interp_dint_to_fl1
		RegRoutine IntToFl2,_interp_int_to_fl2
		RegRoutine DIntToFl2,_interp_dint_to_fl2
		RegRoutine FlToInt1,_interp_fl_to_int1
		RegRoutine DFlToInt1,_interp_dfl_to_int1
		RegRoutine FlToInt2,_interp_fl_to_int2
		RegRoutine DFlToInt2,_interp_dfl_to_int2
		RegRoutine Math_Fonction,_interp_math_fonction
		RegRoutine DMath_Fonction,_interp_dmath_fonction
		RegRoutine Float_Compare,_interp_float_compare
		RegRoutine DFloat_Compare,_interp_dfloat_compare
		RegRoutine Float_Operation,_interp_float_operation
		RegRoutine DFloat_Operation,_interp_dfloat_operation
		RegRoutine Float_Test,_interp_float_test
		RegRoutine Float_TestF,_interp_float_testf
		RegRoutine Math_Operation,_interp_math_operation
		RegRoutine DMath_Operation,_interp_dmath_operation
		RegRoutine Float_Fonction,_interp_float_fonction
		RegRoutine DFloat_Fonction,_interp_dfloat_fonction
		RegRoutine FlPos,_interp_fl_pos
		RegRoutine FlPosD,_interp_fl_pos_d
		RegRoutine AAngle,_interp_aangle
		RegRoutine AAngleD,_interp_aangle_d
		RegRoutine FFAngle,_interp_ffangle
		RegRoutine FAngleD,_interp_fangle_d
		RegRoutine FnParamF,_interp_fn_param_f
		RegRoutine FnParamD,_interp_fn_param_d
