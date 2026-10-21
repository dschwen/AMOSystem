; ---------------------------------------------------------------------
;	Banks and lists: register glue for lib/banks.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; Lists and program banks (were in +B.s)
		RegRoutine Lst.ChipNew,_lib_lst_chip_new
		RegRoutine Lst.New,_lib_lst_new
		RegRoutine Lst.Cree,_lib_lst_cree
		RegRoutine Lst.DelAll,_lib_lst_del_all
		RegRoutine Lst.Del,_lib_lst_del
		RegRoutine Lst.Insert,_lib_lst_insert
		RegRoutine Lst.Remove,_lib_lst_remove
		RegRoutine Bnk.PrevProgram,_lib_bnk_prev_program
		RegRoutine Bnk.CurProgram,_lib_bnk_cur_program

; Bank routines of +Lib.s
		RegRoutine Bnk.GetAdr,_lib_bnk_get_adr
		RegRoutine Bnk.GetBobs,_lib_bnk_get_bobs
		RegRoutine Bnk.GetIcons,_lib_bnk_get_icons
		RegRoutine Bnk.Eff,_lib_bnk_eff
		RegRoutine Bnk.EffA0,_lib_bnk_eff_a0
		RegRoutine Bnk.EffBobA0,_lib_bnk_eff_bob_a0
		RegRoutine Bnk.EffAll,_lib_bnk_eff_all
		RegRoutine Bnk.EffTemp,_lib_bnk_eff_temp
		RegRoutine Bnk.OrAdr,_lib_bnk_or_adr
		RegRoutine Bnk.AdBob,_lib_bnk_ad_bob
		RegRoutine Bnk.AdIcon,_lib_bnk_ad_icon
		RegRoutine Bnk.ResIco,_lib_bnk_res_ico
		RegRoutine Bnk.ResBob,_lib_bnk_res_bob
		RegRoutine Bnk.Ric,_lib_bnk_ric
		RegRoutine Bnk.Schrink,_lib_bnk_schrink
		RegRoutine Bnk.InsBob,_lib_bnk_ins_bob
		RegRoutine Bnk.DelBob,_lib_bnk_del_bob
		RegRoutine Bnk.UnRev,_lib_bnk_un_rev
		RegRoutine Bnk.Reserve,_lib_bnk_reserve
		RegRoutine Bnk.GetLength,_lib_bnk_get_length
		RegRoutine Bnk.Length,_lib_bnk_length
		RegRoutine Bnk.Change,_lib_bnk_change

; Instructions and functions
		LibCall	InListBank,_lib_in_list_bank,0,None		LIST BANK
		LibCall	InErase,_lib_in_erase,1,None			ERASE n
		LibCall	InEraseTemp,_lib_in_erase_temp,0,None		ERASE TEMP
		LibCall	InEraseAll,_lib_in_erase_all,0,None		ERASE ALL
		LibCall	InBankSwap,_lib_in_bank_swap,2,None		BANK SWAP a,b
		LibCall	FnBStart,_lib_fn_bstart,1,Int			=BSTART(n)
		LibCall	FnBLength,_lib_fn_blength,1,Int			=BLENGTH(n)
		LibCall	InBGrab,_lib_in_bgrab,1,None			BGRAB n
		LibCall	InBSend,_lib_in_bsend,1,None			BSEND n
		LibCall	InInsSprite,_lib_in_ins_sprite,1,None		INS BOB n
		LibCall	InInsIcon,_lib_in_ins_icon,1,None		INS ICON n
		LibCall	InDelIcon1,_lib_in_del_icon1,1,None		DEL ICON n
		LibCall	InDelIcon2,_lib_in_del_icon2,2,None		DEL ICON a,b
		LibCall	InDelSprite1,_lib_in_del_sprite1,1,None		DEL BOB n
		LibCall	InDelSprite2,_lib_in_del_sprite2,2,None		DEL BOB a,b
		LibCall	InResWork,_lib_in_res_work,2,None		RESERVE AS WORK n,l
		LibCall	InResChipWork,_lib_in_res_chip_work,2,None	RESERVE AS CHIP WORK
		LibCall	InResData,_lib_in_res_data,2,None		RESERVE AS DATA
		LibCall	InResChipData,_lib_in_res_chip_data,2,None	RESERVE AS CHIP DATA
		LibCall	InBankSchrink,_lib_in_bank_schrink,2,None	BANK SHRINK n TO l
		LibCall	FnStart,_lib_fn_start,1,Int			=START(n)
		LibCall	FnLength,_lib_fn_length,1,Int			=LENGTH(n)
		LibCall	FnChipFree,_lib_fn_chip_free,0,Int		=CHIP FREE
		LibCall	FnFastFree,_lib_fn_fast_free,0,Int		=FAST FREE
