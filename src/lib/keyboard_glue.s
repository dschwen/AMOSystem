; ---------------------------------------------------------------------
;	Keyboard and joystick: register glue for lib/keyboard.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	FnInkey,_lib_fn_inkey,0,String		=INKEY$
		LibCall	FnScancode,_lib_fn_scancode,0,Int	=SCANCODE
		LibCall	FnScanshift,_lib_fn_scanshift,0,Int	=SCANSHIFT
		LibCall	FnKeyState,_lib_fn_key_state,1,Int	=KEY STATE(n)
		LibCall	FnKeyShift,_lib_fn_key_shift,0,Int	=KEY SHIFT
		LibCall	FnJoy,_lib_fn_joy,1,Int			=JOY(n)
		LibCall	FnJup,_lib_fn_jup,1,Int			=JUP(n)
		LibCall	FnJdown,_lib_fn_jdown,1,Int		=JDOWN(n)
		LibCall	FnJleft,_lib_fn_jleft,1,Int		=JLEFT(n)
		LibCall	FnJright,_lib_fn_jright,1,Int		=JRIGHT(n)
		LibCall	FnFire,_lib_fn_fire,1,Int		=FIRE(n)
		LibCall	InPutKey,_lib_in_put_key,1,None		PUT KEY a$
		LibCall	InClearKey,_lib_in_clear_key,0,None	CLEAR KEY
		LibCall	InKeyD,_lib_in_key,2,None		KEY$(n)=a$
		LibCall	FnKeyD,_lib_fn_key,1,String		=KEY$(n)
		LibCall	FnScan1,_lib_fn_scan1,1,String		=SCAN$(n)
		LibCall	FnScan2,_lib_fn_scan2,2,String		=SCAN$(n,shift)
