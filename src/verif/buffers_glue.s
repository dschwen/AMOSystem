; ---------------------------------------------------------------------
;	The interpreter's buffers, includes and equates (+Verif.s):
;	verif/buffers.cpp, verif/includes.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go

		SECTION	.text,CODE

VerEntry	MACRO
		XREF	_ver_\1
		xdef	_ver_\1_entry
_ver_\1_entry	WEntry _ver_\1,l
		ENDM

		VerEntry clear_variables
		VerEntry clear_buffers
		VerEntry stack_reserve
		VerEntry direct_variables
		VerEntry resize_variables
		VerEntry resize_names
		VerEntry includes_clear
		VerEntry includes_address

; The routine record of Equ_Free for the memory flush routines: a link,
; then the code
		XREF	_ver_equ_free
		xdef	_ver_equ_flush,_ver_equ_free_entry
_ver_equ_flush	dc.l	0
_ver_equ_free_entry WEntry _ver_equ_free,l
