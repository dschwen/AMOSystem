; ---------------------------------------------------------------------
;	Storing lines in the program: editor/storage.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_store,_ed_store_block,_ed_delete_line,_ed_delete_chunk_at,_ed_delete_chunk
		XREF	_ed_find_line,_ed_next_line

		SECTION	.text,CODE

		xdef	_ed_store_entry,_ed_store_block_entry,_ed_delete_line_entry
		xdef	_ed_delete_chunk_at_entry,_ed_delete_chunk_entry,_ed_find_line_entry,_ed_next_line_entry
_ed_store_entry	WEntry	_ed_store,l
_ed_store_block_entry WEntry _ed_store_block,l
_ed_delete_line_entry WEntry _ed_delete_line,l
_ed_delete_chunk_at_entry WEntry _ed_delete_chunk_at,l
_ed_delete_chunk_entry WEntry _ed_delete_chunk,l
_ed_find_line_entry WEntry _ed_find_line,w
_ed_next_line_entry WEntry _ed_next_line,w
