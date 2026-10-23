; ---------------------------------------------------------------------
;	The programs in memory (+Verif.s Prg_*): verif/program.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go

		SECTION	.text,CODE

PrgEntry	MACRO
		XREF	_ver_prg_\1
		xdef	_ver_prg_\1_entry
_ver_prg_\1_entry WEntry _ver_prg_\1,l
		ENDM

		PrgEntry run_it
		PrgEntry test_it
		PrgEntry push
		PrgEntry pull
		PrgEntry data_save
		PrgEntry data_load
		PrgEntry data_new
		PrgEntry new_structure
		PrgEntry del_structure
		PrgEntry set_banks
		PrgEntry reset_banks
		PrgEntry new
		PrgEntry chg_ttexte
		PrgEntry load
		PrgEntry cpt_lines
		PrgEntry acc_adr
		PrgEntry save

		XREF	_ver_prg_deja_runned
		xdef	_ver_prg_deja_runned_entry
_ver_prg_deja_runned_entry WEntryF _ver_prg_deja_runned
