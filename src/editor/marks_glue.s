; ---------------------------------------------------------------------
;	The marks: editor/marks.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_marks_clear,_ed_auto_marks_fn,_ed_set_mark,_ed_goto_mark
		XREF	_ed_marks_to_addresses,_ed_marks_to_numbers,_ed_marks_change_fn

		SECTION	.text,CODE

		xdef	_ed_marks_clear_entry,_ed_auto_marks_entry,_ed_set_mark_entry,_ed_goto_mark_entry
		xdef	_ed_marks_to_addresses_entry,_ed_marks_to_numbers_entry,_ed_marks_change_entry
_ed_marks_clear_entry WEntry _ed_marks_clear,l
_ed_auto_marks_entry WEntry _ed_auto_marks_fn,l
_ed_set_mark_entry WEntry _ed_set_mark,l
_ed_goto_mark_entry WEntry _ed_goto_mark,l
_ed_marks_to_addresses_entry WEntry _ed_marks_to_addresses,l
_ed_marks_to_numbers_entry WEntry _ed_marks_to_numbers,l
_ed_marks_change_entry WEntry _ed_marks_change_fn,l
