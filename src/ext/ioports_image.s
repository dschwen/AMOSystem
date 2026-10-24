; ---------------------------------------------------------------------
;	AMOSPro_IOPorts.Lib, linked into AMOSPro: the image of the file
;	without its hunk header (main/libraries.cpp loads it from here)
; ---------------------------------------------------------------------

		SECTION	.data,DATA

		xdef	_ext_ioports_image
_ext_ioports_image
		Include	"+IO_Ports.s"
		even
