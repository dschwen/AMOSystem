; ---------------------------------------------------------------------
;	The text window functions of AMOS.library: glue for w/windows.cpp
; ---------------------------------------------------------------------
;
; WiCall n jumps to entry n of the table T_WiVect points to (WiInit in
; +W.s). Every entry gives all its registers to w_window(n, regs), which
; changes those the function returns, and sets the condition codes from
; d0 as the assembler did.
;
; The registers d6-d7/a3-a6 of the caller become the context of the task
; (see src/glue.i) for the C++ code and its calls back into AMOS.library
; (a5: the data zone). The context is put back afterwards: the caller may
; be C++ (amos_call) or a task that is not the interpreter's.

		Include	"glue.i"

		XREF	_w_window

		SECTION	.text,CODE

WiEntry		MACRO
		movem.l	d0-d7/a0-a6,-(sp)
		moveq	#\1,d0
		bsr	WiGo
		movem.l	(sp)+,d0-d7/a0-a6
		tst.\2	d0
		rts
		ENDM

		xdef	_w_wi_in
_w_wi_in	bra.w	WOutC
		bra.w	WPrint
		bra.w	WCentre
		bra.w	_w_wopen
		bra.w	WLocate
		bra.w	WQWind
		bra.w	WDel
		bra.w	WSBor
		bra.w	WSTit
		bra.w	WAdr
		bra.w	WiMove
		bra.w	WiCls
		bra.w	WiSize
		bra.w	WiSCur
		bra.w	WiXYCu
		bra.w	WiXGr
		bra.w	WiYGr
		bra.w	WPrint2
		bra.w	WPrint3
		bra.w	WiXYWi

WOutC		WiEntry	0,l
WPrint		WiEntry	1,l
WCentre		WiEntry	2,w
		xdef	_w_wopen
_w_wopen	WiEntry	3,l
WLocate		WiEntry	4,l
WQWind		WiEntry	5,l
WDel		WiEntry	6,l
WSBor		WiEntry	7,l
WSTit		WiEntry	8,l
WAdr		WiEntry	9,l
WiMove		WiEntry	10,l
WiCls		WiEntry	11,l
WiSize		WiEntry	12,l
WiSCur		WiEntry	13,l
WiXYCu		WiEntry	14,l
WiXGr		WiEntry	15,l
WiYGr		WiEntry	16,l
WPrint2		WiEntry	17,l
WPrint3		WiEntry	18,w
WiXYWi		WiEntry	19,l
; WiDelA: close all the windows of the current screen
		xdef	_w_widela
_w_widela	WiEntry	20,l

; d0 = the function, the registers at 4(sp)
WiGo		lea	4(sp),a1
		GetCtx	a0
		movem.l	(a0),d1-d6		keep the context
		movem.l	d1-d6,-(sp)
		move.l	24(a1),(a0)+		the caller's d6-d7/a3-a6
		move.l	28(a1),(a0)+
		move.l	44(a1),(a0)+
		move.l	48(a1),(a0)+
		move.l	52(a1),(a0)+
		move.l	56(a1),(a0)
		move.l	a1,-(sp)
		move.l	d0,-(sp)
		jsr	_w_window
		addq.l	#8,sp
		GetCtx	a0
		movem.l	(sp)+,d1-d6
		movem.l	d1-d6,(a0)
		rts
