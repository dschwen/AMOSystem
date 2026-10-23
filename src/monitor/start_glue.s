; ---------------------------------------------------------------------
;	The monitor started and stopped: monitor/start.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_cold_start
		XREF	_mon_free

		SECTION	.text,CODE

		xdef	_mon_cold_start_entry
_mon_cold_start_entry WEntry _mon_cold_start,l
		xdef	_mon_free_entry
_mon_free_entry WEntry _mon_free,l
