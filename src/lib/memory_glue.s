; ---------------------------------------------------------------------
;	Memory access: register glue for lib/memory.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		RegRoutine TransMem,_lib_trans_mem
		LibCall	InCopy,_lib_in_copy,3,None		COPY s,e TO d
		LibCall	InFill,_lib_in_fill,3,None		FILL s TO e,v
		LibCall	FnHunt,_lib_fn_hunt,3,Int		=HUNT(s TO e,a$)
		LibCall	InAReg,_lib_in_areg,2,None		AREG(n)=v
		LibCall	FnAReg,_lib_fn_areg,1,Int		=AREG(n)
		LibCall	InDReg,_lib_in_dreg,2,None		DREG(n)=v
		LibCall	FnDReg,_lib_fn_dreg,1,Int		=DREG(n)
		LibCall	InPoke,_lib_in_poke,2,None		POKE a,v
		LibCall	InDoke,_lib_in_doke,2,None		DOKE a,v
		LibCall	InLoke,_lib_in_loke,2,None		LOKE a,v
		LibCall	FnPeek,_lib_fn_peek,1,Int		=PEEK(a)
		LibCall	FnDeek,_lib_fn_deek,1,Int		=DEEK(a)
		LibCall	FnLeek,_lib_fn_leek,1,Int		=LEEK(a)
		LibCall	InPokeD,_lib_in_poke_string,2,None	POKE$ a,a$
		LibCall	FnPeekD2,_lib_fn_peek_string2,2,String	=PEEK$(a,l)
		LibCall	FnPeekD3,_lib_fn_peek_string3,3,String	=PEEK$(a,l,s$)
