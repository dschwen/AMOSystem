; ---------------------------------------------------------------------
;	Collisions of bobs and sprites (SyCall): w/collision.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_bob_collisions,_w_sy_sprite_collisions,_w_collision_get

		SECTION	.text,CODE

		xdef	_w_bob_col,_w_sprite_col,_w_get_col
_w_bob_col	WEntry	_w_sy_bob_collisions,l
_w_sprite_col	WEntry	_w_sy_sprite_collisions,l

; GetCol: d1 the number, d0 the result. Also from AMAL in the VBL: a5 the
; data zone, no task context.
_w_get_col	movem.l	d1/a0-a1,-(sp)
