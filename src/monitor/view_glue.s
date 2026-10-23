; ---------------------------------------------------------------------
;	The monitor's top screen: monitor/view.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_view_show
		XREF	_mon_view_next
		XREF	_mon_view_clear
		XREF	_mon_view_info
		XREF	_mon_view_sizes
		XREF	_mon_unpack
		XREF	_mon_button
		XREF	_mon_all_buttons
		XREF	_mon_stop_button
		XREF	_mon_show
		XREF	_mon_hide
		XREF	_mon_mouse_free
		XREF	_mon_recop
		XREF	_mon_move_up
		XREF	_mon_move_down
		XREF	_mon_move_left
		XREF	_mon_move_right
		XREF	_mon_change_screen
		XREF	_mon_see
		XREF	_mon_to_front
		XREF	_mon_to_back
		XREF	_mon_screen_copy
		XREF	_mon_tests_patch
		XREF	_mon_tested
		XREF	_mon_mode_buttons
		XREF	_mon_set_evaluate
		XREF	_mon_set_break
		XREF	_mon_set_help

		SECTION	.text,CODE

		xdef	_mon_view_show_entry
_mon_view_show_entry WEntry _mon_view_show,l
		xdef	_mon_view_next_entry
_mon_view_next_entry WEntry _mon_view_next,l
		xdef	_mon_view_clear_entry
_mon_view_clear_entry WEntry _mon_view_clear,l
		xdef	_mon_view_info_entry
_mon_view_info_entry WEntry _mon_view_info,l
		xdef	_mon_view_sizes_entry
_mon_view_sizes_entry WEntry _mon_view_sizes,l
		xdef	_mon_unpack_entry
_mon_unpack_entry WEntry _mon_unpack,l
		xdef	_mon_button_entry
_mon_button_entry WEntry _mon_button,l
		xdef	_mon_all_buttons_entry
_mon_all_buttons_entry WEntry _mon_all_buttons,l
		xdef	_mon_stop_button_entry
_mon_stop_button_entry WEntry _mon_stop_button,l
		xdef	_mon_show_entry
_mon_show_entry WEntry _mon_show,l
		xdef	_mon_hide_entry
_mon_hide_entry WEntry _mon_hide,l
		xdef	_mon_mouse_free_entry
_mon_mouse_free_entry WEntry _mon_mouse_free,l
		xdef	_mon_recop_entry
_mon_recop_entry WEntry _mon_recop,l
		xdef	_mon_move_up_entry
_mon_move_up_entry WEntry _mon_move_up,l
		xdef	_mon_move_down_entry
_mon_move_down_entry WEntry _mon_move_down,l
		xdef	_mon_move_left_entry
_mon_move_left_entry WEntry _mon_move_left,l
		xdef	_mon_move_right_entry
_mon_move_right_entry WEntry _mon_move_right,l
		xdef	_mon_change_screen_entry
_mon_change_screen_entry WEntry _mon_change_screen,l
		xdef	_mon_see_entry
_mon_see_entry WEntry _mon_see,l
		xdef	_mon_to_front_entry
_mon_to_front_entry WEntry _mon_to_front,l
		xdef	_mon_to_back_entry
_mon_to_back_entry WEntry _mon_to_back,l
		xdef	_mon_screen_copy_entry
_mon_screen_copy_entry WEntry _mon_screen_copy,l
		xdef	_mon_tests_patch_entry
_mon_tests_patch_entry WEntry _mon_tests_patch,l
		xdef	_mon_set_help_entry
_mon_set_help_entry WEntry _mon_set_help,l
		xdef	_mon_set_break_entry
_mon_set_break_entry WEntry _mon_set_break,l
		xdef	_mon_set_evaluate_entry
_mon_set_evaluate_entry WEntry _mon_set_evaluate,l
		xdef	_mon_mode_buttons_entry
_mon_mode_buttons_entry WEntry _mon_mode_buttons,l
		xdef	_mon_tested_entry
_mon_tested_entry WEntry _mon_tested,l
