; ---------------------------------------------------------------------
;	The verifier's loops: verif/main.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ver_test

		SECTION	.text,CODE

		xdef	_ver_test_entry
_ver_test_entry WEntry _ver_test,l
		XREF	_ver_direct_test
		XREF	_ver_program_test
		XREF	_ver_free_tables
		xdef	_ver_direct_test_entry
_ver_direct_test_entry WEntry _ver_direct_test,l
		xdef	_ver_program_test_entry
_ver_program_test_entry WEntry _ver_program_test,l
		xdef	_ver_free_tables_entry
_ver_free_tables_entry WEntry _ver_free_tables,l
