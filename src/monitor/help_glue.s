; ---------------------------------------------------------------------
;	The monitor's help: monitor/help.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_free_map

		SECTION	.text,CODE

		xdef	_mon_free_map_entry
_mon_free_map_entry WEntry _mon_free_map,l
