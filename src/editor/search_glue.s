; ---------------------------------------------------------------------
;	Search and replace: editor/search.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_search,_ed_search_front,_ed_search_back,_ed_replace_text

		XREF	_ed_goto_line
		XREF	_ed_search_dialogue
		XREF	_ed_search_new
		XREF	_ed_search_previous
		XREF	_ed_search_next
		XREF	_ed_replace_new
		XREF	_ed_replace_next
		XREF	_ed_replace_previous

		SECTION	.text,CODE

		xdef	_ed_search_entry,_ed_search_front_entry,_ed_search_back_entry,_ed_replace_text_entry
_ed_search_entry WEntry	_ed_search,l
_ed_search_front_entry WEntry _ed_search_front,l
_ed_search_back_entry WEntry _ed_search_back,l
_ed_replace_text_entry WEntry _ed_replace_text,l
		xdef	_ed_goto_line_entry
_ed_goto_line_entry WEntry _ed_goto_line,l
		xdef	_ed_search_dialogue_entry
_ed_search_dialogue_entry WEntry _ed_search_dialogue,w
		xdef	_ed_search_new_entry
_ed_search_new_entry WEntry _ed_search_new,l
		xdef	_ed_search_previous_entry
_ed_search_previous_entry WEntry _ed_search_previous,l
		xdef	_ed_search_next_entry
_ed_search_next_entry WEntry _ed_search_next,l
		xdef	_ed_replace_new_entry
_ed_replace_new_entry WEntry _ed_replace_new,l
		xdef	_ed_replace_next_entry
_ed_replace_next_entry WEntry _ed_replace_next,l
		xdef	_ed_replace_previous_entry
_ed_replace_previous_entry WEntry _ed_replace_previous,l
