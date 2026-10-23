; ---------------------------------------------------------------------
;	The editor's loop, its functions, programs driving it:
;	editor/loop.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_loop_fn
		XREF	_ed_function_call_fn
		XREF	_ed_zap_in
		XREF	_ed_zap_prepare
		XREF	_ed_zap_out_fn
		XREF	_ed_zap_result
		XREF	_ed_zap_function
		XREF	_ed_zap_new_config
		XREF	_ed_zap_new_line
		XREF	_ed_zap_new_line_tok

		SECTION	.text,CODE

		xdef	_ed_loop_entry
_ed_loop_entry WEntry _ed_loop_fn,l
		xdef	_ed_function_call_entry
_ed_function_call_entry WEntry _ed_function_call_fn,l
		xdef	_ed_zap_in_entry
_ed_zap_in_entry WEntry _ed_zap_in,l
		xdef	_ed_zap_prepare_entry
_ed_zap_prepare_entry WEntry _ed_zap_prepare,l
		xdef	_ed_zap_out_entry
_ed_zap_out_entry WEntry _ed_zap_out_fn,l
		xdef	_ed_zap_result_entry
_ed_zap_result_entry WEntry _ed_zap_result,l
		xdef	_ed_zap_function_entry
_ed_zap_function_entry WEntry _ed_zap_function,l
		xdef	_ed_zap_new_config_entry
_ed_zap_new_config_entry WEntry _ed_zap_new_config,l
		xdef	_ed_zap_new_line_entry
_ed_zap_new_line_entry WEntry _ed_zap_new_line,l
		xdef	_ed_zap_new_line_tok_entry
_ed_zap_new_line_tok_entry WEntry _ed_zap_new_line_tok,l
