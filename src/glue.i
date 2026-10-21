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
; Every call into C++ goes through CCall, which keeps the interpreter
; registers d6-d7/a3-a6 in amos_ctx (core/amos.h) for the C++ code and its
; calls back into the assembler, and reloads them afterwards: C++ may have
; moved a3 or caused a call that changed them.
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

; Call the C function \1. Not for interrupt code: an interrupt would
; overwrite amos_ctx under the code it interrupted.
CCall		MACRO
		movem.l	d6-d7/a3-a6,_amos_ctx
		jsr	\1
		movem.l	_amos_ctx,d6-d7/a3-a6
		ENDM

; LibCall Name,cfunction,params,result
;
; The whole entry of an instruction or function Name implemented by the C
; function cfunction(p1, ..., pn): it gets the n parameters as 32-bit
; values (strings as the address of their length word), and its return
; value becomes the result. result is Int, Float, String or None.
LibCall		MACRO
		LibEntry \1
		IFNE	\3
		move.l	d3,-(sp)
		REPT	\3-1
		move.l	(a3)+,-(sp)
		ENDR
		ENDC
		CCall	\2
		IFNE	\3
		lea	4*\3(sp),sp
		ENDC
		IFNC	"\4","None"
		move.l	d0,d3
		ENDC
		Ret_\4
		ENDM

Ret_Int		MACRO
		moveq	#0,d2
		rts
		ENDM
Ret_Float	MACRO
		moveq	#1,d2
		rts
		ENDM
Ret_None	MACRO
		rts
		ENDM
Ret_String	MACRO
		moveq	#2,d2
		rts
		ENDM
