; ---------------------------------------------------------------------
;	Configuration, resource, sounds: editor/config.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_set_banks_fn
		XREF	_ed_sound_play
		XREF	_ed_sounds_changed
		XREF	_ed_sounds_end
		XREF	_ed_resource_load
		XREF	_ed_resource_free
		XREF	_ed_config_set_palette
		XREF	_ed_config_load
		XREF	_ed_config_save
		XREF	_ed_config_free
		XREF	_ed_config_save_default
		XREF	_ed_config_save_default_quiet
		XREF	_ed_config_save_as
		XREF	_ed_config_load_default
		XREF	_ed_config_load_default_quiet
		XREF	_ed_config_load_as
		XREF	_ed_config_redraw
		XREF	_ed_config_save_changed
		XREF	_ed_wait_input
		XREF	_ed_text_change
		XREF	_ed_text_count
		XREF	_ed_text_free

		SECTION	.text,CODE

		xdef	_ed_set_banks_entry
_ed_set_banks_entry WEntry _ed_set_banks_fn,l
		xdef	_ed_sound_play_entry
_ed_sound_play_entry WEntry _ed_sound_play,l
		xdef	_ed_sounds_changed_entry
_ed_sounds_changed_entry WEntry _ed_sounds_changed,l
		xdef	_ed_sounds_end_entry
_ed_sounds_end_entry WEntry _ed_sounds_end,l
		xdef	_ed_resource_load_entry
_ed_resource_load_entry WEntry _ed_resource_load,l
		xdef	_ed_resource_free_entry
_ed_resource_free_entry WEntry _ed_resource_free,l
		xdef	_ed_config_set_palette_entry
_ed_config_set_palette_entry WEntry _ed_config_set_palette,l
		xdef	_ed_config_load_entry
_ed_config_load_entry WEntry _ed_config_load,l
		xdef	_ed_config_save_entry
_ed_config_save_entry WEntry _ed_config_save,l
		xdef	_ed_config_free_entry
_ed_config_free_entry WEntry _ed_config_free,l
		xdef	_ed_config_save_default_entry
_ed_config_save_default_entry WEntry _ed_config_save_default,l
		xdef	_ed_config_save_default_quiet_entry
_ed_config_save_default_quiet_entry WEntry _ed_config_save_default_quiet,l
		xdef	_ed_config_save_as_entry
_ed_config_save_as_entry WEntry _ed_config_save_as,l
		xdef	_ed_config_load_default_entry
_ed_config_load_default_entry WEntry _ed_config_load_default,l
		xdef	_ed_config_load_default_quiet_entry
_ed_config_load_default_quiet_entry WEntry _ed_config_load_default_quiet,l
		xdef	_ed_config_load_as_entry
_ed_config_load_as_entry WEntry _ed_config_load_as,l
		xdef	_ed_config_redraw_entry
_ed_config_redraw_entry WEntry _ed_config_redraw,l
		xdef	_ed_config_save_changed_entry
_ed_config_save_changed_entry WEntry _ed_config_save_changed,l
		xdef	_ed_wait_input_entry
_ed_wait_input_entry WEntry _ed_wait_input,l
		xdef	_ed_text_change_entry
_ed_text_change_entry WEntry _ed_text_change,l
		xdef	_ed_text_count_entry
_ed_text_count_entry WEntry _ed_text_count,l
		xdef	_ed_text_free_entry
_ed_text_free_entry WEntry _ed_text_free,l
