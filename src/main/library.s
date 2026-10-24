; ---------------------------------------------------------------------
;	AMOSPro.Lib, linked into AMOSPro: the image of the library file
;	without its hunk header (the sizes, "AP20", the sizes of the
;	routines, the token table, the routines, the title). The loader
;	(main/libraries.cpp) takes library 0 from here instead of from the
;	file.
; ---------------------------------------------------------------------

		SECTION	.data,DATA

		xdef	_main_library
_main_library
		Include	"+Lib.s"
		even
