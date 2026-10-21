; ---------------------------------------------------------------------
;	Text window instructions: register glue for lib/windows.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	InLocate,_lib_in_locate,2,None		LOCATE x,y
