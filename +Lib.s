; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		OPT 	P+

;		Nombre de fonctions dans la librairie
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		Include	"+Lib_Size.s"

; 		Includes
; ~~~~~~~~~~~~~~~~~~~~~~
		Include "+AMOS_Includes.s"

; 		Versions
; ~~~~~~~~~~~~~~~~~~~~~~
Version		MACRO
		dc.b	"2.00"
		ENDM
Ver_Number	equ	$0200

;---------------------------------------------------------------------
;   +++  +++  ++   ++ ++++  ++++ ++    ++    ++   ++   ++  +++   +++
;  ++   ++ ++ +++ +++ ++ ++  ++  ++    ++   ++++  +++ +++ ++ ++ ++
;  ++   ++ ++ ++ + ++ ++ ++  ++  ++   ++   ++  ++ ++ + ++ ++ ++  +++
;  ++   ++ ++ ++   ++ ++++   ++  ++        ++++++ ++   ++ ++ ++    ++
;  ++   ++ ++ ++   ++ ++     ++  ++        ++  ++ ++   ++ ++ ++ +  ++
;   +++  +++  ++   ++ ++    ++++ ++++      ++  ++ ++   ++  +++   +++
;---------------------------------------------------------------------
		dc.l	C_Tk-C_Off
		dc.l	C_Lib-C_Tk
		dc.l	C_Title-C_Lib
		dc.l	C_End-C_Title
		dc.w	0
		dc.b	"AP20"

;---------------------------------------------------------------------
;		Pointeurs sur fonctions
;---------------------------------------------------------------------
		MCInit
C_Off
		REPT	Lib_Size
		MC
		ENDR

;---------------------------------------------------------------------
;		Table des tokens
;---------------------------------------------------------------------
;	TOKEN_START

C_Tk	dc.w L_Nul,L_FnNull			//$2F$03$05$00
	dc.b $80,-1

* Variables
	dc.w L_InVar,L_FnVar			//$06$07$05$01
	dc.b $80,-1
	dc.w L_InLab,L_Nul    			//$07$01$05$02
	dc.b $80,-1
	dc.w L_CallProc,L_Nul			//$08$01$05$03
	dc.b $80,-1
	dc.w L_Goto2,L_Nul    			//$01$01$05$04
	dc.b $80,-1

* Constantes
	dc.w L_Nul,L_FnCEntier			//$01$14$05$05
	dc.b $80,"C0",-1
	dc.w L_Nul,L_FnCstCh			//$01$17$05$06
	dc.b $80,"C2",-1
	dc.w L_Nul,L_FnCstCh			//$01$17$05$06
	dc.b $80,"C2",-1
	dc.w L_Nul,L_FnCEntier			//$01$14$05$05
	dc.b $80,"C0",-1
	dc.w L_Nul,L_FnCEntier			//$01$14$05$05
	dc.b $80,"C0",-1
	dc.w L_Nul,L_FnCstFl			//$01$15$05$05
	dc.b $80,"C1",-1
* Token d'extension
	dc.w L_InExtCall,L_FnExtCall 		//$28$06$05$07
	dc.b $80,-1

* Deux points, ...
	dc.w L_InNull,L_FnNull 			//$01$02$06$00
	dc.b ":"," "+$80,-1
	dc.w L_Nul,L_FnNull   			//$01$03$05$00
	dc.b ","+$80,"O",-1
	dc.w L_Nul,L_Nul   			//$01$01$05$00
	dc.b ";"+$80,"O",-1
	dc.w L_Nul,L_Nul   			//$01$01$05$00
	dc.b "#"+$80,"O",-1

* Parentheses
	dc.w L_Nul,L_New_Evalue			//$01$04$05$00
	dc.b "("+$80,"O",-1
	dc.w L_Nul,L_FnNull   			//$01$02$05$00
	dc.b ")"+$80,"O",-1
	dc.w L_Nul,L_New_Evalue			//$01$01$05$00
	dc.b "["+$80,"O",-1
	dc.w L_Nul,L_Nul     			//$01$01$05$00
	dc.b "]"+$80,"O",-1

* Instructions normales
	dc.w L_Nul,L_FnNull			//$01$02$07$00
	dc.b "to"," "+$80,-1
	dc.w L_Nul,L_FnNot			//$01$0A$08$00
	dc.b "not"," "+$80,-1
	dc.w L_InSwap,L_Nul			//$22$01$09$00
	dc.b "swap"," "+$80,-1
	dc.w L_InDFn,L_Nul			//$fc$01$0B$00
	dc.b "def fn"," "+$80,-1
	dc.w L_Nul,L_FnFn			//$01$09$07$00
	dc.b "fn"," "+$80,-1
	dc.w L_Syntax,L_Nul			//$00$01$0E$00
	dc.b "follow of","f"+$80,"I",-1
	dc.w L_Syntax,L_Nul			//$23$01$0A$00
	dc.b "follo","w"+$80,"I",-1
	dc.w L_InResumeNext,L_Nul		//$00$01$0F$00
	dc.b "resume nex","t"+$80,"I",-1
* Tokens AVANT les autres!
	dc.w L_Nul,L_FnInkey			//$01$00$0A$00
	dc.b "inkey","$"+$80,"2",-1
	dc.w L_Nul,L_FnRepeat			//$01$00$0B$00
	dc.b "repeat","$"+$80,"22,0",-1
	dc.w L_Nul,L_FnZoneD			//$01$00$09$00
	dc.b "zone","$"+$80,"22,0",-1
	dc.w L_Nul,L_FnBorderD			//$01$00$0B$00
	dc.b "border","$"+$80,"22,0",-1
	dc.w L_InDoubleBuffer,L_Nul		//$00$01$11$00
	dc.b "double buffe","r"+$80,"I",-1
	dc.w L_Nul,L_FnStart			//$01$00$09$00
	dc.b "star","t"+$80,"00",-1
	dc.w L_Nul,L_FnLength			//$01$00$0A$00
	dc.b "lengt","h"+$80,"00",-1
	dc.w L_InDoke,L_Nul			//$00$01$08$00
	dc.b "dok","e"+$80,"I0,0",-1
	dc.w L_InOnMenuDel,L_Nul		//$00$01$0F$00
	dc.b "on menu de","l"+$80,"I",-1
	dc.w L_InOnMenuOn,L_Nul			//$00$01$0E$00
	dc.b "on menu o","n"+$80,"I",-1
	dc.w L_InOnMenuOff,L_Nul		//$00$01$0F$00
	dc.b "on menu of","f"+$80,"I",-1
	dc.w L_InEveryOn,L_Nul			//$00$01$0C$00
	dc.b "every o","n"+$80,"I",-1
	dc.w L_InEveryOff,L_Nul			//$00$01$0D$00
	dc.b "every of","f"+$80,"I",-1
	dc.w L_Nul,L_FnLogBase			//$01$00$0B$00
	dc.b "logbas","e"+$80,"00",-1
	dc.w L_Nul,L_FnLogic0			//$01$00$0A$00
	dc.b "!logi","c"+$80,"0",-2
	dc.w L_Nul,L_FnLogic1			//$01$19$05$00
	dc.b $80,"00",-1
	dc.w L_Nul,L_FnAsc			//$01$00$07$00
	dc.b "as","c"+$80,"02",-1
	dc.w L_Syntax,L_Syntax			//$01$01$06$00
	dc.b "a","s"+$80,"I",-1
	dc.w L_InCall,L_Nul			//$1A$01$08$00
	dc.b "cal","l"+$80,"I",-1
	dc.w L_Nul,L_FnExeCall			//$01$00$0B$00
	dc.b "execal","l"+$80,"00",-1
	dc.w L_Nul,L_FnGfxCall			//$01$00$0B$00
	dc.b "gfxcal","l"+$80,"00",-1
	dc.w L_Nul,L_FnDosCall			//$01$00$0B$00
	dc.b "doscal","l"+$80,"00",-1
	dc.w L_Nul,L_FnIntCall			//$01$00$0B$00
	dc.b "intcal","l"+$80,"00",-1
	dc.w L_InFreeze,L_Nul			//$00$01$0A$00
	dc.b "freez","e"+$80,"I",-1

* Boucles / Branchements
	dc.w L_InFor,L_Nul			//$30$01$08$00
	dc.b "for"," "+$80,-1
	dc.w L_InNext,L_Nul			//$31$01$09$00
	dc.b "next"," "+$80,-1
	dc.w L_InRepeat,L_Nul			//$32$01$0B$12
	dc.b "repeat"," "+$80,-1
	dc.w L_InUntil,L_Nul			//$33$01$0A$12
	dc.b "until"," "+$80,-1
	dc.w L_InWhile,L_Nul			//$34$01$0A$12
	dc.b "while"," "+$80,-1
	dc.w L_InWend,L_Nul			//$35$01$09$12
	dc.b "wend"," "+$80,-1
	dc.w L_InDo,L_Nul			//$36$01$07$12
	dc.b "do"," "+$80,-1
	dc.w L_InLoop,L_Nul			//$37$01$09$12
	dc.b "loop"," "+$80,-1
	dc.w L_InExitIf,L_Nul			//$39$01$0C$12
	dc.b "exit if"," "+$80,-1
	dc.w L_InExit,L_Nul			//$38$01$09$12
	dc.b "exit"," "+$80,-1
	dc.w L_InGoto,L_Nul			//$3E$01$09$12
	dc.b "goto"," "+$80,-1
	dc.w L_InGosub,L_Nul			//$3F$01$0A$12
	dc.b "gosub"," "+$80,-1
	dc.w L_InIf,L_Nul			//$3A$01$07$12
	dc.b "if"," "+$80,-1
	dc.w L_Nul,L_FnNull			//$52$02$09$12
	dc.b "then"," "+$80,-1
	dc.w L_InElse,L_FnElse			//$3B$02$09$12
	dc.b "else"," "+$80,-1
	dc.w L_InNull,L_FnNull			//$3D$01$0B$12
	dc.b "end if"," "+$80,-1
	dc.w L_InOnError,L_Nul			//$40$01$0D$12
	dc.b "on error"," "+$80,-1
	dc.w L_InOnBreak,L_Nul			//$41$01$12$12
	dc.b "on break proc"," "+$80,-1
	dc.w L_InOnMenu,L_NoFlag		//$42$01$0B$12
	dc.b "on men","u"+$80,"I0",-1
	dc.w L_InOn,L_Nul			//$43$01$07$12
	dc.b "on"," "+$80,-1
	dc.w L_InResumeLabel,L_Nul		//$45$01$11$12
	dc.b "resume label"," "+$80,-1
	dc.w L_InResume,L_Nul			//$44$01$0B$12
	dc.b "resume"," "+$80,-1
	dc.w L_InPopProc,L_Nul			//$46$01$0C$12
	dc.b "pop pro","c"+$80,-1
	dc.w L_InEvery,L_Nul			//$47$01$09$12
	dc.b "ever","y"+$80,"I",-1

	dc.w L_Syntax,L_Nul			//$01$01$09$00
	dc.b "step"," "+$80,-1
	dc.w L_InReturn,L_Nul			//$53$01$0A$00
	dc.b "retur","n"+$80,"I",-1
	dc.w L_InPop,L_Nul			//$54$01$07$00
	dc.b "po","p"+$80,"I",-1
	dc.w L_InProcedure,L_Nul		//$ff$01$0E$0A
	dc.b "procedure"," "+$80,-1
	dc.w L_InProc,L_Nul			//$0F$01$09$0F
	dc.b "proc"," "+$80,-1
	dc.w L_InEndProc,L_Nul			//$fe$01$0C$0B
	dc.b "end pro","c"+$80,-1
	dc.w L_InShared,0     			//$fb$01$0B$00
	dc.b "shared"," "+$80,-1
	dc.w L_InShared,0     			//$fb$01$0B$00
	dc.b "global"," "+$80,-1
	dc.w L_InEnd,L_Nul			//$00$01$07$00
	dc.b "en","d"+$80,"I",-1
	dc.w L_InStop,L_Nul			//$00$01$08$00
	dc.b "sto","p"+$80,"I",-1
	dc.w L_Nul,L_FnParamF			//$01$1C$0A$00
	dc.b "param","#"+$80,"0",-1
	dc.w L_Nul,L_FnParamS			//$01$1D$0A$00
	dc.b "param","$"+$80,"2",-1
	dc.w L_Nul,L_FnParamE			//$01$1B$09$00
	dc.b "para","m"+$80,"0",-1
	dc.w L_InError,L_Nul			//$00$01$09$00
	dc.b "erro","r"+$80,"I0",-1
	dc.w L_Nul,L_FnErrn			//$01$00$08$00
	dc.b "err","n"+$80,"0",-1
	dc.w L_InData,L_Nul			//$fd$01$09$00
	dc.b "data"," "+$80,-1
	dc.w L_InRead,L_Nul			//$13$01$09$00
	dc.b "read"," "+$80,-1
	dc.w L_InRestore,L_Nul			//$14$01$0C$00
	dc.b "restore"," "+$80,-1
	dc.w L_InBreakOff,L_Nul			//$00$01$0D$00
	dc.b "break of","f"+$80,"I",-1
	dc.w L_InBreakOn,L_Nul			//$00$01$0C$00
	dc.b "break o","n"+$80,"I",-1
	dc.w L_InInc,L_Nul			//$16$01$07$00
	dc.b "in","c"+$80,"I",-1
	dc.w L_InDec,L_Nul			//$0E$01$07$00
	dc.b "de","c"+$80,"I",-1
	dc.w L_InAdd2,L_Nul			//$17$01$08$00
	dc.b "!ad","d"+$80,"I",-2
	dc.w L_InAdd4,L_Nul			//$4F$01$05$00
	dc.b $80,"I",-1
* Print/Input
	dc.w L_InPrintH,L_Nul			//$0B$01$0B$00
	dc.b "print ","#"+$80,-1
	dc.w L_InPrint,L_Nul  			//$0A$01$09$00
	dc.b "prin","t"+$80,"I",-1
	dc.w L_InLPrint,L_Nul			//$48$01$0A$00
	dc.b "lprin","t"+$80,"I",-1
	dc.w L_Nul,L_FnInputD1			//$01$00$0B$00
	dc.b "!input","$"+$80,"20",-2
	dc.w L_Nul,L_FnInputD2			//$01$19$05$00
	dc.b $80,"20,0",-1
	dc.w L_Syntax,L_Syntax			//$01$01$09$00
	dc.b "usin","g"+$80,"I",-1
	dc.w L_InInputH,L_Nul			//$0D$01$0B$00
	dc.b "input ","#"+$80,-1
	dc.w L_InLineInputH,L_Nul		//$4A$01$10$00
	dc.b "line input ","#"+$80,-1
	dc.w L_InInput,L_Nul			//$0C$01$09$00
	dc.b "inpu","t"+$80,"I",-1
	dc.w L_InLineInput,L_Nul		//$49$01$0E$00
	dc.b "line inpu","t"+$80,"I",-1
	dc.w L_InRun0,L_Nul			//$00$01$08$00
	dc.b "!ru","n"+$80,"I",-2
	dc.w L_InRun1,L_Nul			//$2D$01$05$00
	dc.b $80,"I2",-1
	dc.w L_InSetBuffer,L_Nul		//$03$01$0E$00
	dc.b "set buffe","r"+$80,"I",-1
* Gestion des chaines
	dc.w L_InMid3,L_NoFlag			//$4B$21$09$00
	dc.b "!mid","$"+$80,"22,0,0",-2
	dc.w L_InMid2,L_NoFlag			//$4C$22$05$00
	dc.b $80,"22,0",-1
	dc.w L_InLeft,L_NoFlag			//$4D$23$09$00
	dc.b "left","$"+$80,"22,0",-1
	dc.w L_InRight,L_NoFlag			//$4E$24$0A$00
	dc.b "right","$"+$80,"22,0",-1
	dc.w L_Nul,L_FnFlip			//$01$00$09$00
	dc.b "flip","$"+$80,"22",-1
	dc.w L_Nul,L_FnChr			//$01$00$08$00
	dc.b "chr","$"+$80,"20",-1
	dc.w L_Nul,L_FnSpace			//$01$00$0A$00
	dc.b "space","$"+$80,"20",-1
	dc.w L_Nul,L_FnString			//$01$00$0B$00
	dc.b "string","$"+$80,"22,0",-1
	dc.w L_Nul,L_FnUpper			//$01$00$0A$00
	dc.b "upper","$"+$80,"22",-1
	dc.w L_Nul,L_FnLower			//$01$00$0A$00
	dc.b "lower","$"+$80,"22",-1
	dc.w L_Nul,L_FnStrE			//$01$00$08$00
	dc.b "str","$"+$80,"24",-1
	dc.w L_Nul,L_FnVal			//$01$05$07$00
	dc.b "va","l"+$80,"02",-1
	dc.w L_Nul,L_FnBin1			//$01$00$09$00
	dc.b "!bin","$"+$80,"20",-2
	dc.w L_Nul,L_FnBin2			//$01$19$05$00
	dc.b $80,"20,0",-1
	dc.w L_Nul,L_FnHex1			//$01$00$09$00
	dc.b "!hex","$"+$80,"20",-2
	dc.w L_Nul,L_FnHex2			//$01$19$05$00
	dc.b $80,"20,0",-1
	dc.w L_Nul,L_FnLen			//$01$00$07$00
	dc.b "le","n"+$80,"02",-1
	dc.w L_Nul,L_FnInstr2			//$01$00$0A$00
	dc.b "!inst","r"+$80,"02,2",-2
	dc.w L_Nul,L_FnInstr3			//$01$19$05$00
	dc.b $80,"02,2,0",-1
	dc.w L_Nul,L_FnTab			//$01$00$08$00
	dc.b "tab","$"+$80,"2",-1
	dc.w L_Nul,L_FnFree			//$01$00$08$00
	dc.b "fre","e"+$80,"0",-1
	dc.w L_Nul,L_FnVarPtr			//$01$08$0A$00
	dc.b "varpt","r"+$80,"0",-1
	dc.w L_InRememberX,L_Nul		//$00$01$0E$00
	dc.b "remember ","x"+$80,"I",-1
	dc.w L_InRememberY,L_Nul		//$00$01$0E$00
	dc.b "remember ","y"+$80,"I",-1

	dc.w L_InDim,L_Nul			//$09$01$07$00
	dc.b "di","m"+$80,"I",-1
	dc.w L_InRem,L_Nul			//$02$01$07$0E
	dc.b "re","m"+$80,-1
	dc.w L_InRem,L_Nul			//$02$01$05$0E
	dc.b "'"+$80,-1
	dc.w L_InSort,L_NoFlag			//$21$01$08$00
	dc.b "sor","t"+$80,"I",-1
	dc.w L_NoFlag,L_FnMatch			//$01$0D$09$00
	dc.b "matc","h"+$80,"03,3",-1
	dc.w L_InEdit,L_Nul			//$00$01$08$00
	dc.b "edi","t"+$80,"I",-1
	dc.w L_InDirect,L_Nul			//$00$01$0A$00
	dc.b "direc","t"+$80,"I",-1
* Fonctions
	dc.w L_Nul,L_FnRnd			//$01$00$07$00
	dc.b "rn","d"+$80,"00",-1
	dc.w L_InRandom,L_Nul			//$00$01$0D$00
	dc.b "randomiz","e"+$80,"I0",-1
	dc.w L_Nul,L_FnSgn			//$01$00$07$00
	dc.b "sg","n"+$80,"04",-1
	dc.w L_Nul,L_FnAbs			//$01$00$07$00
	dc.b "ab","s"+$80,"44",-1
	dc.w L_Nul,L_FnInt			//$01$00$07$00
	dc.b "in","t"+$80,"44",-1
	dc.w L_InRadian,L_Nul			//$00$01$0A$00
	dc.b "radia","n"+$80,"I",-1
	dc.w L_InDegree,L_Nul			//$00$01$0A$00
	dc.b "degre","e"+$80,"I",-1
	dc.w L_Nul,L_FnPi			//$01$00$07$00
	dc.b "pi","#"+$80,"1",-1
	dc.w L_InFix,L_Nul			//$00$01$07$00
	dc.b "fi","x"+$80,"I0",-1
	dc.w L_NoFlag,L_FnMin			//$01$0F$07$00
	dc.b "mi","n"+$80,"00,0",-1
	dc.w L_NoFlag,L_FnMax			//$01$20$07$00
	dc.b "ma","x"+$80,"00,0",-1
	dc.w L_Nul,L_FnSin			//$01$13$07$00
	dc.b "si","n"+$80,"15",-1
	dc.w L_Nul,L_FnCos 			//$01$13$07$00
	dc.b "co","s"+$80,"15",-1
	dc.w L_Nul,L_FnTan			//$01$13$07$00
	dc.b "ta","n"+$80,"15",-1
	dc.w L_Nul,L_FnASin			//$01$13$08$00
	dc.b "asi","n"+$80,"11",-1
	dc.w L_Nul,L_FnACos			//$01$13$08$00
	dc.b "aco","s"+$80,"11",-1
	dc.w L_Nul,L_FnATan			//$01$13$08$00
	dc.b "ata","n"+$80,"11",-1
	dc.w L_Nul,L_FnHSin			//$01$13$08$00
	dc.b "hsi","n"+$80,"15",-1
	dc.w L_Nul,L_FnHCos			//$01$13$08$00
	dc.b "hco","s"+$80,"15",-1
	dc.w L_Nul,L_FnHTan			//$01$13$08$00
	dc.b "hta","n"+$80,"15",-1
	dc.w L_Nul,L_FnSqr			//$01$13$07$00
	dc.b "sq","r"+$80,"11",-1
	dc.w L_Nul,L_FnLog			//$01$13$07$00
	dc.b "lo","g"+$80,"11",-1
	dc.w L_Nul,L_FnLn			//$01$13$06$00
	dc.b "l","n"+$80,"11",-1
	dc.w L_Nul,L_FnExp			//$01$13$07$00
	dc.b "ex","p"+$80,"11",-1
* MENUS!!!
	dc.w L_InMenuToBank,L_Nul		//$00$01$10$00
	dc.b "menu to ban","k"+$80,"I0",-1
	dc.w L_InBankToMenu,L_Nul		//$00$01$10$00
	dc.b "bank to men","u"+$80,"I0",-1
	dc.w L_InMenuOn,L_Nul			//$00$01$0B$00
	dc.b "menu o","n"+$80,"I",-1
	dc.w L_InMenuOff,L_Nul			//$00$01$0C$00
	dc.b "menu of","f"+$80,"I",-1
	dc.w L_InMenuCalc,L_Nul			//$00$01$0D$00
	dc.b "menu cal","c"+$80,"I",-1
	dc.w L_InMenuMouseOn,L_Nul		//$00$01$11$00
	dc.b "menu mouse o","n"+$80,"I",-1
	dc.w L_InMenuMouseOff,L_Nul		//$00$01$12$00
	dc.b "menu mouse of","f"+$80,"I",-1
	dc.w L_InMenuBase,L_Nul			//$00$01$0D$00
	dc.b "menu bas","e"+$80,"I0,0",-1

	dc.w L_InSetMenu,L_NoFlag		//$1D$01$0C$00
	dc.b "set men","u"+$80,"I0t0,0",-1
	dc.w L_NoFlag,L_FnXMenu			//$01$0B$0A$00
	dc.b "x men","u"+$80,"00",-1
	dc.w L_NoFlag,L_FnYMenu			//$01$0B$0A$00
	dc.b "y men","u"+$80,"00",-1
	dc.w L_InMenuKey,L_NoFlag		//$1E$01$0C$00
	dc.b "menu ke","y"+$80,"I0t0,0",-1
	dc.w L_InMenuBar,L_NoFlag		//$1F$01$0C$00
	dc.b "menu ba","r"+$80,"I0",-1
	dc.w L_InMenuLine,L_NoFlag		//$1F$01$0D$00
	dc.b "menu lin","e"+$80,"I0",-1
	dc.w L_InMenuTline,L_NoFlag		//$1F$01$0E$00
	dc.b "menu tlin","e"+$80,"I0",-1
	dc.w L_InMenuMovable,L_NoFlag		//$1F$01$10$00
	dc.b "menu movabl","e"+$80,"I0",-1
	dc.w L_InMenuStatic,L_NoFlag		//$1F$01$0F$00
	dc.b "menu stati","c"+$80,"I0",-1
	dc.w L_InMenuItemMovable,L_NoFlag	//$1F$01$15$00
	dc.b "menu item movabl","e"+$80,"I0",-1
	dc.w L_InMenuItemStatic,L_NoFlag	//$1F$01$14$00
	dc.b "menu item stati","c"+$80,"I0",-1
	dc.w L_InMenuActive,L_NoFlag		//$1F$01$0F$00
	dc.b "menu activ","e"+$80,"I0",-1
	dc.w L_InMenuInactive,L_NoFlag		//$1F$01$11$00
	dc.b "menu inactiv","e"+$80,"I0",-1
	dc.w L_InMenuSeparate,L_NoFlag		//$1F$01$11$00
	dc.b "menu separat","e"+$80,"I0",-1
	dc.w L_InMenuLink,L_NoFlag		//$1F$01$0D$00
	dc.b "menu lin","k"+$80,"I0",-1
	dc.w L_InMenuCalled,L_NoFlag		//$1F$01$0F$00
	dc.b "menu calle","d"+$80,"I0",-1
	dc.w L_InMenuOnce,L_NoFlag		//$1F$01$0D$00
	dc.b "menu onc","e"+$80,"I0",-1

	dc.w L_InMenuDel,L_NoFlag		//$1C$01$0C$00
	dc.b "menu de","l"+$80,"I",-1
	dc.w L_InMenu,L_NoFlag			//$1B$01$09$00
	dc.b "menu","$"+$80,"V",-1
	dc.w L_Nul,L_FnChoice0			//$01$00$0B$00
	dc.b "!choic","e"+$80,"0",-2
	dc.w L_Nul,L_FnChoice1			//$01$19$05$00
	dc.b $80,"00",-1

* Screen instructions
	dc.w L_InScreenCopy2,L_Nul		//$00$01$10$00
	dc.b "!screen cop","y"+$80,"I0t0",-2
	dc.w L_InScreenCopy3,L_Nul		//$2D$01$05$00
	dc.b $80,"I0t00",-2
	dc.w L_InScreenCopy8,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0,0t0,0,0",-2
	dc.w L_InScreenCopy9,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0,0t0,0,0,0",-1
	dc.w L_InScreenClone,L_Nul		//$00$01$10$00
	dc.b "screen clon",$80+"e","I0",-1
	dc.w L_InScreenOpen,L_Nul		//$00$01$0F$00
	dc.b "screen ope","n"+$80,"I0,0,0,0,0",-1
	dc.w L_InScreenClose,L_Nul		//$00$01$10$00
	dc.b "screen clos",$80+"e","I0",-1
	dc.w L_InScreenDisplay,L_Nul		//$00$01$12$00
	dc.b "screen displa","y"+$80,"I0,0,0,0,0",-1
	dc.w L_InScreenOffset,L_Nul		//$00$01$11$00
	dc.b "screen offse","t"+$80,"I0,0,0",-1
	dc.w L_Syntax,L_Syntax			//$01$01$0F$00
	dc.b "screen siz","e"+$80,-1
	dc.w L_Nul,L_FnScreenColour		//$01$00$11$00
	dc.b "screen colou","r"+$80,"0",-1
	dc.w L_InScreenToFront0,L_Nul		//$00$01$14$00
	dc.b "!screen to fron","t"+$80,"I",-2
	dc.w L_InScreenToFront1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InScreenToBack0,L_Nul		//$00$01$13$00
	dc.b "!screen to bac","k"+$80,"I",-2
	dc.w L_InScreenToBack1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InScreenHide0,L_Nul		//$00$01$10$00
	dc.b "!screen hid","e"+$80,"I",-2
	dc.w L_InScreenHide1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InScreenShow0,L_Nul		//$00$01$10$00
	dc.b "!screen sho","w"+$80,"I",-2
	dc.w L_InScreenShow1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InScreenSwap0,L_Nul		//$00$01$10$00
	dc.b "!screen swa","p"+$80,"I",-2
	dc.w L_InScreenSwap1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InSaveIff1,L_Nul			//$00$01$0D$00
	dc.b "!save if","f"+$80,"I2",-2
	dc.w L_InSaveIff2,L_Nul			//$2D$01$05$00
	dc.b $80,"I2,0",-1
	dc.w L_InView,L_Nul			//$00$01$08$00
	dc.b "vie","w"+$80,"I",-1
	dc.w L_InAutoViewOff,L_Nul		//$00$01$11$00
	dc.b "auto view of","f"+$80,"I",-1
	dc.w L_InAutoViewOn,L_Nul		//$00$01$10$00
	dc.b "auto view o","n"+$80,"I",-1
	dc.w L_Nul,L_FnScreenBase		//$01$00$0F$00
	dc.b "screen bas","e"+$80,"0",-1
	dc.w L_Nul,L_FnScreenWidth0		//$01$00$11$00
	dc.b "!screen widt","h"+$80,"0",-2
	dc.w L_Nul,L_FnScreenWidth1		//$01$19$05$00
	dc.b $80,"00",-1
	dc.w L_Nul,L_FnScreenHeight0		//$01$00$12$00
	dc.b "!screen heigh","t"+$80,"0",-2
	dc.w L_Nul,L_FnScreenHeight1		//$01$19$05$00
	dc.b $80,"00",-1
	dc.w L_InGetPalette1,L_Nul		//$00$01$10$00
	dc.b "!get palett","e"+$80,"I0",-2
	dc.w L_InGetPalette2,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0",-1
	dc.w L_InCls0,L_Nul			//$00$01$08$00
	dc.b "!cl","s"+$80,"I",-2
	dc.w L_InCls1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-2
	dc.w L_InCls5,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0t0,0",-1
	dc.w L_InDefScroll,L_Nul		//$00$01$0E$00
	dc.b "def scrol","l"+$80,"I0,0,0t0,0,0,0",-1
* Conversions de coordonnees
	dc.w L_Nul,L_FnXHard1			//$01$00$0B$00
	dc.b "!x har","d"+$80,"00",-2
	dc.w L_Nul,L_FnXHard2			//$01$19$05$00
	dc.b $80,"00,0",-1
	dc.w L_Nul,L_FnYHard1			//$01$00$0B$00
	dc.b "!y har","d"+$80,"00",-2
	dc.w L_Nul,L_FnYHard2			//$01$19$05$00
	dc.b $80,"00,0",-1
	dc.w L_Nul,L_FnXScreen1			//$01$00$0D$00
	dc.b "!x scree","n"+$80,"00",-2
	dc.w L_Nul,L_FnXScreen2			//$01$19$05$00
	dc.b $80,"00,0",-1
	dc.w L_Nul,L_FnYScreen1			//$01$00$0D$00
	dc.b "!y scree","n"+$80,"00",-2
	dc.w L_Nul,L_FnYScreen2			//$01$19$05$00
	dc.b $80,"00,0",-1
	dc.w L_Nul,L_FnXText			//$01$00$0A$00
	dc.b "x tex","t"+$80,"00",-1
	dc.w L_Nul,L_FnYText			//$01$00$0A$00
	dc.b "y tex","t"+$80,"00",-1

	dc.w L_InScreen,L_Nul			//$00$18$0B$00
	dc.b "!scree","n"+$80,"I0",-3
	dc.w L_Nul,L_FnScreen			//$01$19$05$00
	dc.b $80,"0",-1
	dc.w L_Nul,L_FnHires			//$01$00$09$00
	dc.b "hire","s"+$80,"0",-1
	dc.w L_Nul,L_FnLowres			//$01$00$0A$00
	dc.b "lowre","s"+$80,"0",-1
	dc.w L_InDualPlayfield,L_Nul		//$00$01$12$00
	dc.b "dual playfiel","d"+$80,"I0,0",-1
	dc.w L_InDualPriority,L_Nul		//$00$01$11$00
	dc.b "dual priorit","y"+$80,"I0,0",-1
	dc.w L_InWtVbl,L_Nul			//$00$01$0C$00
	dc.b "wait vb","l"+$80,"I",-1
	dc.w L_InDefaultPalette,L_Nul		//$11$01$13$00
	dc.b "default palett","e"+$80,"I",-1
	dc.w L_InDefault,L_Nul			//$00$01$0B$00
	dc.b "defaul","t"+$80,"I",-1
	dc.w L_InPalette,L_Nul			//$12$01$0B$00
	dc.b "palett","e"+$80,"I",-1
	dc.w L_InColourBack,L_Nul		//$00$01$0F$00
	dc.b "colour bac","k"+$80,"I0",-1
	dc.w L_InColour,L_Nul			//$00$18$0B$00
	dc.b "!colou","r"+$80,"I0,0",-3
	dc.w L_Nul,L_FnColour			//$01$19$05$00
	dc.b $80,"00",-1
	dc.w L_InFlashOff,L_Nul			//$00$01$0D$00
	dc.b "flash of","f"+$80,"I",-1
	dc.w L_InFlash,L_Nul			//$00$01$09$00
	dc.b "flas","h"+$80,"I0,2",-1
	dc.w L_InShiftOff,L_Nul			//$00$01$0D$00
	dc.b "shift of","f"+$80,"I",-1
	dc.w L_InShiftUp,L_Nul			//$00$01$0C$00
	dc.b "shift u","p"+$80,"I0,0,0,0",-1
	dc.w L_InShiftDown,L_Nul		//$00$01$0E$00
	dc.b "shift dow","n"+$80,"I0,0,0,0",-1
	dc.w L_InSetRainbow6,L_Nul		//$00$01$10$00
	dc.b "!set rainbo","w"+$80,"I0,0,0,2,2,2",-2
	dc.w L_InSetRainbow7,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,2,2,2,0",-1
	dc.w L_InRainbowDel0,L_Nul		//$00$01$10$00
	dc.b "!rainbow de","l"+$80,"I",-2
	dc.w L_InRainbowDel1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InRainbow,L_Nul			//$00$01$0B$00
	dc.b "rainbo","w"+$80,"I0,0,0,0",-1
	dc.w L_InRain,L_FnRain			//$2B$1A$08$00
	dc.b "rai","n"+$80,"V00,0",-1
	dc.w L_InFade,L_Nul			//$20$01$08$00
	dc.b "fad","e"+$80,"I",-1
	dc.w L_Nul,L_FnPhyBase			//$01$00$0B$00
	dc.b "phybas","e"+$80,"00",-1
	dc.w L_Nul,L_FnPhysic0			//$01$00$0B$00
	dc.b "!physi","c"+$80,"0",-2
	dc.w L_Nul,L_FnPhysic1			//$01$19$05$00
	dc.b $80,"00",-1
	dc.w L_InAutoback,L_Nul			//$00$01$0C$00
	dc.b "autobac","k"+$80,"I0",-1
* Instructions graphiques
	dc.w L_InPlot2,L_Nul			//$00$01$09$00
	dc.b "!plo","t"+$80,"I0,0",-2
	dc.w L_InPlot3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0",-1
	dc.w L_Nul,L_FnPoint			//$01$00$09$00
	dc.b "poin","t"+$80,"00,0",-1
	dc.w L_InDrawTo,L_Nul			//$00$01$0B$00
	dc.b "draw t","o"+$80,"I0,0",-1
	dc.w L_InDraw,L_Nul			//$00$01$08$00
	dc.b "dra","w"+$80,"I0,0t0,0",-1
	dc.w L_InEllipse,L_Nul			//$00$01$0B$00
	dc.b "ellips","e"+$80,"I0,0,0,0",-1
	dc.w L_InCircle,L_Nul			//$00$01$0A$00
	dc.b "circl","e"+$80,"I0,0,0",-1
	dc.w L_InPolyline,L_Nul			//$18$01$0C$00
	dc.b "polylin","e"+$80,"I",-1
	dc.w L_InPolygon,L_Nul			//$18$01$0B$00
	dc.b "polygo","n"+$80,"I",-1
	dc.w L_InBar,L_Nul			//$00$01$07$00
	dc.b "ba","r"+$80,"I0,0t0,0",-1
	dc.w L_InBox,L_Nul			//$00$01$07$00
	dc.b "bo","x"+$80,"I0,0t0,0",-1
	dc.w L_InPaint2,L_Nul			//$00$01$0A$00
	dc.b "!pain","t"+$80,"I0,0",-2
	dc.w L_InPaint3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0",-1
	dc.w L_InGrLocate,L_Nul			//$00$01$0D$00
	dc.b "gr locat","e"+$80,"I0,0",-1
	dc.w L_Nul,L_FnTextLength		//$01$00$0F$00
	dc.b "text lengt","h"+$80,"02",-1
	dc.w L_Nul,L_FnTextStyle		//$01$00$0F$00
	dc.b "text style","s"+$80,"0",-1
	dc.w L_Nul,L_FnTextBase			//$01$00$0D$00
	dc.b "text bas","e"+$80,"0",-1
	dc.w L_InText,L_Nul			//$00$01$08$00
	dc.b "tex","t"+$80,"I0,0,2",-1
	dc.w L_InSetText,L_Nul			//$00$01$0C$00
	dc.b "set tex","t"+$80,"I0",-1
	dc.w L_InSetPaint,L_Nul			//$00$01$0D$00
	dc.b "set pain","t"+$80,"I0",-1
	dc.w L_InGetFonts,L_Nul			//$00$01$0D$00
	dc.b "get font","s"+$80,"I",-1
	dc.w L_InGetDiscFonts,L_Nul		//$00$01$12$00
	dc.b "get disc font","s"+$80,"I",-1
	dc.w L_InGetRomFonts,L_Nul		//$00$01$11$00
	dc.b "get rom font","s"+$80,"I",-1
	dc.w L_InSetFont,L_Nul			//$00$01$0C$00
	dc.b "set fon","t"+$80,"I0",-1
	dc.w L_Nul,L_FnFont			//$01$00$09$00
	dc.b "font","$"+$80,"20",-1
	dc.w L_InHSlider,L_Nul			//$00$01$0B$00
	dc.b "hslide","r"+$80,"I0,0t0,0,0,0,0",-1
	dc.w L_InVSlider,L_Nul			//$00$01$0B$00
	dc.b "vslide","r"+$80,"I0,0t0,0,0,0,0",-1
	dc.w L_InSetSlider,L_Nul		//$00$01$0E$00
	dc.b "set slide","r"+$80,"I0,0,0,0,0,0,0,0",-1
	dc.w L_InSetPattern,L_Nul		//$00$01$0F$00
	dc.b "set patter","n"+$80,"I0",-1
	dc.w L_InSetLine,L_Nul			//$00$01$0C$00
	dc.b "set lin","e"+$80,"I0",-1
	dc.w L_InInk1,L_Nul			//$00$01$08$00
	dc.b "!in","k"+$80,"I0",-2
	dc.w L_InInk2,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0",-2
	dc.w L_InInk3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0",-1
	dc.w L_InGrWriting,L_Nul		//$00$01$0E$00
	dc.b "gr writin","g"+$80,"I0",-1
	dc.w L_InClip0,L_Nul			//$00$01$09$00
	dc.b "!cli","p"+$80,"I",-2
	dc.w L_InClip4,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0t0,0",-1
	dc.w L_InSetTempras0,L_Nul		//$00$01$10$00
	dc.b "!set tempra","s"+$80,"I",-2
	dc.w L_InSetTempras1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-2
	dc.w L_InSetTempras2,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0",-1
	dc.w L_InAppear3,L_Nul			//$00$01$0B$00
	dc.b "!appea","r"+$80,"I0t0,0",-2
	dc.w L_InAppear4,L_Nul			//$2D$01$05$00
	dc.b $80,"I0t0,0,0",-1
	dc.w L_InZoom,L_Nul			//$00$01$08$00
	dc.b "zoo","m"+$80,"I0,0,0,0,0t0,0,0,0,0",-1

* Blocs
	dc.w L_InGetCBlock,L_Nul		//$00$01$0E$00
	dc.b "get cbloc","k"+$80,"I0,0,0,0,0",-1
	dc.w L_InPutCBlock1,L_Nul		//$00$01$0F$00
	dc.b "!put cbloc","k"+$80,"I0",-2
	dc.w L_InPutCBlock3,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0",-1
	dc.w L_InDelCBlock0,L_Nul		//$00$01$0F$00
	dc.b "!del cbloc","k"+$80,"I",-2
	dc.w L_InDelCBlock1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InGetBlock5,L_Nul		//$00$01$0E$00
	dc.b "!get bloc","k"+$80,"I0,0,0,0,0",-2
	dc.w L_InGetBlock6,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0,0,0",-1
	dc.w L_InPutBlock1,L_Nul		//$00$01$0E$00
	dc.b "!put bloc","k"+$80,"I0",-2
	dc.w L_InPutBlock3,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0",-2
	dc.w L_InPutBlock4,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0",-2
	dc.w L_InPutBlock5,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0,0",-1
	dc.w L_InDelBlock0,L_Nul		//$00$01$0E$00
	dc.b "!del bloc","k"+$80,"I",-2
	dc.w L_InDelBlock1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1

* Instructions clavier
	dc.w L_InKeySpeed,L_Nul			//$00$01$0D$00
	dc.b "key spee","d"+$80,"I0,0",-1
	dc.w L_Nul,L_FnKeyState			//$01$00$0D$00
	dc.b "key stat","e"+$80,"00",-1
	dc.w L_Nul,L_FnKeyShift			//$01$00$0D$00
	dc.b "key shif","t"+$80,"0",-1
	dc.w L_Nul,L_FnJoy			//$01$00$07$00
	dc.b "jo","y"+$80,"00",-1
	dc.w L_Nul,L_FnJup			//$01$00$07$00
	dc.b "ju","p"+$80,"00",-1
	dc.w L_Nul,L_FnJdown			//$01$00$09$00
	dc.b "jdow","n"+$80,"00",-1
	dc.w L_Nul,L_FnJleft			//$01$00$09$00
	dc.b "jlef","t"+$80,"00",-1
	dc.w L_Nul,L_FnJright			//$01$00$0A$00
	dc.b "jrigh","t"+$80,"00",-1
	dc.w L_Nul,L_FnFire			//$01$00$08$00
	dc.b "fir","e"+$80,"00",-1
	dc.w L_Nul,L_FnTrue			//$01$1F$08$00
	dc.b "tru","e"+$80,"0",-1
	dc.w L_Nul,L_FnFalse			//$01$1E$09$00
	dc.b "fals","e"+$80,"0",-1
	dc.w L_InPutKey,L_Nul			//$00$01$0B$00
	dc.b "put ke","y"+$80,"I2",-1
	dc.w L_Nul,L_FnScancode			//$01$00$0C$00
	dc.b "scancod","e"+$80,"0",-1
	dc.w L_Nul,L_FnScanshift		//$01$00$0D$00
	dc.b "scanshif","t"+$80,"0",-1
	dc.w L_InClearKey,L_Nul			//$00$01$0D$00
	dc.b "clear ke","y"+$80,"I",-1
	dc.w L_InWtKy,L_Nul			//$00$01$0C$00
	dc.b "wait ke","y"+$80,"I",-1
	dc.w L_InWait,L_Nul			//$00$01$08$00
	dc.b "wai","t"+$80,"I0",-1
	dc.w L_InKeyD,L_FnKeyD			//$2B$1A$08$00
	dc.b "key","$"+$80,"V20",-1
	dc.w L_Nul,L_FnScan1			//$01$00$0A$00
	dc.b "!scan","$"+$80,"20",-2
	dc.w L_Nul,L_FnScan2			//$01$19$05$00
	dc.b $80,"20,0",-1
	dc.w L_InTimer,L_FnTimer		//$2B$1A$09$00
	dc.b "time","r"+$80,"V0",-1

* Instruction fenetres
	dc.w L_InWindopen5,L_Nul		//$00$01$0E$00
	dc.b "!wind ope","n"+$80,"I0,0,0,0,0",-2
	dc.w L_InWindopen6,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0,0,0",-2
	dc.w L_InWindopen7,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0,0,0,0",-1
	dc.w L_InWindclose,L_Nul		//$00$01$0E$00
	dc.b "wind clos","e"+$80,"I",-1
	dc.w L_InWindsave,L_Nul			//$00$01$0D$00
	dc.b "wind sav","e"+$80,"I",-1
	dc.w L_InWindmove,L_Nul			//$00$01$0D$00
	dc.b "wind mov","e"+$80,"I0,0",-1
	dc.w L_InWindsize,L_Nul			//$00$01$0D$00
	dc.b "wind siz","e"+$80,"I0,0",-1
	dc.w L_InWindow,L_Nul			//$00$01$0A$00
	dc.b "windo","w"+$80,"I0",-1
	dc.w L_Nul,L_FnWindon			//$01$00$0A$00
	dc.b "windo","n"+$80,"0",-1

	dc.w L_InLocate,L_Nul			//$00$01$0A$00
	dc.b "locat","e"+$80,"I0,0",-1
	dc.w L_InClw,L_Nul			//$00$01$07$00
	dc.b "cl","w"+$80,"I",-1
	dc.w L_InHome,L_Nul			//$00$01$08$00
	dc.b "hom","e"+$80,"I",-1
	dc.w L_InCursPen,L_Nul			//$00$01$0C$00
	dc.b "curs pe","n"+$80,"I0",-1
	dc.w L_Nul,L_FnPenD			//$01$00$08$00
	dc.b "pen","$"+$80,"20",-1
	dc.w L_Nul,L_FnPaperD			//$01$00$0A$00
	dc.b "paper","$"+$80,"20",-1
	dc.w L_Nul,L_FnAt			//$01$00$06$00
	dc.b "a","t"+$80,"20,0",-1
	dc.w L_InPen,L_Nul			//$00$01$07$00
	dc.b "pe","n"+$80,"I0",-1
	dc.w L_InPaper,L_Nul			//$00$01$09$00
	dc.b "pape","r"+$80,"I0",-1
	dc.w L_InCentre,L_Nul			//$00$01$0A$00
	dc.b "centr","e"+$80,"I2",-1
	dc.w L_InBorder,L_Nul			//$00$01$0A$00
	dc.b "borde","r"+$80,"I0,0,0",-1
	dc.w L_InWriting1,L_Nul			//$00$01$0C$00
	dc.b "!writin","g"+$80,"I0",-2
	dc.w L_InWriting2,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0",-1
	dc.w L_InTitleTop,L_Nul			//$00$01$0D$00
	dc.b "title to","p"+$80,"I2",-1
	dc.w L_InTitleBottom,L_Nul		//$00$01$10$00
	dc.b "title botto","m"+$80,"I2",-1
	dc.w L_InCursOff,L_Nul			//$00$01$0C$00
	dc.b "curs of","f"+$80,"I",-1
	dc.w L_InCursOn,L_Nul			//$00$01$0B$00
	dc.b "curs o","n"+$80,"I",-1
	dc.w L_InInverseOff,L_Nul		//$00$01$0F$00
	dc.b "inverse of","f"+$80,"I",-1
	dc.w L_InInverseOn,L_Nul		//$00$01$0E$00
	dc.b "inverse o","n"+$80,"I",-1
	dc.w L_InUnderOff,L_Nul			//$00$01$0D$00
	dc.b "under of","f"+$80,"I",-1
	dc.w L_InUnderOn,L_Nul			//$00$01$0C$00
	dc.b "under o","n"+$80,"I",-1
	dc.w L_InShadeOff,L_Nul			//$00$01$0D$00
	dc.b "shade of","f"+$80,"I",-1
	dc.w L_InShadeOn,L_Nul			//$00$01$0C$00
	dc.b "shade o","n"+$80,"I",-1
	dc.w L_InScrollOff,L_Nul		//$00$01$0E$00
	dc.b "scroll of","f"+$80,"I",-1
	dc.w L_InScrollOn,L_Nul			//$00$01$0D$00
	dc.b "scroll o","n"+$80,"I",-1
	dc.w L_InScroll,L_Nul			//$00$01$0A$00
	dc.b "scrol","l"+$80,"I0",-1
	dc.w L_Nul,L_FnCup			//$01$00$08$00
	dc.b "cup","$"+$80,"2",-1
	dc.w L_Nul,L_FnCdown			//$01$00$0A$00
	dc.b "cdown","$"+$80,"2",-1
	dc.w L_Nul,L_FnCleft			//$01$00$0A$00
	dc.b "cleft","$"+$80,"2",-1
	dc.w L_Nul,L_FnCright			//$01$00$0B$00
	dc.b "cright","$"+$80,"2",-1
	dc.w L_InCup,L_Nul			//$00$01$07$00
	dc.b "cu","p"+$80,"I",-1
	dc.w L_InCdown,L_Nul			//$00$01$09$00
	dc.b "cdow","n"+$80,"I",-1
	dc.w L_InCleft,L_Nul			//$00$01$09$00
	dc.b "clef","t"+$80,"I",-1
	dc.w L_InCright,L_Nul			//$00$01$0A$00
	dc.b "crigh","t"+$80,"I",-1
	dc.w L_InMemorizeX,L_Nul		//$00$01$0E$00
	dc.b "memorize ","x"+$80,"I",-1
	dc.w L_InMemorizeY,L_Nul		//$00$01$0E$00
	dc.b "memorize ","y"+$80,"I",-1
	dc.w L_Nul,L_FnCMoveD			//$01$00$0A$00
	dc.b "cmove","$"+$80,"20,0",-1
	dc.w L_InCmove,L_Nul			//$00$01$09$00
	dc.b "cmov","e"+$80,"I0,0",-1
	dc.w L_InCline0,L_Nul			//$00$01$0A$00
	dc.b "!clin","e"+$80,"I",-2
	dc.w L_InCline1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InHScroll,L_Nul			//$00$01$0B$00
	dc.b "hscrol","l"+$80,"I0",-1
	dc.w L_InVScroll,L_Nul			//$00$01$0B$00
	dc.b "vscrol","l"+$80,"I0",-1
	dc.w L_InSetTab,L_Nul			//$00$01$0B$00
	dc.b "set ta","b"+$80,"I0",-1
	dc.w L_InSetCurs,L_Nul			//$00$01$0C$00
	dc.b "set cur","s"+$80,"I0,0,0,0,0,0,0,0",-1
	dc.w L_Nul,L_FnXCurs			//$01$00$0A$00
	dc.b "x cur","s"+$80,"0",-1
	dc.w L_Nul,L_FnYCurs			//$01$00$0A$00
	dc.b "y cur","s"+$80,"0",-1
	dc.w L_Nul,L_FnXGraphic			//$01$00$0D$00
	dc.b "x graphi","c"+$80,"00",-1
	dc.w L_Nul,L_FnYGraphic			//$01$00$0D$00
	dc.b "y graphi","c"+$80,"00",-1
	dc.w L_Nul,L_FnXGr			//$01$00$07$00
	dc.b "xg","r"+$80,"0",-1
	dc.w L_Nul,L_FnYGr			//$01$00$07$00
	dc.b "yg","r"+$80,"0",-1


* Zones
	dc.w L_InReserveZone0,L_Nul		//$00$01$11$00
	dc.b "!reserve zon","e"+$80,"I",-2
	dc.w L_InReserveZone1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InResetZone0,L_Nul		//$00$01$0F$00
	dc.b "!reset zon","e"+$80,"I",-2
	dc.w L_InResetZone1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InSetZone,L_Nul			//$00$01$0C$00
	dc.b "set zon","e"+$80,"I0,0,0t0,0",-1
	dc.w L_Nul,L_FnZone2			//$01$00$09$00
	dc.b "!zon","e"+$80,"00,0",-2
	dc.w L_Nul,L_FnZone3			//$01$19$05$00
	dc.b $80,"00,0,0",-1
	dc.w L_Nul,L_FnHZone2			//$01$00$0A$00
	dc.b "!hzon","e"+$80,"00,0",-2
	dc.w L_Nul,L_FnHZone3			//$01$19$05$00
	dc.b $80,"00,0,0",-1
	dc.w L_Nul,L_FnScIn2			//$01$00$09$00
	dc.b "!sci","n"+$80,"00,0",-2
	dc.w L_Nul,L_FnScIn3			//$01$19$05$00
	dc.b $80,"00,0,0",-1
	dc.w L_Nul,L_FnMouseScreen		//$01$00$10$00
	dc.b "mouse scree","n"+$80,"0",-1
	dc.w L_Nul,L_FnMouseZone		//$01$00$0E$00
	dc.b "mouse zon","e"+$80,"0",-1


* Instructions disques
	dc.w L_InSetInput,L_Nul			//$00$01$0D$00
	dc.b "set inpu","t"+$80,"I0,0",-1
	dc.w L_InCloseWorkbench,L_Nul		//$00$01$13$00
	dc.b "close workbenc","h"+$80,"I",-1
	dc.w L_InCloseEditor,L_Nul		//$00$01$10$00
	dc.b "close edito","r"+$80,"I",-1
	dc.w L_Nul,L_FnDirFirst			//$01$00$0E$00
	dc.b "dir first","$"+$80,"22",-1
	dc.w L_Nul,L_FnFillNext			//$01$00$0D$00
	dc.b "dir next","$"+$80,"2",-1
	dc.w L_Nul,L_FnExist			//$01$00$09$00
	dc.b "exis","t"+$80,"02",-1
	dc.w L_InDirD,L_FnDirD			//$2B$1A$08$00
	dc.b "dir","$"+$80,"V2",-1
	dc.w L_InLDirW0,L_Nul			//$51$01$0B$00
	dc.b "!ldir/","w"+$80,"I",-2
	dc.w L_InLDirW1,L_Nul			//$51$01$05$00
	dc.b $80,"I2",-1
	dc.w L_InDirW0,L_Nul			//$51$01$0A$00
	dc.b "!dir/","w"+$80,"I",-2
	dc.w L_InDirW1,L_Nul			//$51$01$05$00
	dc.b $80,"I2",-1
	dc.w L_InLDir0,L_Nul			//$51$01$09$00
	dc.b "!ldi","r"+$80,"I",-2
	dc.w L_InLDir1,L_Nul			//$51$01$05$00
	dc.b $80,"I2",-1
	dc.w L_InDir0,L_Nul			//$51$01$08$00
	dc.b "!di","r"+$80,"I",-2
	dc.w L_InDir1,L_Nul			//$51$01$05$00
	dc.b $80,"I2",-1
	dc.w L_InSetDir1,L_Nul			//$00$01$0B$00
	dc.b "set di","r"+$80,"I0",-2
	dc.w L_InSetDir2,L_Nul			//$2D$01$0B$00
	dc.b "set di","r"+$80,"I0,2",-1
	dc.w L_InLoadIff1,L_Nul			//$00$01$0D$00
	dc.b "!load if","f"+$80,"I2",-2
	dc.w L_InLoadIff2,L_Nul			//$2D$01$05$00
	dc.b $80,"I2,0",-1
	dc.w L_InMaskIff,L_Nul			//$00$01$0C$00
	dc.b "mask if","f"+$80,"I0",-1
	dc.w L_Nul,L_FnPicture			//$01$00$0B$00
	dc.b "pictur","e"+$80,"0",-1
	dc.w L_InBload,L_Nul			//$00$01$09$00
	dc.b "bloa","d"+$80,"I2,0",-1
	dc.w L_InBSave,L_Nul			//$00$01$09$00
	dc.b "bsav","e"+$80,"I2,0t0",-1
	dc.w L_InPLoad,L_Nul			//$00$01$09$00
	dc.b "ploa","d"+$80,"I2,0",-1
	dc.w L_InSave1,L_Nul			//$00$01$09$00
	dc.b "!sav","e"+$80,"I2",-2
	dc.w L_InSave2,L_Nul			//$2D$01$05$00
	dc.b $80,"I2,0",-1
	dc.w L_InLoad1,L_Nul			//$00$01$09$00
	dc.b "!loa","d"+$80,"I2",-2
	dc.w L_InLoad2,L_Nul			//$2D$01$05$00
	dc.b $80,"I2,0",-1

	dc.w L_Nul,L_FnDFree			//$01$00$09$00
	dc.b "dfre","e"+$80,"0",-1
	dc.w L_InMkDir,L_Nul			//$00$01$09$00
	dc.b "mkdi","r"+$80,"I2",-1
	dc.w L_Nul,L_FnLof			//$01$00$07$00
	dc.b "lo","f"+$80,"00",-1
	dc.w L_Nul,L_FnEof			//$01$00$07$00
	dc.b "eo","f"+$80,"00",-1
	dc.w L_InPof,L_FnPof			//$2B$1A$07$00
	dc.b "po","f"+$80,"V00",-1
	dc.w L_Nul,L_FnPort			//$01$00$08$00
	dc.b "por","t"+$80,"00",-1
	dc.w L_InOpenRandom,L_Nul		//$00$01$0F$00
	dc.b "open rando","m"+$80,"I0,2",-1
	dc.w L_InOpenIn,L_Nul			//$00$01$0B$00
	dc.b "open i","n"+$80,"I0,2",-1
	dc.w L_InOpenOut,L_Nul			//$00$01$0C$00
	dc.b "open ou","t"+$80,"I0,2",-1
	dc.w L_InOpenPort,L_Nul			//$00$01$0D$00
	dc.b "open por","t"+$80,"I0,2",-1
	dc.w L_InAppend,L_Nul			//$00$01$0A$00
	dc.b "appen","d"+$80,"I0,2",-1
	dc.w L_InClose0,L_Nul			//$00$01$0A$00
	dc.b "!clos","e"+$80,"I",-2
	dc.w L_InClose1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InParent,L_Nul			//$00$01$0A$00
	dc.b "paren","t"+$80,"I",-1
	dc.w L_InRename,L_Nul			//$00$01$0A$00
	dc.b "renam","e"+$80,"I2t2",-1
	dc.w L_InKill,L_Nul			//$00$01$08$00
	dc.b "kil","l"+$80,"I2",-1
	dc.w L_Nul,L_FnDrive			//$01$00$09$00
	dc.b "driv","e"+$80,"02",-1
	dc.w L_InField,L_Nul			//$19$01$09$00
	dc.b "fiel","d"+$80,"I",-1
	dc.w L_Nul,L_FnFileSelector1		//$01$26$0A$00
	dc.b "!fsel","$"+$80,"22",-2
	dc.w L_Nul,L_FnFileSelector2		//$01$26$05$00
	dc.b $80,"22,2",-2
	dc.w L_Nul,L_FnFileSelector3		//$01$26$05$00
	dc.b $80,"22,2,2",-2
	dc.w L_Nul,L_FnFileSelector4		//$01$26$05$00
	dc.b $80,"22,2,2,2",-1

* Instructions SPRITES
	dc.w L_InSetSpriteBuffer,L_Nul		//$00$01$15$00
	dc.b "set sprite buffe","r"+$80,"I0",-1
	dc.w L_InSpriteOff0,L_Nul		//$00$01$0F$00
	dc.b "!sprite of","f"+$80,"I",-2
	dc.w L_InSpriteOff1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InSpritePriority,L_Nul		//$00$01$13$00
	dc.b "sprite priorit","y"+$80,"I0",-1
	dc.w L_InSpriteUpdateOff,L_Nul		//$00$01$15$00
	dc.b "sprite update of","f"+$80,"I",-1
	dc.w L_InSpriteUpdateOn,L_Nul		//$00$01$14$00
	dc.b "sprite update o","n"+$80,"I",-1
	dc.w L_InSpriteUpdate,L_Nul		//$00$01$11$00
	dc.b "sprite updat","e"+$80,"I",-1
	dc.w L_Nul,L_FnSpriteBobCol1		//$01$00$12$00
	dc.b "!spritebob co","l"+$80,"00",-2
	dc.w L_Nul,L_FnSpriteBobCol3		//$01$19$05$00
	dc.b $80,"00,0t0",-1
	dc.w L_Nul,L_FnSpriteCol1		//$01$00$0F$00
	dc.b "!sprite co","l"+$80,"00",-2
	dc.w L_Nul,L_FnSpriteCol3		//$01$19$05$00
	dc.b $80,"00,0t0",-1
	dc.w L_InSetHardcol,L_Nul		//$00$01$0F$00
	dc.b "set hardco","l"+$80,"I0,0",-1
	dc.w L_Nul,L_FnHardcol			//$01$00$0B$00
	dc.b "hardco","l"+$80,"00",-1
	dc.w L_Nul,L_FnSpriteBase		//$01$00$0F$00
	dc.b "sprite bas","e"+$80,"00",-1
	dc.w L_Nul,L_FnIconBase			//$01$00$0D$00
	dc.b "icon bas","e"+$80,"00",-1
	dc.w L_InSprite,L_Nul			//$00$01$0A$00
	dc.b "sprit","e"+$80,"I0,0,0,0",-1

	dc.w L_InBobOff0,L_Nul			//$00$01$0C$00
	dc.b "!bob of","f"+$80,"I",-2
	dc.w L_InBobOff1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InBobUpdateOff,L_Nul		//$00$01$12$00
	dc.b "bob update of","f"+$80,"I",-1
	dc.w L_InBobUpdateOn,L_Nul		//$00$01$11$00
	dc.b "bob update o","n"+$80,"I",-1
	dc.w L_InBobUpdate,L_Nul		//$00$01$0E$00
	dc.b "bob updat","e"+$80,"I",-1
	dc.w L_InBobClear,L_Nul			//$00$01$0D$00
	dc.b "bob clea","r"+$80,"I",-1
	dc.w L_InBobDraw,L_Nul			//$00$01$0C$00
	dc.b "bob dra","w"+$80,"I",-1
	dc.w L_Nul,L_FnBobSpriteCol1		//$01$00$12$00
	dc.b "!bobsprite co","l"+$80,"00",-2
	dc.w L_Nul,L_FnBobSpriteCol3		//$01$19$05$00
	dc.b $80,"00,0t0",-1
	dc.w L_Nul,L_FnBobCol1			//$01$00$0C$00
	dc.b "!bob co","l"+$80,"00",-2
	dc.w L_Nul,L_FnBobCol3			//$01$19$05$00
	dc.b $80,"00,0t0",-1
	dc.w L_Nul,L_FnCol			//$01$00$07$00
	dc.b "co","l"+$80,"00",-1
	dc.w L_InLimitBob0,L_Nul		//$00$01$0E$00
	dc.b "!limit bo","b"+$80,"I",-2
	dc.w L_InLimitBob4,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0t0,0",-2
	dc.w L_InLimitBob5,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0t0,0",-1
	dc.w L_InSetBob,L_Nul			//$00$01$0B$00
	dc.b "set bo","b"+$80,"I0,0,0,0",-1
	dc.w L_InBob,L_Nul			//$00$01$07$00
	dc.b "bo","b"+$80,"I0,0,0,0",-1
	dc.w L_InGetSpritePalette0,L_Nul	//$00$01$17$00
	dc.b "!get sprite palett","e"+$80,"I",-2
	dc.w L_InGetSpritePalette1,L_Nul	//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InGetSprite5,L_Nul		//$00$01$0F$00
	dc.b "!get sprit","e"+$80,"I0,0,0t0,0",-2
	dc.w L_InGetSprite6,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0t0,0",-1
	dc.w L_InGetSprite5,L_Nul		//$00$01$0C$00
	dc.b "!get bo","b"+$80,"I0,0,0t0,0",-2
	dc.w L_InGetSprite6,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0,0,0t0,0",-1
	dc.w L_InDelSprite1,L_Nul		//$00$01$0F$00
	dc.b "!del sprit","e"+$80,"I0",-2
	dc.w L_InDelSprite2,L_Nul		//$2D$01$05$00
	dc.b $80,"I0t0",-1
	dc.w L_InDelSprite1,L_Nul		//$00$01$0C$00
	dc.b "!del bo","b"+$80,"I0",-2
	dc.w L_InDelSprite2,L_Nul		//$2D$01$05$00
	dc.b $80,"I0t0",-1
	dc.w L_InDelIcon1,L_Nul			//$00$01$0D$00
	dc.b "!del ico","n"+$80,"I0",-2
	dc.w L_InDelIcon2,L_Nul			//$2D$01$05$00
	dc.b $80,"I0t0",-1
	dc.w L_InInsSprite,L_Nul		//$00$01$0E$00
	dc.b "ins sprit","e"+$80,"I0",-1
	dc.w L_InInsSprite,L_Nul		//$00$01$0B$00
	dc.b "ins bo","b"+$80,"I0",-1
	dc.w L_InInsIcon,L_Nul			//$00$01$0C$00
	dc.b "ins ico","n"+$80,"I0",-1
	dc.w L_InGetIconPalette0,L_Nul		//$00$01$15$00
	dc.b "!get icon palett","e"+$80,"I",-2
	dc.w L_InGetIconPalette1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InGetIcon5,L_Nul			//$00$01$0D$00
	dc.b "!get ico","n"+$80,"I0,0,0t0,0",-2
	dc.w L_InGetIcon6,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0,0t0,0",-1
	dc.w L_InPutBob,L_Nul			//$00$01$0B$00
	dc.b "put bo","b"+$80,"I0",-1
	dc.w L_InPasteBob,L_Nul			//$00$01$0D$00
	dc.b "paste bo","b"+$80,"I0,0,0",-1
	dc.w L_InPasteIcon,L_Nul		//$00$01$0E$00
	dc.b "paste ico","n"+$80,"I0,0,0",-1
	dc.w L_InMakeMask0,L_Nul		//$00$01$0E$00
	dc.b "!make mas","k"+$80,"I",-2
	dc.w L_InMakeMask1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InNoMask0,L_Nul			//$00$01$0C$00
	dc.b "!no mas","k"+$80,"I",-2
	dc.w L_InNoMask1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InMakeIconMask0,L_Nul		//$00$01$13$00
	dc.b "!make icon mas","k"+$80,"I",-2
	dc.w L_InMakeIconMask1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InNoIconMask0,L_Nul		//$00$01$11$00
	dc.b "!no icon mas","k"+$80,"I",-2
	dc.w L_InNoIconMask1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InHotSpot2,L_Nul			//$00$01$0D$00
	dc.b "!hot spo","t"+$80,"I0,0",-2
	dc.w L_InHotSpot3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0",-1
	dc.w L_InPriorityOn,L_Nul		//$00$01$0F$00
	dc.b "priority o","n"+$80,"I",-1
	dc.w L_InPriorityOff,L_Nul		//$00$01$10$00
	dc.b "priority of","f"+$80,"I",-1

	dc.w L_InHideOn,L_Nul			//$00$01$0B$00
	dc.b "hide o","n"+$80,"I",-1
	dc.w L_InHide,L_Nul			//$00$01$08$00
	dc.b "hid","e"+$80,"I",-1
	dc.w L_InShowOn,L_Nul			//$00$01$0B$00
	dc.b "show o","n"+$80,"I",-1
	dc.w L_InShow,L_Nul			//$00$01$08$00
	dc.b "sho","w"+$80,"I",-1
	dc.w L_InChangeMouse,L_Nul		//$00$01$10$00
	dc.b "change mous","e"+$80,"I0",-1
	dc.w L_InXMouse,L_FnXMouse		//$2B$1A$0B$00
	dc.b "x mous","e"+$80,"V0",-1
	dc.w L_InYMouse,L_FnYMouse		//$2B$1A$0B$00
	dc.b "y mous","e"+$80,"V0",-1
	dc.w L_Nul,L_FnMouseKey			//$01$00$0D$00
	dc.b "mouse ke","y"+$80,"0",-1
	dc.w L_Nul,L_FnMouseClick		//$01$00$0F$00
	dc.b "mouse clic","k"+$80,"0",-1
	dc.w L_InLimitMouse0,L_Nul		//$00$01$10$00
	dc.b "!limit mous","e"+$80,"I",-2
	dc.w L_InLimitMouse1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-2
	dc.w L_InLimitMouse4,L_Nul		//$2D$01$05$00
	dc.b $80,"I0,0t0,0",-1

* AMAL!
	dc.w L_InUnFreeze,L_Nul			//$00$01$0C$00
	dc.b "unfreez","e"+$80,"I",-1
	dc.w L_InMoveX2,L_Nul			//$00$01$0B$00
	dc.b "!move ","x"+$80,"I0,3",-2
	dc.w L_InMoveX3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,2t0",-1
	dc.w L_InMoveY2,L_Nul			//$00$01$0B$00
	dc.b "!move ","y"+$80,"I0,3",-2
	dc.w L_InMoveY3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,2t0",-1
	dc.w L_InMoveOff0,L_Nul			//$00$01$0D$00
	dc.b "!move of","f"+$80,"I",-2
	dc.w L_InMoveOff1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InMoveOn0,L_Nul			//$00$01$0C$00
	dc.b "!move o","n"+$80,"I",-2
	dc.w L_InMoveOn1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InMoveFreeze0,L_Nul		//$00$01$10$00
	dc.b "!move freez","e"+$80,"I",-2
	dc.w L_InMoveFreeze1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InAnimOff0,L_Nul			//$00$01$0D$00
	dc.b "!anim of","f"+$80,"I",-2
	dc.w L_InAnimOff1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InAnimOn0,L_Nul			//$00$01$0C$00
	dc.b "!anim o","n"+$80,"I",-2
	dc.w L_InAnimOn1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InAnimFreeze0,L_Nul		//$00$01$10$00
	dc.b "!anim freez","e"+$80,"I",-2
	dc.w L_InAnimFreeze1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InAnim2,L_Nul			//$00$01$08$00
	dc.b "ani","m"+$80,"I0,3",-2
	dc.w L_InAnim3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,2t0",-1
	dc.w L_Nul,L_FnMovon			//$01$00$09$00
	dc.b "movo","n"+$80,"00",-1
	dc.w L_Nul,L_FnChanAn			//$01$00$0A$00
	dc.b "chana","n"+$80,"00",-1
	dc.w L_Nul,L_FnChanMv			//$01$00$0A$00
	dc.b "chanm","v"+$80,"00",-1
	dc.w L_InChannel,L_Nul			//$15$01$0B$00
	dc.b "channe","l"+$80,"I",-1
	dc.w L_InAmreg1,L_FnAmreg1		//$2B$1A$0A$00
	dc.b "!amre","g"+$80,"V00",-2
	dc.w L_InAmreg2,L_FnAmreg2		//$2B$1A$05$00
	dc.b $80,"V00,0",-1
	dc.w L_InAmalOn0,L_Nul			//$00$01$0C$00
	dc.b "!amal o","n"+$80,"I",-2
	dc.w L_InAmalOn1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InAmalOff0,L_Nul			//$00$01$0D$00
	dc.b "!amal of","f"+$80,"I",-2
	dc.w L_InAmalOff1,L_Nul			//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InAmalFreeze0,L_Nul		//$00$01$10$00
	dc.b "!amal freez","e"+$80,"I",-2
	dc.w L_InAmalFreeze1,L_Nul		//$2D$01$05$00
	dc.b $80,"I0",-1
	dc.w L_Nul,L_FnAmalerr			//$01$00$0B$00
	dc.b "amaler","r"+$80,"0",-1
	dc.w L_InAmal2,L_Nul			//$00$01$09$00
	dc.b "!ama","l"+$80,"I0,3",-2
	dc.w L_InAmal3,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,2t0",-1
	dc.w L_InAmPlay2,L_Nul			//$00$01$0B$00
	dc.b "!ampla","y"+$80,"I0,0",-2
	dc.w L_InAmPlay4,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0t0",-1
	dc.w L_InSynchroOn,L_Nul		//$00$01$0E$00
	dc.b "synchro o","n"+$80,"I",-1
	dc.w L_InSynchroOff,L_Nul		//$00$01$0F$00
	dc.b "synchro of","f"+$80,"I",-1
	dc.w L_InSynchro,L_Nul			//$00$01$0B$00
	dc.b "synchr","o"+$80,"I",-1
	dc.w L_InUpdateOff,L_Nul		//$00$01$0E$00
	dc.b "update of","f"+$80,"I",-1
	dc.w L_InUpdateOn,L_Nul			//$00$01$0D$00
	dc.b "update o","n"+$80,"I",-1
	dc.w L_InUpdateEvery,L_Nul		//$00$01$10$00
	dc.b "update ever","y"+$80,"I0",-1
	dc.w L_InUpdate,L_Nul			//$00$01$0A$00
	dc.b "updat","e"+$80,"I",-1
	dc.w L_Nul,L_FnXBob			//$01$00$09$00
	dc.b "x bo","b"+$80,"00",-1
	dc.w L_Nul,L_FnYBob			//$01$00$09$00
	dc.b "y bo","b"+$80,"00",-1
	dc.w L_Nul,L_FnXSprite			//$01$00$0C$00
	dc.b "x sprit","e"+$80,"00",-1
	dc.w L_Nul,L_FnYSprite			//$01$00$0C$00
	dc.b "y sprit","e"+$80,"00",-1

* Memoire / Banques
	dc.w L_InResWork,L_Nul			//$00$01$13$00
	dc.b "reserve as wor","k"+$80,"I0,0",-1
	dc.w L_InResChipWork,L_Nul		//$00$01$18$00
	dc.b "reserve as chip wor","k"+$80,"I0,0",-1
	dc.w L_InResData,L_Nul			//$00$01$13$00
	dc.b "reserve as dat","a"+$80,"I0,0",-1
	dc.w L_InResChipData,L_Nul		//$00$01$18$00
	dc.b "reserve as chip dat","a"+$80,"I0,0",-1
	dc.w L_InErase,L_Nul			//$00$01$09$00
	dc.b "eras","e"+$80,"I0",-1
	dc.w L_InListBank,L_Nul			//$00$01$0D$00
	dc.b "list ban","k"+$80,"I",-1
	dc.w L_Nul,L_FnChipFree			//$01$00$0D$00
	dc.b "chip fre","e"+$80,"0",-1
	dc.w L_Nul,L_FnFastFree			//$01$00$0D$00
	dc.b "fast fre","e"+$80,"0",-1
	dc.w L_InFill,L_Nul			//$00$01$08$00
	dc.b "fil","l"+$80,"I0t0,0",-1
	dc.w L_InCopy,L_Nul			//$00$01$08$00
	dc.b "cop","y"+$80,"I0,0t0",-1
	dc.w L_Nul,L_FnHunt			//$01$00$08$00
	dc.b "hun","t"+$80,"00t0,2",-1
	dc.w L_InPoke,L_Nul			//$00$01$08$00
	dc.b "pok","e"+$80,"I0,0",-1
	dc.w L_InLoke,L_Nul			//$00$01$08$00
	dc.b "lok","e"+$80,"I0,0",-1
	dc.w L_Nul,L_FnPeek			//$01$00$08$00
	dc.b "pee","k"+$80,"00",-1
	dc.w L_Nul,L_FnDeek			//$01$00$08$00
	dc.b "dee","k"+$80,"00",-1
	dc.w L_Nul,L_FnLeek			//$01$00$08$00
	dc.b "lee","k"+$80,"00",-1
	dc.w L_InBset,L_NoFlag			//$56$01$08$00
	dc.b "bse","t"+$80,"I0,0",-1
	dc.w L_InBclr,L_NoFlag			//$56$01$08$00
	dc.b "bcl","r"+$80,"I0,0",-1
	dc.w L_InBchg,L_NoFlag			//$56$01$08$00
	dc.b "bch","g"+$80,"I0,0",-1
	dc.w L_NoFlag,L_FnBtst			//$01$27$08$00
	dc.b "bts","t"+$80,"00,0",-1
 	dc.w L_InRorB,L_NoFlag			//$56$01$09$00
	dc.b "ror.","b"+$80,"I0,0",-1
 	dc.w L_InRorW,L_NoFlag			//$56$01$09$00
	dc.b "ror.","w"+$80,"I0,0",-1
 	dc.w L_InRorL,L_NoFlag			//$56$01$09$00
	dc.b "ror.","l"+$80,"I0,0",-1
 	dc.w L_InRolB,L_NoFlag			//$56$01$09$00
	dc.b "rol.","b"+$80,"I0,0",-1
 	dc.w L_InRolW,L_NoFlag			//$56$01$09$00
	dc.b "rol.","w"+$80,"I0,0",-1
 	dc.w L_InRolL,L_NoFlag			//$56$01$09$00
	dc.b "rol.","l"+$80,"I0,0",-1
	dc.w L_InAReg,L_FnAReg			//$2B$1A$08$00
	dc.b "are","g"+$80,"V00",-1
	dc.w L_InDReg,L_FnDReg			//$2B$1A$08$00
	dc.b "dre","g"+$80,"V00",-1

* Copper
	dc.w L_InCopOn,L_Nul			//$00$01$0D$00
	dc.b "copper o","n"+$80,"I",-1
	dc.w L_InCopOff,L_Nul			//$00$01$0E$00
	dc.b "copper of","f"+$80,"I",-1
	dc.w L_InCopSwap,L_Nul			//$00$01$0C$00
	dc.b "cop swa","p"+$80,"I",-1
	dc.w L_InCopReset,L_Nul			//$00$01$0D$00
	dc.b "cop rese","t"+$80,"I",-1
	dc.w L_InCopWait2,L_Nul	 		//$00$01$0D$00
	dc.b "!cop wai","t"+$80,"I0,0",-2
	dc.w L_InCopWait4,L_Nul			//$2D$01$05$00
	dc.b $80,"I0,0,0,0",-1
	dc.w L_InCopMoveL,L_Nul			//$00$01$0D$00
	dc.b "cop move","l"+$80,"I0,0",-1
	dc.w L_InCopMove,L_Nul			//$00$01$0C$00
	dc.b "cop mov","e"+$80,"I0,0",-1
	dc.w L_Nul,L_FnCopLogic			//$01$00$0D$00
	dc.b "cop logi","c"+$80,"0",-1

* Instructions programmes/accessoires
	dc.w L_Nul,L_FnPrgFirst			//$01$00$0E$00
	dc.b "prg first","$"+$80,"22",-1
	dc.w L_Nul,L_FnFillNext			//$01$00$0D$00
	dc.b "prg next","$"+$80,"2",-1
	dc.w L_Nul,L_FnPSel			//$01$00$0A$00
	dc.b "!psel","$"+$80,"22",-2
	dc.w L_Nul,L_FnPSel			//$01$19$05$00
	dc.b $80,"22,2",-2
	dc.w L_Nul,L_FnPSel			//$01$19$05$00
	dc.b $80,"22,2,2",-2
	dc.w L_Nul,L_FnPSel			//$01$19$05$00
	dc.b $80,"22,2,2,2",-1
	dc.w L_InPRun,L_Nul			//$00$01$08$00
	dc.b "pru","n"+$80,"I2",-1
	dc.w L_InBGrab,L_Nul			//$00$01$09$00
	dc.b "bgra","b"+$80,"I0",-1

* En dernier!
	dc.w L_InPut,L_Nul			//$00$01$07$00
	dc.b "pu","t"+$80,"I0,0",-1
	dc.w L_InGet,L_Nul			//$00$01$07$00
	dc.b "ge","t"+$80,"I0,0",-1
	dc.w L_InSystem,L_Nul			//$00$01$0A$00
	dc.b "syste","m"+$80,"I",-1
	dc.w L_InMWait,L_Nul			//$00$01$0E$00
	dc.b "multi wai","t"+$80,"I",-1
	dc.w L_Nul,L_FnIBob			//$01$00$09$00
	dc.b "i bo","b"+$80,"00",-1
	dc.w L_Nul,L_FnISprite			//$01$00$0C$00
	dc.b "i sprit","e"+$80,"00",-1
	dc.w L_InPriorityReverseOn,L_Nul	//$00$01$17$00
	dc.b "priority reverse o","n"+$80,"I",-1
	dc.w L_InPriorityReverseOff,L_Nul	//$00$01$18$00
	dc.b "priority reverse of","f"+$80,"I",-1
	dc.w L_Nul,L_FnDevFirst			//$01$00$0E$00
	dc.b "dev first","$"+$80,"22",-1
	dc.w L_Nul,L_FnFillNext			//$01$00$0D$00
	dc.b "dev next","$"+$80,"2",-1
	dc.w L_InHRevBlock,L_Nul		//$00$01$0E$00
	dc.b "hrev bloc","k"+$80,"I0",-1
	dc.w L_InVRevBlock,L_Nul		//$00$01$0E$00
	dc.b "vrev bloc","k"+$80,"I0",-1
	dc.w L_Nul,L_FnHRev			//$01$00$08$00
	dc.b "hre","v"+$80,"00",-1
	dc.w L_Nul,L_FnVRev			//$01$00$08$00
	dc.b "vre","v"+$80,"00",-1
	dc.w L_Nul,L_FnRev			//$01$00$07$00
	dc.b "re","v"+$80,"00",-1
	dc.w L_InBankSwap,L_Nul			//$00$01$0D$00
	dc.b "bank swa","p"+$80,"I0,0",-1
	dc.w L_InAmosToFront,L_Nul		//$00$01$11$00
	dc.b "amos to fron","t"+$80,"I",-1
	dc.w L_InAmosToBack,L_Nul		//$00$01$10$00
	dc.b "amos to bac","k"+$80,"I",-1
	dc.w L_Nul,L_FnAmosHere			//$01$00$0D$00
	dc.b "amos her","e"+$80,"0",-1
	dc.w L_InAmosLock,L_Nul			//$10$01$0D$00
	dc.b "amos loc","k"+$80,"I",-1
	dc.w L_InAmosUnlock,L_Nul		//$00$01$0F$00
	dc.b "amos unloc","k"+$80,"I",-1
	dc.w L_Nul,L_FnDisplayHeight		//$01$00$12$00
	dc.b "display heigh","t"+$80,"0",-1
	dc.w L_Nul,L_FnNTSC			//$01$00$08$00
	dc.b "nts","c"+$80,"0",-1
	dc.w L_Nul,L_FnLaced			//$01$00$09$00
	dc.b "lace","d"+$80,"0",-1
	dc.w L_Nul,L_FnPrgState			//$01$00$0D$00
	dc.b "prg stat","e"+$80,"0",-1
	dc.w L_InCommandLine,L_FnCommandLine	//$2B$1A$11$00
	dc.b "command line","$"+$80,"V2",-1
	dc.w L_Nul,L_FnDiscInfo			//$01$00$0E$00
	dc.b "disc info","$"+$80,"22",-1
; Nouvelles fonctions AMOSPro
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	dc.w L_Syntax,L_Syntax			//$01$01$14$00
	dc.b "\\\\\\\\\\\\\\\","/"+$80,"I",-1
	dc.w L_InNull,L_Nul			//$24$01$11$00
	dc.b "set accessor","y"+$80,"I",-1
	dc.w L_In_apml_,L_Nul			//$55$01$0C$00
	dc.b "@_apml_","@"+$80,"I",-1
	dc.w L_InTrap,L_Nul			//$25$01$08$00
	dc.b "tra","p"+$80,"I",-1
	dc.w L_InElse,L_Nul			//$3C$02$0C$12
	dc.b "else if"," "+$80,-1
	dc.w L_Nul,L_Nul			//$29$01$0B$00
	dc.b "includ","e"+$80,"I2",-1
	dc.w L_NoFlag,L_FnArray			//$01$0E$09$00
	dc.b "arra","y"+$80,"03",-1

	dc.w L_Nul,L_FnFormLoad2		//$01$00$0F$00
	dc.b "!frame loa","d"+$80,"00t0",-2
	dc.w L_Nul,L_FnFormLoad3		//$01$19$05$00
	dc.b $80,"00t0,0",-1
	dc.w L_Nul,L_FnFormPlay2		//$01$00$0F$00
	dc.b "!frame pla","y"+$80,"00,0",-2
	dc.w L_Nul,L_FnFormPlay3		//$01$19$05$00
	dc.b $80,"00,0,0",-1
	dc.w L_InIffAnim2,L_Nul			//$29$01$0D$00
	dc.b "!iff ani","m"+$80,"I2t0",-2
	dc.w L_InIffAnim3,L_Nul			//$29$01$05$00
	dc.b $80,"I2t0,0",-1
	dc.w L_Nul,L_FnFormLength1		//$01$00$11$00
	dc.b "!frame lengt","h"+$80,"00",-2
	dc.w L_Nul,L_FnFormLength2		//$01$19$05$00
	dc.b $80,"00,0",-1
	dc.w L_Nul,L_FnFormSkip1		//$01$00$0F$00
	dc.b "!frame ski","p"+$80,"00",-2
	dc.w L_Nul,L_FnFormSkip2		//$01$19$05$00
	dc.b $80,"00,0",-1
	dc.w L_Nul,L_FnFormParam		//$01$00$10$00
	dc.b "!frame para","m"+$80,"0",-1
	dc.w L_InCallEditor1,L_Nul		//$29$01$10$00
	dc.b "!call edito","r"+$80,"I0",-2
	dc.w L_InCallEditor2,L_Nul		//$2A$01$05$00
	dc.b $80,"I0,0",-2
	dc.w L_InCallEditor3,L_Nul		//$2A$01$05$00
	dc.b $80,"I0,0,2",-1
	dc.w L_InAskEditor1,L_Nul		//$29$01$0F$00
	dc.b "!ask edito","r"+$80,"I0",-2
	dc.w L_InAskEditor2,L_Nul		//$2A$01$05$00
	dc.b $80,"I0,0",-2
	dc.w L_InAskEditor3,L_Nul		//$2A$01$05$00
	dc.b $80,"I0,0,2",-1
	dc.w L_InEraseTemp,L_Nul		//$29$01$0E$00
	dc.b "erase tem","p"+$80,"I",-1
	dc.w L_InEraseAll,L_Nul			//$29$01$0D$00
	dc.b "erase al","l"+$80,"I",-1
; Dialogues
	dc.w L_Nul,L_FnDialogBox1		//$01$25$0F$00
	dc.b "!dialog bo","x"+$80,"03",-2
	dc.w L_Nul,L_FnDialogBox2		//$01$25$05$00
	dc.b $80,"03,0",-2
	dc.w L_Nul,L_FnDialogBox3		//$01$25$05$00
	dc.b $80,"03,0,2",-2
	dc.w L_Nul,L_FnDialogBox5		//$01$25$05$00
	dc.b $80,"03,0,2,0,0",-1
	dc.w L_InDialogOpen2,L_Nul		//$50$01$10$00
	dc.b "!dialog ope","n"+$80,"I0,3",-2
	dc.w L_InDialogOpen3,L_Nul		//$50$01$05$00
	dc.b $80,"I0,3,0",-2
	dc.w L_InDialogOpen4,L_Nul		//$50$01$05$00
	dc.b $80,"I0,3,0,0",-1
	dc.w L_InDialogClose0,L_Nul		//$50$01$11$00
	dc.b "!dialog clos","e"+$80,"I",-2
	dc.w L_InDialogClose1,L_Nul		//$50$01$05$00
	dc.b $80,"I0",-1
	dc.w L_Nul,L_FnDialogRun1		//$01$25$0F$00
	dc.b "!dialog ru","n"+$80,"00",-2
	dc.w L_Nul,L_FnDialogRun2		//$01$25$05$00
	dc.b $80,"00,0",-2
	dc.w L_Nul,L_FnDialogRun4		//$01$25$05$00
	dc.b $80,"00,0,0,0",-1
	dc.w L_Nul,L_FnDialog			//$01$25$0A$00
	dc.b "dialo","g"+$80,"00",-1
	dc.w L_InVDialog,L_FnVDialog		//$2C$1A$0B$00
	dc.b "vdialo","g"+$80,"V00,0",-1
	dc.w L_InVDialogD,L_FnVDialogD		//$2C$1A$0C$00
	dc.b "vdialog","$"+$80,"V20,0",-1
	dc.w L_Nul,L_FnRDialog2			//$01$25$0C$00
	dc.b "!rdialo","g"+$80,"00,0",-2
	dc.w L_Nul,L_FnRDialog3			//$01$25$05$00
	dc.b $80,"00,0,0",-1
	dc.w L_Nul,L_FnRDialogD2		//$01$25$0D$00
	dc.b "!rdialog","$"+$80,"20,0",-2
	dc.w L_Nul,L_FnRDialogD3		//$01$25$05$00
	dc.b $80,"20,0,0",-1
	dc.w L_Nul,L_FnEDialog			//$01$25$0B$00
	dc.b "edialo","g"+$80,"0",-1
	dc.w L_InDialogClr,L_Nul		//$50$01$0E$00
	dc.b "dialog cl","r"+$80,"I0",-1
	dc.w L_InDialogUpdate2,L_Nul		//$50$01$12$00
	dc.b "!dialog updat","e"+$80,"I0,0",-2
	dc.w L_InDialogUpdate3,L_Nul		//$50$01$05$00
	dc.b $80,"I0,0,3",-2
	dc.w L_InDialogUpdate4,L_Nul		//$50$01$05$00
	dc.b $80,"I0,0,3,0",-2
	dc.w L_InDialogUpdate5,L_Nul		//$50$01$05$00
	dc.b $80,"I0,0,3,0,0",-1
	dc.w L_InDialogFreeze0,L_Nul		//$50$01$12$00
	dc.b "!dialog freez","e"+$80,"I",-2
	dc.w L_InDialogFreeze1,L_Nul		//$50$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InDialogUnFreeze0,L_Nul		//$50$01$14$00
	dc.b "!dialog unfreez","e"+$80,"I",-2
	dc.w L_InDialogUnFreeze1,L_Nul		//$50$01$05$00
	dc.b $80,"I0",-1

	dc.w L_InPokeD,L_Nul			//$29$01$09$00
	dc.b "poke","$"+$80,"I0,2",-1
	dc.w L_Nul,L_FnPeekD2			//$01$00$0A$00
	dc.b "!peek","$"+$80,"20,0",-2
	dc.w L_Nul,L_FnPeekD3			//$01$19$05$00
	dc.b $80,"20,0,2",-1
	dc.w L_InResourceBank,L_Nul		//$50$01$11$00
	dc.b "resource ban","k"+$80,"I0",-1
	dc.w L_Nul,L_FnResource			//$01$25$0D$00
	dc.b "resource","$"+$80,"20",-1
	dc.w L_InResourceScreenOpen,L_Nul	//$50$01$18$00
	dc.b "resource screen ope","n"+$80,"I0,0,0,0",-1
	dc.w L_InResourceUnpack,L_Nul		//$50$01$13$00
	dc.b "resource unpac","k"+$80,"I0,0,0",-1
	dc.w L_InReadText1,L_Nul		//$50$01$0D$00
	dc.b "read tex","t"+$80,"I2",-2
	dc.w L_InReadText3,L_Nul		//$50$01$05$00
	dc.b $8C,"I2,0,0",-1
	dc.w L_Nul,L_FnErrD			//$01$00$08$00
	dc.b "err","$"+$80,"20",-1
	dc.w L_InAssign,L_Nul			//$29$01$0A$00
	dc.b "assig","n"+$80,"I2t2",-1
	dc.w L_Nul,L_FnErrTrap			//$01$00$0B$00
	dc.b "errtra","p"+$80,"0",-1
	dc.w L_InDevOpen,L_Nul			//$29$01$0C$00
	dc.b "dev ope","n"+$80,"I0,2,0,0,0",-1
	dc.w L_InDevClose0,L_Nul		//$29$01$0E$00
	dc.b "!dev clos","e"+$80,"I",-2
	dc.w L_InDevClose1,L_Nul		//$2A$01$05$00
	dc.b $80,"I0",-1
	dc.w L_Nul,L_FnDevBase			//$01$00$0C$00
	dc.b "dev bas","e"+$80,"00",-1
	dc.w L_InDevDo,L_Nul			//$29$01$0A$00
	dc.b "dev d","o"+$80,"I0,0",-1
	dc.w L_InDevSend,L_Nul			//$29$01$0C$00
	dc.b "dev sen","d"+$80,"I0,0",-1
	dc.w L_InDevAbort,L_Nul			//$29$01$0D$00
	dc.b "dev abor","t"+$80,"I0",-1
        dc.w L_Nul,L_FnDevCheck			//$29$00$0D$00
        dc.b "dev chec","k"+$80,"00",-1
	dc.w L_InLibOpen,L_Nul			//$29$01$0C$00
	dc.b "lib ope","n"+$80,"I0,2,0",-1
	dc.w L_InLibClose0,L_Nul		//$29$01$0E$00
	dc.b "!lib clos","e"+$80,"I",-2
	dc.w L_InLibClose1,L_Nul		//$2A$01$05$00
	dc.b $80,"I0",-1
	dc.w L_Nul,L_FnLibCall			//$01$00$0C$00
	dc.b "lib cal","l"+$80,"00,0",-1
	dc.w L_Nul,L_FnLibBase			//$01$00$0C$00
	dc.b "lib bas","e"+$80,"00",-1
	dc.w L_NoFlag,L_FnEqu			//$01$0C$07$10
	dc.b "eq","u"+$80,"02",-1
	dc.w L_NoFlag,L_FnEqu			//$01$10$07$10
        dc.b "lv","o"+$80,"02",-1
	dc.w L_InStruc,L_NoFlag			//$26$11$09$10
	dc.b "stru","c"+$80,"V00,2",-1
	dc.w L_InStrucD,L_NoFlag		//$27$12$0A$10
	dc.b "struc","$"+$80,"V20,2",-1
	dc.w L_Nul,L_FnBStart			//$01$00$0A$00
	dc.b "bstar","t"+$80,"00",-1
	dc.w L_Nul,L_FnBLength			//$01$00$0B$00
	dc.b "blengt","h"+$80,"00",-1
	dc.w L_InBSend,L_Nul			//$29$01$09$00
	dc.b "bsen","d"+$80,"I0",-1
	dc.w L_InBankSchrink,L_Nul		//$29$01$0F$00
	dc.b "bank shrin","k"+$80,"I0t0",-1
	dc.w L_Nul,L_FnPrgUnder			//$01$00$0D$00
	dc.b "prg unde","r"+$80,"0",-1
	dc.w L_InArexxOpen,L_Nul		//$29$01$0E$00
	dc.b "arexx ope","n"+$80,"I2",-1
	dc.w L_InArexxClose,L_Nul		//$29$01$0F$00
	dc.b "arexx clos","e"+$80,"I",-1
	dc.w L_Nul,L_FnArexxExist		//$01$00$0F$00
	dc.b "arexx exis","t"+$80,"02",-1
	dc.w L_Nul,L_FnArexx			//$01$00$09$00
	dc.b "arex","x"+$80,"0",-1
	dc.w L_Nul,L_FnArexxD			//$01$00$0A$00
	dc.b "arexx","$"+$80,"20",-1
	dc.w L_InArexxWait,L_Nul		//$29$01$0E$00
	dc.b "arexx wai","t"+$80,"I",-1
	dc.w L_InArexxAnswer1,L_Nul		//$29$01$11$00
	dc.b "!arexx answe","r"+$80,"I0",-2
	dc.w L_InArexxAnswer2,L_Nul		//$2A$01$05$00
	dc.b $80,"I0,2",-1
	dc.w L_InExec,L_Nul			//$29$01$08$00
	dc.b "exe","c"+$80,"I2",-1
	dc.w L_InMonitor,L_Nul			//$29$01$0B$00
	dc.b "monito","r"+$80,"I",-1
	dc.w L_Nul,L_FnScreenMode		//$01$00$0F$00
	dc.b "screen mod","e"+$80,"0",-1
* Constante double precision!
	dc.w L_Nul,L_FnCstDFl			//$01$16$05$11
	dc.b $80,"C3",-1
	dc.w L_InKillEditor,L_Nul		//$29$01$0F$00
	dc.b "kill edito","r"+$80,"I",-1
	dc.w L_InNull,L_Nul     		//$04$01$18$00
	dc.b "set double precisio","n"+$80,"I",-1
	dc.w L_InSetBuffer,L_Nul		//$05$01$0D$00
	dc.b "set stac","k"+$80,"I",-1
	dc.w L_InGetSpritePalette0,L_Nul	//$29$01$14$00
	dc.b "!get bob palett","e"+$80,"I",-2
	dc.w L_InGetSpritePalette1,L_Nul	//$2A$01$05$00
	dc.b $80,"I0",-1
	dc.w L_InSetBuffer,L_Nul		//$29$01$13$00
	dc.b "set equate ban","k"+$80,"I0",-1
	dc.w L_Nul,L_FnZDialog			//$01$00$0B$00
	dc.b "zdialo","g"+$80,"00,0,0",-1
	dc.w L_InAPCmp,L_Nul			//$57$01$0D$00
	dc.b "||apcmp|","|"+$80,"I",-1
	dc.w 0

;	TOKEN_END
.TokenEnd
; 	Indication des swap floats
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	dc.b	"FSwp"
	dc.w	L_Start_FloatSwap,L_End_FloatSwap

;	Prive pour le compilateur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	dc.b	"ComP"
	dc.w	3*4+2
	dc.w	L_MainLibrary
	dc.w	L_Start_Externes,L_End_Externes
	dc.w	L_Start_FloatSwap,L_End_FloatSwap
	dc.w	L_Start_Type,L_End_Type

; 	Inclusion de la table de test rapide...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	dc.b 	"KwiK"
	dc.w	.Fin-.Debut
.Debut	IncBin	"+Toktab_Verif.Bin"
.Fin

;	Decalage au debut de la librairie
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Here	dc.l	.TokenEnd-.Here

;---------------------------------------------------------------------
;   +++  +++  ++   ++ ++++  ++++ ++    ++    ++   ++   ++  +++   +++
;  ++   ++ ++ +++ +++ ++ ++  ++  ++    ++   ++++  +++ +++ ++ ++ ++
;  ++   ++ ++ ++ + ++ ++ ++  ++  ++   ++   ++  ++ ++ + ++ ++ ++  +++
;  ++   ++ ++ ++   ++ ++++   ++  ++        ++++++ ++   ++ ++ ++    ++
;  ++   ++ ++ ++   ++ ++     ++  ++        ++  ++ ++   ++ ++ ++ +  ++
;   +++  +++  ++   ++ ++    ++++ ++++      ++  ++ ++   ++  +++   +++
;---------------------------------------------------------------------
; ROUTINES
;---------------------------------------------------------------------
C_Lib
;---------------------------------------------------------------------

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	INCLUSION DE LA LIBRARIE INTERNE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Include	"+ILib.s"
; - - - - - - - - - - - - -
	Lib_Def	MainLibrary
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERRN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnErrn		src/lib/system.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=ERR$(b)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnErrD		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SGN (fmath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnSgn			Integer: src/lib/maths.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnSgnF		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=STR$ (fmath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnStrE		src/lib/system.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnStrF		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ABS (FMath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnAbs			Integer: src/lib/maths.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnAbsF		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INT (FMath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnInt			Integer
; - - - - - - - - - - - - -
	rts
; - - - - - - - - - - - - -
	Lib_Int	FnIntF		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TAN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnTan		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SQR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnSqr		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Log10
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLog		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Ln
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLn		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnExp		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnSin		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCos		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ASIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnASin		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ACOS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnACos		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ATAN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnATan		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HSIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHSin		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HCOS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHCos		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HTAN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHTan		src/lib/maths.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RADIUS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRadian		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEGREE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDegree		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FIX
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InFix		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RND
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnRnd		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MULTIPLICATION 32 BITS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Mulu32		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RANDOMIZE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRandom		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TIMER=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InTimer		src/lib/system.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=TIMER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnTimer		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=PI#
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPi		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WAIT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWait		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Wait avec branchement
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Wait_Normal, now in src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Wait sans branchement (extensions)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	WaitRout		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Wait Event
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Wait_Event, now in src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Multi Wait
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMWait		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Wait VBL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWtVbl		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Wait Key
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWtKy		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					KEY SPEED delai,vitesse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InKeySpeed		src/lib/system.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PARAM string
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnParamS		src/lib/system.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PARAM entier
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnParamE		src/lib/system.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LISTBANK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InListBank		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERASE n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InErase		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERASE TEMP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEraseTemp		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERASE ALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEraseAll		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BANK SWAP a,b
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBankSwap		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BSTART()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnBStart		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BLENGTH()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnBLength		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BGRAB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBGrab		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BSEND bank
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBSend		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INS BOB / INS SPRITE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInsSprite		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INS ICON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInsIcon		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEL ICON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDelIcon1		src/lib/banks.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDelIcon2		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEL SPRITE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDelSprite1		src/lib/banks.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDelSprite2		src/lib/banks.cpp
; - - - - - - - - - - - - -
	Lib_Empty			IDIc3, now in src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESERVE AS WORK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResWork		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESERVE AS CHIP WORK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResChipWork		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESERVE AS DATA
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResData		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESERVE AS CHIPDATA
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResChipData		src/lib/banks.cpp
; - - - - - - - - - - - - -
	Lib_Empty			RsBqX, now in src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BANK SCHRINK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBankSchrink		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=START()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnStart		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LENGTH()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLength		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHIPFREE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnChipFree		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=FASTFREE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFastFree		src/lib/banks.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FFree, now in src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TRANSFERT DE MEMOIRE
;	A0>A1, D0 octets
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	TransMem		src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COPY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopy		src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FILL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InFill		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FillBis, now in src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HUNT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHunt		src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=AREG=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAReg		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnAReg		src/lib/memory.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DREG=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDReg		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDReg		src/lib/memory.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routines AREG / DREG
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IReg, now in src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FReg, now in src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Empty			RReg, now in src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					POKEDOKELOKE ameliores
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPoke		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDoke		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	InLoke		src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PEEKDEEKLEEK ameliores
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPeek		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDeek		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnLeek		src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					POKE$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPokeD		src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=PEEK$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPeekD2		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnPeekD3		src/lib/memory.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FPeekD, now in src/lib/memory.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine LIB CLOSE ALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Lib.Close		src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine Lib Close D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Lib.CloseD0		src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Retourne l'adresse de la librarie
;	D0=	Numero canal
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Lib.GetA2		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Appel librarie A0 / D3=fonction
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Lib.Call		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXEDOSINTGFXCALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnExeCall		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnGfxCall		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDosCall		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnIntCall		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LIB OPEN canal,"lib",flags
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLibOpen		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LIB CLOSE [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLibClose0		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	InLibClose1		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LIB CALL lib,function
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLibCall		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LIB BASE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLibBase		src/lib/devices.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine dev: pointe le dev en A2
;	D0= numero canal
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.GetA2		src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: Ouvre un device
;	A0=	nom
;	A2=	Pointeur structure
;	D0=	Longueur structure
;	D1=	Numero
;	D2=	Flags
;	D3= 	First error message
;	D4=	Number of errors
;	D5=	Flags dans structure IO
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.Open		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine Device Close All
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.Close		src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: ferme un device
;	A2=	Pointeur structure
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.CloseA2		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: retourne l'adresse IO
;	A2= pointeur device
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.GetIO		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: SEND
;	A2= 	Base device
;	D0=	Numero fonction
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.SendIO		src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine DO
;	A2= 	Base device
;	D0=	Numero fonction
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.DoIO		src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine Check io
;	A2= 	Base device
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.CheckIO		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Abort IO
;	A2=	Base device
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.AbortIO		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Appel des erreurs device
;	D0=	Erreur Device
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dev.Error		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEVICE OPEN c,n$,l,unit,flags
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDevOpen		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEVICE CLOSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDevClose0		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEVICE CLOSE D
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDevClose1		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DEV BASE(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDevBase		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEV DO dev,command
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDevDo		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEV SEND dev,command
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDevSend		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEV ABORT dev
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDevAbort		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DEV CHECK(dev)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDevCheck		src/lib/devices.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXEC "Program Name"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InExec		src/lib/devices.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CODE AMIGA.LIB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	CreatePort		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	DeletePort		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	CreateExtIO		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Int	DeleteExtIO		src/lib/devices.cpp
; - - - - - - - - - - - - -
	Lib_Empty			AmigaLib, now in src/lib/devices.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Entete des banques
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	BkSpr
; - - - - - - - - - - - - -
	dc.b "Sprites "
; - - - - - - - - - - - - -
	Lib_Def	BkIco
; - - - - - - - - - - - - -
	dc.b "Icons   "
; - - - - - - - - - - - - -
	Lib_Def	BkMus
; - - - - - - - - - - - - -
	dc.b "Music   "
; - - - - - - - - - - - - -
	Lib_Def	BkAmal
; - - - - - - - - - - - - -
	dc.b "Amal    "
; - - - - - - - - - - - - -
	Lib_Def	BkMenu
; - - - - - - - - - - - - -
	dc.b "Menu    "
; - - - - - - - - - - - - -
	Lib_Def	BkDat
; - - - - - - - - - - - - -
	dc.b "Data    "
; - - - - - - - - - - - - -
	Lib_Def	BkWrk
; - - - - - - - - - - - - -
	dc.b "Work    "
; - - - - - - - - - - - - -
	Lib_Def	BkAsm
; - - - - - - - - - - - - -
	dc.b "Asm     "
; - - - - - - - - - - - - -
	Lib_Def	BkIff
; - - - - - - - - - - - - -
	dc.b "Iff     "
; - - - - - - - - - - - - -
	Lib_Def	Bnk_NoLoad
; - - - - - - - - - - - - -
	dc.b "Loading!"

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHVERBUF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	ChVerBuf		src/lib/textout.cpp
; - - - - - - - - - - - - -
	Lib_Int	ChVerBuf2		src/lib/textout.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Str2Chaine
; Recopie un message systeme (a0) fini par zero dans le buffer chaines
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Str2Chaine		src/lib/textout.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Chaine (a2) >>> buffer chaines
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Mes2Chaine		src/lib/textout.cpp
; - - - - - - - - - - - - -
	Lib_Int	MCSuit		src/lib/textout.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Retourne une chaine > hichaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Set_HiChaine		src/lib/textout.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Retourne une chaine vide
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Ret_ChVide		src/lib/textout.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Copie A0 en chaine AMOS + RTS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	A0ToChaine		src/lib/textout.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Fin de BIN, HEX et autres
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FinBin		src/lib/textout.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;			Demande l'espace chaine, positionne Hichaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			AskD3, now in src/lib/files.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SAUVE HUNK BANQUE D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SHunk, now in src/lib/bankfiles.cpp
; - - - - - - - - - - - - -
	Lib_Def	NHunk
; - - - - - - - - - - - - -
	dc.b 	"AmBk"
	dc.b 	"AmSp"
	dc.b 	"AmBs"
	dc.b 	"AmIc"
	dc.l 	0

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SAVE "Banque", n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSave2		src/lib/bankfiles.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SAVE "banques"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSave1		src/lib/bankfiles.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				Routine: Sauve toutes les banques
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.SaveAll		src/lib/bankfiles.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				Routine: sauve les banques VIDES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.SaveVide		src/lib/bankfiles.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve la banque A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.SaveA0		src/lib/bankfiles.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Load "Banque",n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLoad1		src/lib/bankfiles.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Load "Banque"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLoad2		src/lib/bankfiles.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Load0, now in src/lib/bankfiles.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine chargement de banque
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Load		src/lib/bankfiles.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PLOAD "prog",banque
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPLoad		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BLOAD "fichier",adresse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBload		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BSAVE  "fichier",ad to fin
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBSave		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					IFF MASK a%
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMaskIff		src/lib/iff.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=PICTURE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPicture		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LOAD IFF "fichier"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLoadIff1		src/lib/iff.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LOAD IFF "nom",param
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLoadIff2		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;				=FORM LOAD(N To Ad[banque][,NForms])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFormLoad2		src/lib/iff.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFormLoad3		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=FORM LENGTH(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFormLength1		src/lib/iff.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFormLength2		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;					=Form Play(Ad,Number[,param])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFormPlay2		src/lib/iff.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFormPlay3		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=FORM SKIP(ad,[number])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFormSkip1		src/lib/iff.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFormSkip2		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					IFF ANIM "name",screen[,ntimes]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InIffAnim2		src/lib/iff.cpp
; - - - - - - - - - - - - -
	Lib_Int	InIffAnim3		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=FRAME PARAM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFormParam		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SAVE IFF a$,comp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSaveIff1		src/lib/iff.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSaveIff2		src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TROUVE LE DESCRIPTEUR FICHIER
;					D0= numero
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	GetFile		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					NETTOIE LE DESCRIPTEUR FICHIER
;					A2 descripteur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			FiClean, now in src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET INPUT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetInput		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=INPUT$(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnInputD1		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=INPUT$(#1,2)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnInputD2		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ENTREE UN BYTE DANS UN FICHIER
;					A2= Descripteur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	GetByte		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FIN DE GETBYTE
;					A2= Descripteur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	EndByte		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DIR$=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDirD		src/lib/files.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDirD		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PARENT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InParent		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					KILL a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InKill		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RENAME a$ TO b$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRename		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MAKEDIR a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMkDir		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DRIVE(a$)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDrive		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DFREE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDFree		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DISC INFO$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDiscInfo		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PORT(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPort		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OPEN PORT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOpenPort		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OPEN OUT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOpenOut		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OPEN IN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOpenIn		src/lib/files.cpp
; - - - - - - - - - - - - -
	Lib_Empty			OpIn, now in src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Branche la structure CLEARVAR pour le disque
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			DiskClear, now in src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LOF(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLof		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=POF(n)=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPof		src/lib/files.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnPof		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EOF(n) illegal a refaire
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnEof		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine POFLOFEOF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			RLof, now in src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLOSE [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InClose1		src/lib/files.cpp
; - - - - - - - - - - - - -
	Lib_Int	InClose0		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine CLOSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	CloAll		src/lib/files.cpp
; - - - - - - - - - - - - -
	Lib_Int	Cloa1		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OPEN RANDOM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOpenRandom		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					APPEND canal,nom
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAppend		src/lib/files.cpp
; - - - - - - - - - - - - -
	Lib_Empty			RanApp, now in src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET #n,vv
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGet		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PUT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPut		src/lib/files.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINE GET ET PUT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			GetPut, now in src/lib/files.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					IMPRESSION CHAINE A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	ImpChaine		src/lib/textout.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OUVERTURE DE L'IMPRIMANTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	PRT_Open		src/lib/textout.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FERMETURE IMPRIMANTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	PRT_Close		src/lib/textout.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Imprime la chaine A0 > imprimante
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	PRT_Print		src/lib/textout.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET DIR A,[a$]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetDir2		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSetDir1		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DEV FIRST$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPrgFirst		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDevFirst		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			DevAcc, now in src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DIR FIRST("filter")
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDirFirst		src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=DIR NEXT$
;					=ACC NEXT$
;					=DEV NEXT$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFillNext		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ASSIGN "name" TO "pathname"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAssign		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=EXIST("name")
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnExist		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINE EXIST
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	RExist		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PLUS DE REQUESTER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	NoReq		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REMET LE REQUESTER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	YesReq		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DIR/W LDIR/W
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLDirW0		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDirW0		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			DirW0a, now in src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DIR/w LDIR/W a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLDirW1		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDirW1		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			DirW1a, now in src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			DirW2, now in src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DIR / LDIR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLDir0		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDir0		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dir0a, now in src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DIR/LDIR a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLDir1		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDir1		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dir1a, now in src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dir2, now in src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dir3, now in src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Test touches dir
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			TTDir, now in src/lib/dirs.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Trouve le lock NAME1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	LockGet		src/lib/files.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ENLEVE LE LOCK COURANT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	LockFree		src/lib/files.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FILL FILL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillAll		src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Retourne le nom selectionne d0->A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillGet		src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: FILL FILE DEVICES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillDev		src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Cree le buffer fill files
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillFirst		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Boucle de recherche des fichiers
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillNxt		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Lebere les buffers fill file
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillFFree		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Trie le buffer fill file
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillSort		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Poke dans le buffer sans classement
;					D2= Premier caractere
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillFPoke		src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine, compare A0 a A1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FfComp		src/lib/dirs.cpp
; - - - - - - - - - - - - -
	Lib_Int	FfComp2		src/lib/dirs.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve la position de la chaine A0 dans la liste,
;	a partir de #D0. D1= mode de recherche
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FillFFind		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DISQUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				Routine: pointe le debut d'un nom de fichier
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dsk.DNom		src/lib/dos.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					NAME1+PATHACT >>> NAME1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dsk.PathIt		src/lib/dos.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COPIE LE NOM (a2) dans le buffer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	NomDisc		src/lib/dos.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					NOMDIR: (a2) >>> Name1 / Name2
;	Met la racine en NAME1
;	Met le filtre en NAME2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	NomDir		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FILTRE LES NOMS DISQUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Joker		src/lib/dirs.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DISQUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	D_Open		src/lib/dos.cpp
; - - - - - - - - - - - - -
	Lib_Int	D_OpenD1		src/lib/dos.cpp
; - - - - - - - - - - - - -
	Lib_Int	D_Close		src/lib/dos.cpp
; - - - - - - - - - - - - -
	Lib_Int	D_Read		src/lib/dos.cpp
; - - - - - - - - - - - - -
	Lib_Int	D_Write		src/lib/dos.cpp
; - - - - - - - - - - - - -
	Lib_Int	D_Seek		src/lib/dos.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PSEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnPSel
; - - - - - - - - - - - - -
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FSEL(a$,b$,c$,d$)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFileSelector1		src/lib/fsel.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFileSelector2		src/lib/fsel.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFileSelector3		src/lib/fsel.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFileSelector4		src/lib/fsel.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Apparition / disparition par le centre
;	A2= ecran
;	d7= vitesse
;	d6= WTy
;	d5= WY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	AppCentre		src/lib/screens.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FORM LOAD
;	Chargement de formes IFF en memoire
;	D7=	Nombre de FORM a voir
;	D6=	Adresse de chargement / 0 si Skip
;	D5=	Handle fichier
;	Sauver D5-D7 dans SaveRegs
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffFormLoad, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Ramene la taille des FORMS, sans changer la position...
;	D7=	Nombre de FORM a voir
;	D5=	Handle fichier
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffFormSize, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Lecture pour IFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffRead, now in src/lib/iff.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Seek pour IFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffSeek, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Initialisation des flags IFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffInit, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				Charge et joue les formes dans un buffer
;	D5=	Handle fichier
;	D6=
;	D7=	Nombre de formes
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffForm, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Interpretation des formes chargees
;	D7=	Nombre de formes a interpreter
;	Bit #30 >>> Sauter tout
;	D6= 	Adresse � voir
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffFormPlay, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauvegarde d'ecran IFF
;	D7	Compression
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			IffSaveScreen, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve le BMHD
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SaveBMHD, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve la CMAP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SaveCMAP, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve la CAMG
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SaveCAMG, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve le AMSC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SaveAMSC, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve le BODY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SaveBODY, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauve jusqu'a A1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			SaveA1, now in src/lib/iff.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Sauvegarde les registres D5-D7
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	SaveRegs
; - - - - - - - - - - - - -
	movem.l	d6-d7,ErrorSave(a5)
	move.b	#1,ErrorRegs(a5)
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Recupere les registres D5-D7
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	LoadRegs
; - - - - - - - - - - - - -
	movem.l	ErrorSave(a5),d6-d7
	clr.b	ErrorRegs(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	COMMAND LINE$=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCommandLine		src/lib/iff.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=COMMAND LINE$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCommandLine		src/lib/iff.cpp



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TROUVE L'ADRESSE DE LA BANQUE D0
;	OUT	BEQ Pas trouve, BNE Trouve, D0=Flags / A1=Adresse
;	Ne pas changer sans voir GETBOB / GETICON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.GetAdr		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TROUVE ADRESSE BANQUE SPRITES
;	OUT	BNE trouve A0/A1=adresse (D0/D1 inchanges)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.GetBobs		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TROUVE L'ADRESSE BANQUE ICONES
;	OUT	D0/A0/A1=	adresse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.GetIcons		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EFFACEMENT BANQUE D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Eff		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EFFACEMENT BANQUE A0=Adresse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.EffA0		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EFFACEMENT BOBS/ICONS A0=Adresse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.EffBobA0		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EFFACEMENT TOUTES LES BANQUES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.EffAll		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EFFACEMENT TOUTES BANQUES TEMP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.EffTemp		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ADRESS OR BANK
;	D0>>>D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.OrAdr		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TROUVE L'ADRESSE BOB D0>A0
;	IN	D0	Numero
;	OUT	A0/D0	Adresse (BNE)
;		D1	Max de bobs
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.AdBob		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ADRESSE ICONE
;	IN	D0	Numero
;	OUT	A0/D0	Adresse (BNE)
;		D1	Max de bobs
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.AdIcon		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	RESERVATION DE LA BANQUE DE SPRITES ou ICONES
;	D0=	Effacer ou non 0 Efface, 1 Append, -1 pas recopie+garde
;	D1= 	Nombre de sprites/icons
;	Retour: A0= Debut banque A1= ancienne bank
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.ResIco		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOBS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.ResBob		src/lib/banks.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Entree 2
;	A1=	Banque origine (si definie)
;	D0=	Flag
;	D1=	Nombre
;	D2=	Flags
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Ric		src/lib/banks.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Bnk.Ric2, now in src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine Bank Schrink
;	D0=	Bank Number
;	D1=	New size
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Schrink		src/lib/banks.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INSERTION BOB / ICON
;	IN	D0=	Numero Bob/Icon
;		A0=	Adresse Banque Origine
;	OUT	BEQ	Ok
;		BNE	Out of mem!
;		A0	Adresse Nouvelle Structure
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.InsBob		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DELETION BOB / ICON
;	IN	D0=	Numero1 Bob/Icon
;		D1=	Numero2 Bob/Icon
;		A0=	Adresse Banque Origine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.DelBob		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REMET LES BOBS DROITS BANQUE A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.UnRev		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESERVE BANK
;	D0=	Numero
;	D1=	Flags
;	D2= 	Longueur
;	A0=	Nom de la banque
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Reserve		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CALCULE LA TAILLE DES BANQUES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.GetLength		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	CALCULE LA TAILLE D'UNE BANQUE
;	A0= banque
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Length		src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	CHANGEMENT DANS LES BANQUES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Bnk.Change		src/lib/banks.cpp



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine LISTBANK
;	Ecrit la definition dans un buffer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Bnk.List, now in src/lib/banks.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TRUE / FALSE / ONE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnTrue
; - - - - - - - - - - - - -
	moveq	#-1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnOne
; - - - - - - - - - - - - -
	moveq	#1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnFalse
; - - - - - - - - - - - - -
	moveq	#0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Ecran.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEFAULT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDefault		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCls0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InCls1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InCls5		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Cls5a, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN HEIGHT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScreenHeight0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnScreenHeight1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN WIDTH
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScreenWidth0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnScreenWidth1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN BASE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScreenBase		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN COLOUR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScreenColour		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN MODE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScreenMode		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			GetScreen0, now in src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DISPLAY INFOS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDisplayHeight		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnNTSC		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LOG BASE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLogBase		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=PHY BASE(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPhyBase		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DOUBLE BUFFER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDoubleBuffer		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN SWAP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenSwap0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenSwap1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DUAL PLAYFIELD n,m
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDualPlayfield		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DUAL PRIORITY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDualPriority		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN CLONE n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenClone		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN OPEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenOpen		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN CLOSE n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenClose		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN DISPLAY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenDisplay		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN OFFSET
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenOffset		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN SHOW [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenShow0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenShow1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN HIDE [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenHide0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ScShHi0, now in src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenHide1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ScShHi, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					AUTO VIEW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAutoViewOn		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InAutoViewOff		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					VIEW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InView		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN TO FRONT [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenToFront0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenToFront1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN TO BACK [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenToBack0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenToBack1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreen		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnScreen		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Fonctions resolutions
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLaced		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnHires		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnLowres		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Verification du parametre ecran D1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			CheckScreenNumber, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COLOUR n,xxx
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InColour		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnColour		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COLOUR BACK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InColourBack		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET ICON PALETTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetIconPalette0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InGetIconPalette1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET SPRITE PALETTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetSpritePalette0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InGetSpritePalette1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET PALETTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetPalette1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InGetPalette2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			GSPal, now in src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	PalRout		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FLASH OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InFlashOff		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FLASH n,xxx
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InFlash		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHIFT OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InShiftOff		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHIFT UP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InShiftUp		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InShiftDown		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ShD1, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET RAINBOW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetRainbow6		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSetRainbow7		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RAINBOW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRainbow		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RAINBOW DEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRainbowDel0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InRainbowDel1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=RAIN=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRain		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnRain		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COPPER ON/OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopOn		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InCopOff		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COPPER SWAP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopSwap		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COP RESET
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopReset		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COP WAIT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopWait2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InCopWait4		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COP MOVE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopMove		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COP MOVEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCopMoveL		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=COP LOGIC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCopLogic		src/lib/screens.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					AUTOBACK n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAutoback		src/lib/screens.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PLOT x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPlot2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPlot3		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=POINT(x,y)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPoint		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			RPoint, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DRAW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDrawTo		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDraw		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CIRCLE x,y,n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCircle		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ELIPSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InEllipse		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			EllCir, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				XCURS / YCURS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXGr		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYGr		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Gr Locate
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGrLocate		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOX x,y TO x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBox		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Fonctions CLIP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InClip0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InClip4		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET ROM FONTS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetRomFonts		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET DISC FONTS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetDiscFonts		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET FONTS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetFonts		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Igf, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=FONT$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFont		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET FONT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetFont		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TEXT x,y,a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InText		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=TEXT BASE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnTextBase		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=TEXT LENGTH
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnTextLength		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=TEXT STYLES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnTextStyle		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET TEXT n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetText		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET PATTERN n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetPattern		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET PAINT n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetPaint		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PAINT x,y[,mode]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPaint2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPaint3		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BAR x,y TO x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBar		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET TEMPRAS n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; - - - - - - - - - - - - -
	Lib_Int	InSetTempras1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSetTempras0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSetTempras2		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Reserve le tempras
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	GetRas		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Libere le tempras
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FreeRas		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INK a,b,c
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInk1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InInk2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InInk3		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GR WRITING n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGrWriting		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET LINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetLine		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SLIDER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InHSlider		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InVSlider		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			GetSli, now in src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET SLIDER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetSlider		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEF SCROLL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDefScroll		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCROLL n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScroll		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			DoScroll, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCREEN COPY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScreenCopy2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenCopy3		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenCopy8		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InScreenCopy9		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine SCREEN COPY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Sco0		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=PHYSIC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPhysic0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnPhysic1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LOGIC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLogic0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnLogic1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					APPEAR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAppear3		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InAppear4		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ZOOM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InZoom		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					X HARD / YHARD
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXHard1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnXHard2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			XHr1, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=YHARD
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnYHard1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYHard2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			YHr1, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=X SCREEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXScreen1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnXScreen2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			XSc1, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=YSCREEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnYScreen1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYScreen2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			YSc1, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=X TEXT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXText		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=YTEXT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnYText		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					XYGRAPHIC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXGraphic		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYGraphic		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESERVE ZONES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InReserveZone0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InReserveZone1		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					RESET ZONE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResetZone0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InResetZone1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET ZONE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetZone		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=ZONE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnZone2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnZone3		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FZo, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=HZONE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHZone2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnHZone3		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FHZo, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SCIn
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScIn2		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnScIn3		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ScIn, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=MOUSE SCREEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnMouseScreen		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=MOUSEZONE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnMouseZone		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET CBLOCK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetCBlock		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PUT CBLOCK n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPutCBlock1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPutCBlock3		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEL CBLOCK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDelCBlock0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDelCBlock1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET BLOCK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetBlock5		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InGetBlock6		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PUT BLOCK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPutBlock1		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPutBlock3		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPutBlock4		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPutBlock5		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					DEL BLOC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDelBlock0		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDelBlock1		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Bloc Reverse
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InHRevBlock		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InVRevBlock		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Rev, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: set XY curseur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			GrXY, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routines d'appel GFX avec AUTOBACK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Empty			GfxPnt, now in src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	GfxFunc		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Empty			GfxF0, now in src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: #ecran D1 >>> adresse D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	GetEc		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLOSE WORKBENCH
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCloseWorkbench		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					AMOS TO FRONT / BACK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InAmosToFront		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InAmosToBack		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnAmosHere		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InAmosLock		src/lib/screens.cpp
; - - - - - - - - - - - - -
	Lib_Int	InAmosUnlock		src/lib/screens.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine: ferme le WB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	WB_Close		src/lib/screens.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Reouvre le WB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	WB_Open		src/lib/screens.cpp



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: b.s, ERREURS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERROR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InError
; - - - - - - - - - - - - -
	cmp.l	#256,d3
	bcs.s	.skip
	move.l	#255,d3
.skip	move.l	d3,d0
	Rbra	L_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					D0>>>> majuscule
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MajD0
; - - - - - - - - - - - - -
	cmp.b	#"a",d0
	bcs.s	.Ski
	cmp.b	#"z",d0
	bhi.s	.Ski
	sub.b	#$20,d0
.Ski	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: SPRITE.S
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Update et assimiles
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InUpdateOff		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InUpdateOn		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InUpdate		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InBobUpdateOff		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InBobUpdateOn		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InBobUpdate		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSpriteUpdateOff		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSpriteUpdateOn		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSpriteUpdate		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UPDATE EVERY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InUpdateEvery		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOB CLEAR / DRAW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBobClear		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InBobDraw		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LIMIT BOB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLimitBob0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InLimitBob4		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InLimitBob5		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PRIORITY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPriorityOn		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPriorityOff		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPriorityReverseOn		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPriorityReverseOff		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Prooo, now in src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=AMAL ERR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnAmalerr
; - - - - - - - - - - - - -
	moveq	#0,d3
	move.w	PAmalE(a5),d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FREEZE / UNFREEZE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InFreeze
; - - - - - - - - - - - - -
	SyCall	AMALFrz
	rts
; - - - - - - - - - - - - -
	Lib_Par InUnFreeze
; - - - - - - - - - - - - -
	SyCall	AMALUFrz
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SYNCHRO ON / OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSynchroOn
; - - - - - - - - - - - - -
	moveq	#0,d1
	clr.w	InterOff(a5)
	SyCall	SetSync
	rts
; - - - - - - - - - - - - -
	Lib_Par InSynchroOff
; - - - - - - - - - - - - -
	moveq	#-1,d1
	move.w	d1,InterOff(a5)
	SyCall	SetSync
	rts
; - - - - - - - - - - - - -
	Lib_Par InSynchro
; - - - - - - - - - - - - -
	SyCall	Synchro
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					AMAL/MOVE/ANIM on/off/freeze
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InAmalOn0
; - - - - - - - - - - - - -
	moveq	#%0001,d2
	moveq	#1,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InAmalOff0
; - - - - - - - - - - - - -
	moveq	#%0001,d2
	moveq	#-1,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InAmalFreeze0
; - - - - - - - - - - - - -
	moveq	#%0001,d2
	moveq	#0,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InMoveOn0
; - - - - - - - - - - - - -
	moveq	#%1100,d2
	moveq	#1,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InAnimOn0
; - - - - - - - - - - - - -
	moveq	#%0010,d2
	moveq	#1,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InMoveOff0
; - - - - - - - - - - - - -
	moveq	#%1100,d2
	moveq	#-1,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InAnimOff0
; - - - - - - - - - - - - -
	moveq	#%0010,d2
	moveq	#-1,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InMoveFreeze0
; - - - - - - - - - - - - -
	moveq	#%1100,d2
	moveq	#0,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Par InAnimFreeze0
; - - - - - - - - - - - - -
	moveq	#%0010,d2
	moveq	#0,d3
	Rbra	L_MvOnOf0
; - - - - - - - - - - - - -
	Lib_Def	MvOnOf0
; - - - - - - - - - - - - -
	moveq	#-1,d1
	SyCall	AMALMvO
	rts

; - - - - - - - - - - - - -
	Lib_Par InAmalOn1
; - - - - - - - - - - - - -
	moveq	#%0001,d2
	moveq	#1,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InAmalOff1
; - - - - - - - - - - - - -
	moveq	#%0001,d2
	moveq	#-1,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InAmalFreeze1
; - - - - - - - - - - - - -
	moveq	#%0001,d2
	moveq	#0,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InMoveOn1
; - - - - - - - - - - - - -
	moveq	#%1100,d2
	moveq	#1,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InAnimOn1
; - - - - - - - - - - - - -
	moveq	#%0010,d2
	moveq	#1,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InMoveOff1
; - - - - - - - - - - - - -
	moveq	#%1100,d2
	moveq	#-1,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InAnimOff1
; - - - - - - - - - - - - -
	moveq	#%0010,d2
	moveq	#-1,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InMoveFreeze1
; - - - - - - - - - - - - -
	moveq	#%1100,d2
	moveq	#0,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Par InAnimFreeze1
; - - - - - - - - - - - - -
	moveq	#%0010,d2
	moveq	#0,d0
	Rbra	L_MvOnOf1
; - - - - - - - - - - - - -
	Lib_Def	MvOnOf1
; - - - - - - - - - - - - -
	move.l	d3,d1
	move.l	d0,d3
	SyCall	AMALMvO
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MOVE X/Y etc...
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMoveX3
; - - - - - - - - - - - - -
	moveq	#2,d0
	Rbra	L_AMm3
; - - - - - - - - - - - - -
	Lib_Par InMoveY3
; - - - - - - - - - - - - -
	moveq	#3,d0
	Rbra	L_AMm3
; - - - - - - - - - - - - -
	Lib_Par InAnim3
; - - - - - - - - - - - - -
	moveq	#1,d0
	Rbra	L_AMm3
; - - - - - - - - - - - - -
	Lib_Par InAmal3
; - - - - - - - - - - - - -
	moveq	#0,d0
	Rbra	L_AMm3
; - - - - - - - - - - - - -
	Lib_Def	AMm3
; - - - - - - - - - - - - -
	move.l	d3,d5
	and.l	#$FFFFFFFE,d5
	move.l	(a3)+,d3
	Rbra	L_MvA3
; - - - - - - - - - - - - -
	Lib_Par InMoveX2
; - - - - - - - - - - - - -
	moveq	#2,d0
	Rbra	L_MvA
; - - - - - - - - - - - - -
	Lib_Par InMoveY2
; - - - - - - - - - - - - -
	moveq	#3,d0
	Rbra	L_MvA
; - - - - - - - - - - - - -
	Lib_Par InAmal2
; - - - - - - - - - - - - -
	moveq	#0,d0
	Rbra	L_MvA
; - - - - - - - - - - - - -
	Lib_Par InAnim2
; - - - - - - - - - - - - -
	moveq	#1,d0
	Rbra	L_MvA
; - - - - - - - - - - - - -
	Lib_Def	MvA
; - - - - - - - - - - - - -
	moveq	#0,d5
	Rbra	L_MvA3
; - - - - - - - - - - - - -
	Lib_Def	MvA3
; - - - - - - - - - - - - -
	move.l	d3,a1
	move.l	d0,d3
	Rbsr	L_SaveRegs
	move.l	(a3)+,d6
	clr.w	PAmalE(a5)
* 16 si inter / 64 sinon!
	moveq	#16,d0
	tst.w	InterOff(a5)
	beq.s	InMva1
	moveq	#64,d0
InMva1:	cmp.l	d0,d6			* Nombre autorise
	Rbcc	L_FonCall
* Trouve la banque AMAL (numero 2)
	moveq	#0,d7
	moveq	#4,d0
	move.l	a1,a2
	Rjsr	L_Bnk.GetAdr
	move.l	a2,a1
	beq.s	InMb1
	move.l	-8(a0),d0
	cmp.l	#"Amal",d0
	bne.s	InMb1
	move.l	a0,d7
* Est-ce une chaine ou un numero?
InMb1:	cmp.l	#1024,a1
	bcc.s	InMb2
* Dans la banque!
	tst.l	d7
	Rbeq	L_BkNoRes
	tst.l	(a0)
	Rbeq	L_FonCall
	add.l	(a0),a0
	move.w	a1,d0
	move.l	ChVide(a5),a1
	cmp.w	(a0)+,d0
	Rbhi	L_FonCall
	lsl.w	#1,d0
	move.w	0(a0,d0.w),d0
	beq.s	InMb2
	lsl.w	#1,d0
	lea	0(a0,d0.w),a1
* Une chaine!
InMb2:	move.w	(a1)+,d0		* Met un zero a la fin de la chaine
	lea	0(a1,d0.w),a0
	move.b	(a0),d0
	clr.b	(a0)
	move.w	d0,-(sp)
	move.l	a0,-(sp)
	move.l	Buffer(a5),a2		* Buffer de tokenisation
	move.l	#TBuffer-256-64,d2
	move.l	a2,d1			* Buffer variables
	add.l	#TBuffer-256,d1
	tst.l	d5			* Numeros des canaux
	beq.s	InAMv0
	moveq	#5,d4
	bra.s	InAMv2
InAMv0:	lea	AnCanaux(a5),a0
	add.w	d6,a0
	add.w	d6,a0
	moveq	#0,d4
	moveq	#0,d5
	move.b	(a0)+,d4
	move.b	(a0)+,d5
InAMv2:	SyCall	AMALCre
	move.l	(sp)+,a1		* Remet la chaine!
	move.w	(sp)+,d7
	move.b	d7,(a1)
	tst.w	d0
	beq.s	.Exit
	bpl.s	IAmE
	cmp.w	#-1,d0
	Rbeq	L_OOfMem
	neg.w	d0
	Rbra	L_EcWiErr
.Exit	Rbsr	L_LoadRegs
	rts
IAmE:	move.w	a0,PAmalE(a5)		* Offset de l'erreur
	add.w	#SpEBase+2,d0
	Rbra	L_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=MOVON =CHANAM etc...
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnMovon
; - - - - - - - - - - - - -
	Rbsr	L_FnAm1
	movem.l	d6/d7,-(sp)
	SyCall	MovOn
	movem.l	(sp)+,d6/d7
	move.l	d1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnChanAn
; - - - - - - - - - - - - -
	Rbsr	L_FnAm1
	movem.l	d6/d7,-(sp)
	SyCall	ChanA
	movem.l	(sp)+,d6/d7
	move.l	d1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnChanMv
; - - - - - - - - - - - - -
	Rbsr	L_FnAm1
	movem.l	d6/d7,-(sp)
	SyCall	ChanM
	movem.l	(sp)+,d6/d7
	move.l	d1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnAm1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	cmp.l	#64,d1
	Rbcc	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=AMREG(n,[n])=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InAmreg1
; - - - - - - - - - - - - -
	move.l	d3,d5
	move.l	(a3)+,d3
	move.l	#EntNul,d1
	Rbra	L_IAmR
; - - - - - - - - - - - - -
	Lib_Par FnAmreg1
; - - - - - - - - - - - - -
	move.l	#EntNul,d1
	Rbra	L_FAmR
; - - - - - - - - - - - - -
	Lib_Par InAmreg2
; - - - - - - - - - - - - -
	move.l	d3,d5
	move.l	(a3)+,d3
	move.l	(a3)+,d1
	Rbra	L_IAmR
; - - - - - - - - - - - - -
	Lib_Par FnAmreg2
; - - - - - - - - - - - - -
	move.l	(a3)+,d1
	Rbra	L_FAmR
; - - - - - - - - - - - - -
	Lib_Def	IAmR
; - - - - - - - - - - - - -
	Rbsr	L_AmRR
	move.w	d5,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Def	FAmR
; - - - - - - - - - - - - -
	Rbsr	L_AmRR
	move.w	(a0),d3
	ext.l	d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Def	AmRR
; - - - - - - - - - - - - -
	moveq	#0,d2
	cmp.l	#EntNul,d1
	bne.s	IAmRa
	cmp.l	#26,d3
	Rbcc	L_FonCall
	moveq	#-1,d1
	bra.s	IAmRb
IAmRa:	cmp.l	#64,d1
	Rbcc	L_FonCall
	cmp.l	#10,d3
	Rbcc	L_FonCall
IAmRb:	SyCall	AMALReg
	Rbmi	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					AMPLAY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InAmPlay2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	clr.l	-(a3)
	moveq	#63,d3
	Rbra	L_InAmPlay4
; - - - - - - - - - - - - -
	Lib_Par InAmPlay4
; - - - - - - - - - - - - -
	move.l	d3,d2
	cmp.l	#64,d2
	Rbcc	L_FonCall
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	cmp.l	d1,d2
	Rbcs	L_FonCall
	move.l	(a3)+,d4
	move.l	(a3)+,d3
	SyCall	PlaySet
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=X BOB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXBob		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYBob		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=XY SPRITE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXSprite		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYSprite		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=IBOB = ISPRITE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnIBob		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnISprite		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					= XY MOUSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InXMouse		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnXMouse		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InYMouse		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYMouse		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=MOUSEKEY / CLICK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnMouseKey		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnMouseClick		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HIDE ON HIDE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InHide		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InHideOn		src/lib/sprites.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHOW SHOW ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InShow		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InShowOn		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LIMIT MOUSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InLimitMouse0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InLimitMouse1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			LimMX, now in src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InLimitMouse4		src/lib/sprites.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHANGE MOUSE n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InChangeMouse		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET BOB n,back,plane,minterm
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetBob		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOB n,x,y,a
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBob		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOB OFF x
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBobOff0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InBobOff1		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET SPRITE BUFFER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetSpriteBuffer		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SPRITE PRIORITY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSpritePriority		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SPRITE n,x,y,a
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSprite		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SPRITE OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSpriteOff0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InSpriteOff1		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Collision hard
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetHardcol		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnHardcol		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BOB SPRITE COL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnBobSpriteCol1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnBobSpriteCol3		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BOB COL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnBobCol1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnBobCol3		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPRITEBOBCOL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnSpriteBobCol1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnSpriteBobCol3		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPRITE COL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnSpriteCol1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnSpriteCol3		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=COL(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCol		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MASK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMakeIconMask0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMakeIconMask1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InNoIconMask0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InNoIconMask1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMakeMask0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMakeMask1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			MkMa1, now in src/lib/sprites.cpp

; - - - - - - - - - - - - -
	Lib_Int	InNoMask0		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InNoMask1		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			NoMa1, now in src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HOT SPOT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InHotSpot3		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InHotSpot2		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			HotSp, now in src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET SPRITE / BOB [sc],n,x,y to x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetSprite6		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InGetSprite5		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			GS, now in src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET ICON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InGetIcon6		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InGetIcon5		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			GI, now in src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Ritoune, now in src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PUT BOB n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPutBob		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FONCTIONS RETOURNEMENT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHRev		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnVRev		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnRev		src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PASTE BOB x,y,n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPasteBob		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	InPasteIcon		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Paste, now in src/lib/sprites.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPRITE / ICON BASE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnSpriteBase		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnIconBase		src/lib/sprites.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Sb, now in src/lib/sprites.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Trouve l'adresse d'un bob / Icone,
; Avec gestion des erreurs
; 	IN	D1=	Numero
;	OUT	A2=	Adresse
;		D5=	Max de bobs
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	AdBob
; - - - - - - - - - - - - -
	and.l	#$3FFF,d1
	Rbeq	L_FonCall
	move.l	d1,d0
	Rjsr	L_Bnk.AdBob
	Rbeq	L_AdBErr
	move.l	a0,a2
	move.w	d1,d5
	moveq	#0,d0
	rts
; - - - - - - - - - - - - -
	Lib_Def	AdIcon
; - - - - - - - - - - - - -
	and.l	#$3FFF,d1
	Rbeq	L_FonCall
	move.l	d1,d0
	Rjsr	L_Bnk.AdIcon
	Rbeq	L_AdBErr
	move.l	a0,a2
	move.w	d1,d5
	moveq	#0,d0
	rts
; - - - - - - - - - - - - -
	Lib_Def	AdBErr
; - - - - - - - - - - - - -
	tst.w	d1
	Rbeq	L_BkNoRes
	moveq	#EcEBase+30-1,d0
	Rbra	L_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Routine, copie le path dans PATHACT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	CopyPath		src/lib/files.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Branchements aux erreurs
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - -
	Lib_Int	DiskError		src/lib/dos.cpp
; - - - - - - - - - - - - -
	Lib_Int	DiskErr		src/lib/dos.cpp

; - - - - - - - - - - - - -
	Lib_Def	TypeMis
; - - - - - - - - - - - - -
	moveq	#34,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	FilOO
; - - - - - - - - - - - - -
	moveq	#DEBase+17,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	FilNO
; - - - - - - - - - - - - -
	moveq	#DEBase+18,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	FilTM
; - - - - - - - - - - - - -
	moveq	#DEBase+19,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	EOFil
; - - - - - - - - - - - - -
	moveq	#DEBase+21,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	IffFor2
; - - - - - - - - - - - - -
	moveq	#30,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	DiForm
; - - - - - - - - - - - - -
	moveq	#DEBase+16,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	ScNOp
; - - - - - - - - - - - - -
	moveq	#3,d0
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Def	RainErr
; - - - - - - - - - - - - -
	moveq	#31,d0
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Def	EcWiErr
; - - - - - - - - - - - - -
	cmp.w	#1,d0
	Rbeq	L_OOfMem
	add.w	#EcEBase-1,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	CopErr
; - - - - - - - - - - - - -
	add.w	#EcEBase+32-2,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	BkAlRes
; - - - - - - - - - - - - -
	moveq	#35,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	BkNoRes
; - - - - - - - - - - - - -
	moveq	#36,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	OOfMem
; - - - - - - - - - - - - -
	moveq 	#24,d0
	Rbra 	L_GoError
; - - - - - - - - - - - - -
	Lib_Par	Syntax
; - - - - - - - - - - - - -
	moveq 	#22,d0
	Rbra 	L_GoError
; - - - - - - - - - - - - -
	Lib_Par	Syntax2
; - - - - - - - - - - - - -
	Rbra	L_Syntax
; - - - - - - - - - - - - -
	Lib_Def	StooLong
; - - - - - - - - - - - - -
	moveq 	#21,d0
	Rbra 	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	FonCall
; - - - - - - - - - - - - -
	moveq 	#23,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	AdrErr
; - - - - - - - - - - - - -
	moveq	#25,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	CantFit
; - - - - - - - - - - - - -
	moveq	#32,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	IffFor
; - - - - - - - - - - - - -
	moveq	#30,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	IffCmp
; - - - - - - - - - - - - -
	moveq	#31,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	IllScN
; - - - - - - - - - - - - -
	moveq	#6,d0			* Illegal screen number
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Def	FftE			* Error font
; - - - - - - - - - - - - -
	moveq	#37,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Par FnfE			* Font not found
; - - - - - - - - - - - - -
	moveq	#44,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	BFonCall		* Erreur blocks
; - - - - - - - - - - - - -
	moveq	#22,d0
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Def	WFonCall
; - - - - - - - - - - - - -
	moveq	#16,d0
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Def	SpErr
; - - - - - - - - - - - - -
	moveq	#SpEBase,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Par InStop
; - - - - - - - - - - - - -
	moveq	#9,d0
	Rbra	L_GoError
; - - - - - - - - - - - - -
	Lib_Def	GoError
; - - - - - - - - - - - - -
	Rjmp	L_Error


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: ecrans.s / FENETRES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDOPEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWindopen5		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Int	InWindopen6		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Int	InWindopen7		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDSAVE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWindsave		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDMODE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWindmove		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDSIZE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWindsize		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDCLOSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWindclose		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDOW n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWindow		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=WINDON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnWindon		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WRITING
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InWriting1		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Int	InWriting2		src/lib/windows.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BORDER n,paper,n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBorder		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TITLE TOP a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InTitleTop		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TITLE BOTTOM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InTitleBottom		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Empty			WnTT, now in src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=XYCURS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXCurs		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnYCurs		src/lib/windows.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET CURS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetCurs		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LOCATE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int InLocate		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CENTRE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCentre		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CMOVE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCmove		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CURSPEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCursPen		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PAPER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPaper		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPen		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Empty			WnPp, now in src/lib/windows.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InClw		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HOME
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InHome		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEFT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCleft		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CRIGHT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCright		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CUP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCup		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CDOWN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCdown		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CURSOFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCursOff		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CURSON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCursOn		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INVERSE ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInverseOn		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INVERSE OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InInverseOff		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UNDER ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InUnderOn		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UNDER OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InUnderOff		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCROLLING ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScrollOn		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCROLLING OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InScrollOff		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHADE ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InShadeOn		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHADE OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InShadeOff		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEAR LINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCline0		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MEMORIZE X
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMemorizeX		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MEMORIZE Y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMemorizeY		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REMEMBER X
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRememberX		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REMEMBER Y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InRememberY		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEAR LINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InCline1		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HSCROLL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InHScroll		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					VSCROLL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InVScroll		src/lib/windows.cpp
; - - - - - - - - - - - - -
	Lib_Empty			HVSc, now in src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET TAB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InSetTab		src/lib/windows.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Envoie a la trappe
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			GoWn, now in src/lib/windows.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: String.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FREE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFree		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=INKEY$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnInkey		src/lib/keyboard.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SCANCODE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScancode		src/lib/keyboard.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SCANSHIFT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScanshift		src/lib/keyboard.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=KEY STATE(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnKeyState		src/lib/keyboard.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=KEY SHIFT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnKeyShift		src/lib/keyboard.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=JOY(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnJoy		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnJup		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnJdown		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnJleft		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnJright		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnFire		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FJ, now in src/lib/keyboard.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PUTKEY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InPutKey		src/lib/keyboard.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEARKEY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InClearKey		src/lib/keyboard.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					KEY$()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InKeyD		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnKeyD		src/lib/keyboard.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SCAN$()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnScan1		src/lib/keyboard.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnScan2		src/lib/keyboard.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=INSTR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnInstr2		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnInstr3		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINE INSTRFIND
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;       INSTR FIND: trouve une sous chaine dans une chaine a partir de d4
;       routine appelee par - chaine et INSTR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InstrFind		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=FLIP$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnFlip		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnLen		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPACE$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnSpace			src/lib/strings.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=STRING$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnString		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Empty			RString, now in src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=CHR$()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnChr		src/lib/strings.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=TAB$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnTab		src/lib/strings.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=Cleft$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCleft		src/lib/strings.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=Cright
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCright		src/lib/strings.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=Cup
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCup		src/lib/strings.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=Cdown
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCdown		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FinChr, now in src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=PAPER$=PEN$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnPenD		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnPaperD		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Empty			FPn, now in src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					AT(x,y)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnAt		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=CMOVE$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnCMoveD		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=REPEAT$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnRepeat		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BORDER$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnBorderD		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=ZONE$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnZoneD		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Routine Zone/Border
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			FinRpt, now in src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=LOWER$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnLower			src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=UPPER$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnUpper			src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=ASC
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnAsc		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BIN$()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnBin1		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnBin2		src/lib/strings.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=HEX$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnHex1		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnHex2		src/lib/strings.cpp
; - - - - - - - - - - - - -
	Lib_Empty			BinHex, now in src/lib/strings.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: IO.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Initialisation � chaud des dialogues
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dia_WarmInit		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG OPEN channel,programmes[,variable,buffer]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDialogOpen2		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogOpen3		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogOpen4		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Branchement aux erreurs IO
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_GoError, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	=Edialog: position de la derniere erreur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnEDialog		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG CLOSE [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDialogClose0		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogClose1		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG CLR c
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDialogClr		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG FREEZE [c]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDialogFreeze0		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogFreeze1		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG UNFREEZE [c]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDialogUnFreeze0		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogUnFreeze1		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG UPDATE n,z,[value]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InDialogUpdate2		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogUpdate3		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogUpdate4		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	InDialogUpdate5		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=DIALOG RUN( channel[,label][,x,y] )
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDialogRun1		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDialogRun2		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDialogRun4		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=DIALOG(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDialog		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=VDIALOG(n,n)=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InVDialog		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnVDialog		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=VDIALOG$()=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InVDialogD		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnVDialogD		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=RDIALOG(n,n[,n])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnRDialog2		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnRDialog3		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=RDIALOG$()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnRDialogD2		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnRDialogD3		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=ZDIALOG(channel,x,y)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnZDialog		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=DIALOG BOX(a$[,v][,v$][,x,y])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnDialogBox1		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDialogBox2		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDialogBox3		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnDialogBox5		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		READ TEXT a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InReadText1		src/lib/interface.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trois parametres: titre,adresse,longueur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InReadText3		src/lib/interface.cpp
; - - - - - - - - - - - - -
	Lib_Empty			IRText, now in src/lib/interface.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		RESOURCE SCREEN OPEN n,sx,sy,flash
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResourceScreenOpen		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		RESOURCE BANK n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResourceBank		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retourne l'adresse de la banque PUZZLE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dia_GetPuzzle		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retourne la banque par defaut
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dia_GetDefault		src/lib/interface.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	RESOURCE UNPACK n,x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InResourceUnpack		src/lib/interface.cpp

; __________________________________________________________________________
;
; 	AREXX interface!

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=AREXX EXISTS("port")
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnArexxExist		src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		AREXX OPEN "PORT_NAME"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InArexxOpen		src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=AREXX
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnArexx		src/lib/arexx.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		AREXX WAIT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InArexxWait		src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		AREXX CLOSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InArexxClose		src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=AREXX$(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnArexxD		src/lib/arexx.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		AREXX ANSWER error [,"answer"]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InArexxAnswer1		src/lib/arexx.cpp
; - - - - - - - - - - - - -
	Lib_Int	InArexxAnswer2		src/lib/arexx.cpp


; _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
;
;						ROUTINES AREXX

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Close the Arexx port
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Arx_Close, now in src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		Ouverture du port AREXX
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Arx_Open, now in src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Returns -1 if message port exist
;	A0=	Port name
;	D0=-1	Create port!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Arx_RegisterPort, now in src/lib/arexx.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		Get message, and returns -1 if message waiting
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Arx_Message, now in src/lib/arexx.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Menus.s / Instructions
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ON MENU ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOnMenuOn		src/lib/menus.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ON MENU ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOnMenuOff		src/lib/menus.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	ON MENU DEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InOnMenuDel		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Routine: Efface les ON MENU
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	OMnEff		src/lib/menus.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	MENU TO BANK n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuToBank		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	BANK TO MENU n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InBankToMenu		src/lib/menus.cpp




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuOn		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuOff		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU MOUSE ON/OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuMouseOn		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuMouseOff		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU BASE x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuBase		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=MENU X(,,,)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnXMenu		src/lib/menus.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=MENU Y(,,,)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnYMenu		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=CHOICE(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnChoice0		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	FnChoice1		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuBar		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuLine		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuTline		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuMovable		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuStatic		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuItemMovable		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuItemStatic		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuActive		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuInactive		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuSeparate		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuLink		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuCalled		src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Int	InMenuOnce		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU CALCULATE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	InMenuCalc		src/lib/menus.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Menu not opened
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnNOp, now in src/lib/menus.cpp



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEBUT DES ROUTINES EXTERNES:
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Start_Externes
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Menus.s / Routines
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Start_Menus
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GESTION MENUS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MnGere		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Ferme tout les menus!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnEnd, now in src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Empty			MnEnd1, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface le menu---> niveau D5
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnDEff, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la branche A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnEBranch, now in src/lib/menus.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MNCALC: Calcule les positions de l'arbre
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnCalc, now in src/lib/menus.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Dessin d'une branche de l'arbre
;	FIXE LES ZONES!
;	A2=	Base a dessiner
;	D4/D5=	Coordonnee de base
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnBranch, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Dessin / Calcul taille d'une case de menu (A2) - D4/D5
;	#31 de D7->	Dessin(0) / Calcule(1)
;	#30 de D7->	Inactif(0) / Actif(1)
;	#29 de D7->	PasFond(0) / Fond(1)
;	D4/D5	 -> 	Coordonnees
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnDraw, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Dessine un objet
;	D1/D2/D3= Encres par default
;	D4/D5	= X/Y
;	Ramene:	D0/D1 -> Position curseur!
;	Ramene: D2/D3 -> Taille maxi!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnODraw, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Efface espace local pour procedures
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnODVar, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Sauve le contenu du carre D4/D5/D6/D7 / Base:	A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnSave, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Remet la portion de decor sauvee
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnRest, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface -si present- la memoire reservee pour le fond
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnSaDel, now in src/lib/menus.cpp

; - - - - - - - - - - - - -
	Lib_Empty			ObInkC, now in src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ObInkB, now in src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ObInkA, now in src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ObWrite, now in src/lib/menus.cpp
; - - - - - - - - - - - - -
	Lib_Empty			ObPat, now in src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve le menu (A3)++, verifie les params, profondeur D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MnFind		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Insere le menu A2/(A3)/D5 dans la liste
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MnIns		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	RESET MENUS TO DEFAULT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MenuReset		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface tous les menus
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MnRaz		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Insere la routine dans CLEARVAR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MnClearVar		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Enleve l'objet a partir de (D0), et ses collaterales
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MnEff		src/lib/menus.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Enleve l'objet (A2)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			MnDel, now in src/lib/menus.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Tokenisation objets de menu
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Interpreteur de definition d'objet!
;	A1	-> chaine a interpreter
;	Retour	-> D0= adresse de l'objet
; - - - - - - - - - - - - -
	Lib_Int	MnObjet		src/lib/menus.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Exploration des MENU KEY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	MenuKeyExplore		src/lib/menus.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DES MENUS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_Menus
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Fsel.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Start_FSel
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SELECTEUR DE FICHIER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dsk.FileSelector		src/lib/fsel.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DU SELECTEUR DE FICHIERS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_FSel
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CREATE TASK / Amiga.Lib
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			CreateTask, now in src/lib/fsel.cpp





; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Dialog.s
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


;_____________________________________________________________________________
;
;						EDITEUR LIGNE
;_____________________________________________________________________________
;
;
; Initialisation de l'�diteur ligne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0= 	Buffer de donnees
;	A1=	Chaine � imprimer
;	A2=	Buffer de stockage
;	D0=	Flags
;	D1=	Curseur initial
;	D2=	Longueur maxi
;	D3=	Largeur maxi (-1= maximum)
; - - - - - - - - - - - - -
	Lib_Int	LEd_Init		src/lib/lineedit.cpp


; 	Boucle de l'editeur ligne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Buffer de donn�es
;	Sortie:
;	D0/A0	Chaine
;	D1	Derniere touche
;	D2	0=	Rien si out
;		1=	Return
;		2= 	Souris en dehors zone
;		-1=	Control-C si flag
; - - - - - - - - - - - - -
	Lib_Int	LEd_Loop		src/lib/lineedit.cpp

; Remplacement de la ligne editee par une autre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Structure
;	A1=	Nouvelle ligne
;	D0=	Longueur (ou -1)
; - - - - - - - - - - - - -
	Lib_Int	LEd_New		src/lib/lineedit.cpp

; 	Impression de la ligne courante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			LEd_Print, now in src/lib/dialog.cpp

; Arret curseur ligne editee
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			LEd_CuStoppe, now in src/lib/dialog.cpp
; Remise en route curseur
; ~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			LEd_CuMarche, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			LEd_FauCu, now in src/lib/dialog.cpp

;
; Ramene VRAI si D2 est une lettre ou un chiffre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			LEd_Lettre, now in src/lib/dialog.cpp

; - - - - - - - - - - - - -
	Lib_Int	Dia_WaitMul		src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEBUT DES ROUTINES DIALOGUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Start_Dialogs
; - - - - - - - - - - - - -

; ___________________________________________________________________________
;
; 	Initialisation d'un canal de dialogue
; ___________________________________________________________________________
;
; IN
;	A0= 	Adresse du programme
;	A1=	Adresse banque puzzle
;	A2=	Base des messages
;	D0=	Numero du canal
;	D1=	Longueur Buffer
;	D2=	Nombre de Variables internes
;	D3=	Flags ouverture
;			Bit0= Recopier la chaine origine?
; OUT
;	D0=	Erreur
;	D1= 	Position dans le programme si erreur
;	A0=	Adresse du canal
;	A1=	Adresse des variables du canal
; - - - - - - - - - - - - -
	Lib_Int	Dia_OpenChannel		src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Demande un espace buffer temporaire
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_GetBuffer, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Memorise la position du buffer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_SetBuffer, now in src/lib/dialog.cpp

; ___________________________________________________________________________
;
;	Active l'�cran d'une boite de dialogue
; ___________________________________________________________________________
;
; IN
;	A4=	Boite de dialogue
; OUT
;	D0=	Erreur
;
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Active, now in src/lib/dialog.cpp

; ___________________________________________________________________________
;
;	Re-Active l'ancien ecran
; ___________________________________________________________________________
;
; IN
;	A4=	Boite de dialogue
;
; - - - - - - - - - - - - -
	Lib_Empty			Dia_ReActive, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Branchement de la routine de fermeture CLEARVAR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_ClearVar, now in src/lib/dialog.cpp

; ___________________________________________________________________________
;
;	Effacement de tous les canaux de dialogues
; ___________________________________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_CloseChannels		src/lib/dialog.cpp

; ___________________________________________________________________________
;
;	Effacement d'un canal de dialogue
; ___________________________________________________________________________
;
; IN
;	D0=	Numero du canal
; OUT
;	D0=	Erreurs
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_CloseChannel		src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Ferme le canal A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_CloseA0, now in src/lib/dialog.cpp

; _______________________________________________________________________
;
;	Execution Instantann�e
; _______________________________________________________________________
;
; IN
;	A0=	Programme
;	A1= 	Puzzle
;	A2=	Messages
;	D0=	X
;	D1=	Y
;	D2=	Variable 0
;	D3=	Variable 1
;	D4=	Variable 2
;	D5=	Flags
;	D6=	Buffer
; OUT
;	D0=	Erreur
;	D1=	Reponse
; - - - - - - - - - - - - -
	Lib_Int	Dia_RunQuick		src/lib/dialog.cpp

; _______________________________________________________________________
;
;	Update d'une zone
; _______________________________________________________________________
;
; IN
;	D0=	Numero du canal
;	D1=	Numero de la zone
;	D2=	Parametre I
;	D3=	Parametre II
;	D4=	Parametre III
; OUT
;	D0=	Erreur
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_Update		src/lib/dialog.cpp

; _______________________________________________________________________
;
;	Execution / Demarrage d'un dialogue
; _______________________________________________________________________
;
; IN
;	D0=	Numero du canal
;	D1=	-1 / Numero du programme
;	D2=	Base X
;	D3=	Base Y
; OUT
;	D0=	Erreur
;	D1=	Parametre eventuel
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_RunProgram		src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Trouve le label D0>>>D0
;	BEQ= 	pas trouve
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_GetLabel, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Freeze tous les canaux
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dia_FreezeChannels		src/lib/dialog.cpp
; _______________________________________________________________________
;
;	FREEZE / UNFREEZE  canal
; _______________________________________________________________________
;
; IN
;	D0=	Numero du canal
;	D1=	Freeze (-1) Unfreeze (0)
; OUT
;	D0=	Erreur
;
; Un seul canal
; - - - - - - - - - - - - -
	Lib_Int	Dia_FreezeChannel		src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_FrzA0, now in src/lib/dialog.cpp

; _______________________________________________________________________
;
;	Effacement d'un canal
; _______________________________________________________________________
;
; IN
;	D0=	Numero du canal
; OUT
;	D0=	Erreur
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_EffChannel		src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Efface une boite de dialogue dans un canal A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EffChanA0, now in src/lib/dialog.cpp

; ___________________________________________________
;
;	Retourne l'adresse de la variable D0/D1
; ___________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_GetVariable		src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retourne le numero de la zone sous D1/D2 dans le canal D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dia_GetZone		src/lib/dialog.cpp

; ___________________________________________________
;
;	Retourne la valeur de la zone D1/D2, Channel D0
; ___________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_GetValue		src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Retourne l'adresse de la zone Channel D0 / Zone D1 - D2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Dia_GetZoneAd		src/lib/dialog.cpp


; __________________________________________
;
; Positionne les variables en fonction de D0
; __________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_SetVFlags		src/lib/dialog.cpp

; ___________________________________________
;
; Recupere les memes variables
; ___________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_GetVFlags		src/lib/dialog.cpp

;
; Retourne le parametre d'une boite de dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; IN
;	D0=	Channel
; OUT
;	D0=	Erreur
;	D1=	Parametre
; - - - - - - - - - - - - -
	Lib_Int	Dia_GetReturn		src/lib/dialog.cpp

;_____________________________________________________________________________
;
;					RESOURCE SCREEN OPEN
;_____________________________________________________________________________
;
;	A0=	Adresse Puzzle
;	D0=	Numero
;	D1= 	Sx
;	D2=	Sy
;	D3=	Flash
;	D4=	Interlaced? (-1:comme resource / 0-1: force)
; - - - - - - - - - - - - -
	Lib_Int	Dia_RScOpen		src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	LISTE DES INSTRUCTIONS DIALOGUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_Instr, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEFINITIONS DES TOKENS FONCTIONS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_FTokens, now in src/lib/dialog.cpp

; ___________________________________________________________________________
;
;	GESTION DES BOITES DE DIALOGUE
; ___________________________________________________________________________
;

; _____________________________________________________________________________
;

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INTERPRETATION DU PROGRAMME
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_Loop, now in src/lib/dialog.cpp



; ________________________________________________________________________
;
; 	EVALUATION D'EXPRESSION
;
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Evalue, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				ROUTINES GESTIONS CHAINE / CONVERSIONS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FONCTION: Conversion en d�cimal CHIF #
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_FDecimal, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Fixe le debut d'une chaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_StDebut, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Calcule le buffer pour une chaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_StFini, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES GESTIONS ZONE D'EDITION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la fenetre de dialogue A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EdEnd, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Active la ligne de dialogue A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EdActive, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Fenetre dialogue A0 en route
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EdOn, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Inactive la fenetre A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EdInactive, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Active s'il faut, la fenetre D1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_WActive, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Passe paper/pen � zero pour effacement
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_WiZero, now in src/lib/dialog.cpp



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES DE GESTION DE L'HYPERTEXTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Activation de la fenetre texte, A2=structure
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_TxActive, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la fenetre de texte A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_TxEnd, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affiche la page de texte
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_TxDraw, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affiche la ligne #D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_TxAff, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affiche le mot actif A0(pointeurs) D0=Y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_TxAffActive, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES GESTION LISTES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Activation de la fenetre liste A2=liste
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_LiActive, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la fenetre de liste A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_LiEnd, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affichage du contenu d'une liste - A2=liste
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_LiDraw, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affichage d'un des �l�ments de la liste - A2 structure - D0=#
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_LiAff, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DE GESTION SLIDERS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Dessine le slider A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_SlDraw, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINE DESSIN BOUTON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_BtDraw, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DIVERSES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Delimite une routine []
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_GetRout, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve la fin d'une parenthese
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EndP, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UPDATE LA ZONE D2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_ZUpdate, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Routine CHANGE zone active (bouton / liste active / slider)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_ZoChange, now in src/lib/dialog.cpp

;
; 	Trouve l'adresse du dialogue numero D0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	OUT	BEQ Pas trouve, BNE Trouve, D0=Flags / A1=Adresse
; - - - - - - - - - - - - -
	Lib_Int	Dia_GetChannel		src/lib/dialog.cpp


; ________________________________________________________________________
;
;	ROUTINES D'ANIMATION DE L'ARBRE
;
; Exploration de l'arbre par le test
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Int	Dia_AutoTest		src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Int	Dia_AutoTest2		src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UN TOUR DE L'ARBRE D'OBJETS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_Tests, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Recherche une zone dans l'arbre
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_GetMouseZone, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve la zone D1/D2 (coords ecran)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_GetZ, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Activation de la premiere fenetre d'�dition
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_EdFirst, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_EdNext, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			EdaLoop, now in src/lib/dialog.cpp


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERREURS DE L'INTERPRETEUR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_OBuffer, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_NPar, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Fonc, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Synt, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Er, now in src/lib/dialog.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Quit, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHRGET rapide...
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_Chr, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Prend un chiffre
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_FChif, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHRGET SPECIAL CHIFFRES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_ChrC, now in src/lib/dialog.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Conversion Decimal>>>ascii
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty			Dia_DecToAsc, now in src/lib/dialog.cpp


;_____________________________________________________________________________
;
;					Gestion g�n�ralisee des boutons
;_____________________________________________________________________________
;
; __________________________________________________
;
;	Initialisation d'une liste de boutons
; __________________________________________________
;
;	A0=	Premier bouton
;	D0=	Dessiner ?
; _____________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Bt_InitList		src/lib/dialog.cpp

; __________________________________________________
;
;	Initialisation d'un bouton
; __________________________________________________
;
;	A0=	Structure Bouton
;	D0=	Dessiner ?
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Bt_Init		src/lib/dialog.cpp

; __________________________________________________
;
;	Fin d'un bouton
; __________________________________________________
;
;	A0=	Structure Bouton
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Bt_End		src/lib/dialog.cpp

; __________________________________________________
;
;	Gestion des boutons
; __________________________________________________
;
;	A0=	Premiere structure Bouton
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Bt_Gere		src/lib/dialog.cpp

; Appelle la routine de modification bouton
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			Bt_CChange, now in src/lib/dialog.cpp
; Ramene la position du bouton
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			Bt_CPos, now in src/lib/dialog.cpp
; Va dessiner un bouton
; ~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			Bt_CDraw, now in src/lib/dialog.cpp
; Appelle la routine
; ~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			Bt_Call, now in src/lib/dialog.cpp

;_____________________________________________________________________________
;
;					Gestion g�n�ralisee des sliders.
;_____________________________________________________________________________
;
; __________________________________________________
;
;	Initialisation d'un slider
; __________________________________________________
;
;	A0=	Structure slider
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Sl_Init		src/lib/dialog.cpp

; Appel d'une routine slider
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Empty			Sl_Call, now in src/lib/dialog.cpp

; __________________________________________________
;
;	Gestion d'un slider
; __________________________________________________
;
;	A0=	Structure slider
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Sl_Clic		src/lib/dialog.cpp

; __________________________________________________
;
;	Dessin d'un slider
; __________________________________________________
;
;	A0=	Structure slider
;	D0=	0= Normal / 1 Active
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Sl_Draw		src/lib/dialog.cpp

; _________________________________________________
;
;	Attend le relachement de la souris
; _________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_NoMKey		src/lib/dialog.cpp

; _________________________________________________
;
;	Nettoyage du buffer clavier
; _________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	Dia_ClearKey		src/lib/dialog.cpp
; _________________________________________________
;
;	Appel du patch
; _________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Empty			Dia_Patch, now in src/lib/dialog.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DES DIALOGUES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_Dialogs
; - - - - - - - - - - - - -

; _________________________________________________
;
;	Screen copy
; _________________________________________________
;
;	D0=	X1
;	D1= 	Y1
;	D2=	X3
;	D3=	Y3
;	D4=	X2
;	D5=	Y2
;	A0=	Ecran Source
;	A1=	Ecran Destination
; _________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	ScCopy		src/lib/unpack.cpp
; - - - - - - - - - - - - -
	Lib_Empty			Dia_ScCopy, now in src/lib/screens.cpp

; _________________________________________________
;
;	Unpacker Screen
; _________________________________________________
;
;	A0=	Packed picture
;	D0=	Destination Screen
; _________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	UnPack_Screen		src/lib/unpack.cpp

; _________________________________________________
;
;	Unpacker Bitmap
; _________________________________________________
;
;	A0=	Packed picture
;	A1=	Destination Screen
;	D1=	X
;	D2=	Y
; _________________________________
;
; - - - - - - - - - - - - -
	Lib_Int	UnPack_Bitmap		src/lib/unpack.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DE CONVERSION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	LONGDEC (Long>decimal, signe eventuel, proportionel)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	LongToDec		src/lib/convert.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Conversion LONG (D0) >>> Ascii (a0)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	LongToAsc		src/lib/convert.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Conversion D0.l >>> Ascii hex
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	LongToHex		src/lib/convert.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Conversion D0.l >>> Binaire Ascii
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	LongToBin		src/lib/convert.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FFP single precision to IEEE single precision
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	FFP2Ieee
; - - - - - - - - - - - - -
	ADD.L	D0,D0
	BEQ.S	L21418A
	EORI.B	#$80,D0
	ASR.B	#1,D0
	SUBI.B	#$82,D0
	SWAP	D0
	ROL.L	#7,D0
L21418A	RTS

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	IEEE single precision to FFP single precision
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Ieee2FFP
; - - - - - - - - - - - - -
	SWAP	D0
	ROR.L	#7,D0
	EORI.B	#$80,D0
	ADD.B	D0,D0
	BVS.S	L2141A4
	ADDQ.B	#5,D0
	BVS.S	L2141DA
	EORI.B	#$80,D0
	ROR.L	#1,D0
L2141A2	RTS
L2141A4	BCC.S	L2141CC
	CMPI.B	#$7C,D0
	BEQ.S	L2141B2
	CMPI.B	#$7E,D0
	BNE.S	L2141BE
L2141B2	ADDI.B	#$85,D0
	ROR.L	#1,D0
	TST.B	D0
	BNE.S	L2141A2
	BRA.S	L2141C8
L2141BE	ANDI.W	#$FEFF,D0
	TST.L	D0
	BEQ.S	L2141A2
	TST.B	D0
L2141C8	MOVEQ	#0,D0
	BRA.S	L2141A2
L2141CC	CMPI.B	#$FE,D0
	BNE.S	L2141DA
	LSR.L	#8,D0
	LSR.L	#1,D0
	BNE.S	L2141E6
	BRA.S	L2141DC
L2141DA	LSL.W	#8,D0
L2141DC	MOVEQ	#-1,D0
	ROXR.B	#1,D0
	ORI.B	#2,CCR
	BRA.S	L2141A2
L2141E6	MOVEQ	#0,D0
	BRA.S	L2141A2

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	IEEE single precision to ieee double precision
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Sp2Dp
; - - - - - - - - - - - - -
	MOVEA.L	D0,A0
	SWAP	D0
	BEQ.S	.SKIP
	MOVE.W	D0,D1
	ANDI.W	#$7F80,D0
	ASR.W	#3,D0
	ADDI.W	#$3800,D0
	ANDI.W	#$8000,D1
	OR.W	D1,D0
	SWAP	D0
	MOVE.L	A0,D1
	ROR.L	#3,D1
	MOVEA.L	D1,A0
	ANDI.L	#$FFFFF,D1
	OR.L	D1,D0
	MOVE.L	A0,D1
	ANDI.L	#$E0000000,D1
	RTS
.SKIP	MOVEQ	#0,D1
	RTS

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	IEEE Double precision to IEEE single precision
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dp2Sp
; - - - - - - - - - - - - -
	SWAP	D0
	BEQ.S	L2ECFA4
	MOVE.W	D0,D1
	SWAP	D1
	SWAP	D0
	ASL.W	#1,D1
	ROXL.L	#1,D0
	ASL.W	#1,D1
	ROXL.L	#1,D0
	ASL.W	#1,D1
	ROXL.L	#1,D0
	SWAP	D0
	ANDI.W	#$7F,D0
	SWAP	D1
	MOVEA.W	D1,A0
	ANDI.W	#$8000,D1
	OR.W	D1,D0
	MOVE.W	A0,D1
	ANDI.W	#$7FF0,D1
	SUBI.W	#$3800,D1
	BGE.S	L2ECFA6
	CLR.L	D0
L2ECFA4	RTS
L2ECFA6	CMPI.W	#$FF0,D1
	BLE.S	L2ECFBC
	ORI.B	#2,CCR
;	TRAPV
	ORI.L	#$FFFF7FFF,D0
	SWAP	D0
	RTS
L2ECFBC	ASL.W	#3,D1
	OR.W	D1,D0
	SWAP	D0
	RTS

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Ascii vers float, simple precision
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	AscToFloat
; - - - - - - - - - - - - -
	Rlea	L_FloatToAsc,0
	jmp	a2ffp-fldeb(a0)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Float to ascii, simple precision
;	D0	Float
;	A0	Buffer
;	D4	Flags
;	D5	Flags exp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	FloatToAsc
; - - - - - - - - - - - - -
fldeb	movem.l	a1-a3,-(sp)
	move.l	a0,a3
	lea	DeFloat(a5),a0
	bsr	F2a
	btst	#31,d4			* Met un espace si >0?
	bne.s	p00a
	move.b	#32,(a3)+
	cmp.b	#"-",(a0)
	bne.s	p00a
	subq.l	#1,a3
p00a:   tst 	d4
        bmi.s 	p0b
p0a:    move.b 	(a0)+,(a3)+  		* FIX: imprime tout defloat!
        bne.s 	p0a
        bra 	p7
p0b:    move.l 	a0,a2
p1:     move.b 	(a2)+,d0
        cmp.b 	#".",d0
        beq.s 	p1a
        move.b 	d0,(a3)+
        bne.s 	p1
        bra.s 	p7
p1a:    move.l 	a2,a1        		;a1= ancien non nul
        move.l 	a2,a0
p2:     move.b 	(a0)+,d0
        beq.s 	p3
        cmp.b 	#"E",d0
        beq.s 	p3
        cmp.b 	#"0",d0
        beq.s 	p2
        move.l 	a0,a1
        bra.s 	p2
p3:     subq.l 	#1,a0
        move.l 	a0,d0        		;adresse de la fin du chiffre
        cmp.l 	a2,a1         		;imprime les chiffres utiles
        beq.s 	p5
        move.b 	#".",(a3)+
p4:     move.b 	(a2)+,(a3)+
        cmp.l 	a2,a1
        bne.s 	p4
p5:     move.l 	d0,a2
        cmp.b 	#"E",(a2)
        bne.s 	p6
        move.b 	#32,(a3)+    		;imprime un espace avant le E
p6:     move.b 	(a2)+,(a3)+
        bne.s 	p6
p7:     lea	-1(a3),a0
	movem.l	(sp)+,a1-a3
        rts
;------ FLOAT to ASCII
Zero:	dc.b 	"0.0000000",0
F2a	movem.l a2/d1-d5,-(sp)
	movem.l	a3-a6,-(sp)
	move.l 	a0,TempBuf(a5)
	move.l 	d0,TempFl(a5)
	tst.w 	d5		;D5= FIX FLAG
	bne.s 	PaFix
	tst.w 	d4
	bmi.s 	PaFix
; Fix precise NORMAL
	cmp.w 	#8,d4
	bcs.s 	Fx1
	moveq 	#7,d4
Fx1:	move.w 	d4,-(sp)
	move.l 	a0,-(sp)
	move.l 	d0,-(sp)
	bsr 	ffp2a
	lea 	10(sp),sp
	movem.l	(sp)+,a3-a6
	tst.w 	d4		;Si FIX=0, enleve le POINT!
	bne.s 	Fx2
	cmp.b 	#".",-1(a0)
	bne.s 	Fx2
	clr.b 	-(a0)
Fx2:	sub.l 	d0,a0
	exg 	d0,a0
	movem.l (sp)+,a2/d1-d5
	rts
; Representation proportionnelle
PaFix:	move.b 	d0,d1
	and.b 	#$7f,d1
	cmp.b 	#$41,d1
	bcs.s 	PaF1
	move.w 	#7,-(sp)
	bra.s 	PaF5
PaF1:	cmp.b 	#$31,d1
	bcs.s 	PaF2
	move.w 	#7+3,-(sp)
	bra.s 	PaF5
PaF2:	move.w 	#16+6,-(sp)	;Si >-1 et <1, demande 16 chiffres!
PaF5:	pea	BuFloat(a5)
	move.l 	d0,-(sp)
	bsr	ffp2a
	lea 	10(sp),sp
	movem.l	(sp),a3-a6
	move.l 	d0,a1
	cmp.b 	#"-",(a1)
	bne.s 	PaFix1
	addq.l 	#1,a1
PaFix1:	move.l 	a1,a0
	cmp.b 	#"0",(a0)
	beq.s 	PaFix5
;-----> Chiffre >1
PaFix2:	move.b 	(a0)+,d0		;Compte AVANT la virgule
	beq.s 	PaFix3
	cmp.b 	#$2e,d0
	bne.s 	PaFix2
PaFix3:	sub.l 	a1,a0
	tst.w 	d5
	bne 	ExFix1
	moveq 	#7,d0
	cmp.w 	#8,a0		;Si >7 ---> representation E+
	bcc 	ExFix1
	sub.w 	a0,d0
	cmp.b 	#5,d0
	bcs 	Clean
	moveq 	#5,d0
	bra 	Clean
;-----> Chiffre <1
PaFix5:	addq.l 	#1,a0
	addq.l 	#1,a0
	move.l 	a0,a1
PaFix6:	move.b 	(a0)+,d0		;Compte le nombre de ZEROS
	beq.s 	PaFix7
	cmp.b 	#"0",d0
	beq.s 	PaFix6
PaFix7:	sub.l 	a1,a0
	clr.w 	d0
	cmp.w 	#16+6,a0		;Est-ce un vrai ZERO?
	bcs.s 	PaFix8
	move.w 	#6,a0
	moveq 	#1,d0
	bra.s 	PaFix9
PaFix8:	cmp.w 	#4,a0		;0.0001 ---> Exponantielle
	bcc 	ExVir1
PaFix9:	tst.w 	d5
	bne 	ExVir1
	addq.w 	#6,a0
	move.w 	a0,d0
; Calcule BIEN, et nettoie le chiffre
Clean:	move.w 	d0,-(sp)
	pea	BuFloat(a5)
	move.l 	TempFl(a5),-(sp)
	bsr 	ffp2a
	lea 	10(sp),sp
	movem.l	(sp)+,a3-a6
	move.l 	d0,a0
	move.l 	TempBuf(a5),a1
Cl1:	move.b 	(a0)+,d0
	beq.s	Cl5
	move.b 	d0,(a1)+
	cmp.b 	#".",d0
	bne.s 	Cl1
	lea 	-1(a1),a2
Cl2:	move.b 	(a0)+,d0
	beq.s 	Cl3
	move.b 	d0,(a1)+
	cmp.b 	#"0",d0
	beq.s 	Cl2
	move.l 	a1,a2		;Dernier non nul
	bra.s 	Cl2
Cl3:	move.l 	a2,a1
Cl5:	clr.b 	(a1)
	move.l 	TempBuf(a5),a0
	sub.l 	a0,a1
	move.l 	a1,d0
	movem.l (sp)+,a2/d1-d5
	rts

;-----> Representation exponantielle >= 1
ExFix1:	move.w 	a0,d2
	subq.w 	#2,d2
	cmp.w 	#7,a0
	bcs.s 	Exf0
	move.w 	#7,a0
Exf0:	moveq 	#9,d0
	sub.w 	a0,d0
	move.w 	d0,-(sp)
	pea	BuFloat(a5)
	move.l 	TempFl(a5),-(sp)
	bsr 	ffp2a
	lea 	10(sp),sp
	movem.l	(sp)+,a3-a6

	lea	BuFloat(a5),a0
	move.l 	TempBuf(a5),a1
	cmp.b 	#"-",(a0)
	bne.s 	Exf0a
	move.b 	(a0)+,(a1)+
Exf0a:	move.b 	(a0)+,(a1)+
	move.b 	#".",(a1)+
	lea 	-1(a1),a2
	move.w 	d4,d1
	bpl.s 	Exf1
	moveq 	#5,d1
	bra.s 	Exf2
Exf1:	cmp.w 	#5,d1
	bcs.s 	Exf2
	moveq 	#5,d1
Exf2:	move.b 	(a0)+,d0
	beq.s 	Exf2a
	cmp.b 	#".",d0
	beq.s 	Exf2
	move.b 	d0,(a1)+
	subq.w 	#1,d1
	bne.s 	Exf2
Exf2a:	clr.b 	(a1)
	tst.w 	d4
	bpl.s 	Exf5
; Enleve les zeros
	lea 	1(a2),a0
Exf3:	move.b 	(a0)+,d0
	beq.s 	Exf4
	cmp.b 	#"0",d0
	beq.s 	Exf3
	move.l 	a0,a2		;Dernier non nul
	bra.s 	Exf3
Exf4:	move.l 	a2,a1
; Rajoute le E+00
Exf5:	move.b 	#"E",(a1)+
	move.b 	#"+",(a1)+
	move.b 	#"0",(a1)+
Exf6:	cmp.b 	#10,d2
	bcs.s 	Exf7
	add.b 	#1,-1(a1)
	sub.w 	#10,d2
	bra.s 	Exf6
Exf7:	add.b 	#"0",d2
	move.b 	d2,(a1)+
	clr.b 	(a1)
	move.l 	TempBuf(a5),a0
	sub.l 	a0,a1
	move.l 	a1,d0
	movem.l (sp)+,a2/d1-d5
	rts

;-----> Exponantielle <1
ExVir1:	tst.w 	d0
	beq.s 	Exv0
	clr.w 	d2
	moveq 	#1,d3
	lea 	Zero(pc),a0
	bra.s 	Exv0a

Exv0:	clr.w 	d3
	move.w 	a0,d2
	addq.l 	#6,a0
	move.w 	a0,-(sp)
	pea	BuFloat(a5)
	move.l 	TempFl(a5),-(sp)
	bsr 	ffp2a
	lea 	10(sp),sp
	movem.l	(sp)+,a3-a6
	lea	BuFloat(a5),a0

Exv0a:	move.l 	TempBuf(a5),a1
	cmp.b 	#"-",(a0)
	bne.s 	Exv1
	move.b 	(a0)+,(a1)+
Exv1:	lea 	2(a0),a2
Exv2:	move.b 	(a0)+,d0			;Cherche le debut du chiffre
	beq.s 	Exv3
	cmp.b 	#".",d0
	beq.s	Exv2
	cmp.b 	#"0",d0
	beq.s 	Exv2
	bra.s 	Exv4
Exv3:	move.l 	a2,a0
	moveq 	#"0",d0
Exv4:	move.w 	d4,d1
	bpl.s 	Exv5
	moveq 	#6,d1
	bra.s 	Exv6
Exv5:	cmp.w 	#6,d1
	bcs.s 	Exv6
	moveq 	#6,d1
Exv6:	move.b	d0,(a1)+
	move.b 	#".",(a1)+
	lea 	-1(a1),a2
	tst.w 	d1
	beq.s 	Exv7b
Exv7:	move.b 	(a0)+,d0
	beq.s 	Exv7a
	move.b 	d0,(a1)+
	cmp.b 	#"0",d0
	beq.s 	Exv7a
	move.l 	a1,a2
Exv7a	subq.w 	#1,d1
	bne.s 	Exv7
Exv7b:	tst.w 	d4		;Nettoie le chiffre
	bpl.s 	Exv8
	move.l 	a2,a1
Exv8:	tst.w 	d3
	bne 	Exf5
	move.b 	#"E",(a1)+
	move.b 	#"-",(a1)+
	move.b 	#"0",(a1)+
	bra 	Exf6

;	CONVERSION FLOAT ---> ASCII
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ffp2a	LINK	A6,#-8
	MOVEM.L	D3-d7,-(A7)
	MOVE.L	$C(A6),-4(A6)
	TST.W	$10(A6)
	BGT.S	L27CDA
	MOVEQ	#1,D0
	BRA.S	L27CEC
L27CDA	CMPI.W	#$16,$10(A6)
	BLE.S	L27CE6
	MOVEQ	#$17,D0
	BRA.S	L27CEC
L27CE6	MOVE.W	$10(A6),D0
	ADDQ.W	#1,D0
L27CEC	MOVE.W	D0,D4
	CLR.W	D7
	CLR.L	-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BGE.S	L27D1C
	MOVEA.L	$C(A6),A0
	MOVE.B	#$2D,(A0)
	ADDQ.L	#1,$C(A6)
	MOVE.L	8(A6),-(A7)
	BSR	L283A8
	ADDQ.L	#4,A7
	MOVE.L	D0,8(A6)
L27D1C	CLR.L	-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BLE.S	L27D5A
	BRA.S	L27D46
L27D2E	MOVE.L	#$A0000044,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
	SUBQ.W	#1,D7
L27D46	MOVE.L	#$80000041,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BLT.S	L27D2E
L27D5A	BRA.S	L27D74
L27D5C	MOVE.L	#$A0000044,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28250
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
	ADDQ.W	#1,D7
L27D74	MOVE.L	#$A0000044,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BGE.S	L27D5C
	ADD.W	D7,D4
	MOVEQ	#1,D6
	MOVE.W	D6,D0
	EXT.L	D0
	MOVE.L	D0,-(A7)
	BSR	L28270
	ADDQ.L	#4,A7
	MOVE.L	D0,-8(A6)
	BRA.S	L27DB8
L27DA0	MOVE.L	#$A0000044,-(A7)
	MOVE.L	-8(A6),-(A7)
	BSR	L28250
	ADDQ.L	#8,A7
	MOVE.L	D0,-8(A6)
	ADDQ.W	#1,D6
L27DB8	CMP.W	D4,D6
	BLT.S	L27DA0
	MOVE.L	#$80000042,-(A7)
	MOVE.L	-8(A6),-(A7)
	BSR	L28250
	ADDQ.L	#8,A7
	MOVE.L	D0,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28212
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
	MOVE.L	#$A0000044,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BLT.S	L27DFE
	MOVE.L	#$80000041,8(A6)
	ADDQ.W	#1,D7
L27DFE	TST.W	D7
	BGE.S	L27E36
	MOVEA.L	$C(A6),A0
	MOVE.B	#$30,(A0)
	ADDQ.L	#1,$C(A6)
	MOVEA.L	$C(A6),A0
	MOVE.B	#$2E,(A0)
	ADDQ.L	#1,$C(A6)
	TST.W	D4
	BGE.S	L27E20
	SUB.W	D4,D7
L27E20	MOVEQ	#-1,D6
	BRA.S	L27E32
L27E24	MOVEA.L	$C(A6),A0
	MOVE.B	#$30,(A0)
	ADDQ.L	#1,$C(A6)
	SUBQ.W	#1,D6
L27E32	CMP.W	D7,D6
	BGT.S	L27E24
L27E36	CLR.W	D6
	BRA.S	L27EA4
L27E3A	MOVE.L	8(A6),-(A7)
	BSR	L28300
	ADDQ.L	#4,A7
	MOVE.W	D0,D5
	MOVE.W	D5,D0
	ADD.W	#$30,D0
	MOVEA.L	$C(A6),A1
	MOVE.B	D0,(A1)
	ADDQ.L	#1,$C(A6)
	CMP.W	D7,D6
	BNE.S	L27E68
	MOVEA.L	$C(A6),A0
	MOVE.B	#$2E,(A0)
	ADDQ.L	#1,$C(A6)
L27E68	MOVE.W	D5,D0
	EXT.L	D0
	MOVE.L	D0,-(A7)
	BSR	L28270
	ADDQ.L	#4,A7
	MOVE.L	D0,-8(A6)
	MOVE.L	D0,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L283C4
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
	MOVE.L	#$A0000044,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
	ADDQ.W	#1,D6
L27EA4	CMP.W	D4,D6
	BLT.S	L27E3A
	MOVEA.L	$C(A6),A0
	CLR.B	(A0)
	ADDQ.L	#1,$C(A6)
	MOVE.L	-4(A6),D0
	TST.L	(A7)+
	MOVEM.L	(A7)+,D4-D7
	UNLK	A6
	RTS

L28212	LINK	A6,#-4
	MOVEM.L	D3-D7,-(A7)
	MOVE.L	8(A6),D7
	MOVE.L	$C(A6),D6
	BSR	L28422
	MOVE.L	D7,D0
	MOVEM.L	(A7)+,D3-D7
	UNLK	A6
	RTS
L28232	LINK	A6,#-4
	MOVEM.L	D3-D7,-(A7)
	MOVE.L	8(A6),D7
	MOVE.L	$C(A6),D6
	BSR	L283E4
	MOVEM.L	(A7)+,D3-D7
	UNLK	A6
	RTS
L28250	LINK	A6,#-4
	MOVEM.L	D3-D7,-(A7)
	MOVE.L	8(A6),D7
	MOVE.L	$C(A6),D6
	BSR	L28518
	MOVE.L	D7,D0
	MOVEM.L	(A7)+,D3-D7
	UNLK	A6
	RTS
L28270	LINK	A6,#0
	MOVEM.L	D5-D7,-(A7)
	TST.L	8(A6)
	BGE.S	L2828C
	MOVEQ	#1,D6
	MOVE.L	8(A6),D0
	NEG.L	D0
	MOVE.L	D0,8(A6)
	BRA.S	L2828E
L2828C	CLR.W	D6
L2828E	TST.L	8(A6)
	BNE.S	L28298
	CLR.L	D0
	BRA.S	L282F6
L28298	MOVEQ	#$18,D7
	BRA.S	L282A8
L2829C	MOVE.L	8(A6),D0
	ASR.L	#1,D0
	MOVE.L	D0,8(A6)
	ADDQ.L	#1,D7
L282A8	MOVE.L	8(A6),D0
	AND.L	#$7F000000,D0
	BNE.S	L2829C
	BRA.S	L282C2
L282B6	MOVE.L	8(A6),D0
	ASL.L	#1,D0
	MOVE.L	D0,8(A6)
	SUBQ.L	#1,D7
L282C2	BTST	#7,9(A6)
	BEQ.S	L282B6
	MOVE.L	8(A6),D0
	ASL.L	#8,D0
	MOVE.L	D0,8(A6)
	ADD.L	#$40,D7
	MOVE.L	D7,D0
	AND.L	#$7F,D0
	OR.L	D0,8(A6)
	TST.W	D6
	BEQ.S	L282F2
	ORI.L	#$80,8(A6)
L282F2	MOVE.L	8(A6),D0
L282F6	TST.L	(A7)+
	MOVEM.L	(A7)+,D6-D7
	UNLK	A6
	RTS
L28300	LINK	A6,#0
	MOVEM.L	D4-D7,-(A7)
	MOVE.L	8(A6),D0
	AND.L	#$7F,D0
	ADD.L	#$FFFFFFC0,D0
	MOVE.W	D0,D6
	TST.L	8(A6)
	BEQ.S	L28324
	TST.W	D6
	BGE.S	L28328
L28324	CLR.L	D0
	BRA.S	L2837E
L28328	MOVE.L	8(A6),D0
	AND.L	#$80,D0
	MOVE.W	D0,D5
	CMP.W	#$1F,D6
	BLE.S	L2834E
	TST.W	D5
	BEQ.S	L28346
	MOVE.L	#$80000000,D0
	BRA.S	L2834C
L28346	MOVE.L	#$7FFFFFFF,D0
L2834C	BRA.S	L2837E
L2834E	MOVE.L	8(A6),D7
	ASR.L	#8,D7
	AND.L	#$FFFFFF,D7
	SUB.W	#$18,D6
	BRA.S	L28364
L28360	ASR.L	#1,D7
	ADDQ.W	#1,D6
L28364	TST.W	D6
	BLT.S	L28360
	BRA.S	L2836E
L2836A	ASL.L	#1,D7
	SUBQ.W	#1,D6
L2836E	TST.W	D6
	BGT.S	L2836A
	TST.W	D5
	BEQ.S	L2837C
	MOVE.L	D7,D0
	NEG.L	D0
	MOVE.L	D0,D7
L2837C	MOVE.L	D7,D0
L2837E	TST.L	(A7)+
	MOVEM.L	(A7)+,D5-D7
	UNLK	A6
	RTS
L28388	LINK	A6,#-4
	MOVEM.L	D3-D7,-(A7)
	MOVE.L	8(A6),D7
	MOVE.L	$C(A6),D6
	BSR	L2858A
	MOVE.L	D7,D0
	MOVEM.L	(A7)+,D3-D7
	UNLK	A6
	RTS
L283A8	LINK	A6,#-4
	MOVEM.L	D3-D7,-(A7)
	MOVE.L	8(A6),D7
	BSR	L28406
	MOVE.L	D7,D0
	MOVEM.L	(A7)+,D3-D7
	UNLK	A6
	RTS
L283C4	LINK	A6,#-4
	MOVEM.L	D3-D7,-(A7)
	MOVE.L	8(A6),D7
	MOVE.L	$C(A6),D6
	BSR	L28410
	MOVE.L	D7,D0
	MOVEM.L	(A7)+,D3-D7
	UNLK	A6
	RTS
L283E4	TST.B	D6
	BPL.S	L283F4
	TST.B	D7
	BPL.S	L283F4
	CMP.B	D7,D6
	BNE.S	L283FA
	CMP.L	D7,D6
	RTS
L283F4	CMP.B	D6,D7
	BNE.S	L283FA
	CMP.L	D6,D7
L283FA	RTS
	TST.B	D7
	RTS
L28406	TST.B	D7
	BEQ.S	L2840E
	EORI.B	#$80,D7
L2840E	RTS
L28410	MOVE.B	D6,D4
	BEQ.S	L28466
	EORI.B	#$80,D4
	BMI.S	L28484
	MOVE.B	D7,D5
	BMI.S	L2848A
	BNE.S	L2842E
	BRA.S	L28460
L28422	MOVE.B	D6,D4
	BMI.S	L28484
	BEQ.S	L28466
	MOVE.B	D7,D5
	BMI.S	L2848A
	BEQ.S	L28460
L2842E	SUB.B	D4,D5
	BMI.S	L2846A
	MOVE.B	D7,D4
	CMP.B	#$18,D5
	BCC.S	L28466
	MOVE.L	D6,D3
	CLR.B	D3
	LSR.L	D5,D3
	MOVE.B	#$80,D7
	ADD.L	D3,D7
	BCS.S	L2844C
L28448	MOVE.B	D4,D7
	RTS
L2844C	ROXR.L	#1,D7
	ADDQ.B	#1,D4
	BVS.S	L28454
	BCC.S	L28448
L28454	MOVEQ	#-1,D7
	SUBQ.B	#1,D4
	MOVE.B	D4,D7
	ORI.B	#2,CCR
	RTS
L28460	MOVE.L	D6,D7
	MOVE.B	D4,D7
	RTS
L28466	TST.B	D7
	RTS
L2846A	CMP.B	#$E8,D5
	BLE.S	L28460
	NEG.B	D5
	MOVE.L	D6,D3
	CLR.B	D7
	LSR.L	D5,D7
	MOVE.B	#$80,D3
	ADD.L	D3,D7
	BCS.S	L2844C
	MOVE.B	D4,D7
	RTS
L28484	MOVE.B	D7,D5
	BMI.S	L2842E
	BEQ.S	L28460
L2848A	MOVEQ	#-$80,D3
	EOR.B	D3,D5
	SUB.B	D4,D5
	BEQ.S	L284E2
	BMI.S	L284D0
	CMP.B	#$18,D5
	BCC.S	L28466
	MOVE.B	D7,D4
	MOVE.B	D3,D7
	MOVE.L	D6,D3
L284A0	CLR.B	D3
	LSR.L	D5,D3
	SUB.L	D3,D7
	BMI.S	L28448
L284A8	MOVE.B	D4,D5
L284AA	CLR.B	D7
	SUBQ.B	#1,D4
	CMP.L	#$7FFF,D7
	BHI.S	L284BC
	SWAP	D7
	SUB.B	#$10,D4
L284BC	ADD.L	D7,D7
	DBMI	D4,L284BC
	EOR.B	D4,D5
	BMI.S	L284CC
	MOVE.B	D4,D7
	BEQ.S	L284CC
	RTS
L284CC	MOVEQ	#0,D7
	RTS
L284D0	CMP.B	#$E8,D5
	BLE.S	L28460
	NEG.B	D5
	MOVE.L	D7,D3
	MOVE.L	D6,D7
	MOVE.B	#$80,D7
	BRA.S	L284A0
L284E2	MOVE.B	D7,D5
	EXG	D5,D4
	MOVE.B	D6,D7
	SUB.L	D6,D7
	BEQ.S	L284CC
	BPL.S	L284A8
	NEG.L	D7
	MOVE.B	D5,D4
	BRA.S	L284AA
L284F4	DIVU	#0,D7
	TST.L	D6
	BNE.S	L28518
L284FC	OR.L	#$FFFFFF7F,D7
	TST.B	D7
	ORI.B	#2,CCR
L28508	RTS
L2850A	SWAP	D6
	SWAP	D7
L2850E	EOR.B	D6,D7
	BRA.S	L284FC
L28512	BMI.S	L2850E
L28514	MOVEQ	#0,D7
	RTS
L28518	MOVE.B	D6,D5
	BEQ.S	L284F4
	MOVE.L	D7,D4
	BEQ.S	L28508
	MOVEQ	#-$80,D3
	ADD.W	D5,D5
	ADD.W	D4,D4
	EOR.B	D3,D5
	EOR.B	D3,D4
	SUB.B	D5,D4
	BVS.S	L28512
	CLR.B	D7
	SWAP	D7
	SWAP	D6
	CMP.W	D6,D7
	BMI.S	L2853E
	ADDQ.B	#2,D4
	BVS.S	L2850A
	ROR.L	#1,D7
L2853E	SWAP	D7
	MOVE.B	D3,D5
	EOR.W	D5,D4
	LSR.W	#1,D4
	MOVE.L	D7,D3
	DIVU	D6,D3
	MOVE.W	D3,D5
	MULU	D6,D3
	SUB.L	D3,D7
	SWAP	D7
	SWAP	D6
	MOVE.W	D6,D3
	CLR.B	D3
	MULU	D5,D3
	SUB.L	D3,D7
	BCC.S	L28566
	MOVE.L	D6,D3
	CLR.B	D3
	ADD.L	D3,D7
	SUBQ.W	#1,D5
L28566	MOVE.L	D6,D3
	SWAP	D3
	CLR.W	D7
	DIVU	D3,D7
	SWAP	D5
	BMI.S	L2857A
	MOVE.W	D7,D5
	ADD.L	D5,D5
	SUBQ.B	#1,D4
	MOVE.W	D5,D7
L2857A	MOVE.W	D7,D5
	ADD.L	#$80,D5
	MOVE.L	D5,D7
	MOVE.B	D4,D7
	BEQ.S	L28514
	RTS
L2858A	MOVE.B	D7,D5
	BEQ.S	L285E0
	MOVE.B	D6,D4
	BEQ.S	L285FA
	ADD.W	D5,D5
	ADD.W	D4,D4
	MOVEQ	#-$80,D3
	EOR.B	D3,D4
	EOR.B	D3,D5
	ADD.B	D4,D5
	BVS.S	L285FE
	MOVE.B	D3,D4
	EOR.W	D4,D5
	ROR.W	#1,D5
	SWAP	D5
	MOVE.W	D6,D5
	CLR.B	D7
	CLR.B	D5
	MOVE.W	D5,D4
	MULU	D7,D4
	SWAP	D4
	MOVE.L	D7,D3
	SWAP	D3
	MULU	D5,D3
	ADD.L	D3,D4
	SWAP	D6
	MOVE.L	D6,D3
	MULU	D7,D3
	ADD.L	D3,D4
	CLR.W	D4
	ADDX.B	D4,D4
	SWAP	D4
	SWAP	D7
	MULU	D6,D7
	SWAP	D6
	SWAP	D5
	ADD.L	D4,D7
	BPL.S	L285E2
	ADD.L	#$80,D7
	MOVE.B	D5,D7
	BEQ.S	L285FA
L285E0	RTS
L285E2	SUBQ.B	#1,D5
	BVS.S	L285FA
	BCS.S	L285FA
	MOVEQ	#$40,D4
	ADD.L	D4,D7
	ADD.L	D7,D7
	BCC.S	L285F4
	ROXR.L	#1,D7
	ADDQ.B	#1,D5
L285F4	MOVE.B	D5,D7
	BEQ.S	L285FA
	RTS
L285FA	MOVEQ	#0,D7
	RTS
L285FE	BPL.S	L285FA
	EOR.B	D6,D7
	OR.L	#$FFFFFF7F,D7
	TST.B	D7
	ORI.B	#2,CCR
	RTS
;	Conversion ASCII vers FFP
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
a2ffp	LINK	A6,#-$2E
	MOVEM.L	D7/A4-A5,-(A7)
	LEA	-$14(A6),A5
	LEA	-$18(A6),A4
	CLR.W	-$1E(A6)
	CLR.W	-$26(A6)
	BRA.S	L27EDE
L27EDA	ADDQ.L	#1,8(A6)
L27EDE	MOVEA.L	8(A6),A0
	CMPI.B	#$20,(A0)
	BEQ.S	L27EDA
	MOVEA.L	8(A6),A0
	CMPI.B	#9,(A0)
	BEQ.S	L27EDA
	MOVEA.L	8(A6),A0
	CMPI.B	#$2D,(A0)
	BEQ.S	L27F00
	CLR.W	D0
	BRA.S	L27F02
L27F00	MOVEQ	#1,D0
L27F02	MOVE.W	D0,-$22(A6)
	MOVEA.L	8(A6),A0
	CMPI.B	#$2D,(A0)
	BEQ.S	L27F1A
	MOVEA.L	8(A6),A0
	CMPI.B	#$2B,(A0)
	BNE.S	L27F1E
L27F1A	ADDQ.L	#1,8(A6)
L27F1E	BRA.S	L27F44
L27F20	MOVEA.L	8(A6),A0
	CMPI.B	#$2E,(A0)
	BNE.S	L27F30
	ADDQ.W	#1,-$1E(A6)
	BRA.S	L27F40
L27F30	MOVEA.L	8(A6),A0
	MOVE.B	(A0),(A5)+
	TST.W	-$1E(A6)
	BEQ.S	L27F40
	ADDQ.W	#1,-$26(A6)
L27F40	ADDQ.L	#1,8(A6)
L27F44	MOVEA.L	8(A6),A0
	TST.B	(A0)
	BEQ.S	L27F60
	MOVEA.L	8(A6),A0
	CMPI.B	#$65,(A0)
	BEQ.S	L27F60
	MOVEA.L	8(A6),A0
	CMPI.B	#$45,(A0)
	BNE.S	L27F20
L27F60	CLR.B	(A5)
	MOVEA.L	8(A6),A0
	CMPI.B	#$65,(A0)
	BEQ.S	L27F76
	MOVEA.L	8(A6),A0
	CMPI.B	#$45,(A0)
	BNE.S	L27FBA
L27F76	ADDQ.L	#1,8(A6)
	MOVEA.L	8(A6),A0
	CMPI.B	#$2D,(A0)
	BEQ.S	L27F88
	CLR.W	D0
	BRA.S	L27F8A
L27F88	MOVEQ	#1,D0
L27F8A	MOVE.W	D0,-$20(A6)
	MOVEA.L	8(A6),A0
	CMPI.B	#$2D,(A0)
	BEQ.S	L27FA2
	MOVEA.L	8(A6),A0
	CMPI.B	#$2B,(A0)
	BNE.S	L27FA6
L27FA2	ADDQ.L	#1,8(A6)
L27FA6	BRA.S	L27FB2
L27FA8	MOVEA.L	8(A6),A0
	MOVE.B	(A0),(A4)+
	ADDQ.L	#1,8(A6)
L27FB2	MOVEA.L	8(A6),A0
	TST.B	(A0)
	BNE.S	L27FA8
L27FBA	CLR.B	(A4)
	MOVE.L	A6,(A7)
	ADDI.L	#$FFFFFFEC,(A7)
	BSR	L280A8
	MOVE.L	D0,-$2A(A6)
	MOVE.L	A6,(A7)
	ADDI.L	#$FFFFFFE8,(A7)
	BSR	L28654
	MOVE.W	D0,-$24(A6)
	TST.W	-$20(A6)
	BEQ.S	L27FF0
	MOVE.W	-$24(A6),D0
	NEG.W	D0
	SUB.W	-$26(A6),D0
	BRA.S	L27FF8
L27FF0	MOVE.W	-$24(A6),D0
	SUB.W	-$26(A6),D0
L27FF8	MOVE.W	D0,-$26(A6)
	MOVE.L	-$2A(A6),-(A7)
	MOVE.W	-$26(A6),-(A7)
	BSR.S	L28040
	ADDQ.L	#2,A7
	MOVE.L	D0,-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,-$2E(A6)
	MOVE.L	-$2E(A6),(A7)
	BSR	L28116
	MOVE.L	D0,-$1C(A6)
	TST.W	-$22(A6)
	BEQ.S	L28032
	ORI.L	#$80,-$1C(A6)
L28032	MOVE.L	-$1C(A6),D0
	TST.L	(A7)+
	MOVEM.L	(A7)+,A4-A5
	UNLK	A6
	RTS
L28040	LINK	A6,#-8
	TST.W	8(A6)
	BGE.S	L28076
	MOVE.L	#$80000041,-4(A6)
	BRA.S	L2806E
L28054	MOVE.L	#$A0000044,-(A7)
	MOVE.L	-4(A6),-(A7)
	BSR	L28250
	ADDQ.L	#8,A7
	MOVE.L	D0,-4(A6)
	ADDQ.W	#1,8(A6)
L2806E	TST.W	8(A6)
	BLT.S	L28054
	BRA.S	L280A0
L28076	MOVE.L	#$80000041,-4(A6)
	BRA.S	L2809A
L28080	MOVE.L	#$A0000044,-(A7)
	MOVE.L	-4(A6),-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,-4(A6)
	SUBQ.W	#1,8(A6)
L2809A	TST.W	8(A6)
	BGT.S	L28080
L280A0	MOVE.L	-4(A6),D0
	UNLK	A6
	RTS
L280A8	LINK	A6,#-8
	MOVE.L	#0,-4(A6)
	BRA.S	L280FA
L280B6	MOVE.L	#$A0000044,-(A7)
	MOVE.L	-4(A6),-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,-4(A6)
	MOVE.L	-4(A6),-(A7)
	MOVEA.L	8(A6),A0
	MOVE.B	(A0),D0
	EXT.W	D0
	ADD.W	#$FFD0,D0
	EXT.L	D0
	MOVE.L	D0,-(A7)
	BSR	L28270
	ADDQ.L	#4,A7
	MOVE.L	D0,-(A7)
	BSR	L28212
	ADDQ.L	#8,A7
	MOVE.L	D0,-4(A6)
	ADDQ.L	#1,8(A6)
L280FA	MOVEA.L	8(A6),A0
	CMPI.B	#$30,(A0)
	BLT.S	L2810E
	MOVEA.L	8(A6),A0
	CMPI.B	#$39,(A0)
	BLE.S	L280B6
L2810E	MOVE.L	-4(A6),D0
	UNLK	A6
	RTS
L28116	LINK	A6,#-4
	MOVEM.L	D4-D7,-(A7)
	CLR.L	-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BNE.S	L28134
	CLR.L	D0
	BRA	L28208
L28134	CLR.L	-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BGE.S	L28158
	MOVE.L	8(A6),-(A7)
	BSR	L283A8
	ADDQ.L	#4,A7
	MOVE.L	D0,8(A6)
	MOVEQ	#1,D5
	BRA.S	L2815A
L28158	CLR.W	D5
L2815A	CLR.W	D7
	BRA.S	L28176
L2815E	ADDQ.W	#1,D7
	MOVE.L	#$80000042,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28250
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
L28176	MOVE.L	#$80000041,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BGE.S	L2815E
	BRA.S	L281A4
L2818C	SUBQ.W	#1,D7
	MOVE.L	#$80000042,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
L281A4	MOVE.L	#$80000040,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28232
	ADDQ.L	#8,A7
	BLT.S	L2818C
	MOVE.L	#$80000059,-(A7)
	MOVE.L	8(A6),-(A7)
	BSR	L28388
	ADDQ.L	#8,A7
	MOVE.L	D0,8(A6)
	MOVE.L	8(A6),-(A7)
	BSR	L28300
	ADDQ.L	#4,A7
	MOVE.L	D0,-4(A6)
	MOVE.L	-4(A6),D0
	ASL.L	#8,D0
	MOVE.L	D0,-4(A6)
	ADD.W	#$40,D7
	MOVE.W	D7,D0
	AND.W	#$7F,D0
	EXT.L	D0
	OR.L	D0,-4(A6)
	TST.W	D5
	BEQ.S	L28204
	ORI.L	#$80,-4(A6)
L28204	MOVE.L	-4(A6),D0
L28208	TST.L	(A7)+
	MOVEM.L	(A7)+,D5-D7
	UNLK	A6
	RTS

L28654	LINK	A6,#0
	MOVEM.L	D5-D7/A5,-(A7)
	MOVEA.L	8(A6),A5
	CLR.W	D7
	CLR.W	D6
	BRA.S	L28668
L28666	ADDQ.L	#1,A5
L28668	MOVE.B	(A5),D0
	EXT.W	D0
	EXT.L	D0
	ADD.L	#0,D0
	MOVEA.L	D0,A0
;	BTST	#5,(A0)
;	BNE.S	L28666
	CMPI.B	#$2B,(A5)
	BNE.S	L28686
	ADDQ.L	#1,A5
	BRA.S	L28690
L28686	CMPI.B	#$2D,(A5)
	BNE.S	L28690
	ADDQ.L	#1,A5
	ADDQ.W	#1,D6
L28690	BRA.S	L286A0
L28692	MULS	#$A,D7
	MOVE.B	(A5)+,D0
	EXT.W	D0
L2869A	ADD.W	D0,D7
L2869C	ADD.W	#$FFD0,D7
L286A0	CMPI.B	#$30,(A5)
L286A4	BLT.S	L286AC
	CMPI.B	#$39,(A5)
	BLE.S	L28692
L286AC	TST.W	D6
	BEQ.S	L286B6
	MOVE.W	D7,D0
	NEG.W	D0
	MOVE.W	D0,D7
L286B6	MOVE.W	D7,D0
	TST.L	(A7)+
	MOVEM.L	(A7)+,D6-D7/A5
	UNLK	A6
	RTS


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ASCII >>> DOUBLE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	AscToDouble
; - - - - - - - - - - - - -
	CLR.L	-(A7)
	MOVE.L	8(A7),-(A7)
	Rlea	L_DoubleToAsc,0
	jsr	L3D9C0-Dtoa(a0)
	ADDQ.W	#8,A7
	RTS

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DOUBLE>>>ASCII>>>DOUBLE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	DoubleToAsc
; - - - - - - - - - - - - -
Dtoa	LINK	A5,#-4
	MOVEM.L	D0-7/A1-4/A6,-(A7)

	lea	DDebut(pc),a4
	lea	$7FFE(a4),a4
	bset	#5,$1b(a5)

	MOVEA.L	$10(A5),A2
	MOVE.L	$14(A5),D6
	MOVE.W	8(A5),D0
	EXT.L	D0
	ANDI.L	#$7FF0,D0
	CMPI.L	#$7FF0,D0
	BNE	L3D738
	MOVE.W	8(A5),D0
	ANDI.W	#$8000,D0
	BEQ.S	L3D71E
	MOVEQ	#$2D,D0
	BRA.S	L3D720
L3D71E	MOVEQ	#$2B,D0
L3D720	MOVE.L	D0,D4
L3D722	MOVE.L	D6,D0
	SUBQ.L	#1,D6
	TST.L	D0
	BEQ.S	L3D72E
	MOVE.B	D4,(A2)+
	BRA.S	L3D722
L3D72E	CLR.B	(A2)

L3D730	move.l	a2,a0
	bclr	#5,$1b(a5)
	cmp.b	#2,$1b(a5)
	bne.s	.Exit
; Si mode 2, enleve les "0" avant l'exposant!
	moveq	#0,d1
	moveq	#0,d2
	move.l	$10(a5),a0
.Fnd	move.b	(a0)+,d0
	beq.s	.Fin
	cmp.b	#"e",d0
	beq.s	.Fin
	cmp.b	#"E",d0
	beq.s	.Fin
	cmp.b	#".",d0
	beq.s	.Pnt
	cmp.b	#"0",d0
	beq.s	.Fnd
	tst.l	d2
	beq.s	.Fnd
	move.l	a0,d1
	bra.s	.Fnd
.Pnt	move.l	a0,d2
	bra.s	.Fnd
; Fin du chiffre, enlever les 0 en trop
.Fin	tst.l	d2		Une virgule?
	beq.s	.End
	lea	-1(a0),a1
	move.l	d2,a0
	subq.l	#1,a0
	tst.l	d1
	beq.s	.Nul
	move.l	d1,a0
.Nul	move.b	(a1)+,(a0)+
	bne.s	.Nul
.End	subq.l	#1,a0		A0 pointe la fin
; Depile!
.Exit	MOVEM.L	(A7)+,D0-7/A1-4/A6
	UNLK	A5
	RTS

L3D738	MOVEQ	#0,D4
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DE1C(PC)
	BGE.S	L3D760
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DE4E(PC)
	MOVE.L	D1,$C(A5)
	MOVE.L	D0,8(A5)
	MOVE.B	#$2D,(A2)+
L3D760	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DE1C(PC)
	BLE.S	L3D7DA
L3D76E	MOVE.L	-$7FF2(A4),D3
	MOVE.L	-$7FF6(A4),D2
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DDC6(PC)
	BGE.S	L3D7A4
	MOVE.L	-$7FFA(A4),D3
	MOVE.L	-$7FFE(A4),D2
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DFA0(PC)
	MOVE.L	D1,$C(A5)
	MOVE.L	D0,8(A5)
	SUBQ.L	#1,D4
	BRA.S	L3D76E
L3D7A4	MOVE.L	-$7FFA(A4),D3
	MOVE.L	-$7FFE(A4),D2
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DDC6(PC)
	BLT.S	L3D7DA
	MOVE.L	-$7FFA(A4),D3
	MOVE.L	-$7FFE(A4),D2
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3E12A(PC)
	MOVE.L	D1,$C(A5)
	MOVE.L	D0,8(A5)
	ADDQ.L	#1,D4
	BRA.S	L3D7A4
L3D7DA	MOVE.L	$18(A5),D7
	ANDI.L	#3,D7
	CMPI.L	#2,D7
	BNE.S	L3D804
	TST.L	D6
	BNE.S	L3D7F2
	MOVEQ	#1,D6
L3D7F2	CMPI.L	#$FFFFFFFC,D4
	BLT.S	L3D7FE
	CMP.L	D6,D4
	BLT.S	L3D800
L3D7FE	MOVEQ	#-1,D7
L3D800	MOVE.L	D6,D5
	BRA.S	L3D81A
L3D804	CMPI.L	#1,D7
	BNE.S	L3D816
	MOVE.L	D6,D0
	ADD.L	D4,D0
	MOVE.L	D0,D5
	ADDQ.L	#1,D5
	BRA.S	L3D81A
L3D816	MOVE.L	D6,D5
	ADDQ.L	#1,D5
L3D81A	TST.L	D5
	BLE.S	L3D872
	CMPI.L	#$10,D5
	BLE.S	L3D82A
	MOVEQ	#$10,D0
	BRA.S	L3D82C
L3D82A	MOVE.L	D5,D0
L3D82C	ASL.L	#3,D0
	LEA	-$7FF6(A4),A0
	MOVEA.L	D0,A1
	ADDA.L	A0,A1
	MOVE.L	4(A1),D1
	MOVE.L	(A1),D0
	MOVE.L	$C(A5),D3
	MOVE.L	8(A5),D2
	JSR	L3DE80(PC)
	MOVE.L	D1,$C(A5)
	MOVE.L	D0,8(A5)
	MOVE.L	-$7FFA(A4),D3
	MOVE.L	-$7FFE(A4),D2
	JSR	L3DDC6(PC)
	BLT.S	L3D872
	MOVE.L	-$7FF2(A4),$C(A5)
	MOVE.L	-$7FF6(A4),8(A5)
	ADDQ.L	#1,D4
	TST.L	D7
	BLE.S	L3D872
	ADDQ.L	#1,D5
L3D872	TST.L	D7
	BLE.S	L3D8A8
	TST.L	D4
	BGE.S	L3D8A2
	MOVE.B	#$30,(A2)+
	MOVE.B	#$2E,(A2)+
	MOVE.L	D4,D0
	NEG.L	D0
	MOVEA.L	D0,A3
	SUBQ.L	#1,A3
	TST.L	D5
	BGT.S	L3D890
	MOVEA.L	D6,A3
L3D890	MOVE.L	A3,D0
	SUBQ.L	#1,A3
	TST.L	D0
	BEQ.S	L3D89E
	MOVE.B	#$30,(A2)+
	BRA.S	L3D890
L3D89E	SUBA.L	A6,A6
	BRA.S	L3D8A6
L3D8A2	MOVEA.L	D4,A6
	ADDQ.L	#1,A6
L3D8A6	BRA.S	L3D8AE
L3D8A8	MOVEA.L	#1,A6
L3D8AE	TST.L	D5
	BLE.S	L3D920
	SUBA.L	A3,A3
L3D8B4	CMPA.L	#$10,A3
	BGE.S	L3D906
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DD4A(PC)
	MOVE.L	D0,-4(A5)
	MOVE.L	-4(A5),D0
	ADDI.L	#$30,D0
	MOVE.B	D0,(A2)+
	MOVE.L	-4(A5),D0
	JSR	L3DDA6(PC)
	MOVE.L	D1,D3
	MOVE.L	D0,D2
	MOVE.L	$C(A5),D1
	MOVE.L	8(A5),D0
	JSR	L3DE7A(PC)
	MOVE.L	-$7FFA(A4),D3
	MOVE.L	-$7FFE(A4),D2
	JSR	L3DFA0(PC)
	MOVE.L	D1,$C(A5)
	MOVE.L	D0,8(A5)
	BRA.S	L3D90A
L3D906	MOVE.B	#$30,(A2)+
L3D90A	SUBQ.L	#1,D5
	BEQ.S	L3D920
	MOVE.L	A6,D0
	BEQ.S	L3D91C
	SUBQ.L	#1,A6
	MOVE.L	A6,D0
	BNE.S	L3D91C
	MOVE.B	#$2E,(A2)+
L3D91C	ADDQ.L	#1,A3
	BRA.S	L3D8B4
L3D920	MOVE.L	A6,D0
	BEQ.S	L3D930
	BTST	#5,$1B(A5)
	BEQ.S	L3D930
	MOVE.B	#$2E,(A2)+
L3D930	TST.L	D7
	BGT.S	L3D98E
	BTST	#4,$1B(A5)
	BEQ.S	L3D940
	MOVEQ	#$45,D0
	BRA.S	L3D942
L3D940	MOVEQ	#$65,D0
L3D942	MOVE.B	D0,(A2)+
	TST.L	D4
	BGE.S	L3D950
	NEG.L	D4
	MOVE.B	#$2D,(A2)+
	BRA.S	L3D954
L3D950	MOVE.B	#$2B,(A2)+
L3D954	MOVEQ	#$64,D1
	MOVE.L	D4,D0
	JSR	L3E854(PC)
	ADDI.L	#$30,D0
	MOVE.B	D0,(A2)+
	MOVEQ	#$64,D1
	MOVE.L	D4,D0
	JSR	L3E87C(PC)
	MOVE.L	D0,D4
	MOVEQ	#$A,D1
	MOVE.L	D4,D0
	JSR	L3E854(PC)
	ADDI.L	#$30,D0
	MOVE.B	D0,(A2)+
	MOVEQ	#$A,D1
	MOVE.L	D4,D0
	JSR	L3E87C(PC)
	ADDI.L	#$30,D0
	MOVE.B	D0,(A2)+
L3D98E	MOVE.L	A6,D0
	BNE.S	L3D9BA
	CMPI.L	#2,D7
	BEQ.S	L3D9A2
	CMPI.L	#$FFFFFFFF,D7
	BNE.S	L3D9BA
L3D9A2	BTST	#5,$1B(A5)
	BNE.S	L3D9BA
L3D9AA	CMPI.B	#$30,-(A2)
	BNE.S	L3D9B2
	BRA.S	L3D9AA
L3D9B2	CMPI.B	#$2E,(A2)
	BEQ.S	L3D9BA
	ADDQ.L	#1,A2
L3D9BA	CLR.B	(A2)
	BRA	L3D730
L3D9C0	LINK	A5,#-$10
	MOVEM.L	D2-7/A2-4/A6,-(A7)
	lea	DDebut(pc),a4
	lea	$7FFE(a4),a4
	MOVE.L	$C(A5),D7
	MOVEQ	#0,D6
	MOVEA.L	8(A5),A6
	BRA.S	L3D9D6
L3D9D4	ADDQ.L	#1,A6
L3D9D6	MOVE.B	(A6),D0
	EXT.W	D0
	LEA	-$7F6D(A4),A0
	MOVE.B	0(A0,D0.W),D0
	EXT.W	D0
	BTST	#4,D0
	BNE.S	L3D9D4
	TST.B	(A6)
	BNE.S	L3DA04
L3D9EE	TST.L	D7
	BEQ.S	L3D9F8
	MOVEA.L	D7,A0
	MOVE.L	8(A5),(A0)
L3D9F8	MOVEQ	#0,D1
	MOVEQ	#0,D0
L3D9FC	MOVEM.L	(A7)+,D2-7/A2-4/A6
	UNLK	A5
	RTS
L3DA04	MOVE.B	(A6),D0
	EXT.W	D0
	EXT.L	D0
	BRA.S	L3DA32
L3DA0C	MOVEQ	#1,D6
L3DA0E	ADDQ.L	#1,A6
	BRA.S	L3DA3E
L3DA12	MOVE.B	(A6),D0
	EXT.W	D0
	LEA	-$7F6D(A4),A0
	MOVE.B	0(A0,D0.W),D0
	EXT.W	D0
	BTST	#2,D0
	BNE.S	L3DA30
	MOVE.B	(A6),D0
	EXT.W	D0
	CMPI.W	#$2E,D0
	BNE.S	L3D9EE
L3DA30	BRA.S	L3DA3E
L3DA32	SUBI.W	#$2B,D0
	BEQ.S	L3DA0E
	SUBQ.W	#2,D0
	BEQ.S	L3DA0C
	BRA.S	L3DA12
L3DA3E	MOVE.L	#0,-4(A5)
	MOVE.L	#0,-8(A5)
	MOVEA.L	A6,A2
	BRA.S	L3DA54
L3DA52	ADDQ.L	#1,A2
L3DA54	MOVE.B	(A2),D0
	EXT.W	D0
	LEA	-$7F6D(A4),A0
	MOVE.B	0(A0,D0.W),D0
	EXT.W	D0
	BTST	#2,D0
	BNE.S	L3DA52
	MOVEA.L	A2,A3
	MOVE.L	#0,-4(A5)
	MOVE.L	#0,-8(A5)
	SUBQ.L	#1,A2
	BRA	L3DB14
L3DA80	MOVE.L	-4(A5),-$C(A5)
	MOVE.L	-8(A5),-$10(A5)
	MOVE.L	A3,D0
	SUB.L	A2,D0
	SUBQ.L	#1,D0
	MOVE.L	D0,-(A7)
	JSR	L3DCFA(PC)
	MOVE.B	(A2),D2
	EXT.W	D2
	EXT.L	D2
	SUBI.L	#$30,D2
	ADDQ.W	#4,A7
	MOVE.L	D1,-(A7)
	MOVE.L	D0,-(A7)
	MOVE.L	D2,D0
	JSR	L3DDA6(PC)
	MOVE.L	D1,D3
	MOVE.L	D0,D2
	MOVE.L	(A7)+,D0
	MOVE.L	(A7)+,D1
	JSR	L3DFA0(PC)
	MOVE.L	-4(A5),D3
	MOVE.L	-8(A5),D2
	JSR	L3DE80(PC)
	MOVE.L	D1,-4(A5)
	MOVE.L	D0,-8(A5)
	MOVE.L	-$C(A5),D3
	MOVE.L	-$10(A5),D2
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	JSR	L3DDC6(PC)
	BGE.S	L3DB12
L3DAE6	MOVE.L	#$D,-$7E62(A4)
	TST.B	D6
	BEQ.S	L3DB02
	MOVE.L	#$8D2C7175,D1
	MOVE.L	#$C3A99999,D0
	BRA	L3D9FC
L3DB02	MOVE.L	#$8D2C7175,D1
	MOVE.L	#$43A99999,D0
	BRA	L3D9FC
L3DB12	SUBQ.L	#1,A2
L3DB14	CMPA.L	A6,A2
	BCC	L3DA80
	MOVE.B	(A3),D0
	EXT.W	D0
	CMPI.W	#$2E,D0
	BNE.S	L3DB84
	MOVEA.L	A3,A2
	ADDQ.L	#1,A2
	BRA.S	L3DB6C
L3DB2A	MOVE.B	(A2),D0
	EXT.W	D0
	EXT.L	D0
	SUBI.L	#$30,D0
	JSR	L3DDA6(PC)
	MOVE.L	D1,-(A7)
	MOVE.L	D0,-(A7)
	MOVE.L	A2,D0
	SUB.L	A3,D0
	MOVE.L	D0,-(A7)
	JSR	L3DCFA(PC)
	MOVE.L	D1,D3
	MOVE.L	D0,D2
	ADDQ.W	#4,A7
	MOVE.L	(A7)+,D0
	MOVE.L	(A7)+,D1
	JSR	L3E12A(PC)
	MOVE.L	-4(A5),D3
	MOVE.L	-8(A5),D2
	JSR	L3DE80(PC)
	MOVE.L	D1,-4(A5)
	MOVE.L	D0,-8(A5)
	ADDQ.L	#1,A2
L3DB6C	MOVE.B	(A2),D0
	EXT.W	D0
	LEA	-$7F6D(A4),A0
	MOVE.B	0(A0,D0.W),D0
	EXT.W	D0
	BTST	#2,D0
	BNE.S	L3DB2A
	MOVEA.L	A2,A3
	BRA.S	L3DB86
L3DB84	MOVEA.L	A3,A2
L3DB86	MOVE.B	(A2),D0
	EXT.W	D0
	CMPI.W	#$65,D0
	BEQ.S	L3DB9C
	MOVE.B	(A2),D0
	EXT.W	D0
	CMPI.W	#$45,D0
	BNE	L3DCCE
L3DB9C	MOVE.B	1(A2),D0
	EXT.W	D0
	CMPI.W	#$2D,D0
	BEQ.S	L3DBB4
	MOVE.B	1(A2),D0
	EXT.W	D0
	CMPI.W	#$2B,D0
	BNE.S	L3DBB6
L3DBB4	ADDQ.L	#1,A2
L3DBB6	MOVEA.L	A2,A3
	ADDQ.L	#1,A3
	MOVE.B	(A3),D0
	EXT.W	D0
	LEA	-$7F6D(A4),A0
	MOVE.B	0(A0,D0.W),D0
	EXT.W	D0
	BTST	#2,D0
	BEQ	L3DCCE
	MOVEQ	#0,D4
L3DBD2	MOVE.B	(A3),D0
	EXT.W	D0
	LEA	-$7F6D(A4),A0
	MOVE.B	0(A0,D0.W),D0
	EXT.W	D0
	BTST	#2,D0
	BEQ.S	L3DC34
	MOVE.L	D4,D5
	MOVE.L	D4,D0
	MOVE.L	D0,D1
	LSL.L	#2,D0
	ADD.L	D1,D0
	LSL.L	#1,D0
	MOVE.B	(A3),D1
	EXT.W	D1
	EXT.L	D1
	ADD.L	D1,D0
	MOVE.L	D0,D4
	SUBI.L	#$30,D4
	CMP.L	D5,D4
	BGE.S	L3DC30
	MOVE.B	(A2),D0
	EXT.W	D0
	CMPI.W	#$2D,D0
	BNE.S	L3DC2C
	MOVE.L	#0,-4(A5)
	MOVE.L	#0,-8(A5)
	MOVEQ	#0,D4
	MOVE.L	#$D,-$7E62(A4)
	BRA.S	L3DC34
L3DC2C	BRA	L3DAE6
L3DC30	ADDQ.L	#1,A3
	BRA.S	L3DBD2
L3DC34	MOVE.B	(A2),D0
	EXT.W	D0
	CMPI.W	#$2D,D0
	BNE.S	L3DC90
	MOVE.L	-4(A5),-$C(A5)
	MOVE.L	-8(A5),-$10(A5)
	MOVE.L	D4,-(A7)
	JSR	L3DCFA(PC)
	MOVE.L	D1,D3
	MOVE.L	D0,D2
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	JSR	L3E12A(PC)
	MOVE.L	D1,-4(A5)
	MOVE.L	D0,-8(A5)
	MOVE.L	-$C(A5),D1
	MOVE.L	-$10(A5),D0
	JSR	L3DE1C(PC)
	ADDQ.W	#4,A7
	BEQ.S	L3DC8E
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	JSR	L3DE1C(PC)
	BNE.S	L3DC8E
	MOVE.L	#$D,-$7E62(A4)
L3DC8E	BRA.S	L3DCCE
L3DC90	MOVE.L	-4(A5),-$C(A5)
	MOVE.L	-8(A5),-$10(A5)
	MOVE.L	D4,-(A7)
	BSR.S	L3DCFA
	MOVE.L	-4(A5),D3
	MOVE.L	-8(A5),D2
	JSR	L3DFA0(PC)
	MOVE.L	D1,-4(A5)
	MOVE.L	D0,-8(A5)
	MOVE.L	-$C(A5),D3
	MOVE.L	-$10(A5),D2
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	JSR	L3DDC6(PC)
	ADDQ.W	#4,A7
	BLT	L3DAE6
L3DCCE	TST.L	D7
	BEQ.S	L3DCD6
	MOVEA.L	D7,A0
	MOVE.L	A3,(A0)
L3DCD6	TST.B	D6
	BEQ.S	L3DCEE
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	JSR	L3DE4E(PC)
	MOVE.L	D1,-4(A5)
	MOVE.L	D0,-8(A5)
L3DCEE	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	BRA	L3D9FC
L3DCFA	LINK	A5,#-8
	MOVEM.L	D2-4,-(A7)
	MOVE.L	#0,-4(A5)
	MOVE.L	#$3FF00000,-8(A5)
	MOVEQ	#1,D4
	BRA.S	L3DD34
L3DD16	MOVEQ	#0,D3
	MOVE.L	#$40240000,D2
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	JSR	L3DFA0(PC)
	MOVE.L	D1,-4(A5)
	MOVE.L	D0,-8(A5)
	ADDQ.L	#1,D4
L3DD34	CMP.L	8(A5),D4
	BLE.S	L3DD16
	MOVE.L	-4(A5),D1
	MOVE.L	-8(A5),D0
	MOVEM.L	(A7)+,D2-4
	UNLK	A5
	RTS
L3DD4A	MOVEM.L	D1-2,-(A7)
	MOVE.L	D0,D2
	SMI	D1
	SWAP	D2
	LSR.W	#4,D2
	ANDI.W	#$7FF,D2
	SUBI.W	#$3FF,D2
	BMI.S	L3DD90
	SUBI.W	#$1F,D2
	BGT.S	L3DD94
	NEG.W	D2
	LSL.L	#8,D0
	LSL.L	#3,D0
	ORI.L	#$80000000,D0
	EXT.W	D1
	SWAP	D1
	LSR.W	#5,D1
	ANDI.W	#$7FF,D1
	OR.W	D1,D0
	LSR.L	D2,D0
	MOVE.L	D1,D1
	BMI.S	L3DD8C
	TST.L	D0
L3DD86	MOVEM.L	(A7)+,D1-2
	RTS
L3DD8C	NEG.L	D0
	BRA.S	L3DD86
L3DD90	MOVEQ	#0,D0
	BRA.S	L3DD86
L3DD94	MOVE.L	#$7FFFFFFF,D0
	MOVE.L	D1,D1
	BPL.S	L3DDA0
	NEG.L	D0
L3DDA0	ORI.B	#2,CCR
	BRA.S	L3DD86
L3DDA6	TST.L	D0
	BEQ.S	L3DDC2
	MOVEM.L	D4-7,-(A7)
	SMI	D6
	BGT.S	L3DDB4
	NEG.L	D0
L3DDB4	MOVEQ	#0,D1
	MOVEQ	#$1F,D4
	JSR	L3E26A(PC)
	MOVEM.L	(A7)+,D4-7
	RTS
L3DDC2	MOVEQ	#0,D1
	RTS
L3DDC6	MOVEM.L	D1-4,-(A7)
	MOVE.L	D0,D4
	SWAP	D4
	ANDI.W	#$7FF0,D4
	BNE.S	L3DDD8
	MOVEQ	#0,D0
	MOVEQ	#0,D1
L3DDD8	MOVE.L	D2,D4
	SWAP	D4
	ANDI.W	#$7FF0,D4
	BNE.S	L3DDE6
	MOVEQ	#0,D2
	MOVEQ	#0,D3
L3DDE6	MOVE.L	D2,D2
	BMI.S	L3DE12
	MOVE.L	D0,D0
	BMI.S	L3DE02
	CMP.L	D2,D0
L3DDF0	BLT.S	L3DE02
	BGT.S	L3DE0A
	CMP.L	D3,D1
	BCS.S	L3DE02
	BHI.S	L3DE0A
	MOVEQ	#0,D0
	MOVEM.L	(A7)+,D1-4
	RTS
L3DE02	MOVEQ	#-1,D0
	MOVEM.L	(A7)+,D1-4
	RTS
L3DE0A	MOVEQ	#1,D0
	MOVEM.L	(A7)+,D1-4
	RTS
L3DE12	MOVE.L	D0,D0
	BPL.S	L3DE0A
	EXG	D1,D3
	CMP.L	D0,D2
	BRA.S	L3DDF0
L3DE1C	MOVE.L	D0,D1
	SWAP	D1
	ANDI.W	#$7FF0,D1
	BEQ.S	L3DE4A
	MOVE.L	D0,D0
	BPL.S	L3DE2E
	MOVEQ	#-1,D0
	RTS
L3DE2E	MOVEQ	#1,D0
	RTS
	MOVE.L	D0,-(A7)
	SWAP	D0
	ANDI.W	#$7FF0,D0
	BEQ.S	L3DE46
	MOVE.L	(A7)+,D0
	ANDI.L	#$7FFFFFFF,D0
	RTS
L3DE46	ADDQ.L	#4,A7
	MOVEQ	#0,D1
L3DE4A	MOVEQ	#0,D0
	RTS
L3DE4E	MOVE.L	D0,-(A7)
	SWAP	D0
	ANDI.W	#$7FF0,D0
	BEQ.S	L3DE46
	MOVE.L	(A7)+,D0
	EORI.L	#$80000000,D0
	RTS
L3DE62	MOVE.L	D3,D1
	MOVE.L	D2,D0
	SWAP	D2
	ANDI.W	#$7FF0,D2
	BNE.S	L3DE72
L3DE6E	MOVEQ	#0,D1
	MOVEQ	#0,D0
L3DE72	MOVEM.L	(A7)+,D4-7/A0
	MOVE.L	D0,D0
	RTS
L3DE7A	EORI.L	#$80000000,D2
L3DE80	MOVEM.L	D4-7/A0,-(A7)
	MOVEQ	#0,D6
	MOVEQ	#0,D7
	MOVE.L	D0,D4
	SMI	D6
	SWAP	D4
	ANDI.W	#$7FF0,D4
	BEQ.S	L3DE62
	LSR.W	#4,D4
	MOVE.L	D2,D5
	SMI	D7
	SWAP	D5
	ANDI.W	#$7FF0,D5
	BEQ.S	L3DE72
	LSR.W	#4,D5
	SUBI.W	#$3FF,D4
	SUBI.W	#$3FF,D5
	ANDI.L	#$FFFFF,D0
	ORI.L	#$100000,D0
	MOVEA.L	D6,A0
	SWAP	D5
	MOVE.W	#$B,D5
	LSL.L	D5,D0
	MOVE.L	D1,D6
	SWAP	D6
	LSR.W	#5,D6
	OR.W	D6,D0
	LSL.L	D5,D1
	ANDI.L	#$FFFFF,D2
	ORI.L	#$100000,D2
	LSL.L	D5,D2
	MOVE.L	D3,D6
	SWAP	D6
	LSR.W	#5,D6
	OR.W	D6,D2
	LSL.L	D5,D3
	SWAP	D5
	MOVE.L	A0,D6
	CMP.W	D5,D4
	BEQ.S	L3DF66
	BGT.S	L3DEF6
	EXG	D0,D2
	EXG	D1,D3
	EXG	D4,D5
	EXG	D6,D7
L3DEF6	MOVEA.L	D4,A0
	SUB.W	D5,D4
	CMPI.W	#$37,D4
	BMI.S	L3DF06
	MOVEQ	#0,D2
	MOVEQ	#0,D3
	BRA.S	L3DF64
L3DF06	CMPI.W	#5,D4
	BMI.S	L3DF5A
	CMPI.W	#$20,D4
	BMI.S	L3DF36
	SUBI.W	#$20,D4
	MOVE.L	D3,D3
	BEQ.S	L3DF26
	MOVE.L	D2,D3
	LSR.L	D4,D3
L3DF1E	ORI.W	#1,D3
	MOVEQ	#0,D2
	BRA.S	L3DF64
L3DF26	MOVE.L	D2,D3
	LSR.L	D4,D3
	MOVE.L	D3,D5
	LSL.L	D4,D5
	CMP.L	D2,D5
	BNE.S	L3DF1E
	MOVEQ	#0,D2
	BRA.S	L3DF64
L3DF36	MOVE.W	D6,-(A7)
	MOVE.L	D3,D5
	LSR.L	D4,D3
	MOVE.L	D3,D6
	LSL.L	D4,D6
	CMP.L	D5,D6
	BEQ.S	L3DF48
	ORI.W	#1,D3
L3DF48	MOVE.W	(A7)+,D6
	MOVE.L	D2,D5
	LSR.L	D4,D2
	NEG.W	D4
	ADDI.W	#$20,D4
	LSL.L	D4,D5
	OR.L	D5,D3
	BRA.S	L3DF64
L3DF5A	SUBQ.W	#1,D4
L3DF5C	LSR.L	#1,D2
	ROXR.L	#1,D3
	DBF	D4,L3DF5C
L3DF64	MOVE.L	A0,D4
L3DF66	CMP.W	D7,D6
	BGT.S	L3DF7A
	BLT.S	L3DF80
	ADD.L	D3,D1
	ADDX.L	D2,D0
	BCC.S	L3DF8C
	ROXR.L	#1,D0
	ROXR.L	#1,D1
	ADDQ.W	#1,D4
	BRA.S	L3DF8C
L3DF7A	EXG	D0,D2
	EXG	D1,D3
	MOVE.L	D7,D6
L3DF80	SUB.L	D3,D1
	SUBX.L	D2,D0
	BCC.S	L3DF8C
	NEGX.B	D6
	NEG.L	D1
	NEGX.L	D0
L3DF8C	JSR	L3E26A(PC)
	MOVEM.L	(A7)+,D4-7/A0
	RTS
L3DF96	MOVEQ	#0,D0
	MOVEQ	#0,D1
	MOVEM.L	(A7)+,D4-7/A0-2
	RTS
L3DFA0	MOVEM.L	D4-7/A0-2,-(A7)
	MOVE.L	D0,D4
	SMI	D6
	SWAP	D4
	ANDI.W	#$7FF0,D4
	BEQ.S	L3DF96
	MOVE.L	D2,D5
	SMI	D7
	SWAP	D5
	ANDI.W	#$7FF0,D5
	BEQ.S	L3DF96
	ANDI.L	#$FFFFF,D0
	ORI.L	#$100000,D0
	ADD.W	D5,D4
	LSR.W	#4,D4
	SUBI.W	#$7FD,D4
	MOVEA.L	D4,A0
	EOR.B	D7,D6
	MOVEA.L	D6,A1
	MOVEQ	#$B,D5
	LSL.L	D5,D0
	MOVE.L	D1,D6
	SWAP	D6
	LSR.W	#5,D6
	OR.W	D6,D0
	LSL.L	D5,D1
	ANDI.L	#$FFFFF,D2
	ORI.L	#$100000,D2
	LSL.L	D5,D2
	MOVE.L	D3,D6
	SWAP	D6
	LSR.W	#5,D6
	OR.W	D6,D2
	LSL.L	D5,D3
	MOVEQ	#0,D7
	MOVEA.L	D2,A2
	MOVE.L	D3,D2
	MOVEQ	#0,D3
	MOVEQ	#0,D4
	MOVEQ	#0,D5
	MOVEQ	#0,D6
	TST.W	D2
	BEQ.S	L3E024
	MOVE.W	D0,D3
	BEQ.S	L3E018
	MULU	D2,D3
	SWAP	D3
	MOVE.W	D3,D6
L3E018	MOVE.L	D0,D3
	SWAP	D3
	TST.W	D3
	BEQ.S	L3E024
	MULU	D2,D3
	ADD.L	D3,D6
L3E024	SWAP	D2
	TST.W	D2
	BEQ.S	L3E058
	MOVE.L	D1,D3
	SWAP	D3
	TST.W	D3
	BEQ.S	L3E03C
	MULU	D2,D3
	CLR.W	D3
	SWAP	D3
	ADD.L	D3,D6
	ADDX.W	D7,D5
L3E03C	MOVE.W	D0,D3
	BEQ.S	L3E046
	MULU	D2,D3
	ADD.L	D3,D6
	ADDX.W	D7,D5
L3E046	MOVE.L	D0,D3
	SWAP	D3
	MULU	D2,D3
	SWAP	D6
	ADD.W	D3,D6
	SWAP	D6
	CLR.W	D3
	SWAP	D3
	ADDX.L	D3,D5
L3E058	MOVE.L	A2,D2
	TST.W	D2
	BEQ.S	L3E096
	MOVE.W	D1,D3
	BEQ.S	L3E06C
	MULU	D2,D3
	CLR.W	D3
	SWAP	D3
	ADD.L	D3,D6
	ADDX.L	D7,D5
L3E06C	MOVE.L	D1,D3
	SWAP	D3
	TST.W	D3
	BEQ.S	L3E07A
	MULU	D2,D3
	ADD.L	D3,D6
	ADDX.L	D7,D5
L3E07A	MOVE.W	D0,D3
	BEQ.S	L3E08C
	MULU	D2,D3
	SWAP	D6
	ADD.W	D3,D6
	SWAP	D6
	CLR.W	D3
	SWAP	D3
	ADDX.L	D3,D5
L3E08C	MOVE.L	D0,D3
	SWAP	D3
	MULU	D2,D3
	ADD.L	D3,D5
	ADDX.L	D7,D4
L3E096	SWAP	D2
	MOVE.W	D1,D3
	BEQ.S	L3E0A4
	MULU	D2,D3
	ADD.L	D3,D6
	ADDX.L	D7,D5
	ADDX.L	D7,D4
L3E0A4	SWAP	D1
	MOVE.W	D1,D3
	BEQ.S	L3E0BA
	MULU	D2,D3
	SWAP	D6
	ADD.W	D3,D6
	SWAP	D6
	CLR.W	D3
	SWAP	D3
	ADDX.L	D3,D5
	ADDX.L	D7,D4
L3E0BA	MOVE.W	D0,D3
	BEQ.S	L3E0C4
	MULU	D2,D3
	ADD.L	D3,D5
	ADDX.L	D7,D4
L3E0C4	SWAP	D0
	MOVE.W	D0,D3
	MULU	D2,D3
	SWAP	D5
	ADD.W	D3,D5
	SWAP	D5
	CLR.W	D3
	SWAP	D3
	ADDX.L	D3,D4
	CMPI.L	#$FFFF,D4
	BLS.S	L3E0E6
	ADDQ.W	#1,A1
	LSR.L	#1,D4
	ROXR.L	#1,D5
	ROXR.L	#1,D6
L3E0E6	CMPI.W	#$8000,D6
	BEQ.S	L3E10A
	BLS.S	L3E110
	SWAP	D6
	ADDQ.W	#1,D6
	SWAP	D6
	ADDX.L	D7,D5
	ADDX.L	D7,D4
	CMPI.L	#$FFFF,D4
	BLS.S	L3E110
	ADDQ.W	#1,A0
	LSR.L	#1,D4
	ROXR.L	#1,D5
	ROXR.L	#1,D6
	BRA.S	L3E110
L3E10A	ORI.L	#$10000,D6
L3E110	MOVE.W	D5,D6
	SWAP	D6
	MOVE.L	D6,D1
	MOVE.W	D4,D5
	SWAP	D5
	MOVE.L	D5,D0
	MOVE.L	A0,D4
	MOVE.L	A1,D6
	JSR	L3E26C(PC)
	MOVEM.L	(A7)+,D4-7/A0-2
	RTS
L3E12A	MOVEM.L	D4-7/A0,-(A7)
	MOVE.L	D0,D4
	SMI	D6
	SWAP	D4
	ANDI.W	#$7FF0,D4
	BEQ	L3DE6E
	MOVE.L	D2,D5
	SMI	D7
	SWAP	D5
	ANDI.W	#$7FF0,D5
	BEQ	L3E23A
	ANDI.L	#$FFFFF,D0
	ORI.L	#$100000,D0
	SUB.W	D5,D4
	ASR.W	#4,D4
	SUBQ.W	#1,D4
	MOVEA.W	D4,A0
	EOR.B	D7,D6
	SWAP	D6
	MOVEQ	#$B,D5
	LSL.L	D5,D0
	MOVE.L	D1,D7
	SWAP	D7
	LSR.W	#5,D7
	OR.W	D7,D0
	LSL.L	D5,D1
	ANDI.L	#$FFFFF,D2
	ORI.L	#$100000,D2
	LSL.L	D5,D2
	MOVE.L	D3,D7
	SWAP	D7
	LSR.W	#5,D7
	OR.W	D7,D2
	LSL.L	D5,D3
	TST.W	D2
	BNE.S	L3E1CA
	TST.L	D3
	BNE.S	L3E1CA
	SWAP	D2
	SWAP	D0
	CMP.W	D2,D0
	BEQ.S	L3E19A
	BLS.S	L3E1A4
L3E19A	SWAP	D0
	ADDQ.W	#1,A0
	LSR.L	#1,D0
	ROXR.L	#1,D1
	SWAP	D0
L3E1A4	SWAP	D0
	DIVU	D2,D0
	MOVE.W	D0,D4
	SWAP	D4
	SWAP	D1
	MOVE.W	D1,D0
	DIVU	D2,D0
	MOVE.W	D0,D4
	SWAP	D1
	MOVE.W	D1,D0
	DIVU	D2,D0
	MOVE.W	D0,D5
	SWAP	D5
	CLR.W	D0
	DIVU	D2,D0
	MOVE.W	D0,D5
	MOVE.L	D4,D0
	MOVE.L	D5,D1
	BRA.S	L3E22E
L3E1CA	CMP.L	D0,D2
	BNE.S	L3E1DE
	CMP.L	D1,D3
	BNE.S	L3E1DE
	MOVE.L	#$80000000,D0
	ADDQ.W	#1,A0
	MOVEQ	#0,D1
	BRA.S	L3E22E
L3E1DE	BHI.S	L3E1E6
	ADDQ.W	#1,A0
	LSR.L	#1,D0
	ROXR.L	#1,D1
L3E1E6	LSR.L	#1,D2
	ROXR.L	#1,D3
	LSR.L	#1,D0
	ROXR.L	#1,D1
	MOVE.W	#2,D6
L3E1F2	MOVEQ	#0,D4
	MOVEQ	#$1F,D7
L3E1F6	LSL.L	#1,D4
	LSL.L	#1,D1
	ROXL.L	#1,D0
	SUB.L	D3,D1
	SUBX.L	D2,D0
	BMI.S	L3E222
L3E202	ADDQ.W	#1,D4
	DBF	D7,L3E1F6
	SUBQ.W	#1,D6
	BEQ.S	L3E22A
	MOVE.L	D4,D5
	BRA.S	L3E1F2
L3E210	MOVE.L	D4,D5
	MOVEQ	#0,D4
	MOVEQ	#$1F,D7
L3E216	LSL.L	#1,D4
	LSL.L	#1,D1
	ROXL.L	#1,D0
	ADD.L	D3,D1
	ADDX.L	D2,D0
	BPL.S	L3E202
L3E222	DBF	D7,L3E216
	SUBQ.W	#1,D6
	BNE.S	L3E210
L3E22A	MOVE.L	D5,D0
	MOVE.L	D4,D1
L3E22E	MOVE.L	A0,D4
	SWAP	D6
	BSR.S	L3E26A
L3E234	MOVEM.L	(A7)+,D4-7/A0
	RTS
L3E23A	MOVE.L	#$E,-$7E62(A4)
	BSR.S	L3E252
	BRA.S	L3E234
L3E246	MOVEQ	#0,D1
	RTS
L3E24A	MOVE.L	#$D,-$7E62(A4)
L3E252	MOVEQ	#-1,D1
	MOVE.L	#$7FFFFFFF,D0
	TST.B	D6
	BEQ.S	L3E264
	ORI.L	#$80000000,D0
L3E264	ORI.B	#2,CCR
	RTS
L3E26A	MOVEQ	#0,D7
L3E26C	TST.L	D0
	BNE.S	L3E27A
	EXG	D0,D1
	SUBI.W	#$20,D4
	TST.L	D0
	BEQ.S	L3E246
L3E27A	BMI.S	L3E284
L3E27C	SUBQ.W	#1,D4
	LSL.L	#1,D1
	ROXL.L	#1,D0
	BGE.S	L3E27C
L3E284	MOVE.W	D1,D5
	ANDI.W	#$7FF,D5
	CMPI.W	#$400,D5
	BEQ.S	L3E2A4
	BLS.S	L3E2AC
L3E292	ADDI.L	#$800,D1
	ADDX.L	D7,D0
	BCC.S	L3E2AC
	ROXR.L	#1,D0
	ROXR.L	#1,D1
	ADDQ.W	#1,D4
	BRA.S	L3E2AC
L3E2A4	MOVE.W	D1,D5
	ANDI.W	#$800,D5
	BNE.S	L3E292
L3E2AC	MOVEQ	#$B,D5
	MOVE.W	D0,D7
	LSL.W	#5,D7
	LSR.L	D5,D0
	LSR.L	D5,D1
	SWAP	D1
	OR.W	D7,D1
	SWAP	D1
	ANDI.L	#$FFFFF,D0
	ADDI.W	#$3FF,D4
	BMI.S	L3E2E2
	CMPI.W	#$7FF,D4
	BGT	L3E24A
	TST.B	D6
	BEQ.S	L3E2D8
	ORI.W	#$800,D4
L3E2D8	LSL.W	#4,D4
	SWAP	D0
	OR.W	D4,D0
	SWAP	D0
	RTS
L3E2E2	MOVEQ	#0,D1
	MOVE.L	#$D,-$7E62(A4)
	MOVE.L	#$100000,D0
	TST.B	D6
	BEQ.S	L3E2FC
	ORI.L	#$80000000,D0
L3E2FC	RTS


L3E854	MOVEM.L	D1/D4,-(A7)
	CLR.L	D4
	TST.L	D0
	BPL.S	L3E862
	NEG.L	D0
	ADDQ.W	#1,D4
L3E862	TST.L	D1
	BPL.S	L3E86C
	NEG.L	D1
	EORI.W	#1,D4
L3E86C	BSR.S	L3E8AC
L3E86E	TST.W	D4
	BEQ.S	L3E874
	NEG.L	D0
L3E874	MOVEM.L	(A7)+,D1/D4
	TST.L	D0
	RTS
L3E87C	MOVEM.L	D1/D4,-(A7)
	CLR.L	D4
	TST.L	D0
	BPL.S	L3E88A
	NEG.L	D0
	ADDQ.W	#1,D4
L3E88A	TST.L	D1
	BPL.S	L3E890
	NEG.L	D1
L3E890	BSR.S	L3E8AC
	MOVE.L	D1,D0
	BRA.S	L3E86E
	MOVE.L	D1,-(A7)
	BSR.S	L3E8AC
	MOVE.L	D1,D0
	MOVE.L	(A7)+,D1
	TST.L	D0
	RTS
	MOVE.L	D1,-(A7)
	BSR.S	L3E8AC
	MOVE.L	(A7)+,D1
	TST.L	D0
	RTS
L3E8AC	MOVEM.L	D2-3,-(A7)
	SWAP	D1
	TST.W	D1
	BNE.S	L3E8D6
	SWAP	D1
	MOVE.W	D1,D3
	MOVE.W	D0,D2
	CLR.W	D0
	SWAP	D0
	DIVU	D3,D0
	MOVE.L	D0,D1
	SWAP	D0
	MOVE.W	D2,D1
	DIVU	D3,D1
	MOVE.W	D1,D0
	CLR.W	D1
	SWAP	D1
	MOVEM.L	(A7)+,D2-3
	RTS
L3E8D6	SWAP	D1
	MOVE.L	D1,D3
	MOVE.L	D0,D1
	CLR.W	D1
	SWAP	D1
	SWAP	D0
	CLR.W	D0
	MOVEQ	#$F,D2
L3E8E6	ADD.L	D0,D0
	ADDX.L	D1,D1
	CMP.L	D1,D3
	BHI.S	L3E8F2
	SUB.L	D3,D1
	ADDQ.W	#1,D0
L3E8F2	DBF	D2,L3E8E6
	MOVEM.L	(A7)+,D2-3
	RTS

	Rdata

DDebut	DC.B	$40,$24,0,0,0,0,0,0
	DC.B	$3F,$F0,0,0,0,0,0,0
	DC.B	$3F,$E0,0,0,0,0,0,0
	DC.B	$3F,$A9,$99,$99,$99,$99,$99,$9A
	DC.B	$3F,$74,$7A,$E1,$47,$AE,$14,$7B
	DC.B	$3F,$40,$62,$4D,$D2,$F1,$A9,$FC
	DC.B	$3F,$A,$36,$E2,$EB,$1C,$43,$2D
	DC.B	$3E,$D4,$F8,$B5,$88,$E3,$68,$F1
	DC.B	$3E,$A0,$C6,$F7,$A0,$B5,$ED,$8E
	DC.B	$3E,$6A,$D7,$F2,$9A,$BC,$AF,$49
	DC.B	$3E,$35,$79,$8E,$E2,$30,$8C,$3A
	DC.B	$3E,1,$2E,$B,$E8,$26,$D6,$95
	DC.B	$3D,$CB,$7C,$DF,$D9,$D7,$BD,$BB
	DC.B	$3D,$95,$FD,$7F,$E1,$79,$64,$96
	DC.B	$3D,$61,$97,$99,$81,$2D,$EA,$12
	DC.B	$3D,$2C,$25,$C2,$68,$49,$76,$82
	DC.B	$3C,$F6,$84,$9B,$86,$A1,$2B,$9C
	DC.B	$3C,$C2,3,$AF,$9E,$E7,$56,$16
	DC.B	0,$20,$20,$20,$20,$20,$20,$20
	DC.B	$20,$20,$30,$30,$30,$30,$30,$20
	DC.B	$20,$20,$20,$20,$20,$20,$20,$20
	DC.B	$20,$20,$20,$20,$20,$20,$20,$20
	DC.B	$20,$90,$40,$40,$40,$40,$40,$40
	DC.B	$40,$40,$40,$40,$40,$40,$40,$40
	DC.B	$40,$C,$C,$C,$C,$C,$C,$C
	DC.B	$C,$C,$C,$40,$40,$40,$40,$40
	DC.B	$40,$40,9,9,9,9,9,9
	DC.B	1,1,1,1,1,1,1,1
	DC.B	1,1,1,1,1,1,1,1
	DC.B	1,1,1,1,$40,$40,$40,$40
	DC.B	$40,$40,$A,$A,$A,$A,$A,$A
	DC.B	2,2,2,2,2,2,2,2
	DC.B	2,2,2,2,2,2,2,2
	DC.B	2,2,2,2,$40,$40,$40,$40
	DC.B	$20,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,$14,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,0,0,0,0
	DC.B	0,0,0,0,$C2,$9D,$34,$EF
	DC.B	0,0,0,0,0,0,0,0
	Even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DES ROUTINES EXTERNES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_Externes
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TERMINE LA LIBRARIE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_End

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		Titre
C_Title		dc.b 	"AMOSPro 2.0 Main Library V "
		Version
		dc.b	0,"$VER: "
		Version
		dc.b	0
		Even
C_End		dc.w	0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
