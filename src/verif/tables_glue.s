; ---------------------------------------------------------------------
;	The verifier's loops table and labels: verif/tables.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ver_tables_free

		SECTION	.text,CODE

		xdef	_ver_tables_free_entry
_ver_tables_free_entry WEntry _ver_tables_free,l
