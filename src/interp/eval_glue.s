; ---------------------------------------------------------------------
;	Expressions and variables: interp/eval.cpp, interp/vars.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"

		SECTION	.text,CODE

; EvalCall cfunction: cfunction(EvalOut *) gives d0-d4/a0
EvalCall	MACRO
		lea	-24(sp),sp
		pea	(sp)
		CCall	\1
		addq.l	#4,sp
		movem.l	(sp)+,d0-d4/a0
		rts
		ENDM

; New_Evalue: the expression at a6. Out: d2-d4 the value, d0 the token
; after it (a6 there, or past it if it is a closing parenthesis).
		LibRoutine New_Evalue
		EvalCall _interp_new_evalue

; =Max(a,b), =Min(a,b)
		LibEntry FnMax
		EvalCall _interp_fn_max
		LibEntry FnMin
		EvalCall _interp_fn_min

; Var = expression; Var as an operand
		LibEntry InVar
		CCall	_interp_in_var
		rts
		LibEntry FnVar
		EvalCall _interp_fn_var

; FindVar: the variable after the token at a6. Out: a0 its address, d2
; its type (d0, d1 changed).
		LibRoutine FindVar
		movem.l	d3-d4/a1,-(sp)
		lea	-24(sp),sp
		pea	(sp)
		CCall	_interp_find_var
		addq.l	#4,sp
		movem.l	(sp)+,d0-d2
		addq.l	#8,sp
		move.l	(sp)+,a0
		movem.l	(sp)+,d3-d4/a1
		rts

; GetTablo: a0 an array's slot. Out: a0 the element (d0 changed).
		LibRoutine GetTablo
		movem.l	d1/a1,-(sp)
		lea	-24(sp),sp
		move.l	a0,20(sp)
		pea	(sp)
		CCall	_interp_get_tablo
		addq.l	#4,sp
		move.l	20(sp),a0
		lea	24(sp),sp
		movem.l	(sp)+,d1/a1
		rts

; Instructions: InCall Name,cfunction
InCall		MACRO
		LibEntry \1
		CCall	\2
		rts
		ENDM
; Functions: FnCall Name,cfunction (cfunction(EvalOut *))
FnCall		MACRO
		LibEntry \1
		EvalCall \2
		ENDM

		InCall	InDim,_interp_in_dim
		InCall	InSwap,_interp_in_swap
		InCall	InInc,_interp_in_inc
		InCall	InDec,_interp_in_dec
		InCall	InAdd2,_interp_in_add2
		InCall	InAdd4,_interp_in_add4
		InCall	InDFn,_interp_in_dfn
		InCall	InShared,_interp_in_shared
		FnCall	FnVarPtr,_interp_fn_varptr
		FnCall	FnArray,_interp_fn_array
		FnCall	FnFn,_interp_fn_fn
		FnCall	FnNot,_interp_fn_not
		FnCall	FnCEntier,_interp_fn_c_entier
		FnCall	FnCstFl,_interp_fn_cst_fl
		FnCall	FnCstDFl,_interp_fn_cst_dfl
		FnCall	FnCstCh,_interp_fn_cst_ch

; Bits: interp/bits.cpp
		InCall	InBset,_interp_in_bset
		InCall	InBclr,_interp_in_bclr
		InCall	InBchg,_interp_in_bchg
		FnCall	FnBtst,_interp_fn_btst
		InCall	InRorB,_interp_in_ror_b
		InCall	InRorW,_interp_in_ror_w
		InCall	InRorL,_interp_in_ror_l
		InCall	InRolB,_interp_in_rol_b
		InCall	InRolW,_interp_in_rol_w
		InCall	InRolL,_interp_in_rol_l

; Sort, Match: interp/sort.cpp; Data, Read: interp/data.cpp
		InCall	InSort,_interp_in_sort
		FnCall	FnMatch,_interp_fn_match
		InCall	InData,_interp_in_data
		InCall	InRead,_interp_in_read

; Functions with their parameter in d3 (collected): FnCallD3 Name,cfunction
FnCallD3	MACRO
		LibEntry \1
		lea	-24(sp),sp
		move.l	d3,12(sp)
		pea	(sp)
		CCall	\2
		addq.l	#4,sp
		movem.l	(sp)+,d0-d4/a0
		rts
		ENDM

; Strings: interp/text.cpp
		InCall	InLeft,_interp_in_left
		FnCall	FnLeft,_interp_fn_left
		InCall	InRight,_interp_in_right
		FnCall	FnRight,_interp_fn_right
		InCall	InMid2,_interp_in_mid2
		FnCall	FnMid2,_interp_fn_mid2
		InCall	InMid3,_interp_in_mid3
		FnCall	FnMid3,_interp_fn_mid3
		FnCallD3 FnVal,_interp_fn_val
		FnCallD3 FnResource,_interp_fn_resource

; Print: interp/print.cpp; Input: interp/input.cpp
		InCall	InPrint,_interp_in_print
		InCall	InLPrint,_interp_in_lprint
		InCall	InPrintH,_interp_in_print_h
		InCall	InInput,_interp_in_input
		InCall	InLineInput,_interp_in_line_input
		InCall	InInputH,_interp_in_input_h
		InCall	InLineInputH,_interp_in_line_input_h

; Graphics: interp/graphics.cpp
		InCall	InDefaultPalette,_interp_in_default_palette
		InCall	InPalette,_interp_in_palette
		InCall	InFade,_interp_in_fade
		InCall	InPolyline,_interp_in_polyline
		InCall	InPolygon,_interp_in_polygon
		InCall	InChannel,_interp_in_channel

; Call, Equ, Struc: interp/system.cpp
		InCall	InCall,_interp_in_call
		FnCall	FnEqu,_interp_fn_equ
		InCall	InStruc,_interp_in_struc
		FnCall	FnStruc,_interp_fn_struc
		InCall	InStrucD,_interp_in_struc_d
		FnCall	FnStrucD,_interp_fn_struc_d

; Menus: interp/menus.cpp
		InCall	InMenu,_interp_in_menu
		InCall	InMenuKey,_interp_in_menu_key
		InCall	InMenuDel,_interp_in_menu_del
		InCall	InSetMenu,_interp_in_set_menu

; The extensions' instructions and functions: interp/extensions.cpp
		InCall	InExtCall,_interp_in_ext_call
		LibRoutine FnExtCall
		EvalCall _interp_fn_ext_call
		InCall	InField,_interp_in_field
