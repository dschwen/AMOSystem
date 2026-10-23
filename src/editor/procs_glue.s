; ---------------------------------------------------------------------
;	Opening and closing procedures: editor/procs.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_procs_open,_ed_procs_close,_ed_procs,_ed_proc_open,_ed_clear_error_proc

		SECTION	.text,CODE

		xdef	_ed_procs_open_entry,_ed_procs_close_entry,_ed_procs_entry,_ed_proc_open_entry
		xdef	_ed_clear_error_proc_entry
_ed_procs_open_entry WEntry _ed_procs_open,l
_ed_procs_close_entry WEntry _ed_procs_close,l
_ed_procs_entry	WEntry	_ed_procs,l
_ed_proc_open_entry WEntry _ed_proc_open,l
_ed_clear_error_proc_entry WEntry _ed_clear_error_proc,l
