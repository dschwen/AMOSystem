; ---------------------------------------------------------------------
;	The editor's menu: editor/menu.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_menu_fn
		XREF	_ed_menus_end
		XREF	_ed_menu_error_fn
		XREF	_ed_menu_init
		XREF	_ed_previous_hidden
		XREF	_ed_next_hidden
		XREF	_ed_branch_amos_fn
		XREF	_ed_menu_marks
		XREF	_ed_flip_keys
		XREF	_ed_menus_editor
		XREF	_ed_menus_program
		XREF	_ed_set_autosave
		XREF	_ed_save_all
		XREF	_ed_user_menu
		XREF	_ed_add_user
		XREF	_ed_delete_user
		XREF	_ed_program_to_menu
		XREF	_ed_key_to_menu
		XREF	_ed_flip_insert
		XREF	_ed_flip_sounds
		XREF	_ed_insert_button

		SECTION	.text,CODE

		xdef	_ed_menu_entry
_ed_menu_entry WEntry _ed_menu_fn,w
		xdef	_ed_menus_end_entry
_ed_menus_end_entry WEntry _ed_menus_end,l
		xdef	_ed_menu_error_entry
_ed_menu_error_entry WEntry _ed_menu_error_fn,l
		xdef	_ed_menu_init_entry
_ed_menu_init_entry WEntry _ed_menu_init,w
		xdef	_ed_previous_hidden_entry
_ed_previous_hidden_entry WEntry _ed_previous_hidden,l
		xdef	_ed_next_hidden_entry
_ed_next_hidden_entry WEntry _ed_next_hidden,l
		xdef	_ed_branch_amos_entry
_ed_branch_amos_entry WEntry _ed_branch_amos_fn,l
		xdef	_ed_menu_marks_entry
_ed_menu_marks_entry WEntry _ed_menu_marks,l
		xdef	_ed_flip_keys_entry
_ed_flip_keys_entry WEntry _ed_flip_keys,l
		xdef	_ed_menus_editor_entry
_ed_menus_editor_entry WEntry _ed_menus_editor,l
		xdef	_ed_menus_program_entry
_ed_menus_program_entry WEntry _ed_menus_program,l
		xdef	_ed_set_autosave_entry
_ed_set_autosave_entry WEntry _ed_set_autosave,l
		xdef	_ed_save_all_entry
_ed_save_all_entry WEntry _ed_save_all,l
		xdef	_ed_user_menu_entry
_ed_user_menu_entry WEntry _ed_user_menu,l
		xdef	_ed_add_user_entry
_ed_add_user_entry WEntry _ed_add_user,l
		xdef	_ed_delete_user_entry
_ed_delete_user_entry WEntry _ed_delete_user,l
		xdef	_ed_program_to_menu_entry
_ed_program_to_menu_entry WEntry _ed_program_to_menu,l
		xdef	_ed_key_to_menu_entry
_ed_key_to_menu_entry WEntry _ed_key_to_menu,l
		xdef	_ed_flip_insert_entry
_ed_flip_insert_entry WEntry _ed_flip_insert,l
		xdef	_ed_flip_sounds_entry
_ed_flip_sounds_entry WEntry _ed_flip_sounds,l
		xdef	_ed_insert_button_entry
_ed_insert_button_entry WEntry _ed_insert_button,l

; Edt_ClearVar
		XREF	_ed_clear_var_fn
		xdef	_ed_clear_var
_ed_clear_var WEntry _ed_clear_var_fn,l
