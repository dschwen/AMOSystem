; ---------------------------------------------------------------------
;	Tokenizer and detokenizer of the editor: editor/tokens.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_tokenise,_ed_detok_editor,_ed_detok_monitor,_ed_skip_token

		SECTION	.text,CODE

		xdef	_ed_tokenise_entry,_ed_detok_entry,_ed_mon_detok_entry,_ed_tinst_entry
_ed_tokenise_entry WEntry _ed_tokenise,w
_ed_detok_entry	WEntry	_ed_detok_editor,l
_ed_mon_detok_entry WEntry _ed_detok_monitor,l
_ed_tinst_entry	WEntry	_ed_skip_token,l
