; ---------------------------------------------------------------------
;	String helpers and printing: lib/textout.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		RegRoutine ChVerBuf,_lib_ch_ver_buf
		RegRoutine ChVerBuf2,_lib_ch_ver_buf2
		RegRoutine Str2Chaine,_lib_str2_chaine
		RegRoutine Mes2Chaine,_lib_mes2_chaine
		RegRoutine MCSuit,_lib_mc_suit
		RegRoutine Set_HiChaine,_lib_set_hi_chaine
		RegRoutine Ret_ChVide,_lib_ret_ch_vide
		RegRoutine A0ToChaine,_lib_a0_to_chaine
		RegRoutine FinBin,_lib_fin_bin
		RegRoutine ImpChaine,_lib_imp_chaine
		RegRoutine PRT_Open,_lib_prt_open
		RegRoutine PRT_Close,_lib_prt_close
		RegRoutine PRT_Print,_lib_prt_print

; The routine record that closes the printer at ClearVar: a link, code
		XREF	_lib_prt_close_fn
		RoutineRecord _lib_prt_close_record,_lib_prt_close_fn
