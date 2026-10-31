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

; Ed_SMark1-9, Ed_GMark1-9
		XREF	_ed_set_mark_1,_ed_goto_mark_1
		xdef	_ed_set_mark_1_entry,_ed_goto_mark_1_entry
_ed_set_mark_1_entry WEntry _ed_set_mark_1,l
_ed_goto_mark_1_entry WEntry _ed_goto_mark_1,l
		XREF	_ed_set_mark_2,_ed_goto_mark_2
		xdef	_ed_set_mark_2_entry,_ed_goto_mark_2_entry
_ed_set_mark_2_entry WEntry _ed_set_mark_2,l
_ed_goto_mark_2_entry WEntry _ed_goto_mark_2,l
		XREF	_ed_set_mark_3,_ed_goto_mark_3
		xdef	_ed_set_mark_3_entry,_ed_goto_mark_3_entry
_ed_set_mark_3_entry WEntry _ed_set_mark_3,l
_ed_goto_mark_3_entry WEntry _ed_goto_mark_3,l
		XREF	_ed_set_mark_4,_ed_goto_mark_4
		xdef	_ed_set_mark_4_entry,_ed_goto_mark_4_entry
_ed_set_mark_4_entry WEntry _ed_set_mark_4,l
_ed_goto_mark_4_entry WEntry _ed_goto_mark_4,l
		XREF	_ed_set_mark_5,_ed_goto_mark_5
		xdef	_ed_set_mark_5_entry,_ed_goto_mark_5_entry
_ed_set_mark_5_entry WEntry _ed_set_mark_5,l
_ed_goto_mark_5_entry WEntry _ed_goto_mark_5,l
		XREF	_ed_set_mark_6,_ed_goto_mark_6
		xdef	_ed_set_mark_6_entry,_ed_goto_mark_6_entry
_ed_set_mark_6_entry WEntry _ed_set_mark_6,l
_ed_goto_mark_6_entry WEntry _ed_goto_mark_6,l
		XREF	_ed_set_mark_7,_ed_goto_mark_7
		xdef	_ed_set_mark_7_entry,_ed_goto_mark_7_entry
_ed_set_mark_7_entry WEntry _ed_set_mark_7,l
_ed_goto_mark_7_entry WEntry _ed_goto_mark_7,l
		XREF	_ed_set_mark_8,_ed_goto_mark_8
		xdef	_ed_set_mark_8_entry,_ed_goto_mark_8_entry
_ed_set_mark_8_entry WEntry _ed_set_mark_8,l
_ed_goto_mark_8_entry WEntry _ed_goto_mark_8,l
		XREF	_ed_set_mark_9,_ed_goto_mark_9
		xdef	_ed_set_mark_9_entry,_ed_goto_mark_9_entry
_ed_set_mark_9_entry WEntry _ed_set_mark_9,l
_ed_goto_mark_9_entry WEntry _ed_goto_mark_9,l
