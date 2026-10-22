; ---------------------------------------------------------------------
;	Lines in and out of the program: editor/edit.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_del_li_cu_fn,_ed_join_fn,_ed_return_fn,_ed_return_quiet_fn,_ed_ins_line_fn
		XREF	_ed_tok_cur_fn,_ed_tok_stok,_ed_tok_stok2

		SECTION	.text,CODE

		xdef	_ed_del_li_cu_entry,_ed_join_entry,_ed_return_entry,_ed_return_quiet_entry
		xdef	_ed_ins_line_entry,_ed_tok_cur_entry,_ed_tok_stok_entry,_ed_tok_stok2_entry
_ed_del_li_cu_entry WEntry _ed_del_li_cu_fn,l
_ed_join_entry	WEntry	_ed_join_fn,l
_ed_return_entry WEntry	_ed_return_fn,l
_ed_return_quiet_entry WEntry _ed_return_quiet_fn,l
_ed_ins_line_entry WEntry _ed_ins_line_fn,l
_ed_tok_cur_entry WEntry _ed_tok_cur_fn,l
_ed_tok_stok_entry WEntry _ed_tok_stok,l
_ed_tok_stok2_entry WEntry _ed_tok_stok2,l
