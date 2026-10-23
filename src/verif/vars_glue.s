; ---------------------------------------------------------------------
;	The verifier's variables and relocation: verif/vars.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ver_locals
		XREF	_ver_globals
		XREF	_ver_reloc_free

		SECTION	.text,CODE

		xdef	_ver_locals_entry
_ver_locals_entry WEntry _ver_locals,l
		xdef	_ver_globals_entry
_ver_globals_entry WEntry _ver_globals,l
		xdef	_ver_reloc_free_entry
_ver_reloc_free_entry WEntry _ver_reloc_free,l
