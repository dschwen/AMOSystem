; ---------------------------------------------------------------------
;	Hardware sprites (SyCall): w/sprites.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_sprite_set,_w_sy_sprite_unset,_w_sy_sprites_show,_w_sy_sprites_update
		XREF	_w_sy_sprite_bank,_w_sy_sprite,_w_sy_sprite_off,_w_sy_sprites_off,_w_sy_sprite_xy
		XREF	_w_sy_sprites_stop,_w_sy_sprites_restart,_w_sy_sprite_priority,_w_sy_sprite_buffer

		SECTION	.text,CODE

		xdef	_w_sprite_set,_w_sprite_unset,_w_sprites_show,_w_sprites_update
		xdef	_w_sprite_bank,_w_sprite,_w_sprite_off,_w_sprites_off,_w_sprite_xy
		xdef	_w_sprites_stop,_w_sprites_restart,_w_sprite_priority,_w_sprite_buffer
_w_sprite_set	WEntry	_w_sy_sprite_set,l
_w_sprite_unset	WEntry	_w_sy_sprite_unset,l
_w_sprites_show	WEntry	_w_sy_sprites_show,l
_w_sprites_update WEntry _w_sy_sprites_update,l
_w_sprite_bank	WEntry	_w_sy_sprite_bank,l
_w_sprite	WEntry	_w_sy_sprite,l
_w_sprite_off	WEntry	_w_sy_sprite_off,l
_w_sprites_off	WEntry	_w_sy_sprites_off,l
_w_sprite_xy	WEntry	_w_sy_sprite_xy,l
_w_sprites_stop	WEntry	_w_sy_sprites_stop,l
_w_sprites_restart WEntry _w_sy_sprites_restart,l
_w_sprite_priority WEntry _w_sy_sprite_priority,l
_w_sprite_buffer WEntry	_w_sy_sprite_buffer,l
