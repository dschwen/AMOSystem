; ---------------------------------------------------------------------
;	String functions: register glue for lib/strings.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	FnUpper,_lib_fn_upper,1,String		=UPPER$(a$)
		LibCall	FnLower,_lib_fn_lower,1,String		=LOWER$(a$)
		LibCall	FnSpace,_lib_fn_space,1,String		=SPACE$(n)
		LibCall	FnString,_lib_fn_string,2,String	=STRING$(a$,n)
