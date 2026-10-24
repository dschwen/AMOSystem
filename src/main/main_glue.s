; ---------------------------------------------------------------------
;	The routines of the main program (+B.s) the library calls through
;	AMOSJmps: register glue for main/*.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

; The editor and the monitor (main/modules.cpp)
		RegRoutine Edit_Load,_main_edit_load
		RegRoutine Edit_Free,_main_edit_free
		RegRoutine Mon_Load,_main_mon_load
		RegRoutine Mon_Free,_main_mon_free
		RegRoutine MemMaximum,_main_mem_maximum
		RegRoutine MemDelBanks,_main_mem_del_banks
		LibCall	InMonitor,_main_in_monitor,0,None		MONITOR
		LibCall	InKillEditor,_main_in_kill_editor,0,None	KILL EDITOR
		LibCall	InCloseEditor,_main_in_close_editor,0,None	CLOSE EDITOR

; RunErr_RunOnly / RunErr_NoEditor: where the errors of a program run
; without the editor go (Prg_JError), with d0-d1/a0-a1
		xdef	_main_run_error_run_only
		xdef	_main_run_error_no_editor
_main_run_error_run_only
		movem.l	d0-d7/a0-a2,-(sp)
		pea	(sp)
		pea	-1.w
		bra.s	RunError
_main_run_error_no_editor
		movem.l	d0-d7/a0-a2,-(sp)
		pea	(sp)
		clr.l	-(sp)
RunError	CCall	_main_run_error			never comes back

; The system (main/system.cpp)
		RegRoutine RamFast,_main_ram_fast
		RegRoutine RamChip,_main_ram_chip
		RegRoutine RamFast2,_main_ram_fast2
		RegRoutine RamChip2,_main_ram_chip2
		RegRoutine RamFree,_main_ram_free
		RegRoutine ResTempBuffer,_main_res_temp_buffer
		RegRoutine Math_Close,_main_math_close
		RegRoutine Sys_WaitMul,_main_sys_wait_mul
		RegRoutine Def_GetMessage,_main_def_get_message
		RegRoutine Sys_GetMessage,_main_sys_get_message
		RegRoutine GetMessage,_main_get_message
		RegRoutine Sys_AddPath,_main_sys_add_path
		RegRoutine Sys_UnCode,_main_sys_uncode
		RegRoutine Sys_ClearCache,_main_sys_clear_cache
		RegRoutine WOption,_main_w_option
		RegRoutine ReCop,_main_recop
		RegRoutine AskDir,_main_ask_dir
		RegRoutine AskDir2,_main_ask_dir2

; ResTempBuffer's records: the temporary buffer freed when the program is
; cleared or stops on an error (AddRoutine: a link, then the code)
		xdef	_main_temp_on_clear
		xdef	_main_temp_on_error
_main_temp_on_clear
		dc.l	0
		CCall	_main_free_temp_buffer
		rts
_main_temp_on_error
		dc.l	0
		CCall	_main_free_temp_buffer
		rts
