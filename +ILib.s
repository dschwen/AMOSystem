
;---------------------------------------------------------------------
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
;---------------------------------------------------------------------

;---------------------------------------------------------------------
	Lib_Ini	0
;---------------------------------------------------------------------

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INITIALISATION LIBRAIRIE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Init
Init
; - - - - - - - - - - - - -
	move.w	#$00FF,d0		Librairie numero 0
	move.w	#Ver_Number,d1
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES A NETTOYER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; 	Routines de chargement
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Int	Edit_Load
	Lib_Int	Edit_Free
	Lib_Int	Mon_Load
	Lib_Int	Mon_Free

;	Entrees dans le moniteur (5 places)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Def	Mon_Start
	Lib_Def	Mon_In_Editor
	Lib_Def	Mon_In_Program
	Lib_Def	Mon_MonitorChr
	Lib_Def	Mon_Free2
	Lib_Def	Mon_Free3

; 	Entrees dans l'editeur (15 places)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Def	Ed_Start
	Lib_Def	Ed_Cold				0
	Lib_Def	Ed_Title			1
	Lib_Def	Ed_End				2
	Lib_Def	Ed_Loop				3
	Lib_Def	Ed_ErrRun			4
	Lib_Def	Ed_CloseEditor			5
	Lib_Def	Ed_KillEditor			6
	Lib_Def	Ed_ZapFonction			7
	Lib_Def	Ed_ZapIn			8
	Lib_Def	Ed_RunDirect			9
	Lib_Def	Tokenise			10
	Lib_Def	Detok				11
	Lib_Def	Mon_Detok			12
	Lib_Def	TInst				13
	Lib_Def	Ed_Free1			14
	Lib_Def	Ed_Free2			15
	Lib_Def	Ed_Free3			16
	Lib_Def	Ed_Free1			17
	Lib_Def	Ed_Free2			18

; 	Routines dans la verification
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Int	Tk_FindA
	Lib_Int	Tk_EditL
	Lib_Int	Tk_FindL
	Lib_Int	Tk_FindN
	Lib_Int	Tk_SizeL
	Lib_Int	Prg_RunIt
	Lib_Int	Prg_TestIt
	Lib_Int	Prg_Save
	Lib_Int	Prg_Load
	Lib_Int	Prg_New
	Lib_Int	Prg_NewStructure
	Lib_Int	Prg_DelStructure
	Lib_Int	Prg_AccAdr
	Lib_Int	Prg_DejaRunned
	Lib_Int	Prg_DataLoad
	Lib_Int	Prg_DataSave
	Lib_Int	Prg_DataNew
	Lib_Int	Prg_CptLines
	Lib_Int	Prg_ChgTTexte
	Lib_Int	Prg_SetBanks
	Lib_Int	Prg_ReSetBanks
	Lib_Int	Prg_SetBanks
	Lib_Int	Prg_Pull
	Lib_Int	ClearVar
	Lib_Int	PTest
	Lib_Int	SsTest
	Lib_Int	ResVNom
	Lib_Int	ResDir
	Lib_Int	ResVarBuf
	Lib_Int	VerDirect
	Lib_Int	Stack_Reserve
	Lib_Int	Includes_Clear
	Lib_Int	Includes_Adr
	Lib_Int	Equ_Free

; 	Routines internes � "+b.s"
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Int	RamFast			Ram Access
	Lib_Int	RamFast2
	Lib_Int	RamChip
	Lib_Int	RamChip2
	Lib_Int	RamFree
	Lib_Int	ResTempBuffer

	Lib_Int	Math_Close
	Lib_Int	Sys_WaitMul
	Lib_Int	Def_GetMessage
	Lib_Int	Sys_GetMessage
	Lib_Int	GetMessage
	Lib_Int	Sys_AddPath
	Lib_Def	Sys_GetPath		Pour le compilateur!
	Lib_Int	Sys_UnCode
	Lib_Int	MemMaximum
	Lib_Int	MemDelBanks
	Lib_Int	TheEnd
	Lib_Int	UserReg
	Lib_Int	VersionN
	Lib_Int	BugBug
	Lib_Int	PreBug
	Lib_Int	Sys_ClearCache
	Lib_Int	WOption
	Lib_Int	ReCop

	Lib_Int	Lst.ChipNew		Gestion des listes
	Lib_Int	Lst.New
	Lib_Int	Lst.Cree
	Lib_Int	Lst.DelAll
	Lib_Int	Lst.Del
	Lib_Int	Lst.Insert
	Lib_Int	Lst.Remove
	Lib_Int	Bnk.PrevProgram
	Lib_Int	Bnk.CurProgram
	Lib_Int	AskDir
	Lib_Int	AskDir2

;	Special compilateur
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Def	Tokenisation		Routines pour APCMP
	Lib_Def	Testing
	Lib_Def	CValRout

	Lib_Def	CmpInit1		Routines d'initialisation
	Lib_Def	CmpInit2
	Lib_Def	AMOSInit
	Lib_Def	CmpDbMode
	Lib_Def	CmpLineCLI
	Lib_Def	CmpLineSER
	Lib_Def	CmpPrintCLI
	Lib_Def	CmpPrintSER
	Lib_Def	CmpEffVarBuf
	Lib_Def	CmpLibrariesInit
	Lib_Def	CmpLibrariesStop
	Lib_Def	CmpEndRoutines
	Lib_Def	CmpLibClose
	Lib_Def	CmpClearVar

	Lib_Int	FindVar			interp/vars.cpp (the compiler's PlusF before)
	Lib_Def	PlusC
	Lib_Def	MoinsF
	Lib_Def	MoinsC
	Lib_Def	MultE
	Lib_Def	MultF
	Lib_Def	DiviseE
	Lib_Def	DiviseF
	Lib_Def	Puissance
	Lib_Def	Modulo
	Lib_Def	Chaine_Compare

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEFRUN: initialisation graphique
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	DefRun1		src/interp/defrun.cpp

; - - - - - - - - - - - - -
	Lib_Int	DefRun2		src/interp/defrun.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEFRUNACC: semi initialisation graphique pour accessoires
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	DefRunAcc		src/interp/defrun.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	INITIALISATION ECRAN DES EXTENSIONS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	DefRunExtensions		src/interp/defrun.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Set Patch sur CHRGET
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	SetChrPatch
; - - - - - - - - - - - - -
	move.w	d0,Cur_ChrJump(a5)
	lea	_Poke(pc),a1
	tst.w	d0
	beq.s	.Norm
	move.w	.Mon(pc),(a1)+
	lsl.w	#2,d0
	neg.w	d0
	sub.w	#LB_Size+4,d0
	move.w	d0,(a1)+
	bra.s	.Clear
.Norm	move.l	.Chr(pc),(a1)+
.Clear	Rjsr	L_Sys_ClearCache
	rts
.Chr	move.l	-LB_Size(a4,d1.w),a0
.Mon	move.l	0(a4),a0

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 						ENTREE DU CHRGET
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Pos_IRet
; - - - - - - - - - - - - -
	dc.w	_IRet-_DChr
; - - - - - - - - - - - - -
	Lib_Def	New_ChrGet
; - - - - - - - - - - - - - - -
_DChr	move.l	BasSp(a5),sp
	move.l	AdTokens(a5),a4

; Adresse du retour d'instructions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	_IRet(pc),a0
	move.l	a0,Prg_InsRet(a5)
; Buffer des boucles
; ~~~~~~~~~~~~~~~~~~
	Rjsrt	L_Stack_Reserve
;	beq	VerOut				*** Illegal!
	move.l	HoLoop(a5),a3
	move.l	a3,PLoop(a5)
	move.l	a3,BasA3(a5)
	move.l	BaLoop(a5),MinLoop(a5)
	add.l	#64,MinLoop(a5)

; Ouverture des librairies mathematiques / Choix des routines dans la librarie
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Rjsr	L_Open_MathLibraries

; Autres inits
; ~~~~~~~~~~~~
	clr.w	T_Actualise(a5)
	clr.l	AData(a5)
	move.w	#1,DefFlag(a5)
	move.l	Prg_Run(a5),a6
	move.l	a6,DProc(a5)
	move.l	a6,PData(a5)

; Debuggage
	IFNE	Debug>1
	move.l	VarBufL(a5),d0
	cmp.l	#1024*1,d0
	bne.s	.NoBug
	Rjsr	L_PreBug
.NoBug
	ENDC

;	Boucle du ChrGet
; ~~~~~~~~~~~~~~~~~~~~~~
	move.l	AdTokens(a5),a4
_NLine	tst.w	(a6)+
	beq.s	InEnd
_ILoop	move.w	(a6)+,d0
	beq.s	_NLine
_Inst	move.l	a6,d7
	move.w	0(a4,d0.w),d1		Pointe la table de tokens
_Poke	move.l	-LB_Size(a4,d1.w),a0
	IFNE	Debug
	move.l	a3,Chr_Debug(a5)
	movem.l	d6/d7,Chr_Debug+4(a5)
	ENDC
	jsr	(a0)
_IRet
	IFNE	Debug>1
	move.l	Chr_Debug(a5),a0
	movem.l	Chr_Debug+4(a5),d0/d1
 	cmp.l	a0,a3			Test valide PLOOP
	bne.s	.Bug
	cmp.l	d0,d6			Test sauvegarde D6/D7
	bne.s	.Bug
	cmp.l	d1,d7
	beq.s	.Skip
.Bug	Rjsr	L_BugBug
	Rjsr	L_InMonitor
	ENDC
	IFNE	Debug
	move.l	Chr_Debug(a5),a0
	movem.l	Chr_Debug+4(a5),d0/d1
 	cmp.l	a0,a3			Test valide PLOOP
	bne.s	.Bug1
	cmp.l	d0,d6			Test sauvegarde D6/D7
	bne.s	.Bug1
	cmp.l	d1,d7
	beq.s	.Skip
.Bug1	Rjsr	L_InMonitor
	ENDC
.Skip
	move.w	(a6)+,d0
	bne.s	_Inst
	tst.w	(a6)+
	bne.s	_ILoop
	bra.s	InEnd

; - - - - - - - - - - - - -
	Lib_Par InEnd
; - - - - - - - - - - - - -
InEnd	moveq	#NbEnd,d0
	bra	RunErr


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	EXTENSION INSTRUCTION CALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InExtCall		src/interp/extensions.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	EXTENSION FUNCTION CALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnExtCall		src/interp/extensions.cpp

;	Ouverture / Echange des routines mathemetiques
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Lib_Int	Open_MathLibraries	src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	INTERPRETEUR SEUL: recuperation des parametres...
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Parameters
; - - - - - - - - - - - - -
Par	dc.w	Ent_0-Par		0>>> Entier normal
	dc.w	Ent_1-Par		1
	dc.w	Ent_2-Par		2
	dc.w	Ent_3-Par		3
	dc.w	Ent_4-Par		4
	dc.w	Ent_N-Par		5
	dc.w	Flt_0-Par		6>>> Float normal
	dc.w	Flt_1-Par		7
	dc.w	Flt_2-Par		8
	dc.w	Flt_N-Par		9
	dc.w	Flt_N-Par		10
	dc.w	Flt_N-Par		11
	dc.w	IVar_0-Par		12>> Variable reservee
	dc.w	IVar_1-Par		13
	dc.w	IVar_2-Par		14
	dc.w	IVar_3-Par		15
	dc.w	IVar_4-Par		16
	dc.w	IVar_N-Par		17
	dc.w	Par_Angle-Par		18>> Fonction angle
	dc.w	Par_Math-Par		19>> Fonction math
; Si extensions (D2= nombre de params)
	dc.w	Ent_0-Par		0>>> Entier normal
	dc.w	Ent_1-Par		1
	dc.w	Ent_2-Par		2
	dc.w	Ent_3-Par		3
	dc.w	Ent_4-Par		4
	dc.w	EEnt_N-Par		5
	dc.w	Flt_0-Par		6>>> Float normal
	dc.w	Flt_1-Par		7
	dc.w	Flt_2-Par		8
	dc.w	EFlt_N-Par		9
	dc.w	EFlt_N-Par		10
	dc.w	EFlt_N-Par		11
	dc.w	IVar_0-Par		12>> Variable reservee
	dc.w	IVar_1-Par		13
	dc.w	IVar_2-Par		14
	dc.w	IVar_3-Par		15
	dc.w	IVar_4-Par		16
	dc.w	EIVar_N-Par		17
	dc.w	Par_Angle-Par		18>> Fonction angle
	dc.w	Par_Math-Par		19>> Fonction math

; 	Variable reserve en instruction, 1 param
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EIVar_N
	bsr	EFEnt_N
	bra.s	IVar
IVar_N	bsr	FEnt_N
	bra.s	IVar
IVar_4	bsr.s	FEnt_4
	bra.s	IVar
IVar_3	bsr.s	FEnt_3
	bra.s	IVar
IVar_2	bsr.s	FEnt_2
	bra.s	IVar
IVar_1	bsr.s	FEnt_1
IVar	move.l	d3,-(a3)
IVar_0	bsr	Fn_New_Evalue		Recolte du parametre a affecter
	cmp.b	#1,d2
	beq.s	.Ent
	rts
.Ent	Rjmpt	L_FlToInt1

;	QUATRE parametres ENTIER / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FEnt_4	addq.l	#2,a6
Ent_4	bsr	New_Evalue
	cmp.b	#1,d2
	bne.s	.Ent
	Rjsrt	L_FlToInt1
.Ent	move.l	d3,-(a3)
;	TROIS parametres ENTIER / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FEnt_3	addq.l	#2,a6
Ent_3	bsr	New_Evalue
	cmp.b	#1,d2
	bne.s	.Ent
	Rjsrt	L_FlToInt1
.Ent	move.l	d3,-(a3)
;	DEUX parametres ENTIER / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FEnt_2	addq.l	#2,a6
Ent_2	bsr	New_Evalue
	cmp.b	#1,d2
	bne.s	.Ent
	Rjsrt	L_FlToInt1
.Ent	move.l	d3,-(a3)
;	UN parametre ENTIER / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FEnt_1	addq.l	#2,a6
Ent_1	bsr	New_Evalue
	cmp.b	#1,d2
	beq.s	Ent
Ent_0	rts
Ent	Rjmpt	L_FlToInt1

; 	Fonction ANGLE, toujours float, appel FAngle
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Par_Angle
	bsr	Fn_New_Evalue
	tst.b	d2
	bne.s	.Flaot
	Rjsrt	L_IntToFl1
.Flaot	Rjmpt	L_FFAngle

; 	Fonction mathematique: 1 param, branchement >entier >float
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Par_Math
	addq.l	#4,sp
	move.w	d1,-(sp)
	bsr	Fn_New_Evalue
	move.w	(sp)+,d0
	tst.b	d2
	bne.s	.Float
	move.l	-LB_Size-4(a4,d0.w),a0		Saute l'appel params
	jmp	4(a0)
.Float	move.l	-LB_Size-8(a4,d0.w),a0		Pas d'appel params ici...
	jmp	(a0)

; 	N Parametres FLOAT / CHAINES
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EFFlt_N	addq.l	#2,a6
EFlt_N	move.w	d2,-(sp)
	bra.s	SOne
FFlt_N	addq.l	#2,a6
Flt_N	move.w	2(a4,d0.w),-(sp)
	bra.s	SOne
SLoop	move.l	d3,-(a3)
	addq.l	#2,a6
SOne	bsr	New_Evalue
	tst.b	d2
	bne.s	SOk
	Rjsrt	L_IntToFl1
SOk	subq.w	#1,(sp)
	bne.s	SLoop
	rtr
;	DEUX parametres FLOAT / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FFlt_2	addq.l	#2,a6
Flt_2	bsr	New_Evalue
	tst.b	d2
	bne.s	.Ent
	Rjmpt	L_IntToFl1
.Ent	move.l	d3,-(a3)
;	UN parametre FLOAT / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FFlt_1	addq.l	#2,a6
Flt_1	bsr	New_Evalue
	tst.b	d2
	beq.s	Flt
Flt_0	rts
Flt	Rjmpt	L_IntToFl1

; 	N parametres ENTIERS / CHAINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EFEnt_N	addq.l	#2,a6
EEnt_N	move.w	d2,-(sp)
	bra.s	_POne
FEnt_N	addq.l	#2,a6
Ent_N	move.w	2(a4,d0.w),-(sp)
	bra.s	_POne
_PLoop	move.l	d3,-(a3)
	addq.l	#2,a6
_POne	bsr	New_Evalue
	cmp.b	#1,d2
	bne.s	_POk
	Rjsrt	L_FlToInt1
_POk	subq.w	#1,(sp)
	bne.s	_PLoop
	rtr

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TEST INTER SANS SAUT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Test_PaSaut		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TEST NORMAL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Test_Normal		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TEST FORCE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Test_Force		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Fait le branchement ON MENU
;					Appele par TESTS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			GoMenu, now in src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ON BREAK : Fait le branchement en cas de BREAK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			OnBreakGo, now in src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Fait le branchement a EVERY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			EveJump, now in src/interp/control.cpp



; - - - - - - - - - - - - -
	Lib_Def	RIllDir
RIllDir
; - - - - - - - - - - - - -
	moveq	#17,d0
	bra	RunErr
	Lib_Empty			OOfData, now an error() in src/interp
	Lib_Empty			OOfBuf, now an error() in src/interp
	Lib_Empty			InpTL, now an error() in src/interp
	Lib_Empty			EProErr, now an error() in src/interp
	Lib_Empty			ResLNo, now an error() in src/interp
	Lib_Empty			NoOnErr, now an error() in src/interp
	Lib_Empty			ResPLab, now an error() in src/interp
	Lib_Empty			NoResume, now an error() in src/interp
	Lib_Empty			NoErr, now an error() in src/interp
	Lib_Empty			OofStack, now an error() in src/interp
	Lib_Empty			NonDim, now an error() in src/interp
	Lib_Empty			AlrDim, now an error() in src/interp
	Lib_Empty			DByZero, now an error() in src/interp
	Lib_Empty			OverFlow, now an error() in src/interp
	Lib_Empty			RetGsb, now an error() in src/interp
	Lib_Empty			PopGsb, now an error() in src/interp
; The errors: interp/control.cpp
RunErr:	moveq	#19,d1
	moveq	#-1,d2
RunErrExt
	Rjmp	L_ErrorExt
	Lib_Int	Error
	Lib_Int	ErrorExt




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEBUT DES ROUTINES AMOS / CLI
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Start_Type
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	||APCMP|| appel de procedure compilee
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	InAPCmp
; - - - - - - - - - - - - -
	jsr	APrg_Program(a6)	Appelle le programme
	move.l	AdTokens(a5),a4		Recharge a4
	tst.l	d0
	bmi	RunII			-1 >>> deuxieme partie de RUN
	bne.s	.Err			Si erreur >>> BREAK!
	lea	APrg_EndProc(a6),a6	Pointe le delta a la fin
	add.l	(a6),a6
	IFNE	Debug
	move.l	a3,Chr_Debug(a5)
	movem.l	d6/d7,Chr_Debug+4(a5)
	ENDC
	rts
.Err	move.l	#512,d1			Force l'arret!
	moveq	#-1,d2
	Rjmp	L_ErrorExt

; - - - - - - - - - - - - -
	Lib_Def	InAPCmpCLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	INSTRUCTION RUN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InRun0
; - - - - - - - - - - - - -
	tst.w	Direct(a5)
	beq	Synt
	Rjmpt	L_Ed_RunDirect
; - - - - - - - - - - - - -
	Lib_Def	InRun0CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Par InRun1
; - - - - - - - - - - - - -
	tst.w	Direct(a5)
	bne	RIllDir
	tst.l	Mon_Base(a5)
	bne	PRun_Acc
	tst.b	Prg_Accessory(a5)
	bne	PRun_Acc
; Verifie la presence du programme
	move.l	d3,a2
	move.w	(a2)+,d2
	move.l	Name1(a5),a0
	Rjsr	L_ChVerBuf2
	Rjsr	L_Dsk.PathIt
	move.l	#1005,d2		Verifie la presence du fichier!
	Rjsr	L_D_Open
	beq	JDisk
	Rjsr	L_D_Close		Le ferme!
; Effacement des includes
	lea	-2(a4),a0
	Rjsr	L_Includes_Adr
	move.l	a0,VerPos(a5)
	Rjsr	L_Includes_Clear
; - - - - - - - - - - - - -
RunII
; - - - - - - - - - - - - -
; Fait un new, sans effacer les ecrans
	move.l	Prg_Runned(a5),a6
	moveq	#0,d0			Pas d'effacement
	Rjsr	L_Prg_New
; Depile le programme
	Rjsr	L_Prg_Pull
; Charge le nouveau programme dans la structure
	move.b	#1,Prg_Reloaded(a6)
	moveq	#-1,d0			Toujours adapter!
	Rjsr	L_Prg_Load
	cmp.w	#-1,d0
	beq.s	.DErr
	cmp.w	#-2,d0
	ble.s	.OMem
; Run le nouveau programme
	moveq	#-1,d0			Semi init graphique
	move.l	Prg_JError(a5),a1	Ou se brancher en erreur
	sub.l	a2,a2			Pas de message
	Rjsr	L_Prg_RunIt
; Erreurs
; ~~~~~~~
.OMem	moveq	#36,d0
	bra.s	.Err
.DErr	moveq	#101,d0
.Err	sub.l	a0,a0
	move.l	Prg_JError(a5),a1
	jmp	(a1)
Rn_NoF	moveq	#81,d0			Erreur normale!
	Rjmp	L_Error
; - - - - - - - - - - - - -
	Lib_Def	InRun1CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		INSTRUCTION PRUN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InPRun
; - - - - - - - - - - - - -
	tst.w	Direct(a5)
	bne	RIllDir
	tst.l	Mon_Base(a5)
	bne	PRun_Acc
;	tst.b	Prg_Accessory(a5)
;	bne	PRun_Acc
	move.l	d3,a2
	move.w	(a2)+,d2
	move.l	Name1(a5),a0
	Rjsr	L_ChVerBuf2
; Sauve le programme courant
	movem.l	a3-a6/d6/d7,-(sp)
; Le programme est-il deja charge?
	Rjsr	L_Prg_AccAdr
	beq.s	.Loadit
	move.l	a0,a6
	Rjsr	L_Prg_DejaRunned
	beq.s	.Runit
; Il faut charger: verifie la presence du programme
.Loadit	Rjsr	L_Dsk.PathIt
	move.l	#1005,d2		Verifie la presence du fichier!
	Rjsr	L_D_Open
	beq	JDisk
	Rjsr	L_D_Close		Le ferme!
; Ouvre une nouvelle structure
	moveq	#0,d0			Pas de buffer
	Rjsr	L_Prg_NewStructure	Ouvre la structure
	beq	OOfMem
	move.l	d0,a6
; Charge le programme
	moveq	#-1,d0			Toujours adapter
	Rjsr	L_Prg_Load
	tst.w	d0
	bne	.LErr
	move.l	a6,-(sp)		Remet les banques
	move.l	Prg_Runned(a5),a6	du premier programme
	Rjsr	L_Prg_SetBanks
	move.l	(sp)+,a6
; Programme charge: on le demarre!
.Runit	moveq	#-1,d0			Semi init graphique
	lea	PRun_Errors(pc),a1	Retour en cas d'erreur
	sub.l	a2,a2			Pas de message
	move.l	sp,BasSp(a5)		Bas de la pile
	Rjsr	L_Prg_RunIt
	bra.s	.OMm
; Erreur lors du chargement
.LErr	move.w	d0,d1
	moveq	#101,d0
	cmp.w	#-1,d1
	beq.s	.Goerr
.OMm	moveq	#36,d0
; Revient au programme, avec un message d'erreur
.Goerr	move.l	d0,-(sp)
	tst.b	Prg_Edited(a6)		Efface la structure s'il faut
	bne.s	.Edited
	Rjsr	L_Prg_DelStructure
.Edited	move.l	Prg_Runned(a5),a6
	Rjsr	L_Prg_SetBanks
	Rjsr	L_Bnk.Change
	move.l	(sp)+,d0
	move.l	Ed_RunMessages(a5),a0
	Rjsr	L_GetMessage
	movem.l	(sp)+,a3-a6/d6/d7
	move.l	sp,BasSp(a5)
	Rbra	L_ZapReturn
PRun_Acc
 	moveq	#102,d0
	Rjmp	L_Error
;	Retour d'erreur lors de PRUN
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PRun_Errors
	move.l	BasSp(a5),sp
	movem.l	(sp)+,a3-a6/d6/d7	Restore le programme
	move.l	sp,BasSp(a5)
	movem.l	d6/d7,ErrorSave(a5)	Au cas zou
	IFNE	Debug
	movem.l	d6/d7,Chr_Debug+4(a5)	Pour empecher le plantage
	ENDC
	movem.l	a0-a1/d0-d1,-(sp)
	Rjsr	L_Open_MathLibraries	Rouvre les libraries
	movem.l	(sp)+,a0-a1/d0-d1

	cmp.w	#10,d0
	beq.s	.Nul
	cmp.w	#1000,d0
	blt.s	.Null
.Nul	moveq	#0,d0
.Null	move.l	ChVide(a5),a0
	Rbra	L_ZapReturn
JDisk	Rjmp	L_DiskError
; - - - - - - - - - - - - -
	Lib_Def	InPRunCLI
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		ASK EDITOR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InAskEditor1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	move.l	d3,-(a3)
	Rbra	L_InAskEditor3
; - - - - - - - - - - - - -
	Lib_Def	InAskEditor1CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Par	InAskEditor2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	Rbra	L_InAskEditor3
; - - - - - - - - - - - - -
	Lib_Def	InAskEditor2CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Par InAskEditor3
; - - - - - - - - - - - - -
	bsr	Ed_Par
	tst.l	Edit_Segment(a5)
	beq	FonCall
	Rjsrt	L_Ed_ZapFonction
	move.l	d0,ParamE(a5)
	move.l	ChVide(a5),ParamC(a5)
	tst.w	d2
	beq.s	.Skip
	Rjsr	L_A0ToChaine
	move.l	a0,ParamC(a5)
.Skip	rts
; - - - - - - - - - - - - -
	Lib_Def	InAskEditor3CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		ZAPPEUSE D'EDITEUR!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCallEditor1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	move.l	d3,-(a3)
	Rbra	L_InCallEditor3
; - - - - - - - - - - - - -
	Lib_Def	InCallEditor1CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Par InCallEditor2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	Rbra	L_InCallEditor3
; - - - - - - - - - - - - -
	Lib_Def	InCallEditor2CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Par InCallEditor3
; - - - - - - - - - - - - -
	bsr	Ed_Par			Recupere les parametres
	tst.l	Edit_Segment(a5)
	beq	FonCall
	move.l	BasSp(a5),-(sp)		Pousse la pile
	movem.l	a3-a6/d6/d7,-(sp)	Pousse le programme
	move.l	sp,BasSp(a5)
	subq.l	#4,BasSp(a5)
	Rjsrt	L_Ed_ZapIn		Appel de l'editeur
	movem.l	(sp)+,a3-a6/d6/d7
	move.l	(sp)+,BasSp(a5)
	Rbra	L_ZapReturn
; - - - - - - - - - - - - -
	Lib_Def	InCallEditor3CLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Int	Bnk.PrevProgram
	Lib_Def	Bnk.PrevProgramCLI
; - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	Bnk.CurProgram
	Lib_Def	Bnk.CurProgramCLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=Prg Under
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnPrgUnder
; - - - - - - - - - - - - -
	moveq	#-1,d3
	tst.l	Mon_Base(a5)		Si moniteur: RIEN!
	bne.s	.Exit
	move.l	Prg_Runned(a5),a0	Un programme en dessous?
	move.l	Prg_Previous(a0),d0	Non!
	bne.s	.Out
	move.l	Edt_Current(a5),d0	Le programme courant?
	beq.s	.Exit
	move.l	d0,a1
	move.l	Edt_Prg(a1),d0
	cmp.l	d0,a0			Non!
	beq.s	.Exit
	tst.l	Edit_Segment(a5)	Editeur KILL?
	beq.s	.Out
	moveq	#1,d3			OUIIIIIII
	bra.s	.Out
.Exit	moveq	#0,d3
.Out	Ret_Int
; - - - - - - - - - - - - -
	Lib_Def	FnPrgUnderCLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Int	InCloseEditor
	Lib_Def	InCloseEditorCLI
	Lib_Int	InKillEditor
	Lib_Def	InKillEditorCLI
	Lib_Int	InMonitor
	Lib_Def	InMonitorCLI
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DES ROUTINES AMOS / CLI
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_Type
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Def	RunName
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Recupere les parametres
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Ed_Par
; - - - - - - - - - - - - -
Ed_Par	move.l	Name1(a5),a0
	move.l	Name2(a5),a1
	clr.w	(a1)
	cmp.l	#EntNul,d3
	beq.s	.Skip
	move.l	d3,a2
	move.w	(a2)+,d2
	move.w	d2,(a1)
	Rjsr	L_ChVerBuf2
.Skip	move.l	(a3)+,d1
	move.l	(a3)+,d0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retour de zappeuse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	ZapReturn
; - - - - - - - - - - - - -
	move.l	ChVide(a5),ParamC(a5)
	ext.l	d0
	move.l	d0,ParamE(a5)
	beq.s	.Bof
	Rjsr	L_A0ToChaine
	move.l	a0,ParamC(a5)
.Bof	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=Prg State
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnPrgState
; - - - - - - - - - - - - -
	move.w	T_AMOState(a5),d3
	ext.l	d3
	Ret_Int

; - - - - - - - - - - - - -
	Lib_Def	GetInstruction
	Lib_Def	GetInstruction2
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InRem
; - - - - - - - - - - - - -
	add.w	(a6)+,a6
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET BUFFER  / SET STACK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSetBuffer
; - - - - - - - - - - - - -
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: branch.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						SAUTE UN LABEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLab		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SYSTEM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSystem		src/interp/control.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EDIT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEdit		src/interp/control.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DIRECT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDirect		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BREAK ON / OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBreakOn		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Int	InBreakOff		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ON BREAK PROC label
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOnBreak		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ON ERROR ...
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOnError		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Def	InOnErrorGoto
	Lib_Def	InOnErrorProc
; - - - - - - - - - - - - -
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESUME LABEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResumeLabel		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Def	InResumeLabel1
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESUME [label]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResume		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Def	InResume1
; - - - - - - - - - - - - -
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESUME NEXT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResumeNext		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TRAP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InTrap		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=TRAPERR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnErrTrap		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EVERY n PROC GOSUB label
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	InEveryGosub
	Lib_Def	InEveryProc
	Lib_Int	InEvery		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EVERY OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEveryOff		src/interp/control.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EVERY ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEveryOn		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FOR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InFor		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					NEXT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InNext		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Def	InNextF
	Lib_Def	InNextD
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REPEAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRepeat		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UNTIL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InUntil		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WHILE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWhile		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WEND
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWend		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DO
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDo		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LOOP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLoop		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXIT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InExit		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXIT IF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InExitIf		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					IF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InIf		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ELSE IF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InElseIf		src/interp/control.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ELSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InElse		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Def	FnElse
; - - - - - - - - - - - - -
	move.l	#EntNul,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GOSUB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGosub		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RETURN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InReturn		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					POP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPop		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PASSAGE SUR PROCEDURE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	DProc1
	Lib_Def	DProc2F
	Lib_Def	DProc2D
	Lib_Def	FProc
	Lib_Def	PrgInF
	Lib_Def	PrgInD
; - - - - - - - - - - - - -
	Lib_Int	InProcedure		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PROC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InProc		src/interp/control.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					APPEL PROCEDURE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	CallProc		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					END PROC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEndProc		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					POP PROC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPopProc		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine Pop Proc
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			PopP, now in src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GOTO interne: appel direct
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Goto2		src/interp/control.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GOTO
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGoto		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOn		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GETLABEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			GetLabelE, now in src/interp/control.cpp
	Lib_Empty			GetLabelA, now in src/interp/control.cpp
	Lib_Empty			GetLabel, now in src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Instruction finie?
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Finie
Finie
; - - - - - - - - - - - - -
	move.w	(a6),d0
FinieB	beq.s	.Skip
	cmp.w	#_TkDP,d0
	beq.s	.Skip
	cmp.w	#_TkThen,d0
	beq.s	.Skip
	cmp.w	#_TkElse,d0
.Skip	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	RAMENE LA TAILLE DE L'INSTRUCTION D0 en D1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			TInst, now skip_token in src/core/source.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Evalue.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					EVALUATION D'EXPRESSION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Operateur nul (:)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	L_OpeNul
; - - - - - - - - - - - - -
	move.l	#EntNul,d3
	Ret_Int
;						Table des operateurs
; (the operators: interp/eval.cpp. A token is its entry's offset from
; OP_Jumps, so the entries keep their size.)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tk_Operateurs
	dc.w	0,0
	dc.b 	" xor"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	" or"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	" and"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	"<",">"+$80,"O20",-1
	dc.w	0,0
	dc.b 	">","<"+$80,"O20",-1
	dc.w	0,0
	dc.b 	"<","="+$80,"O20",-1
	dc.w	0,0
	dc.b 	"=","<"+$80,"O20",-1
	dc.w	0,0
	dc.b 	">","="+$80,"O20",-1
	dc.w	0,0
	dc.b 	"=",">"+$80,"O20",-1
TOpEg	dc.w	0,0
	dc.b 	"="+$80,"O20",-1
	dc.w	0,0
	dc.b 	"<"+$80,"O20",-1
	dc.w	0,0
	dc.b 	">"+$80,"O20",-1
	dc.w	0,0
	dc.b 	"+"+$80,"O22",-1
TOpM	dc.w	0,0
	dc.b 	"-"+$80,"O22",-1
	dc.w	0,0
	dc.b 	" mod"," "+$80,"O00",-1
	dc.w	0,0
	dc.b 	"*"+$80,"O00",-1
	dc.w	0,0
	dc.b 	"/"+$80,"O00",-1
TOpPuis	dc.w	0,0
	dc.b 	"^"+$80,"O00",-1
	even
OP_Jumps
	dc.l 	0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Entree evaluation
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par Fn_New_Evalue
Fn_New_Evalue
; - - - - - - - - - - - - -
	addq.l	#2,a6

; Debut de parenthese
; ~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Entree de l'evaluation
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
New_Evalue
	Rjmp	L_New_Evalue
; The evaluator and the operators: interp/eval.cpp

;	Saute D0 params
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Saut_D0Param
	subq.l	#2,a6
Fn_Saut_D0Param
	move.w	d0,-(sp)
.Loop	bsr	Fn_New_Evalue
	subq.w	#1,(sp)
	bne.s	.Loop
	addq.l	#2,sp
	rts
;	Recupere D0 params
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ParD0	subq.l	#2,a6
FnParD0	move.w	d0,-(sp)
.Loop	bsr	Fn_New_Evalue
	cmp.b	#1,d2
	bne.s	.Skip
	Rjsrt	L_FlToInt1
.Skip	move.l	d3,-(a3)
	subq.w	#1,(sp)
	bne.s	.Loop
	addq.l	#2,sp
	rts

;	Expentier: retourne un parametre entier / chaine
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fn_New_Expentier
	addq.l	#2,a6
New_Expentier
	bsr	New_Evalue
	cmp.b	#1,d2
	beq.s	.Skip
	rts
.Skip	Rjmpt	L_FlToInt1
	Lib_Int	New_Evalue



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Instruction vide
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	InNull
; - - - - - - - - - - - - -
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OPERATEUR NULL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnNull
; - - - - - - - - - - - - -
	move.l	#EntNul,d3
	subq.l	#2,a6
	Ret_Int

; Variables: interp/vars.cpp
FindVar	Rjmp	L_FindVar
GetTablo	Rjmp	L_GetTablo
	Lib_Int	InVar
	Lib_Int	FnVar

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						DIM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDim		src/interp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						TROUVE ELEMENT DE TABLEAU
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	GetTablo

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						VARPTR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnVarPtr		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					=ARRAY$(a$(0))
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnArray		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					PREND UNE CONSTANTE ENTIERE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCEntier		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					PREND UN FLOAT SIMPLE PRECISION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCstFl		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					PREND UN FLOAT DOUBLE PRECISION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCstDFl		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					PREND UNE CHAINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCstCh		src/interp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						SHARED / GLOBAL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InShared		src/interp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						DEF FN / =FN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDFn		src/interp
; - - - - - - - - - - - - - - - -
	Lib_Int	FnFn		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						SWAP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSwap		src/interp
; - - - - - - - - - - - - -
	Lib_Def	InSwapD
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						NOT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnNot		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						MAX / MIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Max / Min: interp/eval.cpp
	Lib_Int	FnMax
	Lib_Def	FnMaxS
; - - - - - - - - - - - - -
	Lib_Int	FnMin
	Lib_Def	FnMinS
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						INC + DEC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInc		src/interp
; - - - - - - - - - - - - - - - -
	Lib_Int	InDec		src/interp
; - - - - - - - - - - - - - - - -
	Lib_Int	InAdd2		src/interp
; - - - - - - - - - - - - - - - -
	Lib_Int	InAdd4		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						SORT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSort		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						=MATCH
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnMatch		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve les parametres tableau pour SORT et FIND
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			GTablo, now in src/interp
; - - - - - - - - - - - - -
	Lib_Empty			AdSort, now in src/interp
; - - - - - - - - - - - - -
	Lib_Empty			CpBis, now in src/interp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						Instruction DATA
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InData		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						Instruction READ
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRead		src/interp

; - - - - - - - - - - - - -
	Lib_Def	InReadF
	Lib_Def	InReadS
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;						Instruction RESTORE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRestore		src/interp/control.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRestore1
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source:	Diskio.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FIELD n,AA as A$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InField		src/interp/field.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INPUT #
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLineInputH		src/interp
; - - - - - - - - - - - - -
	Lib_Int	InInputH		src/interp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INPUT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInput		src/interp
; - - - - - - - - - - - - -
	Lib_Int	InLineInput		src/interp
; - - - - - - - - - - - - -
	Lib_Def	Input
	Lib_Def	CRet
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PRINT #n,"kjfdjkf";
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPrintH		src/interp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LPRINT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLPrint		src/interp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PRINT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPrint		src/interp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Pour le compilateur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	PrintE
	Lib_Def	PrintF
	Lib_Def	PrintS
	Lib_Def	PrintX
	Lib_Def	LPrintX
	Lib_Def	CRPrint
	Lib_Def	HPrintS
	Lib_Def	PrtRet
	Lib_Def	PrtVir
	Lib_Def	HPrintX
	Lib_Def	UsingC
	Lib_Def	UsingS
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Ecrans.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEFAULT PALETTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDefaultPalette		src/interp/graphics.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PALETTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPalette		src/interp/graphics.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Plt, now in src/interp/graphics.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FADE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	InFade1
	Lib_Def	InFade2
	Lib_Def	InFade3
	Lib_Def	InFadePal
	Lib_Int	InFade		src/interp/graphics.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					POLYLINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPolyline		src/interp/graphics.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					POLYGON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPolygon		src/interp/graphics.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source:	Sprite.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHANNEL x TO SPRITE x
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	ChannelToSprite
	Lib_Def	ChannelToBob
	Lib_Def	ChannelToSDisplay
	Lib_Def	ChannelToSSize
	Lib_Def	ChannelToSOffset
	Lib_Def	ChannelToRainbow
; - - - - - - - - - - - - -
	Lib_Int	InChannel		src/interp/graphics.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: memory.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BSET / BTST / BCHG / BCLR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBset		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InBset1
; - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	InBclr		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InBclr1
; - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	InBchg		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InBchg1
; - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	FnBtst		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	FnBtst1
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROR ROL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRorB		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRorB1
; - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	InRorW		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRorW1
; - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	InRorL		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRorL1
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Int	InRolB		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRolB1
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Int	InRolW		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRolW1
; - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Int	InRolL		src/interp/bits.cpp
; - - - - - - - - - - - - -
	Lib_Def	InRolL1
; - - - - - - - - - - - - -

; BsRout: interp/bits.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CALL PROCEDURE LANGAGE MACHINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	In_apml_		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCall		src/interp/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LVO =EQU
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnEqu		src/interp/system.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=STRUC=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InStruc		src/interp/system.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnStruc		src/interp/system.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=STRUC$=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InStrucD		src/interp/system.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnStrucD		src/interp/system.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: String.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				Demande de l'espace pour les chaines
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Demande		src/interp/heap.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Demande chaine sans erreur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	DDemande		src/interp/heap.cpp




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENAGE ALPHANUMERIQUE
;	Taille maximum chaine: 65472 ($FFC0)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Menage		src/interp/heap.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LEFT$=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLeft		src/interp/text.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Fonction LEFT$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLeft		src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=RIGHT$=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRight		src/interp/text.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnRight		src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MID$=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMid2		src/interp/text.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnMid2		src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=MID$() 3 params
; - - - - - - - - - - - - -
	Lib_Int	InMid3		src/interp/text.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnMid3		src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Commun LEFT MID RIGHT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			RFnMid, now in src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Commun MID LEFT RIGHT =
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			RInMid, now in src/interp/text.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Commun LEFT MID RIGHT II
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			RInMid2, now in src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=VAL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnVal		src/interp/text.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=RESOURCE$(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnResource		src/interp/text.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Menus.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MENU KEY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuKey		src/interp/menus.cpp
; - - - - - - - - - - - - -
	Lib_Def	InMenuKey1
	Lib_Def	InMenuKey2
	Lib_Def	InMenuKey3
	Lib_Def	MnKy
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ON MENU
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOnMenu		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MENU$(,,,)=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenu		src/interp/menus.cpp
; - - - - - - - - - - - - -
	Lib_Def	InMenu2
	Lib_Def	InMenu3
	Lib_Def	InMenu4
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU DEL [(coordonnees,,)]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuDel		src/interp/menus.cpp
; - - - - - - - - - - - - -
	Lib_Def	InMenuDel1
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	SET MENU
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetMenu		src/interp/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Routine-> adresse du flag
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnDim, now in src/interp/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Entree appel de procedure menu
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MenuProcedure		src/interp/control.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	SOUS PROGRAMME UTILISE PAR VAL ET INPUT
;	D0=	Tenir compte du signe (TRUE)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	ValRout		src/interp/number.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEBUT DES SWAP FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Start_FloatSwap
; - - - - - - - - - - - - -
	Lib_Def	CmpInitFloat
	Lib_Def	CmpInitDouble
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ENTIER >>> FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Conversion entier >>> float dans le dernier operateur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	IntToFl1		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DIntToFl1		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ENTIER >>> FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Conversion entier >>> float dans la pile
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	IntToFl2		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DIntToFl2		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FLOAT >>> ENTIER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Conversion float >>> entier dans le dernier operateur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FlToInt1		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DFlToInt1		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FLOAT >>> ENTIER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Conversion float >>> entier dans la pile
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FlToInt2		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DFlToInt2		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FONCTION MATHEMATIQUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Fait une fonction mathematique
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Math_Fonction		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DMath_Fonction		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COMPARAISONS FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Fait une comparaison float
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Float_Compare		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DFloat_Compare		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Operation FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Fait une operation float
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Float_Operation		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DFloat_Operation		src/interp/float.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FLOAT= ZERO?
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Fait un TST sur le float D3
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Float_Test		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	Float_TestF		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Operation MATH
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Fait une operation Math
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Math_Operation		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DMath_Operation		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FONCTION FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       Fait une fonction float
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Float_Fonction		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	DFloat_Fonction		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Verifie que le float est positif
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FlPos		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	FlPosD		src/interp/float.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RETOURNE UN ANGLE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	AAngle		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	AAngleD		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TRANSFORME EN ANGLE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FFAngle		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	FAngleD		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PARAM FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnParamF		src/interp/float.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnParamD		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ASCII Vers Float
;	A0	Buffer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Ascii2Float		src/interp/float.cpp
	Lib_Int	Ascii2FloatD		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Float vers ascii
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Float2Ascii		src/interp/float.cpp
	Lib_Int	Float2AsciiD		src/interp/float.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Pour le compilateur, fonctions internes
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	FnMaxF
	Lib_Def	FnMaxD
	Lib_Def	FnMinF
	Lib_Def	FnMinD

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FIN DES SWAP FLOAT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_FloatSwap
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Relai de saut aux erreurs principales
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FonCall		Rbra	L_FonCall
Synt		Rbra	L_Syntax
OOfMem		Rbra	L_OOfMem
StooLong	Rbra	L_StooLong
ScNOp		Rbra	L_ScNOp
FilTM		Rbra	L_FilTM
AdrErr		Rbra	L_AdrErr
EcWiErr		Rbra	L_EcWiErr
BkNoRes		Rbra	L_BkNoRes



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Espace pour le compilateur!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Pos 500
; - - - - - - - - - - - - -
