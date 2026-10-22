; ---------------------------------------------------------------------
;	Editing the current line: editor/lines.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_current_line,_ed_letter,_ed_insert_text,_ed_delete_text,_ed_word_left
		XREF	_ed_word_right,_ed_type_key,_ed_delete_key,_ed_backspace,_ed_clear_line
		XREF	_ed_delete_to_end,_ed_delete_to_start,_ed_delete_word,_ed_delete_word_left
		XREF	_ed_tab,_ed_untab

		SECTION	.text,CODE

		xdef	_ed_current_line_entry,_ed_letter_entry,_ed_insert_text_entry,_ed_delete_text_entry
		xdef	_ed_word_left_entry,_ed_word_right_entry,_ed_type_key_entry,_ed_delete_key_entry
		xdef	_ed_backspace_entry,_ed_clear_line_entry,_ed_delete_to_end_entry,_ed_delete_to_start_entry
		xdef	_ed_delete_word_entry,_ed_delete_word_left_entry,_ed_tab_entry,_ed_untab_entry
; Ed_LCourant: NE if the line cannot be edited; Une_Lettre: NE for a letter
_ed_current_line_entry WEntryF _ed_current_line
_ed_letter_entry WEntryF _ed_letter
_ed_insert_text_entry WEntry _ed_insert_text,l
_ed_delete_text_entry WEntry _ed_delete_text,l
_ed_word_left_entry WEntry _ed_word_left,l
_ed_word_right_entry WEntry _ed_word_right,l
_ed_type_key_entry WEntry _ed_type_key,l
_ed_delete_key_entry WEntry _ed_delete_key,l
_ed_backspace_entry WEntry _ed_backspace,l
_ed_clear_line_entry WEntry _ed_clear_line,l
_ed_delete_to_end_entry WEntry _ed_delete_to_end,l
_ed_delete_to_start_entry WEntry _ed_delete_to_start,l
_ed_delete_word_entry WEntry _ed_delete_word,l
_ed_delete_word_left_entry WEntry _ed_delete_word_left,l
_ed_tab_entry	WEntry	_ed_tab,l
_ed_untab_entry	WEntry	_ed_untab,l
