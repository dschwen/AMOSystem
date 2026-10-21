; ---------------------------------------------------------------------
;	String functions: register glue for lib/strings.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	FnUpper,_lib_fn_upper,1,String		=UPPER$(a$)
		LibCall	FnLower,_lib_fn_lower,1,String		=LOWER$(a$)
		LibCall	FnSpace,_lib_fn_space,1,String		=SPACE$(n)
		LibCall	FnString,_lib_fn_string,2,String	=STRING$(a$,n)
		LibCall	FnFree,_lib_fn_free,0,Int		=FREE
		LibCall	FnFlip,_lib_fn_flip,1,String		=FLIP$(a$)
		LibCall	FnLen,_lib_fn_len,1,Int			=LEN(a$)
		LibCall	FnAsc,_lib_fn_asc,1,Int			=ASC(a$)
		LibCall	FnChr,_lib_fn_chr,1,String		=CHR$(n)
		LibCall	FnTab,_lib_fn_tab,0,String		=TAB$
		LibCall	FnCleft,_lib_fn_cleft,0,String		=CLEFT$
		LibCall	FnCright,_lib_fn_cright,0,String	=CRIGHT$
		LibCall	FnCup,_lib_fn_cup,0,String		=CUP$
		LibCall	FnCdown,_lib_fn_cdown,0,String		=CDOWN$
		LibCall	FnPenD,_lib_fn_pen,1,String		=PEN$(n)
		LibCall	FnPaperD,_lib_fn_paper,1,String		=PAPER$(n)
		LibCall	FnAt,_lib_fn_at,2,String		=AT(x,y)
		LibCall	FnCMoveD,_lib_fn_cmove,2,String		=CMOVE$(dx,dy)
		LibCall	FnRepeat,_lib_fn_repeat,2,String	=REPEAT$(a$,n)
		LibCall	FnBorderD,_lib_fn_border,2,String	=BORDER$(a$,n)
		LibCall	FnZoneD,_lib_fn_zone,2,String		=ZONE$(a$,n)
		LibCall	FnHex1,_lib_fn_hex1,1,String		=HEX$(n)
		LibCall	FnHex2,_lib_fn_hex2,2,String		=HEX$(n,digits)
		LibCall	FnBin1,_lib_fn_bin1,1,String		=BIN$(n)
		LibCall	FnBin2,_lib_fn_bin2,2,String		=BIN$(n,digits)
		LibCall	FnInstr2,_lib_fn_instr2,2,Int		=INSTR(a$,b$)
		LibCall	FnInstr3,_lib_fn_instr3,3,Int		=INSTR(a$,b$,start)

; L_InstrFind, called by the string subtraction, the verifier and the
; monitor. In: a1/d1 = string and length, a2/d2 = string to find and
; length, d4 = start. Out: d3 = position or 0. Keeps d1/d2/a2.
		LibRoutine InstrFind
		move.l	d1,-(sp)
		move.l	d4,-(sp)
		move.l	d2,-(sp)
		move.l	a2,-(sp)
		move.l	d1,-(sp)
		move.l	a1,-(sp)
		CCall	_lib_instr_find
		lea	20(sp),sp
		move.l	(sp)+,d1
		move.l	d0,d3
		rts
