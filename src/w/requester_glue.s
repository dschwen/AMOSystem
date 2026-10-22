; ---------------------------------------------------------------------
;	System requesters: w/requester.cpp
; ---------------------------------------------------------------------
;
; +W.s enters with a5 = its data zone (the requester comes from any task).

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_auto_request,_w_easy_request

		SECTION	.text,CODE

		xdef	_w_auto_req,_w_easy_req
_w_auto_req	WEntry	_w_auto_request,l
_w_easy_req	WEntry	_w_easy_request,l
