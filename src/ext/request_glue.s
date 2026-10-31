; ---------------------------------------------------------------------
;	The Request extension: ext/request.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	Ext_Request_InRequestWb,_ext_request_wb,0,None
		LibCall	Ext_Request_InRequestOn,_ext_request_on,0,None
		LibCall	Ext_Request_InRequestOff,_ext_request_off,0,None

; The cold start; the warm start, at each run (ExtAdr+4)
		RegRoutine Ext_Request_Cold,_ext_request_cold
		RoutineCall _ext_request_warm_entry,_ext_request_warm
