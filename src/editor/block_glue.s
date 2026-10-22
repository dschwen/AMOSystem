; ---------------------------------------------------------------------
;	The marked block: editor/block.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_block_show,_ed_block_on,_ed_block_cursor,_ed_block_hide,_ed_cursor_normal_fn
		XREF	_ed_block_all,_ed_block_store,_ed_block_forget,_ed_block_cut,_ed_block_paste
		XREF	_ed_block_free,_ed_block_free_a0,_ed_block_limits,_ed_block_insert,_ed_block_delete
		XREF	_ed_block_copy

		SECTION	.text,CODE

		xdef	_ed_block_show_entry,_ed_block_on_entry,_ed_block_cursor_entry,_ed_block_hide_entry
		xdef	_ed_cursor_normal_entry,_ed_block_all_entry,_ed_block_store_entry,_ed_block_forget_entry
		xdef	_ed_block_cut_entry,_ed_block_paste_entry,_ed_block_free_entry,_ed_block_free_a0_entry
		xdef	_ed_block_limits_entry,_ed_block_insert_entry,_ed_block_delete_entry,_ed_block_copy_entry
_ed_block_show_entry WEntry _ed_block_show,l
_ed_block_on_entry WEntry _ed_block_on,l
_ed_block_cursor_entry WEntry _ed_block_cursor,l
_ed_block_hide_entry WEntry _ed_block_hide,l
_ed_cursor_normal_entry WEntry _ed_cursor_normal_fn,l
_ed_block_all_entry WEntry _ed_block_all,l
_ed_block_store_entry WEntry _ed_block_store,l
_ed_block_forget_entry WEntry _ed_block_forget,l
_ed_block_cut_entry WEntry _ed_block_cut,l
_ed_block_paste_entry WEntry _ed_block_paste,l
_ed_block_free_entry WEntry _ed_block_free,l
_ed_block_free_a0_entry WEntry _ed_block_free_a0,l
_ed_block_limits_entry WEntry _ed_block_limits,l
_ed_block_insert_entry WEntry _ed_block_insert,l
_ed_block_delete_entry WEntry _ed_block_delete,l
_ed_block_copy_entry WEntry _ed_block_copy,l
