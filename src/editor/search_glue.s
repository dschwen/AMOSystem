; ---------------------------------------------------------------------
;	Search and replace: editor/search.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_search,_ed_search_front,_ed_search_back,_ed_replace_text

		SECTION	.text,CODE

		xdef	_ed_search_entry,_ed_search_front_entry,_ed_search_back_entry,_ed_replace_text_entry
_ed_search_entry WEntry	_ed_search,l
_ed_search_front_entry WEntry _ed_search_front,l
_ed_search_back_entry WEntry _ed_search_back,l
_ed_replace_text_entry WEntry _ed_replace_text,l
