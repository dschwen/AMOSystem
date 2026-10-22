; ---------------------------------------------------------------------
;	The input handler, the fake event, the cyclic tests: w/input.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_io_handler,_w_sy_send_fake_event,_w_sy_test_cyclic

		SECTION	.text,CODE

		xdef	_w_input,_w_send_fake_event,_w_test_cyclic
_w_input	WEntry	_w_io_handler,l
_w_send_fake_event WEntry _w_sy_send_fake_event,l
_w_test_cyclic	WEntry	_w_sy_test_cyclic,l
