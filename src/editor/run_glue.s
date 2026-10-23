; ---------------------------------------------------------------------
;	Run, test, the returns: editor/run.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_go_monitor
		XREF	_ed_program_command
		XREF	_ed_program_reload
		XREF	_ed_window_of_name
		XREF	_ed_run_hidden
		XREF	_ed_run_hide
		XREF	_ed_run_direct
		XREF	_ed_run
		XREF	_ed_error_direct_run
		XREF	_ed_error_run_hidden
		XREF	_ed_error_test
		XREF	_ed_error_run
		XREF	_ed_error_edit
		XREF	_ed_error_line
		XREF	_ed_test
		XREF	_ed_check_13
		XREF	_ed_indent_program
		XREF	_ed_test_program_fn
		XREF	_ed_test_shown
		XREF	_ed_test_end
		XREF	_ed_test_run
		XREF	_ed_procedure_machine_code

		SECTION	.text,CODE

		xdef	_ed_go_monitor_entry
_ed_go_monitor_entry WEntry _ed_go_monitor,l
		xdef	_ed_program_command_entry
_ed_program_command_entry WEntry _ed_program_command,l
		xdef	_ed_program_reload_entry
_ed_program_reload_entry WEntry _ed_program_reload,l
		xdef	_ed_window_of_name_entry
_ed_window_of_name_entry WEntry _ed_window_of_name,l
		xdef	_ed_run_hidden_entry
_ed_run_hidden_entry WEntry _ed_run_hidden,l
		xdef	_ed_run_hide_entry
_ed_run_hide_entry WEntry _ed_run_hide,l
		xdef	_ed_run_direct_entry
_ed_run_direct_entry WEntry _ed_run_direct,l
		xdef	_ed_run_entry
_ed_run_entry WEntry _ed_run,l
		xdef	_ed_error_direct_run_entry
_ed_error_direct_run_entry WEntry _ed_error_direct_run,l
		xdef	_ed_error_run_hidden_entry
_ed_error_run_hidden_entry WEntry _ed_error_run_hidden,l
		xdef	_ed_error_test_entry
_ed_error_test_entry WEntry _ed_error_test,l
		xdef	_ed_error_run_entry
_ed_error_run_entry WEntry _ed_error_run,l
		xdef	_ed_error_edit_entry
_ed_error_edit_entry WEntry _ed_error_edit,l
		xdef	_ed_error_line_entry
_ed_error_line_entry WEntry _ed_error_line,l
		xdef	_ed_test_entry
_ed_test_entry WEntry _ed_test,l
		xdef	_ed_check_13_entry
_ed_check_13_entry WEntry _ed_check_13,l
		xdef	_ed_indent_program_entry
_ed_indent_program_entry WEntry _ed_indent_program,l
		xdef	_ed_test_program_fn_entry
_ed_test_program_fn_entry WEntry _ed_test_program_fn,l
		xdef	_ed_test_shown_entry
_ed_test_shown_entry WEntry _ed_test_shown,l
		xdef	_ed_test_end_entry
_ed_test_end_entry WEntry _ed_test_end,l
		xdef	_ed_test_run_entry
_ed_test_run_entry WEntry _ed_test_run,l
		xdef	_ed_procedure_machine_code_entry
_ed_procedure_machine_code_entry WEntry _ed_procedure_machine_code,l
