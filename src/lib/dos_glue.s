; ---------------------------------------------------------------------
;	Files and disk errors: register glue for lib/dos.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		RegRoutine Dsk.DNom,_lib_dsk_dnom
		RegRoutine Dsk.PathIt,_lib_dsk_path_it
		RegRoutine NomDisc,_lib_nom_disc
		RegRoutine D_Open,_lib_d_open
		RegRoutine D_OpenD1,_lib_d_open_d1
		RegRoutine D_Close,_lib_d_close
		RegRoutine D_Read,_lib_d_read
		RegRoutine D_Write,_lib_d_write
		RegRoutine D_Seek,_lib_d_seek
		RegRoutine DiskError,_lib_disk_error
		RegRoutine DiskErr,_lib_disk_err

; Routine records for AMOS.library's AddRoutine/CallRoutines: a link,
; then the code (called with jsr 4(a0), all registers saved). They close
; the current file when the program stops on an error or is cleared.
		xdef	_lib_close_on_error
		xdef	_lib_close_on_clear
_lib_close_on_error
		dc.l	0
		CCall	_lib_close_current
		rts
_lib_close_on_clear
		dc.l	0
		CCall	_lib_close_current
		rts

; Saving and loading banks (lib/bankfiles.cpp)
		RegRoutine Bnk.SaveA0,_lib_bnk_save_a0
		RegRoutine Bnk.SaveAll,_lib_bnk_save_all
		RegRoutine Bnk.SaveVide,_lib_bnk_save_vide
		RegRoutine Bnk.Load,_lib_bnk_load
		LibCall	InSave2,_lib_in_save2,2,None		SAVE "file",n
		LibCall	InSave1,_lib_in_save1,1,None		SAVE "file"
		LibCall	InLoad1,_lib_in_load1,1,None		LOAD "file"
		LibCall	InLoad2,_lib_in_load2,2,None		LOAD "file",n
