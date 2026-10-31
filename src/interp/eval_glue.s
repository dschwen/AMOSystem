; ---------------------------------------------------------------------
;	Expressions and variables: interp/eval.cpp, interp/vars.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"

		SECTION	.text,CODE


; New_Evalue: the expression at a6. Out: d2-d4 the value, d0 the token
; after it (a6 there, or past it if it is a closing parenthesis).
		EvalRoutine New_Evalue,_interp_new_evalue

; =Max(a,b), =Min(a,b)
		FnCall	FnMax,_interp_fn_max
		FnCall	FnMin,_interp_fn_min

; Var = expression; Var as an operand
		InCall	InVar,_interp_in_var
		FnCall	FnVar,_interp_fn_var

; FindVar: the variable after the token at a6. Out: a0 its address, d2
; its type (d0, d1 changed).
		RegRoutine FindVar,_interp_find_var_reg

; GetTablo: a0 an array's slot. Out: a0 the element (d0 changed).
		RegRoutine GetTablo,_interp_get_tablo_reg


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
		EvalRoutine FnExtCall,_interp_fn_ext_call
		InCall	InField,_interp_in_field
