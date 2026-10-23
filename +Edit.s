; ______________________________________________________________________________
; ..............................................................................
; ...................................................................2222222....
; ................................................................22222222220...
; ...................................................222........222222.....222..
; ..............................................2202222222222..22000............
; ..................................22000.....20222222222200000200002...........
; .................................2002202...2222200222.220000000200000000022...
; ....................220002......22222200..2200002.......2200000...20000000000.
; ....................22222202....2220000022200000..........200002........200000
; .....200000.........2222200000222200220000000002..........200002........20000.
; .....00222202........2220022000000002200002000002........2000002000020000000..
; ....2222200000.......220002200000002.2000000000000222222000000..2000000002....
; ....220000200002......20000..200002..220000200000000000000002.......22........
; ...2220002.2200002....220002...22.....200002..0000000000002...................
; ...220000..222000002...20000..........200000......2222........................
; ...000000000000000000..200000..........00002..................................
; ..220000000022020000002.200002.........22.......______________________________
; ..0000002........2000000220022.................|
; .200000............2002........................| Editor
; .200002........................................| Modif
; 220002.........................................|______________________________
; ______________________________________________________________________________
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
; ______________________________________________________________________________
		Include	"+Debug.s"
		IFEQ	Debug=2
		Include "+AMOS_Includes.s"
		Include "+Version.s"
		ENDC
;_____________________________________________________________________________


; Branchements internes � l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		XREF	_amos_bridge
		XREF	_ed_store_entry,_ed_store_block_entry,_ed_delete_line_entry,_ed_delete_chunk_at_entry
		XREF	_ed_delete_chunk_entry,_ed_find_line_entry,_ed_next_line_entry
		XREF	_ed_indent_entry
		XREF	_ed_block_save_entry
		XREF	_ed_block_save_ascii_entry
		XREF	_ed_print_program_entry
		XREF	_ed_print_block_entry
		XREF	_ed_block_limits_2_entry
		XREF	_ed_printer_open_entry
		XREF	_ed_printer_print_entry
		XREF	_ed_rload_hidden_entry
		XREF	_ed_load_hidden_entry
		XREF	_ed_load_entry
		XREF	_ed_reload_entry
		XREF	_ed_name_part_entry
		XREF	_ed_get_systeme_entry
		XREF	_ed_get_message_entry
		XREF	_ed_get_message_a0_entry
		XREF	_ed_get_next_message_entry
		XREF	_ed_status_y_entry
		XREF	_ed_status_x_entry
		XREF	_ed_status_free_entry
		XREF	_ed_status_order_entry
		XREF	_ed_status_insert_entry
		XREF	_ed_status_name_entry
		XREF	_ed_name_to_buffer_entry
		XREF	_ed_recall_alert_entry
		XREF	_ed_alert_entry
		XREF	_ed_zap_alert_entry
		XREF	_ed_alert_clean_entry
		XREF	_ed_alert_rts_entry
		XREF	_ed_warn_name_entry
		XREF	_ed_warn_message_entry
		XREF	_ed_warn_entry
		XREF	_ed_warning_end_entry
		XREF	_ed_all_warnings_end_entry
		XREF	_ed_status_line_entry
		XREF	_ed_status_line_d0_entry
		XREF	_ed_status_text_entry
		XREF	_ed_merge_entry
		XREF	_ed_merge_ascii_entry
		XREF	_ed_rename_entry
		XREF	_ed_save_as_name_entry
		XREF	_ed_save_as_entry
		XREF	_ed_save_entry
		XREF	_ed_save_it_entry
		XREF	_ed_save_program_entry
		XREF	_ed_save_program2_entry
		XREF	_ed_make_bak_entry
		XREF	_ed_save_icon_entry
		XREF	_ed_procs_open_entry,_ed_procs_close_entry,_ed_procs_entry,_ed_proc_open_entry,_ed_clear_error_proc_entry
		XREF	_ed_top_up_entry
		XREF	_ed_top_down_entry
		XREF	_ed_bottom_up_entry
		XREF	_ed_bottom_down_entry
		XREF	_ed_drag_top_entry
		XREF	_ed_drag_bottom_entry
		XREF	_ed_link_cursor_entry
		XREF	_ed_unlink_scroll_entry
		XREF	_ed_follow_scroll_entry
		XREF	_ed_split_entry
		XREF	_ed_update_split_entry
		XREF	_ed_open_shown_window_entry
		XREF	_ed_open_window_entry
		XREF	_ed_hide_window_entry
		XREF	_ed_button_close_entry
		XREF	_ed_button_hide_entry
		XREF	_ed_button_size_entry
		XREF	_ed_close_window_quit_entry
		XREF	_ed_close_window_entry
		XREF	_ed_new_hidden_entry
		XREF	_ed_new_all_hidden_entry
		XREF	_ed_edit_hidden_entry
		XREF	_ed_get_hidden_entry
		XREF	_ed_delete_window_entry
		XREF	_ed_delete_windows_entry
		XREF	_ed_draw_windows_entry
		XREF	_ed_erase_windows_entry
		XREF	_ed_delete_all_zones_entry
		XREF	_ed_empty_next_entry
		XREF	_ed_next_window_entry
		XREF	_ed_previous_window_entry
		XREF	_ed_flip_size_entry
		XREF	_ed_window_number_entry
		XREF	_ed_shrink_all_entry
		XREF	_ed_max_size_entry
		XREF	_ed_activate_entry
		XREF	_ed_mouse_entry
		XREF	_ed_key_entry,_ed_key_function_entry,_ed_function_keys_entry
		XREF	_ed_marks_clear_entry,_ed_auto_marks_entry,_ed_set_mark_entry,_ed_goto_mark_entry
		XREF	_ed_marks_to_addresses_entry,_ed_marks_to_numbers_entry,_ed_marks_change_entry
		XREF	_ed_block_show_entry,_ed_block_on_entry,_ed_block_cursor_entry,_ed_block_hide_entry
		XREF	_ed_cursor_normal_entry,_ed_block_all_entry,_ed_block_store_entry,_ed_block_forget_entry
		XREF	_ed_block_cut_entry,_ed_block_paste_entry,_ed_block_free_entry,_ed_block_free_a0_entry
		XREF	_ed_block_limits_entry,_ed_block_insert_entry,_ed_block_delete_entry,_ed_block_copy_entry
		XREF	_ed_del_li_cu_entry,_ed_join_entry,_ed_return_entry,_ed_return_quiet_entry
		XREF	_ed_ins_line_entry,_ed_tok_cur_entry,_ed_tok_stok_entry,_ed_tok_stok2_entry
		XREF	_ed_cursor_up_entry,_ed_cursor_up_t_entry,_ed_cursor_down_entry,_ed_cursor_down_t_entry
		XREF	_ed_cursor_left_entry,_ed_cursor_right_entry,_ed_page_top_entry,_ed_page_bottom_entry
		XREF	_ed_word_left_key_entry,_ed_word_right_key_entry,_ed_line_start_entry,_ed_line_end_entry
		XREF	_ed_page_up_entry,_ed_page_down_entry,_ed_scroll_up_key_entry,_ed_scroll_down_key_entry
		XREF	_ed_scroll_left_key_entry,_ed_scroll_right_key_entry,_ed_text_top_entry,_ed_text_top_t_entry
		XREF	_ed_text_bottom_entry,_ed_text_bottom_t_entry,_ed_label_previous_entry,_ed_label_next_entry
		XREF	_ed_buf_untok_entry,_ed_untok_entry,_ed_cgo_entry,_ed_cdr_entry,_ed_shaut2_entry,_ed_sbas2_entry
		XREF	_ed_show_slider_entry,_ed_cursor_on_entry,_ed_cursor_off_entry,_ed_show_current_line_entry
		XREF	_ed_centre_entry,_ed_goto_x_entry,_ed_goto_y_entry,_ed_set_x_entry,_ed_set_y_entry
		XREF	_ed_set_xy_entry,_ed_labels_around_entry,_ed_place_cursor_entry,_ed_show_column_entry
		XREF	_ed_new_buffer_all_entry,_ed_new_buffer_entry,_ed_show_all_entry,_ed_show_line_entry
		XREF	_ed_show_line_only_entry
		XREF	_ed_current_line_entry,_ed_letter_entry,_ed_insert_text_entry,_ed_delete_text_entry
		XREF	_ed_word_left_entry,_ed_word_right_entry,_ed_type_key_entry,_ed_delete_key_entry
		XREF	_ed_backspace_entry,_ed_clear_line_entry,_ed_delete_to_end_entry,_ed_delete_to_start_entry
		XREF	_ed_delete_word_entry,_ed_delete_word_left_entry,_ed_tab_entry,_ed_untab_entry
		XREF	_ed_undo_start_entry,_ed_undo_line_entry,_ed_undo_advance_entry,_ed_undo_clear_entry
		XREF	_ed_undo_previous_entry,_ed_undo_next_entry,_ed_undo_create_entry,_ed_undo_reset_entry
		XREF	_ed_undo_free_entry,_ed_undo_create_all_entry,_ed_undo_reset_all_entry,_ed_undo_free_all_entry
		XREF	_ed_undo_entry,_ed_redo_entry
		XREF	_ed_search_entry,_ed_search_front_entry,_ed_search_back_entry,_ed_replace_text_entry
		XREF	_ed_tokenise_entry,_ed_detok_entry,_ed_mon_detok_entry,_ed_tinst_entry
EDebut		dc.l	Ed_Cold-EDebut
		dc.l	Ed_Title-EDebut
		dc.l	Ed_End-EDebut
		dc.l	Ed_Loop-EDebut
		dc.l	Ed_ErrRun-EDebut
		dc.l	Ed_CloseEditor-EDebut
		dc.l	Ed_KillEditor-EDebut
		dc.l	Ed_ZapFonction-EDebut
		dc.l	Ed_ZapIn-EDebut
		dc.l	Ed_RunDirect-EDebut
		dc.l	Tokenise-EDebut
		dc.l	Detok-EDebut
		dc.l	Mon_Detok-EDebut
		dc.l	TInst-EDebut
		dc.l	0
		dc.l	_amos_bridge		the C++ contexts (+B.s Program_Load)

; ______________________________________________________________________________

Ed_AutoMarksNb	equ	4
;______________________________________________________________________________
;
Ed_SlHDeltaG	equ	-2
Ed_SlHDeltaD	equ	1
Ed_SlHDeltaH	equ	2
Ed_SlHSy	equ	2
Ed_SlVDeltaG	equ	6
Ed_SlVDeltaH	equ	11
Ed_SlVSx	equ	4
Ed_SlVDeltaB	equ	-1
Ed_SlHDy	equ	2
Ed_SlVDx	equ	4

Ed_ZBoutons	equ	1000
Ed_XScrollG	equ	20
Ed_XScrollD	equ	10
Ed_YScrollH	equ	4
Ed_YScrollB	equ	4

Ed_YTop		equ	0
Ed_TitreSy	equ	16
Edt_EtatSy	equ	11
Edt_BasSy	equ	5
Edt_BtSx	equ	24

; Images dans la banque
; ~~~~~~~~~~~~~~~~~~~~~
Ed_Pics		equ	1
Ed_BtPics	equ	Ed_Pics+4
Ed_BoutonsPics	equ	Ed_BtPics+2*3
Ed_MemoryPics	equ	Ed_BoutonsPics+2*12
Es_Pics		equ	Ed_MemoryPics+3
Es_BoutonsPics	equ	Es_Pics+3

Ed_LogoSx	equ	32*5
Ed_MemorySy	equ	2
Ed_MemoryY1	equ	Ed_YTop+3
Ed_MemoryY2	equ	Ed_YTop+10
; Gros boutons
; ~~~~~~~~~~~~
Ed_BoutonsSx	equ	32
Ed_BoutonsSy	equ	16
Ed_TitleSx	equ	Ed_BoutonsSx*5
Ed_BoutonsX	equ	Ed_BoutonsSx+Ed_TitleSx
Ed_BoutonsY	equ	Ed_YTop
Ed_BoutonsZones	equ	128

; Boutons Escape
; ~~~~~~~~~~~~~~
Es_BoutonsSx	equ	32
Es_BoutonsSy	equ	16
Es_BoutonsZones	equ	1
Es_TitleSx	equ	Es_BoutonsSx*4
Es_TitleSy	equ	16
Es_BoutonsX	equ	Es_BoutonsSx+Es_TitleSx
Es_BoutonsY	equ	0
Es_MiniSy	equ	Es_TitleSy+8

; Nombre maximum de programme hidden dans le menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdM_HiddenMax	equ	12

; Numero du bouton ESCAPE
; ~~~~~~~~~~~~~~~~~~~~~~~
Bt_Insert	equ	10

; Messages
; ~~~~~~~~
Ed_Mes		equ	0		Debut des messages editeur

Ed_DiaImages	equ	66		Debut des images dialogue dans la banque
;	IfEQ	ROnly

;______________________________________________________________________________
;
;					Initialisation de l'editeur
;______________________________________________________________________________
;
Ed_Cold
	clr.b	Ed_Ok(a5)		Pas en etat de marche!
	bsr	Ed_Available
	cmp.l	#50*1024,d1		Au moins 50k de CHIP
	bcs.s	.Mem
	cmp.l	#110*1024,d0		Au moins 110k de memoire en tout!
	bcs.s	.Mem

; Warm ou Cold?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.b	Ed_Warm(a5)
	bne.s	.UnKill
; Chargement vraiment � froid
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_LoadAllConfig	Charge les fichiers
	bne.s	.Err
	bsr	Ed_Computee		Des fenetres vides
	bne.s	.Err
	bsr	Ed_OpenEditor		Ouvre l'editeur
	bne.s	.Err
	move.b	#1,Ed_Warm(a5)		Flag change!
	move.w	#-1,EsFlag(a5)		Force appear
	clr.w	Direct(a5)		Pas mode direct
	moveq	#0,d0			Pas d'erreur
	rts
; Chargement apres un KILL EDITOR
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.UnKill	bsr	Ed_LoadAllConfig	Recharge les configs
	bne.s	.Err
	moveq	#0,d0
	rts
; Une erreur
; ~~~~~~~~~~
.Mem	moveq	#1,d0
.Err	move.l	d0,-(sp)
	bsr	Ed_CloseEditor
	bsr	Ed_FreeAllConfig
	move.l	(sp)+,d0
	rts

; Apparition du titre de l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Title
	bsr	Ed_Appear
	bsr	Ed_WarmStart		Un Warmstart?
	moveq	#"A",d0
	bsr	Ed_SamPlay		Bonjour!
	bsr	Ed_About		Titre
	bsr	Ed_Loca			Curseur
	rts

; Fenetres d'origines d�ja comput�es?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Computee
	move.l	Prg_List(a5),d0
	move.l	Edt_List(a5),a4
	move.w	#2,Ed_WMax(a5)		Par default, au moins
.Loop	move.l	d0,a6
	tst.b	Prg_Edited(a6)
	bne.s	.Deja
	moveq	#-1,d0			Visible
	moveq	#-1,d1			Pas de structure PRG
	bsr	Edt_OpWindow		Va ouvrir >>> fenetre courante
	bne.s	.Err
	move.l	a6,Edt_Prg(a4)
	addq.b	#1,Prg_Edited(a6)
.Deja	move.l	Prg_Next(a6),d0
	bne.s	.Loop
	moveq	#0,d0
	rts
.Err	moveq	#1,d0			Out of memory!
	rts

; Effacement de tous les fichiers de config
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_FreeAllConfig
	bsr	Ed_SamEnd		Les sons
	bsr	EdMa_End		Les macros
	bsr	EdC_Free		Les configs
	bsr	Ed_ResourceFree		La resource
	rts
; Chargement de tous les fichiers de config
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_LoadAllConfig
; Charge la configuration de l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#7,d0
	JJsr	L_Sys_GetMessage
	JJsrR	L_Sys_AddPath,a1
	bsr	EdC_Load
	bne.s	.Err
; Charge la banque de resource
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#45,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	bsr	Ed_ResourceLoad
	bne.s	.Err
; Charge les macros par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#46,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	bsr	EdMa_Load
	bne.s	.Err
; Charge les sons par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_SamChanged
	moveq	#0,d0
.Err	rts


;______________________________________________________________________________
;
;							Fin
;______________________________________________________________________________
;
Ed_System
	JJmp	L_TheEnd
Ed_End	tst.b	Ed_Ok(a5)			Systeme installe?
	beq.s	.Skip
	bsr	Edt_DelWindows			Enleve les fenetres
.Skip	lea	Ed_Prg2ReLoad(a5),a0		Programme a recharger
	bsr	Ed_MemFree
	bsr	Ed_CloseEditor			Ferme l'editeur
	bsr	Ed_FreeAllConfig		Delete les configs
	rts

;______________________________________________________________________________
;
;							CLOSE/OPEN EDITOR
;______________________________________________________________________________
;

Ed_OpenEditor
	movem.l	d1-d7/a0-a6,-(sp)
	tst.b	Ed_Opened(a5)
	bne.s	.Ok

	bsr	Ed_OpenIt		Premier essai
	beq.s	.Ok

	bsr	Ed_CloseIt		Out of mem, on efface
	bsr	EdM_Program		Menus du programme
	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_SetBanks		Change les banques
	JJsr	L_MemMaximum		Nettoie au maximum!

	bsr	Ed_OpenIt		Deuxieme essai!
	beq.s	.Ok
	bsr	Ed_CloseIt
	bsr	EdM_Program
	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_SetBanks		Change les banques
	JJsr	L_MemDelBanks		Ligne: effacer les banques?
	beq	Ed_System		NON: on sort!
	JJsr	L_Bnk.EffAll		Efface toutes les banques
	JJsr	L_Bnk.Change		Changement de banques

	bsr	Ed_OpenIt
	beq.s	.Ok
	bsr	Ed_CloseIt
	moveq	#1,d0			Out of mem
	bra.s	.Out
.Ok	move.b	#1,Ed_Opened(a5)	Flag!
	moveq	#0,d0			C'est bon!
.Out	movem.l	(sp)+,d1-d7/a0-a6
	rts

; Routine d'ouverture de l'editeur...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_OpenIt
	bsr	Ed_SetBanks
	bsr	EdM_Editor
; Verification de la taille des fenetres / Config
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.b	EdC_Modified(a5)
	beq	.ModX
	clr.b	EdC_Modified(a5)
	move.l	Edt_Current(a5),d0	Des fenetres?
	beq	.ModX
; Verification de la taille en Y
	move.l	d0,a4			Fenetre courante
	moveq	#-1,d0			Sans toucher les autres
	bsr	Edt_WMaxSize
	cmp.w	Edt_WindTy(a4),d0	Pareil?
	beq.s	.Mod5
	moveq	#0,d0			Reduit les autres
	bsr	Edt_WSchrinkAll
	moveq	#-1,d0
	bsr	Edt_WMaxSize		Taille maxi acceptable
	move.w	d0,Edt_WindTy(a4)
	bgt	.Mod5			Ok!
.Mod2	move.w	#1,Edt_WindTy(a4)	Taille mini!
	moveq	#Ed_TitreSy,d1		Compte la taille necessaire!
	move.l	Edt_List(a5),d0		Toute la liste
.Mod3	move.l	d0,a0
	tst.b	Edt_Hidden(a0)		Deja cachee
	bne.s	.Mod4
	add.w	#Edt_EtatSy+Edt_BasSy,d1
	move.w	Edt_WindTy(a0),d0
	lsl.w	#3,d0
	add.w	d0,d1
.Mod4	move.l	Edt_Next(a0),d0
	bne.s	.Mod3
	move.w	d1,Ed_Sy(a5)		Change la config!!!!
; Verification de la position du curseur en X
.Mod5	move.w	Ed_Sx(a5),d1		Derniere limite curseur
	lsr.w	#3,d1
	subq.w	#3,d1
	move.l	Edt_List(a5),d0		Toute la liste
.Mod6	move.l	d0,a0
	tst.b	Edt_Hidden(a0)		Deja cachee
	bne.s	.Mod7
	move.w	Edt_XCu(a0),d0
	cmp.w	d1,d0
	bcs.s	.Mod7
	move.w	d1,Edt_XCu(a0)
	sub.w	d1,d0
	add.w	d0,Edt_XPos(a0)
.Mod7	move.l	Edt_Next(a0),d0
	bne.s	.Mod6
.ModX
	bsr	EdC_SetPalette		Update de la palette

	moveq	#EcFonc,d0		Ouverture de l'ecran de fonctions
	move.w	Ed_Sx(a5),d1
	ext.l	d1
	moveq	#Es_TitleSy,d2
	moveq	#0,d3
	move.l	Ed_Resource(a5),a0
	add.l	2(a0),a0
	moveq	#0,d4
	JJsrR	L_Dia_RScOpen,a1
	bne	.Error
	bset	#BitHide,EcFlags(a0)
	move.w	Ed_Wx(a5),EcAWX(a0)
	moveq	#20,d1
	SyCall	ResZone
	bne	.Error

	moveq	#EcEdit,d0		Ouverture de l'ecran principal
	move.w	Ed_Sx(a5),d1
	ext.l	d1
	move.w	Ed_Sy(a5),d2
	addq.w	#7,d2
	and.w	#$FFF8,d2
	move.w	d2,d3
	sub.w	#16,d3
	sub.w	#Ed_YTop,d3
	lsr.w	#3,d3
	move.w	d3,Ed_Ty(a5)
	ext.l	d2
	moveq	#1,d3
	moveq	#0,d4			Interlaced?
	move.b	Ed_Inter(a5),d4
	move.l	Ed_Resource(a5),a0
	add.l	2(a0),a0
	JJsrR	L_Dia_RScOpen,a1
	bne.s	.Error
	bset	#BitHide,EcFlags(a0)
	move.w	Ed_Wx(a5),EcAWX(a0)
	move.w	Ed_Wy(a5),EcAWY(a0)
	moveq	#10,d0
	bsr	Et_Print

	move.w	Ed_Ty(a5),d0		Buffer d'edition
	mulu	#256,d0
	SyCall	MemFast
	move.l	a0,Ed_BufE(a5)
	beq	.Error

	moveq	#0,d0			Nombre maximum de programmes
	move.w	Ed_Ty(a5),d0
	subq.w	#6,d0
	divu	#3,d0
	move.w	d0,Ed_WMax(a5)

	jsr	Tok_Init		Les tables de tokenisation
	bne.s	.Error
	bsr	Ed_InitDialogues	Les dialogues
	bne.s	.Error
	bsr	EdM_Init		Cree les menus
	bne.s	.Error
	bsr	Prg_CreateUndos		Les buffers UNDOS de programmes

; Pas d'erreur
	clr.b	EdC_Modified(a5)	Config adaptee...
	moveq	#0,d0			Pas d'erreur!
	rts
; Out of memory
.Error	moveq	#1,d0			Out of memory
	rts


; 	Fermeture generale de l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_KillEditor
	bsr	Ed_CloseEditor		Plus rien
	bsr	Ed_FreeAllConfig	Plus aucune configuration
	clr.b	Ed_Ok(a5)		Plus en etat de marche...
	rts

;	Fermeture de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_CloseEditor
	tst.b	Ed_Opened(a5)
	beq.s	.Skip
	clr.b	Ed_Opened(a5)
	movem.l	d0-d7/a0-a6,-(sp)
	bsr	Ed_CloseIt
	movem.l	(sp)+,d0-d7/a0-a6
.Skip	rts

; Routine de fermeture..
; ~~~~~~~~~~~~~~~~~~~~~~
Ed_CloseIt
	bsr	Ed_SetBanks		Banques de l'editeur
	bsr	Prg_FreeUndos		Plus de buffer UNDOS
	bsr	Esc_KMemEnd		Plus de touches de mode direct
	bsr	Ed_AllAverFin		Plus d'avertissement
	bsr	Ed_BlocFree		Plus de bloc
	bsr	EdM_End			Plus de menus
	bsr	Ed_EndDialogues		Plus de dialogues
	bsr	Edt_DelAllZones		Plus de zones de fenetres
	EcCalD	Del,EcEdit		Plus d'ecrans
	EcCalD	Del,EcFonc
	jsr	Tok_Del			Plus de tokenisation
	move.l	Ed_BufE(a5),d0		Plus de buffer d'edition
	beq.s	.Skip
	clr.l	Ed_BufE(a5)
	move.l	d0,a1
	move.w	Ed_Ty(a5),d0
	mulu	#256,d0
	SyCall	MemFree
.Skip	rts

; Retourne la memoire libre dans la machine
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Available
	movem.l	a0/a1/a6,-(sp)
	move.l	$4.w,a6
	move.l	#Public|Fast,d1
	jsr	_LVOAvailMem(a6)
	move.l	d0,d2
	move.l	#Public|Chip,d1
	jsr	_LVOAvailMem(a6)
	move.l	d0,d1
	add.l	d2,d0
	movem.l	(sp)+,a0/a1/a6
	rts

;______________________________________________________________________________
;
; 			WARM START Charge le fichier / Prepare l'editeur
;______________________________________________________________________________
;
Ed_WarmStart
; Fichier "Last Cession" present?
	moveq	#47,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	JJsr	L_RExist
	beq	.Out
; Message
	move.w	#159,d0
	bsr	Ed_AverMess
; Efface la premiere fenetre systeme
	move.l	Edt_List(a5),d0
	beq.s	.No1
	move.l	d0,a4
	bsr	Edt_DelWindow
.No1
; PHASE 1: re-creation des listes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#1005,d2		Ouvre le fichier
	bsr	Ed_Open
	beq	.Err1
; Verifie l'entete
	move.l	Buffer(a5),a3
	move.l	a3,d2
	moveq	#8,d3
	bsr	Ed_Read
	bne	.Err0
	cmp.l	#Ed_QuitHead,(a3)
	bne	.Err0
; Charge dans un buffer
	move.l	4(a3),d0		Taille necessaire
	JJsr	L_ResTempBuffer
	beq	.Err0
	move.l	a0,d2
	move.l	4(a3),d3
	bsr	Ed_Read
	bne	.Err0
	bsr	Ed_Close
; Trouve les structures
	move.l	TempBuffer(a5),a6	A6= Structures programmes
	move.l	a6,a4			A4= Structures edition
	moveq	#-8,d0
.GLoop	lea	8(a4,d0.l),a4
	move.l	4(a4),d0
	tst.l	(a4)+
	bne.s	.GLoop
; Recree la liste PRG
	lea	Prg_List(a5),a2
	move.l	a6,a3
.PCree	move.l	#Prg_Long,d0		Reserve la memoire
	SyCall	MemFastClear
	beq	.Err
	move.l	a0,Prg_Next(a2)		Branche dans la liste
	move.l	a0,a2
	move.l	a0,8(a3)		Stocke l'adresse
	moveq	#Prg_Long/2-1,d0
	lea	12(a3),a1
.PCop	move.w	(a1)+,(a0)+		Copie les datas
	dbra	d0,.PCop
	move.l	Prg_StTTexte(a2),Prg_StBas(a2)
	clr.l	Prg_StTTexte(a2)
	clr.l	Prg_Dialogs(a2)		Erase donnees importantes!
	clr.l	Prg_Banks(a2)
	clr.l	Prg_Undo(a2)
	clr.l	Prg_AdEProc(a2)
	clr.l	Prg_ZapData(a2)
	clr.l	Prg_RunData(a2)
	clr.l	Prg_Previous(a2)
	move.l	4(a3),d0		Encore un?
	lea	12(a3,d0.l),a3
	tst.l	(a3)
	bne.s	.PCree
; Recree la liste EDT
	lea	Edt_List(a5),a2
	move.l	a4,a3
.ECree	move.l	#Edt_Long,d0		Reserve la memoire
	SyCall	MemFastClear
	beq	.Err
	move.l	a0,Edt_Next(a2)		Branche dans la liste
	move.l	a0,a2
	move.l	a0,8(a3)		Change le pointeur
	moveq	#Edt_Long/2-1,d0
	lea	12(a3),a1
.ECop	move.w	(a1)+,(a0)+		Copie les datas
	dbra	d0,.ECop
	move.l	4(a3),d0		Encore un?
	lea	12(a3,d0.l),a3
	tst.l	(a3)
	bne.s	.ECree
; Relinke les fenetres
	move.l	Edt_List(a5),a3
.LLoop	lea	Edt_Prg(a3),a0		Structure programme
	move.l	a6,a1			Explorer les programmes
	bsr	.Linke
	lea	Edt_LinkPrev(a3),a0	Link previous
	move.l	a4,a1
	bsr	.Linke
	lea	Edt_LinkNext(a3),a0	Link next
	move.l	a4,a1
	bsr	.Linke
	lea	Edt_LinkScroll(a3),a0	Link de scroll
	move.l	a4,a1
	bsr	.Linke
	tst.w	Edt_Order(a3)		Programme courant?
	beq.s	.Pac
	move.l	a3,Edt_Current(a5)
.Pac	move.l	Edt_Next(a3),d0
	move.l	d0,a3
	bne.s	.LLoop
; On peut effacer le buffer!
	moveq	#0,d0
	JJsr	L_ResTempBuffer

; PHASE 2 : recharge tous les programmes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Prg_List(a5),a6
.CLoop	lea	Prg_NamePrg(a6),a0	Recopie le nom
	move.l	Name1(a5),a1
	bsr	EdCocop
	move.l	Prg_StBas(a6),d0	Change la taille du buffer
	JJsr	L_Prg_ChgTTexte
	beq	.Err
	moveq	#1,d0			Charge sans adapter
	JJsr	L_Prg_Load
	bne	.Err
	tst.b	Prg_NoNamed(a6)		Enleve le nom si necessaire
	beq.s	.Name
	clr.b	Prg_NamePrg(a6)
	clr.b	Prg_NoNamed(a6)
	move.b	#1,Prg_Change(a6)	Force une sauvegarde si close...
	move.l	a6,-(sp)
	move.l	Name1(a5),d1		Efface, si possible
	move.l	DosBase(a5),a6		Sans verification...
	jsr	_LVODeleteFile(a6)
	move.l	(sp)+,a6
.Name	move.l	Prg_Next(a6),d0
	move.l	d0,a6
	bne.s	.CLoop

; TERMINE: efface le fichier, puis raffiche les fenetres
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	.DelFichier
	bsr	Prg_CreateUndos
	bsr	EdM_BranchAMOS
	bsr	Ed_DrawWindows
.Out	rts


; Destruction du fichier d'informations
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.DelFichier
	bsr	Ed_Close
	moveq	#47,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	move.l	Name1(a5),d1
	move.l	DosBase(a5),a6
	jsr	_LVODeleteFile(a6)
	tst.l	d0
	beq.s	.DelPa
	rts
.DelPa	moveq	#EdD_WarmDel,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	beq.s	.DelFichier
; Efface tout, puis FATAL!
	bsr	.Err
	bra	Ed_System

; Change les pointeurs de fenetre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Linke	move.l	(a0),d0
	beq.s	.PoX
.PoL	cmp.l	(a1),d0
	beq.s	.PoF
	move.l	4(a1),d1
	lea	12(a1,d1.w),a1
	tst.l	(a1)
	bne.s	.PoL
.PoF	move.l	8(a1),(a0)
.PoX	rts

; Erreur pendant le rechargement: efface tout!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Err
; Les programmes
	move.l	Prg_List(a5),d6
	bra.s	.EPrg1
.EPrg0	move.l	d6,a6
	move.l	Prg_Next(a6),d6		Stocke le suivant
	moveq	#0,d0			Buffer eventuellement reserve
	JJsr	L_Prg_ChgTTexte
	move.l	Prg_Banks(a6),Cur_Banks(a5)
	JJsr	L_Bnk.EffAll		Banques eventuellement reservees
	move.l	a6,a1			Libere la memoire
	move.l	#Prg_Long,d0
	SyCall	SyFree
.EPrg1	tst.l	d6			Encore un?
	bne.s	.EPrg0
	clr.l	Prg_List(a5)
; Les fenetres
	move.l	Edt_List(a5),d4
	bra.s	.EEdt1
.EEdt0	move.l	d4,a4
	move.l	Edt_Next(a4),d4		Stocke le suivant
	move.l	a4,a1			Libere la memoire
	move.l	#Edt_Long,d0
	SyCall	SyFree
.EEdt1	tst.l	d4			Encore un?
	bne.s	.EEdt0
	clr.l	Edt_List(a5)
; Va effacer le fichier
.Err1	bsr	.DelFichier
; Ferme le fichier courant
.Err0	bsr	Ed_Close		Ferme le fichier
	moveq	#0,d0			Efface le buffer
	JJsr	L_ResTempBuffer
; ROuvre la premiere fenetre
	clr.l	Edt_Current(a5)
	moveq	#-1,d0			Visible
	move.l	PI_DefSize(a5),d1	Taille
	sub.l	a4,a4			Pas de fenetre courante
	bsr	Edt_OpWindow
	bsr	EdM_BranchAMOS
	bsr	Ed_DrawWindows
; Boite de dialogue
	moveq	#EdD_WarmErr,d0
	bsr	Ed_Dialogue
	rts

;______________________________________________________________________________
;
;						Dessin du titre
;______________________________________________________________________________
;
Ed_DrawTop

	movem.l	a2/d2-d7,-(sp)

;
; Reserve les zones
; ~~~~~~~~~~~~~~~~~
	move.l	#Ed_BoutonsZones+15,d1
	SyCall	ResZone
	move.w	Ed_Sx(a5),d7

; Unpack le logo AMOS
; ~~~~~~~~~~~~~~~~~~~
	moveq	#Ed_Pics,d0
	moveq	#Ed_BoutonsSx,d1
	moveq	#Ed_YTop,d2
	bsr	Ed_Unpack
	sub.w	#Ed_TitleSx,d7
;
; Cree les 12 boutons
; ~~~~~~~~~~~~~~~~~~~
	lea	Ed_Boutons(a5),a2
	move.w	#Ed_BoutonsX,d2
	moveq	#Ed_BoutonsY,d3
	moveq	#Ed_BoutonsPics,d4
	move.w	#Ed_BoutonsZones,d5
	moveq	#1,d6
.Loop	move.w	d6,Bt_Number(a2)
	cmp.w	#1,d6
	bne.s	.Pa1
	clr.w	Bt_X(a2)			Bouton DIRECT
	bra.s	.PaB
.Pa1	cmp.w	#2,d6
	bne.s	.Pa2
	move.w	Ed_Sx(a5),Bt_X(a2)		Bouton WB
	sub.w	#Ed_BoutonsSx,Bt_X(a2)
	bra.s	.PaB
.Pa2	move.w	d2,Bt_X(a2)
	add.w	#Ed_BoutonsSx,d2
.PaB	move.w	d3,Bt_Y(a2)
	move.w	d4,Bt_Image(a2)
	move.w	d5,Bt_Zone(a2)
	lea	Bt_RoutIn(pc),a0
	move.l	a0,Bt_Routines(a2)
	move.b	#6,Bt_RDraw(a2)
	clr.b	Bt_RPos(a2)
	clr.b	Bt_RChange(a2)
	clr.b	Bt_Flags(a2)
	clr.w	Bt_Pos(a2)
	cmp.w	#Bt_Insert,d6			Bouton INSERT?
	bne.s	.PaI
	bset	#Bt_FlagOnOf,Bt_Flags(a2)
	tst.b	Ed_Insert(a5)
	bne.s	.PaI
	move.w	#1,Bt_Pos(a2)
.PaI	move.b	#Ed_BoutonsSx,Bt_Sx(a2)
	move.b	#Ed_BoutonsSy,Bt_Sy(a2)
	addq.w	#2,d4
	addq.w	#1,d5
	move.l	a2,a0
	moveq	#1,d0
	JJsrR	L_Bt_Init,a1
	sub.w	#Ed_BoutonsSx,d7
	lea	Bt_Long(a2),a2
	addq.w	#1,d6
	cmp.w	#12,d6
	bls	.Loop
	clr.w	(a2)

; Dessine les sliders de memoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	d2,d1
	moveq	#Ed_YTop,d2
	move.w	d7,d3
	lea	Ed_Unpack(pc),a2
	bsr	Ed_MemoryDraw
	bsr	Ed_MemoryAff

; Fini!
; ~~~~~
	movem.l	(sp)+,a2/d2-d7
	rts

; Dessine les sliders de memoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1=	X
;	D2=	Y
;	D3=	Largeur
;	A2= 	Adresse UNPACK
Ed_MemoryDraw
	move.w	d3,d0
	sub.w	#Ed_BoutonsSx+7,d0
	move.w	d0,Ed_MemorySx(a5)

	moveq	#Ed_MemoryPics,d0	La gauche
	jsr	(a2)
	add.w	#Ed_BoutonsSx,d1
 	move.w	d1,Ed_MemoryX(a5)

	divu	#Ed_BoutonsSx,d3
	subq.w	#2+1,d3
.Loop	moveq	#Ed_MemoryPics+1,d0
	jsr	(a2)
	add.w	#Ed_BoutonsSx,d1
	dbra	d3,.Loop
	moveq	#Ed_MemoryPics+2,d0
	jmp	(a2)

; Dessine le contenu des sliders de memoire.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_MemoryAff
	move.l	a6,-(sp)
	move.l	$4.w,a6
; Dessin du slider de CHIP MEM
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#Chip|Public,d1
	jsr	_LVOAvailMem(a6)
	move.l	d0,-(sp)
	move.l	MemChipTotal(a5),d4
	move.l	d4,d3
	sub.l	d0,d3
	move.w	Ed_MemoryX(a5),d1
	move.w	#Ed_MemoryY1,d2
	bsr	.Memory
; Dessin du slider de FAST MEM
	move.l	#Public|Fast,d1
	jsr	_LVOAvailMem(a6)
	add.l	d0,(sp)
	move.l	MemFastTotal(a5),d4
	move.l	d4,d3
	sub.l	d0,d3
	move.w	Ed_MemoryX(a5),d1
	move.w	#Ed_MemoryY2,d2
	bsr	.Memory
; Stocke le resultat memoire globale
	move.l	(sp)+,Ed_MemCurrent(a5)
	move.l	(sp)+,a6
	rts

; Routine de dessin
; ~~~~~~~~~~~~~~~~~
.Memory
; Une place dans le buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Buffer(a5),a2
	move.l	a2,a0
	moveq	#Sl_Long-1,d0
.Loop	clr.b	(a0)+
	dbra	d0,.Loop
; Longueur du slider
	lsr.l	#8,d4
	lsr.l	#8,d3
	move.w	d4,Sl_Global(a2)
	move.w	d3,Sl_Window(a2)
; Position
	move.w	d1,Sl_X(a2)
	move.w	d2,Sl_Y(a2)
	move.w	Ed_MemorySx(a5),Sl_Sx(a2)
	move.w	#Ed_MemorySy,Sl_Sy(a2)
; Couleurs
	moveq	#23,d0
	bsr	Ed_GetSysteme
	lea	Sl_Inactive(a2),a1
	moveq	#16-1,d0
.Co	moveq	#0,d1
	move.b	(a0)+,d1
	move.w	d1,(a1)+
	dbra	d0,.Co
; Appelle le dessin
	move.l	a2,a0
	JJsrR	L_Sl_Init,a1
	rts

;______________________________________________________________________________
;
;						Boucle principale de l'�diteur
;______________________________________________________________________________
;
		xdef	_ed_loop
_ed_loop
Ed_Loop

	move.l	BasSp(a5),sp
	EcCalD	Active,EcEdit

; Flag: editeur en etat de marche!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.b	#1,Ed_Ok(a5)
 	move.w	#1,T_AMOState(a5)	Editeur present!

; Efface une fenetre en rab
; __________________________
;
	move.l	Ed_WindowToDel(a5),d0
	beq.s	.PaDel
	move.l	d0,a4
	clr.l	Ed_WindowToDel(a5)
	tst.b	Edt_Hidden(a4)
	bne.s	.Visi
	bsr	Ed_CloseWindow
	bra	Ed_Loop
.Visi	bsr	Edt_DelWindow
	bsr	EdM_BranchAMOS
	bra	Ed_Loop
.PaDel
;
; Eventuellement, efface tous les avertissements
; ______________________________________________
;
	bsr	Ed_AllAverFin
;
; Recharge un programme apres PRG COMMAND
; _______________________________________
;
	move.l	Ed_Prg2ReLoad(a5),d0
	bne	Ed_PrgReLoad

	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	move.b	Prg_MathFlags(a6),MathFlags(a5)
;
; Securites
; __________________________
;
	clr.b	Ed_FUndo(a5)		Undo
	clr.b	Ed_RunnedHidden(a5)

;
; Gestion des multi windows
; ___________________________
;
	moveq	#0,d0
	move.l	a4,a0
	or.b	Ed_SCallFlags(a5),d0
	bsr	Ed_Splitted
	clr.b	Ed_SCallFlags(a5)
;
; Gestion des scrollings lies
; _____________________________
;
	bsr	Ed_LinkeScroll
;
; Gestion du curseur avec un bloc
; _______________________________
;
	bsr	Ed_BlocAff
;
; Gestion de la fenetre d'informations pour toutes les fenetres
; _____________________________________________________________
;
; Exploration de la liste
	move.l	Edt_List(a5),d0
.Liip	move.l	d0,a4
	tst.b	Edt_Hidden(a4)
	bne.s	.Naxt
	move.l	Edt_Prg(a4),a6
; Affichage general
; ~~~~~~~~~~~~~~~~
	move.b	Ed_EtatAff(a5),d0
	or.b	d0,Edt_EtatAff(a4)
; Efface le message d'alerte?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.w	Edt_EtMess(a4)
	beq.s	.PaMess
	subq.w	#1,Edt_EtMess(a4)
	bne.s	.PaMess
	move.b	#EtA_BAll,Edt_EtatAff(a4)
.PaMess
; Va reafficher la fenetre
; ~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_EtPrintD0
; Affiche le slider
; ~~~~~~~~~~~~~~~~~
	tst.b	Edt_ASlY(a4)
	beq.s	.PaSlY
	subq.b	#1,Edt_ASlY(a4)
	bne.s	.PaSlY
	bsr	Ed_AffSlV
.PaSlY
; Suivant
; ~~~~~~~
.Naxt	move.l	Edt_Next(a4),d0
	bne.s	.Liip
	clr.b	Ed_EtatAff(a5)

;
; Demarre les menus si pas definis
; ________________________________
;
	tst.b	Ed_Zappeuse(a5)			Pas en zappeuse
	bne.s	.Menula
	cmp.l	#1024*72,Ed_MemCurrent(a5)	Si moins de 72k
	bcc.s	.Domenu
	tst.b	Ed_NewAppear(a5)		Peut-on essayer un clearvar?
	beq.s	.Menula
	subq.b	#1,Ed_NewAppear(a5)
	bne.s	.Deuse
; Fait un peu de place
	JJsr	L_Mon_Free			Plus de moniteur
	JJsr	L_Math_Close			Plus de librairie math
	bsr	Prg_RazUndos			Plus d'UNDO
	SyCall	MemFlush			Flush la memoire...
; Essaie d'ouvrir les menus...
.Deuse	move.w	#186,d0				Low memory!
	bsr	Ed_GetMessage
	moveq	#125,d0
	bsr	Ed_AlertRts
	clr.b	Ed_Sounds(a5)			Plus de sons
	bsr	Ed_SamChanged
	bsr	EdM_Program
	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_SetBanks
	JJsr	L_MemMaximum
.Domenu	tst.l	EdM_Table(a5)			Les menus sont la?
	bne.s	.Menula
	bsr	EdM_Init
	bra	Ed_Loop
.Menula

;
; Gestion du programme courant
; ____________________________________
;
	move.l	Edt_Current(a5),a4		Fenetre
	move.l	Edt_Prg(a4),a6
	move.w	Edt_Window(a4),d1
	WiCall	QWindow
	tst.b	Ed_CuFlag(a5)			Curseur
	bne.s	.PaCu
	move.b	#1,Ed_CuFlag(a5)
	bsr	Ed_CuOn
.PaCu

;
; Scrolling pour recentrer le curseur?
; ____________________________________
;
; 1. A gauche
; ~~~~~~~~~~~
	move.w	Edt_XPos(a4),d0
	move.w	Edt_XCu(a4),d1
	sub.w	d0,d1
	cmp.w	#Ed_XScrollG,d1
	bcc.s	.SkipL
	tst.w	d0
	beq.s	.SkipL
	bsr	Ed_CuOff
	bsr	Ed_CGo
	bra	Ed_Loop
.SkipL
; 2. A droite
; ~~~~~~~~~~~
	move.w	Edt_WindTx(a4),d1
	sub.w	Edt_XCu(a4),d1
	add.w	d0,d1
	cmp.w	#Ed_XScrollD,d1
	bcc.s	.SkipD
	cmp.w	#200,d0
	bcc.s	.SkipD
	bsr	Ed_CuOff
	bsr	Ed_CDr
	bra	Ed_Loop
.SkipD
; Haut / Bas, calcule les tailles
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	Edt_WindTy(a4),d1
	cmp.w	#2,d1
	bls.s	.SEnd
	moveq	#1,d2
	moveq	#1,d3
	cmp.w	#7,d1
	bcs.s	.SkipT
	moveq	#Ed_YScrollH/2,d2
	moveq	#Ed_YScrollB/2,d3
	cmp.w	#(Ed_YScrollH+Ed_YScrollB)*2,d1
	bcs.s	.SkipT
	move.w	d1,d2
	mulu	#Ed_YScrollH,d2
	divu	#25,d2
	move.w	d1,d3
	mulu	#Ed_YScrollB,d3
	divu	#25,d3
.SkipT
; 3. En haut.
; ~~~~~~~~~~~
	cmp.w	Edt_YCu(a4),d2
	bls.s	.SkipH
	tst.w	Edt_YPos(a4)
	beq.s	.SkipH
	bsr	Ed_CuOff
	bsr	Ed_SHaut2
	addq.w	#1,Edt_YCu(a4)
	bsr	Ed_Loca
	bra	Ed_Loop
.SkipH
; 4. En bas
; ~~~~~~~~~
	move.w	Edt_WindTy(a4),d0
	sub.w	Edt_YCu(a4),d0
	cmp.w	d0,d3
	bcs.s	.SEnd
	move.w	Edt_YPos(a4),d0
	add.w	Edt_YCu(a4),d0
	cmp.w	Prg_NLigne(a6),d0
	bcc	.SEnd
	bsr	Ed_CuOff
	bsr	Ed_SBas2
	subq.w	#1,Edt_YCu(a4)
	bsr	Ed_Loca
	bra	Ed_Loop
.SEnd

;
; Un programme en t�l�commande?
; _____________________________
;
	tst.b	Ed_Zappeuse(a5)
	beq.s	.PaProg
	addq.w	#1,Ed_ZapCounter(a5)	Attend que l'editeur soit stabilis�
	cmp.w	#5,Ed_ZapCounter(a5)
	bcs	Ed_Loop
	clr.w	Ed_ZapCounter(a5)
	bra	Ed_ZapOut
.PaProg
;
; Gestion du clavier
; __________________
;
	bsr	Ed_Key
	bne	Ed_Loop

;
; Gestion de la souris
; ____________________
;
	bsr	Ed_Mouse
	bne	Ed_Loop

;
; Reafficher le slider de memoire?
; ________________________________
;
	move.w	T_VblCount+2(a5),d0
	and.w	#63,d0
	bne.s	.Skip
	bsr	Ed_MemoryAff
	bra	Ed_Loop
.Skip
;
; Autosave?
;____________
;
	move.l	Ed_AutoSave(a5),d0
	beq.s	.PaAS
	tst.l	Ed_AutoSaveRef(a5)
	bpl.s	.Pan
	move.l	T_VblCount(a5),Ed_AutoSaveRef(a5)
.Pan	move.l	T_VblCount(a5),d1
	sub.l	Ed_AutoSaveRef(a5),d1
	cmp.l	d0,d1
	bcs.s	.PaAS
	move.l	Edt_Current(a5),a4
	tst.w	Edt_LEdited(a4)		Seulement si ligne non editee
	bne.s	.PaAS
	move.l	#-1,Ed_AutoSaveRef(a5)
	bsr	Ed_SaveAuto
	bra	Ed_Loop
.PaAS
;
; Multitache!
; ___________
;
	JJsr	L_Sys_WaitMul
	bra	Ed_Loop


; __________________________________________________________________________
;
; 	Teste la souris
; __________________________
;
; The mouse: src/editor/mouse.cpp
Ed_Mouse	jmp	_ed_mouse_entry
; ____________________________
;
; The separations of the windows: src/editor/windows.cpp
Ed_EtatUp	jmp	_ed_top_up_entry
Ed_EtatDown	jmp	_ed_top_down_entry
Ed_BasUp	jmp	_ed_bottom_up_entry
Ed_BasDown	jmp	_ed_bottom_down_entry
		xdef	_ed_separation_top
_ed_separation_top
Ed_MSepHaut	jmp	_ed_drag_top_entry
		xdef	_ed_separation_bottom
_ed_separation_bottom
Ed_MSepBas	jmp	_ed_drag_bottom_entry


; __________________________________________________________________________
;
; 	Teste le clavier
; __________________________
;
; The keys: src/editor/keys.cpp
Ed_Key		jmp	_ed_key_entry
Ed_KEnd		rts


; Appelle le menu editeur et retourne le choix, si choix
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_menu
_ed_menu
Ed_MnGere
	bsr	EdM_Editor
	tst.l	MnBase(a5)		Menus definis?
	beq.s	.Err
	move.l	a6,-(sp)
	move.l	$4.w,a6
	move.l	#Public,d1
	jsr	_LVOAvailMem(a6)	Au moins 40k de libre!
	move.l	(sp)+,a6
	cmp.l	#40*1024,d0
	bcs.s	.Err
	moveq	#"H",d0			Va faire du bruit!
	bsr	Ed_SamPlay
	JJsr	L_MnGere
	bne.s	.Err
	moveq	#"H",d0			Va faire du bruit!
	bsr	Ed_SamPlay
	tst.w	MnChoice(a5)
	beq	.NoChoix
	clr.w	MnChoice(a5)
	lea	MnChoix(a5),a0
	moveq	#3,d1
.MnL	lsl.l	#8,d0
	addq.l	#1,a0
	move.b	(a0)+,d0
	dbra	d1,.MnL
	move.l	EdM_Table(a5),a0
	bra.s	.MnM1
.MnM0	addq.l	#8,a0
.MnM1	move.l	(a0),d1
	beq.s	.NoChoix
	cmp.l	d0,d1
	bne.s	.MnM0
	moveq	#0,d2
	move.b	6(a0),d2
	subq.w	#1,d2
	bmi	.NoChoix
	moveq	#1,d0
	rts
; Pas de choix finalement....
.NoChoix
	moveq	#0,d0
	rts
; Erreur out of memory
.Err	moveq	#-1,d0
	rts

Ed_Ky2Fonc	jmp	_ed_key_function_entry

Ed_Fonc2Ky	jmp	_ed_function_keys_entry

; Ed_PKey (d7 the character, d6 insert), R_InsChar, R_DelChar:
; src/editor/lines.cpp
Ed_PKey		jmp	_ed_type_key_entry

R_InsChar	jmp	_ed_insert_text_entry
R_DelChar	jmp	_ed_delete_text_entry

; _________________________________________________________________________
;
; 	GESTION UNDO MULTIPLE
; _________________________________________________________________________
;


; Undo, Redo and the undo buffers: src/editor/undo.cpp
Ed_Undo		jmp	_ed_undo_entry
Ed_Redo		jmp	_ed_redo_entry
Prg_CreateUndos	jmp	_ed_undo_create_all_entry
Prg_FreeUndos	jmp	_ed_undo_free_all_entry
Prg_RazUndos	jmp	_ed_undo_reset_all_entry
Prg_UndoCreate	jmp	_ed_undo_create_entry
Prg_UndoRaz	jmp	_ed_undo_reset_entry
Prg_UndoFree	jmp	_ed_undo_free_entry


; What undo and redo do for each kind of change: src/editor/lines.cpp

; Un_CLine, Un_Debut, Un_Avance, Un_Del, Un_Prev: src/editor/undo.cpp
Un_CLine	jmp	_ed_undo_line_entry
Un_Debut	jmp	_ed_undo_start_entry
Un_Avance	jmp	_ed_undo_advance_entry
Un_Del		jmp	_ed_undo_clear_entry
Un_Prev		jmp	_ed_undo_previous_entry
Un_Next		jmp	_ed_undo_next_entry
; __________________________________________________________________________
;



; __________________________________________________________________________
;
;	Linked and split windows: src/editor/windows.cpp
Ed_LinkCursor	jmp	_ed_link_cursor_entry
Edt_DelLinkScroll	jmp	_ed_unlink_scroll_entry
Ed_LinkeScroll	jmp	_ed_follow_scroll_entry
Ed_SplitWindow	jmp	_ed_split_entry
Ed_Splitted	jmp	_ed_update_split_entry


; ___________________________________________________________________
;
; 	APPEL D'UNE FONCTION DE L'EDITEUR
;
;	D2	Numero de la fonction
; ___________________________________________________________________
;
		xdef	_ed_function_call
_ed_function_call
Ed_FCall
; Nettoyage
; ~~~~~~~~~
	bsr	Ed_ClEProc
	bsr	EdM_Program
	bsr	Ed_CuOff
; Stocke les flags pour le splitter
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	FlagFonc(pc),a0
	move.b	0(a0,d2.w),d0
	tst.w	EdMa_Tape(a5)
	beq.s	.PaM
	btst	#5,d0
	beq.s	.NoMacro
.PaM	move.b	d0,Ed_SCallFlags(a5)
	btst	#2,d0
	beq.s	.Notst
	bsr	Ed_LCourant
	bne	Ed_NotEdit
.Notst	move.w	Edt_YCu(a4),d0
	add.w	Edt_YPos(a4),d0
	move.w	d0,Edt_YOldBloc(a4)
; Fonction ou programme?
; ~~~~~~~~~~~~~~~~~~~~~~
	move.w	d2,d1
	ext.l	d1
	cmp.w	#HiddenCommands-1,d2
	bcs.s	.Fonc
; Hidden programs
	sub.w	#HiddenCommands-1,d1
	divu	#3,d1
	swap	d1
	moveq	#HiddenCall-1,d2		RUN / EDIT ou NEW?
	add.w	d1,d2
	clr.w	d1
	swap	d1
	add.w	EdM_PosHidden(a5),d1		Plus position courante!
	bra.s	.Call
; Autoload?
.Fonc	move.w	d2,d0
	mulu	#3,d0
	lea	Ed_AutoLoad(a5),a0
	tst.b	0(a0,d0.w)
	bne.s	.Prg
; Appelle la fonction!
.Call	lsl.w	#2,d2
	clr.w	T_Actualise(a5)
	lea	JFonc(pc),a0
	moveq	#0,d0
	jmp	0(a0,d2.w)
; Appel d'un programme externe, si pas zappeuse!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Prg	tst.b	Ed_Zappeuse(a5)
	bne.s	.Call
	lea	0(a0,d0.w),a0
	tst.w	EdMa_Tape(a5)	Pas si macro record
	beq	Ed_PrgCommand
; On ne peut utiliser cette commande dans une macro!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.NoMacro
	subq.w	#3,EdMa_Tape(a5)	Recule le pointeur
	moveq	#EdD_Macro3,d0
	bsr	Ed_Dialogue
	bra	Ed_Loop

; Branchement � HELP par F5
; ~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_GoHelp
	moveq	#26,d2
	bra	Ed_FCall

; ___________________________________________________________________
;
;	ENTREE DE LA TELECOMMANDE
;
;	D0	Fonction
;	D1	Parametre
;	Name1	Chaine de commande
; ___________________________________________________________________
;
Ed_ZapIn
; Pas le meme programme!
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	Edt_Runned(a5),a2
	cmp.l	Edt_Current(a5),a2
	beq	.NoAcc
	tst.b	Prg_Accessory(a5)
	beq	.NoAcc
; Sauve les donnees du programme appelant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	d0,d2
	move.l	d1,Ed_ZapParam(a5)
	move.l	Edt_Prg(a2),a0
	lea	Prg_ZapData(a0),a0
	JJsrR	L_Prg_DataSave,a1
	JJsrR	L_Prg_DataNew,a1
; Prepare le terrain
; ~~~~~~~~~~~~~~~~~~
	clr.w	Ed_ZapError(a5)
	subq.w	#1,d2
	cmp.w	#HiddenCommands,d2
	bcc.s	.PaCom
	lea	FlagFonc(pc),a0
	move.b	0(a0,d2.w),d1		Commande Zappable?
	bpl.s	.PaCom
	btst	#6,d1			Une ligne de commande?
	beq.s	.Branch
	move.l	Name2(a5),a0		Oui, est-elle fournie?
	tst.w	(a0)			Longueur en NAME2,
	beq.s	.BadCh			String en NAME1...
; Branche � la fonction!
; ~~~~~~~~~~~~~~~~~~~~~~
.Branch	and.b	#$F0,d1			Mode zappeuse!
	move.b	d1,Ed_Zappeuse(a5)
	EcCalD	Active,EcEdit		Active l'ecran editeur
	move.l	BasSp(a5),sp
	move.l	Edt_Current(a5),a4	Programme courant branche!
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_SetBanks
	clr.b	Ed_YaUTest(a4)		Flags utiles
	move.b	Prg_MathFlags(a6),MathFlags(a5)
	clr.w	Ed_ZapCounter(a5)
	bsr	Ed_FCall		Appel de la fonction
	bra	Ed_Loop
; Erreur, commande non Zappable
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PaCom	move.w	#-4,Ed_ZapError(a5)
	moveq	#13,d0
	bra.s	.Err
; Erreur, pas de chaine de commande
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.BadCh	move.w	#-5,Ed_ZapError(a5)
	moveq	#14,d0
.Err	bsr	Ed_GetMessage
	move.l	a0,Ed_ZapMessage(a5)
	bra.s	Ed_ZapOut
; Erreur immediate, pas un accessoire / Deja runne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.NoAcc	move.w	#-6,Ed_ZapError(a5)
	moveq	#15,d0
	bsr	Ed_GetMessage
	move.l	a0,Ed_ZapMessage(a5)
	bra.s	Ed_ZapX
; _________________________________________________
;
;	SORTIE DE LA TELECOMMANDE
; _________________________________________________
;
Ed_ZapOut
; Rebranche le programme
; ~~~~~~~~~~~~~~~~~~~~~~
	bsr	EdM_Program
	tst.b	Ed_YaUTest(a5)		Nettoyer les variables
	beq.s	.NoNew
	illegal				Ne doit JAMAIS se produire!
;	bsr	Edt_ClearVar
.NoNew	move.l	Edt_Runned(a5),a4	Remet les variables du programme
	move.l	Edt_Prg(a4),a6
	lea	Prg_ZapData(a6),a0
	JJsrR	L_Prg_DataLoad,a1
; Reactive l'�cran courant, si ouvert
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EcCalD	Last,EcEdit		En dernier!
	bsr	EdReCop
	move.w	ScOn(a5),d1
	beq.s	Ed_ZapX
	subq.w	#1,d1
	EcCall	Active
Ed_ZapX	move.w	Ed_ZapError(a5),d0
	ext.l	d0
	move.l	Ed_ZapMessage(a5),a0
	clr.w	Ed_ZapError(a5)
; Retour au programme appelant!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	BasSp(a5),sp
	rts

; ZAP: remplacement d'une chaine dans la config
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdZ_NewConfig
	lea	Sys_Messages(a5),a2	Trouve la bonne config
	move.l	Ed_ZapParam(a5),d0
	beq.s	.Skp
	cmp.l	#5,d0			Met le flag seulement
	bhi.s	.Skip
	lea	Ed_Systeme-4(a5),a2
	lsl.w	#2,d0
	add.l	d0,a2
.Skp	move.l	(a2),a0			Trouve le nombre de messages
	bsr	Ed_GetNbMessage
	move.l	d0,d1
	move.l	Name1(a5),a1		Pas trop loin?
	move.l	(a1)+,d0		Numero
	cmp.l	d1,d0
	bhi.s	.Skip
	move.l	a2,a0
	bsr	EdC_ChangeTexte		Va changer!
	bne	Ed_OMm
.Skip	move.b	#1,EdC_Modified(a5)	On a change le menu
	move.b	#1,EdC_Changed(a5)	On a change la config!
	rts
; ZAP: remplacement d'une ligne dans le programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdZ_NewLineTok
	bsr.s	EdZ_NewLine
	bsr	Ed_TokCur
	rts
EdZ_NewLine
; Remplace la ligne par la nouvelle
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_LCourant
	move.l	a0,a1
	move.l	Name1(a5),a2
.Loop	move.b	(a2)+,(a1)+
	bne.s	.Loop
	subq.l	#1,a1
	sub.l	a0,a1
	move.w	a1,-2(a0)
; Reaffiche la ligne courante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	addq.w	#1,Edt_LEdited(a4)
	bsr	Ed_EALiCu
	rts
; _________________________________________________
;
;	FONCTIONS TELECOMMANDE
; _________________________________________________
;
Ed_ZapFonction
	tst.b	Prg_Accessory(a5)
	beq.s	.PaAcc
	subq.l	#1,d0
	cmp.l	#EdZ_NFonc,d0
	bcc	.IlFonc
	move.l	Edt_Current(a5),a2
	lsl.w	#2,d0
	lea	EdZ_Jump(pc),a0
	moveq	#0,d2
	jsr	0(a0,d0.w)
	rts
; Pas un accessoire
; ~~~~~~~~~~~~~~~~~
.PaAcc	moveq	#15,d0
	moveq	#-6,d1
	bra.s	.Err
; Illegal function call
; ~~~~~~~~~~~~~~~~~~~~~
.IlFonc	moveq	#16,d0
	moveq	#-7,d1
; Retour avec le message
; ~~~~~~~~~~~~~~~~~~~~~~
.Err	bsr	Ed_GetMessage
	move.l	d1,d0
	moveq	#2,d2
	rts

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdZ_Jump
	bra	EdZ_GetLine		1 La ligne courante
	bra	EdZ_GetName		2 Nom du programme courant
	bra	EdZ_GetX		3 Position en X
	bra	EdZ_GetY		4 Position en Y
	bra	EdZ_GetNLigne		5 Nombre de lignes
	bra	EdZ_X1Bloc		6 Debut X du bloc ombre
	bra	EdZ_Y1Bloc		7 Debut Y du bloc ombre
	bra	EdZ_X2Bloc		8 Fin X du bloc ombre
	bra	EdZ_Y2Bloc		9 Fin Y du bloc ombre
	bra	EdZ_GetFree		10 Espace libre
	bra	EdZ_GetStruc		11 Adresse de la structure
	bra	EdZ_Token		12 Tokenise la ligne
	bra	EdZ_GetConfig		13 Adresse de la config
EdZ_NFonc	equ	11
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdZ_GetConfig
	lea	Ed_Config(a5),a0
	move.l	a0,d0
	bra.s	EdZ_Coo
EdZ_X1Bloc
	bsr	EdZ_Bloc
	move.w	d4,d0
	bra.s	EdZ_Coo
EdZ_Y1Bloc
	bsr	EdZ_Bloc
	bra.s	EdZ_Coo
EdZ_X2Bloc
	bsr	EdZ_Bloc
	move.w	d5,d0
	bra.s	EdZ_Coo
EdZ_Y2Bloc
	bsr	EdZ_Bloc
	move.w	d1,d0
	bra.s	EdZ_Coo
EdZ_GetX
	move.w	Edt_XCu(a2),d0
	bra.s	EdZ_Coo
EdZ_GetY
	move.w	Edt_YPos(a2),d0
	add.w	Edt_YCu(a2),d0
EdZ_Coo	addq.w	#1,d0
	ext.l	d0
	rts
EdZ_GetNLigne
	move.l	Edt_Prg(a2),a0
	move.w	Prg_NLigne(a0),d0
	ext.l	d0
	rts
EdZ_GetFree
	move.l	Edt_Prg(a2),a0
	move.l	Prg_StBas(a0),d0
	sub.l	Prg_StMini(a0),d0
	rts
EdZ_GetStruc
	move.l	a2,d0
	rts
EdZ_GetName
	move.l	Edt_Prg(a2),a0
	lea	Prg_NamePrg(a0),a0
	moveq	#0,d0
	moveq	#2,d2
	rts
EdZ_GetLine
	movem.l	a4/a6,-(sp)
	move.l	a2,a4
	move.l	Edt_Prg(a4),a6
	move.l	Ed_BufT(a5),a1
	clr.w	(a1)
	move.l	d1,d0
	ble.s	.LiCu
; Ligne quelconque
	subq.w	#1,d0
	cmp.w	Prg_NLigne(a6),d1
	bhi.s	.Out
	bra.s	.Li
; Ligne sous le curseur
.LiCu	move.w	Edt_YCu(a4),d0
	add.w	Edt_YPos(a4),d0
.Li	bsr	Ed_FindL
	beq.s	.Out
	moveq	#0,d0
	bsr	Detok
.Out	move.l	Ed_BufT(a5),a0
	move.w	(a0)+,d0
	clr.b	0(a0,d0.w)
	moveq	#0,d0
	moveq	#2,d2
	movem.l	(sp)+,a4/a6
	rts
EdZ_Bloc
	move.w	Edt_YBloc(a2),d0
	bmi.s	.No
	move.w	Edt_YCu(a2),d1
	add.w	Edt_YPos(a2),d1
	move.w	Edt_XBloc(a2),d4
	move.w	Edt_XCu(a2),d5
	cmp.w	d0,d1
	bhi.s	.Ok
	bne.s	.Sw
	cmp.w	d4,d5
	bcc.s	.Ok
.Sw	exg	d0,d1
	exg	d4,d5
.Ok	rts
.No	addq.l	#4,sp
	moveq	#-1,d0
	rts
; Tokenise la ligne du buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdZ_Token
	move.l	a2,-(sp)
	move.l	Buffer(a5),a0
	move.l	Ed_BufT(a5),a1
	bsr	Tokenise
	move.l	Ed_BufT(a5),d0
	moveq	#0,d2
	move.l	(sp)+,a2
	rts


; ______________________________________________________________________
;
;						LISTES / MEMOIRE
; ______________________________________________________________________

; Reserve un espace m�moire sur (a5)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Adresse dans (a5)
;	D0=	Longueur
Ed_MemReserve
	movem.l	d1/a1,-(sp)
	move.l	a0,a1
	move.l	d0,d1
	addq.l	#4,d0
	SyCall	MemFastClear
	beq.s	.Out
	move.l	d1,(a0)+
	move.l	a0,(a1)
.Out	movem.l	(sp)+,d1/a1
	rts
; Efface un espace m�moire sur (a5)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Adresse dans (a5)
Ed_MemFree
	movem.l	a0/a1,-(sp)
	move.l	(a0),d0
	beq.s	.Skip
	clr.l	(a0)
	move.l	d0,a1
	move.l	-(a1),d0
	addq.l	#4,d0
	SyCall	SyFree
.Skip	movem.l	(sp)+,a0/a1
	rts

;
; Cree un �l�ment en tete de liste A0 / longueur D0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ListeNew
	movem.l	a0/d0/d1,-(sp)
	addq.l	#8,d0
	exg	a0,a1
	SyCall	MemFastClear
	exg	a0,a1
	beq.s	.Err
	move.l	(a0),(a1)
	move.l	a1,(a0)
	subq.l	#8,d0
	move.l	d0,4(a1)
.Err	movem.l	(sp)+,a0/d0/d1
	rts

; Efface une liste entiere A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ListeDelAll
	bra.s	.In
.Loop	move.l	d0,a1
	bsr	Ed_ListeDel
.In	move.l	(a0),d0
	bne.s	.Loop
	rts
; Efface un �l�ment de liste A1 / Debut liste A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ListeDel
	movem.l	a0/d0-d2,-(sp)
	move.l	a1,d0
	move.l	a0,a1
	move.l	(a1),d2
	beq.s	.NFound
.Loop	move.l	a1,d1
	move.l	d2,a1
	cmp.l	d0,a1
	beq.s	.Found
	move.l	(a1),d2
	bne.s	.Loop
	bra.s	.NFound
; Enleve de la liste
.Found	move.l	d1,a0
	move.l	(a1),(a0)
	move.l	4(a1),d0
	addq.l	#8,d0
	SyCall	MemFree
.NFound	movem.l	(sp)+,a0/d0-d2
	rts

; Clearvar du programme courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_clear_var
_ed_clear_var
Edt_ClearVar
	bsr	EdM_Program
	JJsr	L_Prg_SetBanks
	JJsr	L_ClearVar
	rts
;
; Actualisation des ecrans
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdReCop	SyCall	WaitVbl
	EcCall	CopForce
	rts

; ______________________________________________________________________
;
;						BOITES DE DIALOGUE
; ______________________________________________________________________

; Initialisation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_InitDialogues
	bsr	Ed_SetBanks
	moveq	#1,d0			Canal #1
	move.l	#1024,d1		1024 octets
	moveq	#16,d2			16 variables
	moveq	#0,d3			Ne pas recopier la chaine
	move.l	Ed_Resource(a5),a0
	move.l	a0,a1
	add.l	2+8(a0),a0		Base des programmes
	add.l	2+0(a0),a0		Programme numero 1
	add.l	2(a1),a1		Base des graphiques
	move.l	Ed_Messages(a5),a2	Base des messages
	JJsrP	L_Dia_OpenChannel,a3
	bne.s	.Err
	move.l	a0,Ed_ADialogues(a5)
	move.l	a1,Ed_VDialogues(a5)
	move.l	#Ed_DiaImages,Dia_PuzzleI(a0)
	bsr	Ed_IChaine
	moveq	#0,d0
.Err	rts

; Fin des dialogues
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_EndDialogues
	bsr	Ed_SetBanks
	moveq	#1,d0
	JJsr	L_Dia_CloseChannel
	rts

; Copie une chaine dans le buffer de dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_IChaine
	move.l	Ed_BufT(a5),Ed_DiaCopyC(a5)
	rts
Ed_DChaine
	move.l	Ed_DiaCopyC(a5),Ed_DiaCopyD(a5)
	addq.l	#2,Ed_DiaCopyC(a5)
Ed_AChaine
	move.l	Ed_DiaCopyC(a5),a1
.Loop	move.b	(a0)+,(a1)+
	bne.s	.Loop
	subq.l	#1,a1
	move.l	a1,Ed_DiaCopyC(a5)
	move.l	a1,d0
	move.l	Ed_DiaCopyD(a5),a1
	sub.l	a1,d0
	subq.b	#2,d0
	clr.b	(a1)
	move.b	d0,1(a1)
	rts

; Appel general des dialogues, avec protection memoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_dialogue
_ed_dialogue
Ed_Dialogue:
	tst.b	Ed_Zappeuse(a5)
	beq.s	.NoZap
	move.l	Ed_ZapParam(a5),d0		Zappeuse: retourne parametre
	rts
; Au moins 48k de CHIP pour un effacement...
.NoZap	clr.l	Dia_Magic(a5)
	movem.l	d0/a6,-(sp)
	move.l	$4.w,a6
	move.l	#Public|Chip,d1
	jsr	_LVOAvailMem(a6)
	move.l	d0,d1
	movem.l	(sp)+,d0/a6
	cmp.l	#48*1024,d1
	bcc.s	Ed_DoDialog
; Sans effacement!
	move.l	#"NobL",Dia_Magic(a5)
	bsr	Ed_DoDialog
	movem.l	d0-d1/a0-a1,-(sp)
	bsr	Ed_DrawWindows
	movem.l	(sp)+,d0-d1/a0-a1
	rts
; Appel des dialogues
; ~~~~~~~~~~~~~~~~~~~
Ed_DoDialog
	bsr	Ed_SetBanks
	movem.l	d2-d3,-(sp)
	move.l	d0,d1
	moveq	#1,d0
	move.l	#EntNul,d2
	move.l	d2,d3
	JJsr	L_Dia_RunProgram
	movem.l	(sp)+,d2-d3
	clr.l	Dia_Magic(a5)
	move.l	d1,d0
	bsr	Ed_IChaine
	rts


; _________________________________________________________________________
;
; Jumps aux touches de fonction
; _________________________________________________________________________
;
JFonc:	bra	Ed_CHaut		* 1  Cur Haut
	bra	Ed_CBas			* 2  Cur Bas
	bra	Ed_CGauche		* 3  Cur Gauche
	bra	Ed_CDroite		* 4  Cur Droite
	bra	Ed_HPage		* 5  Haut Page
	bra	Ed_BPage		* 6  Bas Page
	bra	Ed_MGauche		* 7  Mot gauche
	bra	Ed_MDroite		* 8  Mot Droite
	bra	Ed_PHaut		* 9  Page Haut
	bra	Ed_PBas			* 10 Page Bas
	bra	Ed_DLigne		* 11 Debut Ligne
	bra	Ed_FLigne		* 12 Fin Ligne
	bra	Ed_EtatUp		* 13 Bar Haut Haut
	bra	Ed_EtatDown		* 14 Bar haut Bas
	bra	Ed_BasUp		* 15 Bar Bas Haut
	bra	Ed_BasDown		* 16 Bar Bas Bas
	bra	Ed_HTexte		* 17 Haut Texte
	bra	Ed_BTexte		* 18 Bas Texte
	bra	Ed_Return		* 19 Return
	bra	Ed_Back			* 20 Backspace
	bra	Ed_Delete		* 21 Delete
	bra	Ed_EffLigne		* 22 Effacement Ligne
	bra	Ed_DelLiCu		* 23 Delete Ligne
	bra	Ed_Tab			* 24 Plus Tab
	bra	Ed_ShTab		* 25 Moins Tab
	bra	Ed_STab			* 26 Set Tab
	bra	Ed_UserMenu		* 27 Help
	bra	Ed_Escape		* 28 Escape
	bra	Ed_InsLine		* 29 Insert Ligne
	bra	Ed_DelFin		* 30 Delete Fin Ligne
	bra	Ed_PLabel		* 31 Previous Label
	bra	Ed_NLabel		* 32 Next Label
	bra	Ed_Load			* 33 Load
	bra	Ed_SaveAs		* 34 SaveAs
	bra	Ed_Save			* 35 Save
	bra	Ed_DelMot		* 36 Del Mot
	bra	Ed_BackMot		* 37 Back Mot
	bra	Ed_WindowHide		* 38 Hide Project
	bra	Ed_SMark0		* 39 Set Marks
	bra	Ed_SMark1
	bra	Ed_SMark2
	bra	Ed_SMark3
	bra	Ed_SMark4
	bra	Ed_SMark5
	bra	Ed_SMark6
	bra	Ed_SMark7
	bra	Ed_SMark8
	bra	Ed_SMark9
	bra	Ed_GMark0		* 49 Goto Mark
	bra	Ed_GMark1
	bra	Ed_GMark2
	bra	Ed_GMark3
	bra	Ed_GMark4
	bra	Ed_GMark5
	bra	Ed_GMark6
	bra	Ed_GMark7
	bra	Ed_GMark8
	bra	Ed_GMark9
	bra	Ed_BlocOn		* 59 Marche / Arret Bloc
	bra	Ed_BlocForget		* 60 Forget Bloc
	bra	Ed_OpenLoad		* 61 Open + Load
	bra	Ed_BlocCut		* 62 Cut Bloc
	bra	Ed_BlocPaste		* 63 Paste Bloc
	bra	Ed_DelDebut		* 64 Delete to S.O.L
	bra	Ed_Undo			* 65 Undo
	bra	Ed_Search		* 66 Search
	bra	Ed_SearchNext		* 67 Search Next
	bra	Ed_SearchPrev		* 68 Search Previous
	bra	EdZ_NewLine		* 69 ZAPPEUSE: replace line
	bra	Ed_RAlert		* 70 Recall last alert
	bra	EdZ_NewLineTok		* 71 ZAPPEUSE: replace+tokenise
	bra	Ed_BlocStore		* 72 Store bloc

	bra	Ed_Key2Menu		* 73 Set Key Shortcut
	bra	Ed_Prg2Menu		* 74 Affect Program to menu
	bra	Ed_Ins			* 75 Flip Insert Mode
	bra	Ed_GotoL		* 76 Goto Line Number
	bra	Ed_Run			* 77 Run
	bra	Ed_Test			* 78 Test
	bra	Ed_Indent		* 79 Indent
	bra	Ed_New			* 80 Clear
	bra	Ed_CloseWindowQuit	* 81 Close
	bra	Ed_Quit			* 82 Quit
	bra	Ed_Infos		* 83 Editor Informations
	bra	Ed_Merge		* 84 Merge
	bra	Ed_LoadA		* 85 Merge Ascii
	bra	Ed_BlocPrint		* 86 Print Bloc
	bra	Ed_ProcOpen		* 87 Open / Close
	bra	Ed_LoadHidden		* 88 Load Hidden program
	bra	Ed_ProcsOpen		* 89 Open All
	bra	Ed_ProcsClose		* 90 Close All
	bra	Ed_PrevWindow		* 91 Previous Window
	bra	Ed_NextWindow		* 92 Next Window
	bra	Ed_FlipSizeWindow	* 93 Enlarge Window
	bra	Ed_Redo			* 94 Redo
	bra	Ed_SplitWindow		* 95 Split View
	bra	Ed_LinkCursor		* 96 Link Cursor
	bra	Ed_BlocSaveAscii	* 97 Save Block as Ascii
	bra	Ed_BlocSave		* 98 Save Block
	bra	Ed_Replace		* 99 Replace New
	bra	Ed_ReplaceNext		* 100 Replace Next
	bra	Ed_ReplacePrev		* 101 Replace Previous
	bra	Ed_NewAllHidden		* 102 New All Hidden Programs
	bra	Ed_OpenWindow		* 103 Open New
	bra	Ed_ShowKey		* 104 Show Key shortcuts
	bra	Ed_Wb			* 105 Workbench
	bra	EdMa_New		* 106 Set new Macro
	bra	EdMa_Del		* 107 Clear One Macro
	bra	EdMa_DelAll		* 108 Clear All Macros
	bra	EdMa_LoadAs		* 109 Load Macros
	bra	EdMa_SaveAs		* 110 Save Macros
HiddenCommands	equ	184
HiddenCall	equ	111
	bra	Ed_RunHidden		* 111 Run
	bra	Ed_EditHidden		* 112 Edit
	bra	Ed_NewHidden		* 113 New
	bra	Ed_SetBuffer		* 114 Set Buffer
EdM_UserCommands	equ	115
EdM_UserLong		equ	16
EdM_UserMax		equ	20
	bra	Ed_UserMenu		* 0
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 5
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 10
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 15
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu

	bra	Ed_AddUser		* 135 Add user option
	bra	Ed_DelUser		* 136 Del user option
	bra	EdC_SaveDefault		* 137 Save default config
	bra	EdC_SaveAs		* 138 Save As
	bra	EdC_LoadDefault		* 139 Load default config
	bra	EdC_LoadAs		* 140 Load As
	bra	Ed_QuitOptions		* 141 Quit Options
	bra	Ed_SetAutoSave		* 142 Autosave
	bra	EdMa_LoadDefault	* 143 Load Default Macros
	bra	EdMa_SaveDefault	* 144 Save Default Macros
	bra	Ed_GoMonitor		* 145 Monitor
	bra	Ed_PrgPrint		* 146 Program Print
	bra	Ed_Check1.3		* 147 Check 1.3
	bra	Ed_SamOn		* 148 Sound On / Off
 	bra	Ed_AboutExt		* 149 About extensions
	bra	Ed_About		* 150 About AMOS
	bra	Ed_ProcML		* 151 Insert machine langage

; HELP: Insersion des commandes ZAP additionnelles
	bra	Ed_SaveAsName		* 152 Sauve sans changer nom
	bra	Ed_CloseName		* 153 Ferme une fenetre nom
	bra	Ed_Rename		* 154 Rename le programme courant
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 157
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 162
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 167
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu
	bra	Ed_UserMenu		* 172 Interpretor Setup
	bra	Ed_UserMenu		* 173 Editor Setup
	bra	Ed_UserMenu		* 174 Editor Menus
	bra	Ed_UserMenu		* 175 Editor Dialogs
	bra	Ed_UserMenu		* 176 Test-Time
	bra	Ed_UserMenu		* 177 Run-Time
	bra	Ed_UserMenu		* 178 Colour Palette
	bra	EdM_PrevHidden		* 179 Previous programs
	bra	EdM_NextHidden		* 180 Next programs
	bra	Ed_BlocAll		* 181 All text as block
	bra	EdZ_NewConfig		* 182 ZAP: remplace chaine dans config
	bra	Ed_GoHelp		* 183 Branche au HELP
	bra	Ed_UserMenu		* 184

; Flags pour chaque fonction
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
FlagFonc
; Bit 0: redessiner le buffer
; Bit 1: redessiner la ligne
; Bit 2: Interdit si procedure fermee
; Bit 5: autorise en macro!
; Bit 6: transmettre une ligne de commande
; Bit 7: commande t�l�commandable
	dc.b	%10100000		* 1  Cur Haut
	dc.b	%10100000 		* 2  Cur Bas
	dc.b	%10100000		* 3  Cur Gauche
	dc.b	%10100000		* 4  Cur Droite
	dc.b	%10100000		* 5  Haut Page
	dc.b	%10100000		* 6  Bas Page
	dc.b	%10100000		* 7  Mot gauche
	dc.b	%10100000		* 8  Mot Droite
	dc.b	%10100000		* 9  Page Haut
	dc.b	%10100000		* 10 Page Bas
	dc.b	%10100000		* 11 Debut Ligne
	dc.b	%10100000		* 12 Fin Ligne
	dc.b	%10100000		* 13 Bar Haut Haut
	dc.b	%10100000		* 14 Bar haut Bas
	dc.b	%10100000		* 15 Bar Bas Haut
	dc.b	%10100000		* 16 Bar Bas Bas
	dc.b	%10100000  		* 17 Haut Texte
	dc.b	%10100000		* 18 Bas Texte
	dc.b	%10100011 		* 19 Return
	dc.b	%10100101		* 20 Backspace
	dc.b	%10100101		* 21 Delete
	dc.b	%10100110		* 22 Effacement Ligne
	dc.b	%10100110		* 23 Delete Ligne
	dc.b	%10100101		* 24 Plus Tab
	dc.b	%10100101		* 25 Moins Tab
	dc.b	%00000000		* 26 Set Tab
	dc.b	%00000000		* 27 Help
	dc.b	%00000000		* 28 Escape
	dc.b	%10100010		* 29 Insert Ligne
	dc.b	%10100101		* 30 Delete Fin Ligne
	dc.b	%10100000		* 31 Previous Label
	dc.b	%10100000		* 32 Next Label
	dc.b	%10000010		* 33 Load
	dc.b	%10000010		* 34 SaveAs
	dc.b	%10000000		* 35 Save
	dc.b	%10100100		* 36 Del Mot
	dc.b	%10100100		* 37 Back Mot
	dc.b	%00000000		* 38 Hide Project
	dc.b	%10100000		* 39 Set Marks
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000		* 49 Goto Mark
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000
	dc.b	%10100000	 	* 59 Marche / Arret Bloc
	dc.b	%10100010		* 60 Forget Bloc
	dc.b	%10000010		* 61 Open + Load
	dc.b	%10100010		* 62 Cut Bloc
	dc.b	%10100010		* 63 Paste Bloc
	dc.b	%10100110		* 64 Delete to S.O.L
	dc.b	%10000001		* 65 Undo
	dc.b	%10000000		* 66 Search
	dc.b	%10100000		* 67 Search Next
	dc.b	%10100001		* 68 Search Previous
	dc.b	%10000100		* 69 ZAPPEUSE: Newline
	dc.b	%00000000		* 70 Recall last alert
	dc.b	%10000110		* 71 ZAPPEUSE: Newline+Tokenise
	dc.b	%10100000		* 72 Store bloc

	dc.b	%00000010		* 73 Set Key Shortcut
	dc.b	%00000010		* 74 Affect Program to menu
	dc.b	%00100000		* 75 Flip Insert Mode
	dc.b	%10000000		* 76 Goto Line Number
	dc.b	%00000000		* 77 Run
	dc.b	%00000000		* 78 Test
	dc.b	%00000010		* 79 Indent
	dc.b	%10000010		* 80 Clear
	dc.b	%10000000		* 81 Close
	dc.b	%00000000		* 82 Quit
	dc.b	%00000000		* 83 Informations
	dc.b	%10100110		* 84 Merge
	dc.b	%10100110  		* 85 Merge Ascii
	dc.b	%10000000		* 86 Print Bloc
	dc.b	%00000010		* 87 Open / Close
	dc.b	%00000010		* 88 Load Hidden program
	dc.b	%10000010		* 89 Open All
	dc.b	%00000010		* 90 Close All
	dc.b	%10000000		* 91 Previous Window
	dc.b	%10000000		* 92 Next Window
	dc.b	%10000000		* 93 Enlarge Window
	dc.b	%10000000		* 94 Redo
	dc.b	%00000000		* 95 Split View
	dc.b	%00000000		* 96 Link Cursor
	dc.b	%10100000		* 97 Save Block as Ascii
	dc.b	%10100000		* 98 Save Block
	dc.b	%10100010		* 99 Replace
	dc.b	%10000010		* 100 Replace Next
	dc.b	%10000000		* 101 Replace Previous
	dc.b	%00000000		* 102 New All Hidden Programs
	dc.b	%10000000		* 103 Open New
	dc.b	%00000000		* 104 Show Key shortcuts
	dc.b	%00000000		* 105 Workbench
	dc.b	%00000000		* 106 Set New Macro
	dc.b	%00000000		* 107 Clear Macro
	dc.b	%00000000		* 108 Clear All Macros
	dc.b	%00000000		* 109 Load Macros
	dc.b	%00000000		* 110 Save Macros
	dc.b	%00000000		* 111 Run
	dc.b	%00000000		* 112 Edit
	dc.b	%00000000		* 113 New
	dc.b	%00000000		* 114 Set TextBuffer

	dc.b	%00000000		* 0  User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		* 5  User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		* 10 User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		* 15 User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu
	dc.b	%00000000		*    User Menu

	dc.b	%00000000		* 135 Add user option
	dc.b	%00000000		* 136 Del user option
	dc.b	%00000000		* 137 Save default config
	dc.b	%00000000		* 138 Save As
	dc.b	%00000000		* 139 Load default config
	dc.b	%00000000		* 140 Load As
	dc.b	%00000000		* 141 Quit Options
	dc.b	%00000000		* 142 Autosave
	dc.b	%00000000		* 143 Load Default Macros
	dc.b	%00000000		* 144 Save Default Macros
	dc.b	%00000000		* 145 Monitor
	dc.b	%10000000		* 146 Program Print
	dc.b	%00000000		* 147 Check 1.3
	dc.b	%00000000		* 148 Sound On / Off
	dc.b	%00000000		* 149 About extensions
	dc.b	%00000000		* 150 About AMOS
	dc.b 	%00000000		* 151 Insert machine langage
; HELP: Insersion des commandes ZAP additionnelles
	dc.b	%11000000		* 152 Save As Name
	dc.b	%11000000
	dc.b	%11000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000		* 157
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000		* 162
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000		* 167
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000
	dc.b	%00000000		* 172 Interpretor Setup
	dc.b	%00000000		* 173 Editor Setup
	dc.b	%00000000		* 174 Editor Menus
	dc.b	%00000000		* 175 Editor Dialogs
	dc.b	%00000000		* 176 Test-Time
	dc.b	%00000000		* 177 Run-Time
	dc.b	%00000000		* 178 Colour Palette
	dc.b	%00000000		* 179 Previous programs
	dc.b	%00000000		* 180 Next programs
	dc.b	%10000000		* 181 All text as block
	dc.b	%11000000		* 182 Change config string
	dc.b	%00000000		* 183
	dc.b	%00000000		* 184
	even

; Backspace, Delete, deleting parts of the line, words, Tab, Shift-Tab:
; src/editor/lines.cpp
Ed_Back		jmp	_ed_backspace_entry
Ed_Delete	jmp	_ed_delete_key_entry
Ed_EffLigne	jmp	_ed_clear_line_entry
Ed_DelFin	jmp	_ed_delete_to_end_entry
Ed_DelDebut	jmp	_ed_delete_to_start_entry
Ed_DelMot	jmp	_ed_delete_word_entry
Ed_BackMot	jmp	_ed_delete_word_left_entry
Ed_Tab		jmp	_ed_tab_entry
Ed_ShTab	jmp	_ed_untab_entry

;
; SET TAB VALUE
; ~~~~~~~~~~~~~
Ed_STab	bsr	Ed_TokCur
	move.l	Ed_VDialogues(a5),a2
	moveq	#0,d0
	move.w	Ed_Tabs(a5),d0
	move.l	d0,2*4(a2)
	moveq	#EdD_SetTab,d0
	bsr	Ed_Dialogue
	moveq	#1,d0
	moveq	#3,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.w	d1,Ed_Tabs(a5)
; Flag pour QUIT
	move.b	#1,EdC_Changed(a5)
	bra	Ed_Loca

;
; Cursor movement: src/editor/cursor.cpp
Ed_CHaut	jmp	_ed_cursor_up_entry
Ed_ChtT		jmp	_ed_cursor_up_t_entry
CFin:		rts
Ed_CBas		jmp	_ed_cursor_down_entry
Ed_CBasT	jmp	_ed_cursor_down_t_entry
Ed_CGauche	jmp	_ed_cursor_left_entry
Ed_CGo		jmp	_ed_cgo_entry
Ed_CDroite	jmp	_ed_cursor_right_entry
Ed_CDr		jmp	_ed_cdr_entry
Ed_HPage	jmp	_ed_page_top_entry
Ed_BPage	jmp	_ed_page_bottom_entry
Ed_MGauche	jmp	_ed_word_left_key_entry
Ed_MDroite	jmp	_ed_word_right_key_entry
; R_MotGoch, R_MotDroi (d1 the start of the word): src/editor/lines.cpp
R_MotGoch	jmp	_ed_word_left_entry
R_MotDroi	jmp	_ed_word_right_entry
Ed_DLigne
Ed_DLigneT	jmp	_ed_line_start_entry
Ed_FLigne	jmp	_ed_line_end_entry
Ed_PHaut	jmp	_ed_page_up_entry
Ed_PBas		jmp	_ed_page_down_entry
Ed_SHaut	jmp	_ed_scroll_up_key_entry
Ed_SHaut2	jmp	_ed_shaut2_entry
Ed_SBas		jmp	_ed_scroll_down_key_entry
Ed_SBas2	jmp	_ed_sbas2_entry
Ed_SGauche	jmp	_ed_scroll_left_key_entry
Ed_SDroite	jmp	_ed_scroll_right_key_entry
Ed_HTexte	jmp	_ed_text_top_entry
Ed_HTex		jmp	_ed_text_top_t_entry
Ed_BTexte	jmp	_ed_text_bottom_entry
Ed_BTex		jmp	_ed_text_bottom_t_entry
Ed_PLabel	jmp	_ed_label_previous_entry
Ed_NLabel	jmp	_ed_label_next_entry

;
; Copie chaine
; ~~~~~~~~~~~~
EdCocop	move.b	(a0)+,(a1)+
	bne.s	EdCocop
	rts
;
; D0 > majuscule
; ~~~~~~~~~~~~~~
MajD0ed	cmp.b	#"a",d0
	bcs.s	.skip
	cmp.b	#"z",d0
	bhi.s	.skip
	sub.b	#$20,d0
.skip	rts
;
; Ramene VRAI si D2 est une lettre ou un chiffre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Une_Lettre (d2): src/editor/lines.cpp
Une_Lettre	jmp	_ed_letter_entry


; ___________________________________________________________________
;
;								MARKS
; ___________________________________________________________________

; The marks: src/editor/marks.cpp
Prg_MarkRaz	jmp	_ed_marks_clear_entry
Ed_AutoMarks	jmp	_ed_auto_marks_entry

; Set MARK
; ~~~~~~~~
Ed_SMark9
	addq.w	#1,d0
Ed_SMark8
	addq.w	#1,d0
Ed_SMark7
	addq.w	#1,d0
Ed_SMark6
	addq.w	#1,d0
Ed_SMark5
	addq.w	#1,d0
Ed_SMark4
	addq.w	#1,d0
Ed_SMark3
	addq.w	#1,d0
Ed_SMark2
	addq.w	#1,d0
Ed_SMark1
	addq.w	#1,d0
Ed_SMark0	jmp	_ed_set_mark_entry
;
; Va a une marque
; ~~~~~~~~~~~~~~~
Ed_GMark9
	addq.w	#1,d0
Ed_GMark8
	addq.w	#1,d0
Ed_GMark7
	addq.w	#1,d0
Ed_GMark6
	addq.w	#1,d0
Ed_GMark5
	addq.w	#1,d0
Ed_GMark4
	addq.w	#1,d0
Ed_GMark3
	addq.w	#1,d0
Ed_GMark2
	addq.w	#1,d0
Ed_GMark1
	addq.w	#1,d0
Ed_GMark0	jmp	_ed_goto_mark_entry

Ed_Marks2Adress	jmp	_ed_marks_to_addresses_entry
Ed_Marks2Number	jmp	_ed_marks_to_numbers_entry
Ed_MarksChange	jmp	_ed_marks_change_entry

; ____________________________________________________________________________
;
;								QUIT
; ____________________________________________________________________________
;


; QUIT ALL!
; ~~~~~~~~~
Ed_Quit	bsr	Ed_TokCur

; Demander confirmation?
; ~~~~~~~~~~~~~~~~~~~~~~
	btst	#0,Ed_QuitFlags(a5)
	beq.s	.NoDia
	moveq	#EdD_Quit,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_Loop
.NoDia

		xdef	_ed_do_quit
_ed_do_quit
Ed_DoQuit
	bsr	Ed_TokCur
	bsr	Edt_ClearVar
; Au revoir!
; ~~~~~~~~~~
	moveq	#"B",d0
	bsr	Ed_SamPlay
; Sauver la config?
; ~~~~~~~~~~~~~~~~~
	btst	#1,Ed_QuitFlags(a5)		Sauver?
	beq.s	.NoConf
	tst.b	EdC_Changed(a5)			Modification?
	beq.s	.NoConf
	bsr	EdC_SaveDef			Va sauver
.NoConf
; Sauver les macros?
; ~~~~~~~~~~~~~~~~~~
	btst	#2,Ed_QuitFlags(a5)
	beq.s	.NoMac
	tst.b	EdMa_Changed(a5)
	beq.s	.NoMac
	bsr	EdMa_SaveDef			Va sauver
.NoMac
; Sauver les d�finitions des programmes?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	btst	#3,Ed_QuitFlags(a5)
	bne.s	.SavAll
;
; Sauvegarde des programmes non sauves, et QUIT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Edt_List(a5),a4
.SLoop	move.l	Edt_Prg(a4),a6
	bsr	Ed_Saved
	move.l	Edt_Next(a4),d0
	move.l	d0,a4
	bne.s	.SLoop
	bra	Ed_System
;
; Sauvegarde de la configuration de l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.SavAll
	move.w	#149,d0
	bsr	Ed_AverMess
; Sauve toutes structures programmes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#1,d7
	move.l	Edt_List(a5),a4
.QLp	move.l	Edt_Prg(a4),a6		Structure programme...
	tst.l	Edt_LinkPrev(a4)	Sauve une seule fois!
	bne.s	.PNext
	tst.b	Prg_NamePrg(a6)		Un nom?
	beq.s	.NoName
	tst.b	Prg_Change(a6)		Deja sauve!
	beq.s	.PNext
; Programme avec un nom.
	clr.b	Prg_NoNamed(a6)
	lea	Prg_NamePrg(a6),a0
	move.l	Name1(a5),a1
	bsr	EdCocop
	bra.s	.Save
; Programme sans nom: NEW_PROJECT_XX.AMOS
.NoName	addq.b	#1,Prg_NoNamed(a6)	Flag pour reload!
	moveq	#49,d0
	bsr	Ed_GetSysteme		New_Project_
	move.l	Name1(a5),a1
	bsr	EdCocop
	lea	-1(a1),a0
	move.l	d7,d0			##
	JJsrR	L_LongToDec,a2
	move.l	a0,a1
	moveq	#22,d0			.AMOS
	bsr	Ed_GetSysteme
	bsr	EdCocop
.Save	JJsr	L_Dsk.PathIt
	JJsr	L_Prg_Save
	bne	.Err
.PNext	addq.w	#1,d7
	move.l	Prg_Next(a4),d0
	move.l	d0,a4
	bne.s	.QLp
; Ouvre le fichier last cession
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#47,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	move.l	#1006,d2
	bsr	Ed_Open
	beq	.Err
; Place pour l'entete + longueur
	move.l	Buffer(a5),a3
	clr.l	(a3)
	clr.l	4(a3)
	move.l	a3,d2
	moveq	#8,d3
	bsr	Ed_Write
	bne	.Err
; Sauve toutes les structures PRG
	move.l	Prg_List(a5),a6
.PLoop	move.l	a6,a0
	move.l	#Prg_Long,d0
	bsr	.StructureSave
	bne	.Err
	move.l	Prg_Next(a6),d0
	move.l	d0,a6
	bne.s	.PLoop
	sub.l	a0,a0
	bsr	.StructureSave
	bne	.Err
; Sauve toutes les structures EDT
	move.l	Edt_List(a5),a4
.ELoop	clr.w	Edt_Order(a4)		Programme courant
	cmp.l	Edt_Current(a5),a4
	bne.s	.ESkip
	move.w	#1,Edt_Order(a4)	Edt_Order<>0
.ESkip	move.l	a4,a0
	move.l	#Edt_Long,d0
	bsr	.StructureSave
	bne	.Err
	move.l	Edt_Next(a4),d0
	move.l	d0,a4
	bne.s	.ELoop
	sub.l	a0,a0
	bsr	.StructureSave
	bne	.Err
; Met la reconnaissance en tete de fichier
	moveq	#0,d2
	moveq	#-1,d3
	bsr	Ed_Seek
	move.l	#Ed_QuitHead,(a3)
	subq.l	#8,d0
	move.l	d0,4(a3)
	move.l	a3,d2
	moveq	#8,d3
	bsr	Ed_Write
	bne.s	.Err
; On s'en va!
	bsr	Ed_Close
	bsr	Ed_AllAverFin
	bra	Ed_System
; Erreur: message et retour!
.Err	bsr	Ed_Close
	bsr	Ed_AllAverFin
	moveq	#EdD_NoWarm,d0
	bsr	Ed_Dialogue
	bsr	Ed_DrawWindows
	rts
; Sauve une structure avec son adresse en premier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.StructureSave
	movem.l	a0/d0,-(sp)
	move.l	a0,(a3)
	beq.s	.Vide
	move.l	d0,4(a3)
	clr.l	8(a3)
	move.l	a3,d2
	moveq	#12,d3
	bsr	Ed_Write
	bne.s	.SErr
	movem.l	(sp)+,a0/d0
	move.l	a0,d2
	move.l	d0,d3
	bsr	Ed_Write
	rts
.Vide	move.l	a3,d2
	moveq	#4,d3
	bsr	Ed_Write
.SErr	addq.l	#8,sp
	rts

;
; Set QUIT OPTIONS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_QuitOptions
	bsr	Ed_TokCur
	move.l	Ed_VDialogues(a5),a0
	move.b	Ed_QuitFlags(a5),d0
	moveq	#4,d1
	JJsrR	L_Dia_SetVFlags,a1
	moveq	#EdD_OQuit,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_Loca
	move.l	Ed_VDialogues(a5),a0
	moveq	#4,d1
	JJsrR	L_Dia_GetVFlags,a1
	move.b	d0,Ed_QuitFlags(a5)
	move.b	#1,EdC_Changed(a5)
	rts

; ______________________________________________________________________________
;
;							EDITOR INFORMATIONS
; ______________________________________________________________________________
;

; About
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_About
	JLea	L_VersionN,a0
	bsr	Ed_DChaine
	move.l	Ed_VDialogues(a5),a2
	move.l	a1,0*4(a2)
	lea	AdTokens+4(a5),a0	Compte les extensions
	moveq	#26-1,d0
	moveq	#0,d2
.Cpt	tst.l	(a0)+
	beq.s	.PaE
	addq.w	#1,d2
.PaE	dbra	d0,.Cpt
	move.l	d2,1*4(a2)
	JLea	L_UserReg,a0		Registration #
	move.l	Name2(a5),a1
	moveq	#$73,d0
	JJsrP	L_Sys_UnCode,a2
	move.l	a1,3*4(a2)
	lea	16(a0),a0		Registred user
	lea	16(a1),a1
	move.b	#$A5,d0
	JJsrP	L_Sys_UnCode,a2
	move.l	a1,2*4(a2)
	moveq	#EdD_Title,d0
	bsr	Ed_Dialogue
	rts

; About extensions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_AboutExt
	move.l	Ed_VDialogues(a5),a2
; Trouve la premiere extension
	lea	AdTokens(a5),a3
	moveq	#0,d3
	bsr	.Next
	tst.w	d3
	beq.s	.Out
; Appelle le dialogue
.Loop	move.l	d3,0*4(a2)
	lea	.Empty(pc),a1
	move.l	(a3),a0
	move.l	LB_Title(a0),d0
	beq.s	.Sk
	move.l	d0,a1
.Sk	move.l	a1,1*4(a2)
	moveq	#EdD_AboutE,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	beq.s	.Prv
	cmp.w	#2,d0
	beq.s	.Nxt
.Out	rts
; Suivant
.Nxt	bsr	.Next
	bra.s	.Loop
; Precedent
.Prv	bsr	.Prev
	bra.s	.Loop
; Previous extension
.Prev	move.l	d3,d0
	move.l	a3,a0
.Pr1	cmp.w	#1,d0
	bls.s	.POut
	subq.w	#1,d0
	tst.l	-(a0)
	beq.s	.Pr1
	move.l	d0,d3
	move.l	a0,a3
.POut	rts
; Next extension
.Next	move.l	d3,d0
	move.l	a3,a0
.Ne1	cmp.w	#26,d0
	bcc.s	.NOut
	addq.w	#1,d0
	addq.l	#4,a0
	tst.l	(a0)
	beq.s	.Ne1
	move.l	d0,d3
	move.l	a0,a3
.NOut	rts
.Empty	dc.w	0

; Informations
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Infos
	bsr	Ed_TokCur
	bsr	Ed_VaTester
	move.l	Ed_VDialogues(a5),a2
	move.l	a6,-(sp)
	move.l	$4.w,a6
; Chip Free
	move.l	#Chip,d1
	jsr	_LVOAvailMem(a6)
	move.l	d0,0*4(a2)
; Fast Free
	move.l	#Fast,d1
	jsr	_LVOAvailMem(a6)
	move.l	d0,1*4(a2)
	move.l	(sp)+,a6
; Text length
	move.l	Prg_StHaut(a6),d0
	sub.l	Prg_StBas(a6),d0
	move.l	d0,2*4(a2)
; Bank length
	JJsr	L_Bnk.GetLength
	add.l	d2,d0
	add.l	d1,d0
	move.l	d0,3*4(a2)		Banques totales
	move.l	d1,5*4(a2)		Bobs
	move.l	d2,6*4(a2)		Icones
; Number of lines
	moveq	#0,d0
	move.w	Prg_NLigne(a6),d0
	move.l	d0,4*4(a2)
; Number of instructions
	move.l	BMenage(a5),5*4(a2)
	move.l	VerNInst(a5),5*4(a2)
; Appelle les dialogues
	moveq	#EdD_Infos,d0
	bsr	Ed_Dialogue
	rts

; __________________________________________________________________________
;
;						BANQUES / CONFIG EDITEUR
; __________________________________________________________________________
;
;
; Chargement/Effacement de la banque de resource (pas de gestion d'erreurs)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ResourceLoad
; Charge la banque
	bsr	Ed_SetBanks
	move.l	#1005,d2
	bsr	Ed_Open
	beq.s	.DErr
	moveq	#6,d0
	JJsr	L_Bnk.Load
	bne.s	.MErr
	bsr	Ed_Close
; Change les adresses
	moveq	#6,d0
	JJsr	L_Bnk.GetAdr
	beq.s	.2Err
	cmp.l	#"Reso",-8(a0)
	bne.s	.2Err
	move.l	a0,Ed_Resource(a5)
	bsr	EdC_SetPalette
	moveq	#0,d0
	rts
.DErr	moveq	#2,d0
	rts
.MErr	moveq	#1,d0
	rts
.2Err	moveq	#3,d0
	rts
; Effacement de la banque de configuration
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ResourceFree
	bsr	Ed_SetBanks
	moveq	#6,d0
	JJsr	L_Bnk.Eff
	rts
; Change la palette de couleur en fonction des prefs de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_SetPalette
	move.l	Ed_Resource(a5),d0
	beq.s	.Skip
	move.l	d0,a0
	add.l	2(a0),a0
	move.w	(a0),d0
	lsl.w	#2,d0
	lea	2+2+2(a0,d0.w),a0
	lea	Ed_Palette(a5),a1
	moveq	#7,d0
.Copy	move.w	(a1)+,(a0)+
	dbra	d0,.Copy
.Skip	rts

; Chargement/Effacement de la banque de samples (pas de gestion d'erreurs)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SamChanged
	bsr	Ed_SetBanks
	tst.b	Ed_Sounds(a5)
	bne.s	.Sons
; Pas de sons, effacer la banque?
	moveq	#5,d0
	JJsr	L_Bnk.GetAdr
	bne.s	Ed_SamEnd
	bra.s	.Out
; Il faut faire des sons, charger la banque?
.Sons	moveq	#5,d0
	JJsr	L_Bnk.GetAdr
	bne.s	.Out
	moveq	#48,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
; Charge la banque
	move.l	#1005,d2
	bsr	Ed_Open
	beq.s	.Out
	bsr	Ed_SetBanks
	moveq	#5,d0
	JJsr	L_Bnk.Load
	bsr	Ed_Close
; Ok
.Out	rts

; Effacement de la banque de samples
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SamEnd
	bsr	Ed_SetBanks
	moveq	#5,d0
	JJsr	L_Bnk.Eff
	rts
;
; Joue le sample D0 (lettre de A � Z)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_sam_play
_ed_sam_play
Ed_SamPlay
	movem.l	d0-d3/a0-a2,-(sp)
	tst.b	Ed_Sounds(a5)
	beq.s	.Out
	tst.b	Ed_Zappeuse(a5)
	bne.s	.Out
	move.w	d0,d2
	bsr	Ed_SetBanks
	moveq	#5,d0
	JJsr	L_Bnk.GetAdr
	beq.s	.Out
	cmp.l	#"Samp",-8(a0)
	bne.s	.Out
; Trouve le sample
	move.w	(a0),d1
	subq.w	#1,d1
	bmi.s	.Out
	lea	2(a0),a1
.Loop	move.l	(a1)+,d0
	lea	0(a0,d0.l),a2
	cmp.b	(a2),d2
	beq.s	.Found
	dbra	d1,.Loop
	bra.s	.Out
; Appelle la trappe
.Found	moveq	#0,d3
	move.w	8(a2),d3
	move.l	10(a2),d2
	lea	14(a2),a1
	moveq	#%1111,d1
	move.l	ExtAdr(a5),d0
	beq.s	.Out
	move.l	d0,a0
	jsr	-4(a0)
; Fini!
.Out	movem.l	(sp)+,d0-d3/a0-a2
	rts


; ___________________________________________________________________
;
;							CONFIG!
; ___________________________________________________________________
;

; Met les banques de m�moire/dialogue de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_set_banks
_ed_set_banks
Ed_SetBanks
	move.l	a0,-(sp)
	lea	Ed_Banks(a5),a0
	move.l	a0,Cur_Banks(a5)
	lea	Ed_Dialogs(a5),a0
	move.l	a0,Cur_Dialogs(a5)
	move.l	(sp)+,a0
	rts

; Sauvegarde configuration par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_SaveDefault
	bsr	Ed_TokCur
	moveq	#EdD_SvConf,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
EdC_SaveDef
	moveq	#7,d0
	JJsr	L_Sys_GetMessage
	JJsrR	L_Sys_AddPath,a1
	bsr	EdC_SaveIt
	rts
; Sauvegarde de la configuration
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_SaveAs
	bsr	Ed_TokCur
	move.w	#130,d0
	bsr	Ed_File_Selector
	beq	Ed_NotDone
	JJsr	L_Dsk.PathIt
	bsr	Ed_SaveOver
EdC_SaveIt
	move.w	#152,d0
	bsr	Ed_GetMessage
	bsr	Ed_Avertir
	bsr	EdC_Save
	bne.s	.Err
	bsr	Ed_AverFin
	clr.b	EdC_Changed(a5)
	rts
; Erreur disque
.Err	move.w	#139,d0
	bra	Ed_Al100

; Chargement de la configuration par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_LoadDefault
	bsr	Ed_TokCur
	bsr	EdC_Saved
EdC_LoadDef
	moveq	#7,d0
	JJsr	L_Sys_GetMessage
	JJsrR	L_Sys_AddPath,a1
	bsr	EdC_LoadIt
	move.b	#2,EdC_Changed(a5)
	rts
; Chargement de la configuration
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_LoadAs
	bsr	Ed_TokCur
	bsr	EdC_Saved
	move.w	#134,d0
	bsr	Ed_File_Selector
	beq	Ed_NotDone
	JJsr	L_Dsk.PathIt
	bsr	EdC_LoadIt
	move.b	#2,EdC_Changed(a5)
	rts
EdC_LoadIt
	move.w	#153,d0
	bsr	Ed_AverMess
	bsr	EdC_Load
	bne	.Err
	bsr	Ed_AverFin
	move.w	#218,d0			Message
	bsr	Ed_AverMess
	moveq	#75,d0			Attente
	bsr	Ed_TiWait
	bsr	EdC_Redraw
	rts
; Erreur
.Err	move.w	#139,d0
	bra	Ed_Al100

; Redessin de TOUT l'editeur en cas de changement de config
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_Redraw
	bsr	Ed_SamChanged
	bsr	Ed_Hide
	bsr	Ed_CloseEditor
	move.b	#1,EdC_Modified(a5)
	bsr	Ed_OpenEditor
	bsr	Ed_Appear
	rts
;
; Sauvegarde de la configuration si modifiee
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_Saved
	cmp.b	#1,EdC_Changed(a5)
	bne.s	.Skip
	moveq	#EdD_CSaved,d0
	bsr	Ed_Dialogue
	cmp.w	#2,d0
	beq.s	.Skip
	cmp.w	#1,d0
	bne	Ed_NotDone
	bsr	EdC_SaveAs
.Skip	rts

;
; Chargement de la configuration A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_Load
	move.l	#1005,d2
	bsr	Ed_Open
	beq	.DErr
; Charge la config elle meme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Buffer(a5),a0
	move.l	a0,d2
	moveq	#4,d3
	bsr	Ed_Read
	bne	.DErr
	move.l	d2,a0
	move.l	#Ed_FConfig-Ed_DConfig,d3
	cmp.l	(a0),d3
	bne	.Err2
	lea	Ed_DConfig(a5),a0
	move.l	a0,d2
	bsr	Ed_Read
	bne	.DErr
	move.l	d2,Ed_Config(a5)
; Charge les chaines systeme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_Systeme(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les chaines menus
; ~~~~~~~~~~~~~~~~~~~~~~~~
	lea	EdM_Messages(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les messages dialog
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_Messages(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les messages erreur TEST TIME
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_TstMessages(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les messages erreur RUN TIME
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_RunMessages(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les programmes � lancer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_MnPrograms(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les user menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	EdM_User(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Charge les definitions menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	EdM_Definition(a5),a0
	bsr	EdC_LoadTextes
	bne	EdC_Out
; Calcule les positions dans la ligne de config
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#1,d0
	bsr	Ed_GetSysteme
	moveq	#-1,d1
	lea	Ed_EtXX(a5),a1
.Loop1	addq.l	#1,d1
	move.b	(a0)+,d0
	beq.s	.Out1
	cmp.b	#" ",d0
	beq.s	.Loop1
	sub.b	#"0",d0
	cmp.b	#8,d0
	bcc.s	.Loop1
	ext.w	d0
	move.b	d1,0(a1,d0.w)
	bra.s	.Loop1
.Out1
; Ok!
	moveq	#0,d0
	bra	EdC_Out
; File not found
.DErr	moveq	#1,d0
	bra	EdC_Out
; Bad config
.Err2	moveq	#2,d0
	bra	EdC_Out

; Effacement de la configuration courante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_Free
	lea	Ed_Systeme(a5),a0		1
	bsr	Ed_MemFree
	lea	EdM_Messages(a5),a0		2
	bsr	Ed_MemFree
	lea	Ed_Messages(a5),a0		3
	bsr	Ed_MemFree
	lea	Ed_TstMessages(a5),a0		4
	bsr	Ed_MemFree
	lea	Ed_RunMessages(a5),a0		5
	bsr	Ed_MemFree
	lea	Ed_MnPrograms(a5),a0		6
	bsr	Ed_MemFree
	lea	EdM_User(a5),a0			7
	bsr	Ed_MemFree
	lea	EdM_Definition(a5),a0		8
	bsr	Ed_MemFree
	rts

;
; Sauvegarde de la configuration courante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Nom
EdC_Save
	move.l	#1006,d2
	bsr	Ed_Open
	beq	.DErr
; Sauve la config elle meme
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Buffer(a5),a0
	move.l	#Ed_FConfig-Ed_DConfig,(a0)
	move.l	a0,d2
	moveq	#4,d3
	bsr	Ed_Write
	bne.s	.DErr
	lea	Ed_DConfig(a5),a0
	move.l	a0,d2
	move.l	#Ed_FConfig-Ed_DConfig,d3
	bsr	Ed_Write
	bne.s	.DErr
; Sauve les chaines systeme
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_Systeme(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les chaines menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	EdM_Messages(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les messages dialogues
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_Messages(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les messages d'erreur TEST TIME
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_TstMessages(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les messages d'erreur RUN TIME
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_RunMessages(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les programmes � lancer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Ed_MnPrograms(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les user menus
; ~~~~~~~~~~~~~~~~~~~~
	lea	EdM_User(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Sauve les definitions menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	EdM_Definition(a5),a0
	bsr	EdC_SaveTextes
	bne.s	EdC_Out
; Ok!
; ~~~
	moveq	#0,d0
	bra	EdC_Out
.DErr	moveq	#1,d0
; Sortie chargement/sauvegarde fichier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdC_Out	move.l	d0,-(sp)
	bsr	Ed_Close
	move.l	(sp)+,d0
EdC_Out2
	tst.l	d0
	rts

;
; Charge un fichier de textes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Adresse du pointeur dans (a5)
;	D7=	Lock
EdC_LoadTextes
	movem.l	a0-a2/d2-d3,-(sp)
	move.l	a0,a2
	bsr	Ed_MemFree
	move.l	Buffer(a5),d2
	moveq	#4,d3
	bsr	Ed_Read
	bne.s	.Err
	move.l	d2,a0
	move.l	(a0),d3
	beq.s	.Skip
	move.l	d3,d0
	move.l	a2,a0
	bsr	Ed_MemReserve
	beq.s	.Err
	move.l	a0,d2
	bsr	Ed_Read
	bne.s	.Err
.Skip	moveq	#0,d0
	bra.s	.Out
.Err	move.l	a2,a0
	bsr	Ed_MemFree
	moveq	#-1,d0
.Out	movem.l	(sp)+,a0-a2/d2-d3
	rts
;
; Sauve un fichier de textes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Adresse du pointeur dans (a5)
;	D7=	Lock
EdC_SaveTextes
	movem.l	a0-a2/d2-d3,-(sp)
	move.l	a0,a2
	move.l	(a2),d0
	bne.s	.Plin
	move.l	a2,d2
	moveq	#0,d3
	bra.s	.Wri
.Plin	move.l	d0,a0
	move.l	-(a0),d3
	move.l	a0,d2
.Wri	addq.l	#4,d3
	bsr	Ed_Write
	bne.s	.Err
	moveq	#0,d0
	bra.s	.Out
.Err	moveq	#-1,d0
.Out	movem.l	(sp)+,a0-a2/d2-d3
	rts

; Change / Ajoute un message dans un fichier de textes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0= 	Base des textes
;	A1=	Nouveau message
;	D0=	Numero du message
EdC_ChangeTexte
	movem.l	a2-a4/d2-d5,-(sp)
	move.l	a0,a2
	move.l	a1,a3
	move.w	d0,d2
; Taille du nouveau message
.Lg	tst.b	(a1)+
	bne.s	.Lg
	sub.l	a3,a1
	subq.l	#1,a1
	move.l	a1,d3
; Nombre de messages
	move.l	(a2),a0
	bsr	Ed_GetNbMessage
	cmp.w	d0,d2
	bhi.s	.New
; Un ancien message, calcul de la taille
	move.w	d2,d0
	move.l	(a2),a0
	bsr	Ed_GetMessageA0
	neg.w	d0
	add.w	d3,d0
	ext.l	d0
	bra.s	.Res
; Un nouveau message
.New	moveq	#2,d0
	add.l	d3,d0
; Reserve la nouvelle memoire
.Res	move.l	(a2),a0
	add.l	-(a0),d0
	move.l	d0,d1
	addq.l	#4,d0
	SyCall	MemFastClear
	beq.s	.Err
	move.l	d1,(a0)+
	move.l	a0,d5
; Recopie les anciens messages
	move.l	(a2),a1
	addq.l	#1,a1
	clr.b	(a0)+
	moveq	#0,d1
	moveq	#0,d4
.Cp0	addq.w	#1,d1
	moveq	#0,d4
	move.b	(a1),d4
	bmi.s	.CpX
	lea	1(a1),a4
	lea	2(a1,d4.w),a1
	cmp.b	d1,d2
	bne.s	.Cp1
	move.l	a3,a4
	move.b	d3,d4
	moveq	#-1,d2
.Cp1	move.b	d4,(a0)+
	beq.s	.Cp3
	subq.w	#1,d4
.Cp2	move.b	(a4)+,(a0)+
	dbra	d4,.Cp2
.Cp3	clr.b	(a0)+
	bra.s	.Cp0
; Le dernier message?
.CpX	tst.w	d2
	bmi.s	.CpY
	move.b	d3,(a0)+
	beq.s	.Cp5
.Cp4	move.b	(a3)+,(a0)+
	subq.b	#1,d3
	bne.s	.Cp4
.Cp5	clr.b	(a0)+
.CpY
; Marque la fin
	move.b	#-1,(a0)+
; Efface l'ancien texte
	move.l	a2,a0
	bsr	Ed_MemFree
; Branche le nouveau, et fini!
	move.l	d5,(a2)
	moveq	#0,d0
	bra.s	.Out
; Erreur, out of mem!
.Err	moveq	#-1,d0
.Out	movem.l	(sp)+,a2-a4/d2-d5
	rts
;
; Get FIRST message free
; ~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Base des messages
Ed_GetFsMessage
	moveq	#0,d0
	moveq	#0,d1
	move.l	a0,a1
.Loop	addq.l	#1,d0
	move.b	1(a0),d1
	beq.s	.Out
	lea	2(a0,d1.w),a0
	bpl.s	.Loop
.New	move.l	a1,a0
	bsr	Ed_GetNbMessage
	addq.w	#1,d0
.Out	rts

;
; Get number of messages
; ~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Base des messages
Ed_GetNbMessage
	moveq	#0,d0
	moveq	#0,d1
	bra.s	.In
.Loop	addq.w	#1,d0
	lea	2(a0,d1.w),a0
.In	move.b	1(a0),d1
	bpl.s	.Loop
	rts
;
; Get new editor message
; ~~~~~~~~~~~~~~~~~~~~~~
; (editor/status.cpp)
		xdef	_ed_get_systeme
_ed_get_systeme
Ed_GetSysteme	jmp	_ed_get_systeme_entry
Ed_GetMessage	jmp	_ed_get_message_entry
Ed_GetMessageA0	jmp	_ed_get_message_a0_entry
Ed_GetNMessage	jmp	_ed_get_next_message_entry


; ___________________________________________________________________
;
;							AUTOSAVE
; ___________________________________________________________________

;
; Set AUTOSAVE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SetAutoSave
	bsr	Ed_TokCur
	move.l	Ed_VDialogues(a5),a2
	move.l	Ed_AutoSaveMn(a5),2*4(a2)
	moveq	#EdD_ASave,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	moveq	#1,d0
	moveq	#3,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,d2
	cmp.l	Ed_AutoSaveMn(a5),d2
	beq	Ed_Loca
	move.l	d2,Ed_AutoSaveMn(a5)
; PAL ou NTSC
	moveq	#50,d3
	EcCall	NTSC
	tst.w	d1
	beq.s	.PAL
	moveq	#60,d3
; Calcule le temps en 1/50
.PAL	mulu	d3,d2
	mulu	#60,d2
	move.l	d2,Ed_AutoSave(a5)
	move.l	#-1,Ed_AutoSaveRef(a5)
	move.b	#1,EdC_Changed(a5)
	bra	Ed_Loca
;
; AUTOSAVE de tous les programmes modifies
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SaveAuto
	move.w	#-1,-(sp)
	move.l	Edt_List(a5),a4
.Loop	tst.b	Edt_Hidden(a4)
	bne.s	.Skip
	tst.l	Edt_LinkPrev(a4)
	bne.s	.Skip
	addq.w	#1,(sp)
	bne.s	.Snd
	moveq	#"I",d0			Va faire du bruit!
	bsr	Ed_SamPlay
.Snd	move.l	Edt_Prg(a4),a6
	bsr	Ed_Saved
.Skip	move.l	Edt_Next(a4),d0
	move.l	d0,a4
	bne.s	.Loop
	addq.l	#2,sp
	rts

; ___________________________________________________________________
;
;					USER MENU / FONCTION TO MENU
; ___________________________________________________________________

;
; Appel d'une fonction USER MENU>>> il faut affecter un programme!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_UserMenu
	bsr	Ed_TokCur
	moveq	#EdD_MnUs,d0
	bsr	Ed_Dialogue
	bra	Ed_Loca
;
; Add user option to menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_AddUser
	bsr	Ed_TokCur
; Nombre d'options
; ~~~~~~~~~~~~~~~~
	move.l	EdM_User(a5),a0
	bsr	Ed_GetFsMessage
	cmp.w	#EdM_UserMax,d0
	bcc.s	.2Many
; Flag pour QUIT
	move.b	#1,EdC_Changed(a5)
	move.w	d0,d3
; Boite de dialogue
; ~~~~~~~~~~~~~~~~~
	move.l	Ed_VDialogues(a5),a0
	clr.l	2*4(a0)
	move.l	#EdM_UserLong,3*4(a0)
	moveq	#EdD_MnUsA,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	moveq	#1,d0
	moveq	#3,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,a1
	tst.b	(a1)
	beq	Ed_NotDone
; Ajoute la fonction
; ~~~~~~~~~~~~~~~~~~
	lea	EdM_User(a5),a0
	move.w	d3,d0
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
; Recalcule le menu et branche � PRG TO MENU
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	EdM_Init
	bsr	Ed_Prg2Menu
	bsr	Ed_Key2Menu
	bra	Ed_Loca
; Too many options
; ~~~~~~~~~~~~~~~~
.2Many	moveq	#EdD_MnUs2,d0
	bsr	Ed_Dialogue
	bra	Ed_Loca

; Delete User Option
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_DelUser
	bsr	Ed_TokCur
; Boite de dialogue
; ~~~~~~~~~~~~~~~~~
	moveq	#EdD_MnUsD,d0
	bsr	Mn_GetOption
	beq	Ed_NotDone
	cmp.w	#EdM_UserCommands-1,d2
	bcs.s	.PaOpt
	cmp.w	#EdM_UserCommands-1+EdM_UserMax,d2
	bcc.s	.PaOpt
; Flag pour QUIT
	move.b	#1,EdC_Changed(a5)
; Change dans le texte
; ~~~~~~~~~~~~~~~~~~~~
	move.w	d2,d0
	sub.w	#EdM_UserCommands-1-1,d0
	lea	EdM_User(a5),a0
	move.l	Buffer(a5),a1
	clr.w	(a1)
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
; Enleve les programmes affectes dans la liste
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	d2,d0
	mulu	#3,d0
	lea	Ed_AutoLoad(a5),a2
	add.w	d0,a2
	tst.b	(a2)
	beq.s	.np2
	clr.b	(a2)+
	moveq	#0,d0
	move.b	(a2)+,d0
	beq.s	.np1
	lea	Ed_MnPrograms(a5),a0
	move.l	Buffer(a5),a1
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
.np1	moveq	#0,d0
	move.b	(a2)+,d0
	beq.s	.np2
	lea	Ed_MnPrograms(a5),a0
	move.l	Buffer(a5),a1
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
; Change dans les touches
; ~~~~~~~~~~~~~~~~~~~~~~~
.np2	move.w	d2,d0
	bsr	Ed_Fonc2Ky
	beq.s	.Skip
	move.b	#1,(a0)+
	move.b	#0,(a0)
; Recalcule le menu
; ~~~~~~~~~~~~~~~~~
.Skip	bsr	EdM_Init
	bra	Ed_Loca
; Pas une option USER
; ~~~~~~~~~~~~~~~~~~~
.PaOpt	moveq	#EdD_MnUsE,d0
	bsr	Ed_Dialogue
	bra	Ed_Loop
;
; Set Program to Menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Prg2Menu
	bsr	Ed_TokCur
; Va chercher l'option de menu
	moveq	#EdD_PrgMn1,d0
	bsr	Mn_GetOption
	beq	Ed_NotDone
	cmp.w	#HiddenCommands-1,d2
	bcc	.PaAff
	cmp.w	#152,d2			Pas un menu HELP
	bcs.s	.Ok
	cmp.w	#181,d2			Ou CONFIG
	bcs	.PaAff
; Flag pour QUIT
.Ok	move.b	#1,EdC_Changed(a5)
	move.w	d2,d3
	move.l	a1,a3			Libelle de l'option
	moveq	#0,d3			Pour EXIT plus loin
; Un programme d�ja affecte?
	mulu	#3,d2
	lea	Ed_AutoLoad(a5),a2
	add.w	d2,a2
	tst.b	(a2)
	beq.s	.NoP
; Boite de dialogue
	moveq	#0,d0
	move.b	1(a2),d0
	beq.s	.NoP
	move.l	Ed_MnPrograms(a5),a0
	bsr	Ed_GetMessageA0
	move.l	Ed_VDialogues(a5),a1
	move.l	a3,0*4(a1)
	move.l	a0,1*4(a1)
	moveq	#EdD_PrgMn2,d0
	bsr	Ed_Dialogue
	move.w	d0,d3
	cmp.w	#3,d3
	beq	Ed_NotDone
; Effacement
	move.b	1(a2),d0			Efface le programme
	move.l	Buffer(a5),a1
	clr.b	(a1)
	lea	Ed_MnPrograms(a5),a0
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
	move.b	2(a2),d0			Efface la command line
	beq.s	.NoP
	move.l	Buffer(a5),a1
	lea	Ed_MnPrograms(a5),a0
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
.NoP	clr.b	(a2)
	clr.b	1(a2)
	clr.b	2(a2)
	cmp.w	#2,d3
	beq	.Exit
; Selecteur de fichiers
	moveq	#100,d0
	bsr	Ed_File_Selector
	beq	Ed_NotDone
	JJsr	L_Dsk.PathIt
	move.l	Ed_VDialogues(a5),a0
	move.l	a3,(a0)+			0= Option
	move.l	Name1(a5),(a0)+			1= Program
	clr.l	(a0)+				2= Pas de chaine en entree
	move.l	#48,(a0)+			3= Longueur
	moveq	#%001,d0
	moveq	#3,d1
	JJsrP	L_Dia_SetVFlags,a2
	moveq	#EdD_PrgMn3,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	move.l	Ed_VDialogues(a5),a0
	lea	4*4(a0),a0
	moveq	#3,d1
	JJsrP	L_Dia_GetVFlags,a2
	or.b	#$80,d0				Met les flags
	move.b	d0,(a2)
	move.l	Ed_MnPrograms(a5),a0
	bsr	Ed_GetFsMessage
	move.b	d0,1(a2)			Pointe le message
	lea	Ed_MnPrograms(a5),a0
	move.l	Name1(a5),a1			Change le message
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
; Une command line?
	moveq	#1,d0
	moveq	#7,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,a1
	tst.b	(a1)
	beq.s	.Exit
	move.l	a1,-(sp)
	move.l	Ed_MnPrograms(a5),a0
	bsr	Ed_GetFsMessage
	move.b	d0,2(a2)			Met la command line
	lea	Ed_MnPrograms(a5),a0
	move.l	(sp)+,a1
	bsr	EdC_ChangeTexte
	bne	Ed_OMm
; Fini
.Exit	bra	Ed_Loca
; This menu option cannot be affected!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PaAff	moveq	#EdD_PrgMnE,d0
	bsr	Ed_Dialogue
	bra	Ed_Loca

;
; Set Key to menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Key2Menu
	bsr	Ed_TokCur
; Va chercher l'option de menu
	moveq	#EdD_KyMn1,d0
	bsr	Mn_GetOption
	beq	Ed_NotDone
	cmp.w	#HiddenCommands-1,d2
	bcc	.PaAff
	move.w	d2,d3
; Trouve la position dans les definitions
	move.w	d3,d0
	lea	Ed_KFonc(a5),a0
	bra.s	.Skip4
.Loop4	addq.l	#2,a0
	tst.b	(a0)
	bne.s	.Loop4
	addq.l	#1,a0
	cmp.b	#$FF,(a0)
	beq	.PaAff
.Skip4	dbra	d0,.Loop4
	move.b	#1,(a0)			Efface l'ancienne definition
	move.b	#0,1(a0)
	move.l	a0,-(sp)
	bsr	EdM_Init
; Va chercher la touche
	moveq	#EdD_KyMn2,d0
	bsr	Ed_Dialogue
	tst.w	d0
	bne	Ed_NotDone
	move.l	Ed_ADialogues(a5),a0
	move.l	Dia_LastKey(a0),d1
	move.l	d1,d4
        beq     Ed_NotDone
	bsr	Ed_Ky2Fonc
	beq.s	.NoAff
; Flag pour QUIT
	move.b	#1,EdC_Changed(a5)
; Key already affected
	moveq	#EdD_KyMn3,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
.NoAff
; Change dans la table
	move.b	d4,d0
        swap    d4
	bsr	MajD0ed
; Met le code ASCII quand on peut!
	move.l	(sp)+,a0
        cmp.b   #"A",d0
        bcs.s   .Scan
        cmp.b   #"Z",d0
        bhi.s   .Scan
        move.b  d0,(a0)
        bra.s   .Shift
; Met le scan code
.Scan   or.b    #$80,d4
        move.b  d4,(a0)
; Met les shifts
.Shift  lsr.w   #8,d4
	moveq	#0,d0
	lea	.Table(pc),a1
.Lp	move.b	(a1)+,d1
	beq.s	.Ot
	move.b	d4,d2
	and.b	d1,d2
	beq.s	.Lp
	or.b	d1,d0
	bra.s	.Lp
.Ot     move.b  d0,1(a0)
; Reactualiser le menu?
.Out    tst.b   EdM_Keys(a5)
        beq.s   .NoMn
        bsr     EdM_Init
; Retour � l'�diteur
.NoMn   bra     Ed_Loca
; This menu option cannot be affected!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PaAff	moveq	#EdD_KyMnE,d0
	bsr	Ed_Dialogue
	bra	Ed_Loca
.Table	dc.b	Shf,Ami,Ctr,Alt,0
	even

; Routine: attend le choix d'une option de menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;       D0=     Boite de dialogue
;
Mn_GetOption
	movem.l	d3-d7/a2-a3,-(sp)
; Boite de dialog
	bsr	Ed_Dialogue		Affiche le dialogue
	moveq	#1,d0			Efface le canal!
	JJsr	L_Dia_EffChannel
; Boucle d'attente
.Loop	JJsr     L_Sys_WaitMul
        SyCall	Inkey
	moveq	#0,d0
	tst.l	d1
	bne	.Out
	SyCall	MouseKey
	btst	#1,d1
	beq.s	.Loop
	bsr	Ed_MnGere
	beq.s	.Loop
; Trouve le libelle de l'option
.Out	movem.l	a0/d0/d2,-(sp)
	bsr	Ed_DrawWindows
	movem.l	(sp)+,a0/d0/d2
	tst.w	d0
	beq.s	.Exit
	bpl.s	.Tab
	bsr	EdM_Error
	moveq	#0,d0
	bra.s	.Exit
.Tab	move.l	EdM_Messages(a5),a1
	move.w	4(a0),d1
	bpl.s	.Mn
	move.l	EdM_User(a5),a1
	neg.w	d1
.Mn	add.w	d1,a1
.Exit	movem.l	(sp)+,d3-d7/a2-a3
	tst.w	d0
	rts


;_____________________________________________________________________________
;
;					MARQUEURS MENU CONFIG
;_____________________________________________________________________________
;
;
; Insert / Overwrite
; ~~~~~~~~~~~~~~~~~~
Ed_Ins	not.b	Ed_Insert(a5)
	bset	#EtA_Ins,Ed_EtatAff(a5)
	bsr	EdM_MarkAll
	bsr	EdK_MarkAll
	rts
; Sound On / Off
; ~~~~~~~~~~~~~~
Ed_SamOn
	not.b	Ed_Sounds(a5)
	bsr	EdM_MarkAll
	move.b	#1,EdC_Changed(a5)
	bsr	Ed_SamChanged
	rts

;_____________________________________________________________________________
;
;							BLOCS
;_____________________________________________________________________________
;

;
; The marked block: src/editor/block.cpp
Ed_BlocAff	jmp	_ed_block_show_entry
Ed_BlocOn	jmp	_ed_block_on_entry
Ed_BlocCu	jmp	_ed_block_cursor_entry
Ed_BlocHide	jmp	_ed_block_hide_entry
Ed_CuNor	jmp	_ed_cursor_normal_entry
Ed_BlocAll	jmp	_ed_block_all_entry
Ed_BlocStore	jmp	_ed_block_store_entry
Ed_BlocForget	jmp	_ed_block_forget_entry
Ed_BlocCut	jmp	_ed_block_cut_entry
Ed_BlocPaste	jmp	_ed_block_paste_entry
Ed_BlocFree	jmp	_ed_block_free_entry
Ed_BlockFreeA0	jmp	_ed_block_free_a0_entry
Ed_BlockLimits	jmp	_ed_block_limits_entry
Ed_BlockInsertA0 jmp	_ed_block_insert_entry
Ed_BlockDeleteA0 jmp	_ed_block_delete_entry
Ed_BlockCopyA0	jmp	_ed_block_copy_entry

; ________________________________________
;
;	SAVE BLOCK
; ________________________________________
;
; The block saved, printed: src/editor/blocksave.cpp
Ed_BlocSave	jmp	_ed_block_save_entry
Ed_BlocSaveAscii	jmp	_ed_block_save_ascii_entry
Ed_PrgPrint	jmp	_ed_print_program_entry
Ed_BlocPrint	jmp	_ed_print_block_entry
Ed_BlocLimits	jmp	_ed_block_limits_2_entry

;____________________________________________________________________
;
;						MACROS
;____________________________________________________________________
;
; Chargement des macros par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_LoadDefault
	moveq	#46,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	bsr	EdMa_LoadIt
	clr.b	EdMa_Changed(a5)
	rts
; Chargement des macros
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_LoadAs
	moveq	#55,d0
	bsr	Ed_File_Selector
	beq	Ed_NotDone
	JJsr	L_Dsk.PathIt
	bsr	EdMa_LoadIt
	move.b	#1,EdMa_Changed(a5)
	rts

; Charge le fichier macros
; ~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_LoadIt
	tst.l	EdMa_List(a5)		Erase old macros?
	beq.s	.Skip
	moveq	#EdD_MacroEra,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
; Averti et charge
.Skip	moveq	#62,d0
	bsr	Ed_AverMess
	bsr	EdMa_Load
	bsr	Ed_AverFin
	beq	.Ok
	bmi	Ed_OMm
	subq.w	#1,d0
	beq	Ed_DError
; Pas un fichier MACRO
	moveq	#EdD_MacroPas,d0
	bsr	Ed_Dialogue
	bsr	Ed_Loca
; Fini
.Ok	rts

; Sauvegarde des macros par default
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_SaveDefault
EdMa_SaveDef
	moveq	#46,d0
	bsr	Ed_GetSysteme
	JJsrR	L_Sys_AddPath,a1
	bsr	EdMa_SaveIt
	rts
; Sauvegarde des macros
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_SaveAs
	moveq	#51,d0
	bsr	Ed_File_Selector
	beq	Ed_NotDone
	JJsr	L_Dsk.PathIt
	bsr	Ed_SaveOver
	bsr	EdMa_SaveIt
	rts
; Averti et sauve
EdMa_SaveIt
	moveq	#61,d0
	bsr	Ed_AverMess
	bsr	EdMa_Save
	bsr	Ed_AverFin
	bne	Ed_DError
	clr.b	EdMa_Changed(a5)
	rts

; Chargement du fichier de macros
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Out	D0=0	Pas d'erreur
;		D0=-1	Erreur disque
;		D0=1	Pas macros
EdMa_Load
; Effacer les precedentes
	bsr	EdMa_End
	clr.b	EdMa_Change(a5)
; Ouvre le fichier
	move.l	#1005,d2
	bsr	Ed_Open
	beq	.Err
; Charge l'entete AMOSPro MACROS
	move.l	Buffer(a5),a2
	move.l	a2,d2
	moveq	#4,d3
	bsr	Ed_Read
	bne	.Err
	cmp.l	#EdMa_Head,(a2)
	bne.s	.Noma
; Charge les macros une par une
.Loop	move.l	a2,d2
	moveq	#8,d3
	bsr	Ed_Read
	bne.s	.Err
	move.l	4(a2),d3
	beq.s	.End
	move.l	d3,d0
	lea	EdMa_List(a5),a0
	bsr	Ed_ListeNew
	beq.s	.Out
	addq.l	#8,a1
	move.l	a1,d2
	bsr	Ed_Read
	bne.s	.Err
	bra.s	.Loop
; Ferme le fichier
.End	bsr	Ed_Close
	moveq	#0,d0
	rts
; Erreur de chargement
.Err	bsr	Ed_Close
	moveq	#1,d0
	rts
; Pas un fichier macro
.Noma	bsr	Ed_Close
	moveq	#2,d0
	rts
; Out of memory
.Out	bsr	Ed_Close
	moveq	#-1,d0
	rts

; Sauvegarde du fichier de macros
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	OUT	D0=0 	Pas d'erreur
;		D0=-1	Erreur disque
EdMa_Save
	move.l	#1006,d2
	bsr	Ed_Open
	beq	Ed_DError
; Sauve l'entete AMOSPro MACROS
	move.l	Buffer(a5),a0
	move.l	#EdMa_Head,(a0)
	move.l	a0,d2
	moveq	#4,d3
	bsr	Ed_Write
	bne.s	.Err
; Sauve les macros une par une
	move.l	EdMa_List(a5),d2
	beq.s	.Fin
.Loop	move.l	d2,a2
	move.l	4(a2),d3
	addq.l	#8,d3
	bsr	Ed_Write
	bne.s	.Err
	move.l	(a2),d2
	bne.s	.Loop
; Fin : 0/0
.Fin	move.l	Buffer(a5),d2
	move.l	d2,a0
	clr.l	(a0)+
	clr.l	(a0)+
	moveq	#8,d3
	bsr	Ed_Write
	bne.s	.Err
; Ferme le fichier
	bsr	Ed_Close
	moveq	#0,d0
	rts
; Erreur disque
.Err	bsr	Ed_Close
	moveq	#1,d0
	rts

; Effacement d'une macro
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_Del
	bsr	Ed_TokCur
	tst.l	EdMa_List(a5)
	beq	EdMa_No
	moveq	#EdD_MacroD,d0
	bsr	Ed_Dialogue
	tst.w	d0
	bne	Ed_NotDone
	move.l	Ed_ADialogues(a5),a0
	move.l	Dia_LastKey(a0),d2
	beq	Ed_NotDone
	move.l	d2,d1
	bsr	EdMa_Adr
	beq.s	.NotA
	bsr	Ed_ListeDel
; Marque pour QUIT
	move.b	#1,EdMa_Change(a5)
	bra	Ed_Loca
; Key not assigned to a macro
.NotA	moveq	#EdD_MacroNA,d0
	bsr	Ed_Dialogue
	bra	Ed_Loca
; Effacement de toutes les macros
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_DelAll
	bsr	Ed_TokCur
	tst.l	EdMa_List(a5)
	beq	EdMa_No
	moveq	#EdD_MacroAll,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	bsr	EdMa_End
; Marque pour QUIT
	move.b	#1,EdMa_Change(a5)
	bra	Ed_Loca
; D�marre l'enregistrement d'une nouvelle macro
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_New
	bsr	Ed_TokCur
; Boite de dialogue 1
	moveq	#EdD_Macro1,d0
	bsr	Ed_Dialogue
	tst.w	d0
	bne	Ed_NotDone
	move.l	Ed_ADialogues(a5),a0
	move.l	Dia_LastKey(a0),d2
	beq	Ed_NotDone
	move.l	d2,d1
	bsr	EdMa_Adr
	beq.s	.Skip
; Key already assigned to a macro
	move.l	a1,-(sp)
	moveq	#EdD_Macro2,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	move.l	(sp)+,a1
	lea	EdMa_List(a5),a0
	bsr	Ed_ListeDel
; R�serve le buffer temporaire
.Skip	move.l	#1024,d0
	move.l	d0,d1
	lea	EdMa_List(a5),a0
	bsr	Ed_ListeNew
	move.w	#1,EdMa_Tape(a5)
	move.l	d2,8(a1)
	move.l	#-1,8-4(a1,d1.w)
; Provoque l'affichage de la ligne infos
	bsr	Ed_AverFin
	bsr	Ed_Loca
	rts

; Termine l'enregistrement de la macro
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_macro_stop
_ed_macro_stop
EdMa_Stop
; Plus de souris
	JJsr	L_Dia_NoMKey
	move.b	#1,EdMa_Change(a5)
; Reserve le buffer d�finitif
	lea	EdMa_List(a5),a0
	move.l	(a0),a2
	move.l	a2,d3
	lea	8(a2),a2
	move.w	EdMa_Tape(a5),d2
	clr.w	EdMa_Tape(a5)
	subq.w	#1,d2
	beq	.Vide
	moveq	#8,d0
	add.w	d2,d0
	bsr	Ed_ListeNew
; Copie le buffer
	lea	8(a1),a1
	move.l	(a2)+,(a1)+
	subq.w	#1,d2
.Loop	move.b	(a2)+,(a1)+
	dbra	d2,.Loop
	move.b	#-1,(a1)+
; Enleve le buffer temporaire
	move.l	d3,a1
	bsr	Ed_ListeDel
; Avertissement
	moveq	#45,d0
	bra	Ed_Al100
; Pas de touches!
.Vide	move.l	d3,a1
	bsr	Ed_ListeDel
	bra	Ed_NotDone

; Trouve une macro de code D1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_Adr
	lea	EdMa_List(a5),a0
	move.l	(a0),d0
	beq.s	.NFound
.Loop	move.l	d0,a1
	cmp.l	8(a1),d1
	beq.s	.Found
.In	move.l	(a1),d0
	bne.s	.Loop
.NFound	rts
; Trouve
.Found	move.l	a1,d0
	rts
; Efface l'arbre des macros
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdMa_End
	lea	EdMa_List(a5),a0
	bra	Ed_ListeDelAll
; Pas une macro
; ~~~~~~~~~~~~~
EdMa_No
	moveq	#EdD_MacroNo,d0
	bsr	Ed_Dialogue
	bra	Ed_Loca

;_____________________________________________________________________________
;
;						Search / Replace / Goto
;_____________________________________________________________________________
;

; Goto line number
; ~~~~~~~~~~~~~~~~
Ed_GotoL
	bsr	Ed_TokCur
	tst.b	Ed_Zappeuse(a5)
	beq.s	.PaZap
	move.l	Ed_ZapParam(a5),d0
	bra.s	.Zap
.PaZap	moveq	#EdD_GotoL,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	moveq	#1,d0
	moveq	#3,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,d0
.Zap	subq.l	#1,d0
	bmi	Ed_NotDone
	cmp.w	Prg_NLigne(a6),d0
	bcs.s	.Skip
	move.w	Prg_NLigne(a6),d0
.Skip	bsr	Ed_AutoMarks
	bsr	Ed_GotoY
	rts


; Appel le dialogue pour SEARCH, d0DIALOG A APPELER
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_DiaS	move.w	d0,d2
	lea	Ed_SchBuf(a5),a0		Chaine d'origine
	bsr	Ed_DChaine
	move.l	Ed_VDialogues(a5),a2
	move.l	a1,(a2)+
	move.l	#32,(a2)+			Taille de la ligne
	move.w	Ed_SchMode(a5),d0		Met les flags
	and.w	#%0001,d0			Garde LOWUP / Bloc limits
	moveq	#4,d1
	move.l	a2,a0
	JJsrP	L_Dia_SetVFlags,a2
; Appelle les dialogues
	move.w	d2,d0
	bsr	Ed_Dialogue
	move.w	d0,-(sp)
; Reprend les variables
	move.l	Ed_VDialogues(a5),a0
	lea	2*4(a0),a0
	moveq	#4,d1
	JJsrP	L_Dia_GetVFlags,a2
	move.w	d0,Ed_SchMode(a5)
; Reprend la chaine
	moveq	#1,d0
	moveq	#3,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,a0
	lea	Ed_SchBuf(a5),a1
.Loop	move.b	(a0)+,(a1)+
	bne.s	.Loop
; Ramene le retour
	move.w	(sp)+,d0
	rts

; SEARCH
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Search
	bsr	Ed_TokCur
	moveq	#EdD_Search,d0
	bsr	Ed_DiaS
	cmp.w	#1,d0
	bne	Ed_NotDone
	move.w	Ed_SchMode(a5),d5	Garde Low/UP, et direction!
	and.w	#%0011,d5
	bra	Ed_SR
; SEARCH PREVIOUS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SearchPrev
	bsr	Ed_TokCur
	tst.b	Ed_SchBuf(a5)			Quelque chose � trouver?
	beq.s	Ed_Search
	move.w	Ed_SchMode(a5),d5
	and.w	#%0001,d5			Garde LowUp
	bset	#1,d5				En arriere!
	bra.s	Ed_SR
; SEARCH NEXT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SearchNext
	bsr	Ed_TokCur
	tst.b	Ed_SchBuf(a5)			Quelque chose � trouver?
	beq.s	Ed_Search
	move.w	Ed_SchMode(a5),d5
	and.w	#%0001,d5			Garde LowUp, en avant!
; Fait la recherche / Remplacement, D5flags
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SR
	bsr	Ed_SRCompte
; Fait le remplacement
	move.w	Edt_YPos(a4),d7
	add.w	Edt_YCu(a4),d7
	move.w	Edt_XCu(a4),d6
	move.w	#32000,d4			Limites en bas
	move.w	#32000,d3
; Va chercher
	moveq	#36,d0
	bsr	Ed_AverMess
	bsr	Ed_SchBoth
	beq	Ed_NoFound
; Positionne le curseur
	bsr	Ed_AutoMarks
	movem.w	d2-d7,-(sp)
	move.w	d7,d0
	bsr	Ed_GotoY
	movem.w	(sp),d2-d7
	move.w	d6,d0
	bsr	Ed_GotoX
	movem.w	(sp)+,d2-d7
; Trouve, faire une remplacement?
	btst	#15,d5
	beq.s	.NoRep
	bsr	Ed_LCourant
	move.w	d6,d0
	bsr	RepBuffer
	bne	Ed_LToLong
	addq.w	#1,Edt_LEdited(a4)
	bsr	Ed_TokCur
	move.w	d6,d0
	bsr	Ed_GotoX
.NoRep
; Fini!
	bsr	Ed_AverFin
	bsr	Ed_Loca
	rts


; Compte les buffers SEARCH/REPLACE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SRCompte
	lea	Ed_SchBuf(a5),a0
	moveq	#-1,d0
.LoopS	addq.w	#1,d0
	tst.b	(a0)+
	bne.s	.LoopS
	move.b	d0,Ed_SchLong(a5)
	lea	Ed_RepBuf(a5),a0
	moveq	#-1,d0
.LoopR	addq.w	#1,d0
	tst.b	(a0)+
	bne.s	.LoopR
	move.b	d0,Ed_RepLong(a5)
	rts

; Ed_SchBoth, Ed_SchFront, Ed_SchBack: src/editor/search.cpp
Ed_SchBoth	jmp	_ed_search_entry
Ed_SchFront	jmp	_ed_search_front_entry
Ed_SchBack	jmp	_ed_search_back_entry


; REPLACE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Replace
	bsr	Ed_TokCur
	lea	Ed_RepBuf(a5),a0		Buffer d'edition
	bsr	Ed_DChaine
	move.l	Ed_VDialogues(a5),a2
	move.l	a1,8*4(a2)
	moveq	#EdD_Replace,d0
	bsr	Ed_DiaS
	move.w	d0,-(sp)
; Reprend la chaine Replace
	moveq	#1,d0
	moveq	#4,d1
	moveq	#1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,a0
	lea	Ed_RepBuf(a5),a1
.Bli	move.b	(a0)+,(a1)+
	bne.s	.Bli
; Ou se brancher?
	cmp.w	#1,(sp)+
	bne	Ed_Loop
	move.w	Ed_SchMode(a5),d5		Mode TURBO?
	move.w	d5,d0
	and.w	#%1100,d0
	bne.s	.Turbo
; Mode normal, quelle dir?
; ~~~~~~~~~~~~~~~~~~~~~~~~
	btst	#1,d5
	beq	Ed_ReplaceNext
	bne	Ed_ReplacePrev
; En mode turbo
; ~~~~~~~~~~~~~
.Turbo
; Boite de dialogue, pour confirmer...
	move.l	Ed_VDialogues(a5),a0
	moveq	#EdD_WBlock,d0
	btst	#2,d5
	bne.s	.Skip
	moveq	#EdD_WText,d0
.Skip	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
; Quelque chose � chercher?
	tst.b	Ed_SchBuf(a5)
	beq.s	Ed_Replace
	tst.b	Ed_RepBuf(a5)
	beq.s	Ed_Replace
	bsr	Ed_SRCompte
; Fait le remplacement
	moveq	#0,d7				Limites en haut
	moveq	#0,d6
	move.w	#32000,d4			Limites en bas
	move.w	#32000,d3
; Dans le bloc?
	btst	#2,d5				Si bloc
	beq.s	.Pab
	bsr	Ed_BlocLimits
	move.w	d1,d7
	move.w	d0,d6
	move.w	d3,d4
	move.w	d2,d3
	beq.s	.Pab
	subq.b	#1,d3
.Pab
; Averti
	moveq	#37,d0
	bsr	Ed_AverMess
; Boucle de recherche / remplacement
	clr.w	-(sp)
.Loop	subq.w	#1,d6			Reste sur la derniere lettre
	bpl.s	.Pos
	moveq	#0,d6
.Pos	bsr	Ed_SchFront
	beq	.Finish
	move.l	Ed_BufT(a5),a0
	addq.l	#2,a0
	move.w	d6,d0
	bsr	RepBuffer
	bne	Ed_LToLong
	move.l	Ed_BufT(a5),a0
	addq.l	#2,a0
	move.l	Buffer(a5),a1
	bsr	Tokenise
	beq.s	.Llong
	move.l	Buffer(a5),a1
	move.w	d7,d1
	moveq	#0,d0
	bsr	Ed_Stocke
	bne	.Outb
	addq.w	#1,(sp)
	bra.s	.Loop
; Fini, et une boite de dialogue!
.Finish	bsr	Ed_NewBuf
	bsr	Ed_AverFin
	move.b	#EtA_BAll,Edt_EtatAff(a4)
	move.b	#SlDelai,Edt_ASlY(a4)
	move.w	(sp)+,d0
	beq	Ed_NoFound
	ext.l	d0
	move.l	Ed_VDialogues(a5),a0
	move.l	d0,(a0)
	moveq	#EdD_Changes,d0
	bsr	Ed_Dialogue
	rts
; Line too long
.Llong	bsr	Ed_NewBufAff
	bra	Ed_LToLong
; Out of buffer space
.Outb	bsr	Ed_NewBufAff
	bra	Ed_OofBuf
; REPLACE NEXT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ReplaceNext
	move.w	Ed_SchMode(a5),d5
	and.w	#$0001,d5		Vers le bas,
	or.w	#$8000,d5		Flag remplacer
Ed_RSR	bsr	Ed_TokCur
	tst.b	Ed_SchBuf(a5)
	beq	Ed_Replace
	tst.b	Ed_RepBuf(a5)
	beq	Ed_Replace
	bra	Ed_SR
; REPLACE PREVIOUS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ReplacePrev
	move.w	Ed_SchMode(a5),d5
	and.w	#$0001,d5		Vers le haut
	or.w	#$8002,d5		Flag remplacer
	bra.s	Ed_RSR

; Routine REPLACE (a0 the line, d0 the column): src/editor/search.cpp
RepBuffer	jmp	_ed_replace_text_entry

;_____________________________________________________________________________
;
;					Procedures actualisations sliders
;_____________________________________________________________________________
;
		xdef	_ed_slider_routines
_ed_slider_routines
Edt_SliderY
	bra	Edt_SlUpdateY
	bra	Edt_SlMinusY
	bra	Edt_SlPlusY
	bra	Edt_SlDrawY
		xdef	_ed_sl_update_y
_ed_sl_update_y
Edt_SlUpdateY
	move.w	Edt_WindTy(a4),Edt_SlV+Sl_Window(a4)
	move.w	Edt_YPos(a4),Edt_SlV+Sl_Position(a4)
	move.w	Prg_NLigne(a6),Edt_SlV+Sl_Global(a4)
	rts
Edt_SlMinusY
Edt_SlPlusY
Edt_SlDrawY
	movem.l	a2-a6/d2-d7,-(sp)
	move.w	Edt_SlV+Sl_Position(a4),Edt_YPos(a4)
	bsr	Ed_NewBuf
	movem.l	(sp)+,a2-a6/d2-d7
	rts

;_____________________________________________________________________________
;
;							Ligne d'�tat
;_____________________________________________________________________________
;

; The parts of the status line (editor/status.cpp)
Ed_EtLine	jmp	_ed_status_y_entry
Ed_EtCol	jmp	_ed_status_x_entry
Ed_EtFree	jmp	_ed_status_free_entry
Ed_EtOrder	jmp	_ed_status_order_entry
Ed_EtIns	jmp	_ed_status_insert_entry
Ed_EtNom	jmp	_ed_status_name_entry
Ed_NPrgToBuf	jmp	_ed_name_to_buffer_entry
; Alerts, warnings, the status line (editor/status.cpp)
		xdef	_ed_r_alert
_ed_r_alert
Ed_RAlert	jmp	_ed_recall_alert_entry
Ed_Alert	jmp	_ed_alert_entry
Ed_ZapAlert	jmp	_ed_zap_alert_entry
Ed_RazAlert	jmp	_ed_alert_clean_entry
Ed_AlertRts	jmp	_ed_alert_rts_entry
		xdef	_ed_av_name
_ed_av_name
Ed_AvName	jmp	_ed_warn_name_entry
		xdef	_ed_warn
_ed_warn
Ed_AverMess	jmp	_ed_warn_message_entry
Ed_Avertir	jmp	_ed_warn_entry
		xdef	_ed_aver_fin
_ed_aver_fin
Ed_AverFin	jmp	_ed_warning_end_entry
		xdef	_ed_all_aver_fin
_ed_all_aver_fin
Ed_AllAverFin	jmp	_ed_all_warnings_end_entry
		xdef	_ed_status_print
_ed_status_print
Ed_EtPrint	jmp	_ed_status_line_entry
Ed_EtPrintD0	jmp	_ed_status_line_d0_entry
Et_Print	jmp	_ed_status_text_entry


;____________________________________________________________________
;
;				RUN / MULTIRUN / TEST / INDENT
;____________________________________________________________________
;
;
; 	APPEL DU MONITEUR PAR LE MENU
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_GoMonitor
	bsr	Ed_TokCur
	bsr	Edt_ClearVar		Plus aucune variable, si HELP!
	bsr	Ed_Hide			Plein de memoire
	bsr	Ed_CloseEditor
	JJsr	L_Prg_SetBanks
	JJsr	L_Mon_Load		Va charger le moniteur
	bne.s	.Load
	move.l	a4,Edt_Runned(a5)	Programme >> runn�
	lea	Ed_ErrRun(pc),a0	Retour du moniteur
	move.l	a0,Prg_JError(a5)
	JJsr	L_Mon_In_Editor
	clr.l	Edt_Runned(a5)		Revient >>> Out of memory!
.Mem	move.w	#204,d0
.Err	move.l	d0,-(sp)
	bsr	Ed_OpenEditor		Rouvre l'editeur
	bsr	Ed_Appear
	move.l	(sp)+,d0
	bra	Ed_Al100
.Load	cmp.w	#-1,d0
	beq.s	.Mem
	move.w	#222,d0
	bra.s	.Err

; ___________________________________________________________________
;
; 	LANCE/CHARGE UN PROGRAMME COMME OPTION EDITEUR
; ___________________________________________________________________
;
;	A0=	Definition du programme
;
Ed_PrgCommand
	move.l	a0,a2
; Gestion ligne de commande
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d0
	move.b	2(a2),d0
	beq.s	.NoCom
; Copie la ligne de commande
	move.l	Ed_MnPrograms(a5),a0
	bsr	Ed_GetMessageA0
	moveq	#0,d0
	move.b	-1(a0),d0
	bra.s	.Cop
; Copie la ligne courante � partir du curseur
.NoCom	bsr	Ed_LCourant
	move.l	a0,d0
	sub.l	a1,d0
	add.w	-2(a0),d0
	move.l	a1,a0
; Copie
.Cop	move.l	Buffer(a5),a1
	lea	TBuffer-256-6(a1),a1
	move.l	#"CmdL",(a1)+
	move.w	d0,(a1)+
	subq.w	#1,d0
	bmi.s	.Sko
.Loop	move.b	(a0)+,(a1)+
	dbra	d0,.Loop
.Sko
; On peut tokeniser maintenant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_TokCur
	btst	#0,(a2)
	bne	.Hidden
; Charger le programme dans la fenetre courante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a2,-(sp)
	bsr	Ed_Saved		Va sauver le programme
	move.l	(sp)+,a2
; Recharger le programme?
	btst	#2,(a2)
	beq.s	.Sauv
	lea	Ed_ErrRun(pc),a0	Un message de fin...
	bra.s	.NLRun
; Sauve le programme
.Sauv	move.l	#Edt_EReload-Edt_SReload+4+4*10+128,d0
	lea	Ed_Prg2ReLoad(a5),a0
	bsr	Ed_MemReserve
	beq	Ed_OMm
	move.l	Prg_StTTexte(a6),(a0)+
	lea	Prg_Marks(a6),a1
	moveq	#9,d0
.Cm	move.l	(a1)+,(a0)+
	dbra	d0,.Cm
	lea	Edt_SReload(a4),a1
	move.w	#Edt_EReload-Edt_SReload-1,d0
.Cc	move.b	(a1)+,(a0)+
	dbra	d0,.Cc
	lea	Prg_NamePrg(a6),a1
.Cn	move.b	(a1)+,(a0)+
	bne.s	.Cn
	lea	Ed_ErrRunHidden(pc),a0		Pas de message
	move.b	#1,Ed_RunnedHidden(a5)		Flag, si Kill Editor
; NEW + LOAD + RUN!
; ~~~~~~~~~~~~~~~~~
.NLRun	move.l	a0,-(sp)
; Copie le nom
	moveq	#0,d0
	move.b	1(a2),d0
	move.l	Ed_MnPrograms(a5),a0	Prend le nom
	bsr	Ed_GetMessageA0
	move.l	Name1(a5),a1
.Bli	move.b	(a0)+,(a1)+
	bne.s	.Bli
	bsr	Ed_New2			New!
; Charge le programme
	move.w	#155,d0			Message, je charge!
	move.l	Name1(a5),a0
	bsr	Ed_AvName
	moveq	#-1,d0			Adapter la taille
	JJsr	L_Prg_Load
	bne	Ed_PrgLoadErr
	bsr	Ed_AverFin
; Run special
	move.l	Ed_BufT(a5),a0		Plus de Ed_RAlert
	clr.l	256(a0)
	bsr	Ed_BlocFree		Un peu de place
	bsr	Prg_RazUndos
	bsr	EdM_End
	bsr	EdM_Program
	move.l	a4,Edt_Runned(a5)	Programme runn�
	move.l	(sp)+,a1		Retour de fin variable!
	lea	Ed_TestMessage(pc),a2	Affichage tests
	moveq	#0,d0			Programme normal
	JJsr	L_Prg_RunIt
	clr.l	Edt_Runned(a5)		Si revient!
	bra	Ed_OMm			Out of mem si revient

; Charger le programme en HIDDEN
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Hidden	moveq	#0,d0
	move.b	1(a2),d0
	move.l	Ed_MnPrograms(a5),a0
	bsr	Ed_GetMessageA0
	move.l	Name1(a5),a1
.Copy	move.b	(a0)+,(a1)+
	bne.s	.Copy
; Recherche un accessoire deja charge
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Edt_AccAdr
	bne.s	.Deja
; Charge dans une fenetre cachee
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	JJsr	L_Dsk.PathIt
	bsr	Ed_RLoadHidden
	clr.l	Ed_WindowToDel(a5)
	btst	#2,(a2)
	bne.s	.Garde
	move.b	#-1,Edt_PrgDelete(a4)
	bra.s	.Skip
.Garde	bsr	EdM_BranchAMOS
	bsr	EdM_Program
.Skip	move.l	a4,a0
; Branche au RUN HIDDEN normal!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Deja	bra	Ed_RunHide
;
; Recharge le programme pr�c�dent
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_PrgReLoad
; New du programme pr�c�dent
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_New2
; Taille du buffer
; ~~~~~~~~~~~~~~~~
	move.l	Ed_Prg2ReLoad(a5),a2
	move.l	(a2)+,d0
	JJsr	L_Prg_ChgTTexte
	beq.s	.Out
; Marks
; ~~~~~
	lea	Prg_Marks(a6),a0
	moveq	#9,d0
.Cm	move.l	(a2)+,(a0)+
	dbra	d0,.Cm
; Donnee d'edition
; ~~~~~~~~~~~~~~~~
	lea	Edt_SReload(a4),a0
	move.w	#Edt_EReload-Edt_SReload-1,d0
.Cc	move.b	(a2)+,(a0)+
	dbra	d0,.Cc
; Recharge le programme
; ~~~~~~~~~~~~~~~~~~~~~~
	tst.b	(a2)
	beq.s	.Nol
	move.l	Name1(a5),a0
.Cn	move.b	(a2)+,(a0)+
	bne.s	.Cn
; Chargement!
; ~~~~~~~~~~~
	move.w	#155,d0
	move.l	Name1(a5),a0
	bsr	Ed_AvName
	moveq	#1,d0			Revenir si pas assez grand
	JJsr	L_Prg_Load
	bne.s	.Nol
	move.b	#1,Prg_StModif(a6)	Force le test
	bsr	Ed_AverFin
	bsr	Ed_NewBufAff
.Nol	bsr	Ed_AverFin
	bsr.s	.Free			Liberation de la structure
	bsr	Ed_RAlert		Si erreur dans le programme precedent
	bra	Ed_Loop
; Out of mem
; ~~~~~~~~~~
.Out	move.l	#1024,d0
	JJsr	L_Prg_ChgTTexte
	bsr	Edt_New
	bsr	.Free
	bra	Ed_OMm
; Libere la structure + reaffiche le buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Free	lea	Ed_Prg2ReLoad(a5),a0
	bsr	Ed_MemFree
	move.l	a4,a0
	moveq	#%10,d0
	bsr	Ed_Splitted
	rts

;
; TROUVE L'ADRESSE DU PROGRAMME NAME1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	In	Name1	Nom � cherche
;	Out	Trouve	BNE, A0=adresse
;		Non	BEQ
;
Edt_AccAdr
	movem.l	a2/d2,-(sp)
	move.l	Name1(a5),a0
	bsr	Ed_DNom
	move.l	a0,d2
	move.l	Edt_List(a5),d0
	beq.s	.Out
.Loop0	move.l	d0,a2
	move.l	Edt_Prg(a2),a0
	lea	Prg_NamePrg(a0),a0
	bsr	Ed_DNom
	move.l	d2,a1
.Loop1	move.b	(a0)+,d0
	bsr	MajD0ed
	move.b	d0,d1
	move.b	(a1)+,d0
	bsr	MajD0ed
	cmp.b	d0,d1
	bne.s	.Next
	or.b	d1,d0
	bne.s	.Loop1
	move.l	a2,d0
	bra.s	.Ok
.Next	move.l	Edt_Next(a2),d0
	bne.s	.Loop0
.Out	moveq	#0,d0
.Ok	move.l	d0,a0
	movem.l	(sp)+,a2/d2
	rts


; ___________________________________________________________________
;
; 						RUN / TEST / INDENT
; ___________________________________________________________________
;

;
; RUN HIDDEN PROGRAM
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
Ed_RunHidden
	bsr	Ed_TokCur		Ligne courante
	bsr	Ed_GetHidden		Cherche le programme
	beq	Ed_NotDone
	move.l	Buffer(a5),a1		Pas de command line!
	lea	TBuffer-256-6(a1),a1
	clr.l	(a1)
; Entree 2: run le programme A0
Ed_RunHide
	move.l	a0,Edt_Runned(a5)	Programme editeur runne
	move.l	Edt_Prg(a0),a6		Structure programme
	bsr	Ed_BlocFree		Un peu de place
	bsr	Prg_RazUndos
	bsr	EdM_End
	bsr	EdM_Program
	move.b	#1,Ed_RunnedHidden(a5)	Flag en cas de KILL EDITOR
	lea	Ed_ErrRunHidden(pc),a1	Retour de fin, sans ligne
	lea	Ed_TestMessage(pc),a2	Affichage tests
	moveq	#1,d0			Accessoire!
	JJsr	L_Prg_RunIt
	clr.l	Edt_Runned(a5)		Si revient
	beq	Ed_OMm			Out of memory
	bra	Ed_AlRunned		Already runned

;
; RUN EN MODE DIRECT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_RunDirect
	move.l	Edt_Current(a5),a4	Prend le programme courant
	move.l	Edt_Prg(a4),a6
	bsr	Ed_BlocFree		Un peu de place
	bsr	Prg_RazUndos
	bsr	EdM_End
	bsr	EdM_Program
	move.l	Buffer(a5),a1		Pas de command line!
	lea	TBuffer-256-6(a1),a1
	clr.l	(a1)
	move.l	a4,Edt_Runned(a5)	Programme runn�
	lea	Ed_DErrRun(pc),a1	Retour de fin normal
	lea	Ed_DTestMessage(pc),a2	Affichage tests
	moveq	#0,d0			Programme normal
	JJsr	L_Prg_RunIt
	clr.l	Edt_Runned(a5)		Si revient!
	bsr	Esc_Hide
	bsr	Ed_Appear
	bra	Ed_OMm			Out of mem si revient
Ed_DErrRun
	tst.w	Direct(a5)		Si ESC plus la >>> Erreurs normales
	beq	Ed_ErrRun
	bne	Ed_ErrDirect
Ed_DTestMessage
	rts				0 -> Rien
	nop
	rts				1 -> Rien
	nop
	bra	Esc_Hide		2 -> Fermeture de l'ecran

;
; RUN PROGRAMME COURANT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Run	bsr	Ed_TokCur		Ligne courante
	bsr	Ed_BlocFree		Un peu de place
	bsr	Prg_RazUndos
	bsr	EdM_End
	bsr	EdM_Program
	move.l	Buffer(a5),a1		Pas de command line!
	lea	TBuffer-256-6(a1),a1
	clr.l	(a1)
	move.l	a4,Edt_Runned(a5)	Programme runn�
	lea	Ed_ErrRun(pc),a1	Retour de fin normal
	lea	Ed_TestMessage(pc),a2	Affichage tests
	moveq	#0,d0			Programme normal
	JJsr	L_Prg_RunIt
	clr.l	Edt_Runned(a5)		Si revient!
	bra	Ed_OMm			Out of mem si revient

; RETOUR DE PROGRAMME HIDDEN SOUS EDITEUR
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ErrRunHidden
	move.l	Edt_Runned(a5),a4	Prend le programme courant
	move.l	Edt_Prg(a4),a6
	clr.l	Edt_Runned(a5)
	clr.b	Ed_RunnedHidden(a5)	Enleve le flag
; Prend le message
	bsr	Ed_GetError
	move.l	a0,-(sp)
	move.l	d0,-(sp)
; Configuration modifiee?
	tst.b	EdC_Modified(a5)	Tout modifie?
	beq.s	.Normal
	bsr	EdC_Redraw		Ferme et ouvre!
	bra.s	.Suite
; Configuration non modifiee
.Normal	tst.b	Prg_Accessory(a5)	Si accessoire
	beq.s	.Paac
	bsr	Ed_Redraw		Redessin de l'editeur
	bra.s	.Suite
.Paac	bsr	Ed_OpenEditor		ROuvre l'editeur
	bsr	Esc_Hide		Inutile, mais bon
	bsr	Ed_Appear
; Efface le programme si necessaire
.Suite	bsr	Edt_ClearVar
	tst.b	Edt_PrgDelete(a4)
	beq.s	.No
	bsr	Edt_DelWindow
	bsr	EdM_BranchAMOS
; Affiche le message dans le programme courant
.No	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_SetBanks		Change les banques
	JJsr	L_Bnk.Change		Envoie aux trappes / Musiques
	bsr	Ed_SetBanks		Retour aux banques editeur
	move.l	(sp)+,d0
	bmi.s	.Aue
	cmp.w	#10,d0			END / EDIT
	beq	Ed_Loop
	cmp.w	#1000,d0		Retour simple � l'editeur
	beq	Ed_Loop
	cmp.w	#1002,d0		Retour au workbench
	beq	Ed_System
.Aue	move.l	(sp)+,a0
; Fabrique le message Error in hidden
	move.l	a0,-(sp)
	moveq	#9,d0
	bsr	Ed_GetMessage
	move.l	Ed_BufT(a5),a1
.CopM1	move.b	(a0)+,(a1)+
	bne.s	.CopM1
	subq.l	#1,a1
	move.l	(sp)+,a0
.CopM2	move.b	(a0)+,(a1)+
	bne.s	.CopM2
	move.b	#".",-1(a1)
	clr.b	(a1)
; Imprime
	move.l	Ed_BufT(a5),a0
	move.w	#200,d0
	bra	Ed_Alert

; RETOUR DE TEST SOUS EDITEUR
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ErrTest
	move.l	Edt_Current(a5),a4	Pas d'effacement Edt_Runned!
	move.l	Edt_Prg(a4),a6
	bra.s	Ed_Errr
; RETOUR DE PROGRAMME NORMAL SOUS EDITEUR
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ErrRun
	tst.b	Ed_RunnedHidden(a5)
	bne	Ed_ErrRunHidden
	move.l	Edt_Runned(a5),a4	Prend le programme courant
	move.l	Edt_Prg(a4),a6
	clr.l	Edt_Runned(a5)
	tst.b	Prg_Reloaded(a6)	On a change le programme???
	beq.s	Ed_Errr
	clr.b	Prg_Reloaded(a6)
	move.w	#-1,Edt_YBloc(a4)
Ed_Errr	tst.l	d0			Error TESTING
	bmi	Ed_ErrEdit
	cmp.w	#1000,d0		Edit
	beq	Ed_ErrEdit
	cmp.w	#1001,d0		Direct
	beq	Ed_ErrDirect
	cmp.w	#1002,d0		System
	beq	Ed_System
	bsr	Ed_Ligne		Ligne de choix direct / edit
	cmp.w	#1,d1
	beq	Ed_ErrDirect
; Retour � l'�diteur
; ~~~~~~~~~~~~~~~~~~
Ed_ErrEdit
	bsr	Ed_GetError
	move.l	a0,-(sp)
	move.l	d0,-(sp)
; Apparition de l'�diteur
	bsr	Ed_OpenEditor		ROuvre l'editeur
	bsr	Esc_Hide
	bsr	Ed_Appear
; Retour � l'editeur si END ou EDIT
	move.l	(sp)+,d0		Message de test?
	bmi.s	.Skip
	cmp.w	#10,d0
	beq	Ed_Loop
	cmp.w	#1000,d0
	beq	Ed_Loop
; Positionne le curseur sur l'erreur
.Skip	move.l	VerPos(a5),a0
	bsr	Ed_FindA		Cherche le numero / adresse
	clr.l	Prg_AdEProc(a6)
	tst.l	d1
	beq.s	.Ski
	move.l	a0,Prg_AdEProc(a6)	Si procedure fermee--> Stocke!
	move.w	d0,-(sp)
	move.l	d1,-(sp)
	move.l	Buffer(a5),a1
	move.l	VerPos(a5),d0
	bsr	Detok
	move.w	d0,Prg_XEProc(a6)
	clr.l	VerPos(a5)
	move.l	(sp)+,a0
	move.w	(sp)+,d0
.Ski	bsr	Ed_SetY			Localise en Y
	move.l	Buffer(a5),a1		Offset de l'erreur
	move.l	VerPos(a5),d0
	bsr	Detok
	bsr	Ed_SetX
	bsr	Ed_NewBufAff
	move.l	(sp)+,a0		Affiche le message
	move.l	Ed_BufT(a5),a1		Avec un "." a la fin
	bsr	EdCocop
	move.b	#".",-1(a1)
	clr.b	(a1)
	move.l	Ed_BufT(a5),a0
	move.w	#200,d0
	bra	Ed_Alert

; Trouve le libelle de l'erreur en retour de programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_GetError
	ext.l	d0
	move.l	d0,d2
	move.l	a0,d1
	bne.s	.Clair
	cmp.w	#256,d0			Un message?
	bge.s	.Clair
	move.l	Ed_TstMessages(a5),a0	Message testing
	neg.w	d0
	move.w	d0,d2
	bpl.s	.MTest
	neg.w	d0
	move.w	d0,d2
	move.l	Ed_RunMessages(a5),a0
	addq.w	#1,d0
.MTest	bsr	Ed_GetMessageA0
	move.l	d2,d0
.Clair	rts

; Ligne EDITOR <-> DIRECT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Ligne
	movem.l	a0/d0,-(sp)
	bsr	Ed_GetError
	movem.l	a0/d0,-(sp)
; Ouvre l'editeur
	bsr	Ed_OpenEditor
; Fabrique les chaines de caractere
	move.l	Ed_VDialogues(a5),a2
	move.l	4(sp),a0		Message d'erreur
	bsr	Ed_DChaine
	move.l	a1,(a2)

	move.l	VerPos(a5),a0
	bsr	Ed_FindA
	addq.l	#1,d0
	move.l	d0,1*4(a2)		Numero de la ligne

	move.l	Buffer(a5),a1
	move.l	VerPos(a5),d0
	bsr	Detok
	move.w	d0,d3

	move.l	Buffer(a5),a3
	move.w	(a3)+,d2
	clr.b	0(a3,d2.w)
	moveq	#60,d4
	add.w	d3,d4
	clr.b	0(a3,d4.w)
	sub.w	#73,d4
	bpl.s	.S1
	moveq	#0,d4
.S1	lea	0(a3,d3.w),a0
	bsr	Ed_DChaine
	move.l	a1,3*4(a2)
	clr.b	0(a3,d3.w)
	lea	0(a3,d4.w),a0
	bsr	Ed_DChaine
	move.l	a1,2*4(a2)
	lea	EdReCop(pc),a0
	move.l	a0,4*4(a2)

; Fait apparaitre l'ecran
	moveq	#0,d2
	EcCalD	EHide,EcEdit
	EcCalD	First,EcEdit
	move.w	Ed_Wx(a5),d2
	move.w	Es_Y1(a5),d3
	move.l	#EntNul,d4
	moveq	#56,d5
	EcCalD	AView,EcEdit
	moveq	#0,d2
	moveq	#0,d3
	EcCalD	OffSet,EcEdit
; Fait apparaitre la souris
	SyCalD	Show,-1
	bsr	Esc_MaxMouse
; Positionne la souris sur l'ecran
	move.w	Ed_Wx(a5),d1
	move.w	Es_Y1(a5),d2
	addq.w	#1,d1
	addq.w	#1,d2
	SyCall	SetM
; Va demarrer le dialogue
	moveq	#EdD_Ligne,d0
	bsr	Ed_DoDialog
	move.w	d0,-(sp)
; Efface l'ecran
	moveq	#-1,d2
	EcCalD	EHide,EcEdit
	bsr	EdReCop
	move.w	(sp)+,d1
; Retourne a l'appelant
	addq.l	#8,sp
	movem.l	(sp)+,a0/d0
	rts

;
; TEST
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
Ed_Test	bsr	Ed_TokCur
	moveq	#-1,d0
	bsr	Ed_VaTester
; Message de warning si mix de constantes float
	move.w	#197,d0			***-No Errors
	move.b	Ver_SPConst(a5),d1
	tst.b	MathFlags(a5)
	bmi.s	.Test
	move.b	Ver_DPConst(a5),d1
.Test	tst.b	d1
	beq	Ed_Al100
	move.w	#221,d0			***-Precision mismatch
	bra	Ed_Al100

;
; CHECK 1.3
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Check1.3
	bsr	Ed_TokCur
	move.b	#1,VerCheck1.3(a5)	Force l'arret
	move.b	#1,Prg_StModif(a6)	Force le test
	bsr	Ed_VaTester
	moveq	#49,d0			Compatible
	tst.b	VerNot1.3(a5)
	beq.s	.Go
	moveq	#48,d0			Too many banks
.Go	move.l	Ed_TstMessages(a5),a0
	bsr	Ed_GetMessageA0
	moveq	#127,d0
	bra	Ed_Alert

;
; INDENTATION AUTOMATIQUE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Indent
	bsr	Ed_TokCur
	bsr	Ed_VaTester
	move.w	Ed_Tabs(a5),d0
	move.l	Prg_StBas(a6),a0
	bsr	Indent
	bsr	Ed_NewBuf
	rts

; Indentation du programme (a0 the program, d0 the tab): src/editor/indent.cpp
Indent	jmp	_ed_indent_entry

;
; Teste le programme s'il faut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D0	Flag affichage ou non
;
		xdef	_ed_va_tester
_ed_va_tester
Ed_VaTester
	tst.b	Prg_StModif(a6)		Vraiment tester?
	beq.s	.Ok
	move.b	#1,Ed_YaUTest(a5)	Un test!
	lea	Ed_ErrTest(pc),a1	En cas d'erreur
	lea	Ed_TestMessage(pc),a2	Affichage
	JJsr	L_Prg_TestIt		Va tester
	bne	Ed_AlRunned		Seule erreur possible
.Ok	rts


Ed_TestMessage
	bra	Ed_Test1
	bra	Ed_Test2
	bra	Ed_Test3
; Affiche le test si programme > 8K
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Test1
	movem.l	a0-a6/d0-d7,-(sp)
	clr.b	Ed_TstMesOn(a5)		Plus de flag
	move.l	Prg_StHaut(a6),d0
	sub.l	Prg_StBas(a6),d0
	cmp.l	#1024*4,d0
	bcs.s	.Non
	addq.b	#1,Ed_TstMesOn(a5)
	move.w	#198,d0
	bsr	Ed_GetMessage
	bsr	Ed_Avertir
	JJsr	L_Prg_SetBanks
.Non	movem.l	(sp)+,a0-a6/d0-d7
Ed_Rts	rts
; Effacement du message
; ~~~~~~~~~~~~~~~~~~~~~
Ed_Test2
	movem.l	a0-a6/d0-d7,-(sp)
	tst.b	Ed_TstMesOn(a5)
	beq.s	.Non
	clr.b	Ed_TstMesOn(a5)
	bsr	Ed_AverFin
	JJsr	L_Prg_SetBanks
.Non	movem.l	(sp)+,a0-a6/d0-d7
	rts
; Effacement de l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~
Ed_Test3
	bsr	EdM_End
	bra	Ed_Hide


; ___________________________________________________________________
;
;					PROCEDURES
; ___________________________________________________________________

;
; Open / close all procedures: src/editor/procs.cpp
Ed_ProcsOpen	jmp	_ed_procs_open_entry
Ed_ProcsClose	jmp	_ed_procs_close_entry
Ed_Procs	jmp	_ed_procs_entry

;
; INSERT MACHINE LANGAGE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ProcML
	bsr	Ed_TokCur
; Selecteur de fichier
; ~~~~~~~~~~~~~~~~~~~~
	move.w	#178,d0
	bsr	Ed_File_Selector
	beq	Ed_NotDone
	JJsr	L_Dsk.PathIt
; Trouve la ligne
; ~~~~~~~~~~~~~~~
.Reloop	move.w	Edt_YPos(a4),d0
	add.w	Edt_YCu(a4),d0
	bsr	Ed_FindL
	move.l	Edt_DebProc(a4),d0
	beq	Ed_FoE
	move.l	d0,a2
; Va fermer la procedure, s'il faut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	btst	#7,10(a2)
	bne.s	.PaOu
	bsr	Ed_ProcOpen
	bra.s	.Reloop
.PaOu
; Trouve la longueur du fichier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#1005,d2
	bsr	Ed_Open
	beq	Ed_DError
; Saute les premiers HUNKS!
	bsr	.GetH
	cmp.l	#$3F3,d0
	bne	.NoGood
.Plo0	bsr	.GetH
	cmp.l	#$3E9,d0
	bne.s	.Plo0
	bsr	.GetH
	lsl.l	#2,d0
	move.l	d0,d3
	add.l	#512,d0
	JJsr	L_ResTempBuffer
	move.l	a0,a3
	move.l	a0,a1
; Cree le faux programme
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	a2,a0
	moveq	#0,d0				Copie PROCEDURE
	move.b	(a0),d0
	lsl.w	#1,d0
	subq.w	#1,d0
.Cp1	move.b	(a0)+,(a1)+
	dbra	d0,.Cp1
	or.w	#%0101000000000000,10(a3)	Procedure machine langage
	move.w	#$0301,(a1)+
	move.w	#_TkML,(a1)+
	lea	10+6(a3),a0			Pointe les parametres
	moveq	#0,d0
	move.b	(a0),d0
	lea	2+2(a0,d0.w),a0
	move.l	a0,d0
	sub.l	a1,d0
	cmp.w	#_TkBra1,-2(a0)
	beq.s	.Par
	moveq	#0,d0
.Par	move.w	d0,(a1)+
; Charge le langage machine dans le buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a1,d2
	bsr	Ed_Read
	bne	Ed_DError
	bsr	Ed_Close
; Met le ENDPROC
; ~~~~~~~~~~~~~~
	move.l	d2,a1
	add.l	d3,a1
	move.w	#$0301,(a1)+
	move.w	#_TkEndP,(a1)+
	clr.w	(a1)+
	move.l	a1,d0
	sub.l	a3,d0
	sub.l	#14,d0
	move.l	d0,4(a3)
	move.l	a1,d3
	sub.l	a3,d3				D3= Longueur totale
; Efface l'ancienne proc�dure
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	Edt_YCu(a4),d0
	add.w	Edt_YPos(a4),d0
	bsr	Ed_FindL
	move.l	a0,-(sp)
	bsr	Ed_NextL
	move.l	a0,a1
	move.l	(sp)+,a0
	bsr	Ed_StDelChunk
; Insere la nouvelle procedure
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a3,a0
	move.l	d3,d0
	move.w	Edt_YCu(a4),d1
	add.w	Edt_YPos(a4),d1
	bsr	Ed_StoBlock
; Voili Voila
; ~~~~~~~~~~~
	moveq	#0,d0
	JJsr	L_ResTempBuffer
	JJsr	L_Prg_CptLines
	bsr	Ed_AverFin
	rts
; Routinette
; ~~~~~~~~~~
.GetH	move.l	Buffer(a5),d2
	moveq	#4,d3
	bsr	Ed_Read
	bne	Ed_DError
	move.l	Buffer(a5),a0
	move.l	(a0),d0
	rts
; Pas un bon programme!
; ~~~~~~~~~~~~~~~~~~~~~
.NoGood	move.w	#182,d0
	bsr	Ed_GetMessage
	move.w	#250,d0
	bra	Ed_Alert

;
; Open / close the procedure, no more centring on an error: src/editor/procs.cpp
Ed_ProcOpen	jmp	_ed_proc_open_entry
Ed_ClEProc	jmp	_ed_clear_error_proc_entry

;_____________________________________________________________________________
;
;							Mode Escape
;_____________________________________________________________________________
;

Ed_Escape
	moveq	#"E",d0			Va faire du bruit!
	bsr	Ed_SamPlay
	bsr	Ed_TokCur
	bsr	Ed_Hide
	bsr	Esc_Appear

; _________________________________________
;
; Boucle du mode escape
; _________________________________________
;
Esc_Loop
	move.l	BasSp(a5),sp
	move.w	#1,T_AMOState(a5)	Editor present!
	lea	Ed_ErrDirect(pc),a0	Jump si error
	move.l	a0,Prg_JError(a5)
;	move.l	Edt_Current(a5),a4	Change les banques >>> programme!
;	move.l	Edt_Prg(a4),a6
;	JJsr	L_Prg_SetBanks

; Affiche les sliders de m�moire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EcCalD	Active,EcFonc
	bsr	Ed_MemoryAff

	EcCalD	Active,EcEdit

; Met le curseur
	moveq	#14,d0
	bsr	Ed_GetSysteme
	move.l	a0,a1
	WiCall	Print
; Initialise l'editeur ligne
	lea	Es_LEd(a5),a0
	move.l	Buffer(a5),a1
	move.l	a1,a2
	clr.b	(a1)
	move.w	#(1<<LEd_FMulti)|(1<<LEd_FTests)|(1<<LEd_FKeys)|(1<<LEd_FMouse)|(1<<LEd_FCursor)|(1<<LEd_FMouCur),d0
	moveq	#0,d1			Curseur=0
	move.w	#160,d2			Longueur
	moveq	#-1,d3			Largeur maximum
	moveq	#"*",d4			Curseur
	JJsrP	L_LEd_Init,a3
	bra	Esc_Aff

Esc_L1	JJsr	L_Sys_WaitMul	Multitache + Inhibition
	lea	Es_LEd(a5),a0
	JJsrR	L_LEd_Loop,a1
	bmi.s	Esc_L1		Control-C
	cmp.w	#1,d2		Return?
	beq	Esc_R
	cmp.w	#2,d2		La souris?
	beq	Esc_MKey
	swap	d1
	move.w	d1,d0		Isole les SHIFTS
	lsr.w	#8,d0
	cmp.b	#$45,d1		ESC?
	beq	Esc_Esc
	cmp.b	#$4C,d1		Fleche vers le HAUT
	beq	Esc_H
	cmp.b	#$4D,d1		Fleche vers le BAS
	beq	Esc_B
	cmp.b	#$5F,d1		HELP
	beq	Esc_Help
	cmp.b	#$50,d1		Touches de fonction
	bcs	Esc_F
	cmp.b	#$59,d1
	bls	Esc_Fonc
Esc_F	EcCalD	Active,EcEdit
	bra.s	Esc_L1
Esc_Aff	bsr	Es_Display
	bset	#4,T_Actualise(a5)
	bset	#4,ActuMask(a5)
	SyCall	WaitVbl
	bra.s	Esc_F
;
; Bouton escape
; ~~~~~~~~~~~~~
Esc_Bouton
	move.l	d0,a0
	move.w	d1,d0
	move.w	Bt_Number(a0),d1
	subq.w	#1,d1				0 > retour editeur
	beq	Esc_Esc
	subq.w	#1,d1				1 > Workbench
	bne.s	.Pa2
	bsr	Ed_Wb
	bra	Esc_F
.Pa2	subq.w	#1,d1				2 > Out
	bne.s	.Pa3
	move.b	Bt_Pos+1(a0),Esc_Output(a5)
	bra	Esc_F
.Pa3	subq.w	#1,d1				3 > Fonctions
	btst	#1,d0				  > Bouton droit, fonction 10-20
	beq	Esc_BtFonc
	add.w	#10,d1
	bra	Esc_BtFonc
;
; Click souris
; ~~~~~~~~~~~~
Esc_MKey
	SyCall	GetZone
	cmp.w	#EcFonc,d1
	beq.s	Esc_MBoutons
	cmp.w	#EcEdit,d1
	beq.s	Esc_MEcran
	bra	Esc_F
; Dans l'�cran du haut
; ~~~~~~~~~~~~~~~~~~~~
Esc_MBoutons
	lea	Ed_Boutons(a5),a0
	JJsrR	L_Bt_Gere,a1
	bne	Esc_Bouton
	bra.s	Esc_MMove
; Dans l'�cran
; ~~~~~~~~~~~~
Esc_MEcran
	swap	d1
	cmp.w	#1,d1
	beq.s	Esc_MMove
; Change la taille
	SyCall	XyMou
	sub.w	Es_Y2(a5),d2
	move.w	d2,-(sp)
.SLoop	SyCall	XyMou
	sub.w	(sp),d2
	move.w	Es_Y1(a5),d0
	move.w	d0,d1
	add.w	#Es_MiniSy,d0
	add.w	Ed_Sy(a5),d1
	cmp.w	d0,d2
	bge.s	.SOk1
	move.w	d0,d2
.SOk1	cmp.w	d1,d2
	ble.s	.SOk2
	move.w	d1,d2
.SOk2	cmp.w	Es_Y2(a5),d2
	beq.s	.SSkip
	move.w	d2,Es_Y2(a5)
	bsr	Es_Display
	bsr	EdReCop
.SSkip	SyCall	MouseKey
	btst	#0,d1
	bne.s	.SLoop
	tst.w	(sp)+
	bra	Esc_F
; Change la position
Esc_MMove
	SyCall	XyMou
	sub.w	Es_Y1(a5),d2
	move.w	d2,-(sp)
.MLoop	SyCall	XyMou
	sub.w	(sp),d2
	cmp.w	Es_Y1(a5),d2
	beq.s	.MSkip
	move.w	Es_Y2(a5),d0
	sub.w	Es_Y1(a5),d0
	move.w	d2,Es_Y1(a5)
	add.w	d0,d2
	move.w	d2,Es_Y2(a5)
	bsr	Es_Display
	bsr	EdReCop
.MSkip	SyCall	MouseKey
	btst	#0,d1
	bne.s	.MLoop
	tst.w	(sp)+
	bra	Esc_F
;
; Fleche vers le HAUT
; ~~~~~~~~~~~~~~~~~~~
Esc_H	move.b	d0,d1
	and.b	#Ctr,d1
	bne.s	Esc_HH
	and.b	#Shf,d0
	bne.s	Esc_BH
; Recupere une touche AVANT
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Esc_KMem(a5),a0
	move.l	Esc_KMemPos(a5),d0
	beq.s	.Sk1
	move.l	d0,a0
.Sk1	move.l	(a0),d0
	beq	Esc_F
	move.l	d0,a0
	bra	Esc_Recall
; Monte toute la fenetre
; ~~~~~~~~~~~~~~~~~~~~~~
Esc_HH	tst.w	Es_Y2(a5)
	bmi	Esc_F
	subq.w	#8,Es_Y1(a5)
	subq.w	#8,Es_Y2(a5)
	bra	Esc_Aff
; Monte la ligne du bas
; ~~~~~~~~~~~~~~~~~~~~~
Esc_BH	move.w	Es_Y1(a5),d0
	add.w	#Es_MiniSy,d0
	subq.w	#8,Es_Y2(a5)
	cmp.w	Es_Y2(a5),d0
	ble	Esc_Aff
	move.w	d0,Es_Y2(a5)
	bra	Esc_Aff
;
; Fleche vers le BAS
; ~~~~~~~~~~~~~~~~~~
Esc_B	move.b	d0,d1
	and.b	#Ctr,d1
	bne.s	Esc_HB
	and.b	#Shf,d0
	bne.s	Esc_BB
; Recupere une touche APRES
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Esc_KMemPos(a5),d0
	beq	Esc_F
	move.l	Esc_KMem(a5),d1
	beq	Esc_F
	cmp.l	d0,d1
	beq.s	.Spec
.Loop	move.l	d1,a0
	move.l	(a0),d1
	beq	Esc_F
	cmp.l	d1,d0
	bne.s	.Loop
	bra	Esc_Recall
.Spec	clr.l	Esc_KMemPos(a5)
	move.l	Buffer(a5),a1
	clr.w	(a1)
	bra	Esc_Recall2

; Descend toute la fenetre
; ~~~~~~~~~~~~~~~~~~~~~~~~
Esc_HB	move.w	Es_Y1(a5),d0
	cmp.w	#312,d0
	bge	Esc_F
	addq.w	#8,Es_Y1(a5)
	addq.w	#8,Es_Y2(a5)
	bra	Esc_Aff
; Descend la ligne du bas
; ~~~~~~~~~~~~~~~~~~~~~~~
Esc_BB	move.w	Es_Y1(a5),d0
	add.w	Ed_Sy(a5),d0
	addq.w	#8,Es_Y2(a5)
	cmp.w	Es_Y2(a5),d0
	bge	Esc_Aff
	move.w	d0,Es_Y2(a5)
	bra	Esc_Aff
;
; ESCAPE ---> Retour a l'editeur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Esc
	moveq	#"E",d0			Va faire du bruit!
	bsr	Ed_SamPlay
	bsr	Esc_Hide
	clr.w	Edt_EtMess(a4)
	bsr	Ed_Appear
	bra	Ed_Loop
;
; HELP ---> recentre la souris dans l'ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Help
	SyCalD	Show,-1
	bsr	Esc_MaxMouse
	move.w	Ed_Wx(a5),d1
	move.w	Es_Y1(a5),d2
	add.w	#16,d2
	SyCall	SetM
	bra	Esc_Aff
;
; Touches de fonction 1-10
; ~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Fonc
	and.b	#Shf,d0
	beq.s	.NoShf
	add.w	#10,d1
.NoShf	and.w	#$00FF,d1
	sub.w	#$0050,d1
Esc_BtFonc
	moveq	#24,d0
	add.w	d1,d0
	bsr	Ed_GetSysteme
	move.l	Buffer(a5),a1
.Test	move.b	(a0)+,d2
	move.b	d2,(a1)+
	beq.s	.NoRet
	cmp.b	#"`",d2
	bne.s	.Test
	clr.b	-(a1)
.NoRet	lea	Es_LEd(a5),a0
	move.l	Buffer(a5),a1
	moveq	#-1,d0
	JJsrP	L_LEd_New,a2
	tst.b	d2
	beq	Esc_F
	bra	Esc_R
;
; Rappel d'une sequence memorisee
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Recall
	move.l	a0,Esc_KMemPos(a5)
	lea	6(a0),a1
Esc_Recall2
	lea	Es_LEd(a5),a0
	moveq	#-1,d0
	JJsrP	L_LEd_New,a2
	bra	Esc_F
;
; Return en mode ESCAPE
; ~~~~~~~~~~~~~~~~~~~~~
Esc_R	move.l	Buffer(a5),a2
	tst.b	(a2)
	beq	Esc_Loop
	move.w	Esc_KMemMax(a5),d0
	bsr	Esc_KMemEffD0
	move.l	a2,a0
.Count	tst.b	(a0)+
	bne.s	.Count
	sub.l	a2,a0
	move.l	a0,d2
	addq.l	#6,d2
; Met dans la liste
	lea	Esc_KMem(a5),a0
	move.l	d2,d0
	SyCall	MemFastClear
	beq	.Egal
	move.l	Esc_KMem(a5),(a0)
	move.l 	a0,Esc_KMem(a5)
	clr.l	Esc_KMemPos(a5)
	addq.l	#4,a0
	move.w	d2,(a0)+
.Copy	move.b	(a2)+,(a0)+
	bne.s	.Copy
; Retour chariot
.Egal	lea	CRet(pc),a1
	WiCall	Print
	move.w	Ed_Sx(a5),d0
	lsr.w	#3,d0
	sub.w	#9,d0
	cmp.w	d0,d2
	bls.s	.SkipR
	lea	CRet(pc),a1
	WiCall	Print
.SkipR
; Tokenise la ligne
; ~~~~~~~~~~~~~~~~~
	move.l	Buffer(a5),a0
	move.l	Ed_BufT(a5),a1
	bsr	Tokenise
	beq	Esc_Loop
; Appelle l'interpreteur
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	Edt_Current(a5),a4		Change les banques
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_SetBanks
	clr.l	Edt_Runned(a5)			Securites!
	clr.l	Prg_Runned(a5)
	lea	Ed_ErrDirect(pc),a0		Jump si error
	move.l	a0,Prg_JError(a5)
	JJsr	L_VerDirect
;	move.w	VarLong(a5),d0			Verification longueur vars.
;	cmp.w	TVMax(a5),d0
;	bcc	VerVTo
	move.w	ScOn(a5),d1			Branche l'ecran
	beq.s	.Pa
	subq.w	#1,d1
	EcCall	Active
.Pa	JJmp	L_New_ChrGet			Branche au chrget

;
; Efface le dernier si >D0
; ~~~~~~~~~~~~~~~~~~~~~~~~
Esc_KMemEffD0
	movem.l	d2/d3,-(sp)
	moveq	#0,d2
	sub.l	a1,a1
	move.l	Esc_KMem(a5),d1
	beq.s	.Out
.Loop	move.l	a1,d3
	move.l	d1,a1
	addq.w	#1,d2
	move.l	(a1),d1
	bne.s	.Loop
.Exit	cmp.w	d0,d2
	bcs.s	.Out
	tst.l	d3
	beq.s	.Skip
	move.l	d3,a0
	clr.l	(a0)
	bra.s	.Eff
.Skip	clr.l	Esc_KMem(a5)
.Eff	move.l	a0,-(sp)
	moveq	#0,d0
	move.w	4(a1),d0
	SyCall	MemFree
	move.l	(sp)+,a0
.Out	movem.l	(sp)+,d2/d3
	rts
;
; Efface tous les KMEM
; ~~~~~~~~~~~~~~~~~~~~
Esc_KMemEnd
	bra.s	.In
.Loop	moveq	#0,d0
	bsr	Esc_KMemEffD0
.In	tst.l	Esc_KMem(a5)
	bne.s	.Loop
	rts
;
; Retourne la position du bouton OUTPUT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_BtGetOutput
	move.b	Esc_Output(a5),Bt_Pos+1(a2)
	rts

;
;
; Entree des erreurs en mode direct
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_ErrDirect
	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	bsr	Ed_GetError			Va chercher le message
	movem.l	a0/d0,-(sp)
	bsr	Ed_OpenEditor
	clr.l	Edt_Runned(a5)			Plus de programme!
	or.b	#%00010001,ActuMask(a5)
	EcCalD	CopOnOff,-1
	EcCalD	First,EcEdit
	EcCalD	First,EcFonc
	EcCalD	Active,EcEdit
	bsr	EdReCop
	bsr	Ed_Hide
	bsr	Esc_Appear
	movem.l	(sp)+,a1/d1
	cmp.w	#1002,d1
	beq	Ed_System
	cmp.w	#1000,d1
	beq	Esc_Esc
	cmp.w	#1000,d1
	bcc	Esc_Loop
	cmp.w	#10,d1
	beq	Esc_Loop
	WiCall	Print
	bra	Esc_Loop


; ___________________________________________________________________
;
; 	ECRAN ESCAPE
; ___________________________________________________________________
;

; Apparition de l'ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Appear
	movem.l	a2-a4/a6/d2-d7,-(sp)
; Flag!
; ~~~~~
	tst.w	Direct(a5)
	bne	.Out
	move.w	#1,Direct(a5)
	move.w	Ed_Sx(a5),d7

; Active l'�cran de titre
; ~~~~~~~~~~~~~~~~~~~~~~~
	EcCalD	Active,EcFonc
	EcCalD	First,EcEdit
; Unpack le logo AMOS
; ~~~~~~~~~~~~~~~~~~~
	moveq	#Es_Pics,d0
	moveq	#Es_BoutonsSx,d1
	moveq	#Es_BoutonsY,d2
	bsr	Es_Unpack
	sub.w	#Es_TitleSx,d7
;
; Cree les 13 boutons
; ~~~~~~~~~~~~~~~~~~~
	lea	Ed_Boutons(a5),a2
	move.w	#Es_BoutonsX,d2
	moveq	#Es_BoutonsY,d3
	moveq	#Es_BoutonsPics,d4
	move.w	#Es_BoutonsZones,d5
	moveq	#1,d6
.Loop	move.w	d6,Bt_Number(a2)
	cmp.w	#1,d6
	bne.s	.Pa1
	clr.w	Bt_X(a2)			Bouton DIRECT
	bra.s	.PaB
.Pa1	cmp.w	#2,d6
	bne.s	.Pa2
	move.w	Ed_Sx(a5),Bt_X(a2)		Bouton WB
	sub.w	#Ed_BoutonsSx,Bt_X(a2)
	bra.s	.PaB
.Pa2	move.w	d2,Bt_X(a2)
	add.w	#Ed_BoutonsSx,d2
.PaB	move.w	d3,Bt_Y(a2)
	move.w	d4,Bt_Image(a2)
	move.w	d5,Bt_Zone(a2)
	lea	Bt_RoutIn(pc),a0
	move.l	a0,Bt_Routines(a2)
	move.b	#4,Bt_RDraw(a2)
	clr.b	Bt_RPos(a2)
	clr.b	Bt_RChange(a2)
	clr.b	Bt_Flags(a2)
	clr.w	Bt_Pos(a2)
	move.b	#Es_BoutonsSx,Bt_Sx(a2)
	move.b	#Es_BoutonsSy,Bt_Sy(a2)
	addq.w	#2,d4
	addq.w	#1,d5
	sub.w	#Es_BoutonsSx,d7
	lea	Bt_Long(a2),a2
	addq.w	#1,d6
	cmp.w	#13,d6
	bls.s	.Loop
	clr.w	(a2)

; Dessine les sliders de memoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	d2,d1
	moveq	#Es_BoutonsY,d2
	move.w	d7,d3
	lea	Es_Unpack(pc),a2
	bsr	Ed_MemoryDraw
	bsr	Ed_MemoryAff

; Position  du bouton "OUT"
; ~~~~~~~~~~~~~~~~~~~~~~~~
	move.b	#5,Ed_Boutons+Bt_Long*2+Bt_RPos(a5)
	bset	#Bt_FlagOnOf,Ed_Boutons+Bt_Long*2+Bt_Flags(a5)
; Init des boutons
; ~~~~~~~~~~~~~~~~
	lea	Ed_Boutons(a5),a0
	moveq	#1,d0
	JJsrR	L_Bt_InitList,a1
; Reserve la zone de fond
; ~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d2
	moveq	#0,d3
	move.w	Ed_Sx(a5),d4
	moveq	#Es_TitleSy,d5
	SyCalD	SetZone,14

; Active l'�cran de texte
; ~~~~~~~~~~~~~~~~~~~~~~~
	EcCalD	Active,EcEdit
; Dessine le bord droit
; ~~~~~~~~~~~~~~~~~~~~~
	move.w	Ed_Ty(a5),d6
	addq.w	#1,d6
	moveq	#0,d7
.Loopa	moveq	#Es_Pics+1,d0
	move.w	Ed_Sx(a5),d1
	sub.w	#16,d1
	ext.l	d1
	move.w	d7,d2
	moveq	#-1,d3
	bsr	Ed_Unpack
	addq.w	#8,d7
	subq.w	#1,d6
	bne.s	.Loopa
; Dessine le bas
; ~~~~~~~~~~~~~~
	moveq	#Es_Pics+2,d0
	moveq	#0,d1
	move.w	d7,d2
	moveq	#-1,d3
	bsr	Ed_Unpack
	move.w	d7,d0
	addq.w	#8,d7
	move.w	d7,d1
	bsr	Ed_Enlarge
; Ouvre la fenetre
; ~~~~~~~~~~~~~~~~
	moveq	#1,d1
	moveq	#0,d2
	moveq	#0,d3
	move.w	Ed_Sx(a5),d4
	lsr.w	#3,d4
	subq.w	#2,d4
	move.w	Ed_Ty(a5),d5
	addq.w	#1,d5
	moveq	#0,d6
	moveq	#0,d7
	sub.l	a1,a1
	WiCall	WindOp
	moveq	#16,d0
	bsr	Ed_GetSysteme
	move.l	a0,a1
	WiCall	Print
; Defini les zones
; ~~~~~~~~~~~~~~~~
	SyCalD	RazZone,0
	moveq	#0,d2
	moveq	#0,d3
	move.w	Ed_Sx(a5),d4
	move.w	Ed_Sy(a5),d5
	SyCalD	SetZone,2
	sub.w	#16,d4
	subq.w	#8,d5
	SyCalD	SetZone,1
; Descend l'�cran du bas
; ~~~~~~~~~~~~~~~~~~~~~~
	move.w	Es_Y1(a5),d0
	move.w	d0,d1
	add.w	#Es_MiniSy,d1
.Loop1	move.w	d1,d2
	add.w	Ed_VScrol(a5),d1
	cmp.w	Es_Y2(a5),d1
	ble.s	.Skip1
	move.w	Es_Y2(a5),d1
.Skip1	movem.w	d0-d2,-(sp)
	bsr	Es_DisplayD0
	bsr	EdReCop
	movem.w	(sp)+,d0-d2
	cmp.w	d1,d2
	bne.s	.Loop1
	bsr	Esc_MaxMouse
.Out	movem.l	(sp)+,a2-a4/a6/d2-d7
	rts
; Elargi les limites de la souris
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_MaxMouse
	moveq	#128-16,d1
	moveq	#35,d2
	move.l	#500,d3
	move.l	#312,d4
	SyCall	LimitM
	rts

;
; Disparition de l'�cran de mode direct
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Hide
	movem.l	a0-a4/a6/d0-d7,-(sp)
	tst.w	Direct(a5)
	beq	.Out
	clr.w	Direct(a5)
; Remonte l'�cran du bas
; ~~~~~~~~~~~~~~~~~~~~~~
	move.w	Es_Y1(a5),d0
	move.w	Es_Y2(a5),d1
.Loop1	move.w	d1,d2
	sub.w	Ed_VScrol(a5),d1
	move.w	d0,d3
	add.w	#Es_MiniSy,d3
	cmp.w	d3,d1
	bge.s	.Skip1
	move.w	d3,d1
.Skip1	movem.w	d0-d2,-(sp)
	bsr	Es_DisplayD0
	bsr	EdReCop
	movem.w	(sp)+,d0-d2
	cmp.w	d1,d2
	bne.s	.Loop1
	move.l	T_EcAdr+EcEdit*4(a5),a0
	bset	#BitHide,EcFlags(a0)
; Efface l'�cran du haut
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	T_EcAdr+EcFonc*4(a5),a0
	bset	#BitHide,EcFlags(a0)
	bsr	EdReCop
; Efface la fenetre de fond
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	EcCalD	Active,EcEdit
	WiCall	WinDel
; Efface les zones
; ~~~~~~~~~~~~~~~~
	SyCalD	RazZone,0
.Out 	movem.l	(sp)+,a0-a4/a6/d0-d7
	rts

;
; Positionne les ecrans du mode escape
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Es_Display
	move.w	Es_Y1(a5),d0
	move.w	Es_Y2(a5),d1
Es_DisplayD0
	move.l	T_EcAdr+EcFonc*4(a5),a0
	move.l	T_EcAdr+EcEdit*4(a5),a1
; Ecran du haut
	move.w	d0,EcAWY(a0)
	bset	#2,EcAW(a0)
	add.w	#Es_TitleSy,d0
; Ecran du bas
	move.w	d0,EcAWY(a1)		Position
	bset	#2,EcAW(a1)
	sub.w	d0,d1			Hauteur
	move.w	d1,EcAWTY(a1)
	bset	#2,EcAWT(a1)
	move.w	Ed_Sy(a5),d0		D�but de la fenetre
	sub.w	d1,d0
	move.w	d0,EcAVY(a1)
	bset	#2,EcAV(a1)
; Fait apparaitre les �crans
	bclr	#BitHide,EcFlags(a0)
	bclr	#BitHide,EcFlags(a1)
; Ecran de l'�diteur en premier
	EcCalD	First,EcFonc
	EcCalD	First,EcEdit
	rts
;
; Cache l'�cran de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Hide
	movem.l	a0-a4/a6/d0-d7,-(sp)
	tst.w	EsFlag(a5)
	bne	.Out
	move.w	#-1,EsFlag(a5)
; Plus d'avertissements
; ~~~~~~~~~~~~~~~~~~~~~
	bsr	Ed_AllAverFin
; Fait disparaitre lentement
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	T_EcAdr+EcEdit*4(a5),a2
	move.w	Ed_VScrol(a5),d7
	neg.w	d7
	move.w	EcTy(a2),d6
	lsr.w	#1,d6
	move.w	Ed_Wy(a5),d5
	add.w	d6,d5
	JJsr	L_AppCentre
	bset	#BitHide,EcFlags(a2)
	bsr	EdReCop
; Efface les d�finitions de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	EcCalD	Active,EcEdit
	bsr	Edt_EffWindows
; Met la couleur du fond
; ~~~~~~~~~~~~~~~~~~~~~~
	move.w	ColBack(a5),d1
	EcCall	SColB
; Remet les rainbows
; ~~~~~~~~~~~~~~~~~~
	EcCalD	RainHide,0
; Remet les limites souris
; ~~~~~~~~~~~~~~~~~~~~~~~~
	lea	LimSave(a5),a0
	lea	T_MouXMin(a5),a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
; Remet tous les sprites
; ~~~~~~~~~~~~~~~~~~~~~~
	SyCall	RecallM
	SyCall	ReActHs
	SyCall	AMALUFrz
	SyCall	WaitVbl
	SyCall	ActHs
	SyCall	AffHs
; Initialise les touches de fonction AMIGA illegal--> en verif!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	moveq	#0,d1
;	Alea	AmiDef,a1
;.Ami	SyCall	SetFunk
;	move.l	a0,a1
;	addq.w	#1,d1
;	cmp.w	#20,d1
;	bcs.s	.Ami
; Nettoie les touches / Demarre AMAL
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Out	SyCall	ClearKey
	SyCall	AMALUFrz
; Menus >>> Program
; ~~~~~~~~~~~~~~~~~
	bsr	EdM_Program
	movem.l	(sp)+,a0-a4/a6/d0-d7
	rts

;
; Apparition de l'�cran l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Appear
	movem.l	a2-a4/a6/d2-d7,-(sp)
	tst.w	EsFlag(a5)
	beq	.Out
	clr.w	EsFlag(a5)
; Enleve les touches de fonction AMIGA
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d1
.Ami	move.l	Buffer(a5),a1
	clr.b	(a1)
	SyCall	SetFunk
	addq.w	#1,d1
	cmp.w	#20,d1
	bcs.s	.Ami
; Enleve tous les sprites
; ~~~~~~~~~~~~~~~~~~~~~~~
	SyCall	AMALFrz
	SyCall	StActHs
	SyCall	StoreM
; Enleve les rainbows
; ~~~~~~~~~~~~~~~~~~~
	EcCalD	RainHide,-1
; Remet le fond!
; ~~~~~~~~~~~~~~
	move.w	Ed_ColB(a5),d1
	EcCall	SColB
; Dessine le fond
; ~~~~~~~~~~~~~~~
	EcCalD	Active,EcEdit
	bsr	Ed_DrawTop
	move.l	Edt_Current(a5),a4
	move.l	Edt_Prg(a4),a6
	bsr	Ed_DrawWindows
; Apparition de l'�cran
; ~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d2
	EcCalD	EHide,EcEdit
	EcCalD	First,EcEdit
	move.l	T_EcAdr+EcEdit*4(a5),a0
	move.w	Ed_Wy(a5),EcAWY(a0)
	bset	#2,EcAW(a0)
; Fait apparaitre lentement
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	T_EcAdr+EcEdit*4(a5),a2
	move.w	Ed_VScrol(a5),d7
	move.w	#1,d6
	move.w	Ed_Wy(a5),d5
	move.w	Ed_Sy(a5),d0
	lsr.w	#1,d0
	add.w	d0,d5
	JJsr	L_AppCentre
; Limite de la souris
; ~~~~~~~~~~~~~~~~~~~
	lea	T_MouXMin(a5),a0
	lea	LimSave(a5),a1
	move.l	(a0)+,(a1)+
	move.l	(a0)+,(a1)+
	bsr	Ed_LimM
; Affiche le tout!
; ~~~~~~~~~~~~~~~~
	bclr	#BitControl-8,T_Actualise(a5)
	bsr	EdM_Editor
; Flag si peu de memoire...
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.b	#2,Ed_NewAppear(a5)
; Plus de touches
; ~~~~~~~~~~~~~~~
.Out	SyCall	ClearKey
	movem.l	(sp)+,a2-a4/a6/d2-d7
	rts
; Redessine le fond, lors de retour d'accessoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Redraw
	movem.l	a2-a6/d2-d7,-(sp)
	EcCalD	Active,EcEdit
;	bsr	Ed_DrawTop			Voir si redessin!
;	move.l	Edt_Current(a5),a4
;	move.l	Edt_Prg(a4),a6
;	bsr	Ed_DrawWindows
	EcCalD	First,EcEdit
	bsr	EdReCop
	bsr	Ed_LimM
	movem.l	(sp)+,a2-a6/d2-d7
	rts
; Routine, limite la souris
Ed_LimM	moveq	#EcEdit+1,d3
	move.w	Ed_Sx(a5),d1
	subq.w	#4,d1
	move.w	Ed_Sy(a5),d2
	subq.w	#1,d2
	SyCall	XyHard
	movem.w	d1/d2,-(sp)
	moveq	#EcEdit+1,d3
	moveq	#2,d1
	moveq	#2,d2
	SyCall	XyHard
	movem.w	(sp)+,d3/d4
	SyCall	LimitM
	SyCalD	Show,-1
	rts

;_____________________________________________________________________________
;
;							Messages editeur
;_____________________________________________________________________________
;
; Line not editable
; ~~~~~~~~~~~~~~~~~
		xdef	_ed_not_edit
_ed_not_edit
Ed_NotEdit
	move.w	#183,d0
	bsr	Ed_GetMessage
	moveq	#100,d0
	bra	Ed_Alert
;
; Line too long
; ~~~~~~~~~~~~~
		xdef	_ed_l_to_long
_ed_l_to_long
Ed_LToLong
	move.w	#199,d0			***- Line too long
	bsr	Ed_GetMessage
	moveq	#50,d0
	bra	Ed_Alert
;
; Top of text
; ~~~~~~~~~~~
		xdef	_ed_top_of_text
_ed_top_of_text
Ed_CHtE
	move.w	#200,d0			***- Top of text
	bsr	Ed_GetMessage
	moveq	#25,d0
	bra	Ed_Alert
;
; Bottom of text
; ~~~~~~~~~~~~~~
		xdef	_ed_bottom_of_text
_ed_bottom_of_text
Ed_CBasE
	move.w	#201,d0			***- Bottom of text
	bsr	Ed_GetMessage
	moveq	#25,d0
	bra	Ed_Alert
;
; Mark not set
; ~~~~~~~~~~~~
		xdef	_ed_no_mark
_ed_no_mark
Ed_NoMark
	moveq	#65,d0
	bra	Ed_Al100
;
; Out of mem tokenisation
; ~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_oof_buf
_ed_oof_buf
Ed_OofBuf
	bsr	Ed_NewBuf		Reaffiche la ligne!
	move.w	#202,d0			***- Out of buffer space
	bsr	Ed_GetMessage
	move.w	#200,d0
	bra	Ed_Alert
;
; Can't fold procedure
; ~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_fo_e
_ed_fo_e
Ed_FoE	move.w	#203,d0			***- Not a procedure
		xdef	_ed_alert_100
_ed_alert_100
Ed_Al100
	bsr	Ed_GetMessage
	moveq	#100,d0
	bra	Ed_Alert

; Out of memory / alert
; ~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_out_of_memory
_ed_out_of_memory
Ed_OMm	move.w	#204,d0			***- Out of memory
	bsr	Ed_GetMessage
	moveq	#120,d0
	bra	Ed_Alert
;
; Not found
; ~~~~~~~~~
Ed_NoFound
	bsr	Ed_NewBuf
	move.w	#205,d0			***- Not found
	bsr	Ed_GetMessage
	moveq	#100,d0
	bra	Ed_Alert
;
; NOT DONE
; ~~~~~~~~
		xdef	_ed_not_done
_ed_not_done
Ed_NotDone
	bsr	Ed_NewBuf
Ed_NotDone2
	move.w	#206,d0			***- Not done
	bsr	Ed_GetMessage
	moveq	#100,d0
	bra	Ed_Alert

;
; Not an AMOS Program
; ~~~~~~~~~~~~~~~~~~~
Ed_PaAMOS
	bsr	Ed_Close
	move.w	#207,d0			***- Not an amos program
	bsr	Ed_GetMessage
	moveq	#100,d0
	bra	Ed_Alert
;
; Pas de place pour le programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_PaPlace
	bsr	Ed_Close
	move.w	#208,d0			***- Buffer too small
	bsr	Ed_GetMessage
	moveq	#100,d0
	bra	Ed_Alert

; What bloc?
; ~~~~~~~~~~
		xdef	_ed_block_what
_ed_block_what
Ed_BlocWhat
	moveq	#6,d0
	bra.s	Ed_ProAlert
; Too many windows
; ~~~~~~~~~~~~~~~~
		xdef	_ed_too_many_windows
_ed_too_many_windows
Ed_2ManyWindow
	moveq	#3,d0
	bsr	Ed_GetMessage
	bra.s	Ed_AlertDialog

; Hide window, last window
;~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_no_hide
_ed_no_hide
Ed_NoHide
	moveq	#2,d0
	bra	Ed_Al100

; Program already runned
;~~~~~~~~~~~~~~~~~~~~~~~
Ed_AlRunned
	moveq	#12,d0
	bra	Ed_Al100

; No more UNDO
; ~~~~~~~~~~~~
		xdef	_ed_no_undo,_ed_no_redo
_ed_no_undo
Ed_NoUndo
	moveq	#4,d0
	bra	Ed_Al100
; No more REDO
; ~~~~~~~~~~~~
_ed_no_redo
Ed_NoRedo
	moveq	#5,d0
	bra	Ed_Al100
;
; Appel alerte message indexe
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_pro_alert
_ed_pro_alert
Ed_ProAlert
	bsr	Ed_GetMessage
	moveq	#127,d0
	bra	Ed_Alert
; Branche un message en dialog box
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_AlertDialog
	moveq	#127,d0
	bra	Ed_Alert

;_____________________________________________________________________________
;
;						Routines diverses
;_____________________________________________________________________________
;


;____________________________________________________________________
;
;	LOAD / SAVE PROGRAMME
; ___________________________________________________________________
;

;
; LOADING: Demande plus de place / D0= Taille voulue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_get_place
_ed_get_place
Ed_GetPlace
	move.l	d0,d2
; Dialogues
	moveq	#EdD_TooSmall,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne.s	.GtPl2
; Change la taille
	move.l	Prg_StTTexte(a6),d3
	move.l	d2,d0
	JJsr	L_Prg_ChgTTexte
	beq.s	.GtPl1
	rts
; Out of mem, remet l'ancien buffer
.GtPl1	move.l	d3,d0
	JJsr	L_Prg_ChgTTexte
	bne	.GtPl0
; Vraiment out of mem: TOUT petit buffer!
	move.l	#1024,d0
	JJsr	L_Prg_ChgTTexte
; Totalement Out of Mem!
.GtPl0	bsr	Ed_NewBufAff
	bra	Ed_OMm
; Appelle SETBUFFER, et revient!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.GtPl2	clr.b	Prg_Change(a6)
	move.l	d2,d0
	bra.s	Ed_SB
;
; Option SET BUFFER SIZE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SetBuffer
	bsr	Ed_TokCur
	bsr	Edt_ClearVar
	move.l	Prg_StTTexte(a6),d0
Ed_SB
; Boite de dialogue
	move.l	Ed_VDialogues(a5),a2
	move.l	d0,2*4(a2)
	moveq	#EdD_SetBuf,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
	moveq	#1,d0
	moveq	#3,d1
	moveq	#-1,d2
	JJsr	L_Dia_GetValue
	move.l	d1,d0
	and.l	#$FFFFFFFE,d0
	cmp.l	#1024,d0
	bcs	Ed_NotDone
	move.l	d0,d2
	move.l	Prg_StTTexte(a6),d3
	cmp.l	d3,d2
	beq	Ed_NotDone
; Peut-on recopier le buffer?
	bcs.s	.PaCop
; Essaie de changer sans effacer le programme courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Prg_StTTexte(a6),-(sp)
	move.l	Prg_StMini(a6),-(sp)
	move.l	Prg_StHaut(a6),-(sp)
	move.l	Prg_StBas(a6),-(sp)
	clr.l	Prg_StTTexte(a6)	Plus rien
	move.l	d2,d0			Reserve la nouvelle
	JJsr	L_Prg_ChgTTexte
	beq.s	.PaCop
	move.l	(sp)+,a0		Recopie le programme dans le buffer
	move.l	(sp)+,d0
	sub.l	a0,d0
	moveq	#0,d1
	bsr	Ed_StoBlock
	move.l	(sp)+,a1		Libere le precedent
	move.l	(sp)+,d0
	SyCall	SyFree
	bra.s	.Out
; Change le buffer en effacant le programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PaCop	movem.l	d2/d3,-(sp)
	bsr	Ed_Saved
	bsr	Ed_New2
	movem.l	(sp)+,d2/d3
	move.l	d2,d0
	JJsr	L_Prg_ChgTTexte
	beq.s	.OMm
; Fini!
.Out	JJsr	L_Prg_SetBanks
	bsr	Ed_NewBufAff
	rts
; Out of mem!
; ~~~~~~~~~~~
.OMm	move.l	d3,d0
	JJsr	L_Prg_ChgTTexte
	bne.s	.OMmX
	move.l	#1024,d0
	JJsr	L_Prg_ChgTTexte
.OMmX	bsr	Ed_NewBufAff
	bra	Ed_OMm


; ___________________________________________________________________
;
;	ROUTINES AFFICHAGE / POSITIONNEMENT FENETRES
; ___________________________________________________________________
;
; The display of the window, the cursor and the window's position:
; src/editor/display.cpp
Ed_AffSlV	jmp	_ed_show_slider_entry
Ed_CuOn		jmp	_ed_cursor_on_entry
Ed_CuOff	jmp	_ed_cursor_off_entry
Ed_EALiCu	jmp	_ed_show_current_line_entry
Ed_Cent		jmp	_ed_centre_entry
Ed_GotoX	jmp	_ed_goto_x_entry
Ed_GotoY	jmp	_ed_goto_y_entry
Ed_SetX		jmp	_ed_set_x_entry
Ed_SetY		jmp	_ed_set_y_entry
Ed_SetXY	jmp	_ed_set_xy_entry
Ed_NLab		jmp	_ed_labels_around_entry
Ed_Loca		jmp	_ed_place_cursor_entry
Ed_AffBufCar	jmp	_ed_show_column_entry
Ed_NewBufAff	jmp	_ed_new_buffer_all_entry
Ed_NewBuf	jmp	_ed_new_buffer_entry
Ed_AffBuf	jmp	_ed_show_all_entry
Ed_EALigne	jmp	_ed_show_line_entry
Ed_ALigne	jmp	_ed_show_line_only_entry




;_____________________________________________________________________________
;
;						STOCKAGE DU PROGRAMME
;_____________________________________________________________________________
;

;
; Deleting, joining, splitting (Return), inserting lines, the edited line
; tokenised and stored: src/editor/edit.cpp
Ed_DelLiCu	jmp	_ed_del_li_cu_entry
Ed_Join		jmp	_ed_join_entry
Ed_Return	jmp	_ed_return_entry
Ed_ReturnQuiet	jmp	_ed_return_quiet_entry
Ed_InsLine	jmp	_ed_ins_line_entry
Ed_TokCur	jmp	_ed_tok_cur_entry
Ed_TokStok	jmp	_ed_tok_stok_entry
Ed_TokStok2	jmp	_ed_tok_stok2_entry
TokX		rts


; Ed_BufUntok, Ed_Untok (d0 the program's line, d1 the window's): src/editor/cursor.cpp
Ed_BufUntok	jmp	_ed_buf_untok_entry
Ed_Untok	jmp	_ed_untok_entry

;
; NEW
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_new
_ed_new
Ed_New
	bsr	Ed_TokCur
	bsr	Ed_Saved
		xdef	_ed_new2
_ed_new2
Ed_New2
	bsr	EdM_Program
	moveq	#1,d0
	tst.b	Ed_Zappeuse(a5)
	beq.s	.No
	moveq	#0,d0
.No	JJsr	L_Prg_New
	bsr	Edt_New
	bsr	Ed_NewBufAff
	rts
;
; New du contenu d'une fenetre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_new_window
_ed_new_window
Edt_New
; Editeur par devant
	tst.b	Ed_Zappeuse(a5)
	bne.s	.Skp
	EcCalD	Last,0
	bsr	EdReCop
.Skp	EcCalD	Active,EcEdit
; Init des donn�es editeur
	lea	Edt_SInit(a4),a0
	moveq	#(Edt_EInit-Edt_SInit)/2-1,d0
.Loop	clr.w	(a0)+
	dbra	d0,.Loop
	move.w	#-1,Edt_YBloc(a4)
	move.w	#-1,Edt_YOldBloc(a4)
; Init des donnees programme
	bsr	Prg_UndoCreate		Effacement / Creation du buffer undo
	bsr	Prg_MarkRaz
	clr.b	MathFlags(a5)		Simple precision par defaut
	rts

; Ed_StoBlock, Ed_Stocke, Ed_DelChunk, Ed_DeLigne, Ed_StDelChunk,
; Ed_FindL, Ed_NextL: src/editor/storage.cpp
Ed_StoBlock	jmp	_ed_store_block_entry
Ed_Stocke	jmp	_ed_store_entry
Ed_DelChunk	jmp	_ed_delete_chunk_at_entry
Ed_DeLigne	jmp	_ed_delete_line_entry
Ed_StDelChunk	jmp	_ed_delete_chunk_entry
Ed_FindL	jmp	_ed_find_line_entry
Ed_NextL	jmp	_ed_next_line_entry

; Trouve la taille de la CURLIGNE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_SizeL
	move.l	Edt_CurLigne(a4),a0
	JJsrP	L_Tk_SizeL,a1
	rts

; La ligne courante est-elle editable?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_EditL
	move.l	Edt_CurLigne(a4),a0
	JJsrP	L_Tk_EditL,a1
	rts

;
; TROUVE LE NUMERO ET LE DEBUT DE LA LIGNE A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; D0-> Numero
; D1-> Adresse debut proc
;
Ed_FindA
	movem.l	a1/a2,-(sp)
	move.l	Prg_StBas(a6),a1
	JJsrR	L_Tk_FindA,a2
	movem.l	(sp)+,a1/a2
	rts
;
; Ramene l'adresse de la ligne sous le curseur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; A0= adresse
; A1= lettre sous le curseur
; D0= longueur
; D1= position curseur
;
; Ed_LCourant: src/editor/lines.cpp
Ed_LCourant:	jmp	_ed_current_line_entry


;____________________________________________________________________
;
;						GESTION WINDOWS
;____________________________________________________________________
;
;
; Back to Workbench
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Wb	EcCalD	AMOS_WB,0
	rts


; ZAP: Ferme un fenetre contenant un programme donne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_CloseName
	bsr	Edt_AccAdr		Deja en memoire?
	beq.s	.Fini
	move.l	a0,a4			Active la fenetre
	move.l	Edt_Prg(a4),a6
	JJsr	L_Prg_DejaRunned	En train de tourner?
	bne.s	.Fini
	bsr	Ed_CloseWindow		Ferme la fenetre
	bra.s	Ed_CloseName		Encore une fenetre a fermer?
.Fini	rts

; Ouvre une nouvelle fenetre+Charge prg
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
Ed_OpenLoad
	bsr	Ed_OpenWindow
	bra	Ed_Load

;
; The windows: src/editor/windows.cpp
Ed_OpenWindow	jmp	_ed_open_shown_window_entry
Edt_OpWindow	jmp	_ed_open_window_entry
Ed_WindowHide	jmp	_ed_hide_window_entry
Ed_BtWindowClose	jmp	_ed_button_close_entry
Ed_BtWindowHide	jmp	_ed_button_hide_entry
Ed_BtWindowSize	jmp	_ed_button_size_entry
Ed_CloseWindowQuit	jmp	_ed_close_window_quit_entry
Ed_CloseWindow	jmp	_ed_close_window_entry
Ed_NewHidden	jmp	_ed_new_hidden_entry
Ed_NewAllHidden	jmp	_ed_new_all_hidden_entry
Ed_EditHidden	jmp	_ed_edit_hidden_entry
Ed_GetHidden	jmp	_ed_get_hidden_entry
Edt_DelWindow	jmp	_ed_delete_window_entry
Edt_DelWindows	jmp	_ed_delete_windows_entry
Ed_DrawWindows	jmp	_ed_draw_windows_entry
Edt_EffWindows	jmp	_ed_erase_windows_entry
Edt_DelAllZones	jmp	_ed_delete_all_zones_entry
Edt_WVideNext	jmp	_ed_empty_next_entry
Ed_NextWindow	jmp	_ed_next_window_entry
Ed_PrevWindow	jmp	_ed_previous_window_entry
Ed_FlipSizeWindow	jmp	_ed_flip_size_entry
Edt_GetAd	jmp	_ed_window_number_entry
Edt_WSchrinkAll	jmp	_ed_shrink_all_entry
Edt_WMaxSize	jmp	_ed_max_size_entry
		xdef	_ed_window_active
_ed_window_active
Edt_Active	jmp	_ed_activate_entry

; __________________________________________________________________________
;
; 	Creation du menu
; __________________________________________________________________________
;

; Effacement de tous les menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdM_End
	bsr	EdM_Editor
	JJsr	L_MnRaz
	move.l	EdM_Table(a5),d0
	beq.s	.Skip
	clr.l	EdM_Table(a5)
	move.l	d0,a1
	move.l	EdM_TableSize(a5),d0
	SyCall	MemFree
.Skip	rts

;
; Erreur, on ne peut pas ouvrir les menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_menu_error
_ed_menu_error
EdM_Error
	move.w	#185,d0			Out of memory: cannot open menu
	bsr	Ed_GetMessage
	move.w	#1,d0			Relache tout de suite!
	bsr	Ed_AlertRts
	JJsr	L_Dia_NoMKey		Plus de touche souris
	rts

; Initialisation generale des menus
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdM_Init
	movem.l	d2-d7/a2-a6,-(sp)

; Enleve les anciens menus
; ~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	EdM_End

; Assez de memoire?
; ~~~~~~~~~~~~~~~~~
	bsr	Ed_Available			40k minimum!
	cmp.l	#40*1024,d0
	bcs	.Error

; Reserve la table
; ~~~~~~~~~~~~~~~~
	move.l	#EdM_HiddenMax*8*5+16,d0	Menus hiddens
	add.l	#EdM_UserMax*8+8,d0
	move.l	EdM_Definition(a5),a0		Definitions normals
	add.l	-4(a0),d0
	add.l	#16,d0				Un chouuya de securite
	move.l	d0,EdM_TableSize(a5)
	SyCall	MemFastClear
	move.l	a0,EdM_Table(a5)
	beq	.Error

; Reset des definitions
; ~~~~~~~~~~~~~~~~~~~~~
	clr.w	MnMouse(a5)
	lea	MnDFlags(a5),a0
	moveq	#0,d0			* Barre de menu
*	bset	#MnBar,d0
	bset	#MnTotal,d0
*	bset	#MnBouge,d0
*	bset	#MnTBouge,d0
	move.b	d0,(a0)+
	moveq	#0,d0
	bset	#MnBar,d0
*	bset	#MnBouge,d0
*	bset	#MnTBouge,d0
	moveq	#MnNDim-1-1,d1		* Autres dimensions
.DRex3	move.b	d0,(a0)+
	dbra	d1,.DRex3
	moveq	#MnNDim-1,d0
	lea	MnChoix(a5),a0
.DRex4	clr.w	(a0)+
	dbra	d0,.DRex4
	move.l	T_EcAdr+EcEdit*4(a5),MnAdEc(a5)
*	move.w	#1,MnMouse(a5)

; Creation de l'arbre de menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	EdM_Definition(a5),a2
	move.l	EdM_Table(a5),a3
	move.l	EdM_Messages(a5),a4
	addq.l	#2,a4
.MnCr	bsr	EdM_CreObjet
	addq.l	#8,a2
	add.w	d4,a4
	cmp.b	#"*",(a2)
	bne.s	.MnCr
	addq.l	#8,a2
	move.l	a2,EdM_MenuAMOS(a5)
	move.l	a4,EdM_MessAMOS(a5)

; Cree la branche de menu USER
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	EdM_User(a5),d0
	beq	.PaUser
	move.l	d0,a4
	addq.l	#2,a4
; Cree les objets user
; ~~~~~~~~~~~~~~~~~~~~
	moveq	#1,d7			Premier programme
.Loop	move.l	Ed_BufT(a5),a2
	move.l	a2,a1
	move.b	d7,(a1)			Met les donnees
	add.b	#"0"+EdM_UserCommands-1,(a1)+
	move.b	#"3",(a1)+
	move.b	#"0",(a1)+
	move.b	#"1",(a1)+
	move.b	#"7",(a1)+
	move.b	d7,(a1)
	add.b	#"0",(a1)+
	move.b	#"0",(a1)+
	move.b	#"0",(a1)+
	clr.b	(a1)
	move.l	a4,-(sp)
; Met la chaine au propre
; ~~~~~~~~~~~~~~~~~~~~~~~
	tst.b	(a4)
	beq.s	.Cree
	lea	8(a2),a0
	moveq	#EdM_UserLong-1,d0
.Lp1	move.b	#" ",(a0)+
	dbra	d0,.Lp1
	clr.b	(a0)
	lea	9(a2),a0
	moveq	#EdM_UserLong-2-1,d0
.Lp2	move.b	(a4)+,d1
	beq.s	.Cree
	move.b	d1,(a0)+
	dbeq	d0,.Lp2
; Va fabriquer l'objet
; ~~~~~~~~~~~~~~~~~~~~
.Cree	lea	8(a2),a4
	bsr	EdM_CreObjet
	move.l	(sp)+,a4
	moveq	#2,d4
	add.b	-1(a4),d4
	tst.b	(a4)
	beq.s	.PaOb
	move.l	a4,d0
	sub.l	EdM_User(a5),d0
	neg.w	d0
	move.w	d0,-4(a3)
; Encore un objet?
; ~~~~~~~~~~~~~~~~
.PaOb	add.w	d4,a4
	addq.w	#1,d7
	cmp.w	#EdM_UserMax,d7
	ble.s	.Loop
.PaUser

; Va creer la branche AMOS
; ~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a3,EdM_TableAMOS(a5)
	bsr	EdM_BranchAMOS

; Yo, marque les marques
; ~~~~~~~~~~~~~~~~~~~~~~
	bsr	EdM_MarkAll

; Demarrage du menu
; ~~~~~~~~~~~~~~~~~
.Bug	clr.w	MnChoice(a5)
; Pas d'erreur...
	moveq	#0,d0
	bra.s	.Out
; Out of memory
.Error	bsr	EdM_End
	moveq	#1,d0
.Out	movem.l	(sp)+,d2-d7/a2-a6
	rts

; Compte les programmes caches
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Edt_CountHidden
	moveq	#0,d0
	move.l	Edt_List(a5),d1
	beq.s	.Out
.Loop	move.l	d1,a0
	tst.b	Edt_Hidden(a0)
	beq.s	.Skip
	addq.w	#1,d0
.Skip	move.l	Edt_Next(a0),d1
	bne.s	.Loop
.Out	rts

; Previous programs in AMOS menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdM_PrevHidden
	bsr	Ed_TokCur
	move.w	EdM_PosHidden(a5),d0
	beq	Ed_NotDone
	sub.w	#EdM_HiddenMax-1,d0
	bpl.s	.Skip
	clr.w	d0
.Skip	move.w	d0,EdM_PosHidden(a5)
	bsr	EdM_BranchAMOS
	rts
; Next programs in AMOS Menu
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdM_NextHidden
	bsr	Ed_TokCur
	add.w	#EdM_HiddenMax-1,EdM_PosHidden(a5)
	bsr	EdM_BranchAMOS
	rts

; _____________________________________
;
; Efface / Cree la branche de menu AMOS
; _____________________________________
;
		xdef	_ed_branch_amos
_ed_branch_amos
EdM_BranchAMOS
	movem.l	a2-a4/d2-d7,-(sp)
	bsr	EdM_Editor
	tst.l	MnBase(a5)		Menus definis?
	beq	.Pamenu
; Efface la branche AMOS
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	Ed_BufT(a5),a3
	addq.l	#8,a3
	move.l	#1,-(a3)
	moveq	#1,d5
	JJsr	L_MnFind
	beq.s	.ExistPa
	move.l	a2,d0
	moveq	#0,d5
	JJsr	L_MnEff
.ExistPa
; Cree les objets normaux
; ~~~~~~~~~~~~~~~~~~~~~~~
	move.l	EdM_MenuAMOS(a5),a2
	move.l	EdM_TableAMOS(a5),a3
	movem.l	EdM_MessAMOS(a5),a4
.MnCr	move.b	#"1",4(a2)
	bsr	EdM_CreObjet
	addq.l	#8,a2
	add.l	d4,a4
	cmp.b	#"*",(a2)
	bne.s	.MnCr
	addq.l	#8,a2

; Des programmes cach�s?
; ~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d7			Flags � zero
	move.b	4+1(a2),d7		Prend le numero du premier programme
	bsr	Edt_CountHidden
	move.w	d0,d1			Pas de programme
	beq	.EHidden
; Centre au mieux
	sub.w	#EdM_HiddenMax,d0
	bpl.s	.Plus
	clr.w	EdM_PosHidden(a5)
	bra.s	.Hidden
.Plus	cmp.w	EdM_PosHidden(a5),d0
	bcc.s	.Hidden
	move.w	d0,EdM_PosHidden(a5)
; Flags pour les lignes haut / bas
.Hidden	move.w	EdM_PosHidden(a5),d0
	beq.s	.S1
	bset	#31,d7
.S1	add.w	#EdM_HiddenMax,d0
	cmp.w	d0,d1
	ble.s	.S2
	bset	#30,d7
.S2

; Cree les objets de programmes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Met le titre
	move.b	#"1",4(a2)
	move.b	d7,4+1(a2)
	bsr	EdM_CreObjet
	addq.l	#8,a2
	add.l	d4,a4
	addq.b	#1,d7
; Met ... previous programs ...
	bclr	#31,d7
	beq.s	.PHo
	movem.l	a2/a4,-(sp)
	move.b	#"1",4(a2)
	move.b	d7,4+1(a2)
	bsr	EdM_CreObjet
	addq.b	#1,d7
	movem.l	(sp)+,a2/a4
; Saute le ... previous ...
.PHo	addq.l	#8,a2
	move.b	-(a4),d4
	lea	3(a4,d4.w),a4
; Boucle d'exploration
; ~~~~~~~~~~~~~~~~~~~~
	movem.l	a2/a4,-(sp)
	move.b	#"0"+HiddenCommands,d6
	moveq	#EdM_HiddenMax,d3
; Recherche le premier programme cache a afficher
	move.w	EdM_PosHidden(a5),d1
	bsr	Ed_GetHidden		Programme >>> D0
	bra	.PIn
; Boucle !
.PLoop	move.b	#"1",4(a2)
	move.b	d7,4+1(a2)		Numero de la branche
; Nom du programme
	move.l	a6,-(sp)
	move.l	Edt_Prg(a6),a6
	move.l	a4,a0
	move.b	#" ",(a0)+
	bsr	Ed_NPrgToBuf
	move.l	(sp)+,a6
	move.b	#" ",-1(a1)
; Va fabriquer l'objet
	bsr	EdM_CreObjet
	addq.l	#8,a2
	add.l	d4,a4
; Sous menu sp�cifique
.Loop2	move.b	#"1",4(a2)
	move.b	d7,4+1(a2)
	cmp.b	#"0",(a2)
	beq.s	.Skip
	move.b	d6,(a2)
	addq.b	#1,d6
.Skip	bsr	EdM_CreObjet
	addq.l	#8,a2
	add.l	d4,a4
	cmp.b	#"*",(a2)
	bne.s	.Loop2
	addq.l	#8,a2
	addq.b	#1,d7			Un menu de plus
	movem.l	(sp),a2/a4		Restaure les chaines
; Passe au programme suivant
.PNext	move.l	Edt_Next(a6),d0		Programme suivant
.PIn	beq.s	.EHid
	move.l	d0,a6
	tst.b	Edt_Hidden(a6)
	beq.s	.PNext
	dbra	d3,.PLoop		Encore un?
.EHid	addq.l	#8,sp
.EHidden
; Trouve la fin des menus sp�ciaux
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d4
.FLoop	addq.l	#8,a2
	move.b	-(a4),d4
	lea	3(a4,d4.w),a4
	cmp.b	#"*",(a2)
	bne.s	.FLoop
	addq.l	#8,a2
; Met ... next programs ...
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	bclr	#30,d7
	beq.s	.PBa
	movem.l	a2/a4,-(sp)
	move.b	#"1",4(a2)
	move.b	d7,4+1(a2)
	bsr	EdM_CreObjet
	addq.b	#1,d7
	movem.l	(sp)+,a2/a4
; Saute le ... next ...
.PBa	addq.l	#8,a2
	move.b	-(a4),d4
	lea	3(a4,d4.w),a4

; Termine le menu
; ~~~~~~~~~~~~~~~
.MnC	move.b	#"1",4(a2)
	move.b	d7,4+1(a2)
	bsr	EdM_CreObjet
	addq.l	#8,a2
	add.l	d4,a4
	addq.b	#1,d7
	cmp.b	#32,(a2)
	bcc.s	.MnC

; Fini!
	clr.l	(a3)
	move.w	#1,MnChange(a5)
.Pamenu	movem.l	(sp)+,a2-a4/d2-d7
	rts


; _____________________________________
;
; Marque tous les objets markables
; _____________________________________
;
EdM_MarkAll
	bsr	EdM_Editor
; Les touches dans le menu
; ~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#104,d0
	move.b	EdM_Keys(a5),d1
	bsr	EdM_SetMark
; Mode insert / Replace
; ~~~~~~~~~~~~~~~~~~~~~
	moveq	#75,d0
	move.b	Ed_Insert(a5),d1
	bsr	EdM_SetMark
; Sound On / Off
; ~~~~~~~~~~~~~~
	move.w	#148,d0
	move.b	Ed_Sounds(a5),d1
	bsr	EdM_SetMark
	rts

; _____________________________________
;
; Marque Active ou non un objet de menu
;
;	D0= 	numero de la fonction
;	D1=	Active ou non
; _____________________________________
;
EdM_SetMark
	movem.l	a2/a4,-(sp)
	tst.l	MnBase(a5)			Menus definis?
	beq.s	.Pamenu
	move.l	EdM_Definition(a5),a2
	move.l	EdM_Table(a5),a0
	move.l	EdM_Messages(a5),a4
.FLoop	cmp.b	6(a0),d0
	addq.l	#8,a0
	bne.s	.FLoop
	subq.l	#8,a0
	add.w	4(a0),a4
	moveq	#0,d0
	move.b	7(a0),d0
	lsl.w	#3,d0
	add.w	d0,a2
	move.l	a4,a1
	lea	MenuMark0(pc),a0
	tst.b	d1
	beq.s	.Skip
	lea	MenuMark1(pc),a0
.Skip	move.b	(a0)+,d0
	beq.s	.Out
	move.b	d0,(a1)+
	bra.s	.Skip
.Out	moveq	#1,d0
	bsr	EdM_ObCree
	move.w	#1,MnChange(a5)
.Pamenu	movem.l	(sp)+,a2/a4
	rts


; ____________________________________________
;
; Montre / Efface les touches dans les menus
; ____________________________________________
;
Ed_ShowKey
	bsr	Ed_TokCur
	not.b	EdM_Keys(a5)
	bsr	EdM_Init
	rts

;
; Routine de cr�ation du menu A2 / Table A3
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdM_CreObjet
	tst.b	(a4)			Un objet?
	beq.s	.Skip
	moveq	#3,d0
	lea	4(a2),a0		.B Definition de l'arbre
.MnCrL	move.b	(a0)+,(a3)
	sub.b	#"0",(a3)+
	dbra	d0,.MnCrL
	move.l	a4,d0			.W Adresse dans les messages
	sub.l	EdM_Messages(a5),d0
	move.w	d0,(a3)+
	move.b	(a2),d0			.B Touche
	sub.b	#"0",d0
	move.b	d0,(a3)+
	move.l	a2,d1			.B Definition
	sub.l	EdM_Definition(a5),d1
	lsr.l	#3,d1
	move.b	d1,(a3)+
	ext.w	d0
	bsr	EdM_ObCree
.Skip	moveq	#2,d4
	add.b	-1(a4),d4
; ILLEGAL verification table
	move.l	EdM_Table(a5),a0
	add.l	EdM_TableSize(a5),a0
	cmp.l	a0,a3
	bcs.s	.Ok
	illegal
.Ok	rts


; ____________________________________
;
; Initialisation de l'objet de menu A2/A4
; D0=	Actif / Inactif (0/1)
; ____________________________________
;
EdM_ObCree
	movem.l	a2-a6/d2-d7,-(sp)
	move.l	Ed_BufT(a5),a3
	lea	256(a3),a3
	move.l	a2,a6
	move.w	d0,-(sp)

; Recupere la position de l'objet
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d5
	lea	4(a6),a0
	moveq	#0,d0
	moveq	#3,d1
.Loop0	move.b	(a0)+,d0
	sub.b	#"0",d0
	beq.s	.Exit0
	move.l	d0,-(a3)
	addq.w	#1,d5
	dbra	d1,.Loop0
.Exit0	move.w	d5,-(sp)

; Fabrique la structure de l'objet
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	JJsr	L_MnFind
	bne.s	.Exist
	JJsr	L_MnIns
.Exist

; Inactive?
; ~~~~~~~~~
	move.w	(sp)+,d7
	bclr	#MnOff,MnFlag(a2)
	tst.w	(sp)+
	bne.s	.Act
	bset	#MnOff,MnFlag(a2)
.Act

; Couleurs de l'objet
; ~~~~~~~~~~~~~~~~~~~
	move.b	1(a6),d1
	sub.b	#"0",d1
	move.b	2(a6),d0
	sub.b	#"0",d0
	move.b	d0,MnInkA1(a2)
	move.b	d1,MnInkB1(a2)
	move.b	d0,MnInkC1(a2)
	move.b	d1,MnInkA2(a2)
	move.b	d0,MnInkB2(a2)
	move.b	d0,MnInkC2(a2)
; Fabrique la chaine
; ~~~~~~~~~~~~~~~~~~
	move.l	a4,a0
	move.l	Ed_BufT(a5),a1
	lea	2(a3),a1
.Loop2	move.b	(a0)+,d0
	move.b	d0,(a1)+
	cmp.b	#32,d0
	bcc.s	.Loop2
	subq.l	#1,a1
	move.l	a1,d1
	sub.l	a3,d1
	subq.l	#2,d1
	move.w	d1,(a3)
	clr.w	-2(a3)

; Met les codes de touche si necessaire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.b	EdM_Keys(a5)
	beq	.NoKey2
L_KDef	equ	11
	cmp.w	#1,d7
	beq	.NoKey2
	move.l	a2,-(sp)
	cmp.b	#"0",(a6)		Rien si SubMenu
	bcs	.NoKey
	moveq	#L_KDef+1,d0
	add.w	d0,(a3)
	move.b	(a4),d1
	move.l	a1,a0
.Loop3	move.b	d1,(a0)+
	subq.w	#1,d0
	bne.s	.Loop3
; Trouve la touche
	move.b	(a6),d0
	sub.b	#"0",d0
	beq	.NoKey
	bsr	Ed_Fonc2Ky
	beq	.NoKey
	move.l	a0,a2
; Fabrique le texte
	move.b	1(a2),d1
	move.b	(a2),d2
	cmp.b	#1,d2
	bls	.NoKey
	lea	Menus_Touches1(pc),a2
	move.l	a2,a0
.Lp1	cmp.b	(a0)+,d1
	addq.l	#1,a0
	bne.s	.Lp1
	moveq	#0,d0
	move.b	-1(a0),d0
	add.w	d0,a2
.Lp2	move.b	(a2)+,(a1)+
	bne.s	.Lp2
	subq.l	#1,a1
; Touche ASCII
	tst.b	d2
	bmi.s	.SkipSc
	move.b	#'"',(a1)+
	move.b	d2,(a1)+
	move.b	#'"',(a1)+
	bra.s	.EndKey
; Scancode
.SkipSc	lea	Menus_Touches2(pc),a0
	addq.l	#1,a0
	bra.s	.SkipS1
.LoopS1	tst.b	(a0)+
	bpl.s	.LoopS1
	tst.b	(a0)
	beq.s	.ExitS1
.SkipS1	cmp.b	-1(a0),d2
	bne.s	.LoopS1
.LoopS2	move.b	(a0)+,(a1)+
	bpl.s	.LoopS2
	subq.l	#1,a1
	bra.s	.EndKey
.ExitS1	moveq	#2,d3
	move.l	a1,a0
	move.b	d2,d0
	and.b	#$7f,d0
	JJsrP	L_LongToHex,a2
	move.l	a0,a1
; Termine la touche
.EndKey	move.b	(a2)+,(a1)+
	bne.s	.EndKey
	bra.s	.FinKey
; Vers un autre niveau de menu
.NLevel	lea	Menus_Touches3(pc),a2
.LoopL	move.b	(a2)+,(a1)+
	bne.s	.LoopL
; Termine la chaine
.FinKey	move.b	#" ",-1(a1)
.NoKey	move.l	(sp)+,a2
.NoKey2

; Chaine normale
; ~~~~~~~~~~~~~~
	lea	MnOb1(a2),a0
	move.l	a3,a1
	bsr	Ed_MnOb
; Chaine activee
; ~~~~~~~~~~~~~~
	lea	MnOb2(a2),a0
	lea	-2(a3),a1
	bsr	Ed_MnOb
; Chaine inactive?
; ~~~~~~~~~~~~~~~~
	lea	MnOb3(a2),a0
	lea	-2(a3),a1
	bsr	Ed_MnOb
; Chaine du fond
; ~~~~~~~~~~~~~~
	lea	MnObF(a2),a0
	lea	-2(a3),a1
	bsr	Ed_MnOb
; Finito
; ~~~~~~
	move.l	a2,a0
	move.w	#1,MnChange(a5)
	moveq	#0,d0
	movem.l	(sp)+,a2-a6/d2-d7
	rts


; Routine de creation de la chaine
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0= 	chaine dans l'objet
;	A1= 	chaine � affecter � l'objet A2
;
Ed_MnOb
	tst.w	(a1)
	beq	.MnObE
; Cree une chaine!
; ~~~~~~~~~~~~~~~~
	movem.l	a0/a1,-(sp)		* Efface l'ancienne
	bsr.s	.MnObE
	movem.l	(sp)+,a0/a1
	JJsrP	L_MnObjet,a2		* Cree la nouvelle
	beq.s	.Err			* Illegal: tester low mem!
	bmi.s	.Err
	move.l	d0,(a0)
	rts
; Efface la chaine
; ~~~~~~~~~~~~~~~~
.MnObE	move.l	(a0),d0
	beq.s	.Out
	clr.l	(a0)
	move.l	d0,a1
	moveq	#0,d0
	move.w	(a1),d0
	SyCall	MemFree
.Out	rts
.Err	illegal
	rts


; _______________________________________________
;
; Copie la zone de menus editeur
; _______________________________________________
;
EdM_Editor
	tst.b	EdM_Flag(a5)
	bne.s	.Done
	move.b	#1,EdM_Flag(a5)
	movem.l	a0/a1/d0/d1,-(sp)
	lea	EdM_Copie(a5),a0
	lea	Mn_SSave(a5),a1
	moveq	#(Mn_ESave-Mn_SSave)/2-1,d0
.Swap	move.w	(a0),d1
	move.w	(a1),(a0)+
	move.w	d1,(a1)+
	dbra	d0,.Swap
	movem.l	(sp)+,a0/a1/d0/d1
.Done	rts
; _______________________________________________
;
; Copie la zone de menus programme
; _______________________________________________
;
		xdef	_ed_menu_program
_ed_menu_program
EdM_Program
	tst.b	EdM_Flag(a5)
	beq.s	.Done
	clr.b	EdM_Flag(a5)
	movem.l	a0/a1/d0/d1,-(sp)
	lea	EdM_Copie(a5),a1
	lea	Mn_SSave(a5),a0
	moveq	#(Mn_ESave-Mn_SSave)/2-1,d0
.Swap	move.w	(a0),d1
	move.w	(a1),(a0)+
	move.w	d1,(a1)+
	dbra	d0,.Swap
	movem.l	(sp)+,a0/a1/d0/d1
.Done	rts

;_____________________________________________________________________________
;
;					Access Disque sous editeur
;_____________________________________________________________________________
;
; Entete AMOS Basic
; ~~~~~~~~~~~~~~~~~
		xdef	_ed_header
_ed_header
EnHead	dc.b	"AMOS ProEd.v    "
	even

;
; NAME1 already exist. Overwrite?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_save_over
_ed_save_over
Ed_SaveOver
	JJsr	L_RExist
	beq.s	.Skip
	move.l	Ed_VDialogues(a5),a0
	move.l	Name1(a5),(a0)
	moveq	#EdD_AExist,d0
	bsr	Ed_Dialogue
	cmp.w	#1,d0
	bne	Ed_NotDone
.Skip	rts
;
; Sauve le programme si on l'a change!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_saved
_ed_saved
Ed_Saved
	tst.b	Prg_Change(a6)
	beq.s	.Skip
; Cree la chaine avec le nom, en 0VA
	move.l	Buffer(a5),a0
	move.l	a0,a1
	moveq	#31,d0
.Clr	move.b	#" ",(a1)+
	dbra	d0,.Clr
	clr.b	(a1)
	bsr	Ed_NPrgToBuf
	clr.b	(a0)
	move.l	Buffer(a5),a0
	bsr	Ed_DChaine
	move.l	Ed_VDialogues(a5),a0
	move.l	a1,0*4(a0)
; Appelle la boite de dialogue
	moveq	#EdD_Saved,d0
	bsr	Ed_Dialogue
	cmp.w	#2,d0
	beq.s	.Skip
	cmp.w	#1,d0
	bne	Ed_NotDone2
	bsr	Ed_SaveIt
.Skip	rts


;
; Loading and saving: src/editor/disk.cpp
Ed_RLoadHidden	jmp	_ed_rload_hidden_entry
Ed_LoadHidden	jmp	_ed_load_hidden_entry
Ed_Load	jmp	_ed_load_entry
Ed_ReLoad	jmp	_ed_reload_entry
Ed_DNom	jmp	_ed_name_part_entry
Ed_Merge	jmp	_ed_merge_entry
Ed_LoadA	jmp	_ed_merge_ascii_entry
Ed_Rename	jmp	_ed_rename_entry
Ed_SaveAsName	jmp	_ed_save_as_name_entry
Ed_SaveAs	jmp	_ed_save_as_entry
Ed_Save	jmp	_ed_save_entry
Ed_SaveIt	jmp	_ed_save_it_entry
Ed_SavePrg	jmp	_ed_save_program_entry
Ed_SavePrg2	jmp	_ed_save_program2_entry
Ed_MakeBak	jmp	_ed_make_bak_entry
Ed_SaveIcon	jmp	_ed_save_icon_entry

;
; ERREUR DISQUE SOUS EDITEUR
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;Ed_DiskError
; Arret du flag
; ~~~~~~~~~~~~~
;	clr.w	Ed_Disk(a5)
; Trouve l'erreur
; ~~~~~~~~~~~~~~~
;	move.l	Ed_RunMessages(a5),a0
;	addq.w	#1,d0
;	bsr	Ed_GetMessageA0
;	movem.l	a0/d0,-(sp)
;	bsr	Ed_BufUntok
;	movem.l	(sp)+,a0/d0
; ALERTE!
; ~~~~~~~
;	moveq	#125,d0
;	bra	Ed_Alert

; ___________________________________________________________________
;
; 					DIVERS
; ___________________________________________________________________
;
; Agrandi une image aux dimensions de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; 	D0=	Y1
;	D1= 	Y2
;
		xdef	_ed_enlarge
_ed_enlarge
Ed_Enlarge
	movem.l	a0-a2/d0-d5,-(sp)
	move.l	T_EcAdr+EcEdit*4(a5),a0
	move.l	a0,a1
	ext.l	d0
	ext.l	d1
	move.l	d1,d5
	move.l	d0,d1
	move.l	d0,d3
	move.l	#160,d0
	move.l	#480,d4
	move.w	Ed_Sx(a5),d2
	sub.w	#480-160,d2
	ext.l	d2
	JJsrR	L_ScCopy,a2
	movem.l	(sp)+,a0-a2/d0-d5
	rts

;
; Copie la palette de la souris dans l'�cran A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Ecran Destination
;
Ed_CopyPal
	lea	EcPal+32(a0),a0
	lea	PI_DefEPa+32(a5),a1
	moveq	#16-1,d0
.loop	move.w	(a1)+,(a0)+
	dbra	d0,.loop
	rts
; _____________________________________________________
;
;	Attente souris dans le titre
; _____________________________________________________
;
Ed_TiWait
	move.w	d0,-(sp)
.loop	JJsr	L_Sys_WaitMul
	SyCall	Inkey
	tst.l	d1
	bne.s	.end
	SyCall	MouseKey
	and.w	#%111,d1
	bne.s	.end
	subq.w	#1,(sp)
	bne.s	.loop
.end	addq.l	#2,sp
	rts

;
; Unpack Image de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D0=	Numero
;	D1=	X
;	D2=	Y
;
Es_Unpack
	move.l	T_EcAdr+EcFonc*4(a5),a1
	bra.s	EdU
		xdef	_ed_unpack
_ed_unpack
Ed_Unpack
	move.l	T_EcAdr+EcEdit*4(a5),a1
EdU	move.l	Ed_Resource(a5),a0
	add.l	2(a0),a0
	lsl.w	#2,d0
	add.l	-4+2(a0,d0.w),a0
	ext.l	d1
	ext.l	d2
	JJsrP	L_UnPack_Bitmap,a2
	rts

; ___________________________________________________________________
;
; 						ROUTINES BOUTONS
; ___________________________________________________________________
;
		xdef	_ed_button_routines
_ed_button_routines
Bt_RoutIn
	bra	Ed_BtWindowClose		1- Close window
	bra	Ed_BtWindowHide			2- Hide window
	bra	Ed_BtWindowSize			3- Shrink window
	bra	Bt_EsCDraw			4- Dessin bouton escape
	bra	Esc_BtGetOutput			5- Position actuelle OUTPUT
	bra	Bt_EdDraw			6- Dessin d'un bouton
; Dessin d'un bouton ESCAPE!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
Bt_EsCDraw
	move.l	d2,-(sp)
	move.w	Bt_Image(a2),d0
	add.w	Bt_Pos(a2),d0
	move.w	Bt_X(a2),d1
	move.w	Bt_Y(a2),d2
	bsr	Es_Unpack
	move.l	(sp)+,d2
	rts
; Dessin d'un bouton EDITOR!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
Bt_EdDraw
	move.l	d2,-(sp)
	move.w	Bt_Image(a2),d0
	add.w	Bt_Pos(a2),d0
	move.w	Bt_X(a2),d1
	move.w	Bt_Y(a2),d2
	bsr	Ed_Unpack
	move.l	(sp)+,d2
	rts

;
; Gestion des boutons speciaux
; ____________________________________
;
EdK_MarkAll
	move.b	Ed_Insert(a5),d0
	and.w	#$0001,d0
	bchg	#0,d0
	lea	Ed_Boutons+(Bt_Insert-1)*Bt_Long(a5),a2
	move.w	d0,Bt_Pos(a2)
	bsr	Bt_EdDraw
.Skip	rts

; ___________________________________________________________________
;
;						ACCES DISQUE EDITEUR
; ___________________________________________________________________
;


; OPEN: ouvre le fichier systeme (diskname1) access mode D2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Open	move.l 	Name1(a5),d1
Ed_OpenD1
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOOpen(a6)
	move.l	(sp)+,a6
	move.l	d0,Handle(a5)
	rts

; CLOSE fichier systeme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_close
_ed_close
Ed_Close
	move.l	Handle(a5),d1
	beq.s	.Skip
	clr.l	Handle(a5)
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOClose(a6)
	move.l	(sp)+,a6
.Skip	rts

; READ fichier systeme D3 octets dans D2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Read	movem.l	d1/a0/a1/a6,-(sp)
	move.l	Handle(a5),d1
	move.l	DosBase(a5),a6
	jsr	_LVORead(a6)
	movem.l	(sp)+,d1/a0/a1/a6
	cmp.l	d0,d3
	rts

; WRITE fichier systeme D3 octets de D2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Write
	movem.l	d1/a0/a1/a6,-(sp)
	move.l	Handle(a5),d1
	move.l	DosBase(a5),a6
	jsr	_LVOWrite(a6)
	movem.l	(sp)+,d1/a0/a1/a6
	cmp.l	d0,d3
	rts

; SEEK fichier system D3 mode D2 deplacement
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_Seek	move.l	Handle(a5),d1
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOSeek(a6)
	move.l	(sp)+,a6
	tst.l	d0
	rts
;
; Ouverture de l'imprimante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_PRTOpen	jmp	_ed_printer_open_entry
		xdef	_ed_print_error
_ed_print_error
Ed_PErr	move.w	#216,d0
	bra.s	Ed_DBis
Ed_PRTPrint	jmp	_ed_printer_print_entry
;
; Erreurs dans le chargement programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_prg_load_err
_ed_prg_load_err
Ed_PrgLoadErr
	cmp.w	#-2,d0
	beq	Ed_OMm
	cmp.w	#-3,d0
	beq	Ed_PaAMOS
;
; Gestion des erreurs disque
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_d_error
_ed_d_error
Ed_DError
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOIoErr(a6)
	move.l	(sp)+,a6
	lea	ErDisked(pc),a0
	moveq	#DEBase-1,d1
.Explo	addq.l	#1,d1
	move.b	(a0)+,d2
	beq.s	Paerr
	cmp.b	d0,d2
	bne.s	.Explo
	move.w	d1,d0
	addq.w	#1,d0
; Imprime le message
Ed_DBis	move.l	Ed_RunMessages(a5),a0
	bsr	Ed_GetMessageA0
; En cas de ZAPPEUSE, branche � ALERT normal
	tst.b	Ed_Zappeuse(a5)
	bne	Ed_ZapAlert
; Boite de dialogue
	move.l	Ed_VDialogues(a5),a1
	move.l	a0,0*4(a1)
	bsr	Ed_RazAlert
	moveq	#"G",d0			Va faire du bruit!
	bsr	Ed_SamPlay
	moveq	#EdD_DiskErr,d0
	bsr	Ed_Dialogue
	bra	Ed_Loop
Paerr	moveq	#DEBase+22+1,d0
	bra.s	Ed_DBis
; Table des erreurs reconnues
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
ErDisked
	dc.b 203,204,205,210,213,214,216,218
	dc.b 220,221,222,223,224,225,226,0
	even

; Appel du selecteur de fichier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_file_selector
_ed_file_selector
Ed_File_Selector
	tst.b	Ed_Zappeuse(a5)		Zappeuse>>> on revient !
	bne.s	.Zap
	movem.l	a2-a3/d2-d7,-(sp)
	bsr	Ed_GetMessage
	pea	-2(a0)
	bsr	Ed_GetNMessage
	pea	-2(a0)
	bsr	Ed_GetNMessage
	pea	-2(a0)
	bsr	Ed_GetNMessage
	pea	-2(a0)
	move.l	sp,a3
	move.w	#1,Ed_FSel(a5)
	bsr	Ed_SetBanks
	JJsr	L_Dsk.FileSelector
	clr.w	Ed_FSel(a5)
	lea	16(sp),sp
	movem.l	(sp)+,a2-a3/d2-d7
	tst.w	d0
	rts
.Zap	moveq	#1,d0
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	TOKENISATION / DETOKENISATION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; Initialisation des tables de tokenisation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tok_Init
	tst.l	Ed_BufT(a5)
	bne	MTokX
; Reserve le buffer de tokenisation...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#EdBTT,d0
	SyCall	MemFastClear
	beq	MTError
	move.l	a0,Ed_BufT(a5)
; Fabrique les tables de tokenisation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#1024*12,d0
	JJsr	L_ResTempBuffer
	beq	MTError
	lea	AdTokens(a5),a3
	lea	AdTTokens(a5),a4
	moveq	#-4,d7
MTok1	addq.w	#4,d7
	cmp.w	#26*4,d7
	bcc	MTokX
	move.l	0(a3,d7.w),d6
	beq.s	MTok1
	move.l	d6,a1
	addq.l	#6,a1
; Premiere table
	move.l	TempBuffer(a5),a6
	move.l	a6,a0
	lea	2048(a0),a2
	moveq	#0,d0
MTok2	move.l	a1,d1
	lea	4(a1),a1
MTok0	move.b	(a1)+,d0
	bpl.s	MTok3
	and.w	#$7f,d0
	subq.l	#1,a1
MTok3	cmp.b	#"!",d0
	beq.s	MTok0
	cmp.b	#" ",d0
	beq.s	MTok0
	bsr	MnD0
	cmp.b	#"a",d0
	bcs.s	MTok4
	cmp.b	#"z",d0
	bhi.s	MTok4
	move.w	d0,(a0)+
	sub.l	d6,d1
	move.w	d1,(a2)+
MTok4	bsr	EdlNext
	bne.s	MTok2
	move.w	#-1,(a0)+
; Deuxieme table
	move.l	a0,d0
	sub.l	a6,d0
	add.l	#26*4+2+4,d0
	move.l	d0,d1
	SyCall	MemFastClear
	beq	MTError
	move.l	a0,0(a4,d7.w)
	move.l	a0,a1
	move.l	a0,d3
	move.l	d1,(a1)+
	lea	26*2+2(a1),a2
	moveq	#"a",d5
	move.w	#2048,d4
MTok5	move.l	a2,d0
	sub.l	d3,d0
	move.w	d0,(a1)+
	move.l	a6,a0
MTok6	move.w	(a0)+,d0
	bmi.s	MTok7
	cmp.w	d0,d5
	bne.s	MTok6
	move.w	-2(a0,d4.w),(a2)+
	bra.s	MTok6
MTok7	clr.w	(a2)+
	addq.w	#1,d5
	cmp.w	#"z",d5
	bls.s	MTok5
	clr.w	(a1)+
	bra	MTok1
MTokX	moveq	#0,d0
	JJsr	L_ResTempBuffer
	moveq	#0,d0
	rts
; Out of memory!
; ~~~~~~~~~~~~~~
MTError	moveq	#0,d0
	JJsr	L_ResTempBuffer
	moveq	#1,d0
	rts
; Routine: token suivant
; ~~~~~~~~~~~~~~~~~~~~~~
EdlNext	tst.b	(a1)+			; Saute le nom
	bpl.s	EdlNext
.Tkln1	tst.b	(a1)+			; Saute les params
	bpl.s	.Tkln1
	move.w	a1,d1
	btst	#0,d1			; Rend pair
	beq.s	.Tkln2
	addq.l	#1,a1
.Tkln2	tst.w	(a1)
	rts
; Routine: D0 minuscule
MnD0	cmp.b	#"A",d0
	bcs.s	.Mnd0a
	cmp.b	#"Z",d0
	bhi.s	.Mnd0a
	add.b	#32,d0
.Mnd0a	rts

; Effacement de la tokenisation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tok_Del
; Efface le buffer de tokenisation
	move.l	Ed_BufT(a5),d0
	beq.s	DlTkX
	clr.l	Ed_BufT(a5)
	move.l	d0,a1
	move.l	#EdBTT,d0
	SyCall	MemFree
; Efface les tables de tokens rapide
	lea	AdTTokens(a5),a2
	moveq	#25,d1
DlTk1	move.l	(a2)+,d0
	beq.s	DlTk2
	clr.l	-4(a2)
	move.l	d0,a1
	move.l	(a1),d0
	SyCall	MemFree
DlTk2	dbra	d1,DlTk1
DlTkX	rts


; Tokenise, Detok, Mon_Detok, TInst: src/editor/tokens.cpp
Tokenise:	jmp	_ed_tokenise_entry
Mon_Detok	jmp	_ed_mon_detok_entry
Detok:		jmp	_ed_detok_entry
TInst:		jmp	_ed_tinst_entry


;	Passe en minuscules
; ~~~~~~~~~~~~~~~~~~~~~~~~~
Minus:	cmp.b 	#"A",d0
	bcs.s	.Skip
	cmp.b	#"Z",d0
	bhi.s	.Skip
	add.b	#"a"-"A",d0
.Skip	rts


;						Table des operateurs
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_operators,_ed_operators_end,_ed_extension_missing
_ed_operators
Dtk_Operateurs
	dc.w	1,1
	dc.b 	" xor"," "+$80,"O00",-1
	dc.w	1,1
	dc.b 	" or"," "+$80,"O00",-1
	dc.w	1,1
	dc.b 	" and"," "+$80,"O00",-1
	dc.w	1,1
	dc.b 	"<",">"+$80,"O20",-1
	dc.w	1,1
	dc.b 	">","<"+$80,"O20",-1
	dc.w	1,1
	dc.b 	"<","="+$80,"O20",-1
	dc.w	1,1
	dc.b 	"=","<"+$80,"O20",-1
	dc.w	1,1
	dc.b 	">","="+$80,"O20",-1
	dc.w	1,1
	dc.b 	"=",">"+$80,"O20",-1
	dc.w	1,1
	dc.b 	"="+$80,"O20",-1
	dc.w	1,1
	dc.b 	"<"+$80,"O20",-1
	dc.w	1,1
	dc.b 	">"+$80,"O20",-1
	dc.w	1,1
	dc.b 	"+"+$80,"O22",-1
	dc.w	1,1
	dc.b 	"-"+$80,"O22",-1
	dc.w	1,1
	dc.b 	" mod"," "+$80,"O00",-1
	dc.w	1,1
	dc.b 	"*"+$80,"O00",-1
	dc.w	1,1
	dc.b 	"/"+$80,"O00",-1
	dc.w	1,1
	dc.b 	"^"+$80,"O00",-1
	even
_ed_operators_end
Dtk_OpFin
	dc.l 	0
_ed_extension_missing
ExtNot	dc.b	"Extension ",$80

	Even


; _____________________________________________________________________________

Ed_Vide		dc.w 0
		xdef	_ed_cursor_normal
_ed_cursor_normal
CurNor		dc.b	%00000000
		dc.b	%00000000
		dc.b	%00000000
		dc.b	%00000000
		dc.b	%00000000
		dc.b	%11111111
		dc.b	%11111111
		dc.b	%00000000
		xdef	_ed_cursor_block
_ed_cursor_block
CurBloc		dc.b	%10101010
		dc.b	%01010101
		dc.b	%10101010
		dc.b	%01010101
		dc.b	%10101010
		dc.b	%01010101
		dc.b	%10101010
		dc.b	%01010101

; Controle curseur
; ~~~~~~~~~~~~~~~~
CRet		dc.b	13,10,0,0
		xdef	_ed_cursor_on_text
_ed_cursor_on_text
ChCuOn		dc.b	27,"C1",0
		xdef	_ed_cursor_off_text
_ed_cursor_off_text
ChCuOff		dc.b	27,"C0",0

; Marque des menus actives
; ~~~~~~~~~~~~~~~~~~~~~~~~
MenuMark0	dc.b	"   ",0
MenuMark1	dc.b	" ->",0
; Nom des touches de fonction
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
Mt1
Menus_Touches1	dc.b	0,K0-Mt1			0
		dc.b	Shf,K1-Mt1			1
		dc.b	Ctr,K2-Mt1			2
		dc.b	Ami,K3-Mt1			3
		dc.b	Alt,K4-Mt1			4
		dc.b	Ctr+Shf,K5-Mt1
		dc.b	Ctr+Ami,K6-Mt1
		dc.b	Ctr+Alt,K7-Mt1
		dc.b	Ami+Shf,K8-Mt1
		dc.b	Ami+Alt,K9-Mt1
		dc.b	Alt+Shf,K10-Mt1			0
		dc.b	Shf+Alt+Ami,K11-Mt1		11
		dc.b	Ctr+Alt+Ami,K12-Mt1		12
		dc.b	Ctr+Shf+Ami,K13-Mt1		13
		dc.b	Ctr+Shf+Alt,K14-Mt1		14
		dc.b	Ctr+Shf+Alt+Ami,K15-Mt1		5
K0		dc.b	"[   ",0,"   ]",0		0
K1		dc.b	"[Shift+",0,"]",0		1
K2		dc.b	"[Contr+",0,"]",0		2
K3		dc.b	"[Amiga+",0,"]",0		3
K4		dc.b	"[Alter+",0,"]",0		4
K5		dc.b 	"[Ct+Sh+",0,"]",0		5
K6		dc.b	"[Ct+Am+",0,"]",0		6
K7		dc.b	"[Ct+Al+",0,"]",0		7
K8		dc.b	"[Am+Sh+",0,"]",0		8
K9		dc.b	"[Am+Al+",0,"]",0		9
K10		dc.b	"[Al+Sh+",0,"]",0		10
K11		dc.b	"[SAlAm+",0,"]",0		11
K12		dc.b	"[CAlAm+",0,"]",0		12
K13		dc.b 	"[CShAm+",0,"]",0		13
K14		dc.b	"[CShAl+",0,"]",0		14
K15		dc.b	"[CSAAm+",0,"]",0		15
Menus_Touches3	dc.b	"[ SubMenu ]",0
Menus_Touches2	dc.b	$80+$45,"Esc"
		dc.b	$80+$42,"Tab"
		dc.b	$80+$46,"Del"
		dc.b	$80+$41,"Bck"
		dc.b	$80+$5f,"Hlp"
		dc.b	$80+$44,"Ret"
		dc.b	$80+$4c,"Up "
		dc.b	$80+$4f,"Lft"
		dc.b	$80+$4e,"Rgt"
		dc.b	$80+$4d,"Dwn"
		dc.b	$80+$0f,"N0"
		dc.b	$80+$1d,"N1"
		dc.b	$80+$1e,"N2"
		dc.b	$80+$1f,"N3"
		dc.b	$80+$2d,"N4"
		dc.b	$80+$2e,"N5"
		dc.b	$80+$2f,"N6"
		dc.b	$80+$3d,"N7"
		dc.b	$80+$3e,"N8"
		dc.b	$80+$3f,"N9"
		dc.b	$80+$3c,"N."
		dc.b	$80+$43,"Ent"
		dc.b	$80+$50,"F1 "
		dc.b	$80+$51,"F2 "
		dc.b	$80+$52,"F3 "
		dc.b	$80+$53,"F4 "
		dc.b	$80+$54,"F5 "
		dc.b	$80+$55,"F6 "
		dc.b	$80+$56,"F7 "
		dc.b	$80+$57,"F8 "
		dc.b	$80+$58,"F9 "
		dc.b	$80+$59,"F10"
		dc.b	$80,$00
		Even

; Numero des messages dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EdD_Title	equ	0
EdD_Quit	equ	2
EdD_NAll	equ	3
EdD_Search	equ	4
EdD_Replace	equ	6
EdD_WBlock	equ	8
EdD_WText	equ	9
EdD_Changes	equ	10
EdD_Saved	equ	11
EdD_Macro1	equ	13
EdD_Macro2	equ	14
EdD_Macro3	equ	15
EdD_MacroD	equ	18
EdD_MacroNA	equ	19
EdD_MacroAll	equ	20
EdD_MacroEra	equ	21
EdD_MacroNo	equ	22
EdD_MacroPas	equ	23
EdD_KyMn1       equ     24
EdD_KyMn2       equ     27
EdD_KyMn3       equ     28
EdD_KyMnE       equ     26
EdD_PrgMn1	equ	30
EdD_PrgMn2	equ	31
EdD_PrgMn3	equ	32
EdD_PrgMnE	equ	33
EdD_GotoL	equ	35
EdD_SetBuf	equ	36
EdD_TooSmall	equ	37
EdD_SetTab	equ	38
EdD_MnUsA	equ	40
EdD_MnUsD	equ	41
EdD_MnUs2	equ	42
EdD_MnUsE	equ	43
EdD_MnUs	equ	44
EdD_SvConf	equ	45
EdD_CSaved	equ	46
EdD_AExist	equ	47
EdD_ASave	equ	48
EdD_OQuit	equ	49
EdD_Avert	equ	50
EdD_NoWarm	equ	51
EdD_WarmErr	equ	52
EdD_WarmDel	equ	53
EdD_Infos	equ	54
EdD_AboutE	equ	55
EdD_DiskErr	equ	58
EdD_Ligne	equ	59
EdD_PBloc	equ	60
EdD_PProg	equ	61
EdD_WQuit	equ	62
;	ENDC
