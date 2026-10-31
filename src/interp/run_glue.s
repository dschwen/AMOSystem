; ---------------------------------------------------------------------
;	RUN, PRUN, ASK/CALL EDITOR, =PRG UNDER, =PRG STATE: interp/run.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

Ru_BasSp	equ	1060			IData offset (core/layout.h)

		LibCall	FnPrgState,_interp_fn_prg_state,0,Int		=PRG STATE
		LibCall	FnPrgUnder,_interp_fn_prg_under,0,Int		=PRG UNDER
		LibCall	InRun0,_interp_in_run0,0,None			RUN
		LibCall	InRun1,_interp_in_run1,1,None			RUN "program"
		LibCall	InPRun,_interp_in_prun,1,None			PRUN "program"
		LibCall	InAskEditor1,_interp_in_ask_editor1,1,None	ASK EDITOR f
		LibCall	InAskEditor2,_interp_in_ask_editor2,2,None	ASK EDITOR f,p
		LibCall	InAskEditor3,_interp_in_ask_editor3,3,None	ASK EDITOR f,p,t$
		LibCall	InCallEditor1,_interp_in_call_editor1,1,None	CALL EDITOR f
		LibCall	InCallEditor2,_interp_in_call_editor2,2,None	CALL EDITOR f,p
		LibCall	InCallEditor3,_interp_in_call_editor3,3,None	CALL EDITOR f,p,t$

; RunII: the program in Name1 in place of the current one (a compiled
; procedure jumps here)
		LibRoutine RunII

; uint32_t interp_prun_run(program, Prg_RunIt): runs the program on top of
; this one. Its errors come back to PRun_Errors with the stack at BasSp:
; the interpreter registers and this frame are under it. Returns the error
; (d0), or -1 if Prg_RunIt came back (out of memory).
		xdef	_interp_prun_run
_interp_prun_run
		movem.l	d2-d7/a2-a6,-(sp)
PRun_Back	movem.l	(sp)+,a3-a6/d6/d7
		xdef	_interp_prun_errors
_interp_prun_errors
PRun_Errors	move.l	Ru_BasSp(a5),sp

; void interp_zap_in(Regs *r, Ed_ZapIn): the editor driven, nested: it
; comes back with the stack at BasSp-4 (Ed_ZapOut); d0 and a0 its answer
		xdef	_interp_zap_in
_interp_zap_in
		movem.l	d2-d7/a2-a6,-(sp)
