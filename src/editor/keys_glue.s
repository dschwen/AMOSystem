; ---------------------------------------------------------------------
;	The editor's keys: editor/keys.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_ed_key,_ed_key_function,_ed_function_keys

		SECTION	.text,CODE

		xdef	_ed_key_entry,_ed_key_function_entry,_ed_function_keys_entry
_ed_key_entry	WEntry	_ed_key,l
_ed_key_function_entry WEntry _ed_key_function,l
_ed_function_keys_entry WEntry _ed_function_keys,l
