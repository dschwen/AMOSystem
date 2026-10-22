; ---------------------------------------------------------------------
;	The Interface: register glue for lib/dialog.cpp, lib/interface.cpp
;	and lib/lineedit.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; The line editor
		RegRoutine LEd_Init,_lib_led_init
		RegRoutine LEd_Loop,_lib_led_loop
		RegRoutine LEd_New,_lib_led_new

; Channels and zones
		RegRoutine Dia_OpenChannel,_lib_dia_open_channel
		RegRoutine Dia_CloseChannel,_lib_dia_close_channel
		RegRoutine Dia_CloseChannels,_lib_dia_close_channels
		RegRoutine Dia_EffChannel,_lib_dia_eff_channel
		RegRoutine Dia_FreezeChannels,_lib_dia_freeze_channels
		RegRoutine Dia_FreezeChannel,_lib_dia_freeze_channel
		RegRoutine Dia_RunQuick,_lib_dia_run_quick
		RegRoutine Dia_Update,_lib_dia_update
		RegRoutine Dia_RunProgram,_lib_dia_run_program
		RegRoutine Dia_GetVariable,_lib_dia_get_variable
		RegRoutine Dia_GetZone,_lib_dia_get_zone
		RegRoutine Dia_GetValue,_lib_dia_get_value
		RegRoutine Dia_GetZoneAd,_lib_dia_get_zone_ad
		RegRoutine Dia_SetVFlags,_lib_dia_set_vflags
		RegRoutine Dia_GetVFlags,_lib_dia_get_vflags
		RegRoutine Dia_GetReturn,_lib_dia_get_return
		RegRoutine Dia_RScOpen,_lib_dia_rsc_open
		RegRoutine Dia_GetChannel,_lib_dia_get_channel
		RegRoutine Dia_AutoTest,_lib_dia_auto_test
		RegRoutine Dia_AutoTest2,_lib_dia_auto_test2
		RegRoutine Dia_NoMKey,_lib_dia_no_mkey
		RegRoutine Dia_ClearKey,_lib_dia_clear_key
		RegRoutine Dia_WaitMul,_lib_dia_wait_mul
		RegRoutine Dia_WarmInit,_lib_dia_warm_init
		RegRoutine Dia_GetDefault,_lib_dia_get_default
		RegRoutine Dia_GetPuzzle,_lib_dia_get_puzzle

; The buttons and sliders of the editor
		RegRoutine Bt_InitList,_lib_bt_init_list
		RegRoutine Bt_Init,_lib_bt_init
		RegRoutine Bt_End,_lib_bt_end
		RegRoutine Bt_Gere,_lib_bt_gere
		RegRoutine Sl_Init,_lib_sl_init
		RegRoutine Sl_Clic,_lib_sl_clic
		RegRoutine Sl_Draw,_lib_sl_draw

; Instructions
		LibCall	InDialogOpen2,_lib_in_dialog_open2,2,None	DIALOG OPEN c,a$
		LibCall	InDialogOpen3,_lib_in_dialog_open3,3,None
		LibCall	InDialogOpen4,_lib_in_dialog_open4,4,None
		LibCall	FnEDialog,_lib_fn_edialog,0,Int			=EDIALOG
		LibCall	InDialogClose0,_lib_in_dialog_close0,0,None	DIALOG CLOSE
		LibCall	InDialogClose1,_lib_in_dialog_close1,1,None
		LibCall	InDialogClr,_lib_in_dialog_clr,1,None		DIALOG CLR c
		LibCall	InDialogFreeze0,_lib_in_dialog_freeze0,0,None	DIALOG FREEZE
		LibCall	InDialogFreeze1,_lib_in_dialog_freeze1,1,None
		LibCall	InDialogUnFreeze0,_lib_in_dialog_unfreeze0,0,None
		LibCall	InDialogUnFreeze1,_lib_in_dialog_unfreeze1,1,None
		LibCall	InDialogUpdate2,_lib_in_dialog_update2,2,None	DIALOG UPDATE c,z...
		LibCall	InDialogUpdate3,_lib_in_dialog_update3,3,None
		LibCall	InDialogUpdate4,_lib_in_dialog_update4,4,None
		LibCall	InDialogUpdate5,_lib_in_dialog_update5,5,None
		LibCall	FnDialogRun1,_lib_fn_dialog_run1,1,Int		=DIALOG RUN(c...)
		LibCall	FnDialogRun2,_lib_fn_dialog_run2,2,Int
		LibCall	FnDialogRun4,_lib_fn_dialog_run4,4,Int
		LibCall	FnDialog,_lib_fn_dialog,1,Int			=DIALOG(c)
		LibCall	InVDialog,_lib_in_vdialog,3,None		VDIALOG(c,n)=v
		LibCall	FnVDialog,_lib_fn_vdialog,2,Int
		LibCall	InVDialogD,_lib_in_vdialog,3,None		VDIALOG$(c,n)=v$
		LibCall	FnVDialogD,_lib_fn_vdialog,2,String
		LibCall	FnRDialog2,_lib_fn_rdialog2,2,Int		=RDIALOG(c,z[,n])
		LibCall	FnRDialog3,_lib_fn_rdialog3,3,Int
		LibCall	FnRDialogD2,_lib_fn_rdialogd2,2,String		=RDIALOG$(c,z[,n])
		LibCall	FnRDialogD3,_lib_fn_rdialogd3,3,String
		LibCall	FnZDialog,_lib_fn_zdialog,3,Int			=ZDIALOG(c,x,y)
		LibCall	FnDialogBox1,_lib_fn_dialog_box1,1,Int		=DIALOG BOX(a$...)
		LibCall	FnDialogBox2,_lib_fn_dialog_box2,2,Int
		LibCall	FnDialogBox3,_lib_fn_dialog_box3,3,Int
		LibCall	FnDialogBox5,_lib_fn_dialog_box5,5,Int
		LibCall	InResourceScreenOpen,_lib_in_resource_screen_open,4,None
		LibCall	InResourceBank,_lib_in_resource_bank,1,None
		LibCall	InResourceUnpack,_lib_in_resource_unpack,3,None

; Routine records of Dia_ClearVar: a link, then the code (called with
; jsr 4(a0), all registers saved). The channels close when the program is
; cleared or reset.
		xdef	_lib_dia_on_clear
		xdef	_lib_dia_on_default
_lib_dia_on_clear
		dc.l	0
		CCall	_lib_dia_close_all
		rts
_lib_dia_on_default
		dc.l	0
		CCall	_lib_dia_close_all
		rts
