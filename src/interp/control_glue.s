; ---------------------------------------------------------------------
;	Jumps of the interpreter: interp/control.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"

		SECTION	.text,CODE

; [[noreturn]] void interp_continue(uint32_t sp, uint32_t ret)
;
; The interpreter goes on: its registers from the context, the stack at
; sp (BasSp: the instruction is abandoned), then ret (Prg_InsRet: back
; into the ChrGet loop).
		xdef	_interp_continue
_interp_continue
		move.l	4(sp),d0
		move.l	8(sp),d1
		GetCtx	a0
		movem.l	(a0),d6-d7/a3-a6
		move.l	d0,sp
		move.l	d1,-(sp)
		rts

; [[noreturn]] void interp_jump(const Regs *r, const void *fn)
;
; Jumps to fn with d0-d5/a0-a2 from r and the interpreter registers of the
; context (d6-d7 too), on the stack as it is (fn resets it).
		xdef	_interp_jump
_interp_jump
		GetCtx	a0
		movem.l	(a0),d6-d7/a3-a6
		move.l	4(sp),a0
		move.l	8(sp),-(sp)
		movem.l	(a0),d0-d5
		movem.l	8*4(a0),a0-a2
		rts

; [[noreturn]] void interp_menu_return(uint32_t pile)
;
; End Proc of a procedure called by a menu: back to the instruction after
; the call (the stack pointer MnPile kept), d0 = 0
		xdef	_interp_menu_return
_interp_menu_return
		move.l	4(sp),a0
		lea	-4(a0),sp
		moveq	#0,d0
		rts

; Error d0 (L_Error: of AMOSPro, d1 = 19 the first that On Error can
; take, d2 = -1) / ErrorExt (d1, d2 = the extension, a0 its messages).
; Never returns.
		LibRoutine Error
		moveq	#19,d1
		moveq	#-1,d2
		LibRoutine ErrorExt
		move.l	a0,-(sp)
		move.l	d2,-(sp)
		move.l	d1,-(sp)
		move.l	d0,-(sp)
		CCall	_interp_error

; uint32_t interp_next_float(uint32_t *var, uint32_t step, uint32_t limit,
;                            void *floatbase)
;
; Next of a float loop, with mathffp (as the original, in either
; precision): var += step; 1 if the loop goes on, 0 if it ends
		xdef	_interp_next_float
_interp_next_float
		movem.l	d2-d4/a2/a6,-(sp)
		move.l	24(sp),a2
		move.l	28(sp),d3
		move.l	32(sp),d4
		move.l	36(sp),a6
		move.l	d3,d1
		jsr	-48(a6)			SPTst
		move.l	d0,d2
		move.l	d3,d0
		move.l	(a2),d1
		jsr	-66(a6)			SPAdd
		move.l	d0,(a2)
		move.l	d4,d1
		jsr	-42(a6)			SPCmp
		blt.s	.Lt
		tst.l	d2
		bmi.s	.Stay
		bra.s	.Out
.Lt		tst.l	d2
		bpl.s	.Stay
.Out		moveq	#0,d0
		bra.s	.X
.Stay		moveq	#1,d0
.X		movem.l	(sp)+,d2-d4/a2/a6
		rts

; The routine record of On Menu's OMnEff for ClearVar: a link, then code
		XREF	_interp_omn_eff
		xdef	_interp_omn_record
_interp_omn_record
		dc.l	0
		CCall	_interp_omn_eff
		rts

; The tests between instructions: all registers kept
TestCall	MACRO
		LibRoutine \1
		movem.l	d0-d7/a0-a2,-(sp)
		CCall	\2
		movem.l	(sp)+,d0-d7/a0-a2
		rts
		ENDM
		TestCall Test_PaSaut,_interp_test_pasaut
		TestCall Test_Normal,_interp_test_normal
		TestCall Test_Force,_interp_test_force

; Instructions (InCall: with the "GetP" of Lib_Par; RoutineCall: Lib_Def)
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
		InCall	InLab,_interp_in_lab
		InCall	InSystem,_interp_in_system
		InCall	InEdit,_interp_in_edit
		InCall	InDirect,_interp_in_direct
		InCall	InBreakOn,_interp_in_break_on
		InCall	InBreakOff,_interp_in_break_off
		InCall	InOnBreak,_interp_in_on_break
		InCall	InOnError,_interp_in_on_error
		InCall	InResumeLabel,_interp_in_resume_label
		InCall	InResume,_interp_in_resume
		InCall	InResumeNext,_interp_in_resume_next
		InCall	InTrap,_interp_in_trap
		InCall	InEvery,_interp_in_every
		InCall	InEveryOff,_interp_in_every_off
		InCall	InEveryOn,_interp_in_every_on
		InCall	InFor,_interp_in_for
		InCall	InNext,_interp_in_next
		InCall	InRepeat,_interp_in_repeat
		InCall	InUntil,_interp_in_until
		InCall	InWhile,_interp_in_while
		InCall	InWend,_interp_in_wend
		InCall	InDo,_interp_in_do
		InCall	InLoop,_interp_in_loop
		InCall	InExit,_interp_in_exit
		InCall	InExitIf,_interp_in_exit_if
		InCall	InIf,_interp_in_if
		InCall	InElse,_interp_in_else
		InCall	InElseIf,_interp_in_else_if
		InCall	InGosub,_interp_in_gosub
		InCall	InReturn,_interp_in_return
		InCall	InPop,_interp_in_pop
		InCall	InProcedure,_interp_in_procedure
		InCall	InProc,_interp_in_proc
		RoutineCall CallProc,_interp_call_proc
		InCall	InEndProc,_interp_in_end_proc
		InCall	InPopProc,_interp_in_pop_proc
		RoutineCall Goto2,_interp_goto2
		InCall	InGoto,_interp_in_goto
		InCall	InOn,_interp_in_on
		InCall	InRestore,_interp_in_restore
		InCall	In_apml_,_interp_in_apml
		InCall	InOnMenu,_interp_in_on_menu

; =Errtrap
		LibEntry FnErrTrap
		lea	-24(sp),sp
		pea	(sp)
		CCall	_interp_fn_err_trap
		addq.l	#4,sp
		movem.l	(sp)+,d0-d4/a0
		rts

; MenuProcedure: a0 the procedure's name. Out: d0 = -1 if there is none
; (else the procedure runs, and its End Proc comes back through MnPile)
		LibRoutine MenuProcedure
		lea	-24(sp),sp
		move.l	a0,20(sp)
		pea	(sp)
		CCall	_interp_menu_procedure
		addq.l	#4,sp
		movem.l	(sp)+,d0-d4/a0
		rts
