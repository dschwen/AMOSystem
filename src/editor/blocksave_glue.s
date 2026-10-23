; ---------------------------------------------------------------------
;	The block saved and printed: editor/blocksave.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_block_save
		XREF	_ed_block_save_ascii
		XREF	_ed_print_program
		XREF	_ed_print_block
		XREF	_ed_block_limits_2
		XREF	_ed_printer_open
		XREF	_ed_printer_print

		SECTION	.text,CODE

		xdef	_ed_block_save_entry
_ed_block_save_entry WEntry _ed_block_save,l
		xdef	_ed_block_save_ascii_entry
_ed_block_save_ascii_entry WEntry _ed_block_save_ascii,l
		xdef	_ed_print_program_entry
_ed_print_program_entry WEntry _ed_print_program,l
		xdef	_ed_print_block_entry
_ed_print_block_entry WEntry _ed_print_block,l
		xdef	_ed_block_limits_2_entry
_ed_block_limits_2_entry WEntry _ed_block_limits_2,l
		xdef	_ed_printer_open_entry
_ed_printer_open_entry WEntry _ed_printer_open,l
		xdef	_ed_printer_print_entry
_ed_printer_print_entry WEntry _ed_printer_print,l
