; ---------------------------------------------------------------------
;	The small routines of AMOSPro.Lib: the errors, the constant
;	functions, the names of the banks, the registers kept for the
;	errors, the bob and icon addresses
; ---------------------------------------------------------------------
		Include	"glue.i"
		Include	"+LEqu.s"

		SECTION	.text,CODE

		RegRoutine GoError,_lib_go_error
		RegRoutine TypeMis,_lib_err_type_mis
		RegRoutine FilOO,_lib_err_fil_oo
		RegRoutine FilNO,_lib_err_fil_no
		RegRoutine FilTM,_lib_err_fil_tm
		RegRoutine EOFil,_lib_err_eo_fil
		RegRoutine IffFor2,_lib_err_iff_for2
		RegRoutine DiForm,_lib_err_di_form
		RegRoutine BkAlRes,_lib_err_bk_al_res
		RegRoutine BkNoRes,_lib_err_bk_no_res
		RegRoutine OOfMem,_lib_err_oo_f_mem
		RegRoutine StooLong,_lib_err_stoo_long
		RegRoutine FonCall,_lib_err_fon_call
		RegRoutine AdrErr,_lib_err_adr_err
		RegRoutine CantFit,_lib_err_cant_fit
		RegRoutine IffFor,_lib_err_iff_for
		RegRoutine IffCmp,_lib_err_iff_cmp
		RegRoutine FftE,_lib_err_fft_e
		RegRoutine SpErr,_lib_err_sp_err
		RegRoutine ScNOp,_lib_err_sc_n_op
		RegRoutine RainErr,_lib_err_rain_err
		RegRoutine IllScN,_lib_err_ill_sc_n
		RegRoutine BFonCall,_lib_err_b_fon_call
		RegRoutine WFonCall,_lib_err_w_fon_call
		RegRoutine EcWiErr,_lib_ec_wi_err
		RegRoutine CopErr,_lib_cop_err
		RegRoutine AdBErr,_lib_ad_b_err

; Syntax error, STOP, font not found, ERROR n
		LibCall	Syntax,_lib_syntax,0,None
		LibCall	Syntax2,_lib_syntax,0,None
		LibCall	InStop,_lib_in_stop,0,None
		LibCall	FnfE,_lib_fnf_e,0,None
		LibCall	InError,_lib_in_error,1,None

; =TRUE, =FALSE, the functions that are only a token
		LibCall	FnTrue,_lib_fn_true,0,Int
		LibCall	FnOne,_lib_fn_one,0,Int
		LibCall	FnFalse,_lib_fn_false,0,Int
		LibNop	FnInt
		LibNop	FnPSel
		LibNop	InSetBuffer
		LibNop	InNull
		LibCall	FnElse,_lib_fn_else,0,Int
		LibCall	FnNull,_lib_fn_null,0,Int

; The library's start (its number, its version), END, REM, illegal
; direct mode
		RegRoutine Init,_lib_init
		LibCall	InEnd,_lib_in_end,0,None
		InCall	InRem,_lib_in_rem
		RegRoutine RIllDir,_lib_r_ill_dir

; The registers of the program kept for the errors, MajD0, AdBob, AdIcon
		RegRoutine SaveRegs,_lib_save_regs
		RegRoutine LoadRegs,_lib_load_regs
		RegRoutine MajD0,_lib_maj_d0
		RegRoutine AdBob,_lib_ad_bob
		RegRoutine AdIcon,_lib_ad_icon

; The names of the banks
		xdef	BkSpr,BkIco,BkMus,BkAmal,BkMenu,BkDat,BkWrk,BkAsm,BkIff,Bnk_NoLoad,NHunk
BkSpr		dc.b	"Sprites "
BkIco		dc.b	"Icons   "
BkMus		dc.b	"Music   "
BkAmal		dc.b	"Amal    "
BkMenu		dc.b	"Menu    "
BkDat		dc.b	"Data    "
BkWrk		dc.b	"Work    "
BkAsm		dc.b	"Asm     "
BkIff		dc.b	"Iff     "
Bnk_NoLoad	dc.b	"Loading!"
NHunk		dc.b	"AmBk"
		dc.b	"AmSp"
		dc.b	"AmBs"
		dc.b	"AmIc"
		dc.l	0
