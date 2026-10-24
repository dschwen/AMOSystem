; ---------------------------------------------------------------------
;	AMOSPro_Music.Lib, linked into AMOSPro: the image of the file
;	without its hunk header (main/libraries.cpp loads it from here)
; ---------------------------------------------------------------------

		SECTION	.data,DATA

		xdef	_ext_music_image
_ext_music_image
		Include	"+Music.s"
		even
