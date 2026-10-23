; ---------------------------------------------------------------------
;	Lists, dialogues, macros: editor/macros.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_mem_reserve
		XREF	_ed_mem_free
		XREF	_ed_list_new
		XREF	_ed_list_delete_all
		XREF	_ed_list_delete
		XREF	_ed_dialogues_open
		XREF	_ed_dialogues_close
		XREF	_ed_strings_reset
		XREF	_ed_string_new
		XREF	_ed_string_add
		XREF	_ed_dialogue_fn
		XREF	_ed_do_dialogue
		XREF	_ed_macro_load_default
		XREF	_ed_macro_load_as
		XREF	_ed_macro_save_default
		XREF	_ed_macro_save_as
		XREF	_ed_macro_load
		XREF	_ed_macro_erase
		XREF	_ed_macro_erase_all
		XREF	_ed_macro_record
		XREF	_ed_macro_stop_fn
		XREF	_ed_macros_free

		SECTION	.text,CODE

		xdef	_ed_mem_reserve_entry
_ed_mem_reserve_entry WEntryA0 _ed_mem_reserve
		xdef	_ed_mem_free_entry
_ed_mem_free_entry WEntry _ed_mem_free,l
		xdef	_ed_list_new_entry
_ed_list_new_entry WEntryF _ed_list_new
		xdef	_ed_list_delete_all_entry
_ed_list_delete_all_entry WEntry _ed_list_delete_all,l
		xdef	_ed_list_delete_entry
_ed_list_delete_entry WEntry _ed_list_delete,l
		xdef	_ed_dialogues_open_entry
_ed_dialogues_open_entry WEntry _ed_dialogues_open,l
		xdef	_ed_dialogues_close_entry
_ed_dialogues_close_entry WEntry _ed_dialogues_close,l
		xdef	_ed_strings_reset_entry
_ed_strings_reset_entry WEntry _ed_strings_reset,l
		xdef	_ed_string_new_entry
_ed_string_new_entry WEntry _ed_string_new,l
		xdef	_ed_string_add_entry
_ed_string_add_entry WEntry _ed_string_add,l
		xdef	_ed_dialogue_entry
_ed_dialogue_entry WEntry _ed_dialogue_fn,w
		xdef	_ed_do_dialogue_entry
_ed_do_dialogue_entry WEntry _ed_do_dialogue,w
		xdef	_ed_macro_load_default_entry
_ed_macro_load_default_entry WEntry _ed_macro_load_default,l
		xdef	_ed_macro_load_as_entry
_ed_macro_load_as_entry WEntry _ed_macro_load_as,l
		xdef	_ed_macro_save_default_entry
_ed_macro_save_default_entry WEntry _ed_macro_save_default,l
		xdef	_ed_macro_save_as_entry
_ed_macro_save_as_entry WEntry _ed_macro_save_as,l
		xdef	_ed_macro_load_entry
_ed_macro_load_entry WEntry _ed_macro_load,l
		xdef	_ed_macro_erase_entry
_ed_macro_erase_entry WEntry _ed_macro_erase,l
		xdef	_ed_macro_erase_all_entry
_ed_macro_erase_all_entry WEntry _ed_macro_erase_all,l
		xdef	_ed_macro_record_entry
_ed_macro_record_entry WEntry _ed_macro_record,l
		xdef	_ed_macro_stop_entry
_ed_macro_stop_entry WEntry _ed_macro_stop_fn,l
		xdef	_ed_macros_free_entry
_ed_macros_free_entry WEntry _ed_macros_free,l
