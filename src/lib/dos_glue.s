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

; File channels and disk instructions (lib/files.cpp)
		RegRoutine GetFile,_lib_get_file
		RegRoutine Cloa1,_lib_cloa1
		RegRoutine CloAll,_lib_clo_all
		RegRoutine GetByte,_lib_get_byte
		RegRoutine EndByte,_lib_end_byte
		RegRoutine RExist,_lib_rexist
		RegRoutine NoReq,_lib_no_req
		RegRoutine YesReq,_lib_yes_req
		RegRoutine LockGet,_lib_lock_get
		RegRoutine LockFree,_lib_lock_free
		RegRoutine CopyPath,_lib_copy_path
		LibCall	InSetInput,_lib_in_set_input,2,None		SET INPUT a,b
		LibCall	FnInputD1,_lib_fn_input1,1,String		=INPUT$(n)
		LibCall	FnInputD2,_lib_fn_input2,2,String		=INPUT$(#f,n)
		LibCall	InDirD,_lib_in_dir,1,None			DIR$=a$
		LibCall	FnDirD,_lib_fn_dir,0,String			=DIR$
		LibCall	InParent,_lib_in_parent,0,None			PARENT
		LibCall	InKill,_lib_in_kill,1,None			KILL a$
		LibCall	InRename,_lib_in_rename,2,None			RENAME a$ TO b$
		LibCall	InMkDir,_lib_in_mkdir,1,None			MKDIR a$
		LibCall	FnDrive,_lib_fn_drive,1,Int			=DRIVE(a$)
		LibCall	FnDFree,_lib_fn_dfree,0,Int			=DFREE
		LibCall	FnDiscInfo,_lib_fn_disc_info,1,String		=DISC INFO$(a$)
		LibCall	FnPort,_lib_fn_port,1,Int			=PORT(f)
		LibCall	InOpenPort,_lib_in_open_port,2,None		OPEN PORT f,a$
		LibCall	InOpenOut,_lib_in_open_out,2,None		OPEN OUT f,a$
		LibCall	InOpenIn,_lib_in_open_in,2,None			OPEN IN f,a$
		LibCall	FnLof,_lib_fn_lof,1,Int				=LOF(f)
		LibCall	InPof,_lib_in_pof,2,None			POF(f)=n
		LibCall	FnPof,_lib_fn_pof,1,Int				=POF(f)
		LibCall	FnEof,_lib_fn_eof,1,Int				=EOF(f)
		LibCall	InClose1,_lib_in_close1,1,None			CLOSE f
		LibCall	InClose0,_lib_in_close0,0,None			CLOSE
		LibCall	InOpenRandom,_lib_in_open_random,2,None		OPEN RANDOM f,a$
		LibCall	InAppend,_lib_in_append,2,None			APPEND f,a$
		LibCall	InGet,_lib_in_get,2,None			GET f,n
		LibCall	InPut,_lib_in_put,2,None			PUT f,n
		LibCall	FnExist,_lib_fn_exist,1,Int			=EXIST(a$)
		LibCall	InPLoad,_lib_in_pload,2,None			PLOAD a$,n
		LibCall	InBload,_lib_in_bload,2,None			BLOAD a$,a
		LibCall	InBSave,_lib_in_bsave,3,None			BSAVE a$,s TO e

; Closes all the channels when the program is cleared
		xdef	_lib_close_all_on_clear
_lib_close_all_on_clear
		dc.l	0
		CCall	_lib_close_all
		rts

; Libraries and devices (lib/devices.cpp). The amiga.lib routines take
; their arguments on the stack, the C way.
		LibRoutine CreatePort
		jmp	_lib_create_port
		LibRoutine DeletePort
		jmp	_lib_delete_port
		LibRoutine CreateExtIO
		jmp	_lib_create_ext_io
		LibRoutine DeleteExtIO
		jmp	_lib_delete_ext_io
		RegRoutine Lib.Close,_lib_lib_close
		RegRoutine Lib.CloseD0,_lib_lib_close_d0
		RegRoutine Lib.GetA2,_lib_lib_get_a2
		RegRoutine Dev.GetA2,_lib_dev_get_a2
		RegRoutine Dev.Open,_lib_dev_open
		RegRoutine Dev.Close,_lib_dev_close
		RegRoutine Dev.CloseA2,_lib_dev_close_a2
		RegRoutine Dev.GetIO,_lib_dev_get_io
		RegRoutine Dev.SendIO,_lib_dev_send_io
		RegRoutine Dev.DoIO,_lib_dev_do_io
		RegRoutine Dev.CheckIO,_lib_dev_check_io
		RegRoutine Dev.AbortIO,_lib_dev_abort_io
		RegRoutine Dev.Error,_lib_dev_error
		LibCall	InLibOpen,_lib_in_lib_open,3,None		LIB OPEN n,a$,v
		LibCall	InLibClose0,_lib_in_lib_close0,0,None		LIB CLOSE
		LibCall	InLibClose1,_lib_in_lib_close1,1,None		LIB CLOSE n
		LibCall	FnLibBase,_lib_fn_lib_base,1,Int		=LIB BASE(n)
		LibCall	InDevOpen,_lib_in_dev_open,5,None		DEV OPEN n,a$,l,u,f
		LibCall	InDevClose0,_lib_in_dev_close0,0,None		DEV CLOSE
		LibCall	InDevClose1,_lib_in_dev_close1,1,None		DEV CLOSE n
		LibCall	FnDevBase,_lib_fn_dev_base,1,Int		=DEV BASE(n)
		LibCall	InDevDo,_lib_in_dev_do,2,None			DEV DO n,c
		LibCall	InDevSend,_lib_in_dev_send,2,None		DEV SEND n,c
		LibCall	InDevAbort,_lib_in_dev_abort,1,None		DEV ABORT n
		LibCall	FnDevCheck,_lib_fn_dev_check,1,Int		=DEV CHECK(n)
		LibCall	InExec,_lib_in_exec,1,None			EXEC a$

		xdef	_lib_close_libs_on_clear
		xdef	_lib_close_devs_on_clear
_lib_close_libs_on_clear
		dc.l	0
		CCall	_lib_close_libs
		rts
_lib_close_devs_on_clear
		dc.l	0
		CCall	_lib_close_devs
		rts

; Directories (lib/dirs.cpp)
		RegRoutine FillAll,_lib_fill_all
		RegRoutine FillGet,_lib_fill_get
		RegRoutine FillDev,_lib_fill_dev
		RegRoutine FillFirst,_lib_fill_first
		RegRoutine FillNxt,_lib_fill_nxt
		RegRoutine FillFFree,_lib_fill_ffree
		RegRoutine FillSort,_lib_fill_sort
		RegRoutine FillFPoke,_lib_fill_fpoke
		RegRoutine FfComp,_lib_ff_comp
		RegRoutine FfComp2,_lib_ff_comp2
		RegRoutine FillFFind,_lib_fill_ffind
		RegRoutine NomDir,_lib_nom_dir
		RegRoutine Joker,_lib_joker
		LibCall	InDir0,_lib_in_dir0,0,None			DIR
		LibCall	InLDir0,_lib_in_ldir0,0,None			LDIR
		LibCall	InDir1,_lib_in_dir1,1,None			DIR a$
		LibCall	InLDir1,_lib_in_ldir1,1,None			LDIR a$
		LibCall	InDirW0,_lib_in_dirw0,0,None			DIR/W
		LibCall	InLDirW0,_lib_in_ldirw0,0,None			LDIR/W
		LibCall	InDirW1,_lib_in_dirw1,1,None			DIR/W a$
		LibCall	InLDirW1,_lib_in_ldirw1,1,None			LDIR/W a$
		LibCall	InSetDir1,_lib_in_set_dir1,1,None		SET DIR n
		LibCall	InSetDir2,_lib_in_set_dir2,2,None		SET DIR n,a$
		LibCall	FnDirFirst,_lib_fn_dir_first,1,String		=DIR FIRST$(a$)
		LibCall	FnFillNext,_lib_fn_fill_next,0,String		=DIR NEXT$
		LibCall	FnDevFirst,_lib_fn_dev_first,1,String		=DEV FIRST$(a$)
		LibCall	FnPrgFirst,_lib_fn_dev_first,1,String		=PRG FIRST$(a$)

		xdef	_lib_fill_free_on_clear
_lib_fill_free_on_clear
		dc.l	0
		CCall	_lib_fill_free
		rts
