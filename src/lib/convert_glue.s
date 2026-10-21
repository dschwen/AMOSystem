; ---------------------------------------------------------------------
;	Number conversions: register glue for lib/convert.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; L_LongToHex, L_LongToBin. In: d0 = number, d3 = digits (negative: no
; leading zeros), a0 = output. Out: a0 after the text. Keeps a1.
		LibRoutine LongToHex
		move.l	a1,-(sp)
		move.l	d3,-(sp)
		move.l	d0,-(sp)
		move.l	a0,-(sp)
		CCall	_lib_long_to_hex
		lea	12(sp),sp
		move.l	d0,a0
		move.l	(sp)+,a1
		rts

		LibRoutine LongToBin
		move.l	a1,-(sp)
		move.l	d3,-(sp)
		move.l	d0,-(sp)
		move.l	a0,-(sp)
		CCall	_lib_long_to_bin
		lea	12(sp),sp
		move.l	d0,a0
		move.l	(sp)+,a1
		rts
