; ---------------------------------------------------------------------
;	Glue for AMOSPro.Lib routines implemented outside the library
; ---------------------------------------------------------------------
;
; A routine moved out of AMOSPro.Lib is declared there with Lib_Int (no
; body). Its address reaches the library's jump table through the
; AMOSJmps list in +B.s (generated as +Internal_Jumps.s), and the code is
; linked into the AMOSPro executable.
;
; Library conventions on entry: a5 = data zone, a4 = AdTokens (the token
; table, jump table below it), a3 = parameter stack, a6 = program pointer.
; d6/d7 and a3-a6 must be preserved; the C calling convention does that
; for us (d2-d7/a2-a6 are callee-saved), so glue only moves values
; between the AMOS registers and the C stack.
;
; Parameters: the last parameter is in d3, earlier ones on the a3 stack
; (first parameter deepest). Results: d3 = value, d2 = type (0 int,
; 1 float, 2 string).

; Entry of an instruction or function, called through the token table.
; The "GetP" placeholder is replaced by Library_GetParams with the call
; to the parameter collector; the jump table then points at that call.
LibEntry	MACRO
		xdef	\1
		dc.b	"GetP"
\1
		ENDM

; Entry of a plain routine (called with Rjsr / JJsr)
LibRoutine	MACRO
		xdef	\1
\1
		ENDM

Ret_Int		MACRO
		moveq	#0,d2
		rts
		ENDM
Ret_Float	MACRO
		moveq	#1,d2
		rts
		ENDM
Ret_String	MACRO
		moveq	#2,d2
		rts
		ENDM
