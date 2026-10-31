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
		XREF	_mon_cold_start_entry,_mon_nothing_entry
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
		xdef	_mon_debut
_mon_debut
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

; The interpreter registers of the task's context (GetCtx, CCall of
; src/glue.i)
MGetCtx		MACRO
		move.l	$4.w,a0
		move.l	276(a0),-(sp)
		move.l	_amos_bridge,a0
		cmpm.l	(a0)+,(sp)+
		beq.s	.Main\@
		lea	24(a0),a0
.Main\@	movem.l	(a0),d6-d7/a3-a6
		ENDM

		XREF	_mon_tests_entry,_mon_reloop_entry,_mon_quit_entry,_interp_step_instruction
		XREF	_mon_step0_entry,_mon_step1_entry,_mon_step2_entry,_mon_step3_entry
		XREF	_mon_initialise_entry,_mon_reinit_entry

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
.OMem	rts

; 	Entree du moniteur par le programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In_Program
	movem.l	a3-a6/d6/d7,-(sp)
.Ok	moveq	#0,d0
.Out	movem.l	(sp)+,a3-a6/d6/d7
.DejaLa	move.l	d0,a0

;	Boucle de tests du moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; The command loop: monitor/shell.cpp
Tt_Tests	jmp	_mon_tests_entry
Tt_ReLoop	jmp	_mon_reloop_entry

; The routines of the buttons of the view screen, of the zones of the
; text screen and of its dialogue (for the loop)
		xdef	_mon_keys,_mon_text_zones
_mon_keys	dc.l	_mon_nothing_entry			1:  STOP, teste separement
		dc.l	_mon_step0_entry		2:  mode pas a pas
		dc.l	_mon_step1_entry		3:  mode semi-rapide
		dc.l	_mon_step2_entry		4:  mode rapide
		dc.l	_mon_step3_entry		5:  mode ultra-rapide
		dc.l	_mon_move_up_entry			6:
		dc.l	_mon_move_down_entry			7:
		dc.l	_mon_move_left_entry			8:
		dc.l	_mon_move_right_entry		9:
		dc.l	_mon_change_screen_entry			10: Change ecran
		dc.l	_mon_initialise_entry		11: Init
		dc.l	_mon_quit_entry			12: Quit
		dc.l	_mon_see_entry			13: Voir l'affichage
		dc.l	_mon_set_help_entry		14: Set Help
		dc.l	_mon_set_break_entry		15: Set break
		dc.l	_mon_set_evaluate_entry		16: Set evalue
_mon_text_zones	dc.l	_mon_listing_click_entry		1 : Zone de listing
		dc.l	_mon_separation_drag_entry		2 : Zone de separation
		dc.l	_mon_info_click_entry		3 : Zone d'information
		dc.l	_mon_slider_horizontal_click_entry		1 : Slider horizontal
		dc.l	_mon_slider_vertical_click_entry		2 : Slider vertical
		dc.l	_mon_info_slider_click_entry		3 : Slider infos
		dc.l	_mon_centre_click_entry		4 : Bouton de centrage
; _mon_dispatch: monitor/shell.cpp

; ___________________________________________________________________
;
;	INITIALISATION GENERALE
; ___________________________________________________________________
;
; The monitor started and stopped: monitor/start.cpp
; (a4 the monitor's data after the start; the flags: out of memory)
Tt_ColdStart
	jsr	_mon_cold_start_entry
Tt_Free	jmp	_mon_free_entry

;	TOUCHE: quit/ Retour ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; QUIT: monitor/shell.cpp, then mon_quit_back(data) here
		xdef	_mon_quit_back
_mon_quit_back
	MGetCtx
; Retour a l'editeur ou au programme???
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Retour ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½diteur
.parun	SyCall	MemFree			Plus de datazone
; Retour au programme
.torun	tst.w	Tt_Init(a4)
.skip	move.l	d4,d0

;
; Actualisation des ecrans
TtReCop	jmp	_mon_recop_entry
Tt_Show	jmp	_mon_show_entry
Tt_Hide	jmp	_mon_hide_entry
Tt_Mouse	jmp	_mon_mouse_free_entry

; The patches on AMOS.library (Set_WPatch, EcIn, SyIn): monitor/patch.cpp

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
Ttt_Centre	jmp	_mon_listing_centre_entry
Ttt_AffBuf	jmp	_mon_listing_show_entry
Ttt_NewBuf	jmp	_mon_listing_new_entry
Ttt_Untok	jmp	_mon_listing_untokenise_entry


; ___________________________________________________________________
;
;	ECRAN DU BAS
; ___________________________________________________________________



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
Ttt_AMod	jmp	_mon_mode_buttons_entry

;	TOUCHE: une instruction
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; The step keys and INIT: monitor/shell.cpp
NoInit	jmp	_mon_no_init_entry
Tt_ReInit	jmp	_mon_reinit_entry

; Branchement aux messages de testing
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_testing
_mon_testing
Tt_Testing
	bra	Tt_Tst1
	bra	Tt_Tst2
	bra	Tt_Tst3
; Enleve le message de test
; ~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_test_nothing
_mon_test_nothing
Tt_Tst1	rts
Tt_Tst2	jmp	_mon_tested_entry
Tt_Tst3	rts

;	Entree du pas a pas...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MonitorChr
	pea	_interp_step_instruction	The instruction, its parameters collected (interp/chrget.cpp)
; Mode RAPIDE, saute tout sauf si breakpoint...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.skip	tst.w	Step_Stop(a0)
.skipA	clr.w	Step_Mode(a0)
; Mode LENT, avec affichage...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.skipB	tst.w	Tt_Init(a0)
.skipB0	clr.l	Step_Break(a0)
.skipB1	cmp.w	#_TkEndP,-2(a6)			Fin de procedure?
; Rebranche ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'instruction
.GoOut	bset	#1,T_Actualise+1(a5)		Reaffichage si long...
.GoBack	move.l	d4,d0				Recharge les registres
.Back	rts					Rebranche ÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½ l'instruction
; Localise l'instruction
.skipB2	move.l	Prg_Run(a5),a1
.stopit	tst.l	d1				Procedure fermee?
.lockee	addq.l	#2,d1
; Poke les adresses
.nolock	move.l	a0,a2
.skippy	move.l	Mon_Base(a5),a0
; The screens, the listing, the instruction: monitor/step.cpp

;	Sortie du mode pas a pas, reactive l'ancien ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; mon_step_out(), mon_step_out2(): from the C++
		xdef	_mon_step_out,_mon_step_out2
_mon_step_out
	MGetCtx
_mon_step_out2
	MGetCtx
Step_Out
	bset	#1,T_Actualise+1(a5)
Step_Out2
	bsr	Tt_PrgBanks
.Skip

	IFNE	Debug=2
	tst.w	Ttt_Mode(a4)			*** Illegal, debugging!
	bne.s	.Skip2
	JJsr	L_BugBug
.Skip2
	ENDC

	move.l	Step_Data(a4),a0


;	Entree generale des erreurs "Patch2"
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_error
_mon_error
Tt_Error
	move.l	BasSp(a5),sp
; Erreur lors de RUN
; ~~~~~~~~~~~~~~~~~~
.Mh	move.w	d0,d1
; Erreurs lors du test! (monitor/step.cpp)
.Test	jsr	_mon_test_error_entry

; Trouve le message d'erreur en retour de programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_GetTheMessage
	move.l	a1,-(sp)
.MTest	JJsrR	L_GetMessage,a1
.Clair	move.l	(sp)+,a1

Tt_Souligne	jmp	_mon_choose_entry
Tt_MouMou	jmp	_mon_mouse_position_entry

; The help: monitor/help.cpp
Hlp_FreeMap	jmp	_mon_free_map_entry

; A line split into its parts: monitor/listing.cpp
Tt_Decoup	jmp	_mon_split_entry
; Partie appelle par la detokenisation...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Tt_Decoupe, the detokeniser's hook: monitor/shell.cpp (_mon_split_hook)

;	Va evaluer une expression
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_evaluate
_mon_evaluate
Tt_Evalue
	movem.l	d1/d4-d7/a0-a5,-(sp)
Tt_ReEv	move.l	sp,Tt_EvPile(a4)	Position pile en cas de menage
;	En cas d'erreurs d'evaluation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_ev_error
_mon_ev_error
Tt_EvError
	move.l	Mon_Base(a5),a4
;	En cas de menage
; ~~~~~~~~~~~~~~~~~~~~~~
		xdef	_mon_menage
_mon_menage
Tt_Menage
	move.l	Mon_Base(a5),a4
.err	moveq	#11,d0


; ___________________________________________________________________
;
;	GESTION INFORMATIONS
; ___________________________________________________________________

; The information window: monitor/info.cpp
Inf_AffLine	jmp	_mon_info_show_line_entry
Inf_CptLines	jmp	_mon_info_count_lines_entry
Inf_AffAll	jmp	_mon_info_show_all_entry
Inf_PrDeb	jmp	_mon_info_begin_entry
Inf_Print	jmp	_mon_info_print_entry
Inf_PrFin	jmp	_mon_info_end_entry
Inf_Del	jmp	_mon_info_delete_entry
Inf_Ins	jmp	_mon_info_insert_entry
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
View_Aff	jmp	_mon_view_show_entry
View_Ec	jmp	_mon_view_next_entry

;	Calcule la table lors d'init!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
View_ScUpdate	jmp	_mon_view_sizes_entry

; View_Inter: monitor/view.cpp (mon_view_inter)


;	Activation de l'ecran de texte
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

;	XY Mouse dans l'ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

; 	Attente multitache
; ~~~~~~~~~~~~~~~~~~~~~~~~

; 	Banques de mÃÂÃÂÃÂÃÂ¯ÃÂÃÂÃÂÃÂ¿ÃÂÃÂÃÂÃÂ½moire/dialogue du moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tt_TtBanks	jmp	_mon_banks_monitor_entry
Tt_PrgBanks	jmp	_mon_banks_program_entry

;		Donnees tutor
		Even
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
