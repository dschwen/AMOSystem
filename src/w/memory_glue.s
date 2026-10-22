; ---------------------------------------------------------------------
;	Memory and routine lists (SyCall): w/memory.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_mem_reserve,_w_sy_mem_fast_clear,_w_sy_mem_fast,_w_sy_mem_chip_clear
		XREF	_w_sy_mem_chip,_w_sy_mem_free,_w_sy_add_routine,_w_sy_add_flush_routine
		XREF	_w_sy_call_routines,_w_sy_mem_flush

		SECTION	.text,CODE

		xdef	_w_mem_reserve,_w_mem_fast_clear,_w_mem_fast,_w_mem_chip_clear
		xdef	_w_mem_chip,_w_mem_free,_w_add_routine,_w_add_flush_routine
		xdef	_w_call_routines,_w_mem_flush
_w_mem_reserve	WEntryA0 _w_sy_mem_reserve
_w_mem_fast_clear WEntryA0 _w_sy_mem_fast_clear
_w_mem_fast	WEntryA0 _w_sy_mem_fast
_w_mem_chip_clear WEntryA0 _w_sy_mem_chip_clear
_w_mem_chip	WEntryA0 _w_sy_mem_chip
_w_mem_free	WEntry	_w_sy_mem_free,l
_w_add_routine	WEntry	_w_sy_add_routine,l
_w_add_flush_routine WEntry _w_sy_add_flush_routine,l
_w_call_routines WEntry	_w_sy_call_routines,l
_w_mem_flush	WEntry	_w_sy_mem_flush,l
