; ---------------------------------------------------------------------
;	Number conversions: register glue for lib/convert.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; L_LongToHex, L_LongToBin. In: d0 = number, d3 = digits (negative: no
; leading zeros), a0 = output. Out: a0 after the text.
		RegRoutine LongToHex,_lib_long_to_hex_reg
		RegRoutine LongToBin,_lib_long_to_bin_reg
		RegRoutine LongToAsc,_lib_long_to_asc
		RegRoutine LongToDec,_lib_long_to_dec
