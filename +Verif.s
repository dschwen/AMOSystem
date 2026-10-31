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
; ...2220002.220000 2....220002...22.....200002..0000000000002...................
; ...220000..222000002...20000..........200000......2222........................
; ...000000000000000000..200000..........00002..................................
; ..220000000022020000002.200002.........22.......______________________________
; ..0000002........2000000220022.................|
; .200000............2002........................| GESTION SOURCE
; .200002........................................| CHARGEMENT / TEST / VERIF
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

		XREF	_ver_test_entry
		XREF	_ver_direct_test_entry
		XREF	_ver_program_test_entry
		XREF	_ver_free_tables_entry

;_____________________________________________________________________________
;
;						Test en mode direct
;_____________________________________________________________________________
;
; The tests of a line of direct mode, of the program: verif/main.cpp
VerDirect
Ver_Direct	jmp	_ver_direct_test_entry
PTest	jmp	_ver_program_test_entry
Free_VerTables	jmp	_ver_free_tables_entry

;	Sous programme de verification
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; The loops of the verification and the routines of the instructions:
; verif/main.cpp and the rest of verif/
SsTest	jmp	_ver_test_entry


; Equates and compiled procedures: verif/includes.cpp
Equ_Free	jmp	_ver_equ_free_entry

		RSRESET

; The buffers: verif/buffers.cpp; the includes: verif/includes.cpp
ClearVar	jmp	_ver_clear_variables_entry
Stack_Reserve	jmp	_ver_stack_reserve_entry
ResDir		jmp	_ver_direct_variables_entry
ResVarBuf	jmp	_ver_resize_variables_entry
ResVNom		jmp	_ver_resize_names_entry
Includes_Clear	jmp	_ver_includes_clear_entry
Includes_Adr	jmp	_ver_includes_address_entry


		xdef	_ver_equ_lvo,_ver_equ_nul
_ver_equ_lvo
Equ_LVO	dc.b	10,"_LVO",0
_ver_equ_nul
Equ_Nul	dc.b	10,0
	even


; ___________________________________________________________________
;
; The programs in memory: verif/program.cpp. Prg_RunIt ends with the
; program: its C++ part makes it ready (d0 = 1), then the ChrGet starts.
; ___________________________________________________________________
;
Prg_RunIt
	movem.l	a2-a6/d2-d7,-(sp)
	jsr	_ver_prg_run_it_entry
	bgt.s	.Go
	movem.l	(sp)+,a2-a6/d2-d7
	tst.l	d0
	rts
.Go	JJmp	L_New_ChrGet
Prg_TestIt	jmp	_ver_prg_test_it_entry
Prg_DejaRunned	jmp	_ver_prg_deja_runned_entry
Prg_Push	jmp	_ver_prg_push_entry
Prg_Pull	jmp	_ver_prg_pull_entry
Prg_DataSave	jmp	_ver_prg_data_save_entry
Prg_DataLoad	jmp	_ver_prg_data_load_entry
Prg_DataNew	jmp	_ver_prg_data_new_entry
Prg_NewStructure jmp	_ver_prg_new_structure_entry
Prg_DelStructure jmp	_ver_prg_del_structure_entry
Prg_ReSetBanks	jmp	_ver_prg_reset_banks_entry
Prg_SetBanks	jmp	_ver_prg_set_banks_entry
Prg_New		jmp	_ver_prg_new_entry
Prg_ChgTTexte	jmp	_ver_prg_chg_ttexte_entry
Prg_Load	jmp	_ver_prg_load_entry
Prg_CptLines	jmp	_ver_prg_cpt_lines_entry
Prg_AccAdr	jmp	_ver_prg_acc_adr_entry
Prg_Save	jmp	_ver_prg_save_entry


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES GESTION SOURCE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; FIND_LINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; 	Trouve la ligne 	D0
;	Debut source		A0
;	Debut Proc >>>		A1
; Tk_FindN / Tk_FindL, Tk_SizeL, Tk_EditL, Tk_FindA: src/core/source.cpp
Tk_FindN
	jmp	_tk_find_first
Tk_FindL
	jmp	_tk_find_line
Tk_SizeL
	jmp	_tk_size_line
Tk_EditL
	jmp	_tk_editable
Tk_FindA
	jmp	_tk_find_address

; Locked procedures: verif/handlers.cpp (decode_procedure). The token
; tables swapped for verifying / running: verif/main.cpp (tables_swap)
Ver_Run		jmp	_ver_tables_run_entry


;						Table des operateurs / Test
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tst_Operateurs
	dc.w	0,0
	dc.b 	" xor"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	" or"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	" and"," "+$80,"O00",-1
	dc.w	0,1
	dc.b 	"<",">"+$80,"O20",-1
	dc.w	0,1
	dc.b 	">","<"+$80,"O20",-1
	dc.w	0,1
	dc.b 	"<","="+$80,"O20",-1
	dc.w	0,1
	dc.b 	"=","<"+$80,"O20",-1
	dc.w	0,1
	dc.b 	">","="+$80,"O20",-1
	dc.w	0,1
	dc.b 	"=",">"+$80,"O20",-1
	dc.w	0,1
	dc.b 	"="+$80,"O20",-1
	dc.w	0,1
	dc.b 	"<"+$80,"O20",-1
	dc.w	0,1
	dc.b 	">"+$80,"O20",-1
	dc.w	0,2
	dc.b 	"+"+$80,"O22",-1
	dc.w	0,2
	dc.b 	"-"+$80,"O22",-1
	dc.w	0,0
	dc.b 	" mod"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	"*"+$80,"O00",-1
	dc.w	0,0
	dc.b 	"/"+$80,"O00",-1
	dc.w	0,3
	dc.b 	"^"+$80,"O00",-1
	even
		xdef	_ver_operators
_ver_operators
Tst_Jumps
	dc.l 	0
