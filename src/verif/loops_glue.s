; ---------------------------------------------------------------------
;	The verifier's loops and tests, the routines of pass 2: their
;	addresses go in TablA (verif/loops.cpp)
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ver_for_two_c
		XREF	_ver_repeat_two_c
		XREF	_ver_while_two_c
		XREF	_ver_do_two_c
		XREF	_ver_exit_two_c
		XREF	_ver_if_two_c
		XREF	_ver_if_then_two_c
		XREF	_ver_else_if_two_c
		XREF	_ver_else_two_c
		XREF	_ver_else_then_two_c
		XREF	_ver_goto_two_c
		XREF	_ver_on_goto_two_c
		XREF	_ver_next_alone_c
		XREF	_ver_until_alone_c
		XREF	_ver_wend_alone_c
		XREF	_ver_loop_alone_c
		XREF	_ver_else_alone_c
		XREF	_ver_end_if_alone_c

		SECTION	.text,CODE

		xdef	_ver_for_two
_ver_for_two WEntry _ver_for_two_c,l
		xdef	_ver_repeat_two
_ver_repeat_two WEntry _ver_repeat_two_c,l
		xdef	_ver_while_two
_ver_while_two WEntry _ver_while_two_c,l
		xdef	_ver_do_two
_ver_do_two WEntry _ver_do_two_c,l
		xdef	_ver_exit_two
_ver_exit_two WEntry _ver_exit_two_c,l
		xdef	_ver_if_two
_ver_if_two WEntry _ver_if_two_c,l
		xdef	_ver_if_then_two
_ver_if_then_two WEntry _ver_if_then_two_c,l
		xdef	_ver_else_if_two
_ver_else_if_two WEntry _ver_else_if_two_c,l
		xdef	_ver_else_two
_ver_else_two WEntry _ver_else_two_c,l
		xdef	_ver_else_then_two
_ver_else_then_two WEntry _ver_else_then_two_c,l
		xdef	_ver_goto_two
_ver_goto_two WEntry _ver_goto_two_c,l
		xdef	_ver_on_goto_two
_ver_on_goto_two WEntry _ver_on_goto_two_c,l
		xdef	_ver_next_alone
_ver_next_alone WEntry _ver_next_alone_c,l
		xdef	_ver_until_alone
_ver_until_alone WEntry _ver_until_alone_c,l
		xdef	_ver_wend_alone
_ver_wend_alone WEntry _ver_wend_alone_c,l
		xdef	_ver_loop_alone
_ver_loop_alone WEntry _ver_loop_alone_c,l
		xdef	_ver_else_alone
_ver_else_alone WEntry _ver_else_alone_c,l
		xdef	_ver_end_if_alone
_ver_end_if_alone WEntry _ver_end_if_alone_c,l
