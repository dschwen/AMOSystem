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
; registers d6-d7/a3-a6 in the task's context (core/amos.h) for the C++
; code and its calls back into the assembler, and reloads them afterwards:
; C++ may have moved a3 or caused a call that changed them.
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

; GetCtx An: An = the context of the running task, where CCall keeps
; d6-d7/a3-a6: amos_ctx for the main task (amos_task), amos_ctx2 for any
; other (the file selector's)
GetCtx		MACRO
		move.l	$4.w,\1
		move.l	276(\1),\1		ThisTask
		cmp.l	_amos_task,\1
		bne.s	.Other\@
		lea	_amos_ctx,\1
		bra.s	.Done\@
.Other\@	lea	_amos_ctx2,\1
.Done\@
		ENDM

; Call the C function \1 (a0 is lost). Not for interrupt code: an
; interrupt would overwrite the context under the code it interrupted.
CCall		MACRO
		GetCtx	a0
		movem.l	d6-d7/a3-a6,(a0)
		jsr	\1
		GetCtx	a0
		movem.l	(a0),d6-d7/a3-a6
		ENDM

; LibCall Name,cfunction,params,result
;
; The whole entry of an instruction or function Name implemented by the C
; function cfunction(p1, ..., pn): it gets the n parameters as 32-bit
; values (strings as the address of their length word), and its return
; value becomes the result. result is Int, Float, String, None or Double
; (a 64-bit value for d3/d4: a float in either precision).
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
		IFC	"\4","Double"
		move.l	d1,d4
		ENDC
		Ret_\4
		ENDM

Ret_Int		MACRO
		moveq	#0,d2
		rts
		ENDM
Ret_Double	MACRO
		moveq	#1,d2
		rts
		ENDM
Ret_Float	MACRO
		moveq	#1,d2
		rts
		ENDM
; RegRoutine Name,cfunction
;
; A routine the assembler calls with registers: uint32_t cfunction(RegFrame
; *r) gets d0-d7/a0-a2 in r and changes them there; they are reloaded on
; return. The condition codes come from its return value (tst.l), for the
; callers that branch on them.
RegRoutine	MACRO
		LibRoutine \1
		movem.l	d0-d7/a0-a2,-(sp)
		pea	(sp)
		CCall	\2
		addq.l	#4,sp
		tst.l	d0
		movem.l	(sp)+,d0-d7/a0-a2
		rts
		ENDM

Ret_None	MACRO
		rts
		ENDM
Ret_String	MACRO
		moveq	#2,d2
		rts
		ENDM
