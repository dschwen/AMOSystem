; ---------------------------------------------------------------------
;	AMOSPro_Request.Lib, linked into AMOSPro: the image of the file
;	without its hunk header (main/libraries.cpp loads it from here)
; ---------------------------------------------------------------------

		SECTION	.data,DATA

		xdef	_ext_request_image
_ext_request_image
		Include	"+Request.s"
		even
