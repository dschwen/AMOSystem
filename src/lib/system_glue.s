; ---------------------------------------------------------------------
;	Errors, random numbers, timers, waits: glue for lib/system.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	FnErrn,_lib_fn_errn,0,Int		=ERRN
		LibCall	FnErrD,_lib_fn_err,1,String		=ERR$(n)
		LibCall	FnStrE,_lib_fn_str_int,1,String		=STR$(n), integer
		LibCall	InRadian,_lib_in_radian,0,None		RADIAN
		LibCall	InDegree,_lib_in_degree,0,None		DEGREE
		LibCall	InFix,_lib_in_fix,1,None		FIX n
		LibCall	FnRnd,_lib_fn_rnd,1,Int			=RND(n)
		RegRoutine Mulu32,_lib_mulu32
		LibCall	InRandom,_lib_in_randomize,1,None	RANDOMIZE n
		LibCall	InTimer,_lib_in_timer,1,None		TIMER=n
		LibCall	FnTimer,_lib_fn_timer,0,Int		=TIMER
		LibCall	FnPi,_lib_fn_pi,0,Double		=PI#
		LibCall	InWait,_lib_in_wait,1,None		WAIT n
		RegRoutine WaitRout,_lib_wait_rout
		LibCall	InMWait,_lib_in_multi_wait,0,None	MULTI WAIT
		LibCall	InWtVbl,_lib_in_wait_vbl,0,None		WAIT VBL
		LibCall	InWtKy,_lib_in_wait_key,0,None		WAIT KEY
		LibCall	InKeySpeed,_lib_in_key_speed,2,None	KEY SPEED d,s
		LibCall	FnParamS,_lib_fn_param_string,0,String	=PARAM$
		LibCall	FnParamE,_lib_fn_param_int,0,Int	=PARAM
