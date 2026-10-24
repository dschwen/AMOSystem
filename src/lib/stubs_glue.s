; ---------------------------------------------------------------------
;	The small routines of AMOSPro.Lib: the errors, the constant
;	functions, the names of the banks, the registers kept for the
;	errors, the bob and icon addresses
; ---------------------------------------------------------------------
		Include	"glue.i"
		Include	"+LEqu.s"

		SECTION	.text,CODE

St_AdTokens	equ	32			IData offsets (core/layout.h)
St_ErrorSave	equ	2640
St_ErrorRegs	equ	2567
EcEBase		equ	45			+Equ.s
DEBase		equ	EcEBase+35-1
SpEBase		equ	DEBase+25

; A routine of AMOSPro.Lib, through its jump table (a0 lost)
LJmp		MACRO
		move.l	St_AdTokens(a5),a0
		move.l	-36-4-\1*4(a0),a0
		jmp	(a0)
		ENDM
LJsr		MACRO
		move.l	St_AdTokens(a5),a0
		move.l	-36-4-\1*4(a0),a0
		jsr	(a0)
		ENDM

; Error n
Err		MACRO
		LibRoutine \1
		moveq	#\2,d0
		LJmp	L_GoError
		ENDM
; Error n of the screens and windows
EcErr		MACRO
		LibRoutine \1
		moveq	#\2,d0
		LJmp	L_EcWiErr
		ENDM

		LibRoutine GoError
		LJmp	L_Error
		Err	TypeMis,34
		Err	FilOO,DEBase+17
		Err	FilNO,DEBase+18
		Err	FilTM,DEBase+19
		Err	EOFil,DEBase+21
		Err	IffFor2,30
		Err	DiForm,DEBase+16
		Err	BkAlRes,35
		Err	BkNoRes,36
		Err	OOfMem,24
		Err	StooLong,21
		Err	FonCall,23
		Err	AdrErr,25
		Err	CantFit,32
		Err	IffFor,30
		Err	IffCmp,31
		Err	FftE,37
		Err	SpErr,SpEBase
		EcErr	ScNOp,3
		EcErr	RainErr,31
		EcErr	IllScN,6			Illegal screen number
		EcErr	BFonCall,22
		EcErr	WFonCall,16

; EcWiErr: d0 an error of the screens (1: out of memory)
		LibRoutine EcWiErr
		cmp.w	#1,d0
		bne.s	.Err
		LJmp	L_OOfMem
.Err		add.w	#EcEBase-1,d0
		LJmp	L_GoError
; CopErr: d0 an error of the copper
		LibRoutine CopErr
		add.w	#EcEBase+32-2,d0
		LJmp	L_GoError
; AdBErr: d1 = 0 no bank, else no such image
		LibRoutine AdBErr
		tst.w	d1
		bne.s	.Img
		LJmp	L_BkNoRes
.Img		moveq	#EcEBase+30-1,d0
		LJmp	L_GoError

; Syntax error, STOP, font not found, ERROR n
		LibEntry Syntax
		moveq	#22,d0
		LJmp	L_GoError
		LibEntry Syntax2
		LJmp	L_Syntax
		LibEntry InStop
		moveq	#9,d0
		LJmp	L_GoError
		LibEntry FnfE
		moveq	#44,d0
		LJmp	L_GoError
		LibEntry InError
		cmp.l	#256,d3
		bcs.s	.Skip
		move.l	#255,d3
.Skip		move.l	d3,d0
		LJmp	L_GoError

; =TRUE, =FALSE, the functions that are only a token
		LibEntry FnTrue
		moveq	#-1,d3
		Ret_Int
		LibEntry FnOne
		moveq	#1,d3
		Ret_Int
		LibEntry FnFalse
		moveq	#0,d3
		Ret_Int
		LibEntry FnInt
		rts
		LibEntry FnPSel
		rts

; The registers of the program kept for the errors
		LibRoutine SaveRegs
		movem.l	d6-d7,St_ErrorSave(a5)
		move.b	#1,St_ErrorRegs(a5)
		rts
		LibRoutine LoadRegs
		movem.l	St_ErrorSave(a5),d6-d7
		clr.b	St_ErrorRegs(a5)
		rts

; MajD0: d0 in capitals
		LibRoutine MajD0
		cmp.b	#"a",d0
		bcs.s	.Ski
		cmp.b	#"z",d0
		bhi.s	.Ski
		sub.b	#$20,d0
.Ski		rts

; AdBob, AdIcon: d1 an image (its flags above). Out: a2 its address, d5
; its number, d0 = 0
AdImage		MACRO
		LibRoutine \1
		and.l	#$3FFF,d1
		bne.s	.Some\@
		LJmp	L_FonCall
.Some\@		move.l	d1,d0
		LJsr	\2
		bne.s	.Ok\@
		LJmp	L_AdBErr
.Ok\@		move.l	a0,a2
		move.w	d1,d5
		moveq	#0,d0
		rts
		ENDM
		AdImage	AdBob,L_Bnk.AdBob
		AdImage	AdIcon,L_Bnk.AdIcon

; The names of the banks
		xdef	BkSpr,BkIco,BkMus,BkAmal,BkMenu,BkDat,BkWrk,BkAsm,BkIff,Bnk_NoLoad,NHunk
BkSpr		dc.b	"Sprites "
BkIco		dc.b	"Icons   "
BkMus		dc.b	"Music   "
BkAmal		dc.b	"Amal    "
BkMenu		dc.b	"Menu    "
BkDat		dc.b	"Data    "
BkWrk		dc.b	"Work    "
BkAsm		dc.b	"Asm     "
BkIff		dc.b	"Iff     "
Bnk_NoLoad	dc.b	"Loading!"
NHunk		dc.b	"AmBk"
		dc.b	"AmSp"
		dc.b	"AmBs"
		dc.b	"AmIc"
		dc.l	0
