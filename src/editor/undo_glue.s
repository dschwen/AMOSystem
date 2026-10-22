; ---------------------------------------------------------------------
;	The undo buffer: editor/undo.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_undo_start,_ed_undo_line,_ed_undo_advance,_ed_undo_clear
		XREF	_ed_undo_previous,_ed_undo_next,_ed_undo_create,_ed_undo_reset,_ed_undo_free
		XREF	_ed_undo_create_all,_ed_undo_reset_all,_ed_undo_free_all,_ed_undo,_ed_redo

		SECTION	.text,CODE

		xdef	_ed_undo_start_entry,_ed_undo_line_entry,_ed_undo_advance_entry
		xdef	_ed_undo_clear_entry,_ed_undo_previous_entry,_ed_undo_next_entry
		xdef	_ed_undo_create_entry,_ed_undo_reset_entry,_ed_undo_free_entry
		xdef	_ed_undo_create_all_entry,_ed_undo_reset_all_entry,_ed_undo_free_all_entry
		xdef	_ed_undo_entry,_ed_redo_entry
; Un_Debut, Un_CLine: EQ when an entry is made
_ed_undo_start_entry WEntryF _ed_undo_start
_ed_undo_line_entry WEntryF _ed_undo_line
_ed_undo_advance_entry WEntry _ed_undo_advance,l
_ed_undo_clear_entry WEntry _ed_undo_clear,l
_ed_undo_previous_entry WEntry _ed_undo_previous,l
_ed_undo_next_entry WEntry _ed_undo_next,l
_ed_undo_create_entry WEntry _ed_undo_create,l
_ed_undo_reset_entry WEntry _ed_undo_reset,l
_ed_undo_free_entry WEntry _ed_undo_free,l
_ed_undo_create_all_entry WEntry _ed_undo_create_all,l
_ed_undo_reset_all_entry WEntry _ed_undo_reset_all,l
_ed_undo_free_all_entry WEntry _ed_undo_free_all,l
_ed_undo_entry	WEntry	_ed_undo,l
_ed_redo_entry	WEntry	_ed_redo,l
