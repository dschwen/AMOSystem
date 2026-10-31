; ---------------------------------------------------------------------
;	The ChrGet: interp/chrget.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; New_ChrGet: a program starts, the stack at BasSp
Ch_BasSp	equ	1060			IData offsets (core/layout.h)
Ch_AdTokens	equ	32

		LibRoutine New_ChrGet

; Prg_InsRet: back into the loop after an instruction, at a6
		xdef	_interp_iret
_interp_iret
		CCall	_interp_chrget_continue

; void interp_exec_value(const void *routine, uint32_t d0, uint32_t d1,
;                        const Value *v)
;
; The same with d2/d3/d4 from v: the last parameter, as its collector left
; it (interp/params.cpp)
		xdef	_interp_exec_value
_interp_exec_value
		movem.l	d2-d7/a2-a6,-(sp)

; The instruction of token d0, as the ChrGet runs it (the monitor's
; stepping goes back to the instruction here)
		xdef	_interp_step_instruction
_interp_step_instruction
		move.l	d0,-(sp)

; void interp_exec(const void *routine, uint32_t d0, uint32_t d1)
;
; An instruction: its routine called with d0 (the token), d1 (its
; entry) and the interpreter registers of the context, which get them
; back
		xdef	_interp_exec
_interp_exec
		movem.l	d2-d7/a2-a6,-(sp)

		RegRoutine SetChrPatch,_interp_set_chr_patch
