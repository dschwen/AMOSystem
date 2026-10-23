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
		XREF	_ed_set_tab_entry
		XREF	_ed_slider_update_entry
		XREF	_ed_slider_draw_entry
		XREF	_ed_close_name_entry
		XREF	_ed_enlarge_fn_entry
		XREF	_ed_button_draw_entry
		XREF	_ed_button_draw_title_entry
		XREF	_ed_disk_error_entry
		XREF	_ed_load_error_entry
		XREF	_ed_print_error_fn_entry
		XREF	_ed_close_fn_entry
		XREF	_ed_workbench_entry
		XREF	_ed_cold_entry
		XREF	_ed_title_entry
		XREF	_ed_end_entry
		XREF	_ed_open_editor_entry
		XREF	_ed_kill_editor_entry
		XREF	_ed_close_editor_entry
		XREF	_ed_draw_top_fn_entry
		XREF	_ed_tokens_init_entry
		XREF	_ed_tokens_end_entry
		XREF	_ed_loop_entry
		XREF	_ed_function_call_entry
		XREF	_ed_zap_in_entry
		XREF	_ed_zap_prepare_entry
		XREF	_ed_zap_out_entry
		XREF	_ed_zap_result_entry
		XREF	_ed_zap_function_entry
		XREF	_ed_zap_new_config_entry
		XREF	_ed_zap_new_line_entry
		XREF	_ed_zap_new_line_tok_entry
		XREF	_ed_go_monitor_entry
		XREF	_ed_program_command_entry
		XREF	_ed_program_reload_entry
		XREF	_ed_window_of_name_entry
		XREF	_ed_run_hidden_entry
		XREF	_ed_run_hide_entry
		XREF	_ed_run_direct_entry
		XREF	_ed_run_entry
		XREF	_ed_error_direct_run_entry
		XREF	_ed_error_run_hidden_entry
		XREF	_ed_error_test_entry
		XREF	_ed_error_run_entry
		XREF	_ed_error_edit_entry
		XREF	_ed_error_line_entry
		XREF	_ed_test_entry
		XREF	_ed_check_13_entry
		XREF	_ed_indent_program_entry
		XREF	_ed_test_program_fn_entry
		XREF	_ed_test_shown_entry
		XREF	_ed_test_end_entry
		XREF	_ed_test_run_entry
		XREF	_ed_procedure_machine_code_entry
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
; The start, the end, the screens opened and closed: editor/startup.cpp
Ed_Cold	jmp	_ed_cold_entry
Ed_Title	jmp	_ed_title_entry
		xdef	_ed_system
_ed_system
Ed_System
	JJmp	L_TheEnd
Ed_End	jmp	_ed_end_entry
		xdef	_ed_open_editor
_ed_open_editor
Ed_OpenEditor	jmp	_ed_open_editor_entry
Ed_KillEditor	jmp	_ed_kill_editor_entry
		xdef	_ed_close_editor
_ed_close_editor
Ed_CloseEditor	jmp	_ed_close_editor_entry

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
Ed_DrawTop	jmp	_ed_draw_top_fn_entry

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
; The editor's loop: editor/loop.cpp
		xdef	_ed_loop
_ed_loop
Ed_Loop
	move.l	BasSp(a5),sp
	jmp	_ed_loop_entry


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
; A function of the editor (d2): editor/loop.cpp
		xdef	_ed_function_call
_ed_function_call
Ed_FCall	jmp	_ed_function_call_entry

; Branchement a HELP par F5
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
; Programs driving the editor: editor/loop.cpp
Ed_ZapIn
	jsr	_ed_zap_in_entry
	subq.w	#1,d0
	bmi.s	.Go
	beq	Ed_ZapOut
	bra	Ed_ZapX
.Go	move.l	BasSp(a5),sp
	jsr	_ed_zap_prepare_entry
	bsr	Ed_FCall
	bra	Ed_Loop
		xdef	_ed_zap_out
_ed_zap_out
Ed_ZapOut
	jsr	_ed_zap_out_entry
Ed_ZapX	jsr	_ed_zap_result_entry
	move.l	BasSp(a5),sp
	rts
Ed_ZapFonction	jmp	_ed_zap_function_entry
EdZ_NewConfig	jmp	_ed_zap_new_config_entry
EdZ_NewLineTok	jmp	_ed_zap_new_line_tok_entry
EdZ_NewLine	jmp	_ed_zap_new_line_entry


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
		xdef	_ed_recop
_ed_recop
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
		xdef	_ed_function_table
_ed_function_table
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
		xdef	_ed_function_flags
_ed_function_flags
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
; The tab value: editor/misc.cpp
Ed_STab	jmp	_ed_set_tab_entry

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
; The windows' sliders: editor/misc.cpp
		xdef	_ed_sl_update_y
_ed_sl_update_y
Edt_SlUpdateY	jmp	_ed_slider_update_entry
Edt_SlMinusY
Edt_SlPlusY
Edt_SlDrawY	jmp	_ed_slider_draw_entry

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
; Run, test, the programs of the menu, the returns: editor/run.cpp
Ed_GoMonitor	jmp	_ed_go_monitor_entry
Ed_PrgCommand	jmp	_ed_program_command_entry
Ed_PrgReLoad	jmp	_ed_program_reload_entry
Edt_AccAdr	jmp	_ed_window_of_name_entry
Ed_RunHidden	jmp	_ed_run_hidden_entry
Ed_RunHide	jmp	_ed_run_hide_entry
Ed_RunDirect	jmp	_ed_run_direct_entry
Ed_Run	jmp	_ed_run_entry
		xdef	_ed_err_direct_run
_ed_err_direct_run
Ed_DErrRun	jmp	_ed_error_direct_run_entry
		xdef	_ed_direct_test_message
_ed_direct_test_message
Ed_DTestMessage
	rts				0 -> Rien
	nop
	rts				1 -> Rien
	nop
	bra	Esc_Hide		2 -> Fermeture de l'ecran
		xdef	_ed_err_run_hidden
_ed_err_run_hidden
Ed_ErrRunHidden	jmp	_ed_error_run_hidden_entry
		xdef	_ed_err_test
_ed_err_test
Ed_ErrTest	jmp	_ed_error_test_entry
		xdef	_ed_err_run
_ed_err_run
Ed_ErrRun	jmp	_ed_error_run_entry
Ed_ErrEdit	jmp	_ed_error_edit_entry
Ed_GetError	jmp	_ed_get_error_entry

Ed_Ligne	jmp	_ed_error_line_entry

Ed_Test	jmp	_ed_test_entry
Ed_Check1.3	jmp	_ed_check_13_entry
Ed_Indent	jmp	_ed_indent_program_entry
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
Ed_VaTester	jmp	_ed_test_program_fn_entry
		xdef	_ed_test_message
_ed_test_message
Ed_TestMessage
	bra	Ed_Test1
	bra	Ed_Test2
	bra	Ed_Test3
Ed_Test1	jmp	_ed_test_shown_entry
Ed_Test2	jmp	_ed_test_end_entry
Ed_Test3	jmp	_ed_test_run_entry


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
Ed_ProcML	jmp	_ed_procedure_machine_code_entry

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
		xdef	_ed_not_amos
_ed_not_amos
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
		xdef	_ed_already_runned
_ed_already_runned
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
Ed_Wb	jmp	_ed_workbench_entry


; ZAP: Ferme un fenetre contenant un programme donne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_CloseName	jmp	_ed_close_name_entry

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
Ed_Enlarge	jmp	_ed_enlarge_fn_entry

;
; Copie la palette de la souris dans l'�cran A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Ecran Destination
;
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
Bt_EsCDraw	jmp	_ed_button_draw_title_entry
Bt_EdDraw	jmp	_ed_button_draw_entry

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
		xdef	_ed_close
_ed_close
Ed_Close	jmp	_ed_close_fn_entry
;
; Ouverture de l'imprimante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Ed_PRTOpen	jmp	_ed_printer_open_entry
		xdef	_ed_print_error
_ed_print_error
Ed_PErr	jmp	_ed_print_error_fn_entry
Ed_PRTPrint	jmp	_ed_printer_print_entry
;
; Erreurs dans le chargement programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ed_prg_load_err
_ed_prg_load_err
Ed_PrgLoadErr	jmp	_ed_load_error_entry
		xdef	_ed_d_error
_ed_d_error
Ed_DError	jmp	_ed_disk_error_entry

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
; The tokeniser's tables: editor/startup.cpp
Tok_Init	jmp	_ed_tokens_init_entry
Tok_Del	jmp	_ed_tokens_end_entry


; Tokenise, Detok, Mon_Detok, TInst: src/editor/tokens.cpp
Tokenise:	jmp	_ed_tokenise_entry
Mon_Detok	jmp	_ed_mon_detok_entry
Detok:		jmp	_ed_detok_entry
TInst:		jmp	_ed_tinst_entry




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
