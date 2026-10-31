; ---------------------------------------------------------------------
;	Menus: register glue for lib/menus.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		RegRoutine OMnEff,_lib_omn_eff
		RegRoutine MnGere,_lib_mn_gere
		RegRoutine MnFind,_lib_mn_find
		RegRoutine MnIns,_lib_mn_ins
		RegRoutine MenuReset,_lib_menu_reset
		RegRoutine MnRaz,_lib_mn_raz
		RegRoutine MnClearVar,_lib_mn_clear_var
		RegRoutine MnEff,_lib_mn_eff
		RegRoutine MnObjet,_lib_mn_objet
		RegRoutine MenuKeyExplore,_lib_menu_key_explore

		LibCall	InOnMenuOn,_lib_in_on_menu_on,0,None		ON MENU ON
		LibCall	InOnMenuOff,_lib_in_on_menu_on,0,None		ON MENU OFF
		LibCall	InOnMenuDel,_lib_in_on_menu_del,0,None		ON MENU DEL
		LibCall	InMenuToBank,_lib_in_menu_to_bank,1,None	MENU TO BANK n
		LibCall	InBankToMenu,_lib_in_bank_to_menu,1,None	BANK TO MENU n
		LibCall	InMenuOn,_lib_in_menu_on,0,None			MENU ON
		LibCall	InMenuOff,_lib_in_menu_off,0,None		MENU OFF
		LibCall	InMenuMouseOn,_lib_in_menu_mouse_on,0,None	MENU MOUSE ON
		LibCall	InMenuMouseOff,_lib_in_menu_mouse_off,0,None	MENU MOUSE OFF
		LibCall	InMenuBase,_lib_in_menu_base,2,None		MENU BASE x,y
		LibCall	FnChoice0,_lib_fn_choice0,0,Int			=CHOICE
		LibCall	FnChoice1,_lib_fn_choice1,1,Int			=CHOICE(n)
		LibCall	InMenuCalc,_lib_in_menu_calc,0,None		MENU CALC
; These read their (,,,) parameters themselves (MnDim)
		LibCall	FnXMenu,_lib_fn_xmenu,0,Int			=MENU X(,,)
		LibCall	FnYMenu,_lib_fn_ymenu,0,Int			=MENU Y(,,)
		LibCall	InMenuBar,_lib_in_menu_bar,0,None
		LibCall	InMenuLine,_lib_in_menu_line,0,None
		LibCall	InMenuTline,_lib_in_menu_tline,0,None
		LibCall	InMenuMovable,_lib_in_menu_movable,0,None
		LibCall	InMenuStatic,_lib_in_menu_static,0,None
		LibCall	InMenuItemMovable,_lib_in_menu_item_movable,0,None
		LibCall	InMenuItemStatic,_lib_in_menu_item_static,0,None
		LibCall	InMenuActive,_lib_in_menu_active,0,None
		LibCall	InMenuInactive,_lib_in_menu_inactive,0,None
		LibCall	InMenuSeparate,_lib_in_menu_separate,0,None
		LibCall	InMenuLink,_lib_in_menu_link,0,None
		LibCall	InMenuCalled,_lib_in_menu_called,0,None
		LibCall	InMenuOnce,_lib_in_menu_once,0,None

; Routine records of MnClearVar: a link, then the code (called with
; jsr 4(a0), all registers saved). The menu goes when the program is
; cleared or reset; an error in a menu procedure stops the program.
		RoutineRecord _lib_menu_on_clear,_lib_menu_clear
		RoutineRecord _lib_menu_on_default,_lib_menu_clear
		RoutineRecord _lib_menu_on_error,_lib_menu_error

; uint32_t mn_call_procedure(const void *fn, const Ctx *regs,
;                            uint32_t *pile, const uint8_t *name)
;
; MnOCall: call L_MenuProcedure (fn) for the procedure name with the
; interpreter registers regs (d6-d7/a3-a6, saved by MnGere). The stack
; pointer goes to *pile (MnPile): End Proc returns to the instruction
; after the jsr with it. Returns d0: 0, or not 0 if there is no such
; procedure.
		xdef	_mn_call_procedure
_mn_call_procedure
		movem.l	d2-d7/a2-a6,-(sp)
