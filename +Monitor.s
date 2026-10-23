;_____________________________________________________________________________
;.............................................................................
;...................................................................2222222...
;................................................................22222222220..
;...................................................222........222222.....222.
;..............................................2202222222222..22000...........
;..................................22000.....20222222222200000200002..........
;.................................2002202...2222200222.220000000200000000022..
;....................220002......22222200..2200002.......2200000...20000000000
;....................22222202....2220000022200000..........200002........20000
;.....200000.........2222200000222200220000000002..........200002........20000
;.....00222202........2220022000000002200002000002........2000002000020000000.
;....2222200000.......220002200000002.2000000000000222222000000..2000000002...
;....220000200002......20000..200002..220000200000000000000002.......22.......
;...2220002.2200002....220002...22.....200002..0000000000002..................
;...220000..222000002...20000..........200000......2222.......................
;...000000000000000000..200000..........00002.................................
;..220000000022020000002.200002.........22......._____________________________
;..0000002........2000000220022.................|
;.200000............2002........................| MONITOR
;.200002........................................|
;220002.........................................|_____________________________
;_____________________________________________________________________________
;
;  Published under the MIT Licence
;
;  Copyright (c) 1992 Europress Software
;  Copyright (c) 2020 Francois Lionet
;
;  Permission is hereby granted, free of charge, to any person
;  obtaining a copy of this software and associated documentation
;  files (the "Software"), to deal in the Software without
;  restriction, including without limitation the rights to use,
;  copy, modify, merge, publish, distribute, sublicense, and/or
;  sell copies of the Software, and to permit persons to whom the
;  Software is furnished to do so, subject to the following
;  conditions:
;
;  The above copyright notice and this permission notice shall be
;  included in all copies or substantial portions of the Software.
;
;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
;  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
;  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
;  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
;  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;
;
;_____________________________________________________________________________
		Include	"+Debug.s"
		IFEQ	Debug=2
		Include "+AMOS_Includes.s"
		Include "+Version.s"
		ENDC
;_____________________________________________________________________________


; Branchements internes au moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		XREF	_amos_bridge
		XREF	_mon_view_show_entry
		XREF	_mon_view_next_entry
		XREF	_mon_view_clear_entry
		XREF	_mon_view_info_entry
		XREF	_mon_view_sizes_entry
		XREF	_mon_unpack_entry
		XREF	_mon_button_entry
		XREF	_mon_all_buttons_entry
		XREF	_mon_stop_button_entry
		XREF	_mon_show_entry
		XREF	_mon_hide_entry
		XREF	_mon_mouse_free_entry
		XREF	_mon_recop_entry
		XREF	_mon_move_up_entry
		XREF	_mon_move_down_entry
		XREF	_mon_move_left_entry
		XREF	_mon_move_right_entry
		XREF	_mon_change_screen_entry
		XREF	_mon_see_entry
		XREF	_mon_to_front_entry
		XREF	_mon_to_back_entry
		XREF	_mon_screen_copy_entry
		XREF	_mon_tests_patch_entry
		XREF	_mon_free_map_entry
		XREF	_mon_set_help_entry
		XREF	_mon_set_break_entry
		XREF	_mon_set_evaluate_entry
		XREF	_mon_mode_buttons_entry
		XREF	_mon_no_init_entry
		XREF	_mon_tested_entry
		XREF	_mon_step_show_entry
		XREF	_mon_cold_start_entry
		XREF	_mon_free_entry
		XREF	_mon_test_error_entry
		XREF	_mon_centre_click_entry
		XREF	_mon_slider_horizontal_click_entry
		XREF	_mon_slider_vertical_click_entry
		XREF	_mon_listing_centre_entry
		XREF	_mon_listing_show_entry
		XREF	_mon_listing_new_entry
		XREF	_mon_listing_untokenise_entry
		XREF	_mon_separation_drag_entry
		XREF	_mon_listing_screen_entry
		XREF	_mon_listing_size_entry
		XREF	_mon_listing_click_entry
		XREF	_mon_choose_entry
		XREF	_mon_mouse_position_entry
		XREF	_mon_split_entry
		XREF	_mon_info_centre_entry
		XREF	_mon_info_click_entry
		XREF	_mon_info_show_line_entry
		XREF	_mon_info_count_lines_entry
		XREF	_mon_info_show_all_entry
		XREF	_mon_info_begin_entry
		XREF	_mon_info_print_entry
		XREF	_mon_info_end_entry
		XREF	_mon_info_delete_entry
		XREF	_mon_info_insert_entry
		XREF	_mon_info_slider_click_entry
		XREF	_mon_info_slider_entry
		XREF	_mon_alert_entry
		XREF	_mon_alert_delete_entry
		XREF	_mon_delete_show_entry
		XREF	_mon_activate_info_entry
		XREF	_mon_activate_text_entry
		XREF	_mon_activate_entry
		XREF	_mon_message_entry
		XREF	_mon_wait_release_entry
		XREF	_mon_banks_monitor_entry
		XREF	_mon_banks_program_entry
MDebut		dc.l	In_Editor-MDebut	0- Depuis l'editeur
		dc.l	In_Program-MDebut	1- Depuis le programme
		dc.l	MonitorChr-MDebut	2- Entree du moniteur
		dc.l	0
		dc.l	_amos_bridge		the C++ contexts (+B.s Program_Load)

; Equates internes au moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ttt_MinTy	equ	8
Ttt_MaxTy	equ	16

* NumÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ro des ecrans
Tt_Ecran1	equ	EcEdit
Tt_Ecran2	equ	EcFonc
E1_Sx		equ	640
E1_Sy		equ	116
E2_Sx		equ	640
E2_MinY		equ	128

; Position de base des boutons
BtX		equ	384
BtY		equ	35
RedX		equ	16
RedY		equ	9
RedTX		equ	320
RedTY		equ	100
BoutInfos	equ	12
NBoutons	equ	16

Tjsr	MACRO
	jsr	\1
	move.l	Mon_Base(a5),a4
	ENDM

;					------------------------------------
;--------------------------------------    Zone de donnees spÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½cifique TUTOR
;					------------------------------------
		Include	"+MEqu.s"



; ___________________________________________________________________
;
;	BOUCLE PRINCIPALE
; ___________________________________________________________________
;

;	Entree du moniteur par l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In_Editor
	move.l	Edt_Runned(a5),a3	Le programme courant
	move.l	Edt_Prg(a3),a3		Structure programme
	bsr	Tt_ColdStart
	bne	.OMem
	clr.w	Tt_Run(a4)
	bra.s	Tt_Tests
.OMem	rts

; 	Entree du moniteur par le programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In_Program
	movem.l	a3-a6/d6/d7,-(sp)
	move.l	Mon_Base(a5),d0
	bne.s	.DejaLa
	move.l	Prg_Runned(a5),a3
	bsr	Tt_ColdStart
	bne.s	.Out
	move.w	#1,Tt_Init(a4)
	move.w	#1,Tt_Run(a4)
	lea	Tt_Error(pc),a0		Erreur>>> se branche au moniteur
	move.l	a0,Prg_JError(a5)
.Ok	moveq	#0,d0
.Out	movem.l	(sp)+,a3-a6/d6/d7
	rts
.DejaLa	move.l	d0,a0
	addq.w	#1,Step_Stop(a0)
	bra.s	.Ok

;	Boucle de tests du moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Tests
	move.l	Mon_Base(a5),a4
	clr.w	Tt_Active(a4)
Tt_ReLoop
	bsr	Tt_TtBanks
	move.l	Tt_Program(a4),a3
	bsr	View_Act
	clr.w	Tt_Accel(a4)
	tst.w	Tt_Active(a4)
	beq.s	Tt_Loop
.loop	bsr	Tt_Multi
	bsr	Tt_MBout
	btst	#0,d1
	beq.s	.Out
	SyCall	GetZone
	cmp.w	#Tt_Ecran1,d1
	bne.s	.Out
	swap	d1
	cmp.w	Tt_Active(a4),d1
	beq.s	.loop
.Out	move.w	Tt_Active(a4),d0
	clr.w	Tt_Active(a4)
	moveq	#0,d1
	bsr	ABouton
Tt_Loop
; Attente multitache
	bsr	Tt_Multi
; Gestion des zones actives!
	bsr	Tt_MBout
	move.w	d1,Tt_MKey(a4)
	and.w	#$03,d1			Bouton de gauche--> normal
	beq	Tt_ReLoop

	bsr	Ttt_InfAct
	bsr	Ttt_AlertDel		Enleve les alertes
	bsr	View_Act

	SyCall	GetZone
	cmp.w	#Tt_Ecran2,d1
	beq	Ttt_E2
	cmp.w	#Tt_Ecran1,d1
	bne	Tt_DBL
	swap 	d1
	tst.w	d1
	beq	Tt_DBL
	move.w	d1,d0
; Touches HELP/BREAK/EVAL?
	cmp.w	#14,d0
	bcc.s	.Skip
	cmp.w	Tt_Active(a4),d1
	beq.s	.Skip
; Active la touche
	tst.w	Tt_Active(a4)
	bne	Tt_ReLoop
	move.w	d1,Tt_Active(a4)
	moveq	#1,d1
	bsr	ABouton
; Appel des fonctions
.Skip	lsl.w	#2,d0
	lea	Tt_Bra-4(pc),a0
	jsr	0(a0,d0.w)
	bne	Tt_ReLoop
	bra	Tt_Loop

; 	Ecran de texte
; ~~~~~~~~~~~~~~~~~~~~
Ttt_E2	swap 	d1
	subq.w	#1,d1
	bmi	Tt_ReLoop
	move.w	d1,-(sp)
	bsr	Ttt_TttAct
	move.w	(sp)+,d1
	lsl.w	#2,d1
	lea	Ttt_Bra(pc),a0
	jsr	0(a0,d1.w)
	bra	Tt_ReLoop

; 	Une zone DBL?
; ~~~~~~~~~~~~~~~~~~~
Tt_DBL	moveq	#1,d0
	JJsr	L_Dia_AutoTest2
	move.l	#Tt_ChannelN,d0
	JJsr	L_Dia_GetReturn
	lsl.w	#2,d1
	beq	Tt_ReLoop
	move.w	d1,-(sp)
	bsr	Ttt_TttAct
	move.w	(sp)+,d1
	lea	J_DBL(pc),a0
	jsr	-4(a0,d1.w)
	bra	Tt_ReLoop

;	Table des sauts
; ~~~~~~~~~~~~~~~~~~~~~
Tt_Bra
	bra	Tt_Rien			1:  STOP, teste separement
	bra	Tt_Step0		2:  mode pas a pas
	bra	Tt_Step1		3:  mode semi-rapide
	bra	Tt_Step2		4:  mode rapide
	bra	Tt_Step3		5:  mode ultra-rapide
	bra	Tt_Up			6:
	bra	Tt_Down			7:
	bra	Tt_Left			8:
	bra	Tt_Right		9:
	bra	Tt_CEc			10: Change ecran
	bra	Tt_Initialise		11: Init
	bra	Tt_Quit			12: Quit
	bra	Tt_See			13: Voir l'affichage
	bra	Tt_SHelp		14: Set Help
	bra	Tt_SBreak		15: Set break
	bra	Tt_SEval		16: Set evalue
Tt_Rien	rts

Ttt_Bra	bra	Ttt_ListClic		1 : Zone de listing
	bra	Ttt_Separ		2 : Zone de separation
	bra	Inf_Clic		3 : Zone d'information
J_DBL	bra	Ttt_Slhclic		1 : Slider horizontal
	bra	Ttt_Slvclic		2 : Slider vertical
	bra	Inf_Sliclic		3 : Slider infos
	bra	Ttt_CText		4 : Bouton de centrage

; ___________________________________________________________________
;
;	INITIALISATION GENERALE
; ___________________________________________________________________
;
; The monitor started and stopped: monitor/start.cpp
; (a4 the monitor's data after the start; the flags: out of memory)
Tt_ColdStart
	jsr	_mon_cold_start_entry
	move.l	Mon_Base(a5),a4
	rts
Tt_Free	jmp	_mon_free_entry

;	TOUCHE: quit/ Retour ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Quit
; Si Break Off >>> on peut pas!!!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.w	Tt_Init(a4)
	ble.s	.Quit
	move.w	ActuMask(a5),d0
	btst	#BitControl,d0
	bne.s	.Quit
	moveq	#7,d0
	bsr	Tt_Message
	bsr	Ttt_Alert
	bsr	Ttt_InfAct
	bra	Inf_AffAll
.Quit
; Effacement de tous les buffers
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Tt_Free
	bsr	TtReCop
; Retour ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'editeur ou au programme???
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a4,a1
	move.l	#Tt_DzLong,d0
	clr.l	Mon_Base(a5)
	tst.w	Tt_Run(a4)
	bne.s	.torun
; Retour ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½diteur
.parun	SyCall	MemFree			Plus de datazone
	clr.l	Prg_Runned(a5)		Plus de programme courant
	JJsr	L_Includes_Clear	Libere la memoire
	JJsr	L_ClearVar
	move.l	#1000,d0		Fait un EDIT
	sub.l	a0,a0
	move.l	Prg_JError(a5),a1
	jmp	(a1)
; Retour au programme
.torun	tst.w	Tt_Init(a4)
	ble.s	.parun
	move.l	BasSp(a5),sp
	move.l	Prg_InsRet(a5),-(sp)
	lea	Step_Data(a4),a0
	move.l	(a0)+,-(sp)
	movem.l	(a0),a3-a6/d4-d5/d6-d7
	SyCall	MemFree
	move.w	ScOn(a5),d1
	beq.s	.skip
	subq.w	#1,d1
	EcCall	Active
.skip	move.l	d4,d0
	move.l	d5,d1
	rts

;
; Actualisation des ecrans
TtReCop	jmp	_mon_recop_entry
Tt_Show	jmp	_mon_show_entry
Tt_Hide	jmp	_mon_hide_entry
Tt_Mouse	jmp	_mon_mouse_free_entry

; ___________________________________________________________________
;
;	PATCH SUR AMOS.LIBRARY
; ___________________________________________________________________
;
;	Branche le patch sur W
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_set_wpatch
_mon_set_wpatch
Set_WPatch
	move.l	a0,-(sp)
	lea	Old_EcVect(pc),a0
	move.l	T_EcVect(a5),(a0)
	lea	Old_SyVect(pc),a0
	move.l	T_SyVect(a5),(a0)
	lea	EcIn(pc),a0
	move.l	a0,T_EcVect(a5)
	lea	SyIn(pc),a0
	move.l	a0,T_SyVect(a5)
	move.l	(sp)+,a0
	rts
;	Enleve le patch sur W
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_del_wpatch
_mon_del_wpatch
Del_WPatch
	move.l	a0,-(sp)
	lea	Old_EcVect(pc),a0
	tst.l	(a0)
	beq.s	.Skip1
	move.l	(a0),T_EcVect(a5)
	clr.l	(a0)
.Skip1	lea	Old_SyVect(pc),a0
	tst.l	(a0)
	beq.s	.Skip2
	move.l	(a0),T_SyVect(a5)
	clr.l	(a0)
.Skip2	move.l	(sp)+,a0
	rts

;	Patch des fonctions W ecrans...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EcIn	bsr	TT_WPatch		RAZ
	bsr	TT_WPatch2		Copper
	bsr	TT_WPatch2		Libre
	bsr	TT_WPatchC		Cree
	bsr	TT_WPatch		Del
	bsr	TT_WPatch		First
	bsr	TT_WPatch		Last
	bsr	TT_WPatch		Active
	bsr	TT_WPatch2		Forcecop
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch		5 Fonctions de securite!
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch
	bsr	TT_WPatch

;	Restore les ecrans puis appelle
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_WPatch2
	bsr.s	Tt_first
;	Patch normal...
; ~~~~~~~~~~~~~~~~~~~~~
TT_WPatch
	move.l	a0,-(sp)
	move.l	4(sp),a0
	sub.l	T_EcVect(a5),a0
	subq.l	#4,a0
	add.l	Old_EcVect(pc),a0
	move.l	a0,4(sp)
	move.l	(sp)+,a0
	rts
;	Appelle puis restore les ecrans
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_WPatch1
	move.l	(sp)+,a0
	sub.l	T_EcVect(a5),a0
	subq.l	#4,a0
	add.l	Old_EcVect(pc),a0
	jsr	(a0)
; Routine: met les deux en premier...
Tt_first
	movem.l	a0-a6/d0-d7,-(sp)
	move.l	Old_EcVect(pc),a0	* Ecran Tutor1
	moveq	#Tt_Ecran1,d1
	jsr	First*4(a0)
	move.l	Old_EcVect(pc),a0	* Ecran Tutor2
	moveq	#Tt_Ecran2,d1
	jsr	First*4(a0)
	move.l	Old_EcVect(pc),a0	* Ecran FSEL
	moveq	#EcFsel,d1
	jsr	First*4(a0)
	move.l	Old_EcVect(pc),a0	* Ecran Requester
	moveq	#EcReq,d1
	jsr	First*4(a0)
	movem.l	(sp)+,a0-a6/d0-d7
	tst.l	d0
	rts
;	Patch pour CREATION ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_WPatchC
; Appelle
	move.l	(sp)+,a0
	sub.l	T_EcVect(a5),a0
	subq.l	#4,a0
	add.l	Old_EcVect(pc),a0
	jsr	(a0)
	bne.s	.Err
; Change les tables du TUTOR
	movem.l	a0/a1/d0/d1,-(sp)
	move.w	EcNumber(a0),d1
	cmp.w	#8,d1
	bcc.s	.skuo
	lsl.w	#3,d1
	move.l	Mon_Base(a5),a1
	lea	ViewScrols(a1),a1
	add.w	d1,a1
	clr.l	(a1)
	move.w	EcTx(a0),4(a1)
	move.w	EcTy(a0),d0
	tst.w	EcCon0(a0)
	bmi.s	.skip
	lsr.w	#1,d0
.skip	move.w	d0,6(a1)
.skuo	movem.l	(sp)+,a0/a1/d0/d1
; Ok!
.Err	tst.w	d0
	rts

;	Patch des fonctions W systeme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SyIn:	bsr 	TT_SPatch	ClInky		;0
	bsr	TT_SPatch	ClVide		;1
	bsr 	TT_SPatch	ClSh		;2
	bsr	TT_SPatch	ClInst		;3
	bsr	TT_SPatch	ClKeyM		;4
	bsr	TT_SPatch	ClJoy		;5
	bsr	TT_SPatch	ClPutK		;6
	bsr	TT_SPatch	MHide		;7
	bsr	TT_SPatch	MShow		;8
	bsr	TT_SPatch	MChange		;9 - ChMouse
	bsr	TT_SPatch	MXy		;10- XY Mouse
	bsr	TT_SPatch	CXyHard		;11- Conversion SCREEN-> HARD
	bsr	TT_SXyScr	CXyScr		;12- Conversion HARD-> SCREEN
	bsr	TT_SPatch	MBout		;13
	bsr	TT_SPatch	MSetAb		;14
	bsr	TT_SGSIn	GetSIn		;15- Get screen IN
	bsr	TT_SPatch	CXyWi		;16- Conversion SCREEN-> WINDOW courante
	bsr	TT_SPatch	MLimA		;17- Limit mouse
	bsr	TT_SZoHd	SyZoHd		;18- Zone coordonnees HARD
	bsr	TT_SPatch	SyResZ		;19- Reserve des zones
	bsr	TT_SPatch	SyRazZ		;20- Effacement zones
	bsr	TT_SPatch	SySetZ		;21- Set zone
	bsr	TT_SPatch	SyMouZ		;22- Zone souris!
	bsr	TT_SPatch	WVbl		;23
	bsr	TT_SPatch	HsSet		;24- Affiche un hard sprite
	bsr	TT_SPatch	HsUSet		;25- Efface un hard sprite
	bsr	TT_SPatch	ClFFk		;26
	bsr	TT_SPatch	ClGFFk		;27
	bsr	TT_SPatch	HsAff		;28- Recalcule les hard sprites
	bsr	TT_SPatch	HsBank		;29- Fixe la banque de sprites
	bsr	TT_SPatch	HsNXYA		;30- Instruction sprite
	bsr	TT_SPatch	HsXOff		;31- Sprite off n
	bsr	TT_SPatch	HsOff		;32- All sprite off
	bsr	TT_SPatch	HsAct		;33- Actualisation HSprite
	bsr	TT_SPatch	HsSBuf		;34- Set nombre de lignes
	bsr	TT_SPatch	HsStAct		;35- Arrete les HS sans deasctiver!
	bsr	TT_SPatch	HsReAct		;36- Re-Active tous!
	bsr	TT_SPatch	MStore		;37- Stocke etat souris / Show on
	bsr	TT_SPatch	MRecall		;38- Remet la souris
	bsr	TT_SPatch	HsPri		;39- Priorites SPRITES/PLAYFIELD
	bsr	TT_SPatch	TokAMAL		;40- Tokenise AMAL
	bsr	TT_SPatch	CreAMAL		;41- Demarre AMAL
	bsr	TT_SPatch	MvOAMAL		;42- On/Off/Freeze AMAL
	bsr	TT_SPatch	DAllAMAL	;43- Enleve TOUT!
	bsr	TT_SPatch	Animeur		;44- Un coup d'animation
	bsr	TT_SPatch	RegAMAL		;45- Registre!
	bsr	TT_SPatch	ClrAMAL		;46- Clear
	bsr	TT_SPatch	FrzAMAL		;47- FREEZE all
	bsr	TT_SPatch	UFrzAMAL	;48- UNFREEZE all
	bsr	TT_SPatch	BobSet		;49- Entree set bob
	bsr	TT_SPatch	BobOff		;50- Arret bob
	bsr	TT_SPatch	BobSOff		;51- Arret tous bobs
	bsr 	TT_SPatch	BobAct		;52- Actualisation bobs
	bsr	TT_SPatch	BobAff		;53- Affichage bobs
	bsr	TT_SPatch	BobEff		;54- Effacement bobs
	bsr	TT_SPatch	ChipMM		;55- Reserve CHIP
	bsr	TT_SPatch	FastMM		;56- Reserve FAST
	bsr	TT_SPatch	BobLim		;57- Limite bobs!
	bsr	TT_SPatch	SyZoGr		;58- Zone coord graphiques
	bsr	TT_SPatch	GetBob		;59- Saisie graphique
	bsr	TT_SPatch	Masque		;60- Calcul du masque
	bsr	TT_SPatch	SpotH		;61- Fixe le point chaud
	bsr	TT_SPatch	BbColl		;62- Collisions bob
	bsr	TT_SPatch	GetCol		;63- Fonction collision
	bsr	TT_SPatch	SpColl		;64- Collisions sprites
	bsr	TT_SPatch	SyncO		;65- Synchro on/off
	bsr	TT_SPatch	Sync		;66- Synchro step
	bsr	TT_SPatch	SetPlay		;67- Set play direction...
	bsr	TT_SPatch	BobXY		;68- Get XY Bob
	bsr	TT_SPatch	HsXY		;69- Get XY Sprite
	bsr	TT_SPatch	BobPut		;70- Put Bob!
	bsr	TT_SPatch	TPatch		;71- Patch icon/bob!
	bsr	TT_SPatch	MRout		;72- Souris relachee
	bsr	TT_SPatch	MLimEc		;73- Limit mouse ecran
	bsr	TT_SPatch	FreeMM		;74- Libere mem
	bsr	TT_SPatch	HColSet		;75- Set HardCol
	bsr	TT_SPatch	HColGet		;76- Get HardCol
	bsr	TT_SPatch	TMovon		;77- Movon!
	bsr	TT_SPatch	TKSpeed		;78- Key speed
	bsr	TT_SPatch	TChanA		;79- =ChanAn
	bsr	TT_SPatch	TChanM		;80- =ChanMv
	bsr	TT_SPatch	TPrio		;81- Set priority
	bsr	TT_SPatch	TGetDisc	;82- State of disc drive
	bsr	TT_SPatch	Add_VBL		;83- Restart VBL
	bsr	TT_SPatch	Rem_VBL		;84- Stop VBL
	bsr	TT_SPatch	ClKwait		;85- Une touche en attente?
	bsr	TT_SPatch	WMouScrFront	;86- Coordonnees souris devant ILLEGAL
	bsr	TT_SPatch	WMemReserve	;87- (P) Reservation memoire secure
	bsr	TT_SPatch	WMemFree	;88- (P) Liberation memoire secure
	bsr	TT_SPatch	WMemCheck	;89- (P) Verification memoire
	bsr	TT_SPatch	WMemFastClear	;90- (P)
	bsr	TT_SPatch	WMemChipClear	;91-
	bsr	TT_SPatch	WMemFast	;92-
	bsr	TT_SPatch	WMemChip	;93-
	bsr	TT_SPatch			;94-
	bsr	TT_SPatch			;95-
	bsr	TT_SPatch			;96-
	bsr	TT_SPatch			;97-
	bsr	TT_SPatch			;98-
	bsr	TT_SPatch			;99-
	bsr	TT_SPatch			;100-


;	Patch systeme normal...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_SPatch
	move.l	a0,-(sp)
	move.l	4(sp),a0
	sub.l	T_SyVect(a5),a0
	subq.l	#4,a0
	add.l	Old_SyVect(pc),a0
	move.l	a0,4(sp)
	move.l	(sp)+,a0
	rts
;	Patch fonction XyScr
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_SXyScr
	bsr	TT_SHere
	bne	TT_SPatch
	move.l	a4,(sp)
	move.l	Mon_Base(a5),a4
	bsr	TT_Hard2Screen
	movem.l	(sp)+,a4
	bpl.s	.Skip
	moveq	#0,d1
	moveq	#0,d2
.Skip	moveq	#0,d0
	rts
;	Patch fonction Zone Hard
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_SZoHd
	bsr	TT_SHere
	bne	TT_SPatch
	move.l	a4,(sp)
	move.l	Mon_Base(a5),a4
	bsr	TT_Hard2Screen
	move.l	(sp)+,a4
	bmi.s	.Out
	moveq	#8,d5
	move.l	Old_SyVect(pc),a0
	jsr	ZoGr*4(a0)
.Skip	rts
.Out	moveq	#0,d1
	moveq	#0,d0
	rts
;	Patch fonction GetSCin
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_SGSIn
	bsr	TT_SHere
	bne	TT_SPatch
	move.l	a4,(sp)
	move.l	Mon_Base(a5),a4
	bsr	TT_EcIn
	bmi.s	.Out
	lsl.w	#2,d0
	lea	T_EcAdr(a5),a1
	move.l	0(a1,d0.w),d0
	beq.s	.Out
	move.l	d0,a1
	bsr	XY2Ec
	bmi.s	.Out
	cmp.w	EcTx(a1),d1
	bcc.s	.Out
	cmp.w	EcTy(a1),d2
	bcc.s	.Out
	moveq	#0,d1
	move.w	EcNumber(a1),d1
	move.l	(sp)+,a4
	moveq	#0,d0
	rts
.Out	move.l	#EntNul,d1
	move.l	(sp)+,a4
	moveq	#0,d0
	rts

;;	Boutons de la souris, uniquement dans le petit ecran
;; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;TT_SMBout
;	move.l	Old_SyVect(pc),a0
;	jsr	MouseKey*4(a0)
;	tst.b	d1
;	beq.s	.Skip
;	bsr	TT_SHere
;	bne.s	.Skip
;	movem.l	d2-d7/a1-a2,-(sp)
;	move.w	d1,-(sp)
;	move.l	d2,d7
;	move.l	Old_SyVect(pc),a0
;	jsr	XyMou*4(a0)
;	bsr	TT_EcIn
;	cmp.l	#$01234567,d7
;	beq.s	.Mon
;; Demande de la part du programme: uniquement DANS l'ecran
;	tst.l	d0
;	bmi.s	.Non
;	bra.s	.Oui
;; Demande de la part du moniteur: uniquement en dehors de l'ecran, si RUN
;.Mon	tst.l	d0
;	bmi.s	.Oui
;	move.l	Mon_Base(a5),a0
;	tst.w	Step_Mode(a0)
;	beq.s	.Oui
;.Non	clr.w	(sp)
;.Oui	move.w	(sp)+,d1
;	movem.l	(sp)+,d2-d7/a1-a2
;.Skip	rts

;	Conversion coordonnÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½es hard (d1/d2/d3) -> coordonnÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½es ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_Hard2Screen
	bsr	TT_EcToA1
	bmi.s	.Out
	move.l	a0,a1
	bsr	TT_EcIn
	bmi.s	.Out
	move.w	EcNumber(a1),d3
	addq.w	#1,d3
	cmp.w	EcNumber(a1),d0
	beq.s	XY2Ec
.Out	moveq	#-1,d0
	rts
;	Converti D1/D2 en coord dans l'ecran A1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
XY2Ec	movem.w	d1-d3,-(sp)
	bsr	View_ScAd
	movem.w	(sp)+,d1-d3
	add.w	(a0)+,d1
	add.w	(a0)+,d2
; Si lowres, multiplie par 2 en Y
	btst	#7,EcCon0(a1)
	bne.s	.Hires
	lsl.w	#1,d2
.Hires
	moveq	#0,d0
	rts

;	Trouve l'ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½cran sous les coordonnees hard...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1/D2= X/Y HARD
;	D3= 1er ecran
;	D4= Ecran MAX
TT_EcIn	movem.l	a0/d3,-(sp)
	move.w	d2,-(sp)
	move.w	d1,-(sp)
	moveq	#Tt_Ecran1+1,d3
	move.l	Old_SyVect(pc),a0
	jsr	XyScr*4(a0)
	tst.w	(sp)
	beq.s	.Skip1
	sub.w	#RedX,d1
	bmi.s	.Pas
	cmp.w	#RedTX,d1
	bcc.s	.Pas
.Skip1	tst.w	2(sp)
	beq.s	.Ok
	sub.w	#RedY,d2
	bmi.s	.Pas
	cmp.w	#RedTY,d2
	bcc.s	.Pas
; Dans le petit ecran...
.Ok	moveq	#0,d0
	move.w	Tt_View(a4),d0
	bpl.s	.In
.Pas	moveq	#-1,d0
.In	addq.l	#4,sp
	movem.l	(sp)+,a0/d3
	rts

;	Ecrans D3
; ~~~~~~~~~~~~~~~
;	<0	=> Rien du tout
;	0	=> Ecran courant
;	>0	=> Ecran+1
TT_EcToA1
	tst.w	d3
	bmi.s	EcToD4
	bne.s	EcToD2
	move.l	T_EcCourant(a5),a0
	rts
EcToD2:	lsl.w	#2,d3
	lea	T_EcAdr(a5),a0
	move.l	-4(a0,d3.w),d3
	beq.s	EcToD3
	move.l	d3,a0
	rts
EcToD3:	addq.l	#4,sp
	moveq	#3,d0
	rts
EcToD4:	moveq	#-1,d0
	rts

;	Retourne BNE si l'ecran du tuteur n'est pas la
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TT_SHere
	movem.l	a0/d0/d1,-(sp)
	moveq	#1,d1
	move.l	Mon_Base(a5),a0
	tst.b	Tt_NotHere(a0)
	bne.s	.Out
	move.l	T_EcCourant(a5),d0
	beq.s	.Out
	move.l	d0,a0
	move.w	EcNumber(a0),d0
	cmp.w	#8,d0
	bcc.s	.Out
	moveq	#0,d1
.Out	tst.w	d1
	movem.l	(sp)+,a0/d0/d1
	rts

; ___________________________________________________________________
;
;	Patch SCREEN TO FRONT
; ___________________________________________________________________
;
		xdef	_mon_screen_front
_mon_screen_front
Tt_PScFront
	bra	ToFront
	bra	ToBack
; Passe devant
; ~~~~~~~~~~~~
ToFront	jmp	_mon_to_front_entry
ToBack	jmp	_mon_to_back_entry
; ___________________________________________________________________
;
;	Patch SCREEN COPY
; ___________________________________________________________________
;
Tt_PScCopy	jmp	_mon_screen_copy_entry
; ___________________________________________________________________
;
;	Patch branche sur tests: reaffiche l'ecran (mode 3)
; ___________________________________________________________________
;
TstPatch	jmp	_mon_tests_patch_entry
BoutStop	jmp	_mon_stop_button_entry


; ___________________________________________________________________
;
;	GESTION PROGRAMME
; ___________________________________________________________________


; The listing window: monitor/listing.cpp
Ttt_CText	jmp	_mon_centre_click_entry
Ttt_Slhclic	jmp	_mon_slider_horizontal_click_entry
Ttt_Slvclic	jmp	_mon_slider_vertical_click_entry
Ttt_Centre	jmp	_mon_listing_centre_entry
Ttt_AffBuf	jmp	_mon_listing_show_entry
Ttt_NewBuf	jmp	_mon_listing_new_entry
Ttt_Untok	jmp	_mon_listing_untokenise_entry


; ___________________________________________________________________
;
;	ECRAN DU BAS
; ___________________________________________________________________


Ttt_Separ	jmp	_mon_separation_drag_entry

Inf_Centre	jmp	_mon_info_centre_entry


Ttt_Cls	jmp	_mon_listing_screen_entry
Ttt_SetSize	jmp	_mon_listing_size_entry

; The windows of the text screen: monitor/info.cpp
Ttt_InfAct	jmp	_mon_activate_info_entry
Ttt_TttAct	jmp	_mon_activate_text_entry

; ___________________________________________________________________
;
;	PAS A PAS / PATCH BASIC
; ___________________________________________________________________



;	TOUCHES: help / break / eval
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_SHelp	jmp	_mon_set_help_entry
Tt_SBreak	jmp	_mon_set_break_entry
Tt_SEval	jmp	_mon_set_evaluate_entry
Ttt_AMod	jmp	_mon_mode_buttons_entry

;	TOUCHE: une instruction
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Step0
	bsr	Wmky
	tst.w	Tt_Init(a4)
	ble.s	NoInit
	bra	Step_Out
NoInit	jmp	_mon_no_init_entry
;	TOUCHE: deroulement semi-rapide
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Step1
	bsr	Wmky
	tst.w	Tt_Init(a4)
	ble.s	NoInit
	move.w	#1,Step_Mode(a4)
	bra	Step_Out
;	TOUCHE: deroulement rapide
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Step2
	bsr	Wmky
	tst.w	Tt_Init(a4)
	ble.s	NoInit
	move.w	#2,Step_Mode(a4)
	bra	Step_Out
;	TOUCHE: deroulement ultra-rapide
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Step3
	bsr	Wmky
	tst.w	Tt_Init(a4)
	ble.s	NoInit
	bsr	Tt_Hide
	move.w	#3,Step_Mode(a4)
	bra	Step_Out2

;	TOUCHE:	Init
; ~~~~~~~~~~~~~~~~~~
Tt_Initialise
	tst.w	Tt_Init(a4)		Arrete?
	ble.s	Tt_ReInit
	move.w	ActuMask(a5),d0		En marche, Break-Off mode?
	btst	#BitControl,d0
	beq.s	Tt_Noq
; Stoppe le programme
	move.w	#1003,d0		Erreur 1003= moniteur!
	JJmp	L_Error
; Fait l'initialisation!
Tt_ReInit
	clr.l	Step_Mess(a4)
	clr.w	Tt_Init(a4)
	moveq	#1,d0			Message d'info
	bsr	Inf_Del
	moveq	#3,d0
	bsr	Inf_Del
	moveq	#12,d0			Testing program
	bsr	Tt_Message
	bsr	Ttt_Alert
	bsr	Ttt_InfAct
	bsr	Inf_AffAll
	bsr	Inf_Slider
; Lance le programme
	bsr	Tt_PrgBanks
	moveq	#0,d0			Programme normal
	lea	Tt_Error(pc),a1		Adresse routine d'erreur
	lea	Tt_Testing(pc),a2	Messages
	move.l	Tt_Program(a4),a6	Structure programme
	JJsr	L_Prg_RunIt		Lance le programme!
; Out of memory, si revient
	moveq	#17,d0
	bra.s	Tt_IMes
; No break mode
Tt_Noq	moveq	#16,d0			No initialise mode!
Tt_IMes	bsr	Tt_Message
	bsr	Ttt_Alert
	bsr	Ttt_InfAct
	bra	Inf_AffAll

; Branchement aux messages de testing
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Testing
	bra	Tt_Tst1
	bra	Tt_Tst2
	bra	Tt_Tst3
; Enleve le message de test
; ~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Tst1	rts
Tt_Tst2	jmp	_mon_tested_entry
Tt_Tst3	rts

;	Entree du pas a pas...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MonitorChr
	move.l	-LB_Size(a4,d1.w),-(sp)	Adresse de l'instruction
	cmp.w	#_TkDP,-2(a6)		Pas sur un :
	beq	.Back			RTS direct
	lea	-2(a6),a1
	move.l	d0,d4			Sauve les adresses importantes
	move.l	d1,d5
; Mode RAPIDE, saute tout sauf si breakpoint...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d2			Pas STOP!
	move.l	Mon_Base(a5),a0
	cmp.w	#2,Step_Mode(a0)
	bcs.s	.skipB
	bne.s	.skip
	bset	#1,T_Actualise+1(a5)
.skip	tst.w	Step_Stop(a0)
	bne.s	.skipA
	cmp.l	Step_Break(a0),a1
	bne.s	.GoBack
.skipA	clr.w	Step_Mode(a0)
	moveq	#-1,d2
; Mode LENT, avec affichage...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.skipB	tst.w	Tt_Init(a0)
	beq	.skippy
	bmi.s	.skipB0
	bclr	#BitControl,T_Actualise(a5)	Control-C a chaque coup!
	bne.s	.skipB0
	tst.w	Step_Stop(a0)
	bne.s	.skipB0
	cmp.l	Step_Break(a0),a1
	bne.s	.skipB1
	exg	a0,a1				Break message
	moveq	#10,d0
	bsr	Tt_Message
	exg	a0,a1
	move.l	a1,Step_Mess(a0)
.skipB0	clr.l	Step_Break(a0)
	clr.w	Step_Mode(a0)
	clr.w	Step_Stop(a0)
	moveq	#-1,d2
	bra.s	.skipB2
.skipB1	cmp.w	#_TkEndP,-2(a6)			Fin de procedure?
	bne.s	.skipB2
	bsr	TstPatch			Reaffiche + sort
	bra.s	.GoBack
; Rebranche ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'instruction
.GoOut	bset	#1,T_Actualise+1(a5)		Reaffichage si long...
	bset	#1,ActuMask+1(a5)
.GoBack	move.l	d4,d0				Recharge les registres
	move.l	d5,d1
.Back	rts					Rebranche ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'instruction
; Localise l'instruction
.skipB2	move.l	Prg_Run(a5),a1
	move.l	d7,a0				Adresse de la ligne
	tst.l	d7
	beq.s	.GoBack
	JJsrR	L_Tk_FindA,a2
	tst.w	d2				Arret force?
	bne.s	.stopit
	tst.l	d1				Procedure fermee?
	bne.s	.GoOut
.stopit	tst.l	d1				Procedure fermee?
	bne.s	.lockee
	move.l	a6,d1
	subq.l	#2,d1
	moveq	#0,d2
	bra.s	.nolock
.lockee	addq.l	#2,d1
	moveq	#-1,d2
; Poke les adresses
.nolock	move.l	a0,a2
.skippy	move.l	Mon_Base(a5),a0
	lea	Step_Data(a0),a0	Stockage des donnees
	move.l	(sp),(a0)+		Adresse de retour
	movem.l	a3-a6/d4-d5/d6-d7,(a0)	Registres importants
	move.l	BasSp(a5),sp
	move.l	Prg_InsRet(a5),-(sp)
	move.l	Mon_Base(a5),a4		Sauve la pile / Flag general
	move.l	Tt_Program(a4),a3
; The screens, the listing, the instruction: monitor/step.cpp
	jsr	_mon_step_show_entry
	beq	Tt_ReLoop

;	Sortie du mode pas a pas, reactive l'ancien ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Step_Out
	bset	#1,T_Actualise+1(a5)
	bset	#1,ActuMask+1(a5)
Step_Out2
	bsr	Tt_PrgBanks
	move.l	BasSp(a5),sp
	move.l	Prg_InsRet(a5),-(sp)
	move.w	ScOn(a5),d1
	beq.s	.Skip
	subq.w	#1,d1
	EcCall	Active
.Skip

	IFNE	Debug=2
	tst.w	Ttt_Mode(a4)			*** Illegal, debugging!
	bne.s	.Skip2
	JJsr	L_BugBug
.Skip2
	ENDC

	move.l	Step_Data(a4),a0
	movem.l	Step_Data+4(a4),a3-a6/d0-d1/d6-d7
	jmp	(a0)


;	Entree generale des erreurs "Patch2"
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Error
	move.l	BasSp(a5),sp
	move.l	Prg_InsRet(a5),-(sp)
	bsr	Tt_GetTheMessage
	move.l	Mon_Base(a5),a1
	tst.w	Tt_Init(a1)
	beq.s	.Test
; Erreur lors de RUN
; ~~~~~~~~~~~~~~~~~~
	move.w	d2,d3
	move.l	a0,d1			Si EDIT ou DIRECT
	bne.s	.Mh
	moveq	#15,d0
	bsr	Tt_Message
.Mh	move.w	d0,d1
	move.l	a0,Step_Mess(a1)
	move.l	d7,a6
	move.l	PLoop(a5),a3
	move.w	-2(a6),d0
	move.w	0(a4,d0.w),d1		Pointe la table de tokens
	bsr	Tt_Show
	clr.w	Step_Mode(a1)
	cmp.w	#9,d3			Control-C?
	beq	MonitorChr
	cmp.w	#1003,d3		INIT
	beq	Tt_ReInit
	move.w	#-1,Tt_Init(a1)		Arret du STEP, pas des expressions
	bra	MonitorChr
; Erreurs lors du test! (monitor/step.cpp)
.Test	jsr	_mon_test_error_entry
	move.l	Mon_Base(a5),a4
	bra	Tt_ReLoop

; Trouve le message d'erreur en retour de programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_GetTheMessage
	move.l	a1,-(sp)
	move.w	d0,d2
	move.l	a0,d1
	bne.s	.Clair
	cmp.w	#256,d0			Un message?
	bge.s	.Clair
	move.l	Ed_TstMessages(a5),a0	Message testing
	neg.w	d0
	bpl.s	.MTest
	neg.w	d0
	move.l	Ed_RunMessages(a5),a0
	addq.w	#1,d0
.MTest	JJsrR	L_GetMessage,a1
.Clair	move.l	(sp)+,a1
	rts

Ttt_ListClic	jmp	_mon_listing_click_entry
Tt_Souligne	jmp	_mon_choose_entry
Tt_MouMou	jmp	_mon_mouse_position_entry

; The help: monitor/help.cpp
Hlp_FreeMap	jmp	_mon_free_map_entry

; A line split into its parts: monitor/listing.cpp
Tt_Decoup	jmp	_mon_split_entry
; Partie appelle par la detokenisation...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_split_hook
_mon_split_hook
Tt_Decoupe
	movem.l	a0-a1/d0-d3,-(sp)
	moveq	#0,d1
	move.w	(a6),d0
	beq	.skip1
	moveq	#"-",d1
	cmp.w	#_TkM,d0
	beq	.skip1
	moveq	#":",d1
	cmp.w	#_TkDP,d0
	beq	.skip1
	cmp.w	#_TkThen,d0
	beq	.skip1
	cmp.w	#_TkElse,d0
	beq	.skip1
	moveq	#"V",d1
	cmp.w	#_TkVar,d0
	beq.s	.skip1
	moveq	#"X",d1
	cmp.w	#_TkPro,d0
	beq.s	.skip1
	moveq	#"G",d1
	cmp.w	#_TkLGo,d0
	beq.s	.skip1
	moveq	#"2",d1
	cmp.w	#_TkCh1,d0
	beq.s	.skip1
	cmp.w	#_TkCh2,d0
	beq.s	.skip1
	moveq	#"1",d1
	cmp.w	#_TkFl,d0
	beq.s	.skip1
	moveq	#"(",d1
	cmp.w	#_TkPar1,d0
	beq.s	.skip1
	moveq	#")",d1
	cmp.w	#_TkPar2,d0
	beq.s	.skip1
	moveq	#",",d1
	cmp.w	#_TkVir,d0
	beq.s	.skip1
	cmp.w	#_TkPVir,d0
	beq.s	.skip1
	cmp.w	#_TkTo,d0
	beq.s	.skip1
	cmp.w	#_TkAs,d0
	beq.s	.skip1
	moveq	#"0",d1
	cmp.w	#_TkExt,d0
	bcs.s	.skip1
	beq.s	.jmp1
; An operator: its definition is in the interpreter's table, not before
; the instructions' (the original read whatever was there)
	moveq	#"O",d1
	tst.w	d0
	bmi.s	.skip1
; Instruction/fonction normale
	move.l	AdTokens(a5),a0
	lea	4(a0,d0.w),a0
.loop1	tst.b	(a0)+
	bpl.s	.loop1
	move.b	(a0)+,d1
	bpl.s	.skip2
.skip0	moveq	#"I",d1
.skip2	cmp.b	#"V",d1
	bne.s	.skip1
	move.b	(a0)+,d1
; Poke l'adresse dans la table
.skip1	move.l	Mon_Base(a5),a1
	move.l	Tt_PLine(a1),a0
	cmp.b	#"V",d1
	beq.s	.skup
	cmp.b	#"0",d1
	beq.s	.skup
	cmp.b	#"1",d1
	beq.s	.skup
	cmp.b	#"2",d1
	bne.s	.skip3
.skup	cmp.b	#":",-4(a0)
	bne.s	.skip3
	moveq	#"W",d1
.skip3	move.b	d1,(a0)+
	move.l	a4,d0
	sub.l	Tt_BufT(a1),d0
	subq.l	#2,d0
	move.b	d0,(a0)+
	move.l	a6,d0
	sub.l	Tt_ALine(a1),d0
	move.w	d0,(a0)+
	move.l	a0,Tt_PLine(a1)
	movem.l	(sp)+,a0-a1/d0-d3
	rts
; Detokenise une extension
.jmp1	move.w	4(a6),d1
	move.b	2(a6),d2
	ext.w	d2
	lsl.w	#2,d2
	tst.l	AdTokens(a5,d2.w)
	beq.s	.skip0
	move.l	AdTokens(a5,d2.w),a0
	lea	4(a0,d1.w),a0
	bra.s	.loop1

;	Va evaluer une expression
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_evaluate
_mon_evaluate
Tt_Evalue
	movem.l	d1/d4-d7/a0-a5,-(sp)
	move.l	a6,Tt_EvDebut(a4)
	lea	Tt_EvError(pc),a0	Patche les erreurs
	move.l	a0,Patch_Errors(a5)
	lea	Tt_Menage(pc),a0	Patche le menage
	move.l	a0,Patch_Menage(a5)
	bsr	Tt_PrgBanks
Tt_ReEv	move.l	sp,Tt_EvPile(a4)	Position pile en cas de menage
	move.l	AdTokens(a5),a4		Recharge la table
	moveq	#0,d7
	move.l	PLoop(a5),a3
	JJsr	L_New_Evalue
	movem.l	(sp)+,d1/d4-d7/a0-a5
	bsr	Tt_TtBanks
	clr.l	Patch_Errors(a5)
	clr.l	Patch_Menage(a5)
	moveq	#0,d0
	rts
;	En cas d'erreurs d'evaluation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_EvError
	move.l	Mon_Base(a5),a4
	move.l	Tt_EvPile(a4),sp
	movem.l	(sp)+,d1/d4-d7/a0-a5
	bsr	Tt_TtBanks
	clr.l	Patch_Errors(a5)
	clr.l	Patch_Menage(a5)
	moveq	#0,d3
	moveq	#0,d2
	moveq	#-1,d0
	rts
;	En cas de menage
; ~~~~~~~~~~~~~~~~~~~~~~
Tt_Menage
	move.l	Mon_Base(a5),a4
	move.l	Tt_EvPile(a4),sp
	move.l	Tt_EvDebut(a4),d0
	beq.s	.err
	move.l	d0,a6
	clr.l	Tt_EvDebut(a4)
	movem.l	(sp),d1/d4-d7/a0-a5
	bra.s	Tt_ReEv
.err	moveq	#11,d0
	JJsr	L_Error


; ___________________________________________________________________
;
;	GESTION INFORMATIONS
; ___________________________________________________________________

; The information window: monitor/info.cpp
Inf_Clic	jmp	_mon_info_click_entry
Inf_AffLine	jmp	_mon_info_show_line_entry
Inf_CptLines	jmp	_mon_info_count_lines_entry
Inf_AffAll	jmp	_mon_info_show_all_entry
Inf_PrDeb	jmp	_mon_info_begin_entry
Inf_Print	jmp	_mon_info_print_entry
Inf_PrFin	jmp	_mon_info_end_entry
Inf_Del	jmp	_mon_info_delete_entry
Inf_Ins	jmp	_mon_info_insert_entry
Inf_Sliclic	jmp	_mon_info_slider_click_entry
Inf_Slider	jmp	_mon_info_slider_entry
Ttt_Alert	jmp	_mon_alert_entry
Ttt_AlertDel	jmp	_mon_alert_delete_entry
Ttt_DelAff	jmp	_mon_delete_show_entry

; ___________________________________________________________________
;
;	ECRAN DU PROGRAMME
; ___________________________________________________________________



;	TOUCHE: changement d'ecran view
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_CEc	jmp	_mon_change_screen_entry
Tt_Down	jmp	_mon_move_down_entry
Tt_Up	jmp	_mon_move_up_entry
Tt_Left	jmp	_mon_move_left_entry
Tt_Right	jmp	_mon_move_right_entry
Tt_See	jmp	_mon_see_entry
View_Aff	jmp	_mon_view_show_entry
View_Ec	jmp	_mon_view_next_entry

;	Trouve l'adresse des positions de scrolling.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1->	A0
View_ScAd
	move.w	Tt_View(a4),d0
	bmi.s	.no
	lsl.w	#3,d0
	lea	ViewScrols(a4),a0
	add.w	d0,a0
	moveq	#0,d0
	rts
.no	moveq	#-1,d0
	rts
;	Calcule la table lors d'init!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
View_ScUpdate	jmp	_mon_view_sizes_entry

;	Routine d'interruption, recopie la palette...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_view_inter
_mon_view_inter
View_Inter
	move.l	Mon_Base(a5),a0
	move.w	Tt_View(a0),d2
	bmi.s	.skip
	lsl.w	#2,d2
	lea	T_EcAdr(a5),a0
	move.l	0(a0,d2.w),d0
	beq.s	.skip
	move.l	d0,a2
	lea	EcPal(a2),a2
	move.l	Tt_Ecran1*4(a0),a1
	lea	EcPal(a1),a1
	move.l	T_CopMark+Tt_Ecran1*128(a5),d0
	beq.s	.skip
	move.l	d0,a0
	addq.l	#2,a0
	moveq	#11-1,d0
.loop	move.w	(a2),(a0)
	addq.l	#4,a0
	move.w	(a2)+,(a1)+
	dbra	d0,.loop
.skip	rts


;	Activation de l'ecran de texte
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ttt_Act	moveq	#Tt_Ecran2,d1
	bra	Tt_EcAct
View_Act
	moveq	#Tt_Ecran1,d1
Tt_EcAct	jmp	_mon_activate_entry

;	CLS de l'ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½cran de zoom
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
View_Cls	jmp	_mon_view_clear_entry
View_Info	jmp	_mon_view_info_entry

; ___________________________________________________________________
;
;	GESTION DES BOUTONS
; ___________________________________________________________________

;	Impression de tous les boutons
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ABoutons	jmp	_mon_all_buttons_entry
ABouton	jmp	_mon_button_entry
Tt_UPack	jmp	_mon_unpack_entry

;	Trouve le message D0 >>> A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Message	jmp	_mon_message_entry

;	Attente souris
; ~~~~~~~~~~~~~~~~~~~~~
Wmky	jmp	_mon_wait_release_entry
; 	Entree souris moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_MBout
	SyCall	MouseKey
	rts

;	XY Mouse dans l'ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
XYMouEc	SyCall	XyMou
	moveq	#0,d3
	SyCall	XyScr
	rts

; 	Attente multitache
; ~~~~~~~~~~~~~~~~~~~~~~~~
Tt_Multi
	JJmp	L_Sys_WaitMul

; 	Banques de mÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½moire/dialogue du moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_TtBanks	jmp	_mon_banks_monitor_entry
Tt_PrgBanks	jmp	_mon_banks_program_entry

;		Donnees tutor
		Even
* Ancien vecteur ECRANS
Old_EcVect	dc.l	0
Old_SyVect	dc.l	0
* Textes fenetre listing
		xdef	_mon_window_init
_mon_window_init
Ttt_WInit	dc.b	27,"C0",27,"B2",27,"P3",25,27,"V0",0

		Even
; 		Petit DBL pour dessiner le fond
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_dbl
_mon_dbl
DBL		dc.w	.Fin-.Debut
.Debut		dc.b	"BA	0,0;"
		dc.b	"SI	SW,SH;"
		dc.b	"LI	0,0,94,SX16-;"
		dc.b	"LI	0,YB 0VA8* +,100,SX;"
		dc.b	"LI	0,SY4-,106,SX;"
		dc.b	"VL	SX16-,6,97,0VA8* 8+;"
		dc.b	"VL	SX16-,YB 3+,103,SY3-;"
		dc.b	"HS	1,2,1,SX21-,4,0,79,250,10;[]"
		dc.b	"VS	2,SX12-,7,10,0VA8*,0,0VA,2VA,1;[]"
		dc.b	"VS	3,SX12-,0VA8*12+,10,1VA8*,0,1VA,3VA,1;[]"
		dc.b	"BU	4,SX16-,0,16,7,0,0,1;[UN 0,0,109BP+;][BR0;]"
		dc.b	"EX;"
		dc.b	0
.Fin		even
