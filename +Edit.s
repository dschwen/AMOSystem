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
		XREF	_ed_escape_entry
		XREF	_ed_escape_loop_entry
		XREF	_ed_escape_leave_entry
		XREF	_ed_escape_forget_entry
		XREF	_ed_escape_forget_all_entry
		XREF	_ed_error_direct_entry
		XREF	_ed_escape_appear_entry
		XREF	_ed_escape_max_mouse_entry
		XREF	_ed_escape_hide_entry
		XREF	_ed_escape_display_entry
		XREF	_ed_escape_display_d0_entry
		XREF	_ed_get_error_entry
		XREF	_ed_memory_draw_entry
		XREF	_ed_memory_draw_title_entry
		XREF	_ed_memory_show_entry
		XREF	_ed_unpack_entry
		XREF	_ed_unpack_title_entry
		XREF	_ed_hide_entry
		XREF	_ed_appear_entry
		XREF	_ed_redraw_entry
		XREF	_ed_limit_mouse_entry
		XREF	_ed_new_entry
		XREF	_ed_new2_entry
		XREF	_ed_window_new_entry
		XREF	_ed_save_over_entry
		XREF	_ed_saved_entry
		XREF	_ed_set_buffer_entry
		XREF	_ed_set_buffer_from_entry
		XREF	_ed_get_place_entry
		XREF	_ed_file_selector_entry
		XREF	_ed_open_load_entry
		XREF	_ed_quit_entry
		XREF	_ed_do_quit_entry
		XREF	_ed_warm_start_entry
		XREF	_ed_quit_options_entry
		XREF	_ed_about_entry
		XREF	_ed_about_extensions_entry
		XREF	_ed_information_entry
		XREF	_ed_goto_line_entry
		XREF	_ed_search_dialogue_entry
		XREF	_ed_search_new_entry
		XREF	_ed_search_previous_entry
		XREF	_ed_search_next_entry
		XREF	_ed_replace_new_entry
		XREF	_ed_replace_next_entry
		XREF	_ed_replace_previous_entry
		XREF	_ed_menu_entry
		XREF	_ed_menus_end_entry
		XREF	_ed_menu_error_entry
		XREF	_ed_menu_init_entry
		XREF	_ed_previous_hidden_entry
		XREF	_ed_next_hidden_entry
		XREF	_ed_branch_amos_entry
		XREF	_ed_menu_marks_entry
		XREF	_ed_flip_keys_entry
		XREF	_ed_menus_editor_entry
		XREF	_ed_menus_program_entry
		XREF	_ed_set_autosave_entry
		XREF	_ed_save_all_entry
		XREF	_ed_user_menu_entry
		XREF	_ed_add_user_entry
		XREF	_ed_delete_user_entry
		XREF	_ed_program_to_menu_entry
		XREF	_ed_key_to_menu_entry
		XREF	_ed_flip_insert_entry
		XREF	_ed_flip_sounds_entry
		XREF	_ed_insert_button_entry
		XREF	_ed_set_banks_entry
		XREF	_ed_sound_play_entry
		XREF	_ed_sounds_changed_entry
		XREF	_ed_sounds_end_entry
		XREF	_ed_resource_load_entry
		XREF	_ed_resource_free_entry
		XREF	_ed_config_set_palette_entry
		XREF	_ed_config_load_entry
		XREF	_ed_config_save_entry
		XREF	_ed_config_free_entry
		XREF	_ed_config_save_default_entry
		XREF	_ed_config_save_default_quiet_entry
		XREF	_ed_config_save_as_entry
		XREF	_ed_config_load_default_entry
		XREF	_ed_config_load_default_quiet_entry
		XREF	_ed_config_load_as_entry
		XREF	_ed_config_redraw_entry
		XREF	_ed_config_save_changed_entry
		XREF	_ed_wait_input_entry
		XREF	_ed_text_change_entry
		XREF	_ed_text_count_entry
		XREF	_ed_text_free_entry
		XREF	_ed_mem_reserve_entry
		XREF	_ed_mem_free_entry
		XREF	_ed_list_new_entry
		XREF	_ed_list_delete_all_entry
		XREF	_ed_list_delete_entry
		XREF	_ed_dialogues_open_entry
		XREF	_ed_dialogues_close_entry
		XREF	_ed_strings_reset_entry
		XREF	_ed_string_new_entry
		XREF	_ed_string_add_entry
		XREF	_ed_dialogue_entry
		XREF	_ed_do_dialogue_entry
		XREF	_ed_macro_load_default_entry
		XREF	_ed_macro_load_as_entry
		XREF	_ed_macro_save_default_entry
		XREF	_ed_macro_save_as_entry
		XREF	_ed_macro_load_entry
		XREF	_ed_macro_erase_entry
		XREF	_ed_macro_erase_all_entry
		XREF	_ed_macro_record_entry
		XREF	_ed_macro_stop_entry
		XREF	_ed_macros_free_entry
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
		xdef	_ed_system
_ed_system
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

		xdef	_ed_open_editor
_ed_open_editor
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
		xdef	_ed_close_editor
_ed_close_editor
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
; The last session again: editor/session.cpp
Ed_WarmStart	jmp	_ed_warm_start_entry

;______________________________________________________________________________
;
;						Dessin du titre
;______________________________________________________________________________
;
		xdef	_ed_draw_top
_ed_draw_top
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
; The memory gauges: editor/screen.cpp
Ed_MemoryDraw	jmp	_ed_memory_draw_entry
Es_MemoryDraw	jmp	_ed_memory_draw_title_entry
Ed_MemoryAff	jmp	_ed_memory_show_entry

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
Ed_MnGere	jmp	_ed_menu_entry

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

; Memory blocks and lists: editor/macros.cpp
Ed_MemReserve	jmp	_ed_mem_reserve_entry
Ed_MemFree	jmp	_ed_mem_free_entry
Ed_ListeNew	jmp	_ed_list_new_entry
Ed_ListeDelAll	jmp	_ed_list_delete_all_entry
Ed_ListeDel	jmp	_ed_list_delete_entry

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

; The dialogues: editor/macros.cpp
Ed_InitDialogues	jmp	_ed_dialogues_open_entry
Ed_EndDialogues	jmp	_ed_dialogues_close_entry
Ed_IChaine	jmp	_ed_strings_reset_entry
Ed_DChaine	jmp	_ed_string_new_entry
Ed_AChaine	jmp	_ed_string_add_entry
		xdef	_ed_dialogue
_ed_dialogue
Ed_Dialogue:	jmp	_ed_dialogue_entry
Ed_DoDialog	jmp	_ed_do_dialogue_entry


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


; Quit: editor/session.cpp
Ed_Quit	jmp	_ed_quit_entry
		xdef	_ed_do_quit
_ed_do_quit
Ed_DoQuit	jmp	_ed_do_quit_entry

;
; Quit options: editor/info.cpp
Ed_QuitOptions	jmp	_ed_quit_options_entry

; ______________________________________________________________________________
;
;							EDITOR INFORMATIONS
; ______________________________________________________________________________
;

; About, about the extensions, information: editor/info.cpp
Ed_About	jmp	_ed_about_entry
Ed_AboutExt	jmp	_ed_about_extensions_entry
Ed_Infos	jmp	_ed_information_entry

; __________________________________________________________________________
;
;						BANQUES / CONFIG EDITEUR
; __________________________________________________________________________
;
;
; The resource bank, the sounds: editor/config.cpp
Ed_ResourceLoad	jmp	_ed_resource_load_entry
Ed_ResourceFree	jmp	_ed_resource_free_entry
EdC_SetPalette	jmp	_ed_config_set_palette_entry
Ed_SamChanged	jmp	_ed_sounds_changed_entry
Ed_SamEnd	jmp	_ed_sounds_end_entry
		xdef	_ed_sam_play
_ed_sam_play
Ed_SamPlay	jmp	_ed_sound_play_entry


; ___________________________________________________________________
;
;							CONFIG!
; ___________________________________________________________________
;

; Met les banques de m�moire/dialogue de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_set_banks
_ed_set_banks
Ed_SetBanks	jmp	_ed_set_banks_entry

; The configuration: editor/config.cpp
EdC_SaveDefault	jmp	_ed_config_save_default_entry
EdC_SaveDef	jmp	_ed_config_save_default_quiet_entry
EdC_SaveAs	jmp	_ed_config_save_as_entry
EdC_LoadDefault	jmp	_ed_config_load_default_entry
EdC_LoadDef	jmp	_ed_config_load_default_quiet_entry
EdC_LoadAs	jmp	_ed_config_load_as_entry
EdC_Redraw	jmp	_ed_config_redraw_entry
EdC_Saved	jmp	_ed_config_save_changed_entry
EdC_Load	jmp	_ed_config_load_entry
EdC_Free	jmp	_ed_config_free_entry
EdC_Save	jmp	_ed_config_save_entry
EdC_ChangeTexte	jmp	_ed_text_change_entry
Ed_GetFsMessage	jmp	_ed_text_free_entry
Ed_GetNbMessage	jmp	_ed_text_count_entry
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

; Autosave, the user menu, programs and keys to the menu, the marks:
; editor/menu.cpp
Ed_SetAutoSave	jmp	_ed_set_autosave_entry
Ed_SaveAuto	jmp	_ed_save_all_entry
Ed_UserMenu	jmp	_ed_user_menu_entry
Ed_AddUser	jmp	_ed_add_user_entry
Ed_DelUser	jmp	_ed_delete_user_entry
Ed_Prg2Menu	jmp	_ed_program_to_menu_entry
Ed_Key2Menu	jmp	_ed_key_to_menu_entry
Ed_Ins	jmp	_ed_flip_insert_entry
Ed_SamOn	jmp	_ed_flip_sounds_entry

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
; Macros: editor/macros.cpp
EdMa_LoadDefault	jmp	_ed_macro_load_default_entry
EdMa_LoadAs	jmp	_ed_macro_load_as_entry
EdMa_SaveDefault
EdMa_SaveDef	jmp	_ed_macro_save_default_entry
EdMa_SaveAs	jmp	_ed_macro_save_as_entry
EdMa_Load	jmp	_ed_macro_load_entry
EdMa_Del	jmp	_ed_macro_erase_entry
EdMa_DelAll	jmp	_ed_macro_erase_all_entry
EdMa_New	jmp	_ed_macro_record_entry
		xdef	_ed_macro_stop
_ed_macro_stop
EdMa_Stop	jmp	_ed_macro_stop_entry
EdMa_End	jmp	_ed_macros_free_entry

;_____________________________________________________________________________
;
;						Search / Replace / Goto
;_____________________________________________________________________________
;

; Goto, search and replace: src/editor/search.cpp
Ed_GotoL	jmp	_ed_goto_line_entry
Ed_DiaS	jmp	_ed_search_dialogue_entry
Ed_Search	jmp	_ed_search_new_entry
Ed_SearchPrev	jmp	_ed_search_previous_entry
Ed_SearchNext	jmp	_ed_search_next_entry
Ed_Replace	jmp	_ed_replace_new_entry
Ed_ReplaceNext	jmp	_ed_replace_next_entry
Ed_ReplacePrev	jmp	_ed_replace_previous_entry

; Ed_SchBoth, Ed_SchFront, Ed_SchBack: src/editor/search.cpp
Ed_SchBoth	jmp	_ed_search_entry
Ed_SchFront	jmp	_ed_search_front_entry
Ed_SchBack	jmp	_ed_search_back_entry


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
Ed_GetError	jmp	_ed_get_error_entry

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
		xdef	_ed_test_program
_ed_test_program
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

; Direct mode: editor/direct.cpp
Ed_Escape	jmp	_ed_escape_entry
		xdef	_ed_esc_loop
_ed_esc_loop
Esc_Loop
	move.l	BasSp(a5),sp
	jmp	_ed_escape_loop_entry
Esc_Esc	jmp	_ed_escape_leave_entry
Esc_KMemEffD0	jmp	_ed_escape_forget_entry
Esc_KMemEnd	jmp	_ed_escape_forget_all_entry
Esc_BtGetOutput
	move.b	Esc_Output(a5),Bt_Pos+1(a2)
	rts
		xdef	_ed_err_direct
_ed_err_direct
Ed_ErrDirect	jmp	_ed_error_direct_entry


; ___________________________________________________________________
;
; 	ECRAN ESCAPE
; ___________________________________________________________________
;

; Apparition de l'ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Esc_Appear	jmp	_ed_escape_appear_entry
Esc_MaxMouse	jmp	_ed_escape_max_mouse_entry
Esc_Hide	jmp	_ed_escape_hide_entry
Es_Display	jmp	_ed_escape_display_entry
Es_DisplayD0	jmp	_ed_escape_display_d0_entry
;
; Cache l'�cran de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; The editor's screen away and back: editor/screen.cpp
		xdef	_ed_hide
_ed_hide
Ed_Hide	jmp	_ed_hide_entry
		xdef	_ed_appear
_ed_appear
Ed_Appear	jmp	_ed_appear_entry
Ed_Redraw	jmp	_ed_redraw_entry
Ed_LimM	jmp	_ed_limit_mouse_entry

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
		xdef	_ed_no_found
_ed_no_found
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
		xdef	_ed_not_done2
_ed_not_done2
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
; The text buffer's size: editor/program.cpp
		xdef	_ed_get_place
_ed_get_place
Ed_GetPlace	jmp	_ed_get_place_entry
Ed_SetBuffer	jmp	_ed_set_buffer_entry
Ed_SB	jmp	_ed_set_buffer_from_entry


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
; New: editor/program.cpp
		xdef	_ed_new
_ed_new
Ed_New	jmp	_ed_new_entry
		xdef	_ed_new2
_ed_new2
Ed_New2	jmp	_ed_new2_entry
		xdef	_ed_new_window
_ed_new_window
Edt_New	jmp	_ed_window_new_entry

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
		xdef	_ed_wb
_ed_wb
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
Ed_OpenLoad	jmp	_ed_open_load_entry

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

; The menu: editor/menu.cpp
EdM_End	jmp	_ed_menus_end_entry
		xdef	_ed_menu_error
_ed_menu_error
EdM_Error	jmp	_ed_menu_error_entry
EdM_Init	jmp	_ed_menu_init_entry
EdM_PrevHidden	jmp	_ed_previous_hidden_entry
EdM_NextHidden	jmp	_ed_next_hidden_entry
		xdef	_ed_branch_amos
_ed_branch_amos
EdM_BranchAMOS	jmp	_ed_branch_amos_entry
EdM_MarkAll	jmp	_ed_menu_marks_entry
Ed_ShowKey	jmp	_ed_flip_keys_entry
EdM_Editor	jmp	_ed_menus_editor_entry
		xdef	_ed_menu_program
_ed_menu_program
EdM_Program	jmp	_ed_menus_program_entry

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
; Overwrite? Save first? editor/program.cpp
		xdef	_ed_save_over
_ed_save_over
Ed_SaveOver	jmp	_ed_save_over_entry
		xdef	_ed_saved
_ed_saved
Ed_Saved	jmp	_ed_saved_entry


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
Ed_TiWait	jmp	_ed_wait_input_entry

;
; Unpack Image de l'�diteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D0=	Numero
;	D1=	X
;	D2=	Y
;
Es_Unpack	jmp	_ed_unpack_title_entry
		xdef	_ed_unpack
_ed_unpack
Ed_Unpack	jmp	_ed_unpack_entry

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
EdK_MarkAll	jmp	_ed_insert_button_entry

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
; The file selector: editor/program.cpp
		xdef	_ed_file_selector
_ed_file_selector
Ed_File_Selector	jmp	_ed_file_selector_entry


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
