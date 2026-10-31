; ---------------------------------------------------------------------
;	Keyboard (SyCall): w/keyboard.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_inkey,_w_sy_key_shift,_w_sy_key_state,_w_sy_put_key
		XREF	_w_sy_set_function_key,_w_sy_get_function_key,_w_sy_key_speed,_w_sy_clear_key

		SECTION	.text,CODE

		xdef	_w_inkey,_w_key_shift,_w_key_state,_w_put_key
		xdef	_w_set_function_key,_w_get_function_key,_w_key_speed
_w_inkey	WEntry	_w_sy_inkey,l
_w_key_shift	WEntry	_w_sy_key_shift,l
_w_key_state	WEntry	_w_sy_key_state,l
_w_put_key	WEntry	_w_sy_put_key,l
_w_set_function_key WEntry _w_sy_set_function_key,l
_w_get_function_key WEntry _w_sy_get_function_key,l
_w_key_speed	WEntry	_w_sy_key_speed,l
		xdef	_w_clear_key
_w_clear_key	WEntry	_w_sy_clear_key,l

; ClKWait (the condition codes from its answer), TGetDisc, ClKeyM/WMemCheck
		XREF	_w_sy_key_waiting,_w_sy_get_disc,_w_sy_nothing
		xdef	_w_key_waiting_entry,_w_get_disc_entry,_w_nothing_entry
_w_key_waiting_entry WEntryF _w_sy_key_waiting
_w_get_disc_entry WEntry _w_sy_get_disc,l
_w_nothing_entry WEntry _w_sy_nothing,l
