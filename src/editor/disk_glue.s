; ---------------------------------------------------------------------
;	Loading and saving in the editor: editor/disk.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_load_hidden_fn
		XREF	_ed_load_hidden_key
		XREF	_ed_load
		XREF	_ed_reload
		XREF	_ed_name_part
		XREF	_ed_merge
		XREF	_ed_merge_ascii
		XREF	_ed_rename
		XREF	_ed_save_as_name
		XREF	_ed_save_as
		XREF	_ed_save
		XREF	_ed_save_it
		XREF	_ed_save_program
		XREF	_ed_save_program2
		XREF	_ed_make_bak
		XREF	_ed_save_icon

		SECTION	.text,CODE

		xdef	_ed_rload_hidden_entry
_ed_rload_hidden_entry WEntry _ed_load_hidden_fn,l
		xdef	_ed_load_hidden_entry
_ed_load_hidden_entry WEntry _ed_load_hidden_key,l
		xdef	_ed_load_entry
_ed_load_entry WEntry _ed_load,l
		xdef	_ed_reload_entry
_ed_reload_entry WEntry _ed_reload,l
		xdef	_ed_name_part_entry
_ed_name_part_entry WEntry _ed_name_part,l
		xdef	_ed_merge_entry
_ed_merge_entry WEntry _ed_merge,l
		xdef	_ed_merge_ascii_entry
_ed_merge_ascii_entry WEntry _ed_merge_ascii,l
		xdef	_ed_rename_entry
_ed_rename_entry WEntry _ed_rename,l
		xdef	_ed_save_as_name_entry
_ed_save_as_name_entry WEntry _ed_save_as_name,l
		xdef	_ed_save_as_entry
_ed_save_as_entry WEntry _ed_save_as,l
		xdef	_ed_save_entry
_ed_save_entry WEntry _ed_save,l
		xdef	_ed_save_it_entry
_ed_save_it_entry WEntry _ed_save_it,l
		xdef	_ed_save_program_entry
_ed_save_program_entry WEntry _ed_save_program,l
		xdef	_ed_save_program2_entry
_ed_save_program2_entry WEntry _ed_save_program2,l
		xdef	_ed_make_bak_entry
_ed_make_bak_entry WEntry _ed_make_bak,l
		xdef	_ed_save_icon_entry
_ed_save_icon_entry WEntry _ed_save_icon,l
