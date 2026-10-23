; ---------------------------------------------------------------------
;	The monitor stopped on an instruction: monitor/step.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_step_show
		XREF	_mon_test_error

		SECTION	.text,CODE

		xdef	_mon_step_show_entry
_mon_step_show_entry WEntry _mon_step_show,l
		xdef	_mon_test_error_entry
_mon_test_error_entry WEntry _mon_test_error,l
