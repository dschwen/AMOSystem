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
; d6-d7/a3-a6. amos_bridge points to the block of the contexts (see
; core/call.s): the main task, its context, the context of another task.
GetCtx		MACRO
		move.l	$4.w,\1
		move.l	276(\1),-(sp)		ThisTask
		move.l	_amos_bridge,\1
		cmpm.l	(\1)+,(sp)+
		beq.s	.Main\@
		lea	24(\1),\1
.Main\@
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

; The tag before a LibCall entry, for the ChrGet (interp/chrget.cpp),
; which calls cfunction itself: cfunction, params, result, "LCal" (then
; "GetP" and the entry)
LC_Int		equ	0
LC_Float	equ	1
LC_String	equ	2
LC_None		equ	3
LC_Double	equ	4
LibCallTag	MACRO
		dc.l	\1
		dc.w	\2,LC_\3
		dc.l	"LCal"
		ENDM

; LibCall Name,cfunction,params,result
;
; The whole entry of an instruction or function Name implemented by the C
; function cfunction(p1, ..., pn): it gets the n parameters as 32-bit
; values (strings as the address of their length word), and its return
; value becomes the result. result is Int, Float, String, None or Double
; (a 64-bit value for d3/d4: a float in either precision).
LibCall		MACRO
		LibCallTag \2,\3,\4
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
; LibNop Name: an instruction or function whose parameter collector does
; it all (the value as it left it)
LibNop		MACRO
		LibEntry \1
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

; StackEntry Name,target: the interpreter's stack back (BasSp: what was
; on it given up), then target
StackEntry	MACRO
\1		move.l	1060(a5),sp		BasSp
		jmp	\2
		ENDM

; RoutineRecord name,cfunction: a record for AMOS.library's AddRoutine /
; CallRoutines (a link, then the code, called with jsr 4(a0), all the
; registers saved), calling cfunction()
RoutineRecord	MACRO
		xdef	\1
\1		dc.l	0
		CCall	\2
		rts
		ENDM

; CRoutine Name,cfunction: a routine taking its parameters on the stack,
; the C way (for the binary extensions)
CRoutine	MACRO
		LibRoutine \1
		jmp	\2
		ENDM

; Instructions and routines calling cfunction() with no registers:
; InCall Name,cfunction (with the "GetP" of Lib_Par), RoutineCall
; Name,cfunction (Lib_Def)
InCall		MACRO
		LibEntry \1
		CCall	\2
		rts
		ENDM
RoutineCall	MACRO
		LibRoutine \1
		CCall	\2
		rts
		ENDM

; TestCall Name,cfunction: the same, all registers kept (the tests
; between instructions)
TestCall	MACRO
		LibRoutine \1
		movem.l	d0-d7/a0-a2,-(sp)
		CCall	\2
		movem.l	(sp)+,d0-d7/a0-a2
		rts
		ENDM

; EvalCall cfunction: cfunction(EvalOut *) gives d0-d4/a0 (interp.h)
EvalCall	MACRO
		lea	-24(sp),sp
		pea	(sp)
		CCall	\1
		addq.l	#4,sp
		movem.l	(sp)+,d0-d4/a0
		rts
		ENDM
; Functions: FnCall Name,cfunction; routines: EvalRoutine Name,cfunction
FnCall		MACRO
		LibEntry \1
		EvalCall \2
		ENDM
EvalRoutine	MACRO
		LibRoutine \1
		EvalCall \2
		ENDM
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

Ret_None	MACRO
		rts
		ENDM
Ret_String	MACRO
		moveq	#2,d2
		rts
		ENDM
