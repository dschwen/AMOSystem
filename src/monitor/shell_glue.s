; ---------------------------------------------------------------------
;	The monitor's command loop and keys: monitor/shell.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_tests,_mon_reloop,_mon_step0,_mon_step1,_mon_step2,_mon_step3
		XREF	_mon_initialise,_mon_reinit,_mon_quit,_mon_split_token

		SECTION	.text,CODE

		xdef	_mon_tests_entry
_mon_tests_entry WEntry _mon_tests,l
		xdef	_mon_reloop_entry
_mon_reloop_entry WEntry _mon_reloop,l
		xdef	_mon_step0_entry
_mon_step0_entry WEntry _mon_step0,l
		xdef	_mon_step1_entry
_mon_step1_entry WEntry _mon_step1,l
		xdef	_mon_step2_entry
_mon_step2_entry WEntry _mon_step2,l
		xdef	_mon_step3_entry
_mon_step3_entry WEntry _mon_step3,l
		xdef	_mon_initialise_entry
_mon_initialise_entry WEntry _mon_initialise,l
		xdef	_mon_reinit_entry
_mon_reinit_entry WEntry _mon_reinit,l
		xdef	_mon_quit_entry
_mon_quit_entry WEntry _mon_quit,l
; The detokeniser's hook: all registers kept
		xdef	_mon_split_hook
_mon_split_hook WEntry _mon_split_token,l

; Tt_Rien: the STOP key, tested apart
		XREF	_mon_nothing
		xdef	_mon_nothing_entry
_mon_nothing_entry WEntry _mon_nothing,l
