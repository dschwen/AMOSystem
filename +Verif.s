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

Reloc_Step	equ	1024
TablA_Step	equ	1024
Reloc_End	equ	$80
Reloc_Var	equ	$82
Reloc_Long	equ	$84
Reloc_NewBuffer	equ	$86
Reloc_Proc1	equ	$88
Reloc_Proc2	equ	$8A
Reloc_Proc3	equ	$8C
Reloc_Proc4	equ	$8E
Reloc_Debug	equ	$90
Reloc_Label	equ	$92

		XREF	_ver_locals_entry
		XREF	_ver_globals_entry
		XREF	_ver_reloc_free_entry
		XREF	_ver_tables_free_entry
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

; Met le flag 1.3!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SetNot1.3
	move.b	#1,VerNot1.3(a5)
	tst.b	VerCheck1.3(a5)
	bne.s	.Stop
	rts
.Stop	moveq	#47,d0
	bra	VerErr


;	Sous programme de verification
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; The loops of the verification and the routines of the instructions:
; verif/main.cpp and the rest of verif/
SsTest	jmp	_ver_test_entry


******* MESSAGES D'ERREUR VERIFICATION
* Loops crossing / ERREUR RETARDEE
VerCrs:	move.l	a0,-(sp)
	moveq	#1,d0			Bad Structure
	bsr	ERetard
	move.l	(sp)+,a0
	rts
ERetard:tst.l	ErrRet(a5)
	bne.s	ERet1
	move.l	d0,ErrRet(a5)
	move.l	VerPos(a5),ErrRAd(a5)
ERet1:	rts
* User function
VerNFn	moveq	#2,d0
	bra	VerEr
* Impossible to change buffer
VerNoB	moveq	#3,d0
	bra	VerEr
* Datas en debut de ligne
VerDaL:	moveq	#4,d0
	bra	VerEr
* Extension not present
VerExN:	moveq	#5,d0
	bra	VerEr
* Too many direct variables
VerVTo:	moveq	#6,d0
	bra	VerEr
* Illegal direct mode
VerIlD:	moveq	#7,d0
	bra.s	VerEr
* Buffer variable too small
VerVNm:	moveq	#8,d0
	bra.s	VerEr
* Goto dans une boucle
VerPaGo:moveq	#9,d0
	bra.s	VerEr
* Structure too long
VerLong:moveq	#10,d0
	bra.s	VerEr
* Shared
VerShp:	moveq	#11,d0
	bra.s	VerEr
VerAlG:	moveq	#12,d0
	bra.s	VerEr
VerPaG:	moveq	#13,d0
	bra.s	VerEr
VerNoPa:moveq	#14,d0
	bra.s	VerEr
VerShal:moveq	#15,d0
	bra.s	VerEr
* Procedures
VerPDb:	moveq	#16,d0
	bra.s	VerEr
VerPOp:	moveq	#17,d0
	bra.s	VerEr
VerPNo:	moveq	#18,d0
	bra.s	VerEr
VerPRTy:moveq	#18,d0
	bra.s	VerEr
VerIlP:	moveq	#19,d0
	bra.s	VerEr
VerUndP:moveq	#20,d0
	bra.s	VerEr
* Else without If
VerElI:	moveq	#21,d0
VerEr:	bra	VerErr
VerIfE:	moveq	#22,d0
	bra	VerErr
VerEIf:	moveq	#23,d0
	bra	VerErr
VerElE:	moveq	#24,d0
	bra	VerErr
VerNoT:	moveq	#25,d0
	bra	VerErr
* Not enough loop
VerNoL:	moveq	#26,d0
	bra	VerErr
* Do/Loop
VerDoL:	moveq	#27,d0
	bra	VerErr
VerLDo:	moveq	#28,d0
	bra.s	VerErr
* While/Wend
VerWWn:	moveq	#29,d0
	bra.s	VerErr
VerWnW:	moveq	#30,d0
	bra.s	VerErr
* Repeat/until
VerRUn:	moveq	#31,d0
	bra.s	VerErr
VerUnR:	moveq	#32,d0
	bra.s	VerErr
* For/Next
VerFoN:	moveq	#33,d0
	bra.s	VerErr
VerNFo:	moveq	#34,d0
	bra.s	VerErr
* Syntax
VerSynt:moveq	#35,d0
	bra.s	VerErr
* Out of mem
VerOut:	moveq	#36,d0
	bra.s	VerErr
* Out of variable name space
VerNmO:	moveq	#37,d0
	bra.s	VerErr
* Non dimensionned
VerNDim	moveq	#38,d0
	bra.s	VerErr
* Already dimensionned
VerAlD:	moveq	#39,d0
	bra.s	VerErr
* Type mismatch
VerType	moveq	#40,d0
	bra.s	VerErr
* Label not defined
VerUnd:	moveq	#41,d0
	bra.s	VerErr
* Label defined twice
VerLb2:	moveq	#42,d0


; Traitement message d'erreur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ver_error
_ver_error
VerErr	move.l	d0,-(sp)
; Remet les tokens
; ~~~~~~~~~~~~~~~~
	bsr	Ver_Run
; Efface les buffers d'inclusion
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	VerPos(a5),a0
	bsr	Includes_Adr
	move.l	a0,VerPos(a5)
	bsr	Includes_Clear
; Efface les equates
; ~~~~~~~~~~~~~~~~~~
	jsr	D_Close
; Efface les tables
; ~~~~~~~~~~~~~~~~~
	bsr	Free_VerTables
; Plus de check1.3
; ~~~~~~~~~~~~~~~~
	clr.b	VerCheck1.3(a5)
; Depile le programme
; ~~~~~~~~~~~~~~~~~~~
	move.l	Prg_JError(a5),a2
	bsr	Prg_Pull
; Branche � l'appelant
; ~~~~~~~~~~~~~~~~~~~~
	move.l	(sp)+,d0
	neg.l	d0				Message negatif= TEST
	sub.l	a0,a0				Trouver le message
	jmp	(a2)				On branche


		xdef	_ver_equate
_ver_equate
Equ_Verif
	bsr	SetNot1.3		AMOSPro!
	btst	#7,5(a1)		Flag, equate correct?
	bne	.Ok
; Poke l'equate dans le buffer, � la suite du header
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	movem.l	a0-a2,-(sp)
	move.l	a1,-(sp)
	move.l	Buffer(a5),a2
.Cop	move.b	(a0)+,(a2)+
	bne.s	.Cop
	subq.l	#1,a2
	move.w	(a6),d0
	cmp.w	#_TkCh1,d0
	beq.s	.Ch
	cmp.w	#_TkCh2,d0
	bne	VerSynt
.Ch	move.w	2(a6),d0
	beq	VerSynt
	cmp.w	#127,d0
	bcc	VerSynt
	move.w	d0,d2
	move.l	a2,a1
	lea	4(a6),a0
	subq.w	#1,d0
.Lop2	move.b	(a0)+,(a1)+
	dbra	d0,.Lop2
	move.b	#":",(a1)+
	move.l	Buffer(a5),a2
	move.l	a1,d2
	sub.l	a2,d2
; Va charger le fichier d'equates>>> A1/D1 positionnes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Equ_Load
; Recherche dans le fichier
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	movem.l	a1/d1/d2,-(sp)
	moveq	#0,d4
	JJsr	L_InstrFind		***
	movem.l	(sp)+,a1/d1/d2
	tst.l	d3
	beq	.NoDef
; Trouve: poke dans le listing
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	-1(a1,d3.l),a1
.Fnd	cmp.b	#":",(a1)+		Trouve le debut
	bne.s	.Fnd
	clr.w	-(sp)
	move.l	a1,a0
	cmp.b	#"-",(a0)
	bne.s	.Pam
	addq.w	#1,(sp)
	addq.l	#1,a0
.Pam	moveq	#0,d0			Pas de signe!
	JJsrR	L_ValRout,a1
	bne.s	.Bad
	cmp.w	#_TkFl,d1
	beq.s	.Bad
	cmp.w	#_TkDFl,d1
	beq.s	.Bad
	tst.w	(sp)+
	beq.s	.Pamm
	neg.l	d0
.Pamm	move.l	(sp),a2
	move.l	d0,(a2)
	cmp.b	#",",(a0)+
	bne.s	.Bad
	move.b	(a0),d0
	sub.b	#"0",d0
	cmp.b	#7,d0
	bhi.s	.Bad
	move.b	d0,4(a2)
	bset	#7,5(a2)
	addq.l	#4,sp
	movem.l	(sp)+,a0-a2
; Saute la variable alphanumerique
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Ok	move.w	2(a6),d0
	move.w	d0,d1
	and.w	#1,d1
	add.w	d1,d0
	lea	4(a6,d0.w),a6
	rts
; Not defined!
; ~~~~~~~~~~~~
.NoDef	moveq	#51,d0
	bra	VerErr
; Bad equates file
; ~~~~~~~~~~~~~~~~
.Bad	moveq	#53,d0
	bra	VerErr

; Charge le fichier d'equates dans un buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Equ_Load
	movem.l	a0/d2/d3,-(sp)
	tst.l	Equ_Base(a5)
	bne	.Ok
; Branche la routine de FLUSH equates
	JLea	L_Equ_Free,a0
	lea	Equ_FlushStructure(pc),a1
	move.l	a0,(a1)
	SyCall	AddFlushRoutine
; Charge le fichier
	moveq	#9,d0
	JJsr	L_Sys_GetMessage
	JJsrR	L_Sys_AddPath,a1
	move.l	#1005,d2
	jsr	D_Open
	beq.s	.Err
; Trouve la taille du fichier!
	moveq	#0,d2
	moveq	#1,d3
	jsr	D_Seek
	moveq	#0,d2
	moveq	#-1,d3
	jsr	D_Seek
; Reserve la memoire
	move.l	d0,d3
	move.l	#Fast|Public,d1
	lea	Equ_Base(a5),a0
	bsr	A5_Reserve
	beq.s	.Err
; Charge le fichier
	move.l	a0,d2
	jsr	D_Read
	bne.s	.Err
; Ferme le fichier
	jsr	D_Close
; Retourne l'adresse et la longueur
.Ok	move.l	Equ_Base(a5),a1
	move.l	-4(a1),d1
	movem.l	(sp)+,a0/d2/d3
	rts
; Erreur!
.Err	jsr	D_Close
	bsr	Equ_Free
	moveq	#52,d0
	bra	VerErr
;	Structure FLUSH pour les equates
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Equ_FlushStructure
	dc.l	0
; 	Libere le fichier d'equates (ne pas bouger!)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Equ_Free
	lea	Equ_Base(a5),a0
	bsr	A5_Free
	rts

;	Procedure compilee: reloge le programme (verif/handlers.cpp)
		xdef	_ver_relocate_compiled
_ver_relocate_compiled
Ver_APCmp
	movem.l	a0-a6/d0-d7,-(sp)
	lea	2(a0),a6
	move.w	APrg_MathFlags(a6),d0	Prend les flags mathematiques
	or.b	d0,MathFlags(a5)
	lsr.w	#8,d0			Et le flag accessory...
	move.b	d0,Prg_Accessory(a5)
	lea	APrg_Relocation(a6),a1	Table de relocation
	add.l	(a1),a1
	lea	APrg_Program(a6),a2	Pointe le programme
	move.l	a2,d1
	sub.l	APrg_OldReloc(a6),d1	Ancienne relocation
	beq.s	.RFin			Deja reloge!
	move.l	a2,APrg_OldReloc(a6)
.RLoop	moveq	#0,d0
	move.b	(a1)+,d0
	beq.s	.RFin
	cmp.b	#1,d0
	bne.s	.RReloc
	lea	508(a2),a2
	bra.s	.RLoop
.RReloc	lsl.w	#1,d0
	add.l	d0,a2
	move.l	(a2),d0
	add.l	d1,(a2)
	bra.s	.RLoop
.RFin	movem.l	(sp)+,a0-a6/d0-d7
	rts

;	Procedure PHASE >0, Passe 1, stocke les variables!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



;			Verification des test, boucles et branchements
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

; Flags pour table-A
VF_Boucles	equ	0
VF_If		equ	1
VF_Proc		equ	2
VF_Exit		equ	3
VF_Goto		equ	4
VF_Debug	equ	5

; An entry of TablA (verif/verif.h Entry)
		RSRESET
Vta_Next	rs.l	1		0
Vta_Prev	rs.l	1		4
Vta_Token	rs.w	1		8
Vta_Flag	rs.b	1		10
Vta_UFlag	rs.b	1		11
Vta_Prog	rs.l	1		12
Vta_NBoucles	rs.w	1		16
Vta_PBoucles	rs.w	1		18
Vta_Jump	rs.l	1		20
Vta_Variable	equ	__RS		24

; The table of loops and structures: verif/tables.cpp
Free_TablA	jmp	_ver_tables_free_entry

;					Verification des expressions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

; Expressions and parameters: verif/expr.cpp




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	NETTOYAGES DES VARIABLES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Variables and relocation: verif/vars.cpp
Locale	jmp	_ver_locals_entry
Globale	jmp	_ver_globals_entry
Free_Reloc	jmp	_ver_reloc_free_entry

ClearVar
; - - - - - - - - - - - - -

	movem.l	d0-d7/a0-a6,-(sp)

; Variables du programme
; ~~~~~~~~~~~~~~~~~~~~~~
	lea	DebRaz(a5),a0
	lea	FinRaz(a5),a1
ClV1:	clr.w	(a0)+
	cmp.l	a1,a0
	bcs.s	ClV1
	clr.b	Test_Flags(a5)
; Initialisation du disque
; ~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#$FFFFFFFF,IffMask(a5)
	moveq	#47,d0
	JJsr	L_Sys_GetMessage
	move.l	DirFNeg(a5),a1
ClV2:	move.b	(a0)+,(a1)+
	bne.s	ClV2
	move.w	PI_DirSize(a5),DirLNom(a5)
	clr.l	T_ClLast(a5)
	move.w	#$0A0D,ChrInp(a5)
; DREG/AREG
; ~~~~~~~~~
	lea	CallReg(a5),a0
	move.l	a5,(8+5)*4(a0)			* A5-> Datazone
	move.l	T_ClAsc(a5),(8+4)*4(a0)		* A4-> Clavier actuel
	move.l	Prg_Source(a5),(8+3)*4(a0)	* A3-> Bas du programme
	move.l	T_RastPort(a5),(8+0)*4(a0)	* A0-> Rastport
	move.l	DosBase(a5),7*4(a0)		* D7-> Dos Base
	move.l	T_GfxBase(a5),6*4(a0)		* D6-> Gfx Base
	move.l	T_IntBase(a5),5*4(a0)		* D5-> Int Base
	move.l	BasSp(a5),4*4(a0)		* D4-> BasSp
	lea	Ed_Config(a5),a1		* D3-> Configuration Base Editor
	move.l	a1,3*4(a0)
; Ferme toutes les routines appellees
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Sys_ClearRoutines(a5),a1
	SyCall	CallRoutines
; Initialisations diverses
; ~~~~~~~~~~~~~~~~~~~~~~~~
	JJsr	L_Bnk.EffTemp
	JJsr	L_Bnk.Change
	JJsr	L_MenuReset
	JJsr	L_Dia_WarmInit
; Plus de buffers!
; ~~~~~~~~~~~~~~~~
	bsr	ClearBuffers
; Init float
; ~~~~~~~~~~
	move.w	#-1,FixFlg(a5)
	clr.w	ExpFlg(a5)
	movem.l	(sp)+,d0-d7/a0-a6
	rts

; 	Nettoie tous les buffers
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ClearBuffers
	moveq	#0,d1
	bsr	ResVarBuf
	clr.w	VarBufFlg(a5)
	moveq	#0,d1
	bsr	ResVNom
	clr.w	Stack_Size(a5)
	bsr	Stack_Reserve
	rts

; 	Reservation de la memoire pour la pile
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Stack_Reserve
	move.w	Stack_CSize(a5),d0
	cmp.w	Stack_Size(a5),d0
	beq.s	.Ok
	tst.w	d0
	beq.s	.NoFree
	clr.w	Stack_CSize(a5)
	addq.w	#1,d0
	mulu	#Stack_ProcSize,d0
	move.l	BaLoop(a5),a1
	SyCall	MemFree
.NoFree	move.w	Stack_Size(a5),d0
	beq.s	.Ok
	move.w	d0,Stack_CSize(a5)
	addq.w	#1,d0
	mulu	#Stack_ProcSize,d0
	move.l	d0,d1
	SyCall	MemFastClear
	beq	.Out
	move.l	a0,BaLoop(a5)
	add.l	d1,a0
	move.l	a0,HoLoop(a5)
.Ok	move.w	Stack_CSize(a5),d0
	rts
.Out	moveq	#0,d0
	rts

;	RESERVE DE L'ESPACE POUR LES VARIABLES DIRECTES
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ver_direct_variables
_ver_direct_variables
ResDir:	movem.l	d0/d1/a0,-(sp)
	tst.w	TVMax(a5)
	bne.s	RsD3
	move.l	TabBas(a5),d1
	move.l	d1,a0
	sub.l	HiChaine(a5),d1
	bcs.s	RsD3
	moveq	#0,d0
	move.w	PI_TVDirect(a5),d0
	cmp.l	d1,d0
	bls.s	RsD1
	move.l	d1,d0
RsD1:	divu	#6,d0
	mulu	#6,d0
	subq.l	#6,d0
	beq.s	RsD3
	move.l	VarLoc(a5),-(a0)
	move.w	#$FFFF,-(a0)
	sub.l	d0,a0
	move.l	a0,VarLoc(a5)
	move.l	a0,TabBas(a5)
	move.w	d0,TVMax(a5)
	lsr.w	#1,d0
	subq.w	#1,d0
	bmi.s	RsD3
RsD2:	clr.w	(a0)+
	dbra	d0,RsD2
RsD3:	movem.l	(sp)+,d0/d1/a0
	rts

;	Reserve le buffer chaines / variables
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1= Taille
		xdef	_ver_resize_variables
_ver_resize_variables
ResVarBuf
	move.l	VarBuf(a5),d0
	beq.s	Vbr1
	clr.l	VarBuf(a5)
	move.l	d0,a1
	move.l	VarBufL(a5),d0
	SyCall	MemFree
Vbr1	move.l	d1,d0
	beq.s	Vbr2
	SyCall	MemFastClear
 	beq	VerOut
	move.l	a0,VarBuf(a5)
	lea	0(a0,d1.l),a1
* Adresses dans ce buffer
	move.l	a1,LabHaut(a5)
	clr.w	-(a1)
	move.l	a1,LabBas(a5)
	clr.l	-(a1)
	move.w	#-1,-(a1)
	move.l	a1,VarGlo(a5)
	move.l	a1,TabBas(a5)
	move.l	a1,VarLoc(a5)
* Chaines
	move.l	a0,LoChaine(a5)
	move.l	a0,ChVide(a5)
	move.l	a0,ParamC(a5)
	clr.w	(a0)+
	move.l 	a0,HiChaine(a5)
	move.l	a0,LabMini(a5)
* Fini!
Vbr2	clr.w	VarLong(a5)
	clr.w	GloLong(a5)
	clr.w	TVMax(a5)
	move.l	d1,VarBufL(a5)
	rts

;	Reserve le buffer des noms de variable
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1= taille
		xdef	_ver_resize_names
_ver_resize_names
ResVNom	move.l	VNmMini(a5),d0
	beq.s	RVn1
	clr.l	VNmMini(a5)
	move.l	d0,a1
	move.l	VNmLong(a5),d0
	clr.l	VNmLong(a5)
	SyCall	MemFree
RVn1	move.l	d1,d0
	beq.s	RVn2
	SyCall	MemFastClear
 	beq	VerOut
	move.l	a0,VNmMini(a5)
	move.l	d1,VNmLong(a5)
	add.l	d1,a0
	move.l	a0,DVNmHaut(a5)
	clr.w	-(a0)
	move.l	a0,DVNmBas(a5)
	move.l	a0,VNmHaut(a5)
	clr.w	-(a0)
	move.l	a0,VNmBas(a5)
RVn2	rts

; Inclus les INCLUDES dans le programme courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ver_includes
_ver_includes
Get_Includes
; Efface d'eventuels anciens
	bsr	Includes_Clear
; Demande un buffer
	lea	Prg_Includes(a5),a0
	move.l	#20*16,d0
	move.l	#Public|Clear,d1
	bsr	A5_Reserve
	beq	.XX
	move.l	a0,a3
; Explore le d�but
	move.l	Prg_Source(a5),a0
	move.l	a0,(a3)
	moveq	#0,d6
	moveq	#0,d7
	moveq	#0,d0
	bsr	Tk_FindL
	beq	.Inclus
.Incl1	cmp.w	#_TkIncl,2(a0)
	beq.s	.Inclu
	bsr	Tk_FindN
	bne.s	.Incl1
	bra	.Inclus
; Une inclusion
.Inclu	bsr	SetNot1.3		Non compatible!
	addq.w	#1,d7
	move.l	a0,d0
	sub.l	(a3),d0
	move.l	a0,(a3)
	add.l	d0,d6
	move.l	d0,4(a3)
; Ouvre le fichier, sauve le lock
	cmp.w	#_TkCh1,4(a0)
	bne	.SErr
	lea	6(a0),a0
	move.w	(a0)+,d0
	subq.w	#1,d0
	bmi	.SErr
	cmp.w	#107,d0
	bcc	.SErr
	move.l	Name1(a5),a1
.Incl2	move.b	(a0)+,(a1)+
	dbra	d0,.Incl2
	clr.b	(a1)
	move.l	#1005,d2
	jsr	D_Open
	beq	.DErr
; Verifie entete, prend la taille du source
	move.l	Buffer(a5),d2
	moveq	#16+4,d3
	jsr	D_Read
	bne	.DErr
	move.l	d2,a2
	move.l	d2,a1			Entete AMOSPRO
	lea	H_Pro(pc),a0
	moveq	#8-1,d0
.Ver1	cmp.b	(a0)+,(a1)+
	bne	.13
	dbra	d0,.Ver1
	bra.s	.Ver3
.13	move.l	d2,a1			Entete AMOS1.3
	lea	H_1.3(pc),a0
	moveq	#10-1,d0
.Ver2	cmp.b	(a0)+,(a1)+
	bne	.AErr
	dbra	d0,.Ver2
.Ver3	add.l	16(a2),d6		Taille du source
	move.l	16(a2),12(a3)
	move.l	Handle(a5),8(a3)	Sauve le lock
	clr.l	Handle(a5)
; Reprend le cours
	move.l	(a3),a0
	bsr	Tk_FindN
	move.l	a0,d0
	sub.l	(a3),d0
	move.l	d0,16(a3)
	lea	20(a3),a3
	move.l	a0,(a3)
	bra	.Incl1
; Reserve le buffer / Charge les programmes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Inclus	move.l	a0,d0
	sub.l	(a3),d0
	move.l	a0,(a3)
	move.l	d0,4(a3)
	tst.w	d7
	beq	.PaIncl
	add.l	d6,d0				Taille du dernier incluse
	addq.l	#4,d0
	move.l	#Public,d1
	lea	Prg_FullSource(a5),a0
	bsr	A5_Reserve
	beq	.MErr
	move.l	a0,a2
	move.l	Prg_Includes(a5),a3
	move.l	Prg_Source(a5),a4
; Copie le programme
.Copy	move.l	4(a3),d0
	lsr.l	#1,d0
	beq.s	.Copf
.Cop	move.w	(a4)+,(a2)+
	subq.l	#1,d0
	bne.s	.Cop
.Copf
; Charge le chunk
	move.l	8(a3),Handle(a5)
	beq.s	.X
	move.l	a2,d2
	move.l	12(a3),d3
	jsr	D_Read
	bne	.DErr
	add.l	d0,a2
	jsr	D_Close
	clr.l	8(a3)
; Le suivant!
	add.l	16(a3),a4		Saute le INCLUDE
	lea	20(a3),a3		Suivant
	bra.s	.Copy
; Fini
.X	clr.l	(a2)+
.XX	rts
; Pas d'include
; ~~~~~~~~~~~~~
.PaIncl	bsr	Includes_Clear
	rts
; Erreur dans les includes
; ~~~~~~~~~~~~~~~~~~~~~~~~
.MErr	moveq	#36,d0
	bra.s	.Err
.DErr	moveq	#45,d0
	bra.s	.Err
.AErr	moveq	#46,d0
	bra.s	.Err
.SErr	moveq	#35,d0
.Err	move.l	d0,-(sp)
	move.l	(a3),VerPos(a5)
; Ferme tous les fichiers
	jsr	D_Close
	move.l	Prg_Includes(a5),a3
.Clo	move.l	8(a3),d0
	beq.s	.Nx
	clr.l	8(a3)
	move.l	d0,Handle(a5)
	jsr	D_Close
.Nx	lea	20(a3),a3
	subq.w	#1,d7
	bne.s	.Clo
; Efface les zones
	bsr	Includes_Clear
; Erreur!
	move.l	(sp)+,d0
	bra	VerErr

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	INCLUDES ET FIN DE PROGRAMME
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; 	Effacement des buffers includes / Retour � la normale
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Includes_Clear
	movem.l	a0-a1/d0-d1,-(sp)
	lea	Prg_FullSource(a5),a0
	bsr	A5_Free
	lea	Prg_Includes(a5),a0
	bsr	A5_Free
	movem.l	(sp)+,a0-a1/d0-d1
	rts

; 	Transforme une adresse FullSource (eventuellement) en adresse Source
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Includes_Adr
	movem.l	a1/d0-d1,-(sp)
	move.l	Prg_Includes(a5),d1
	beq.s	.Out
	move.l	d1,a1
	lea	-20(a1),a1
	sub.l	Prg_FullSource(a5),a0
	moveq	#0,d1
	moveq	#0,d2
; Chunk de source
.Loop	lea	20(a1),a1
	add.l	4(a1),d2		Longueur de source
	cmp.l	d2,a0
	bcs.s	.Source
; Chunk d'include
	tst.l	12(a1)
	beq.s	.Out
	add.l	12(a1),d1		A soustraire au source
	sub.l	16(a1),d1		Sans l'include lui meme
	add.l	12(a1),d2
	cmp.l	d2,a0
	bcc.s	.Loop
; Dans un include
	move.l	(a1),a0
	bra.s	.Out
; Dans le source
.Source	sub.l	d1,a0
	add.l	Prg_Source(a5),a0
; Sortie
.Out	movem.l	(sp)+,a1/d0-d1
	rts


		xdef	_ver_equ_lvo,_ver_equ_nul
_ver_equ_lvo
Equ_LVO	dc.b	10,"_LVO",0
_ver_equ_nul
Equ_Nul	dc.b	10,0
	even


; __________________________________________________________________________
;
; 					CHARGEMENTS / SAUVEGARDE PROGRAMMES
; __________________________________________________________________________
;
; Entete AMOS Basic
; ~~~~~~~~~~~~~~~~~
H_1.3	dc.b	"AMOS Basic v134 "
H_Pro	dc.b	"AMOS Pro101v",0,0,0,0
	even

; ___________________________________________________________________
;
; 	RUN programme general (A6)
;
;	D0=	0:Normal / 1:Accessoire / -1:PRUN
;	A1=	Adresse Errors
;	A2=	Patches lors du test
; ___________________________________________________________________
;
Prg_RunIt
	movem.l	a2-a6/d2-d7,-(sp)
	move.w	d0,d2
; Deja en route?
; ~~~~~~~~~~~~~~
	bsr	Prg_DejaRunned
	bne	.Deja
; Sauve les donnees courantes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Prg_Push
	bne	.Omm
; Va tester le programme
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	a1,Prg_JError(a5)
	bsr	Prg_SetBanks			Banques courantes
	JJsr	L_Bnk.Change			Envoie aux trappes
	clr.b	Prg_Not1.3(a6)			Non compatible au depart
	move.l	a2,d0				Premier affichage
	beq.s	.Skip1
	jsr	(a2)
.Skip1	bsr	ClearVar			Plus de variables
	bsr	PTest				Tests
	clr.b	Prg_StModif(a6)			Programme teste!
	move.b	VerNot1.3(a5),Prg_Not1.3(a6)	Stocke la comptibilite
	move.b	MathFlags(a5),Prg_MathFlags(a6)	Double precision
	move.l	a2,d0				Deuxieme affichage
	beq.s	.Skip2
	jsr	4(a2)
; Initialisation graphique
; ~~~~~~~~~~~~~~~~~~~~~~~~
.Skip2	tst.w	d2			Un accessoire
	beq.s	.Nor
	bmi.s	.PRun
; Accessoire
	tst.b	Prg_Accessory(a5)	Vraiment un accessoire?
	beq.s	.Nor
; PRun
.PRun	JJsr	L_DefRunAcc
	JJsr	L_ReCop
	JJsr	L_WOption		AMOS en premier s'il faut!
	JJmp	L_New_ChrGet
; Programme normal
.Nor	move.w	#-1,DefFlag(a5)		Non, programme normal
	JJsr	L_DefRun1
	move.l	a2,d0
	beq.s	.Skip3
	jsr	8(a2)
.Skip3	JJsr	L_DefRun2
	JJsr	L_ReCop
	EcCalD	Active,0
	clr.b	Prg_Accessory(a5)
	JJsr	L_WOption		AMOS en premier s'il faut!
	JJmp	L_New_ChrGet
; Out of memory
; ~~~~~~~~~~~~~
.Omm	moveq	#0,d0
	bra.s	.Out
; Deja runned
; ~~~~~~~~~~~
.Deja	moveq	#-1,d0
.Out	movem.l	(sp)+,a2-a6/d2-d7
	rts
; ___________________________________________________________________
;
; 	TEST programme general (A6)
;
;	A1=	Adresse Errors
;	A2=	Patches lors du test
; ___________________________________________________________________
;
Prg_TestIt
	movem.l	a2-a6/d2-d7,-(sp)
	move.w	d0,d2
; Deja en route?
; ~~~~~~~~~~~~~~
	bsr	Prg_DejaRunned
	bne.s	.Deja
; Sauve les donnees courantes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Prg_Push
	bne	.Omm
; Va tester le programme
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	a1,Prg_JError(a5)
	bsr	Prg_SetBanks
	clr.b	Prg_Not1.3(a6)			Non compatible au depart
	move.l	a2,d0				Premier affichage
	beq.s	.Skip1
	jsr	(a2)
.Skip1	bsr	ClearVar			Plus de variables
	bsr	PTest				Tests
	clr.b	Prg_StModif(a6)			Programme teste!
	move.b	VerNot1.3(a5),Prg_Not1.3(a6)	Stocke la compatibilite
	move.b	MathFlags(a5),Prg_MathFlags(a6)	Double precision
	move.l	a2,d0				Deuxieme affichage
	beq.s	.Skip2
	jsr	4(a2)
.Skip2
; Depile le programme
; ~~~~~~~~~~~~~~~~~~~
	bsr	Prg_Pull
	moveq	#0,d0
.Out	movem.l	(sp)+,a2-a6/d2-d7
	rts
; Deja runned
; ~~~~~~~~~~~
.Deja	moveq	#-1,d0
	bra.s	.Out
; Out of memory
; ~~~~~~~~~~~~~
.Omm	moveq	#0,d0
	bra.s	.Out

;
; Regarde si le programme A6 est deja RUNNE!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Prg_DejaRunned
	movem.l	a0-a1/d0-d1,-(sp)
	move.l	Prg_Runned(a5),d0
	beq.s	.Ok
.DejaL	move.l	d0,a0
	cmp.l	a0,a6
	beq.s	.Deja
	move.l	Prg_Previous(a0),d0
	bne.s	.DejaL
.Ok	moveq	#0,d0
	bra.s	.Out
.Deja	moveq	#-1,d0
.Out	movem.l	(sp)+,a0-a1/d0-d1
	rts

;
; STOCKAGE DES DONNEES DU PROGRAMME A6
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
Prg_Push
	movem.l	a0-a4/a6/d1-d7,-(sp)
	move.l	Prg_Runned(a5),d0
	move.l	a6,Prg_Runned(a5)
	move.l	d0,Prg_Previous(a6)
	beq	.Ok
; Programme en route, stocke les donn�es pour les programmes suivants
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Prg_Previous(a6),a0
	lea	Prg_RunData(a0),a0
	bsr	Prg_DataSave
	beq	.Err
; Prepare le nouveau programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Prg_DataNew
; Pas d'erreur
; ~~~~~~~~~~~~
.Ok	moveq	#0,d0
.Out	movem.l	(sp)+,a0-a4/a6/d1-d7
	rts
; Out of mem
; ~~~~~~~~~~
.Err	move.l	Prg_Previous(a6),Prg_Runned(a5)
	clr.l	Prg_Previous(a6)
	moveq	#-1,d0
	bra.s	.Out

;
; RESTAURATION DES DONNEES DU PROGRAMME POUSSE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
Prg_Pull
	movem.l	a0-a4/a6/d0-d7,-(sp)
	move.l	Prg_Runned(a5),d0
	beq.s	.Skip
	move.l	d0,a6			Prend l'ancien programme
	move.l	Prg_Previous(a6),d0	Programme precedent
	clr.l	Prg_Previous(a6)	Plus de programme precedent!
	move.l	d0,Prg_Runned(a5)	Le programme actuel
	beq.s	.Skip			Si dernier programme, on garde tout!
; Efface les variables
; ~~~~~~~~~~~~~~~~~~~~
	bsr	ClearVar
	bsr	Includes_Clear
; Efface la structure programme ancienne, si non editee...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.b	Prg_Edited(a6)
	bne.s	.Edited
	bsr	Prg_DelStructure
.Edited
; Remet les donnees
; ~~~~~~~~~~~~~~~~~
	move.l	Prg_Runned(a5),a6
	lea	Prg_RunData(a6),a0
	bsr	Prg_DataLoad
; Fini!
; ~~~~~
.Skip	movem.l	(sp)+,a0-a4/a6/d0-d7
	rts

;
; Programme en route, stocke les donn�es pour les programmes suivants
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Adresse ou poker la structure
;
Prg_DataSave
	move.l	#FinSave-DebSave+4+4+4*32,d0
	move.l	#Public|Clear,d1
	bsr	A5_Reserve
	beq	.Err
	move.l	a0,a1
; Sauve les donnees
; ~~~~~~~~~~~~~~~~~
	lea	DebSave(a5),a0
	move.w	#(FinSave-DebSave)/2-1,d0
.Copy1	move.w	(a0)+,(a1)+
	dbra	d0,.Copy1
; Sauve la liste CLEARVAR
; ~~~~~~~~~~~~~~~~~~~~~~~
	lea	Sys_ClearRoutines(a5),a2
	move.l	(a2),d0
.Loop	move.l	d0,(a1)+
	beq.s	.Out
	move.l	d0,a0
	move.l	(a0),d0
	lsl.l	#1,d0
	bra.s	.Loop
.Out
; Pas d'erreur
; ~~~~~~~~~~~~
	moveq	#-1,d0
	rts
.Err	moveq	#0,d0
	rts

;
; Remet les donn�es pour retour de programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Adresse ou prendre la structure
;
Prg_DataLoad
	move.l	a0,a2
	move.l	(a2),a0
	lea	DebSave(a5),a1
	move.w	#(FinSave-DebSave)/2-1,d0
.Copy1	move.w	(a0)+,(a1)+
	dbra	d0,.Copy1
; Remet la liste CLEARVAR
; ~~~~~~~~~~~~~~~~~~~~~~~
	lea	Sys_ClearRoutines(a5),a1
	move.l	(a0)+,d0
	move.l	d0,(a1)
	bra.s	.In
.Loop	move.l	d0,a1
	move.l	(a0)+,d0
	move.l	d0,d1
	lsr.l	#1,d1
	bset	#31,d1
	move.l	d1,(a1)
.In	tst.l	d0
	bne.s	.Loop
; Remet le moniteur s'il faut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	Cur_ChrJump(a5),d0
	JJsr	L_SetChrPatch
; Les banques ont change
; ~~~~~~~~~~~~~~~~~~~~~~
	JJsr	L_Bnk.Change
; Efface la zone de donn�es
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a2,a0
	bsr	A5_Free
	moveq	#0,d0
	rts

; Nettoie les structures interpreteur pour appel en boucle
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Prg_DataNew
	clr.l	VarBuf(a5)		Plus de buffers
	clr.l	VNmMini(a5)
	clr.w	Stack_Size(a5)
	clr.w	Stack_CSize(a5)
	clr.l	MnBase(a5)		Plus de menus
	clr.w	OMnNb(a5)
	clr.l	OMnBase(a5)
	clr.l	Prg_Includes(a5)	Plus d'includes
	clr.l	Prg_FullSource(a5)
	clr.l	Patch_Errors(a5)	Plus de moniteur
	clr.l	Patch_Menage(a5)
	clr.l	Patch_ScFront(a5)
	clr.l	Patch_ScCopy(a5)
	bclr	#1,ActuMask+1(a5)
	moveq	#0,d0
	JJsr	L_SetChrPatch
	clr.b	Ed_Zappeuse(a5)		Plus de zappeuse
	clr.b	Prg_Accessory(a5)	Plus une accessoire
	lea	Fichiers(a5),a0		Plus de fichiers
	moveq	#NFiche-1,d0
.New1	clr.l	(a0)
	lea	TFiche(a0),a0
	dbra	d0,.New1
	lea	Dev_List(a5),a0		Plus de devices (12 byte/device)
	moveq	#(3*Dev_Max)-1,d0
.New2	clr.l	(a0)+
	dbra	d0,.New2
	lea	Lib_List(a5),a0		Plus de librairies
	moveq	#Lib_Max-1,d0
.New3	clr.l	(a0)+
	dbra	d0,.New3
	rts

;
; Ouvre une structure de programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	IN	D0=	Longueur buffer
;	OUT	D0=	0(rate) Structure(reussi)
Prg_NewStructure
	movem.l	d2/a6,-(sp)
	move.l	d0,d2
; Reserve la structure
	move.l	#Prg_Long,d0
	SyCall	SyFast
	beq	.Err2
	move.l	d0,a0
; Insere dans la liste
	move.l	Prg_List(a5),Prg_Next(a0)
	move.l	a0,Prg_List(a5)
; Reserve le buffer
	move.l	a0,a6
	move.l	d2,d0
	bsr	Prg_ChgTTexte
	beq.s	.Err1
	move.l	a6,d0
	movem.l	(sp)+,d2/a6
	rts
; Rate: efface le buffer
.Err1	bsr	Prg_DelStructure
.Err2	moveq	#0,d0
	movem.l	(sp)+,d2/a6
	rts

;
; Ferme une structure de programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	IN	A6=	Adresse structure
Prg_DelStructure
	movem.l	a0-a6/d0-d7,-(sp)
; Efface le programme
	tst.l	Prg_StTTexte(a6)	Efface programme + Banques
	beq.s	.Skip
	bsr	Prg_SetBanks
	bsr	ClearVar
	JJsr	L_Bnk.EffAll		Enleve les banques
	moveq	#0,d0			Efface le buffer de texte
	bsr	Prg_ChgTTexte
; Enleve de la liste
.Skip	move.l	a6,a1			Enleve de la liste
	move.l	Prg_List(a5),d0
	cmp.l	d0,a6
	beq.s	.First
.Loop	move.l	d0,a0
	move.l	Prg_Next(a0),d0
	cmp.l	d0,a6
	bne.s	.Loop
	move.l	Prg_Next(a6),Prg_Next(a0)
	bra.s	.End
.First	move.l	Prg_Next(a6),Prg_List(a5)
; Efface la structure
.End	move.l	#Prg_Long,d0		Efface la structure
	move.l	a6,a1
	SyCall	SyFree
	movem.l	(sp)+,a0-a6/d0-d7
	rts

;
; Programme A6 >>> Programme courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Prg_ReSetBanks
	move.l	a6,-(sp)
	move.l	Prg_Runned(a5),a6
	bsr.s	Prg_SetBanks
	move.l	(sp)+,a6
	rts
Prg_SetBanks
	move.l	a0,-(sp)
	lea	Prg_Banks(a6),a0
	move.l	a0,Cur_Banks(a5)
	lea	Prg_Dialogs(a6),a0
	move.l	a0,Cur_Dialogs(a5)
	move.l	Prg_StBas(a6),Prg_Source(a5)
	move.l	(sp)+,a0
	rts

; New du program A6 - D0=0 Pas de DEFRUN 1- DEFRUN -2-DEFRUN sans ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Prg_New	move.w	d0,-(sp)
; Plus rien dans le buffer
	clr.w	Prg_NLigne(a6)
	tst.l	Prg_StTTexte(a6)
	beq.s	.Vide
	move.l	Prg_StHaut(a6),a0
	clr.w	-(a0)
	move.l	a0,Prg_StBas(a6)
; Plus de nom!
	clr.b	Prg_NamePrg(a6)
	clr.b	Prg_Change(a6)
	move.b	#1,Prg_StModif(a6)
	clr.b	Prg_MathFlags(a6)
; Raz des banques
.Vide	clr.b	T_Actualise(a5)
	bsr	Prg_SetBanks
	JJsr	L_Bnk.EffAll
	bsr	ClearVar
; Raz de l'affichage
	move.w	(sp)+,d0
	beq.s	.Skip
	move.w	d0,DefFlag(a5)
	JJsr	L_DefRun1
	JJsr	L_DefRun2
.Skip	rts

;
; Change la taille du buffer de texte
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	IN	D0=	Nouvelle taille
;		A6= 	Structure programme
;	OUT	D0=	0(rate) StBas(reussi)
Prg_ChgTTexte
	movem.l	d1/a1,-(sp)
	and.l	#$FFFFFFFE,d0
	move.l	d0,d1
	move.l	Prg_StTTexte(a6),d0
	beq.s	.Ctt1
	move.l	Prg_StMini(a6),a1
	SyCall	MemFree
	clr.l	Prg_StTTexte(a6)
.Ctt1	move.l	d1,d0
	beq.s	.Ctt2
	SyCall	MemFastClear
	beq.s	.Ctt3
	move.l	d1,Prg_StTTexte(a6)	Raz du programme
	move.l	a0,Prg_StMini(a6)
	add.l	d1,a0
	move.l	a0,Prg_StHaut(a6)
	clr.w	-(a0)
	move.l	a0,Prg_StBas(a6)
; Ok!
.Ctt2	move.l	a0,d0
.Ctt3	movem.l	(sp)+,d1/a1
	rts
;
; Charge un programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	IN	Name1=	nom du programme
;		A6	structure dans laquelle charger
;		D0=	<1: TOUJOURS adapter
;			=0: Adapter si necessaire
;			>0: Revenir si trop petit
Prg_Load
	movem.l	a2-a4/d2-d7,-(sp)
	move.w	d0,-(sp)
	move.l	#1005,d2
	jsr	D_Open
	beq	.DErr
; Verifie l'entete 1.3 ou pro
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Buffer(a5),d2
	moveq	#16+4,d3
	jsr	D_Read
	bne	.DErr
	move.l	d2,a2
	clr.b	Prg_MathFlags(a6)
; 1.3?
	clr.b	Prg_Not1.3(a6)
	lea	H_1.3(pc),a0
	move.l	d2,a1
	moveq	#10-1,d0
.Ver1	cmp.b	(a0)+,(a1)+
	bne.s	.P13
	dbra	d0,.Ver1
	moveq	#0,d7
	bra.s	.Load
; Pro?
.P13	move.b	#1,Prg_Not1.3(a6)
	lea	H_Pro(pc),a0
	move.l	d2,a1
	moveq	#8-1,d0
.Ver2	cmp.b	(a0)+,(a1)+
	bne	.PAmos
	dbra	d0,.Ver2
	move.l	d2,a1			Prend le flag maths...
	move.b	15(a1),d7
; Verifie la taille du buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Load	move.l	16(a2),d3
	move.l	d3,d1
	tst.w	(sp)
	beq.s	.Sine
	bmi.s	.2Joor
; Charge si possible
	cmp.l	Prg_StTTexte(a6),d1
	blt.s	.CBon
	add.l	#256,d1
	bra.s	.Papo
; Adapter si necessaire
.Sine		cmp.l	Prg_StTTexte(a6),d1
	blt.s	.CBon
; Adapter!
.2Joor	add.l	#256,d1
	move.l	d1,d0
	bsr	Prg_ChgTTexte
	beq.s	.MErr
.CBon	move.l	Prg_StHaut(a6),a0
	clr.w	-(a0)
	move.l	a0,d2
	sub.l	d3,d2
; Charge le fichier
; ~~~~~~~~~~~~~~~~~
	jsr	D_Read
	bne	.DErr
	move.l	d2,Prg_StBas(a6)
; Charge les banques
; ~~~~~~~~~~~~~~~~~~
	bsr	Prg_SetBanks
	move.l	#EntNul,d0
	JJsr	L_Bnk.Load
	bne	.Out
; Change le nom
; ~~~~~~~~~~~~~
	move.l	Name1(a5),a0
	lea	Prg_NamePrg(a6),a1
.Copy	move.b	(a0)+,(a1)+
	bne.s	.Copy
; Programme non modifie
; ~~~~~~~~~~~~~~~~~~~~~
	bsr	Prg_CptLines
	clr.b	Prg_Change(a6)
	move.b	d7,Prg_MathFlags(a6)
	moveq	#0,d0
	bra.s	.Out
; Erreur de disque
; ~~~~~~~~~~~~~~~~
.DErr	moveq	#-1,d0
	bra.s	.Out
; Out of mem
; ~~~~~~~~~~
.MErr	moveq	#-2,d0
	bra.s	.Out
; Pas un programme AMOS
; ~~~~~~~~~~~~~~~~~~~~~
.PAmos	moveq	#-3,d0
	bra.s	.Out
; Impossible de charger: buffer trop petit
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Papo	moveq	#1,d0
; Ferme le fichier et sort
; ~~~~~~~~~~~~~~~~~~~~~~~~
.Out	jsr	D_Close
	addq.l	#2,sp
	movem.l	(sp)+,a2-a4/d2-d7
	tst.l	d0
	rts
;
; COMPTE LE NOMBRE DE LIGNES
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
Prg_CptLines
	move.l	Prg_StBas(a6),a0
	moveq	#0,d0
	moveq	#0,d1
CpL1	add.w	d1,a0
	add.w	d1,a0
CpL2	move.b	(a0),d1
	beq.s	CpLx
	addq.w	#1,d0
	cmp.w	#_TkProc,2(a0)
	bne.s	CpL1
; Saute une procedure fermee
	tst.b	10(a0)
	bpl.s	CpL1
	add.l	4(a0),a0
	moveq	#(10+2+2)/2,d1
	bra.s	CpL1
; Nombre de lignes
CpLx	move.w	d0,Prg_NLigne(a6)
	rts

;
; TROUVE L'ADRESSE DU PROGRAMME NAME1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	In	Name1	Nom � cherche
;	Out	Trouve	BNE, A0=adresse
;		Non	BEQ
;
Prg_AccAdr
	movem.l	a2-a3/d2,-(sp)
	move.l	Name1(a5),a0
	JJsrR	L_Dsk.DNom,a3
	move.l	a0,d2
	move.l	Prg_List(a5),d0
	beq.s	.Out
.Loop0	move.l	d0,a2
	lea	Prg_NamePrg(a2),a0
	JJsrR	L_Dsk.DNom,a3
	move.l	d2,a1
.Loop1	move.b	(a0)+,d0
	bsr	MajD0
	move.b	d0,d1
	move.b	(a1)+,d0
	bsr	MajD0
	cmp.b	d0,d1
	bne.s	.Next
	or.b	d1,d0
	bne.s	.Loop1
	move.l	a2,d0
	bra.s	.Ok
.Next	move.l	Prg_Next(a2),d0
	bne.s	.Loop0
.Out	moveq	#0,d0
.Ok	move.l	d0,a0
	movem.l	(sp)+,a2-a3/d2
	rts
MajD0	cmp.b	#"a",d0
	bcs.s	.Ski
	cmp.b	#"z",d0
	bhi.s	.Ski
	sub.b	#$20,d0
.Ski	rts

;
; Sauve un programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	IN	Name1=	nom du programme
;		A6	structure � sauver
Prg_Save
	movem.l	a2/d2-d4,-(sp)
	move.l	#1006,d2
	jsr	D_Open
	beq	.Err
; Sauve le header du programme: PRO / 1.3
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	H_Pro(pc),a0			Header PRO
	move.b	Prg_MathFlags(a6),15(a0)	Double precision
	tst.b	Prg_Not1.3(a6)
	bne.s	.N13
	lea	H_1.3(pc),a0
.N13	moveq	#"V",d0			Teste!
	tst.b	Prg_StModif(a6)
	beq.s	.Skip
	moveq	#"v",d0			Non teste!
.Skip	move.b	d0,11(a0)		Met la marque
	move.l	a0,d2
	moveq	#16,d3
	jsr	D_Write
	bne	.Err
; Trouve la fin REELLE du programme (pas les zeros de fin!)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Prg_StBas(a6),a0
.Loop	bsr	Tk_FindN		Explore le programme
	bne.s	.Loop
.Fin	move.l	a0,d4			Stoppe sur le premier zero
; Sauve la taille PRG
; ~~~~~~~~~~~~~~~~~~~
	move.l	Buffer(a5),a0
	move.l	d4,d0
	sub.l	Prg_StBas(a6),d0
	move.l	d0,(a0)
	move.l	a0,d2
	moveq	#4,d3
	jsr	D_Write
	bne	.Err
; Sauve le corps du programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Prg_StBas(a6),d2
	move.l	d4,d3
	sub.l	d2,d3
	jsr	D_Write
	bne	.Err
; Sauve les banques....
; ~~~~~~~~~~~~~~~~~~~~~
	bsr	Prg_SetBanks
	JJsr	L_Bnk.SaveAll
	bne	.Err
; Fin de la sauvegarde
; ~~~~~~~~~~~~~~~~~~~~
	jsr	D_Close
; Change le nom
; ~~~~~~~~~~~~~
	move.l	Name1(a5),a0
	lea	Prg_NamePrg(a6),a1
.Copy	move.b	(a0)+,(a1)+
	bne.s	.Copy
; Pas d'erreur
; ~~~~~~~~~~~~
	clr.b	Prg_Change(a6)
	moveq	#0,d0
	bra.s	.Out
.Err	moveq	#-1,d0
.Out	movem.l	(sp)+,a2/d2-d4
	rts


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
	moveq	#1,d0
Tk_FindL
	jmp	_tk_find_line
Tk_SizeL
	jmp	_tk_size_line
Tk_EditL
	jmp	_tk_editable
Tk_FindA
	jmp	_tk_find_address

;				Codage / Decodage procedure LOCKEE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; 	A6---> "PROC"
		xdef	_ver_decode_procedure
_ver_decode_procedure
ProCode	movem.l	d0-d7/a0-a6,-(sp)
	btst	#4,8(a6)		* Flag COMPILED?
	bne	PaCo
	move.l	2(a6),d0
	lea	10+2+4(a6,d0.l),a2	* A2---> ENDPROC
	move.w	-2(a6),d0
	lsr.w	#8,d0
	lsl.w	#1,d0
	lea	-2(a6,d0.w),a1		* A1---> Ligne suivante
	move.l	2(a6),d5
	rol.l	#8,d5
	move.b	9(a6),d5
	moveq	#1,d4
	move.w	6(a6),d3
	bra.s	PrCo2
PrCo1	eor.w	d5,(a0)+
	add.w	d4,d5
	add.w	d3,d4
	ror.l	#1,d5
	cmp.l	a0,a1
	bne.s	PrCo1
PrCo2	move.l	a1,a0
	move.w	(a0)+,d0
	lsr.w	#8,d0
	lsl.w	#1,d0
	lea	-2(a0,d0.w),a1
	addq.l	#2,a0
	cmp.l	a0,a2
	bne.s	PrCo1
* Change le flag
	bchg	#5,8(a6)
PaCo	movem.l	(sp)+,d0-d7/a0-a6
	rts

;				Echange des tables de tokenisation
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_ver_tables_verify
_ver_tables_verify
Ver_Verif
	moveq	#-1,d3
	bra.s	Ver_Echange
		xdef	_ver_tables_run
_ver_tables_run
Ver_Run	moveq	#0,d3
Ver_Echange
	lea	AdTokens(a5),a2
	moveq	#27-1,d2
.Loop	tst.l	(a2)
	beq.s	.Next
	move.l	(a2),a1
	tst.l	LB_Verif(a1)		Une table?
	beq.s	.Next
	tst.w	d3			Run (0)  ou Verif (1)
	bne.s	.Verif
; Run!
	bclr	#LBF_Verif,LB_Flags(a1)		Deja RUN?
	beq	.Next
	bsr.s	Ver_Ech			On echange
	bra.s	.Next
; Verif!
.Verif	bset	#LBF_Verif,LB_Flags(a1)		Deja VERIF?
	bne	.Next
	bsr.s	Ver_Ech
; Table suivante
.Next	addq.l	#4,a2
	dbra	d2,.Loop
	rts
; Echange des tables
; ~~~~~~~~~~~~~~~~~~
Ver_Ech	move.l	a1,a0			Debut des tokens
	move.l	LB_Verif(a0),a1		Adresse table
	move.w	(a1)+,d1		Longueur table
	ext.l	d1
	add.l	a1,d1			Fin table
.Loop	move.l	(a0),d0
	move.l	(a1),(a0)+
	move.l	d0,(a1)+
.Skip1	tst.b	(a0)+
	bpl.s	.Skip1
.Skip2	tst.b	(a0)+
	bpl.s	.Skip2
	move.w	a0,d0
	and.w	#$0001,d0
	add.w	d0,a0
	cmp.l	d1,a1
	bcs.s	.Loop
	rts


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
