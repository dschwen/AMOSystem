; ---------------------------------------------------------------------
;	Bobs (SyCall) and blocks (EcCall): w/bobs.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_bob_set,_w_sy_bob_off,_w_sy_bobs_off,_w_sy_bob_update,_w_sy_bob_show
		XREF	_w_sy_bob_erase,_w_sy_bob_limit,_w_sy_get_bob,_w_sy_mask,_w_sy_hot_spot
		XREF	_w_sy_bob_xy,_w_sy_bob_put,_w_sy_paste,_w_sy_priority,_w_sy_bobs_screen_off
		XREF	_w_ec_block_get,_w_ec_block_delete,_w_ec_blocks_delete,_w_ec_block_put
		XREF	_w_ec_block_flip,_w_ec_flip,_w_ec_cblock_get,_w_ec_cblock_put
		XREF	_w_ec_cblock_delete,_w_ec_cblocks_delete

		SECTION	.text,CODE

		xdef	_w_bob_set,_w_bob_off,_w_bobs_off,_w_bob_update,_w_bob_show
		xdef	_w_bob_erase,_w_bob_limit,_w_get_bob,_w_mask,_w_hot_spot
		xdef	_w_bob_xy,_w_bob_put,_w_paste,_w_priority,_w_bobs_screen_off
		xdef	_w_block_get,_w_block_delete,_w_blocks_delete,_w_block_put
		xdef	_w_block_flip,_w_flip,_w_cblock_get,_w_cblock_put
		xdef	_w_cblock_delete,_w_cblocks_delete
_w_bob_set	WEntry	_w_sy_bob_set,w
_w_bob_off	WEntry	_w_sy_bob_off,l
_w_bobs_off	WEntry	_w_sy_bobs_off,l
_w_bob_update	WEntry	_w_sy_bob_update,l
_w_bob_show	WEntry	_w_sy_bob_show,l
_w_bob_erase	WEntry	_w_sy_bob_erase,l
_w_bob_limit	WEntry	_w_sy_bob_limit,l
_w_get_bob	WEntry	_w_sy_get_bob,l
_w_mask		WEntry	_w_sy_mask,l
_w_hot_spot	WEntry	_w_sy_hot_spot,l
_w_bob_xy	WEntry	_w_sy_bob_xy,l
_w_bob_put	WEntry	_w_sy_bob_put,l
_w_paste	WEntry	_w_sy_paste,l
_w_priority	WEntry	_w_sy_priority,l
_w_bobs_screen_off WEntry _w_sy_bobs_screen_off,l
_w_block_get	WEntry	_w_ec_block_get,l
_w_block_delete	WEntry	_w_ec_block_delete,l
_w_blocks_delete WEntry	_w_ec_blocks_delete,l
_w_block_put	WEntry	_w_ec_block_put,l
_w_block_flip	WEntry	_w_ec_block_flip,l
_w_flip		WEntry	_w_ec_flip,l
_w_cblock_get	WEntry	_w_ec_cblock_get,w
_w_cblock_put	WEntry	_w_ec_cblock_put,l
_w_cblock_delete WEntry	_w_ec_cblock_delete,l
_w_cblocks_delete WEntry _w_ec_cblocks_delete,l
