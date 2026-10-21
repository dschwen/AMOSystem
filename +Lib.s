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
	Lib_Def	FnSgnF			Float / Double
; - - - - - - - - - - - - -
	Rjsrt	L_Float_Test
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=STR$ (fmath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	FnStrE		src/lib/system.cpp
; - - - - - - - - - - - - -
	Lib_Def FnStrF			FLOAT/DOUBLE
; - - - - - - - - - - - - -
	movem.l	d3/d4,-(sp)
        moveq 	#40,d3
        Rjsr 	L_Demande
        movem.l	(sp)+,d3/d4
	lea	2(a0),a0
	RjsrtR	L_Float2Ascii,2
        Rbra 	L_FinBin

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ABS (FMath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int FnAbs			Integer: src/lib/maths.cpp
; - - - - - - - - - - - - -
	Lib_Def FnAbsF			Float + DFloat
; - - - - - - - - - - - - -
	moveq	#_LVOSPAbs,d2
	Rjmpt	L_Float_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INT (FMath)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnInt			Integer
; - - - - - - - - - - - - -
	rts
; - - - - - - - - - - - - -
	Lib_Def FnIntF			Float + DFloat
; - - - - - - - - - - - - -
	moveq	#_LVOSPFloor,d2
	Rjmpt	L_Float_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TAN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnTan
; - - - - - - - - - - - - -
	moveq	#_LVOSPTan,d2		Float + DFloat
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SQR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnSqr
; - - - - - - - - - - - - -
	Rjsrt	L_FlPos
	moveq	#_LVOSPSqrt,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Log10
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnLog
; - - - - - - - - - - - - -
	Rjsrt	L_FlPos
	moveq	#_LVOSPLog10,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Ln
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnLn
; - - - - - - - - - - - - -
	Rjsrt	L_FlPos
	moveq	#_LVOSPLog,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnExp
; - - - - - - - - - - - - -
	moveq	#_LVOSPExp,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnSin
; - - - - - - - - - - - - -
	moveq	#_LVOSPSin,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					COS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnCos
; - - - - - - - - - - - - -
	moveq	#_LVOSPCos,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ASIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnASin
; - - - - - - - - - - - - -
	moveq	#_LVOSPAsin,d2
	Rjmpt	L_AAngle

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ACOS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnACos
; - - - - - - - - - - - - -
	moveq	#_LVOSPAcos,d2
	Rjmpt	L_AAngle

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ATAN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnATan
; - - - - - - - - - - - - -
	moveq	#_LVOSPAtan,d2
	Rjmpt	L_AAngle

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HSIN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnHSin
; - - - - - - - - - - - - -
	moveq	#_LVOSPSinh,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HCOS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnHCos
; - - - - - - - - - - - - -
	moveq	#_LVOSPCosh,d2
	Rjmpt	L_Math_Fonction

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HTAN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnHTan
; - - - - - - - - - - - - -
	moveq	#_LVOSPTanh,d2
	Rjmpt	L_Math_Fonction

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
	Lib_Def	Lib.Call
; - - - - - - - - - - - - -
	movem.l	a3-a6/d6-d7,-(sp)
	pea	.Return(pc)
	pea	0(a0,d3.l)
	move.l	a0,-(sp)
	lea	CallReg(a5),a6
	movem.l	(a6),d0-d7/a0-a5
	move.l	(sp)+,a6
	rts
.Return	movem.l	(sp)+,a3-a6/d6-d7
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					EXEDOSINTGFXCALL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnExeCall
; - - - - - - - - - - - - -
	move.l	$4.W,a0
	Rbra	L_Lib.Call
; - - - - - - - - - - - - -
	Lib_Par	FnGfxCall
; - - - - - - - - - - - - -
	move.l	T_GfxBase(a5),a0
	Rbra	L_Lib.Call
; - - - - - - - - - - - - -
	Lib_Par	FnDosCall
; - - - - - - - - - - - - -
	move.l	DosBase(a5),a0
	Rbra	L_Lib.Call
; - - - - - - - - - - - - -
	Lib_Par	FnIntCall
; - - - - - - - - - - - - -
	move.l	T_IntBase(a5),a0
	Rbra	L_Lib.Call

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
	Lib_Par	FnLibCall
; - - - - - - - - - - - - -
	move.l	(a3)+,d0
	Rjsr	L_Lib.GetA2
	move.l	(a2),a0
	bne.s	.Ok
	move.w	#169,d0			Library not opened
	Rbra	L_GoError
.Ok	Rbra	L_Lib.Call

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
	Lib_Def	ChVerBuf
; - - - - - - - - - - - - -
	move.l	Buffer(a5),a0
	Rbra	L_ChVerBuf2
; - - - - - - - - - - - - -
	Lib_Def	ChVerBuf2
; - - - - - - - - - - - - -
	move.l	a2,a1
	move.w	d2,d0
	beq.s	Chv2
	subq.w	#1,d0
	cmp.w	#510,d0
	bcs.s	Chv1
	move.w	#509,d0
Chv1:	move.b	(a1)+,(a0)+
	dbra	d0,Chv1
Chv2:	clr.b	(a0)+
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Str2Chaine
; Recopie un message systeme (a0) fini par zero dans le buffer chaines
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Str2Chaine
; - - - - - - - - - - - - -
	move.l	a2,a0
.c	tst.b	(a0)+
	bne.s	.c
	sub.l	a2,a0
	move.l	a0,d3
	subq.l	#1,d3
	Rbne	L_MCSuit
	Rbra	L_Ret_ChVide
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Chaine (a2) >>> buffer chaines
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Mes2Chaine
; - - - - - - - - - - - - -
	moveq	#0,d3
	move.b	-1(a2),d3
	Rbeq	L_Ret_ChVide
	cmp.b	#$FF,d3
	Rbne	L_MCSuit
	Rbra	L_Ret_ChVide
; - - - - - - - - - - - - -
	Lib_Def	MCSuit
; - - - - - - - - - - - - -
	Rjsr	L_Demande
	move.w	d3,(a0)+
	move.w	d3,d0
	move.l	a1,d3
.Loop	move.b	(a2)+,(a0)+
	subq.w	#1,d0
	bne.s	.Loop
	Rbra	L_Set_HiChaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Retourne une chaine > hichaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Set_HiChaine
; - - - - - - - - - - - - -
	move.l	d3,a0
	moveq	#0,d0
	move.w	(a0)+,d0
	addq.w	#1,d0
	and.w	#$FFFE,d0
	add.l	d0,a0
	move.l	a0,HiChaine(a5)
	Ret_String
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Retourne une chaine vide
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Ret_ChVide
; - - - - - - - - - - - - -
	move.l	ChVide(a5),d3
	Ret_String
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Copie A0 en chaine AMOS + RTS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	A0ToChaine
; - - - - - - - - - - - - -
	move.l	a0,a2
.Count	tst.b	(a0)+
	bne.s	.Count
	subq.l	#1,a0
	move.l	a0,d3
	sub.l	a2,d3
	Rjsr	L_DDemande
	move.w	d3,(a1)+
	move.w	d3,d0
.Copy	move.b	(a2)+,(a1)+
	dbra	d0,.Copy
	move.l	a1,d0
	Pair	d0
	move.l	d0,HiChaine(a5)
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Fin de BIN, HEX et autres
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	FinBin
; - - - - - - - - - - - - -
        move.l 	a0,d0        	;rend pair
        btst 	#0,d0
        beq.s 	hx5
        addq.l 	#1,d0
hx5:    move.l 	d0,HiChaine(a5)
        sub.l 	a1,a0
        subq.l 	#2,a0
        move 	a0,(a1)        	;poke la longueur
        move.l	a1,d3		;Ramene la chaine
	Ret_String

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
	Lib_Def	ImpChaine
; - - - - - - - - - - - - -
	tst.w	ImpFlg(a5)		Imprimante?
	Rbne	L_PRT_Print
	tst.w	Direct(a5)
	bne.s	.Dir
.Norm	tst.w	ScOn(a5)		Mode programme
	Rbeq	L_ScNOp
	move.l	a0,a1
	WiCall	Print
	rts
.Dir	tst.b	Esc_Output(a5)		Mode direct > ESC / Normal
	beq.s	.Norm
	move.l	a0,-(sp)
	EcCalD	Active,EcEdit
        move.l	(sp)+,a1
	WiCall	Print
	move.w	ScOn(a5),d1
	beq.s	.Skip
	subq.w	#1,d1
	EcCall	Active
.Skip	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					OUVERTURE DE L'IMPRIMANTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	PRT_Open
; - - - - - - - - - - - - -
	move.l	PrtHandle(a5),d1
	bne.s	.OpPrx
	movem.l	a0-a2/d2-d7,-(sp)
	moveq	#43,d0
	Rjsr	L_Sys_GetMessage
	move.l	a0,d1
	move.l	#1005,d2
	DosCall	_LVOOpen
	move.l	d0,PrtHandle(a5)
	bne.s	*+8
	Rjmp	L_DiskError
; Branche la routine de fermeture
	lea	.Struc(pc),a1
	lea	Sys_ClearRoutines(a5),a2
	SyCall	AddRoutine
; Ok!
	move.l	PrtHandle(a5),d0
	move.l	d0,d1
	movem.l	(sp)+,a0-a2/d2-d7
.OpPrx	rts
.Struc	dc.l	0
	Rbra	L_PRT_Close

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FERMETURE IMPRIMANTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	PRT_Close
; - - - - - - - - - - - - -
	movem.l	a0-a1/a6/d0-d1,-(sp)
	move.l	DosBase(a5),a6
	move.l	PrtHandle(a5),d1
	beq.s	.Skip
	clr.l	PrtHandle(a5)
	jsr	DosClose(a6)
.Skip	movem.l	(sp)+,a0-a1/a6/d0-d1
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Imprime la chaine A0 > imprimante
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	PRT_Print
; - - - - - - - - - - - - -
	Rbsr	L_PRT_Open
	movem.l	a0-a2/d2-d7,-(sp)
	move.l	a0,d2
	move.l	a0,a1
* Compte et enleve les CARRIAGE RETURN
Ip1:	move.b	(a0)+,d0
	move.b	d0,(a1)+
	beq.s	Ip2
	cmp.b	#13,d0
	bne.s	Ip1
	tst.b	PI_PrtRet(a5)
	bne.s	Ip1
	cmp.b	#10,(a0)
	bne.s	Ip1
	move.b	(a0)+,-1(a1)
	bra.s	Ip1
* Envoie!
Ip2	sub.l	d2,a0
	subq.l	#1,a0
	move.l	a0,d3
	beq.s	Ip3
	DosCall	_LVOWrite
	tst.l	d0
	bpl.s	*+8
	Rjmp	L_DiskError
Ip3:	movem.l	(sp)+,a0-a2/d2-d7
	rts

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
	Lib_Par	InAssign
; - - - - - - - - - - - - -
	tst.w	WB2.0(a5)
	beq.s	.1_3
; WB2.0, appel direct de l'instruction ASSIGN
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Rjsr	L_LockFree		Plus de LOCK
	move.l	d3,a2
	move.w	(a2)+,d2
	beq.s	.Vide
	cmp.w	#108,d2
	Rbcc	L_FonCall
	move.l	Name1(a5),a0
	Rbsr	L_ChVerBuf2
	Rjsr	L_Dsk.PathIt
	Rjsr	L_LockGet
.Vide	move.l	(a3)+,a2		Prend l'assign
	move.w	(a2)+,d2
	Rbeq	L_FonCall
	cmp.w	#108,d2
	Rbcc	L_FonCall
	Rbsr	L_ChVerBuf
	move.l	Buffer(a5),a0
.DP	tst.b	(a0)+			Enleve le ":"
	bne.s	.DP
	cmp.b	#":",-2(a0)
	Rbne	L_FonCall
	clr.b	-2(a0)
	move.l	Buffer(a5),d1		Appelle la fonction
	move.l	LockSave(a5),d2
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOAssignLock(a6)
	move.l	(sp)+,a6
	tst.l	d0
	beq.s	.Err
	clr.l	LockSave(a5)		Success: enleve le lock!
	rts
.Err	Rjsr	L_LockFree
	Rjmp	L_DiskError
; Version 1.3: appelle la commande ASSIGN dans le directory C:
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.1_3	move.l	d3,-(a3)
; Assign disponible?
	lea	.AssI(pc),a0
	move.l	Name1(a5),a1
	CoCopy
	Rjsr	L_RExist
	bne.s	*+8
	Rjmp	L_DiskError
; Pathname
	move.l	Name1(a5),a0
	clr.b	(a0)
	move.l	(a3)+,a2		Prend le pathname
	move.w	(a2)+,d2
	beq.s	.Vide2
	cmp.w	#108,d2
	Rbcc	L_FonCall
	Rbsr	L_ChVerBuf2
	Rjsr	L_Dsk.PathIt
	Rjsr	L_LockGet		Verifie le path
	Rjsr	L_LockFree
; Nom
.Vide2	move.l	Buffer(a5),a2
	lea	.AssI(pc),a0		Command ASSIGN
.Copy1	move.b	(a0)+,(a2)+
	bne.s	.Copy1
	move.b	#" ",-1(a2)
	move.l	(a3)+,a1		Nom a affecter
	move.w	(a1)+,d1
	Rbeq	L_FonCall
	cmp.w	#64,d1
	Rbcc	L_FonCall
.Copy2	move.b	(a1)+,d0		Nom disque
	move.b	d0,(a2)+
	cmp.b	#32,d0
	Rbls	L_FonCall
	cmp.b	#":",d0
	beq.s	.Fini
	subq.w	#1,d1
	bne.s	.Copy2
	Rbra	L_FonCall
.Fini	move.b	#" ",(a2)+
	move.l	Name1(a5),a1
.Copy3	move.b	(a1)+,(a2)+
	bne.s	.Copy3
; Appel de la fonction
	lea	.AssC(pc),a0		Ouvre le NIL:
	move.l	a0,d1
	Rjsr	L_D_OpenD1
	bne.s	*+8
	Rjmp	L_DiskError
	move.l	Buffer(a5),d1		Execute >NIL: >NIL:
	move.l	Handle(a5),d2
	move.l	d2,d3
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOExecute(a6)
	move.l	(sp)+,a6
	Rjsr	L_D_Close
	tst.l	d0
	bne.s	*+8
	Rjmp	L_DiskError
	rts
.AssI	dc.b	"c:assign",0
.AssC	dc.b	"NIL:",0
	even

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
	Lib_Par	FnFileSelector1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	ChVide(a5),d3
	move.l	d3,-(a3)
	move.l	d3,-(a3)
	Rbra	L_FnFileSelector4
; - - - - - - - - - - - - -
	Lib_Par	FnFileSelector2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	ChVide(a5),d3
	move.l	d3,-(a3)
	Rbra	L_FnFileSelector4
; - - - - - - - - - - - - -
	Lib_Par	FnFileSelector3
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	ChVide(a5),d3
	Rbra	L_FnFileSelector4
; - - - - - - - - - - - - -
	Lib_Par	FnFileSelector4
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	Rjsr	L_Dsk.FileSelector
	move.l	d0,d3
	Rbeq	L_Ret_ChVide
; Recopie la chaine
	move.l	a0,a2
	Rjsr	L_Demande
	move.w	d3,(a0)+
	lsr.w	#1,d3
.Copy	move.w	(a2)+,(a0)+
	dbra	d3,.Copy
	move.l	a0,HiChaine(a5)
	move.l	a1,d3
	Ret_String

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Apparition / disparition par le centre
;	A2= ecran
;	d7= vitesse
;	d6= WTy
;	d5= WY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	AppCentre
; - - - - - - - - - - - - -
FsApp1	move.w	d6,d4
	move.w	d6,EcAWTY(a2)
	add.w	d6,EcAWTY(a2)
	bset	#2,EcAWT(a2)
	move.w	EcTy(a2),d0
	lsr.w	#1,d0
	sub.w	d6,d0
	move.w	d0,EcAVY(a2)
	bset	#2,EcAV(a2)
	move.w	d5,EcAWY(a2)
	sub.w	d6,EcAWY(a2)
	bset	#2,EcAW(a2)
	movem.l	a2/d4-d7,-(sp)
	Rjsr	L_ReCop
	movem.l	(sp)+,a2/d4-d7
	add.w	d7,d6
	bpl.s	FsApp2
	moveq	#1,d6
FsApp2:	move.w	EcTy(a2),d0
	lsr.w	#1,d0
	cmp.w	d0,d6
	bcs.s	FsApp3
	move.w	d0,d6
FsApp3:	cmp.w	d4,d6
	bne.s	FsApp1
	rts


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
	Lib_Def	PalRout
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	Buffer(a5),a1
	moveq	#0,d0
PalR1:	move.w	#$FFFF,(a1)
	btst	d0,d3
	beq.s	PalR2
	move.w	(a0),(a1)
PalR2:	addq.l	#2,a0
	addq.l	#2,a1
	addq.w	#1,d0
	cmp.w	#32,d0
	bcs.s	PalR1
	move.l	Buffer(a5),a1
	rts

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
	Lib_Def	GetRas
; - - - - - - - - - - - - -
	movem.l	a0/a1/d0/d1,-(sp)
	Rbsr	L_FreeRas
	bmi.s	GRas2
	bne.s	GRas1
	move.l	ScOnAd(a5),d0
	Rbeq	L_ScNOp
	move.l	d0,a0
	move.l	EcTPlan(a0),d0
GRas1:	move.l	d0,d1
	move.l	Buffer(a5),a1
	cmp.l	#TBuffer,d0
	bls.s	GRas2
	SyCall	MemChip
	Rbeq	L_OOfMem
	move.l	a0,RasAd(a5)
	move.l	d1,RasLong(a5)
	move.l	a0,a1
GRas2:	move.l	d1,d0
	lea	ATmpRas(a5),a0
	move.l	T_RastPort(a5),a2
	move.l	a0,12(a2)
	GfxCa5	InitTmpRas
	movem.l	(sp)+,a0/a1/d0/d1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Libere le tempras
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	FreeRas
; - - - - - - - - - - - - -
	move.l	RasAd(a5),d0
	beq.s	FriRX
	move.l	d0,a1
	move.l	RasLong(a5),d0
	SyCall	MemFree
	clr.l	RasAd(a5)
	clr.l	RasLong(a5)
	move.l	T_RastPort(a5),a1
	clr.l	12(a1)
FriRX:	tst.l	RasLock(a5)
	bne.s	FriRY
	moveq	#0,d0
	move.w	RasSize(a5),d0
	clr.w	RasSize(a5)
	tst.l	d0
	rts
* Raster locke!
FriRY:	move.l	RasLock(a5),a1
	moveq	#0,d1
	move.w	RasSize(a5),d1
	moveq	#-1,d0
	rts

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
	Lib_Def	GfxPnt
; - - - - - - - - - - - - -
; Entree speciale pour PAINT!
	movem.l	a3-a6/d6-d7,-(sp)
	move.l	T_EcVect(a5),a3
	lea	SuPaint*4(a3),a3
	Rbra	L_GfxF0
; - - - - - - - - - - - - -
	Lib_Def	GfxFunc
; - - - - - - - - - - - - -
; Appel d'une fonction GFX avec autoback!
	movem.l	a3-a6/d6-d7,-(sp)
	move.l	T_GfxBase(a5),a6
	lea	0(a6,d5.w),a3
	Rbra	L_GfxF0
; - - - - - - - - - - - - -
	Lib_Def	GfxF0
; - - - - - - - - - - - - -
	move.l	ScOnAd(a5),a2
	tst.w	EcAuto(a2)
	bne.s	GfxF1
	jsr	(a3)
	bra.s	GfxX1
* Autoback!!
GfxF1	move.l 	36(a1),a4
	moveq	#0,d7
	movem.l	d0-d7/a0-a3,-(sp)
	EcCall	AutoBack1
	movem.l	(sp),d0-d7/a0-a3
	btst	#BitDble,EcFlags(a2)
	beq.s	GfxF2
	jsr	(a3)
	EcCall	AutoBack2
	movem.l	(sp),d0-d7/a0-a3
	move.l	a4,36(a1)
	jsr	(a3)
	EcCall	AutoBack3
GfxX	movem.l	(sp)+,d0-d7/a0-a3
GfxX1	movem.l	(sp)+,a3-a6/d6-d7
	rts
* Single buffer
GfxF2	jsr	(a3)
	EcCall	AutoBack4
	bra.s	GfxX

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
	Lib_Par InUpdateOff
; - - - - - - - - - - - - -
	bclr	#5,ActuMask(a5)
	bclr	#6,ActuMask(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InUpdateOn
; - - - - - - - - - - - - -
	bset	#5,ActuMask(a5)
	bset	#6,ActuMask(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InUpdate
; - - - - - - - - - - - - -
	movem.l	a3-a6,-(sp)		* Bobs
	move.l	a5,-(sp)		*** CHANGER!!!
	SyCall	EffBob
	SyCall	ActBob
	SyCall	AffBob
	EcCall	SwapScS
	move.l	(sp)+,a5
	SyCall	ActHs			* Hard sprites
	SyCall	AffHs
	movem.l	(sp)+,a3-a6
	rts
; - - - - - - - - - - - - -
	Lib_Par InBobUpdateOff
; - - - - - - - - - - - - -
	bclr	#5,ActuMask(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InBobUpdateOn
; - - - - - - - - - - - - -
	bset	#5,ActuMask(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InBobUpdate
; - - - - - - - - - - - - -
	movem.l	a3-a6,-(sp)		* Bobs
	SyCall	EffBob
	SyCall	ActBob
	SyCall	AffBob
	EcCall	SwapScS
	movem.l	(sp)+,a3-a6
	rts
; - - - - - - - - - - - - -
	Lib_Par InSpriteUpdateOff
; - - - - - - - - - - - - -
	bclr	#6,ActuMask(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InSpriteUpdateOn
; - - - - - - - - - - - - -
	bset	#6,ActuMask(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InSpriteUpdate
; - - - - - - - - - - - - -
	movem.l	a3-a6,-(sp)
	SyCall	ActHs
	SyCall	AffHs
	movem.l	(sp)+,a3-a6
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UPDATE EVERY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InUpdateEvery
; - - - - - - - - - - - - -
	cmp.l	#65536,d3
	Rbcc	L_FonCall
	move.w	d3,VBLDelai(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOB CLEAR / DRAW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InBobClear
; - - - - - - - - - - - - -
	movem.l	a3-a6,-(sp)
	SyCall	EffBob
	movem.l	(sp)+,a3-a6
	rts
; - - - - - - - - - - - - -
	Lib_Par InBobDraw
; - - - - - - - - - - - - -
	movem.l	a3-a6,-(sp)
	SyCall	ActBob
	SyCall	AffBob
	movem.l	(sp)+,a3-a6
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LIMIT BOB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InLimitBob0
; - - - - - - - - - - - - -
	moveq	#-1,d1
	move.l	#EntNul,d2
	move.l	d2,d3
	move.l	d3,d4
	move.l	d4,d5
	SyCall	LimBob
	Rbne	L_FonCall
	rts
; - - - - - - - - - - - - -
	Lib_Par InLimitBob4
; - - - - - - - - - - - - -
	moveq	#-1,d1
	move.l	d3,d5
	move.l	(a3)+,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	SyCall	LimBob
	Rbne	L_FonCall
	rts
; - - - - - - - - - - - - -
	Lib_Par InLimitBob5
; - - - - - - - - - - - - -
	move.l	d3,d5
	move.l	(a3)+,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	SyCall	LimBob
	Rbne	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PRIORITY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InPriorityOn
; - - - - - - - - - - - - -
	moveq	#1,d1
	moveq	#-1,d2
	Rbra	L_Prooo
; - - - - - - - - - - - - -
	Lib_Par InPriorityOff
; - - - - - - - - - - - - -
	moveq	#0,d1
	moveq	#-1,d2
	Rbra	L_Prooo
; - - - - - - - - - - - - -
	Lib_Par InPriorityReverseOn
; - - - - - - - - - - - - -
	moveq	#-1,d1
	moveq	#1,d2
	Rbra	L_Prooo
; - - - - - - - - - - - - -
	Lib_Par InPriorityReverseOff
; - - - - - - - - - - - - -
	moveq	#-1,d1
	moveq	#0,d2
	Rbra	L_Prooo
; - - - - - - - - - - - - -
	Lib_Def	Prooo
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	SyCall	SPrio
	rts

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
	Lib_Par FnXBob
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	XYBob
	Rbne	L_FonCall
	move.w	d1,d3
	ext.l	d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnYBob
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	XYBob
	Rbne	L_FonCall
	move.w	d2,d3
	ext.l	d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=XY SPRITE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnXSprite
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	XYSp
	Rbne	L_FonCall
	move.w	d1,d3
	ext.l	d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnYSprite
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	XYSp
	Rbne	L_FonCall
	move.w	d2,d3
	ext.l	d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=IBOB = ISPRITE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnIBob
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	XYBob
	Rbne	L_FonCall
	ext.l	d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnISprite
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	XYSp
	Rbne	L_FonCall
	ext.l	d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					= XY MOUSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InXMouse
; - - - - - - - - - - - - -
	move.l	d3,d1
	move.l	#EntNul,d2
	SyCall	SetM
	rts
; - - - - - - - - - - - - -
	Lib_Par FnXMouse
; - - - - - - - - - - - - -
	SyCall	XyMou
	move.w	d1,d3
	ext.l	d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par InYMouse
; - - - - - - - - - - - - -
	move.l	d3,d2
	move.l	#EntNul,d1
	SyCall	SetM
	rts
; - - - - - - - - - - - - -
	Lib_Par FnYMouse
; - - - - - - - - - - - - -
	SyCall	XyMou
	move.w	d2,d3
	ext.l	d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=MOUSEKEY / CLICK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnMouseKey
; - - - - - - - - - - - - -
	SyCall	MouseKey
	moveq	#0,d3
	move.w	d1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnMouseClick
; - - - - - - - - - - - - -
	SyCall	MouRel
	moveq	#0,d3
	move.w	d1,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HIDE ON HIDE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InHide
; - - - - - - - - - - - - -
	SyCalD	Hide,0
	rts
; - - - - - - - - - - - - -
	Lib_Par InHideOn
; - - - - - - - - - - - - -
	SyCalD	Hide,-1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHOW SHOW ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InShow
; - - - - - - - - - - - - -
	SyCalD	Show,0
	rts
; - - - - - - - - - - - - -
	Lib_Par InShowOn
; - - - - - - - - - - - - -
	SyCalD	Show,-1
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LIMIT MOUSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InLimitMouse0
; - - - - - - - - - - - - -
	moveq	#0,d1
	Rbra	L_LimMX
; - - - - - - - - - - - - -
	Lib_Par InLimitMouse1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	addq.l	#1,d1
	Rbra	L_LimMX
; - - - - - - - - - - - - -
	Lib_Def	LimMX
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	SyCall	LimitMEc		??? Nb d'ecrans
	Rbne	L_EcWiErr
	rts
; - - - - - - - - - - - - -
	Lib_Par InLimitMouse4
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	SyCall	LimitM
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHANGE MOUSE n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InChangeMouse
; - - - - - - - - - - - - -
	subq.l	#1,d3
	Rbmi	L_FonCall
	move.l	d3,d1
	SyCall	ChangeM
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET BOB n,back,plane,minterm
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSetBob
; - - - - - - - - - - - - -
	Rbsr	L_SaveRegs
	move.l	d3,d7
	move.l	(a3)+,d6
	move.l	(a3)+,d5
	move.l	#EntNul,d4
	move.l	d4,d3
	move.l	d3,d2
	move.l	(a3)+,d1
	cmp.l	d4,d7
	bne.s	IStb1
	moveq	#0,d7
IStb1	cmp.l	d4,d6
	bne.s	IStb2
	moveq	#-1,d6
IStb2	cmp.l	d4,d5
	bne.s	IStb3
	moveq	#0,d5
IStb3	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	SyCall	SetBob
	Rbmi	L_OOfMem
	Rbne	L_FonCall
	Rbsr	L_LoadRegs
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOB n,x,y,a
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InBob
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	movem.l	d6-d7,-(sp)
	moveq	#0,d7
	moveq	#-1,d6
	moveq	#0,d5
	SyCall	SetBob
	movem.l	(sp)+,d6-d7
	Rbmi	L_OOfMem
	Rbne	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BOB OFF x
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InBobOff0
; - - - - - - - - - - - - -
	SyCall	OffBobS
	rts
; - - - - - - - - - - - - -
	Lib_Par InBobOff1
; - - - - - - - - - - - - -
	move.l	d3,d1
	SyCall	OffBob
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET SPRITE BUFFER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSetSpriteBuffer
; - - - - - - - - - - - - -
	move.l	d3,d1
	cmp.l	#16,d1
	Rbcs	L_FonCall
	SyCall	SBufHs
	Rbne	L_SpErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SPRITE PRIORITY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSpritePriority
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d1
	cmp.l	#4,d1
	Rbhi	L_FonCall
	SyCall	PriHs
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SPRITE n,x,y,a
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSprite
; - - - - - - - - - - - - -
	move.l	d3,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	SyCall	NXYAHs
	Rbne	L_FonCall
	bset	#6,T_Actualise(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SPRITE OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSpriteOff0
; - - - - - - - - - - - - -
	SyCall	OffHs
	bset	#6,T_Actualise(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InSpriteOff1
; - - - - - - - - - - - - -
	move.l	d3,d1
	SyCall	XOffHs
	Rbne	L_FonCall
	bset	#6,T_Actualise(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Collision hard
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSetHardcol
; - - - - - - - - - - - - -
	move.l	(a3)+,d2
	moveq	#$0F,d1
	SyCall	SetHCol
	rts
; - - - - - - - - - - - - -
	Lib_Par FnHardcol
; - - - - - - - - - - - - -
	tst.l	d3
	bmi.s	FHc1
	cmp.l	#8,d3
	Rbcc	L_FonCall
FHc1	move.w	d3,d1
	SyCall	GetHCol
	move.l	d1,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BOB SPRITE COL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnBobSpriteCol1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	moveq	#0,d2
	bset	#31,d2
	moveq	#63,d3
	SyCall	ColBob
	move.l	d0,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnBobSpriteCol3
; - - - - - - - - - - - - -
	cmp.l	#63,d3
	Rbhi	L_FonCall
	move.l	(a3)+,d2
	Rbmi	L_FonCall
	bset	#31,d2
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	SyCall	ColBob
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=BOB COL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnBobCol1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	moveq	#0,d2
	move.l	#10000,d3
	SyCall	ColBob
	move.l	d0,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnBobCol3
; - - - - - - - - - - - - -
	tst.l	d3
	Rbmi	L_FonCall
	move.l	(a3)+,d2
	Rbmi	L_FonCall
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	SyCall	ColBob
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPRITEBOBCOL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnSpriteBobCol1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	moveq	#0,d2
	bset	#31,d2
	move.l	#10000,d3
	SyCall	ColSpr
	move.l	d0,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnSpriteBobCol3
; - - - - - - - - - - - - -
	tst.l	d3
	Rbmi	L_FonCall
	move.l	(a3)+,d2
	Rbmi	L_FonCall
	bset	#31,d2
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	SyCall	ColSpr
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPRITE COL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnSpriteCol1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	moveq	#0,d2
	moveq	#63,d3
	SyCall	ColSpr
	move.l	d0,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnSpriteCol3
; - - - - - - - - - - - - -
	cmp.l	#63,d3
	Rbhi	L_FonCall
	move.l	(a3)+,d2
	Rbmi	L_FonCall
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	SyCall	ColSpr
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=COL(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnCol
; - - - - - - - - - - - - -
	move.l	d3,d1
	SyCall	ColGet
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MASK
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMakeIconMask0
; - - - - - - - - - - - - -
	moveq	#1,d1
	Rbsr	L_AdIcon
	Rbne	L_GoError
	subq.w	#1,d5
	Rbra	L_MkMa1
; - - - - - - - - - - - - -
	Lib_Par InMakeIconMask1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbsr	L_AdIcon
	Rbne	L_GoError
	moveq	#0,d5
	Rbra	L_MkMa1
; - - - - - - - - - - - - -
	Lib_Par InNoIconMask0
; - - - - - - - - - - - - -
	moveq	#1,d1
	Rbsr	L_AdIcon
	Rbne	L_GoError
	subq.w	#1,d5
	Rbra	L_NoMa1
; - - - - - - - - - - - - -
	Lib_Par InNoIconMask1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbsr	L_AdIcon
	Rbne	L_GoError
	moveq	#0,d5
	Rbra	L_NoMa1
; - - - - - - - - - - - - -
	Lib_Par InMakeMask0
; - - - - - - - - - - - - -
	moveq	#1,d1
	Rbsr	L_AdBob
	Rbne	L_GoError
	subq.w	#1,d5
	Rbra	L_MkMa1
; - - - - - - - - - - - - -
	Lib_Par InMakeMask1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbsr	L_AdBob
	Rbne	L_GoError
	moveq	#0,d5
	Rbra	L_MkMa1
; - - - - - - - - - - - - -
	Lib_Def	MkMa1
; - - - - - - - - - - - - -
.Loop	tst.l	(a2)
	beq.s	.Skip
	SyCall	MaskMk
	Rbne	L_OOfMem
.Skip	addq.l	#8,a2
	dbra	d5,.Loop
	rts

; - - - - - - - - - - - - -
	Lib_Par InNoMask0
; - - - - - - - - - - - - -
	moveq	#1,d1
	Rbsr	L_AdBob
	Rbne	L_GoError
	subq.w	#1,d5
	Rbra	L_NoMa1
; - - - - - - - - - - - - -
	Lib_Par InNoMask1
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbsr	L_AdBob
	Rbne	L_GoError
	moveq	#0,d5
	Rbra	L_NoMa1
; - - - - - - - - - - - - -
	Lib_Def	NoMa1
; - - - - - - - - - - - - -
.Loop	tst.l	(a2)
	beq.s	.Skip2
	tst.l	4(a2)
	ble.s	.Skip1
	move.l	4(a2),a1
	move.l	(a1),d0
	SyCall	MemFree
.Skip1	move.l	#$C0000000,4(a2)
.Skip2	addq.l	#8,a2
	dbra	d5,.Loop
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HOT SPOT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InHotSpot3
; - - - - - - - - - - - - -
	move.l	(a3)+,d2
	moveq	#0,d1
	Rbra	L_HotSp
; - - - - - - - - - - - - -
	Lib_Par InHotSpot2
; - - - - - - - - - - - - -
	move.l	d3,d1
	and.w	#%01110111,d1
	addq.w	#1,d1
	Rbra	L_HotSp
; - - - - - - - - - - - - -
	Lib_Def	HotSp
; - - - - - - - - - - - - -
	movem.w	d1-d3,-(sp)
	move.l	(a3)+,d1
	Rbsr	L_AdBob
	movem.w	(sp)+,d1-d3
	SyCall	SpotHot
	Rbne	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET SPRITE / BOB [sc],n,x,y to x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InGetSprite6
; - - - - - - - - - - - - -
	move.l	4*4(a3),d1
	EcCall	AdrEc
	Rbeq	L_ScNOp
	move.l	d0,-(sp)
	move.l	d0,a0
	Rbsr	L_Ritoune
	move.l	(a3),4(a3)
	addq.l	#4,a3
	Rbra	L_GS
; - - - - - - - - - - - - -
	Lib_Par InGetSprite5
; - - - - - - - - - - - - -
	move.l	ScOnAd(a5),a0
	move.l	a0,-(sp)
	Rbeq	L_ScNOp
	Rbsr	L_Ritoune
	Rbra	L_GS
; - - - - - - - - - - - - -
	Lib_Def	GS
; - - - - - - - - - - - - -
	move.l	(a3),d0
	Rble	L_FonCall
	Rjsr	L_Bnk.AdBob
	beq.s	.New
	move.l	a0,a2
	Rjsr	L_Bnk.EffBobA0
	bra.s	.Suite
; Appelle l'agrandissement (soit pas banque, soit trop grand)
.New	moveq	#1,d0			Recopier l'ancienne!
	move.l	(a3),d1			Taille nouvelle banque
	Rjsr	L_Bnk.ResBob
	Rbne	L_OOfMem
	Rjsr	L_Bnk.Change		Changement de banque
	move.l	(a3),d0			Adresse du nouveau bob
	Rjsr	L_Bnk.AdBob
	Rbeq	L_FonCall
	move.l	a0,a2
.Suite	addq.l	#4,a3
; Appelle la trappe
	move.l	(sp)+,a1		Ecran en A1
	SyCall	SprGet
	Rbne	L_OOfMem
	Rbsr	L_LoadRegs
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					GET ICON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InGetIcon6
; - - - - - - - - - - - - -
	move.l	4*4(a3),d1
	EcCall	AdrEc
	Rbeq	L_ScNOp
	move.l	d0,-(sp)
	move.l	d0,a0
	Rbsr	L_Ritoune
	move.l	(a3),4(a3)
	addq.l	#4,a3
	Rbra	L_GI
; - - - - - - - - - - - - -
	Lib_Par InGetIcon5
; - - - - - - - - - - - - -
	move.l	ScOnAd(a5),a0
	move.l	a0,-(sp)
	Rbeq	L_ScNOp
	Rbsr	L_Ritoune
	Rbra	L_GI
; - - - - - - - - - - - - -
	Lib_Def	GI
; - - - - - - - - - - - - -
	move.l	(a3),d0
	Rble	L_FonCall
	Rjsr	L_Bnk.AdIcon
	beq.s	.New
	move.l	a0,a2
	Rjsr	L_Bnk.EffBobA0
	bra.s	.Suite
; Appelle l'agrandissement (soit pas banque, soit trop grand)
.New	moveq	#1,d0			Recopier la banque
	move.l	(a3),d1			Adresse nouvel icone
	Rjsr	L_Bnk.ResIco
	Rbne	L_OOfMem
	Rjsr	L_Bnk.Change
	move.l	(a3),d0
	Rjsr	L_Bnk.AdIcon
	Rbeq	L_FonCall
	move.l	a0,a2
.Suite	addq.l	#4,a3
; Appelle la trappe
	move.l	(sp)+,a1		Ecran en A1
	SyCall	SprGet
	Rbne	L_OOfMem
	move.l	#$C0000000,4(a2)	Pas de masque!
	Rbsr	L_LoadRegs
	rts
; - - - - - - - - - - - - -
	Lib_Def	Ritoune
; - - - - - - - - - - - - -
	Rbsr	L_SaveRegs
	move.l	d3,d5
	Rbmi	L_FonCall
	move.l	(a3)+,d4
	Rbmi	L_FonCall
	move.l	(a3)+,d3
	Rbmi	L_FonCall
	move.l	(a3)+,d2
	Rbmi	L_FonCall
* Calcule taille
	cmp.w	EcTx(a0),d4
	Rbhi	L_FonCall
	move.w	d4,d6
	cmp.w	EcTy(a0),d5
	Rbhi	L_FonCall
	sub.w	d2,d4
	Rbls	L_FonCall
	sub.w	d3,d5
	Rbls	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PUT BOB n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InPutBob
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	SyCall	PutBob
	Rbne	L_FonCall
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					FONCTIONS RETOURNEMENT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnHRev
; - - - - - - - - - - - - -
	bset	#15,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnVRev
; - - - - - - - - - - - - -
	bset	#14,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnRev
; - - - - - - - - - - - - -
	bset	#15,d3
	bset	#14,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PASTE BOB x,y,n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InPasteBob
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	Rbsr	L_AdBob
	tst.l	(a2)
	Rbne	L_Paste
	moveq	#24,d0			Paste bob error
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Par InPasteIcon
; - - - - - - - - - - - - -
	move.l	d3,d1
	Rbmi	L_FonCall
	Rbsr	L_AdIcon
	tst.l	(a2)
	beq.s	.Err
	tst.l	4(a2)
	Rbne	L_Paste
	move.l	#$C0000000,4(a2)
	Rbra	L_Paste
.Err	moveq	#30,d0
	Rbra	L_EcWiErr
; - - - - - - - - - - - - -
	Lib_Def	Paste
; - - - - - - - - - - - - -
	move.l	d3,d1
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	BufBob(a5),a1
	moveq	#0,d4
	moveq	#-1,d5
	SyCall	Patch
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=SPRITE / ICON BASE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnSpriteBase
; - - - - - - - - - - - - -
	Rlea	L_AdBob,0
	Rbra	L_Sb
; - - - - - - - - - - - - -
	Lib_Par FnIconBase
; - - - - - - - - - - - - -
	Rlea	L_AdIcon,0
	Rbra	L_Sb
; - - - - - - - - - - - - -
	Lib_Def	Sb
; - - - - - - - - - - - - -
	move.l	d3,d1
	bpl.s	FsBi1
	neg.l	d1
FsBi1	jsr	(a0)
	tst.l	d3
	bpl.s	FsBi2
	addq.l	#4,a2
FsBi2	move.l	(a2),d3
	Ret_Int

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
	Lib_Par InWindopen5
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	moveq	#0,d3
	move.l	d3,-(a3)
	Rbra	L_InWindopen7
; - - - - - - - - - - - - -
	Lib_Par InWindopen6
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	moveq	#0,d3
	Rbra	L_InWindopen7
; - - - - - - - - - - - - -
	Lib_Par InWindopen7
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	Rbsr	L_SaveRegs
	move.l	d3,a1
	move.l	(a3)+,d7
	moveq	#1,d6			Faire un CLW!
	move.l	(a3)+,d5
	move.l	(a3)+,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	cmp.l	#65536,d1
	Rbcc	L_WFonCall
	WiCall	WindOp
	Rbne	L_EcWiErr
	Rbsr	L_LoadRegs
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDSAVE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InWindsave
; - - - - - - - - - - - - -
	move.l	ScOnAd(a5),d0
	Rbeq	L_ScNOp
	move.l	d0,a0
	move.w	#-1,EcWiDec(a0)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDMODE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InWindmove
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d2
	Rbmi	L_FonCall
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	WiCall	MoveWi
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDSIZE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InWindsize
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d2
	Rbmi	L_FonCall
	move.l	(a3)+,d1
	Rbmi	L_FonCall
	WiCall	SizeWi
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDCLOSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InWindclose
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	WiCall	WinDel
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WINDOW n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InWindow
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d1
	WiCall	QWindow
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=WINDON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnWindon
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	WiCall	GAdr
	move.l	d1,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					WRITING
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InWriting1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	moveq	#0,d3
	Rbra	L_InWriting2
; - - - - - - - - - - - - -
	Lib_Par InWriting2
; - - - - - - - - - - - - -
	move.l	d3,d0
	cmp.l	#3,d0
	Rbcc	L_WFonCall
	lsl.w	#3,d0
	move.l	(a3)+,d1
	cmp.l	#5,d1
	Rbcc	L_WFonCall
	or.w	d1,d0
	add.b	#"0",d0
	lea	ChWrt(pc),a1
	move.b	d0,2(a1)
	Rbra	L_GoWn
ChWrt:	dc.b 	27,"W0",0


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					BORDER n,paper,n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InBorder
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	WiCall	SBord
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TITLE TOP a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InTitleTop
; - - - - - - - - - - - - -
	move.l	d3,a2
	move.w	(a2)+,d2
	Rbsr	L_ChVerBuf
	move.l	Buffer(a5),d1
	moveq	#0,d2
	Rbra	L_WnTT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					TITLE BOTTOM
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InTitleBottom
; - - - - - - - - - - - - -
	move.l	d3,a2
	move.w	(a2)+,d2
	Rbsr	L_ChVerBuf
	move.l	Buffer(a5),d2
	moveq	#0,d1
	Rbra	L_WnTT
; - - - - - - - - - - - - -
	Lib_Def	WnTT
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	WiCall	STitle
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=XYCURS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnXCurs
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	WiCall	XYCuWi
	move.l	d1,d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnYCurs
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	WiCall	XYCuWi
	move.l	d2,d3
	Ret_Int


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET CURS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSetCurs
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	Buffer(a5),a1
	lea	8(a1),a0
	moveq	#7,d0
WnSCu1:	move.l	(a3)+,d1
	move.b	d1,-(a0)
	dbra	d0,WnSCu1
	WiCall	SCurWi
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					LOCATE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int InLocate		src/lib/windows.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CENTRE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCentre
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,a2
	move.w	(a2)+,d2
	Rbsr	L_ChVerBuf
	move.l	Buffer(a5),a1
	WiCall	Centre
	Rbne	L_EcWiErr
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CMOVE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCmove
; - - - - - - - - - - - - -
	move.l	d3,d0
	cmp.l	#EntNul,d0
	bne.s	WnCm2
WnCm1:	moveq	#0,d0
WnCm2:	move.l	(a3)+,d1
	cmp.l	#EntNul,d1
	bne.s	WnCm4
WnCm3:	moveq	#0,d1
WnCm4:	add.l	#128,d0
	add.l	#128,d1
	cmp.l	#255,d0
	Rbhi	L_WFonCall
	cmp.l	#255,d1
	Rbhi	L_WFonCall
	lea	ChCMv(pc),a1
	move.b	d1,2(a1)
	move.b	d0,5(a1)
	Rbra	L_GoWn
ChCMv:	dc.b 27,"N0",27,"O0",0
	even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CURSPEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCursPen
; - - - - - - - - - - - - -
	lea	ChCPe(pc),a1
	Rbra	L_WnPp
ChCPe	dc.b 27,"D0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PAPER
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InPaper
; - - - - - - - - - - - - -
	lea	ChPap(pc),a1
	Rbra	L_WnPp
ChPap	dc.b 27,"B0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					PEN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InPen
; - - - - - - - - - - - - -
	lea	ChPen(pc),a1
	Rbra	L_WnPp
ChPen:	dc.b 27,"P0",0
; - - - - - - - - - - - - -
	Lib_Def	WnPp
; - - - - - - - - - - - - -
	add.b	#"0",d3
	move.b	d3,2(a1)
	Rbra	L_GoWn


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLW
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InClw
; - - - - - - - - - - - - -
	lea	ChClw(pc),a1
	Rbra	L_GoWn
ChClw:	dc.b 	25,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HOME
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InHome
; - - - - - - - - - - - - -
	lea	ChHom(pc),a1
	Rbra	L_GoWn
ChHom:	dc.b 	12,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEFT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCleft
; - - - - - - - - - - - - -
	lea	ChCLf(pc),a1
	Rbra	L_GoWn
ChCLf:	dc.b 	29,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CRIGHT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCright
; - - - - - - - - - - - - -
	lea	ChCRt(pc),a1
	Rbra	L_GoWn
ChCRt:	dc.b 	28,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CUP
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCup
; - - - - - - - - - - - - -
	lea	ChCUp(pc),a1
	Rbra	L_GoWn
ChCUp:	dc.b 	30,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CDOWN
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCdown
; - - - - - - - - - - - - -
	lea	ChCDn(pc),a1
	Rbra	L_GoWn
ChCDn:	dc.b 	31,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CURSOFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCursOff
; - - - - - - - - - - - - -
	lea	ChCOf(pc),a1
	Rbra	L_GoWn
ChCOf:	dc.b 	27,"C0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CURSON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCursOn
; - - - - - - - - - - - - -
	lea	ChCOn(pc),a1
	Rbra	L_GoWn
ChCOn:	dc.b 	27,"C1",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INVERSE ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InInverseOn
; - - - - - - - - - - - - -
	lea	ChIOn(pc),a1
	Rbra	L_GoWn
ChIOn:	dc.b 	27,"I1",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					INVERSE OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InInverseOff
; - - - - - - - - - - - - -
	lea	ChIOf(pc),a1
	Rbra	L_GoWn
ChIOf:	dc.b 	27,"I0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UNDER ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InUnderOn
; - - - - - - - - - - - - -
	lea	ChUOn(pc),a1
	Rbra	L_GoWn
ChUOn:	dc.b 	27,"U1",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UNDER OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InUnderOff
; - - - - - - - - - - - - -
	lea	ChUOf(pc),a1
	Rbra	L_GoWn
ChUOf:	dc.b 	27,"U0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCROLLING ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InScrollOn
; - - - - - - - - - - - - -
	lea	ChScOn(pc),a1
	Rbra	L_GoWn
ChScOn:	dc.b 	27,"V1",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SCROLLING OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InScrollOff
; - - - - - - - - - - - - -
	lea	ChScOf(pc),a1
	Rbra	L_GoWn
ChScOf:	dc.b 	27,"V0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHADE ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InShadeOn
; - - - - - - - - - - - - -
	lea	ChSOn(pc),a1
	Rbra	L_GoWn
ChSOn:	dc.b 	27,"S1",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SHADE OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InShadeOff
; - - - - - - - - - - - - -
	lea	ChSOf(pc),a1
	Rbra	L_GoWn
ChSOf:	dc.b 	27,"S0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEAR LINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCline0
; - - - - - - - - - - - - -
	lea	ChCll(pc),a1
	Rbra	L_GoWn
ChCll:	dc.b 26,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MEMORIZE X
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMemorizeX
; - - - - - - - - - - - - -
	lea	ChMx1(pc),a1
	Rbra	L_GoWn
ChMx1:	dc.b 	27,"M0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					MEMORIZE Y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMemorizeY
; - - - - - - - - - - - - -
	lea	ChMy1(pc),a1
	Rbra	L_GoWn
ChMy1:	dc.b 	27,"M2",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REMEMBER X
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InRememberX
; - - - - - - - - - - - - -
	lea	ChMx2(pc),a1
	Rbra	L_GoWn
ChMx2:	dc.b 	27,"M1",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					REMEMBER Y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InRememberY
; - - - - - - - - - - - - -
	lea	ChMy2(pc),a1
	Rbra	L_GoWn
ChMy2:	dc.b 	27,"M3",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CLEAR LINE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InCline1
; - - - - - - - - - - - - -
	tst.l	d3
	Rbeq	L_WFonCall
	cmp.l	#255-48,d3
	Rbcc	L_WFonCall
	add.w	#48,d3
	lea	ChCln(pc),a1
	move.b	d3,2(a1)
	Rbra	L_GoWn
ChCln:	dc.b 	27,"Q0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					HSCROLL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InHScroll
; - - - - - - - - - - - - -
	lea	ChHSc(pc),a1
	Rbra	L_HVSc
ChHSc:	dc.b 	16,0,17,0,18,0,19,0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					VSCROLL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InVScroll
; - - - - - - - - - - - - -
	lea	ChVSc(pc),a1
	Rbra	L_HVSc
ChVSc:	dc.b 20,0,21,0,22,0,23,0
; - - - - - - - - - - - - -
	Lib_Def	HVSc
; - - - - - - - - - - - - -
	cmp.l	#4,d3
	Rbhi	L_WFonCall
Hv1:	subq.l	#1,d3
	Rbmi	L_WFonCall
	beq.s	Hv3
Hv2:	tst.b	(a1)+
	bne.s	Hv2
	bra.s	Hv1
Hv3	Rbra	L_GoWn
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					SET TAB
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InSetTab
; - - - - - - - - - - - - -
	cmp.l	#255-48,d3
	Rbhi	L_WFonCall
	add.w	#48,d3
	lea	ChSTa(pc),a1
	move.b	d3,2(a1)
	Rbra	L_GoWn
ChSTa:	dc.b 27,"T0",0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Envoie a la trappe
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	GoWn
; - - - - - - - - - - - - -
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	WiCall	Print
	Rbne	L_EcWiErr
	rts


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
	Lib_Def	Dia_WarmInit
; - - - - - - - - - - - - -
	clr.l	IDia_BankPuzzle(a5)
	clr.l	IDia_Error(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG OPEN channel,programmes[,variable,buffer]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InDialogOpen2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#16,-(a3)
	move.l	#1024,d3
	Rbra	L_InDialogOpen4
; - - - - - - - - - - - - -
	Lib_Par InDialogOpen3
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#1024,d3
	Rbra	L_InDialogOpen4
; - - - - - - - - - - - - -
	Lib_Par InDialogOpen4
; - - - - - - - - - - - - -
; Adresse de la banque puzzle
	Rbsr	L_Dia_GetPuzzle
	move.l	d0,d5
; Parametres
	move.l	d3,d1			Taille du buffer
	cmp.l	#256,d1
	Rbls	L_FonCall
	move.l	(a3)+,d2		Nombre de variables
	Rbmi	L_FonCall
; Chaine ou numero?
	moveq	#1,d3			Recopier la chaine
	move.l	(a3)+,d0
	cmp.l	#1024,d0
	bcc.s	.Ch
; Numero de programme dans la banque de ressource
	cmp.w	d5,d0			Programme defini?
	Rbhi	L_FonCall
	subq.w	#1,d0
	Rbmi	L_FonCall
	lsl.w	#2,d0
	add.l	2(a0,d0.w),a0		Pointe le programme
	moveq	#0,d3			Ne plus recopier la chaine
	bra.s	.Suite
; Numero du canal
.Ch	move.l	d0,a0
.Suite	move.l	(a3)+,d0
	Rble	L_FonCall
; Appelle l'initialisation
	clr.l	IDia_Error(a5)
	Rjsr	L_Dia_OpenChannel
	tst.w	d0
	bne.s	.Err
	rts
; Une erreur
.Err	move.l	d1,IDia_Error(a5)
	Rbra	L_Dia_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Branchement aux erreurs IO
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GoError
; - - - - - - - - - - - - -
	add.w	#IDia_Errors,d0
	Rjmp	L_Error

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	=Edialog: position de la derniere erreur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnEDialog
; - - - - - - - - - - - - -
	move.l	IDia_Error(a5),d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG CLOSE [n]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InDialogClose0
; - - - - - - - - - - - - -
	Rjsr	L_Dia_CloseChannels
	rts
; - - - - - - - - - - - - -
	Lib_Par InDialogClose1
; - - - - - - - - - - - - -
	move.l	d3,d0
	Rble	L_FonCall
	Rjsr	L_Dia_CloseChannel
	Rbne	L_Dia_GoError
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG CLR c
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InDialogClr
; - - - - - - - - - - - - -
	move.l	d3,d0
	Rble	L_FonCall
	Rjsr	L_Dia_EffChannel
	Rbne	L_Dia_GoError
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG FREEZE [c]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InDialogFreeze0
; - - - - - - - - - - - - -
	moveq	#-1,d1
	Rjsr	L_Dia_FreezeChannels
	rts
; - - - - - - - - - - - - -
	Lib_Par InDialogFreeze1
; - - - - - - - - - - - - -
	move.l	d3,d0
	Rble	L_FonCall
	moveq	#-1,d1
	Rjsr	L_Dia_FreezeChannel
	Rbne	L_Dia_GoError
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG UNFREEZE [c]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InDialogUnFreeze0
; - - - - - - - - - - - - -
	moveq	#0,d1
	Rjsr	L_Dia_FreezeChannels
	rts
; - - - - - - - - - - - - -
	Lib_Par InDialogUnFreeze1
; - - - - - - - - - - - - -
	move.l	d3,d0
	Rble	L_FonCall
	moveq	#0,d1
	Rjsr	L_Dia_FreezeChannel
	Rbne	L_Dia_GoError
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		DIALOG UPDATE n,z,[value]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InDialogUpdate2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	move.l	d3,-(a3)
	move.l	d3,-(a3)
	Rbra	L_InDialogUpdate5
; - - - - - - - - - - - - -
	Lib_Par InDialogUpdate3
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	move.l	d3,-(a3)
	Rbra	L_InDialogUpdate5
; - - - - - - - - - - - - -
	Lib_Par InDialogUpdate4
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	Rbra	L_InDialogUpdate5
; - - - - - - - - - - - - -
	Lib_Par InDialogUpdate5
; - - - - - - - - - - - - -
	move.l	d3,d4
	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	Rble	L_FonCall
	Rjsr	L_Dia_Update
	Rbne	L_Dia_GoError
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=DIALOG RUN( channel[,label][,x,y] )
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnDialogRun1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#-1,-(a3)
	move.l	#EntNul,d3
	move.l	d3,-(a3)
	Rbra	L_FnDialogRun4
; - - - - - - - - - - - - -
	Lib_Par FnDialogRun2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#EntNul,d3
	move.l	d3,-(a3)
	Rbra	L_FnDialogRun4
; - - - - - - - - - - - - -
	Lib_Par FnDialogRun4
; - - - - - - - - - - - - -
;	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	cmp.l	#65536,d1
	Rbge	L_FonCall
	move.l	(a3)+,d0
	Rble	L_FonCall
	tst.l	Patch_Errors(a5)		Monitor present?
	Rbne	L_FonCall
	clr.l	IDia_Error(a5)
	Rjsr	L_Dia_RunProgram
	tst.w	d0
	bne.s	.Err
	move.l	d1,d3
	Ret_Int
.Err	move.l	d1,IDia_Error(a5)
	Rbra	L_Dia_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=DIALOG(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnDialog
; - - - - - - - - - - - - -
	move.l	d3,d0
	Rble	L_FonCall
	Rjsr	L_Dia_GetReturn
	Rbne	L_Dia_GoError
	move.l	d1,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=VDIALOG(n,n)=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InVDialog
; - - - - - - - - - - - - -
;	move.l	(a3)+,d3
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	Rjsr	L_Dia_GetVariable
	Rbne	L_Dia_GoError
	move.l	d3,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par FnVDialog
; - - - - - - - - - - - - -
	move.l	d3,d1
	move.l	(a3)+,d0
	Rjsr	L_Dia_GetVariable
	Rbne	L_Dia_GoError
	move.l	(a0),d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=VDIALOG$()=
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InVDialogD
; - - - - - - - - - - - - -
	Rbra	L_InVDialog
; - - - - - - - - - - - - -
	Lib_Par FnVDialogD
; - - - - - - - - - - - - -
	move.l	d3,d1
	move.l	(a3)+,d0
	Rjsr	L_Dia_GetVariable
	Rbne	L_Dia_GoError
	move.l	(a0),d3
	Ret_String

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=RDIALOG(n,n[,n])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnRDialog2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	moveq	#1,d3
	Rbra	L_FnRDialog3
; - - - - - - - - - - - - -
	Lib_Par FnRDialog3
; - - - - - - - - - - - - -
	move.l	d3,d2
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	Rjsr	L_Dia_GetValue
	Rbne	L_Dia_GoError
	tst.b	d2
	beq.s	.Nul
	moveq	#0,d1
	moveq	#0,d2
.Nul	move.l	d1,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					=RDIALOG$()
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnRDialogD2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	moveq	#1,d3
	Rbra	L_FnRDialogD3
; - - - - - - - - - - - - -
	Lib_Par FnRDialogD3
; - - - - - - - - - - - - -
	move.l	d3,d2
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	Rjsr	L_Dia_GetValue
	Rbne	L_Dia_GoError
	tst.b	d2
	Rbeq	L_Ret_ChVide
	move.l	d1,a2
	Rbra	L_Str2Chaine

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=ZDIALOG(channel,x,y)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par	FnZDialog
; - - - - - - - - - - - - -
	move.l	d3,d2
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	Rjsr	L_Dia_GetZone
	Rbne	L_Dia_GoError
	move.l	d1,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=DIALOG BOX(a$[,v][,v$][,x,y])
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnDialogBox1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)		a$
	move.l	#EntNul,d3		y
	move.l	d3,-(a3)		v
	move.l	ChVide(a5),-(a3)	v$
	move.l	d3,-(a3)		x
	Rbra	L_FnDialogBox5
; - - - - - - - - - - - - -
	Lib_Par FnDialogBox2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)		v
	move.l	#EntNul,d3		y
	move.l	ChVide(a5),-(a3)	v$
	move.l	d3,-(a3)		x
	Rbra	L_FnDialogBox5
; - - - - - - - - - - - - -
	Lib_Par FnDialogBox3
; - - - - - - - - - - - - -
	move.l	d3,-(a3)		v$
	move.l	#EntNul,d3		y
	move.l	d3,-(a3)		x
	Rbra	L_FnDialogBox5
; - - - - - - - - - - - - -
	Lib_Par FnDialogBox5
; - - - - - - - - - - - - -
	tst.l	Patch_Errors(a5)		Monitor present?
	Rbne	L_FonCall
	Rbsr	L_SaveRegs
	Rbsr	L_Dia_GetPuzzle
	move.l	d0,d5			Nombre de programmes
	move.l	d3,d1			Y
	move.l	(a3)+,d0		X
	move.l	(a3)+,d3		Variable $
	move.l	(a3)+,d2		Variable
	move.l	(a3)+,d4		Programme
	cmp.l	#1024,d4		Une chaine?
	bcc.s	.Chaine
	cmp.w	d5,d4			Programme defini?
	Rbhi	L_FonCall
	subq.w	#1,d4
	Rbmi	L_FonCall
	lsl.w	#2,d4
	add.l	2(a0,d4.w),a0
	moveq	#0,d5			Ne pas copier le programme
	bra.s	.Suite
.Chaine	move.l	d4,a0
	moveq	#1,d5			Copier le programme
.Suite	moveq	#0,d4			Rien!
	move.l	#1024,d6		Buffer
	Rjsr	L_Dia_RunQuick
	tst.w	d0
	bne.s	.Err
	move.l	d1,d3
	Rbsr	L_LoadRegs
	Ret_Int
.Err	move.l	d1,IDia_Error(a5)
	Rbra	L_Dia_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		READ TEXT a$
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InReadText1
; - - - - - - - - - - - - -
; Nom de fichier
	move.l	d3,a2
	move.l	#256,d3			256 pour Param$
	Rjsr	L_Demande
	Rjsr	L_NomDisc
; Titre: AMOS Ascii Reader
	moveq	#20,d0
	Rjsr	L_Def_GetMessage
	subq.l	#2,a0
	move.l	a0,-(a3)
; Ouvre le fichier
	move.l	#1005,d2
	Rjsr	L_D_Open
	bne.s	*+8
	Rjmp	L_DiskError
; Trouve la taille
	moveq	#0,d2
	moveq	#1,d3
	Rjsr	L_D_Seek
	moveq	#0,d2
	moveq	#-1,d3
	Rjsr	L_D_Seek
	move.l	d0,d3
	addq.l	#4,d0
	Rjsr	L_ResTempBuffer
	move.l	a0,d2
; Charge le fichier
	Rjsr	L_D_Read
	beq.s	*+8
	Rjmp	L_DiskError
	Rjsr	L_D_Close
; Pousse l'adresse
	move.l	TempBuffer(a5),-(a3)
	Rbra	L_IRText
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trois parametres: titre,adresse,longueur
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InReadText3
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	#256,d3			256 pour Param$
	Rjsr	L_Demande
	move.l	(a3)+,d3
	Rbra	L_IRText
; - - - - - - - - - - - - -
	Lib_Def	IRText
; - - - - - - - - - - - - -
	Rbsr	L_SaveRegs
; De l'hypertexte?
; ~~~~~~~~~~~~~~~~
	moveq	#0,d0
	lsr.l	#4,d3			moyenne 16 par ligne
	lsl.l	#2,d3			* 4
	add.l	#2048,d3		Securite
	move.l	(a3),a0
	cmp.l	#"#HYP",(a0)
	bne.s	.Skip
	move.b	4(a0),d0
	sub.b	#"0",d0
	bcs.s	.Skip
	beq.s	.Skip
	cmp.b	#9,d0
	bhi.s	.Skip
	add.l	#2048,d3
	addq.l	#8,(a3)
.Skip	move.l	d0,-(a3)
	move.l	d3,-(a3)
; Stocke le numero de l'ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#-1,d0
	move.l	T_EcCourant(a5),d1
	beq.s	.Skup
	move.l	d1,a0
	move.w	EcNumber(a0),d0
.Skup	move.w	d0,TRd_OldEc(a5)
; Ouverture de l'ecran
; ~~~~~~~~~~~~~~~~~~~~
	Rbsr	L_Dia_GetDefault
	move.l	a1,a0
	moveq	#EcFsel,d0
	move.w	PI_RtSx(a5),d1
	move.w	PI_RtSy(a5),d2
	ext.l	d1
	ext.l	d2
	moveq	#0,d3
	moveq	#-1,d4
	Rjsr	L_Dia_RScOpen
	Rbne	L_EcWiErr
;	move.l	a0,TRd_AdEc(a5)
; Trouve le bon numero de canal dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	movem.l	d0/d1/a0/a1,-(sp)
	move.l	#65536,d7
.Find	move.l	d7,d0
	Rjsr	L_Dia_GetChannel
	beq.s	.Ok
	addq.l	#1,d7
	bra.s	.Find
.Ok	movem.l	(sp)+,d0/d1/a0/a1
; Appel du DBL
; ~~~~~~~~~~~~
	Rbsr	L_Dia_GetDefault
	add.l	2+0(a0),a0		Programme numero 1
	move.l	d7,d0
	move.l	(a3)+,d1		Longueur du buffer
	moveq	#8,d2			Variables internes
	moveq	#0,d3			Pas de recopie
	Rjsr	L_Dia_OpenChannel
	bne	.OutM
	move.l	(a3)+,8(a1)		Variable 2 = Hypertexte
	move.l	(a3)+,0(a1)		Variable 0 = Base texte
	move.l	(a3)+,4(a1)		Variable 1 = Titre
; Demarre le programme
; ~~~~~~~~~~~~~~~~~~~~
	move.l	d7,d0
	moveq	#-1,d1
	moveq	#0,d2
	moveq	#0,d3
	Rjsr	L_Dia_RunProgram
	tst.l	d0
	bne	.OutM
; Ouverture de l'ecran
; ~~~~~~~~~~~~~~~~~~~~
	move.l	d7,-(sp)
	move.l	T_EcAdr+EcFsel*4(a5),a2
	move.w	PI_RtSpeed(a5),d7
	moveq	#1,d6
	move.w	PI_RtWy(a5),d5
	move.w	PI_RtSy(a5),d0
	lsr.w	#1,d0
	add.w	d0,d5
	Rjsr	L_AppCentre
	move.l	(sp)+,d7
; Animation de l'ecran, uniquement READ TEXT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	ChVide(a5),ParamC(a5)
.Loop	move.l	d7,-(sp)
	Rjsr	L_Sys_WaitMul
	Rjsr	L_Test_PaSaut
	move.l	(sp)+,d7
	move.l	d7,d0
	moveq	#5,d1
	moveq	#0,d2
	Rjsr	L_Dia_GetValue
	cmp.b	#2,d2
	beq.s	.Copy
.Back	move.l	d7,d0
	Rjsr	L_Dia_GetReturn
	bne.s	.Quit
	tst.l	d1
	bpl.s	.Loop
	bra.s	.Quit
; Copie la chaine >>> Param$
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
.Copy	move.l	d1,a2
	tst.b	(a2)
	beq.s	.Back
	move.l 	#128,d3
	Rjsr	L_Demande
	clr.w	(a0)+
.Cop	move.b	(a2)+,(a0)+
	bne.s	.Cop
	move.l	a0,d0
	and.l	#$FFFFFFFE,d0
	move.l	d0,HiChaine(a5)
	sub.l	a1,a0
	subq.l	#3,a0
	move.w	a0,(a1)
	move.l	a1,ParamC(a5)
	moveq	#0,d0
	bra.s	.Quit
; Fermeture du canal
; ~~~~~~~~~~~~~~~~~~
.OutM	moveq	#1,d0
.Quit	move.l	d0,-(sp)
	move.l	d7,d0
	Rjsr	L_Dia_CloseChannel
; Fermeture de l'ecran
; ~~~~~~~~~~~~~~~~~~~~
	move.l	T_EcAdr+EcFsel*4(a5),a2
	move.w	PI_RtSpeed(a5),d7
	neg.w	d7
	move.w	EcTy(a2),d6
	lsr.w	#1,d6
	move.w	EcAWY(a2),d5
	add.w	d6,d5
	Rjsr	L_AppCentre
	EcCalD	Del,EcFsel
	move.w	TRd_OldEc(a5),d1
	bmi.s	.NoEc
	EcCall	Active
.NoEc
; Ok!
; ~~~
	moveq	#0,d0
	Rjsr	L_ResTempBuffer
	move.l	(sp)+,d0
	Rbne	L_OOfMem
	Rbsr	L_LoadRegs
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		RESOURCE SCREEN OPEN n,sx,sy,flash
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InResourceScreenOpen
; - - - - - - - - - - - - -
	Rbsr	L_Dia_GetPuzzle
	move.l	a1,a0
	moveq	#-1,d4			Interlaced comme la banque
;	move.l	(a3)+,d3
	move.l	(a3)+,d2
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	cmp.l	#8,d0
	Rbcc	L_IllScN
	Rjsr	L_Dia_RScOpen
	Rbne	L_EcWiErr
	move.l	a0,ScOnAd(a5)
	move.w	EcNumber(a0),ScOn(a5)
	addq.w	#1,ScOn(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		RESOURCE BANK n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InResourceBank
; - - - - - - - - - - - - -
	tst.l	d3
	Rbmi	L_FonCall
	move.l	d3,IDia_BankPuzzle(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retourne l'adresse de la banque PUZZLE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetPuzzle
; - - - - - - - - - - - - -
	move.l	IDia_BankPuzzle(a5),d0
	Rbeq	L_Dia_GetDefault
; Une banque normale
	move.l	a3,-(sp)
	Rjsr	L_Bnk.GetAdr
	Rbeq	L_BkNoRes
	cmp.l	#"Reso",-8(a0)
	Rbne	L_BkNoRes
	move.l	a0,a3
	Rbsr	L_Dia_GetDefault
; Les graphiques
	cmp.w	#1,(a3)
	bcs.s	.PaG
	move.l	2+0(a3),d0
	beq.s	.PaG
	lea	0(a3,d0.l),a1
; Les messages
.PaG	cmp.w	#2,(a3)
	bcs.s	.PaB
	move.l	2+4(a3),d0
	beq.s	.PaB
	lea	0(a3,d0.l),a2
; Les Programmes
.PaB	moveq	#0,d0			Pas de programmes par defaut
	cmp.w	#3,(a3)
	bcs.s	.PaP
	move.l	2+8(a3),d0
	beq.s	.PaP
	lea	0(a3,d0.l),a0		Adresse des programmes
	move.w	(a0),d0			Nombre de programmes par defaut
; Ok!
.PaP	move.l	(sp)+,a3
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retourne la banque par defaut
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetDefault
; - - - - - - - - - - - - -
	move.l	Sys_Resource(a5),d0
	Rbeq	L_BkNoRes
	move.l	d0,a0
	move.l	a0,a1
	add.l	2+0(a0),a1		Base des graphiques
	move.l	a0,a2
	add.l	2+4(a0),a2		Base des messages
	add.l	2+8(a0),a0		Base des programmes
	moveq	#0,d0			Programmes par defaut
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	RESOURCE UNPACK n,x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InResourceUnpack
; - - - - - - - - - - - - -
	Rbsr	L_Dia_GetPuzzle
	move.l	a1,a0
	move.l	d3,d2
	move.l	(a3)+,d1
	move.l	(a3)+,d0
	Rble	L_FonCall
	cmp.w	(a0),d0
	Rbhi	L_FonCall
	lsl.w	#2,d0
	move.l	-4+2(a0,d0.w),d0
	add.l	d0,a0
	move.l	ScOnAd(a5),d0
	Rbeq	L_ScNOp
	move.l	d0,a1
	Rjsr	L_UnPack_Bitmap
	Rbeq	L_FonCall
	rts

; __________________________________________________________________________
;
; 	AREXX interface!

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=AREXX EXISTS("port")
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnArexxExist
; - - - - - - - - - - - - -
	move.l	d3,a2
	move.w	(a2)+,d2
	Rbsr	L_ChVerBuf
	move.l	Buffer(a5),a0
	moveq	#0,d0
	Rbsr	L_Arx_RegisterPort
	move.l	d0,d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		AREXX OPEN "PORT_NAME"
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InArexxOpen
; - - - - - - - - - - - - -
; Chaine port-name
; ~~~~~~~~~~~~~~~~
	move.l	d3,a2
	move.w	(a2)+,d2
	lea	Arx_PortName(a5),a0
	cmp.w	#32,d2
	Rbcc	L_StooLong
	subq.w	#1,d2
	bmi.s	.Skip1
.Loop1	move.b	(a2)+,(a0)+
	cmp.b	#" ",-1(a0)
	Rble	L_FonCall
	dbra	d2,.Loop1
.Skip1	clr.b	(a0)
; Va ouvrir
; ~~~~~~~~~
	Rbsr	L_Arx_Open
	Rbne	L_GoError
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		=AREXX
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnArexx
; - - - - - - - - - - - - -
	Rbsr	L_Arx_Message
	move.l	d0,d3
	beq.s	.Skip
	moveq	#1,d3
	move.l	Arx_Answer(a5),a0
	move.l	rm_Action(a0),d0
	and.l	#RXFF_RESULT,d0
	beq.s	.Skip
	moveq	#2,d3
.Skip	Ret_Int


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		AREXX WAIT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InArexxWait
; - - - - - - - - - - - - -
.Loop	Rjsr	L_Sys_WaitMul
	Rjsr	L_Test_Normal
	Rbsr	L_Arx_Message
	beq.s	.Loop
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		AREXX CLOSE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InArexxClose
; - - - - - - - - - - - - -
	tst.l	Arx_Answer(a5)
	bne.s	.Err
	Rbsr	L_Arx_Close
	rts
.Err	move.w	#198,d0
	Rbra	L_GoError

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		=AREXX$(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnArexxD
; - - - - - - - - - - - - -
	cmp.l	#16,d3
	Rbcc	L_FonCall
	move.l	Arx_Answer(a5),d0
	Rbeq	L_Ret_ChVide
	move.l	d0,a0
	lsl	#2,d3
	move.l	rm_Args(a0,d3.w),d0
	Rbeq	L_Ret_ChVide
	move.l	d0,a2
	move.w	ra_Length-ra_Buff(a2),d2
	move.w	d2,d3
	Rbeq	L_Ret_ChVide
	Rjsr	L_Demande
	move.w	d3,(a0)+
.Copy	move.b	(a2)+,(a0)+
	subq.w	#1,d2
	bne.s	.Copy
	moveq	#1,d0
	add.l	a0,d0
	and.w	#$FFFE,d0
	move.l	d0,HiChaine(a5)
	move.l	a1,d3
	Ret_String


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		AREXX ANSWER error [,"answer"]
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InArexxAnswer1
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	ChVide(a5),d3
	Rbra	L_InArexxAnswer2
; - - - - - - - - - - - - -
	Lib_Par InArexxAnswer2
; - - - - - - - - - - - - -
	move.l	d3,-(a3)
	move.l	Arx_Answer(a5),d0
	beq.s	.Err1
	move.l	d0,a2
; Une r�ponse?
	move.l	rm_Action(a2),d0
	and.l	#RXFF_RESULT,d0
	beq.s	.NoResult
	move.l	(a3),a0
	moveq	#0,d0
	move.w	(a0)+,d0
	move.l	a6,-(sp)
	move.l	Arx_Base(a5),a6
	jsr	_LVOCreateArgstring(a6)
	move.l	(sp)+,a6
	tst.l	d0
	beq.s	.Err2
	move.l	d0,rm_Result2(a2)
.NoResult
; Le code de retour
	move.l	4(a3),rm_Result1(a2)
; Renvoie le message
	addq.l	#8,a3
	move.l	a2,a1
	move.l	a6,-(sp)
	move.l	4.w,a6
	jsr	_LVOReplyMsg(a6)
	move.l	(sp)+,a6
	clr.l	Arx_Answer(a5)
	rts
; Not defined
.Err1	move.w	#197,d0
	Rbra	L_GoError
; Out of mem
.Err2	moveq	#24,d0
	Rbra	L_GoError


; _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
;
;						ROUTINES AREXX

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Close the Arexx port
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Arx_Close
; - - - - - - - - - - - - -
	movem.l	a0-a2/d0-d2/a6,-(sp)
	move.l	$4.w,a6
	tst.l	Arx_Base(a5)
	beq.s	.NoArx
	tst.l	Arx_Port(a5)
	beq.s	.NoPort
; Un message re�u � renvoyer?
	move.l	Arx_Answer(a5),d0
	beq.s	.Skip0
	clr.l	Arx_Answer(a5)
	move.l	d0,a1
	move.l	#RC_FATAL,rm_Result1(a1)
	clr.l	rm_Result2(a1)
	jsr	_LVOReplyMsg(a6)
; Receptionne les messages en attente
.Skip0	jsr	_LVOForbid(a6)
	bra.s	.In1
.Loop1	move.l	d0,a1
	move.l	#RC_FATAL,rm_Result1(a1)
	clr.l	rm_Result2(a1)
	jsr	_LVOReplyMsg(a6)
.In1	move.l	Arx_Port(a5),a0
	jsr	_LVOGetMsg(a6)
	tst.l	d0
	bne.s	.Loop1
; D�truit le port de communication
	move.l	Arx_Port(a5),-(sp)
	clr.l	Arx_Port(a5)
	Rjsr	L_DeletePort
	addq.l	#4,sp
	jsr	_LVOPermit(a6)
.NoPort
; Ferme la librairie
	move.l	Arx_Base(a5),a1
	clr.l	Arx_Base(a5)
	jsr	_LVOCloseLibrary(a6)
; Finished!
.NoArx	movem.l	(sp)+,d0-d2/a0-a2/a6
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		Ouverture du port AREXX
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Arx_Open
; - - - - - - - - - - - - -
	tst.l	Arx_Base(a5)
	bne.s	.Err1
; Ouvre la librairie
; ~~~~~~~~~~~~~~~~~~
	lea	.Rexx(pc),a1
	moveq	#0,d0
	move.l	a6,-(sp)
	move.l	$4.w,a6
	jsr	_LVOOpenLibrary(a6)
	move.l	(sp)+,a6
	move.l	d0,Arx_Base(a5)
	beq	.Err2
; Cree le message-port
; ~~~~~~~~~~~~~~~~~~~~
	lea	Arx_PortName(a5),a0
	moveq	#-1,d0
	Rbsr	L_Arx_RegisterPort
	move.l	a0,Arx_Port(a5)
	beq.s	.Err3
; Branche la routine de fermeture
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	movem.l	a0-a2/d0-d1,-(sp)
	lea	.Struc(pc),a1
	lea	Sys_ClearRoutines(a5),a2
	SyCall	AddRoutine
	movem.l	(sp)+,a0-a2/d0-d1
	moveq	#0,d0
	rts
; Already opened
.Err1	move.w	#193,d0
	rts
; No library
.Err2	move.w	#194,d0
	rts
; Cannot open
.Err3	Rbsr	L_Arx_Close
	move.w	#195,d0
	rts
; Structure pour CLEARVAR
.Struc	dc.l	0
	Rbra	L_Arx_Close
; Nom de la librairie
.Rexx	dc.b	"rexxsyslib.library",0
	even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Returns -1 if message port exist
;	A0=	Port name
;	D0=-1	Create port!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Arx_RegisterPort
; - - - - - - - - - - - - -
	move.l	a6,-(sp)
	move.w	d0,-(sp)
	clr.l	-(sp)
	move.l	a0,-(sp)
	moveq	#0,d3
	move.l	$4.w,a6
	jsr	_LVOForbid(a6)
	move.l	(sp),a1
	jsr	_LVOFindPort(a6)
	tst.l	d0
	bne.s	.Exist
	tst.w	8(sp)
	beq.s	.Skip
	Rjsr	L_CreatePort
	move.l	d0,a0
.Skip	moveq	#0,d0
	bra.s	.Out
.Exist	sub.l	a0,a0
	moveq	#-1,d0
.Out	lea	10(sp),sp
	movem.l	d0/a0,-(sp)
	jsr	_LVOPermit(a6)
	movem.l	(sp)+,d0/a0
	move.l	(sp)+,a6
	tst.w	d0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		Get message, and returns -1 if message waiting
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Arx_Message
; - - - - - - - - - - - - -
	move.l	a6,-(sp)
	tst.l	Arx_Base(a5)
	beq.s	.NoMess
	tst.l	Arx_Answer(a5)
	bne.s	.Mess
; Asks for the message
; ~~~~~~~~~~~~~~~~~~~~
	move.l	$4.w,a6
	move.l	Arx_Port(a5),a0
	jsr	_LVOGetMsg(a6)
	tst.l	d0
	beq.s	.NoMess
	move.l	d0,Arx_Answer(a5)
	move.l	d0,a0
	move.l	Arx_Base(a5),a6
	jsr	_LVOIsRexxMsg(a6)
	tst.l	d0
	beq.s	.No
	move.l	Arx_Answer(a5),a0
	move.l	rm_Action(a0),d0
	and.l	#RXCODEMASK,d0
	cmp.l	#RXCOMM,d0
	bne.s	.No
.Mess	moveq	#-1,d0
	bra.s	.Out
.No	clr.l	Arx_Answer(a5)
.NoMess	moveq	#0,d0
.Out	move.l	(sp)+,a6
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Source: Menus.s / Instructions
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ON MENU ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InOnMenuOn
; - - - - - - - - - - - - -
	tst.w	OMnNb(a5)
	beq.s	.Skip
	bset	#BitJump,ActuMask(a5)
.Skip	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ON MENU ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InOnMenuOff
; - - - - - - - - - - - - -
	tst.w	OMnNb(a5)
	beq.s	.Skip
	bset	#BitJump,ActuMask(a5)
.Skip	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	ON MENU DEL
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InOnMenuDel
; - - - - - - - - - - - - -
	Rbsr	L_OMnEff
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Routine: Efface les ON MENU
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	OMnEff
; - - - - - - - - - - - - -
	movem.l	d0/a1,-(sp)
	moveq	#0,d0
	move.w	OMnNb(a5),d0
	beq.s	OMnEx
	lsl.w	#2,d0
	move.l	OMnBase(a5),a1
	SyCall	MemFree
	clr.w	OMnNb(a5)
	clr.l	OMnBase(a5)
OMnEx:	movem.l	(sp)+,d0/a1
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	MENU TO BANK n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMenuToBank
; - - - - - - - - - - - - -
* Taille de l'arbre de menu
	move.l	MnBase(a5),d0
	Rbeq	L_FonCall
	sub.l	a1,a1
	bsr	MnLg
* Reserve la banque!
	move.l	a1,-(sp)
	move.l	d3,d0			Numero
	moveq	#(1<<Bnk_BitData),d1	Flags DATA, FAST
	move.l	(sp)+,d2		Longueur
	Rlea	L_BkMenu,0
	Rjsr	L_Bnk.Reserve
	move.l	a0,a1
	Rbeq	L_OOfMem
	Rjsr	L_Bnk.Change
* Recopie les objets
	move.l	MnBase(a5),a2
	movem.l	a3/a4,-(sp)
	move.l	a1,d3
	bsr	MnTb
	movem.l	(sp)+,a3/a4
	rts
;------ Routine recursive de copie
MnTb	move.l	a1,a3
	move.l	a2,a0
	moveq	#MnLong/2-1,d0
MnTb1	move.w	(a0)+,(a1)+
	dbra	d0,MnTb1
	lea	MnObF(a3),a0
	bsr	MnTbC
	lea	MnOb1(a3),a0
	bsr	MnTbC
	lea	MnOb2(a3),a0
	bsr	MnTbC
	lea	MnOb3(a3),a0
	bsr	MnTbC
	move.l	MnLat(a2),d0
	beq.s	MnTb2
	move.l	a1,MnLat(a3)
	sub.l	d3,MnLat(a3)
	movem.l	a2/a3,-(sp)
	move.l	d0,a2
	bsr	MnTb
	movem.l	(sp)+,a2/a3
MnTb2	move.l	MnNext(a2),d0
	move.l	d0,a2
	beq.s	MnTb3
	move.l	a1,MnNext(a3)
	sub.l	d3,MnNext(a3)
	bra.s	MnTb
MnTb3	rts
* Copie d'un objet
MnTbC	move.l	(a0),d0
	beq.s	MnTbc2
	move.l	a1,(a0)
	sub.l	d3,(a0)
	move.l	d0,a0
	move.w	(a0),d0
	lsr.w	#1,d0
	subq.w	#1,d0
MnTbc1	move.w	(a0)+,(a1)+
	dbra	d0,MnTbc1
MnTbc2	rts
;------ Routine recursive calcul taille
MnLg	move.l	d0,a2
	add.w	#MnLong,a1
	move.l	MnObF(a2),d0
	bsr	MnLgR
	move.l	MnOb1(a2),d0
	bsr	MnLgR
	move.l	MnOb2(a2),d0
	bsr	MnLgR
	move.l	MnOb3(a2),d0
	bsr	MnLgR
	move.l	MnLat(a2),d0
	beq.s	MnLg1
	move.l	a2,-(sp)
	bsr	MnLg
	move.l	(sp)+,a2
MnLg1	move.l	MnNext(a2),d0
	bne.s	MnLg
	rts
* Routine pour un objet
MnLgR	beq.s	MnLgRX
	move.l	d0,a0
	add.w	(a0),a1
MnLgRX	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	BANK TO MENU n
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InBankToMenu
; - - - - - - - - - - - - -
	Rjsr	L_MnRaz
	Rjsr	L_MnClearVar
	tst.w	ScOn(a5)
	Rbeq	L_ScNOp
	move.l	d3,d0
	Rjsr	L_Bnk.GetAdr
	Rbeq	L_BkNoRes
	move.l	a0,d3
	move.l	-8(a0),d0
	Rlea	L_BkMenu,1
	cmp.l	(a1),d0
	Rbne	L_FonCall
	move.l	a0,-(sp)
	moveq	#MnLong,d0
	SyCall	MemFast
	Rbeq	L_OOfMem
	move.l	a0,d0
	move.l	(sp)+,a0
	move.l	d0,MnBase(a5)
	bsr	MnTMn
	move.l	ScOnAd(a5),MnAdEc(a5)
	addq.w	#1,MnChange(a5)
	rts
* Routine recursive
MnTMn	move.l	d0,a1
	move.l	d0,a2
	moveq	#MnLong/2-1,d0
MnTm1	move.w	(a0)+,(a1)+
	dbra	d0,MnTm1
	lea	MnObF(a2),a0
	bsr.s	MnTmR
	lea	MnOb1(a2),a0
	bsr.s	MnTmR
	lea	MnOb2(a2),a0
	bsr.s	MnTmR
	lea	MnOb3(a2),a0
	bsr.s	MnTmR
	move.l	MnLat(a2),d0
	beq.s	MnTm2
	clr.l	MnLat(a2)
	add.l	d3,d0
	move.l	d0,a0
	move.l	a2,MnPrev(a0)
	move.l	a0,-(sp)
	moveq	#MnLong,d0
	SyCall	MemFast
	Rbeq	L_OOfMem
	move.l	a0,d0
	move.l	(sp)+,a0
	move.l	d0,MnLat(a2)
	move.l	a2,-(sp)
	bsr	MnTMn
	move.l	(sp)+,a2
MnTm2	move.l	MnNext(a2),d0
	beq.s	MnTm3
	clr.l	MnNext(a2)
	add.l	d3,d0
	move.l	d0,a0
	move.l	a0,-(sp)
	moveq	#MnLong,d0
	SyCall	MemFast
	Rbeq	L_OOfMem
	move.l	a0,d0
	move.l	(sp)+,a0
	move.l	a2,MnPrev(a0)
	move.l	d0,MnNext(a2)
	bra	MnTMn
MnTm3	rts
* Routine de copie des objets
MnTmR	move.l	(a0),d0
	beq.s	MnTmRX
	clr.l	(a0)
	add.l	d3,d0
	move.l	d0,a1
	moveq	#0,d0
	move.w	(a1),d0
	move.w	d0,d1
	move.l	a0,-(sp)
	SyCall	MemFast
	Rbeq	L_OOfMem
	move.l	a0,d0
	move.l	(sp)+,a0
	move.l	d0,(a0)
	move.l	d0,a0
	lsr.w	#1,d1
	subq.w	#1,d1
MnTmR1	move.w	(a1)+,(a0)+
	dbra	d1,MnTmR1
MnTmRX	rts




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU ON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMenuOn
; - - - - - - - - - - - - -
	tst.l	MnBase(a5)
	beq.s	.Skip
	bset	#BitMenu,ActuMask(a5)
	clr.l	T_ClLast(a5)
.Skip	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMenuOff
; - - - - - - - - - - - - -
	bclr	#BitMenu,ActuMask(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU MOUSE ON/OFF
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMenuMouseOn
; - - - - - - - - - - - - -
	move.w	#1,MnMouse(a5)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuMouseOff
; - - - - - - - - - - - - -
	clr.w	MnMouse(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU BASE x,y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMenuBase
; - - - - - - - - - - - - -
	move.l	MnBase(a5),d0
	Rbeq	L_MnNOp
	move.l	d0,a0
	move.l	#EntNul,d1
	move.l	d3,d0
	cmp.l	d0,d1
	beq.s	Imnbs1
	move.w	d0,MnY(a0)
	bset	#MnFixed,MnFlag(a0)
Imnbs1	move.l	(a3)+,d0
	cmp.l	d0,d1
	beq.s	Imnbs2
	move.w	d0,MnX(a0)
	bset	#MnFixed,MnFlag(a0)
Imnbs2	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=MENU X(,,,)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnXMenu
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	moveq	#0,d3
	move.w	MnX(a2),d3
	Ret_Int
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=MENU Y(,,,)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnYMenu
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	moveq	#0,d3
	move.w	MnY(a2),d3
	Ret_Int

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	=CHOICE(n)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par FnChoice0
; - - - - - - - - - - - - -
	move.w	MnChoice(a5),d3
	clr.w	MnChoice(a5)
	ext.l	d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par FnChoice1
; - - - - - - - - - - - - -
	tst.l	d3
	Rbls	L_FonCall
	cmp.l	#MnNDim,d3
	Rbhi	L_FonCall
	lsl.w	#1,d3
	lea	MnChoix(a5),a0
	move.w	-2(a0,d3.w),d3
	Ret_Int
; - - - - - - - - - - - - -
	Lib_Par InMenuBar
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bset	#MnBar,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuLine
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bclr	#MnBar,(a0)
	bclr	#MnTotal,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuTline
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bclr	#MnBar,(a0)
	bset	#MnTotal,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuMovable
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bset	#MnTBouge,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuStatic
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bclr	#MnTBouge,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuItemMovable
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bset	#MnBouge,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuItemStatic
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bclr	#MnBouge,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuActive
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bclr	#MnOff,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuInactive
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bset	#MnOff,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuSeparate
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bset	#MnSep,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuLink
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	bclr	#MnSep,(a0)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuCalled
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	move.b	#-1,MnFlag+1(a2)
	rts
; - - - - - - - - - - - - -
	Lib_Par InMenuOnce
; - - - - - - - - - - - - -
	Rjsr	L_MnDim
	clr.b	MnFlag+1(a2)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MENU CALCULATE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Par InMenuCalc
; - - - - - - - - - - - - -
	tst.l	MnBase(a5)
	Rbeq	L_MnNOp
* Active l'ecran du menu!
	move.l	T_EcCourant(a5),a0
	move.w	EcNumber(a0),MnScOn(a5)
	move.l	MnAdEc(a5),d0
	Rbeq	L_ScNOp
	move.l	d0,a0
	move.w	EcNumber(a0),d1
	EcCall	Active
	Rbne	L_ScNOp
	move.w	#-1,MnProc(a5)
	EcCall	MnStart
* Va calculer les coordonnees
	Rjsr	L_MnCalc
* Remet l'ecran
	Rjsr	L_MnEnd1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Menu not opened
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnNOp
; - - - - - - - - - - - - -
	moveq	#38,d0
	Rbra	L_GoError



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
	Lib_Def	MnGere
; - - - - - - - - - - - - -
	movem.l	a0-a2/d0-d5,-(sp)
	movem.l	a3-a6/d6-d7,-(sp)
	move.l	sp,MnRA3(a5)		Pour recupere les registres
	clr.w	MnError(a5)		Pas d'erreur
* Active l'ecran du menu!
	move.l	T_EcCourant(a5),a0
	move.w	EcNumber(a0),MnScOn(a5)
	move.l	MnAdEc(a5),d0
	Rbeq	L_ScNOp
	move.l	d0,a0
	move.w	EcNumber(a0),d1
	EcCall	Active
	Rbne	L_ScNOp
* Appel du patch monitor, si defini
	move.l	Patch_ScFront(a5),d0
	beq.s	.Skip
	move.l	d0,a0
	jsr	(a0)
* Fixe les parametres par defaut
.Skip	EcCall	MnStart
* Va calculer les coordonnees
	tst.w	MnChange(a5)
	beq.s	MnGe0
	move.w	#-1,MnProc(a5)
	Rbsr	L_MnCalc
* Reserve les zones
MnGe0	move.w	MnNZone(a5),d1
	SyCall	ResZone
	beq	MnGe1
	move.w	#24,MnError(a5)
* Va afficher la barre/Arret/Avec fond
MnGe1	move.w	#1,MnProc(a5)
	SyCall	AMALFrz
	moveq	#0,d4
	moveq	#0,d5
	tst.w	MnMouse(a5)
	beq.s	MnGe2
	SyCall	XyMou
	moveq	#0,d3
	SyCall	XyScr
	move.w	d1,d4
	move.w	d2,d5
MnGe2:	move.w	d4,MnBaseX(a5)
	move.w	d5,MnBaseY(a5)
	move.l	MnBase(a5),a2
	Rbsr	L_MnBranch

;------ Init params boucle de test
	lea	MnTable(a5),a6		* Niveau de menu
	clr.l	(a6)
	moveq	#0,d6
	clr.w	MnZoAct(a5)

;------ Boucle de test!
MnLoop:	SyCall	WaitVbl
	tst.w	MnError(a5)
	bne	MnErr
* Appelle les menus CALLABLE
	move.l	MnBase(a5),a2
	lea	MnTable(a5),a1
MnCl1	tst.b	MnFlag+1(a2)
	beq.s	MnCl3
	moveq	#0,d7
	cmp.l	(a1),a2
	bne.s	MnCl2
	bset	#30,d7
MnCl2	move.w	MnXX(a2),d4
	move.w	MnYY(a2),d5
	Rbsr	L_MnDraw
MnCl3	move.l	MnNext(a2),d0
	move.l	d0,a2
	bne.s	MnCl1
	move.l	(a1)+,d0
	beq.s	MnCl4
	move.l	d0,a2
	move.l	MnLat(a2),a2
	bra.s	MnCl1
MnCl4:
*	SyCall	Inkey
*	cmp.b	#" ",d1
*	beq	MnExit
*	cmp.b	#"b",d1
*	bne.s	MnDebug
*	jsr	Bug
* Appuie sur SHIFT -> inactive la souris: MOUVEMENTS LIBRES!!!!
MnDebug	SyCall	Shifts
	and.b	#$03,d1
	bne.s	MnLp2b
* Souris dans une zone???
	moveq	#0,d3
	SyCall	GetZone
	tst.l	d1
	beq.s	MnLp0
	move.l	MnAdEc(a5),a1
	cmp.w	EcNumber(a1),d1
	bne.s	MnLp0
	swap	d1
	tst.w	d1
	bne.s	MnLp1
MnLp0:	clr.w	d1
	move.w	d6,d5
MnLp1:	move.w	d1,d4
* Lache le bouton de droite?
	SyCall	MouseKey
	btst	#1,d1
	beq	MnExit
	btst	#0,d1
	bne	MnBGoch
* Verifie que la nouvelle case n'est pas DEJA ouverte
MnLp2	tst.w	d4
	beq.s	MnLp2a
	bsr	ZoToMn
	btst	#MnOff,MnFlag(a2)
	beq.s	MnLp2a
MnLp2b	clr.w	d4
	move.w	d6,d5
MnLp2a	cmp.w	MnZoAct(a5),d4
	beq	MnLoop
	tst.w	d4
	beq.s	MnLp4
	lea	MnTable(a5),a0
	clr.w	d0
MnLp3	cmp.w	d0,d6
	beq.s	MnLp4
	move.l	(a0)+,a1
	cmp.w	MnZone(a1),d4
	beq.s	MnLp0
	addq.w	#1,d0
	bra.s	MnLp3
* Ramene le niveau a celui du menu
MnLp4	Rbsr	L_MnDEff
* Efface la case activee
	tst.w	MnZoAct(a5)
	beq.s	MnLp5
	movem.l	a2/d4/d5,-(sp)
	move.l	MnAct(a5),a2
	move.w	MnXX(a2),d4
	move.w	MnYY(a2),d5
	moveq	#0,d7
	Rbsr	L_MnDraw
	movem.l	(sp)+,a2/d4/d5
	clr.w	MnZoAct(a5)
* Active la nouvelle
MnLp5	move.w	d4,MnZoAct(a5)
	beq	MnLoop
	move.l	a2,MnAct(a5)
	move.w	MnXX(a2),d4
	move.w	MnYY(a2),d5
	moveq	#0,d7
	bset	#30,d7
	Rbsr	L_MnDraw
* Dessine la collaterale?
	move.l	MnLat(a2),d0
	beq	MnLoop
	move.l	a2,(a6)+
	addq.w	#1,d6
	move.l	d0,a2
	Rbsr	L_MnBranch
	clr.w	MnZoAct(a5)
	bra	MnLoop

;------ Fin du menu! Efface tout!
MnExit
* Efface les anciens choix
	moveq	#MnNDim-1,d0
	lea	MnChoix(a5),a0
	move.l	a0,a2
MnEx1	clr.w	(a0)+
	dbra	d0,MnEx1
	clr.w	MnChoice(a5)
* Trouve le nouveau
	tst.w	MnZoAct(a5)
	beq.s	MnExX
	move.w	d6,d0
	beq.s	MnEx3
	lea	MnTable(a5),a0
MnEx2	move.l	(a0)+,a1
	move.w	MnNb(a1),(a2)+
	subq.w	#1,d0
	bne.s	MnEx2
MnEx3	move.l	MnAct(a5),a1
	move.w	MnNb(a1),(a2)+
	bset	#BitJump,T_Actualise(a5)
	move.w	#-1,MnChoice(a5)
* Ferme tout
MnExX	Rbsr	L_MnEnd
* Remet l'ecran du moniteur
	move.l	Patch_ScFront(a5),d0
	beq.s	.Skip
	move.l	d0,a0
	jsr	4(a0)
* Sort
.Skip	movem.l	(sp)+,a3-a6/d6-d7
	IFNE	Debug=2
	move.l	a3,Chr_Debug(a5)		Pour la sortie du menu!
	movem.l	d6/d7,Chr_Debug+4(a5)
	ENDC
	movem.l	(sp)+,a0-a2/d0-d5
	moveq	#0,d0				Pas d'erreur!
	rts

;	CLIQUE AVEC LE BOUTON GAUCHE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnBGoch
* En dehors?
	tst.w	d4
	beq	MnLoop
* Efface tout jusqu'a son niveau
	bsr	ZoToMn
	Rbsr	L_MnDEff
	tst.w	MnZoAct(a5)
	beq.s	MnMg1
	movem.l	a2/d4/d5,-(sp)
	move.l	MnAct(a5),a2
	move.w	MnXX(a2),d4
	move.w	MnYY(a2),d5
	moveq	#0,d7
	Rbsr	L_MnDraw
	movem.l	(sp)+,a2/d4/d5
	clr.w	MnZoAct(a5)
* Trouve les coordonnees maximum
MnMg1	movem.l	a3-a6/d4-d7,-(sp)
	btst	#MnFlat,MnFlag(a2)
	bne.s	MnMg3
	btst	#MnSep,MnFlag(a2)
	bne.s	MnMg3
* Trouve la coordonnee maxi de la zone!
	btst	#MnBouge,MnFlag(a2)
	beq	MnMgX
	move.l	a2,a0
MnMg0	move.l	MnPrev(a0),a0
	btst	#MnFlat,MnFlag(a0)
	bne.s	MnMg2
	btst	#MnSep,MnFlag(a0)
	beq.s	MnMg0
MnMg2	move.w	MnXX(a0),d4
	move.w	MnYY(a0),d5
	move.w	MnMX(a0),d6
	move.w	MnMY(a0),d7
	add.w	d4,d6
	add.w	d5,d7
	move.w	MnTx(a2),a3
	move.w	MnTy(a2),a4
	move.l	a0,-(sp)
	bra.s	MnMg4
* Arbre entier: pas de limite
MnMg3	btst	#MnTBouge,MnFlag(a2)
	beq	MnMgX
	move.w	MnMX(a2),a3
	move.w	MnMY(a2),a4
	move.w	a3,d4
	neg.w	d4
	move.w	a4,d5
	neg.w	d5
	move.l	MnAdEc(a5),a0
	move.w	EcTx(a0),d6
	move.w	EcTy(a0),d7
	add.w	a3,d6
	add.w	a4,d7
	addq.w	#1,d4
	addq.w	#1,d5
	subq.w	#2,d6
	subq.w	#2,d7
	move.l	a2,-(sp)
* Boucle de test!
MnMg4	SyCall	XyMou			* Trouve le decalage de la souris
	moveq	#0,d3
	SyCall	XyScr
	swap	d4
	swap	d5
	move.w	MnXX(a2),d4
	sub.w	d1,d4
	move.w	MnYY(a2),d5
	sub.w	d2,d5
	swap	d4
	swap	d5
	bsr	MnMgI
MnMgL	bsr	MnMgD
	SyCall	MouseKey
	and.b	#3,d1
	cmp.b	#3,d1
	beq.s	MnMgL
* Change les coordonnees relatives de l'objet/arbre
MnMgR	move.l	MnTDraw(a5),a0
	move.w	(a0)+,d2
	move.w	(a0)+,d3
	sub.w	MnXX(a2),d2
	sub.w	MnYY(a2),d3
	add.w	d2,MnX(a2)
	add.w	d3,MnY(a2)
	bset	#MnFixed,MnFlag(a2)
* Retabli l'objet suivant dans l'arbre
	move.l	MnNext(a2),d0
	beq.s	MnMgR3
	btst	#MnFlat,MnFlag(a2)
	bne.s	MnMgR1
	btst	#MnSep,MnFlag(a2)
	beq.s	MnMgR2
MnMgR1	move.l	d0,a0
	btst	#MnSep,MnFlag(a0)
	bne.s	MnMgR2
	move.l	MnNext(a0),d0
	bne.s	MnMgR1
	bra.s	MnMgR3
MnMgR2	move.l	d0,a0
	sub.w	d2,MnX(a0)
	sub.w	d3,MnY(a0)
	bset	#MnFixed,MnFlag(a0)
* Efface l'arbre
MnMgR3	bsr	MnMgF
	move.l	(sp)+,a2
	Rbsr	L_MnEBranch
* Redessine le nouveau!
	move.w	MnBaseX(a5),d4
	move.w	MnBaseY(a5),d5
	move.l	MnPrev(a2),d0
	beq.s	MnMgR4
	move.l	d0,a0
	move.w	MnXX(a0),d4
	move.w	MnYY(a0),d5
MnMgR4	Rbsr	L_MnBranch
* Ca y est!
MnMgX	movem.l	(sp)+,a3-a6/d4-d7
	bra	MnLoop

;	Trouve l'adresse d'un zone
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D4= Zone
;	D5= Niveau sortie
;
ZoToMn	lea	MnTable(a5),a0
	moveq	#0,d5
	move.l	MnBase(a5),d0
	bra.s	ZoTm2
ZoTm1	move.l	(a0)+,a2
	move.l	MnLat(a2),d0
	addq.w	#1,d5
ZoTm2:	move.l	d0,a2
	cmp.w	MnZone(a2),d4
	beq.s	ZoTm3
	move.l	MnNext(a2),d0
	bne.s	ZoTm2
	cmp.w	d5,d6
	bne.s	ZoTm1
ZoTm3:	rts

;	Initialisation dessin OUTLINE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnMgI	move.l	T_RastPort(a5),a1
	moveq	#31,d0
	Rbsr	L_ObInkA
	moveq	#0,d0
	Rbsr	L_ObInkB
	moveq	#0,d0
	Rbsr	L_ObInkC
	moveq	#%11,d0
	Rbsr	L_ObWrite
	move.w	#$00FF,34(a1)
	rts
;	Remet dessin normal
; ~~~~~~~~~~~~~~~~~~~~~~~~~
MnMgF	move.l	T_RastPort(a5),a1
	moveq	#1,d0
	Rbsr	L_ObWrite
	move.w	#$FFFF,34(a1)
	rts
;	Dessin de la outline objet (a2) en XMOUSE/YMOUSE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnMgD	SyCall	XyMou
	moveq	#0,d3
	SyCall	XyScr
* Plus decalage de la souris
	move.w	d1,d0
	move.w	d2,d1
	swap	d4
	swap	d5
	add.w	d4,d0
	add.w	d5,d1
	swap	d4
	swap	d5
* Sort des limites?
	cmp.w	d4,d0
	bge.s	MnMgd1
	move.w	d4,d0
MnMgd1	cmp.w	d5,d1
	bge.s	MnMgd2
	move.w	d5,d1
MnMgd2	move.w	d0,d2
	move.w	d1,d3
	add.w	a3,d2
	add.w	a4,d3
	cmp.w	d6,d2
	ble.s	MnMgd3
	move.w	d6,d2
	move.w	d2,d0
	sub.w	a3,d0
MnMgd3	cmp.w	d7,d3
	ble.s	MnMgd4
	move.w	d7,d3
	move.w	d3,d1
	sub.w	a4,d1
* Poke les nouvelles coords ABSOLUES en (a2)
MnMgd4	move.l	MnTDraw(a5),a0
	move.l	T_RastPort(a5),a1
	subq.w	#1,d2
	subq.w	#1,d3
	move.w	d0,36(a1)
	move.w	d1,38(a1)
	move.w	d0,(a0)+
	move.w	d1,(a0)+
	move.w	d2,(a0)+
	move.w	d1,(a0)+
	move.w	d2,(a0)+
	move.w	d3,(a0)+
	move.w	d0,(a0)+
	move.w	d3,(a0)+
	move.w	d0,(a0)+
	move.w	d1,(a0)+
	moveq	#5,d0
	move.l	MnTDraw(a5),a0
	movem.l	a0-a1/d0,-(sp)
	SyCall	WaitVbl
	movem.l	(sp),a0-a1/d0
	GfxCa5	PolyDraw
	SyCall	WaitVbl
	movem.l	(sp)+,a0-a1/d0
	GfxCa5	PolyDraw
* Fait tourner la ligne
	rol.w	34(a1)
	rts

; 	Fin du menu avec erreur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnErr	Rbsr	L_MnEnd
	movem.l	(sp)+,d0-d7/a0-a6
	bclr	#BitJump,T_Actualise(a5)
	clr.w	MnChoice(a5)
	move.w	MnError(a5),d0			Une erreur!
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Ferme tout les menus!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnEnd
; - - - - - - - - - - - - -
	tst.w	MnProc(a5)
	Rbmi	L_MnEnd1
	movem.l	a0-a6/d0-d7,-(sp)
* Trouve la fin de la table
	lea	MnTable(a5),a6
	moveq	#-1,d6
.Loop	addq.l	#1,d6
	tst.l	(a6)+
	bne.s	.Loop
	subq.l	#4,a6
* Efface les menus
	moveq	#0,d5			* Efface tous les menus
	Rbsr	L_MnDEff
	move.l	MnBase(a5),a2
	Rbsr	L_MnEBranch
	SyCall	AMALUFrz
	movem.l	(sp)+,a0-a6/d0-d7
	Rbra	L_MnEnd1
; - - - - - - - - - - - - -
	Lib_Def	MnEnd1
; - - - - - - - - - - - - -
	movem.l	a0-a6/d0-d7,-(sp)
	moveq	#0,d1			* Efface toutes les zones
	SyCall	ResZone
	EcCall	MnStop			* Remet l'ecran
	move.w	MnScOn(a5),d1		* Reactive l'ecran courant
	EcCall	Active
	clr.w	MnProc(a5)		* Plus de procedure!
	movem.l	(sp)+,a0-a6/d0-d7
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface le menu---> niveau D5
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnDEff
; - - - - - - - - - - - - -
	movem.l	a2/d0-d2,-(sp)
MnDD1	cmp.w	d5,d6
	bls.s	MnDdX
	subq.w	#1,d6
	move.l	-(a6),a2
	clr.l	(a6)
* Remet la zone activee
	move.l	a2,MnAct(a5)
	move.w	MnZone(a2),MnZoAct(a5)
* Efface la collaterale
	move.l	MnLat(a2),a2
	Rbsr	L_MnEBranch
	bra.s	MnDD1
* Fini
MnDdX:	movem.l	(sp)+,a2/d0-d2
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la branche A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnEBranch
; - - - - - - - - - - - - -
	movem.l	a2/d0/d1/d7,-(sp)
	move.l	a2,d0
	move.l	a2,d7
MnEBr1	move.l	d0,a2
	move.w	MnZone(a2),d1
	SyCall	RazZone
	move.l	MnNext(a2),d0
	bne.s	MnEBr1
* Restore l'image -en sens inverse!-
MnEBr2	tst.l	MnAdSave(a2)
	beq.s	MnEBr3
	Rbsr	L_MnRest
	Rbsr	L_MnSaDel
MnEBr3	cmp.l	d7,a2
	beq.s	MnEBr4
	move.l	MnPrev(a2),d0
	move.l	d0,a2
	bne.s	MnEBr2
MnEBr4	movem.l	(sp)+,a2/d0/d1/d7
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	MNCALC: Calcule les positions de l'arbre
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnCalc
; - - - - - - - - - - - - -
	movem.l	d0-d7/a2,-(sp)
	clr.w	MnChange(a5)
* Explore tout l'arbre
	clr.w	MnNZone(a5)
	move.l	MnBase(a5),d0
	moveq	#0,d4
	moveq	#0,d5
	bsr	MnCa0
* Poke les numeros de zone!
	move.w	MnNZone(a5),d7
	move.l	MnBase(a5),d0
	bsr	MnPZo
	movem.l	(sp)+,d0-d7/a2
	rts

;	Routine recursive de pokage des zones
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnPZo:	move.w	d6,-(sp)
	clr.w	d6
MnPZo2	move.l	d0,a2
	move.l	MnLat(a2),d0
	beq.s	MnPZo3
	move.l	d0,-(sp)
	addq.w	#1,d6
MnPZo3	move.w	d7,MnZone(a2)
	subq.w	#1,d7
	move.l	MnNext(a2),d0
	bne.s	MnPZo2
MnPZo4	subq.w	#1,d6
	bmi.s	MnPZo5
	move.l	(sp)+,d0
	bsr	MnPZo
	tst.w	d6
	bne.s	MnPZo4
MnPZo5:	move.w	(sp)+,d6
	rts

; 	Routine recursive de calcul!
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnCa0:	movem.l	a0-a2/d4-d7,-(sp)
* Si tour: tout decale un cran!
MnCa0a	move.l	d0,a2
	tst.l	MnObF(a2)
	bne.s	MnCa1
	addq.w	#2,d4
	addq.w	#2,d5
* Exploration d'un arbre
MnCa1	move.l	d0,a2
	addq.w	#1,MnNZone(a5)
* Fixe les coordonnees d'un objet
	btst	#MnFixed,MnFlag(a2)
	bne.s	MnCa2
	move.w	d4,MnX(a2)
	move.w	d5,MnY(a2)
MnCa2	moveq	#0,d4
	moveq	#0,d5
	bset	#31,d7				* Va calculer la taille
	bclr	#30,d7
	bset	#29,d7
	Rbsr	L_MnDraw
	move.w	d2,MnTx(a2)
	move.w	d3,MnTy(a2)
	move.w	d0,d2
	move.w	d1,d3
* Exploration recursive d'une collaterale
	move.l	MnLat(a2),d0
	beq.s	MnCa7
	movem.w	d2-d5,-(sp)
	btst	#MnBar,MnFlag(a2)
	bne.s	MnCa5
	clr.w	d2
	bra.s	MnCa6
MnCa5	clr.w	d3
MnCa6	move.w	d2,d4
	move.w	d3,d5
	bsr	MnCa0
	movem.w	(sp)+,d2-d5
* Coordonnees pour le suivant
MnCa7	btst	#MnBar,MnFlag(a2)		* Taille automatique!
	bne.s	MnCa8
	clr.w	d3
	bra.s	MnCa9
MnCa8	clr.w	d2
MnCa9	move.w	d2,d4
	move.w	d3,d5
* Passe a l'objet suivant
MnCaN:	move.l	MnNext(a2),d0
	beq.s	MnCaNx
	move.l	d0,a2
	btst	#MnSep,MnFlag(a2)
	beq	MnCa1
	bra 	MnCa0a
MnCaNx	movem.l	(sp)+,a0-a2/d4-d7
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Dessin d'une branche de l'arbre
;	FIXE LES ZONES!
;	A2=	Base a dessiner
;	D4/D5=	Coordonnee de base
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnBranch
; - - - - - - - - - - - - -
	movem.l a0-a2/d0-d7,-(sp)
* Sauve le fond
MnBrR	bsr	MnMaxi
	Rbsr	L_MnSave
* Dessine le tour, si pas dessine dans le premier objet
	tst.l	MnObF(a2)
	bne.s	MnBrO
	move.l	T_RastPort(a5),a1
	moveq	#0,d0			* Pattern VIDE!
	Rbsr	L_ObPat
	bset	#4,33(a1)		* Outline
	move.w	#$FFFF,34(a1)		* Ligne pleine
	moveq	#0,d0
	move.b	MnInkB1(a2),d0		* Change les encres
	Rbsr	L_ObInkA
	move.b	MnInkC1(a2),d0
	Rbsr	L_ObInkC
	moveq	#1,d0
	Rbsr	L_ObWrite
	move.w	d4,d0
	move.w	d5,d1
	move.w	d6,d2
	move.w	d7,d3
	subq.w	#1,d2
	subq.w	#1,d3
	GfxCa5	RectFill
* Dessine les objets
MnBrO	move.w	MnXX(a2),d4
	move.w	MnYY(a2),d5
	moveq	#0,d7			* Dessine
	bset	#29,d7
	Rbsr	L_MnDraw
	move.w	MnZone(a2),d1
	move.w	d4,d2
	move.w	d5,d3
	add.w	MnTx(a2),d4
	add.w	MnTy(a2),d5
	tst.w	d2
	bpl.s	MnBro1
	clr.w	d2
MnBro1	tst.w	d3
	bpl.s	MnBro2
	clr.w	d3
MnBro2	SyCall	SetZone
* Encore un objet ?
	move.l	a2,a0
	move.l	MnNext(a2),d0
	beq.s	MnBrX
	move.l	d0,a2
	btst	#MnSep,MnFlag(a2)
	beq.s	MnBrO
	move.w	MnXX(a0),d4
	move.w	MnYY(a0),d5
	bra	MnBrR
MnBrX:	movem.l	(sp)+,a0-a2/d0-d7
	rts

;	Ramene les coord maxi d'une branche
;	Poke les coordonnees des objets
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A2= 	Base
;	D4/D5->	XG/YH (entree)
;	D6/D7-> XD/YB
MnMaxi:	movem.l	d0-d3/a2,-(sp)
	move.w	d4,d2
	move.w	d5,d3
	move.w	#32766,d4
	move.w	d4,d5
	clr.w	d6
	clr.w	d7
MnMx0	add.w	MnX(a2),d2
	add.w	MnY(a2),d3
	move.w	d2,MnXX(a2)
	move.w	d3,MnYY(a2)
	cmp.w	d4,d2
	bge.s	MnMx1
	move.w	d2,d4
MnMx1	cmp.w	d5,d3
	bge.s	MnMx2
	move.w	d3,d5
MnMx2	cmp.w	d6,d2
	ble.s	MnMx3
	move.w	d2,d6
MnMx3	cmp.w	d7,d3
	ble.s	MnMx4
	move.w	d3,d7
MnMx4	move.w	d2,d0
	move.w	d3,d1
	add.w	MnTx(a2),d0
	add.w	MnTy(a2),d1
	cmp.w	d6,d0
	ble.s	MnMx5
	move.w	d0,d6
MnMx5	cmp.w	d7,d1
	ble.s	MnMx6
	move.w	d1,d7
MnMx6	move.l	MnNext(a2),d0
	beq.s	MnMx7
	move.l	d0,a2
	btst	#MnSep,MnFlag(a2)
	beq.s	MnMx0
* Poke la taille INTERNE de cet arbre
MnMx7	movem.l	(sp),d0-d3/a2
	move.w	d6,d0
	move.w	d7,d1
	sub.w	d4,d0
	sub.w	d5,d1
	move.w	d0,MnMX(a2)
	move.w	d1,MnMY(a2)
* Un tour?
	tst.l	MnObF(a2)
	bne.s	MnMxX
	subq.w	#2,d4
	subq.w	#2,d5
	addq.w	#2,d6
	addq.w	#2,d7
	btst	#MnBar,MnFlag(a2)
	bne.s	MnMxX
	btst	#MnTotal,MnFlag(a2)
	beq.s	MnMxX
	move.l	MnAdEc(a5),a2
	moveq	#0,d4
	move.w	EcTx(a2),d6
* Poke la taille maxi de l'arbre
MnMxX	movem.l	(sp)+,d0-d3/a2
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Dessin / Calcul taille d'une case de menu (A2) - D4/D5
;	#31 de D7->	Dessin(0) / Calcule(1)
;	#30 de D7->	Inactif(0) / Actif(1)
;	#29 de D7->	PasFond(0) / Fond(1)
;	D4/D5	 -> 	Coordonnees
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnDraw
; - - - - - - - - - - - - -
	movem.l	d4-d7,-(sp)
	move.l	a2,MnDAd(a5)
	clr.w	d0
	clr.w	d1
	clr.w	d2
	clr.w	d3
	clr.w	d6
	clr.w	d7
* Imprime/Calcule l'objet de fond?
	tst.l	MnObF(a2)
	beq.s	MnDr1
	btst	#29,d7
	beq.s	MnDr0
	move.l	MnObF(a2),d0
	move.b	MnInkB1(a2),d1
	move.b	MnInkA1(a2),d2
	move.b	MnInkC1(a2),d3
	Rbsr	L_MnODraw
	move.w	d2,d6
	move.w	d3,d7
	add.w	d0,d4
	add.w	d1,d5
* Pointe et Imprime/Calcule le bon objet
MnDr0:	btst	#MnOff,MnFlag(a2)
	beq.s	MnDr1
	tst.l	MnOb3(a2)
	beq.s	MnDr1
	move.l	MnOb3(a2),d0
	move.b	MnInkA1(a2),d1
	move.b	MnInkB1(a2),d2
	move.b	MnInkC1(a2),d3
	bclr	#30,d7
	bra.s	MnDr3
MnDr1:	tst.l	MnOb1(a2)
	beq.s	MnDr4
	move.l	MnOb1(a2),d0
	move.b	MnInkA1(a2),d1
	move.b	MnInkB1(a2),d2
	move.b	MnInkC1(a2),d3
	btst	#30,d7
	beq.s	MnDr3
	tst.l	MnOb2(a2)
	bne.s	MnDr2
	move.b	MnInkA2(a2),d1
	move.b	MnInkB2(a2),d2
	move.b	MnInkC2(a2),d3
	bra.s	MnDr3
MnDr2:	move.l	MnOb2(a2),d0
	bclr	#30,d7
MnDr3:	Rbsr	L_MnODraw
* Ramene la taille maxi en D2/D3
* D0/D1-> Decalage au prochain!
MnDr4:	cmp.w	d6,d2
	bls.s	MnDr5
	move.w	d2,d6
MnDr5	cmp.w	d7,d3
	bls.s	MnDr6
	move.w	d3,d7
MnDr6	move.w	d6,d2
	move.w	d7,d3
	movem.l	(sp)+,d4-d7
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Dessine un objet
;	D1/D2/D3= Encres par default
;	D4/D5	= X/Y
;	Ramene:	D0/D1 -> Position curseur!
;	Ramene: D2/D3 -> Taille maxi!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnODraw
; - - - - - - - - - - - - -
	movem.l	a0-a6/d4-d7,-(sp)
	move.l	d0,a4
	move.l	T_RastPort(a5),a1
	move.w	d4,36(a1)
	move.w	d5,38(a1)
	move.w	d4,a2
	move.w	d5,a3
	moveq	#0,d4
	moveq	#0,d5
	move.w	d4,d6
	move.w	d5,d7
* Change les encres --> par defaut
	tst.l	d7
	bmi.s	MnODr0
	moveq	#0,d0
	move.w	d1,d0
	Rbsr	L_ObInkA
	move.w	d2,d0
	Rbsr	L_ObInkB
	move.w	d3,d0
	Rbsr	L_ObInkC
	moveq	#1,d0
	Rbsr	L_ObWrite
* Appelle la fonction
MnODr0	lea	2(a4),a4
MnODr1	move.w 	(a4)+,d0
	jsr	ObJumps(pc,d0.w)
* Trouve le maximum
	cmp.w	d6,d4
	ble.s	MnODr2
	move.w	d4,d6
MnODr2:	cmp.w	d7,d5
	ble.s	MnODr1
	move.w	d5,d7
	bra.s	MnODr1
* Fini!
MnODrX:	move.w	d4,d0
	move.w	d5,d1
	move.w	d6,d2
	move.w	d7,d3
	movem.l	(sp)+,a0-a6/d4-d7
	rts

;	Branchements aux instructions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ObJumps	bra	MnOFin
	bra	MnOPr
	bra	MnOBar			* 08 Bar
	bra	MnOLine			* 0C Line
	bra	MnOEl			* 10 Ellipse
	bra	MnOPat			* 14 Pattern
	bra	MnOInk			* 18 Ink
	bra	MnOBob			* 1C Bob
	bra	MnOIco			* 20 Icon
	bra	MnOLoc			* 24 Locate
	bra	MnOOut			* 28 Out line
	bra	MnOSL			* 2C Set Line
	bra	MnOFont			* 30 Set Font
	bra	MnOCall			* 34 CALL procedure
	bra	MnORes			* 38 Reserve space
	bra	MnOSty			* 3C Set style

;	Fin dessin objet
; ~~~~~~~~~~~~~~~~~~~~~~
MnOFin:	addq.l	#4,sp
	bra	MnODrX
MnORien
MnORet	rts

;	Impression texte
; ~~~~~~~~~~~~~~~~~~~~~~
MnOPr	move.w	(a4)+,d0
	move.l	a4,a0
	add.w	d0,a4
	btst	#0,d0
	beq.s	MnOP0
	addq.l	#1,a4
MnOP0:	tst.l	d7
	bmi.s	MnOP1
	move.w	d4,d2			* X
	add.w	a2,d2
	move.w	d2,36(a1)
	move.w	d5,d3			* Y
	add.w	62(a1),d3
	add.w	a3,d3
	move.w	d3,38(a1)
	move.w	d0,d1
	move.w	#Text,a6
	bsr	MnGfx
	move.w	d1,d0
MnOP1:	move.w	#TextLength,a6		* Taille
	bsr	MnGfx
	add.w	d0,d4
	add.w	58(a1),d5
	rts

;	LOCATE
; ~~~~~~~~~~~~
MnOLoc	move.l	a4,a0
	addq.l	#4,a4
	move.w	(a0)+,d0
	bmi	MnORien
	move.w	(a0)+,d1
	bmi	MnORien
	move.w	d0,d4
	move.w	d1,d5
	rts

;	BAR TO x,y
; ~~~~~~~~~~~~~~~~
MnOBar	move.l	a4,a0
	addq.l	#4,a4
	move.w	d4,d0
	move.w	d5,d1
	move.w	(a0)+,d2
	bmi	MnORien
	move.w	(a0)+,d3
	bmi	MnORien
	cmp.w	d0,d2
	bls	MnORien
	cmp.w	d1,d3
	bls	MnORien
	move.w	d2,d4
	move.w	d3,d5
	addq.w	#1,d4
	addq.w	#1,d5
	add.w	a2,d0
	add.w	a3,d1
	add.w	a2,d2
	add.w	a3,d3
	tst.l	d7
	bmi	MnORet
	move.w	#RectFill,a6
	bra	MnGfx

;	LINE x,y
; ~~~~~~~~~~~~~~
MnOLine	move.l	a4,a0
	addq.l	#4,a4
	move.w	(a0)+,d0
	bmi	MnORien
	move.w	(a0)+,d1
	bmi	MnORien
	move.w	d4,d2
	add.w	a2,d2
	move.w	d2,36(a1)
	move.w	d5,d3
	add.w	a3,d3
	move.w	d3,38(a1)
	move.w	d0,d4
	move.w	d1,d5
	addq.w	#1,d4
	addq.w	#1,d5
	add.w	a2,d0
	add.w	a3,d1
	tst.l	d7
	bmi	MnORet
	move.w	#RDraw,a6
	bra	MnGfx

;	ELLIPSE
; ~~~~~~~~~~~~~
MnOEl:	move.l	a4,a0
	lea	4(a4),a4
	move.w	d4,d0
	move.w	d5,d1
	move.w	(a0)+,d2
	bls	MnORien
	move.w	(a0),d3
	bls	MnORien
	add.w	d2,d4
	add.w	d3,d5
	addq.w	#1,d4
	addq.w	#1,d5
	tst.l	d7
	bmi	MnORet
	add.w	a2,d0
	add.w	a3,d1
	move.w	#DrawEllipse,a6
	bra	MnGfx

;	ICON n
; ~~~~~~~~~~~~
MnOIco	move.w	(a4)+,d1
	move.l	a2,a0
	movem.l	a2/d5/d7,-(sp)
	Rjsr	L_AdIcon
	bra.s	MnObb0

;	BOB n
; ~~~~~~~~~~~
MnOBob	move.w	(a4)+,d1
	move.l	a2,a0
	movem.l	a2/d5/d7,-(sp)
	Rjsr	L_AdBob
MnObb0	move.l	a2,a0
	movem.l	(sp)+,a2/d5/d7
	tst.w	d0
	bne	MnORien
	tst.l	(a0)
	beq	MnORien
	tst.l	d7
	bmi.s	MnObb1
	movem.l	a0-a2/d4-d7,-(sp)
	move.w	d4,d2
	move.w	d5,d3
	add.w	a2,d2
	add.w	a3,d3
	move.l	BufBob(a5),a1
	move.l	a0,a2
	moveq	#0,d4
	moveq	#-1,d5
	SyCall	Patch
	movem.l	(sp)+,a0-a2/d4-d7
MnObb1	move.l	(a0),a0
	move.w	(a0)+,d0
	lsl.w	#4,d0
	add.w	d0,d4
	add.w	(a0)+,d5
	rts

;	INK
; ~~~~~~~~~
MnOInk	move.w	(a4)+,d1
	move.w	(a4)+,d0
	tst.l	d7
	bmi	MnORet
	subq.w	#2,d1
	Rbmi	L_ObInkA
	Rbeq	L_ObInkB
	Rbra	L_ObInkC

;	SET PATTERN
; ~~~~~~~~~~~~~~~~~
MnOPat	move.w	(a4)+,d0
	tst.l	d7
	bmi	MnORet
	Rbra	L_ObPat

;	OUTLINE
; ~~~~~~~~~~~~~
MnOOut	bclr	#4,33(a1)
	move.w	(a4)+,d0
	beq.s	MnOo1
	bset	#4,33(a1)
MnOo1:	rts

;	SET LINE
; ~~~~~~~~~~~~~~
MnOSL:	move.w	(a4)+,34(a1)
	rts

;	SET FONT
; ~~~~~~~~~~~~~~
MnOSF	move.w	(a4)+,d1
	EcCall	SFont
	rts

;	SET STYLE
; ~~~~~~~~~~~~~~~
MnOSty	move.w	(a4)+,d0
	move.b	d0,56(a1)
	rts

;	SET FONT
; ~~~~~~~~~~~~~~
MnOFont	move.w	(a4)+,d1
	EcCall	SFont
	rts

;	Appel d'une fonction GFX
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnGfx	movem.l	a0/a1/d1/d6/a6,-(sp)
	move.w	a6,d6
	move.l	T_GfxBase(a5),a6
	jsr	0(a6,d6.w)
	movem.l	(sp)+,a0/a1/d1/d6/a6
	rts

;	APPEL DE PROCEDURE!
; ~~~~~~~~~~~~~~~~~~~~~~~~~
MnOCall	movem.l	a1-a3/d6-d7,-(sp)
	tst.w	Direct(a5)		* Pas en mode direct!!!
	bne	MnOcE
* Prepare les registres
	lea	CallReg(a5),a0
	moveq	#0,d0
	moveq	#0,d1
	move.w	d4,d0
	move.w	d5,d1
	add.w	a2,d0
	add.w	a3,d1
	move.l	d0,(a0)			* D0/D1---> X et Y
	move.l	d1,4(a0)
	moveq	#-1,d0
	tst.l	d7
	bpl.s	MnoC1
	moveq	#0,d0
MnoC1	move.l	d0,8(a0)		* D2------> Flag dessin
	moveq	#0,d0
	btst	#30,d7
	beq.s	MnoC2
	moveq	#-1,d0
MnoC2	move.l	d0,12(a0)		* D3------> Active ou non?
	moveq	#0,d0
	btst	#29,d7
	beq.s	MnoC3
	moveq	#-1,d0
MnoC3	move.l	d0,16(a0)		* D4------> 1er dessin?
	move.l	MnDAd(a5),a1		* A0------> Adresse menu
	move.l	a1,8*4(a0)
	move.l	MnDatas(a1),9*4(a0)	* A1------> Adresse datazone
; Appel de la procedure
	move.l	a4,a0
	add.w	(a4)+,a4
	movem.l	a4-a6,-(sp)
	move.l	MnRA3(a5),a1		Recharge les registres
	movem.l	(a1),a3-a6/d6-d7
	IFNE	Debug=2
	move.l	a3,Chr_Debug(a5)
	movem.l	d6/d7,Chr_Debug+4(a5)
	ENDC
	move.l	sp,MnPile(a5)
	Rjsr	L_MenuProcedure
; Retour de la procedure
	movem.l	(sp)+,a4-a6
	movem.l	(sp)+,a1-a3/d6-d7
	tst.l	d0
	bne.s	MnOcE
	lea	CallReg(a5),a0
	move.l	(a0),d4
	move.l	4(a0),d5
	sub.w	a2,d4
	sub.w	a3,d5
	rts
* Erreur!
MnOcE	movem.l	(sp)+,a1-a3/d6-d7
	rts

;	RESERVE ESPACE MEMOIRE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MnORes	moveq	#0,d1
	move.w	(a4)+,d1
	bls.s	MnOR2
	tst.l	d7
	bpl.s	MnOR2
* Reserve l'espace
	move.l	a2,-(sp)
	move.l	MnDAd(a5),a2
	Rbsr	L_MnODVar
	move.l	d1,d0
	SyCall	MemFastClear
	beq.s	MnOR1
	move.l	a0,d0
	move.l	d0,MnDatas(a2)
	move.w	d1,MnLData(a2)
MnOR1	move.l	(sp)+,a2
MnOR2	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Efface espace local pour procedures
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnODVar
; - - - - - - - - - - - - -
	movem.l	d0/a1,-(sp)
	moveq	#0,d0
	move.w	MnLData(a2),d0
	beq.s	MnODV1
	move.l	MnDatas(a2),a1
	SyCall	MemFree
	clr.w	MnLData(a2)
	clr.l	MnDatas(a2)
MnODV1	movem.l	(sp)+,d0/a1
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Sauve le contenu du carre D4/D5/D6/D7 / Base:	A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnSave
; - - - - - - - - - - - - -
* Efface l'ancien
	Rbsr	L_MnSaDel
	movem.l	a0-a3/d0-d7,-(sp)
	addq.w	#1,d6
	addq.w	#1,d7
* Clippe
	move.l	MnAdEc(a5),a3
	tst.w	d4
	bpl.s	MnSa1
	clr.w	d4
MnSa1:	tst.w	d5
	bpl.s	MnSa2
	clr.w	d5
MnSa2:	cmp.w	EcTx(a3),d4
	bge	MnSaX
	cmp.w	EcTy(a3),d5
	bge	MnSaX
	cmp.w	EcTx(a3),d6
	ble.s	MnSa3
	move.w	EcTx(a3),d6
MnSa3:	cmp.w	EcTy(a3),d7
	ble.s	MnSa4
	move.w	EcTy(a3),d7
MnSa4:	tst.w	d6
	bmi.s	MnSaX
	tst.w	d7
	bmi.s	MnSaX
* Calcule la taille / reserve la memoire
	lsr.w	#4,d4
	ext.l	d4
	add.w	#15,d6
	lsr.w	#4,d6
	sub.w	d4,d6
	bls.s	MnSaX
	sub.w	d5,d7
	bls.s	MnSaX
	move.w	EcTLigne(a3),d3
	ext.l	d3
	mulu	d3,d5
	add.l	d4,d5
	add.l	d4,d5
	move.w	d6,d0
	lsl.w	#1,d0
	mulu	d7,d0
	move.w	EcNPlan(a3),d4
	mulu	d4,d0
	addq.l	#8,d0
	addq.l	#8,d0
	move.l	d0,d1
	SyCall	MemFast
	bne.s	MnSaO
	move.w 	#24,MnError(a5)
	bra.s	MnSaX
MnSaO	move.l	a0,MnAdSave(a2)
	move.l	d1,(a0)+
	move.l	a3,(a0)+
	subq.w	#1,d4
	subq.w	#1,d6
	subq.w	#1,d7
	move.w	d5,(a0)+
	move.w	d6,(a0)+
	move.w	d7,(a0)+
	move.w	d4,(a0)+
* Sauve le contenu!
	lea	EcPhysic(a3),a3
MnSa5:	move.l	a3,a2
	move.w	d4,d2
MnSa6:	move.l	(a2)+,a1
	add.l	d5,a1
	move.w	d6,d1
MnSa7:	move.w	(a1)+,(a0)+
	dbra	d1,MnSa7
	dbra	d2,MnSa6
	add.l	d3,d5
	dbra	d7,MnSa5
* Ca y est!
MnSaX:	movem.l	(sp)+,a0-a3/d0-d7
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Remet la portion de decor sauvee
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnRest
; - - - - - - - - - - - - -
	movem.l	a0-a3/d0-d7,-(sp)
	move.l	MnAdSave(a2),d0
	beq.s	MnRsX
	move.l	d0,a0
	addq.l	#4,a0
	move.l	(a0)+,a3
	moveq	#0,d5
	move.w	(a0)+,d5
	move.w	(a0)+,d6
	move.w	(a0)+,d7
	move.w	(a0)+,d4
	moveq	#0,d3
	move.w	EcTLigne(a3),d3
	ext.l	d3
	lea	EcPhysic(a3),a3
MnRs1:	move.l	a3,a2
	move.w	d4,d2
MnRs2:	move.l	(a2)+,a1
	add.l	d5,a1
	move.w	d6,d1
MnRs3:	move.w	(a0)+,(a1)+
	dbra	d1,MnRs3
	dbra	d2,MnRs2
	add.l	d3,d5
	dbra	d7,MnRs1
MnRsX:	movem.l	(sp)+,a0-a3/d0-d7
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface -si present- la memoire reservee pour le fond
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnSaDel
; - - - - - - - - - - - - -
	move.l	MnAdSave(a2),d0
	beq.s	MnSad
	move.l	d0,a1
	move.l	(a1),d0
	SyCall	MemFree
	clr.l	MnAdSave(a2)
MnSad:	rts

; - - - - - - - - - - - - -
	Lib_Def	ObInkC
; - - - - - - - - - - - - -
	move.b	d0,27(a1)
	rts
; - - - - - - - - - - - - -
	Lib_Def	ObInkB
; - - - - - - - - - - - - -
	GfxCa5	SetBPen
	rts
; - - - - - - - - - - - - -
	Lib_Def	ObInkA
; - - - - - - - - - - - - -
	GfxCa5	SetAPen
	rts
; - - - - - - - - - - - - -
	Lib_Def	ObWrite
; - - - - - - - - - - - - -
	GfxCa5	SetDrMd
	rts
; - - - - - - - - - - - - -
	Lib_Def	ObPat
; - - - - - - - - - - - - -
	move.w	d0,d1
	EcCall	Pattern
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve le menu (A3)++, verifie les params, profondeur D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnFind
; - - - - - - - - - - - - -
	move.w	d5,d2
	move.w	d2,d1
	lsl.w	#2,d1
	lea	0(a3,d1.w),a1
	move.l	(a3),d0
	Rbls	L_FonCall
	cmp.l	#1024,d0
	Rbcc	L_FonCall
	move.l	MnBase(a5),d0
	beq.s	MnFE
MnF1:	move.l	-(a1),d1
	Rbls	L_FonCall
	cmp.l	#1024,d1
	Rbcc	L_FonCall
MnF0:	move.l	d0,a2
	cmp.w	MnNb(a2),d1
	beq.s	MnF2
	bcs.s	MnFE
	move.l	MnNext(a2),d0
	bne.s	MnF0
	bra.s	MnFE
MnF2:	subq.w	#1,d2
	beq.s	MnFOk
	move.l	MnLat(a2),d0
	bne.s	MnF1
MnFE:	moveq	#0,d0
	rts
MnFOk:	moveq	#-1,d0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Insere le menu A2/(A3)/D5 dans la liste
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnIns
; - - - - - - - - - - - - -
	movem.l	a0/a1/d0-d2,-(sp)
	move.w	d5,d2
	move.w	d2,d1
	subq.w	#1,d2
	lsl.w	#2,d1
	lea	0(a3,d1.w),a1
	move.l	MnBase(a5),d0
	beq	MnI8
MnI1:	move.l	-(a1),d1
MnI0:	move.l	d0,a2
	cmp.w	MnNb(a2),d1
	beq.s	MnI2
	bcs.s	MnI3
	move.l	MnNext(a2),d0
	bne.s	MnI0
	bra.s	MnI6
MnI2:	subq.w	#1,d2
	Rbmi	L_FonCall
	move.l	MnLat(a2),d0
	bne.s	MnI1
	bra.s	MnI8
* Insere?
MnI3:	tst.w	d2
	Rbne	L_FonCall
	bsr	MnRam
	move.l	MnPrev(a2),a0		* Au Milieu
	move.l	a1,MnPrev(a2)
	move.l	a0,MnPrev(a1)
	move.l	a2,MnNext(a1)
	bclr	#MnFlat,MnFlag(a2)
	beq.s	MnI5
	bset	#MnFlat,MnFlag(a1)
	move.l	a0,d0
	beq.s	MnI4
	move.l	a1,MnLat(a0)
	bra.s	MnIF
MnI4:	move.l	a1,MnBase(a5)
	bra.s	MnIF
MnI5:	move.l	a1,MnNext(a0)
	bra.s	MnIF
* Dernier objet
MnI6:	tst.w	d2
	Rbne	L_FonCall
	bsr	MnRam
	move.l	a1,MnNext(a2)
	move.l	a2,MnPrev(a1)
	bra.s	MnIF
* Cree une collaterale
MnI8:	tst.w	d2
	Rbne	L_FonCall
	bsr	MnRam
	tst.l	MnBase(a5)
	beq.s	MnI9
	move.l	a1,MnLat(a2)		* Au milieu
	move.l	a2,MnPrev(a1)
	bset	#MnFlat,MnFlag(a1)
	bra.s	MnIF
MnI9:	move.l	a1,MnBase(a5)		* Au debut
	bset	#MnFlat,MnFlag(a1)
* Fini
MnIF:	move.l	a1,a2
	move.w	d5,d0			* Flag par defaut!
	lea	MnDFlags(a5),a0
	move.b	-1(a0,d0.w),d0
	or.b	d0,MnFlag(a2)
	move.w	2(a3),MnNb(a2)		* Marque le numero!
	movem.l	(sp)+,a0/a1/d0-d2
	rts
;	Demande la RAM
; ~~~~~~~~~~~~~~~~~~~~
MnRam:	move.l	a0,-(sp)
	moveq	#MnLong,d0
	SyCall	MemFastClear
	Rbeq	L_OOfMem
	move.l	a0,a1
	move.l	(sp)+,a0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	RESET MENUS TO DEFAULT
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MenuReset
; - - - - - - - - - - - - -
	clr.w	MnMouse(a5)
	lea	MnDFlags(a5),a0
	moveq	#0,d0			* Barre de menu
	bset	#MnTotal,d0
*	bset	#MnBouge,d0
	bset	#MnTBouge,d0
	move.b	d0,(a0)+
	moveq	#0,d0
	bset	#MnBar,d0
*	bset	#MnBouge,d0
	bset	#MnTBouge,d0
	moveq	#MnNDim-1-1,d1		* Autres dimensions
.DRex3	move.b	d0,(a0)+
	dbra	d1,.DRex3
	moveq	#MnNDim-1,d0
	lea	MnChoix(a5),a0
.DRex4	clr.w	(a0)+
	dbra	d0,.DRex4
	bclr	#BitMenu,ActuMask(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface tous les menus
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnRaz
; - - - - - - - - - - - - -
MnRaz	move.l	MnBase(a5),d0
	beq.s	MnRzX
	moveq	#1,d5
	Rbsr	L_MnEff
	bra.s	MnRaz
MnRzX	clr.l	MnAdEc(a5)
	bclr	#BitMenu,ActuMask(a5)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Insere la routine dans CLEARVAR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnClearVar
; - - - - - - - - - - - - -
	move.l	a2,-(sp)
	lea	.Struc1(pc),a1
	lea	Sys_ClearRoutines(a5),a2
	SyCall	AddRoutine
	lea	.Struc2(pc),a1
	lea	Sys_DefaultRoutines(a5),a2
	SyCall	AddRoutine
	lea	.Struc3(pc),a1
	lea	Sys_ErrorRoutines(a5),a2
	SyCall	AddRoutine
	Rlea	L_MnGere,0
	move.l	a0,GoTest_Menus(a5)
	Rlea	L_MenuKeyExplore,0
	move.l	a0,GoTest_MenuKey(a5)
	move.l	(sp)+,a2
	rts
.Struc1	dc.l	0			Effacement general!
	bra.s	.Clr
.Struc2	dc.l	0
.Clr	Rbra	L_MnRaz
.Struc3	dc.l	0
	tst.w	MnProc(a5)		Procedure menu ouverte?
	beq.s	.Skip
	move.w	#-1,ErrorOn(a5)		Force l'arret du programme!
.Skip	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Enleve l'objet a partir de (D0), et ses collaterales
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnEff
; - - - - - - - - - - - - -
MnEff	movem.l	a2/d0,-(sp)
MnEf0:	move.l	d0,a2
	move.l	MnLat(a2),d0
	beq.s	MnEf1
	addq.w	#1,d5
	bsr	MnEff
MnEf1:	move.l	MnNext(a2),d0
	Rbsr	L_MnDel
	tst.w	d5
	beq.s	MnEf2
	tst.l	d0
	bne.s	MnEf0
	subq.w	#1,d5
MnEf2:	movem.l	(sp)+,a2/d0
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Enleve l'objet (A2)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MnDel
; - - - - - - - - - - - - -
	movem.l	a0-a2/d0-d2,-(sp)
* Debut d'une collaterale?
	btst	#MnFlat,MnFlag(a2)
	beq.s	MnD3
	move.l	MnNext(a2),d1
	move.l	MnPrev(a2),d0
	bne.s	MnD1
	move.l	d1,MnBase(a5)
	bra.s	MnD2
MnD1:	move.l	d0,a0
	move.l	d1,MnLat(a0)
MnD2:	tst.l	d1
	beq.s	MnD5
	move.l	d1,a1
	bset	#MnFlat,MnFlag(a1)
	move.l	d0,MnPrev(a1)
	bra.s	MnD5
* Menu normal
MnD3:	move.l	MnPrev(a2),d0
	move.l	MnNext(a2),d1
	beq.s	MnD4
	move.l	d1,a0
	move.l	d0,MnPrev(a0)
MnD4:	move.l	d0,a0
	move.l	d1,MnNext(a0)
* Libere les memoire des objets
MnD5:	move.l	MnObF(a2),d0
	beq.s	MnD7
	move.l	d0,a1
	moveq	#0,d0
	move.w	(a1),d0
	SyCall	MemFree
MnD7:	move.l	MnOb1(a2),d0
	beq.s	MnD8
	move.l	d0,a1
	moveq	#0,d0
	move.w	(a1),d0
	SyCall	MemFree
MnD8:	move.l	MnOb2(a2),d0
	beq.s	MnD9
	move.l	d0,a1
	moveq	#0,d0
	move.w	(a1),d0
	SyCall	MemFree
* Libere la copie du fond ?!?!?
MnD9:	Rbsr	L_MnSaDel
* Memoire variables locales
	Rbsr	L_MnODVar
* Libere la memoire du menu
	move.l	a2,a1
	moveq	#MnLong,d0
	SyCall	MemFree
	movem.l	(sp)+,a0-a2/d0-d2
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Tokenisation objets de menu
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	Interpreteur de definition d'objet!
;	A1	-> chaine a interpreter
;	Retour	-> D0= adresse de l'objet
; - - - - - - - - - - - - -
	Lib_Def	MnObjet
; - - - - - - - - - - - - -
	movem.l	d1-d7/a0-a3,-(sp)
	move.l	sp,a3
	move.w	(a1)+,d7
	move.l	Buffer(a5),a2
* Cherche une commande
MnO0	move.w	a2,d0
	btst	#0,d0
	beq.s	MnO1
	clr.b	(a2)+
MnO1	move.l	a1,d5
	move.w	d7,d6
MnO2	bsr	MnOChr
	bhi.s	MnO2
* Texte normal
	move.w	d6,d0
	sub.w	d7,d0
	beq.s	MnO4
	move.w	#4,(a2)+
	move.w	d0,(a2)+
	move.l	d5,a1
	move.w	d6,d7
MnO3	bsr	MnOChr
	bls.s	MnO0
	move.b	d0,(a2)+
	bra.s	MnO3
* Interprete la commande
MnO4	tst.w	d7
	beq	MnOX
	subq.w	#1,d7
	addq.l	#1,a1
MnO5	bsr	MnOTok
* Cherche la commande suivante
	bsr	MnOChS
	bls	MnOE1
	cmp.b	#")",d0
	beq	MnO0
	cmp.b	#":",d0
	beq.s	MnO5
	bra	MnOE1
;------ Marque la fin!
MnOX:	clr.w	(a2)+
* Reserve la memoire
	move.l	a2,d0
	move.l	Buffer(a5),a1
	sub.l	a1,d0
	addq.w	#2,d0
	move.w	d0,d1
	SyCall	MemFastClear
	beq.s	MnOXx
	move.l	a0,d0
	move.w	d1,(a0)+
	subq.w	#2,d1
	lsr.w	#1,d1
	subq.w	#1,d1
	bmi.s	MnOXx
MnOX1:	move.w	(a1)+,(a0)+
	dbra	d1,MnOX1
* Ok!
MnOXx:	move.l	a3,sp
	movem.l	(sp)+,d1-d7/a0-a3
	tst.l	d0
	rts
* Erreur de syntaxe
MnOE1	moveq	#-1,d0
	bra.s	MnOXx

;------ Tokenise (a2)
MnOTok:	bsr	MnOChS
	bls	MnOE1
	Rjsr	L_MajD0
	lsl.w	#8,d0
	bsr	MnOChr
	bls	MnOE1
	Rjsr	L_MajD0
	move.w	d0,d1
* Saute le reste du nom
MnOT0:	bsr	MnOChr
	bls	MnOE1
	Rjsr	L_MajD0
	cmp.b	#"A",d0
	bcs.s	MnOTa
	cmp.b	#"Z",d0
	bls.s	MnOT0
MnOTa	addq.w	#1,d7
	subq.l	#1,a1
* Explore la table des tokens
	lea	MnOToken(pc),a0
	moveq	#8,d2
MnOT1	tst.w	(a0)
	beq	MnOE1
	cmp.w	(a0)+,d1
	beq.s	MnOT2
	addq.l	#2,a0
	addq.w	#4,d2
	bra.s	MnOT1
MnOT2:	move.w	d2,(a2)+
	move.b 	(a0)+,d6
	beq.s	MnOT4
	bmi.s	MnOT5
MnOT3:	bsr	MnOExp
	subq.b	#1,d6
	beq.s	MnOT4
	bsr	MnOChS
	bls	MnOE1
	cmp.b	#",",d0
	beq.s	MnOT3
	bra	MnOE1
MnOT4:	rts
* Appel de procedure: stocke le nom
MnOT5:	moveq	#0,d6
	move.l	a2,a0
	clr.w	(a2)+
MnoT6:	bsr	MnOChS
	beq.s	MnoT8
	cmp.b	#")",d0
	beq.s	MnoT7
	cmp.b	#":",d0
	beq.s	MnoT7
	move.b	d0,(a2)+
	addq.w	#1,d6
	bra.s	MnoT6
MnoT7:	subq.l	#1,a1
	addq.w	#1,d7
MnoT8:	tst.w	d6
	beq	MnOE1
	btst	#0,d6
	beq.s	MnoT9
	clr.b	(a2)+
	addq.w	#1,d6
MnoT9:	move.w	d6,(a0)
	rts

;------ EXPRESSION
MnOExp:	bsr	MnOLong
	move.w	d0,(a2)+
	rts
* Conversion DEC-> HEXA
MnOLong	moveq	#0,d0
	moveq	#1,d3
	bsr	MnOChS
	cmp.b	#"-",d0
	bne.s	Mnh0
	subq.w	#1,d3
	bsr	MnOChS
Mnh0:	sub.b	#"0",d0
	bcs	MnOE1
	cmp.b	#10,d0
	bcc	MnOE1
	move.l	d0,d1
	subq.w	#1,d3
Mnh1:   bsr 	MnOChS
	sub.b	#"0",d0
	bcs.s	Mnh2
	cmp.b 	#10,d0
        bcc.s 	Mnh2
	add.l	d1,d1
	move.l	d1,d2
	lsl.l	#2,d1
	add.l	d2,d1
	add.l	d0,d1
	bra.s	Mnh1
Mnh2:   subq.l 	#1,a1
	addq.w	#1,d7
        tst 	d3
	beq.s	MnhX
	bpl	MnOE1
	neg.l	d1
MnhX:	move.l	d1,d0
	rts

;------ CHR GET
MnOChS	moveq	#33,d4		* Saute les espace
	bra.s	MnOCh0
MnOChr	moveq	#32,d4		* Ramene les espaces
MnOCh0	tst.w	d7
	beq.s	MnOChX
	move.b	(a1)+,d0
	subq.w	#1,d7
	cmp.b	#"(",d0
	beq.s	MnOCh2
	cmp.b	#27,d0
	beq.s	MnOCh1
	cmp.b	d4,d0
	bcs.s	MnOCh0
	moveq	#1,d4
MnOChX	rts
MnOCh1	addq.l	#2,a1
	subq.w	#2,d7
	bpl.s	MnOCh0
	bmi	MnOE1
MnOCh2	subq.l	#1,a1
	addq.w	#1,d7
	moveq	#0,d0
	rts

;	Instructions
; ~~~~~~~~~~~~~~~~~~
MnOToken
	dc.b	"BA",2,0		* Bar
	dc.b	"LI",2,0		* Line
	dc.b	"EL",2,0		* Ellipse
	dc.b	"PA",1,0		* Pattern
	dc.b 	"IN",2,0		* Ink
	dc.b 	"BO",1,0		* Bob
	dc.b	"IC",1,0		* Icon
	dc.b	"LO",2,0		* Locate
	dc.b 	"OU",1,0		* Out line
	dc.b 	"SL",1,0		* Set Line
	dc.b	"SF",1,0		* Set Font
	dc.b 	"PR",-1,0		* CALL procedure
	dc.b	"RE",1,0		* Reserve space
	dc.b	"SS",1,0		* Set style
	dc.w	0


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Exploration des MENU KEY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	MenuKeyExplore
; - - - - - - - - - - - - -
	movem.l	a0-a3/d0-d7,-(sp)
	link	a3,#-(MnNDim*2+2)
	move.l	a3,a1
	move.w	T_ClLast+2(a5),d4
	move.b	T_ClLast+1(a5),d5
	move.b	T_ClLast(a5),d6
	move.l	MnBase(a5),d0
	bsr	MnKE0
	clr.l	T_ClLast(a5)
	unlk	a3
	movem.l	(sp)+,a0-a3/d0-d7
	rts
;	Routine recursive
; ~~~~~~~~~~~~~~~~~~~~~~~~
MnKE0	clr.w	-2(a1)
MnKE1	move.l	d0,a2
	move.w	MnNb(a2),d7
	move.l	MnLat(a2),d0
	beq.s	MnKE2
	move.w	d7,-(a1)
	move.l	a2,-(sp)
	bsr	MnKE0
	move.l	(sp)+,a2
	clr.w	(a1)+
	bra.s	MnKEN
* Compare les touche (si fin de branche)
MnKE2	move.b	MnKFlag(a2),d0
	beq.s	MnKEN
	bmi.s	MnKE3
* Code ASCII
	cmp.b	MnKAsc(a2),d4
	beq.s	MnKET
	bne.s	MnKEN
* Shift et Scancode
MnKE3	cmp.b	MnKSc(a2),d5
	bne.s	MnKEN
	cmp.b	MnKSh(a2),d6
	beq.s	MnKET
* Suivant
MnKEN	move.l	MnNext(a2),d0
	bne.s	MnKE1
	rts
;	Trouve!!!
; ~~~~~~~~~~~~~~~
MnKET:	lea	MnChoix(a5),a0
	move.l	a0,a1
	moveq	#MnNDim-1,d0
MnKet1	clr.w	(a0)+
	dbra	d0,MnKet1
	move.l	a3,a0
MnKet2	move.w	-(a0),d0
	beq.s	MnKet3
	move.w	d0,(a1)+
	addq.l	#8,sp
	bra.s	MnKet2
MnKet3	move.w	d7,(a1)+
	move.w	#-1,MnChoice(a5)
	bset	#BitJump,T_Actualise(a5)
	rts

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
	Lib_Def	Dsk.FileSelector
; - - - - - - - - - - - - -
	movem.l	d2-d7/a2-a6,-(sp)

; Monitor present?
; ~~~~~~~~~~~~~~~~
	tst.l	Patch_Errors(a5)
	Rbne	L_FonCall

; Branche la routine de FLUSH / Routine END
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	Fs_FlushStructure(pc),a1
	SyCall	AddFlushRoutine
	lea	Fs_EndStructure(pc),a1
	lea	Sys_EndRoutines(a5),a2
	SyCall	AddRoutine

; Reserve la zone de data
; ~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#Fs_Long,d0
	SyCall	MemFastClear
	beq	Fs_OoMem
	move.l	a0,a4
	move.l	a0,Fs_Base(a5)
	move.l	sp,Fs_Sp(a4)

; Patch pour les erreurs
; ~~~~~~~~~~~~~~~~~~~~~~
	lea	Fs_Error(pc),a0
	move.l	a0,Fs_ErrPatch(a5)

; Stocke le numero de l'ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#-1,d0
	move.l	T_EcCourant(a5),d1
	beq.s	.Skip
	move.l	d1,a0
	move.w	EcNumber(a0),d0
.Skip	move.w	d0,Fs_OldEc(a4)

; Stocke les limites de la souris
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	T_MouXMin(a5),a0
	lea	Fs_LimSave(a4),a1
	moveq	#3,d0
.Mou	move.w	(a0)+,(a1)+
	dbra	d0,.Mou
	move.l	#128,d1
	moveq	#30,d2
	move.l	#448,d3
	move.l	#312,d4
	SyCall	LimitM

; Assez de memoire?
; ~~~~~~~~~~~~~~~~~
	move.l	$4.w,a6
	move.l	#Public,d1
	jsr	_LVOAvailMem(a6)
	cmp.l	#32*1024,d0
	bcs	Fs_LowMemory

; Ouverture de l'ecran
; ~~~~~~~~~~~~~~~~~~~~
	move.w	PI_FsDSx(a5),d1
	move.w	PI_FsDSy(a5),d2
	bsr	Fs_ScOpen
	beq.s	.Ok
	move.w	#320,d1
	move.w	#128,d2
	bsr	Fs_ScOpen
	beq	Fs_LowMemory
.Ok	move.l	a0,Fs_AdEc(a4)
	lea	Fs_COff(pc),a1
	WiCall	Print

; Encore assez de memoire?
; ~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	$4.w,a6
	move.l	#Public,d1
	jsr	_LVOAvailMem(a6)
	cmp.l	#12*1024,d0
	bcs	Fs_LowMemory

; Ouverture du canal de dialogue...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Fs_GetPuzzle
	add.l	6(a0),a0		DEUXIEME programme
	move.l	#Fs_ChannelN,d0		Numero du canal
	move.l	#256*5,d1		Buffer
	moveq	#FsV_Max,d2		Nombre de variables
	moveq	#0,d3			Ne PAS recopier
	Rbsr	L_Dia_OpenChannel
	tst.l	d0			Une erreur?
	bne	Fs_BadAPI
	move.l	a0,Fs_Channel(a4)
	move.l	a1,Fs_Variables(a4)
	bset	#4,Dia_Flags(a0)	Flag, Return non active sur edit...
	bsr	Fs_GetInputs		Recupere les entrees dans le DBL

; Sort / Size par default
; ~~~~~~~~~~~~~~~~~~~~~~~
	move.b	PI_FsSort(a5),FsV_Sort+3(a1)
	move.b	PI_FsSize(a5),FsV_Size+3(a1)
	move.b	PI_FsStore(a5),FsV_Store+3(a1)

; Tableau magique!
; ~~~~~~~~~~~~~~~~
	lea	Fs_Array(a4),a0
	move.l	a0,FsV_Array(a1)
	move.b	#1,Fs_Array(a4)
	move.b	#2,Fs_Array+1(a4)
	clr.w	Fs_ASize(a4)
	move.l	#"MgIk",Fs_AMagic(a4)
	lea	Fs_GetName(pc),a0
	move.l	a0,Fs_ACall(a4)

; Dessin du fond
; ~~~~~~~~~~~~~~
	move.l	#Fs_ChannelN,d0
	moveq	#-1,d1
	moveq	#0,d2
	moveq	#0,d3
	Rbsr	L_Dia_RunProgram
	tst.l	d0
	bne	Fs_BadAPI
	bsr	FsI_SliStore

; Apparition de l'�cran
; ~~~~~~~~~~~~~~~~~~~~~
	bsr	Fs_Appear
	move.b	#1,Fs_Opened(a4)

; Demarrage de la tache de dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	TaskBase(pc),a0
	move.l	a5,(a0)
	move.l	#4*512,-(sp)
	pea	FsI_Entry(pc)
	move.l	#-1,-(sp)
	pea	TaskName(pc)
	Rbsr	L_CreateTask

; Init de Fill-File
; ~~~~~~~~~~~~~~~~~
	Rjsr	L_NoReq				No more requester
	move.l	PathAct(a5),a0			Path par defaut
	tst.b	(a0)				Si non defini
	bne.s	.SkipP
	Rjsr	L_AskDir			Le demande
	Rjsr	L_CopyPath
.SkipP	Rjsr	L_FillFFree			FillFree
	move.l	Fs_Variables(a4),a0
	move.w	#64,FillFSize(a5)		Taille maxi
	clr.w	FillF32(a5)			Pas de 32
	bsr	Fs_First

; Boucle d'attente...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Loop
	tst.b	Fs_DirOn(a4)
	beq.s	.Wait
	bsr	Fs_Next			Prend le nom suivant
	bra.s	.Suit
.Wait	Rjsr	L_Sys_WaitMul
.Suit	tst.w	Fs_Waiting(a4)
	bmi	Fs_Cancel
	move.w	Fs_Input(a4),d0
	beq.s	Fs_Loop
	lsl.w	#2,d0
	lea	Fs_Jumps(pc),a0
	jsr	-4(a0,d0.w)		Appelle la fonction
	clr.w	Fs_Waiting(a4)
	clr.w	Fs_Input(a4)
	bra.s	Fs_Loop

; Saut aux routines de gestion
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Jumps
	bra	Fs_Ok			1 - Ok
	bra	Fs_Cancel		2 - Cancel
	bra	Fs_Parent		3 - Parent
	bra	Fs_Device		4 - Devices
	bra	Fs_Assign		5 - Assigns
	bra	Fs_GDir			6 - Get Dir
	bra	Fs_Sort			7 - Sort
	bra	Fs_Size			8 - Sizes
	bra	Fs_Rien			9 -
	bra	Fs_Rien			10- Up
	bra	Fs_Rien			11- Down
	bra	Fs_Rien			12- Slider V
	bra	Fs_Name			13- Active List
	bra	Fs_GDir			14- Edit Path
	bra	Fs_Return		15- Edit Name
	bra	Fs_BStore		16- Bouton store
	bra	Fs_SliDel		17- Del
	bra	Fs_StoreList		18- Slider 2
	bra	Fs_Help			19- Help
	bra	Fs_Droite		20- Bouton de droite
Fs_Rien	rts

; HELP: trouve le premier fichier dans la liste
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Help	bsr	Fs_GetFName
	tst.b	(a1)
	beq.s	.Skip
	move.b	#" ",-(a1)		Un espace au debut!
	move.l	a1,a0
	move.l	Fs_Variables(a4),a2
	moveq	#0,d0
	tst.b	FillFSorted(a5)		Si pas sorted, � partir du courant!
	bne.s	.Sl
; SORT pas en route
	move.l	FsV_PList(a2),d0
	addq.l	#1,d0
	move.l	a0,-(sp)
	Rjsr	L_FillFFind
	move.l	(sp)+,a0
	tst.w	d0
	bpl.s	.S0
	moveq	#0,d0
; SORT en route
.Sl	Rjsr	L_FillFFind		Va chercher
	tst.w	d0
	bmi.s	.S3
.S0	add.w	FsV_Ty+2(a2),d0
	cmp.w	FillFNb(a5),d0
	bls.s	.S1
	move.w	FillFNb(a5),d0
.S1	sub.w	FsV_Ty+2(a2),d0
	bcc.s	.S2
	moveq	#0,d0
.S2	move.l	d0,FsV_PList(a2)
	move.l	d0,FsV_PosFirst(a2)
.S3	bsr	FsI_AffF
.Skip	rts

; Trouve le VRAI nom du fichier, en extrayant un eventuel pathname.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Path:	Buffer+640 = A0
;	Name:	Buffer+768 = A1
Fs_GetFName
	move.l	a2,-(sp)
	move.l	Buffer(a5),a2
	lea	640(a2),a2
	clr.b	(a2)
	clr.b	768(a2)
	move.l	#Fs_ChannelN,d0
	moveq	#Fs_FileN,d1
	moveq	#0,d2
	Rbsr	L_Dia_GetValue
	move.l	d1,a1
	tst.b	(a1)
	beq.s	.Ex1
; Trouve le dernier signe de pathname
	move.l	a1,d1
	move.l	a1,a0
.Lp1	move.b	(a0)+,d0
	beq.s	.Ex1
	cmp.b	#":",d0
	beq.s	.Ok1
	cmp.b	#"/",d0
	bne.s	.Lp1
.Ok1	move.l	a0,d1
	bra.s	.Lp1
; Recopie l'eventuel pathname
.Ex1	cmp.l	a1,d1
	beq.s	.Sk2
	move.l	a2,a0
.Lp2	move.b	(a1)+,(a0)+
	cmp.l	a1,d1
	bne.s	.Lp2
	clr.b	(a0)
; Recopie le nom
.Sk2	move.l	d1,a0
	lea	128(a2),a1
.Lp3	move.b	(a0)+,(a1)+
	bne.s	.Lp3
; Retourne les adresses
	move.l	a2,a0
	lea	128(a0),a1
	move.l	(sp)+,a2
	rts


; Return dans le nom du fichier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Return
	bsr	Fs_GetFName
	tst.b	(a0)			Si pas de path >>> OK!
	beq.s	Fs_Ok
	movem.l	a0/a1,-(sp)
	bsr	Fs_Store
	bsr	FsI_SliStore
	movem.l	(sp)+,a0/a1
; Enleve le path du nom
	move.l	a0,-(sp)
	move.l	a1,a0
	bsr	Fs_NewName
	move.l	(sp)+,a0
; Compute le nouveau path
.Skip	move.l	a0,a2
	move.l	Name1(a5),a1
.Lp1	move.b	(a2)+,d0
	beq.s	.Plus
	cmp.b	#":",d0
	bne.s	.Lp1
	clr.b	(a1)			Un nom de device=> RAZ path
.Plus	bsr	Fs_NewDir
	rts

; OK!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Ok	bsr	Fs_Store
	bsr	Fs_NomDir
; Recopie NAME1 dans PATHACT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Name1(a5),a0
	move.l	PathAct(a5),a1
.Cp	move.b	(a0)+,(a1)+
	bne.s	.Cp
; Demande le fichier selectionne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#Fs_ChannelN,d0
	moveq	#Fs_FileN,d1
	moveq	#0,d2
	Rbsr	L_Dia_GetValue
	move.l	d1,a1			Si NUL>>> va en CANCEL!
	tst.b	(a1)
	beq	Fs_Cancel
; Copie le nom � la suite de name 1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Name1(a5),a0
.Fin	tst.b	(a0)+
	bne.s	.Fin
	subq.l	#1,a0
.Copy	move.b	(a1)+,(a0)+
	bne.s	.Copy
	subq.l	#1,a0
	move.l	Name1(a5),d0
	sub.l	d0,a0
	exg	d0,a0
	bra	Fs_Close

; CANCEL
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Cancel
	bsr	Fs_Store
	moveq	#0,d0
	move.l	Name1(a5),a0
	clr.b	(a0)
	bra	Fs_Close

; SORT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Sort	move.l	Fs_Variables(a4),a0
	tst.l	FsV_Sort(a0)
	beq.s	.Skip
	Rjsr	L_FillSort
	move.b	#-1,FillFSorted(a5)
	bsr	FsI_AffF
.Skip	rts

; SIZE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Size	bsr	FsI_AffF
	rts

; GET DIR
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_GDir	bsr	Fs_New
	bsr	Fs_First
	rts

; STORE-LIST
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_StoreList
	tst.b	PI_FsStore(a5)
	beq	.Exit
	bsr	Fs_Store
	bsr	Fs_New
	move.b	#3,Fs_DevFlag(a4)
	move.b	#-1,FillFSorted(a5)
	move.l	Buffer(a5),a1
	lea	128(a1),a3		Faux premier fichier
	clr.l	124(a3)
	clr.b	8(a3)
	moveq	#" ",d2
	move.l	a3,a2
	Rjsr	L_FillFPoke
	clr.w	FillFNb(a5)		RAZ nombre de fichiers
; Explore la liste.
	move.l	Fs_Liste(a5),d2
	beq.s	.Out
.Loop	move.l	d2,a2
	move.l	a2,-(sp)
	lea	8+4+2+4+1(a2),a0
	move.l	Buffer(a5),a1
	move.l	a1,d1
.Cp1	move.b	(a0)+,(a1)+
	bne.s	.Cp1
	subq.l	#1,a1
.Cp2	move.b	(a0)+,(a1)+
	bne.s	.Cp2
	move.l	a1,d1
	subq.l	#1,d1
	sub.l	Buffer(a5),d1		Longueur du nom
	move.l	Fs_Variables(a4),a0
	move.l	FsV_Tx(a0),d2		Longueur maxi
	cmp.w	d2,d1
	bls.s	.Long
	move.w	d2,d1
	subq.w	#1,d1
.Long	addq.w	#1,d1
	sub.w	d1,a1			Pointe le debut a recopier
	lea	8(a3),a0
.Cp	move.b	(a1)+,(a0)+
	bne.s	.Cp
	move.l	a2,d0
	lsr.l	#1,d0
	bset	#31,d0			Pas de taille dans l'affichage
	move.l	d0,124(a3)		Adresse de la structure
	moveq	#" ",d2			Va poker le nom dans la liste
	move.l	a3,a2
	Rjsr	L_FillFPoke
	move.l	(sp)+,a2
	move.l	(a2),d2
	bne.s	.Loop
; Va afficher
.Out	bsr	FsI_AffF
	bsr	FsI_SliStore
.Exit	rts


; Bouton de droite: DEVICES / ASSIGNS / Directory
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Droite
	move.b	Fs_DevFlag(a4),d0
	beq.s	.Dev
	cmp.b	#1,d0
	beq.s	.Ass
	cmp.b	#2,d0
	beq.s	.Store
.List	bsr	Fs_NomDir
	lea	.Null(pc),a0		Retour aux fichiers
	bsr	Fs_NewDir
	bra.s	.End
.Dev	bsr	Fs_Device		Devices
	bra.s	.End
.Ass	bsr	Fs_Assign		Assigns
	bra.s	.End
.Store	tst.b	PI_FsStore(a5)		Store en route?
	beq.s	.List
	bsr	Fs_StoreList		Stored directories
; Attend le relachement de la touche de droite
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.End	bsr	Fs_Multi
	SyCall	MouseKey
	btst	#1,d1
	bne.s	.End
	rts
.Null	dc.w	0

; DEVICES
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Device
	bsr	Fs_Store
	moveq	#"D",d0
	moveq	#1,d1
	bra.s	Fs_Dev
; ASSIGNS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Assign
	bsr	Fs_Store
	moveq	#"A",d0
	moveq	#2,d1
Fs_Dev	move.w	d1,-(sp)
	move.l	Name1(a5),a0
	move.b	d0,(a0)
	move.l	Name2(a5),a0
	clr.b	(a0)
	bsr	Fs_New
	Rjsr	L_FillDev
	move.w	(sp)+,d0
	move.b	d0,Fs_DevFlag(a4)
	bsr	FsI_AffF
	bsr	FsI_SliStore
	rts

; Click dans la liste des noms
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Name	move.l	#Fs_ChannelN,d0		Demande la position dans la liste
	moveq	#Fs_ListN,d1
	moveq	#0,d2
	Rbsr	L_Dia_GetValue
	move.l	d1,d0
	move.l	d1,d2
	Rjsr	L_FillGet
	bne.s	.Quoi
	rts
; Un fichier? Un directory? Un device?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Quoi	lea	6+4(a0),a2		Debut du nom
	cmp.b	#1,Fs_DevFlag(a4)	Un device?
	beq.s	.Device
	cmp.b	#2,Fs_DevFlag(a4)	Un Assign?
	beq.s	.Device
	cmp.b	#3,Fs_DevFlag(a4)	Un directory store?
	beq.s	.Stored
	cmp.b	#" ",(a2)+
	beq.s	.Fiche
; Changement de directory!
; ~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Fs_Store
	bsr	Fs_NomDir
	move.l	a2,a0
	bsr	Fs_NewDir
	rts
; Un nom de fichier
; ~~~~~~~~~~~~~~~~~
.Fiche	cmp.w	Fs_Click(a4),d2		Double click?
	beq	Fs_Ok
	move.w	d2,Fs_Click(a4)
	move.l	a2,a0
	bsr	Fs_NewName
	rts
; Un device.
; ~~~~~~~~~~
.Device	move.w	#-1,Fs_Click(a4)
	clr.b	Fs_DevFlag(a4)
	bsr	Fs_NomDir
	move.l	Name1(a5),a0
	clr.b	(a0)
	lea	1(a2),a0
	bsr	Fs_NewDir
	rts
; Un directory stored
; ~~~~~~~~~~~~~~~~~~~
.Stored	move.w	#-1,Fs_Click(a4)
	clr.b	Fs_DevFlag(a4)
	move.l	-4(a2),d0		Adresse du directory stocke!
	lsl.l	#1,d0			Enleve le bit#31
	bsr	Fs_StoDir2		Va changer le directory!
	rts

; Routine: recopie du nom A2 >>> Nom de fichier...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_NewName
	move.l	a0,a2
	move.l	Buffer(a5),a0		Copie le nom
	lea	512(a0),a0
	lea	2(a0),a1
.Cp1	move.b	(a2)+,(a1)+
	bne.s	.Cp1
	sub.l	a0,a1
	subq.w	#3,a1
	move.w	a1,(a0)
	move.l	#Fs_ChannelN,d0		Update la zone FILE NAME
	moveq	#Fs_FileN,d1
	move.l	a0,d2
	Rbsr	L_Dia_Update
	rts

; PARENT
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Parent
	bsr	Fs_NomDir
	move.l	Name1(a5),a0
	tst.b	(a0)
	beq.s	.PaX
.Pa0	tst.b	(a0)+
	bne.s	.Pa0
	subq.l	#1,a0
	cmp.b	#"/",-(a0)
	bne.s	.PaX
.Pa1:	cmp.l	Name1(a5),a0
	beq.s	.Pa3
	move.b	-(a0),d0
	cmp.b	#"/",d0
	beq.s	.Pa2
	cmp.b	#":",d0
	bne.s	.Pa1
.Pa2	lea	1(a0),a0
.Pa3	move.l	a0,-(sp)
	bsr	Fs_Store
	bsr	FsI_SliStore
	move.l	(sp)+,a0
	clr.b	(a0)
	bsr	Fs_NewDir
.PaX	rts

; Bouton store
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_BStore
	move.l	Fs_Variables(a4),a0
	move.b	FsV_Store+3(a0),PI_FsStore(a5)
	bne.s	.Exit
	bsr	Fs_DelStores
	bsr	FsI_SliStore
.Exit	rts

; Change le directory >>> Directory STORE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_StoDir
	moveq	#126,d0			Retourne le dernier element
	bsr	Fs_NumStore
Fs_StoDir2
	move.l	d0,a0
	lea	8+4+4+2+1(a0),a0	Recopie les noms >>> name1/name2
	move.l	Name1(a5),a1
.Cop1	move.b	(a0)+,(a1)+
	bne.s	.Cop1
	move.l	Name2(a5),a1
.Cop2	move.b	(a0)+,(a1)+
	bne.s	.Cop2
	subq.l	#1,a0			Reste sur un zero!
	bsr	Fs_NewDir		Il le trouve tout seul!
.Exit	rts

; Detruit le directory affiche
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_SliDel
	tst.b	PI_FsStore(a5)		Securites...
	beq.s	.Exit
	tst.l	Fs_Liste(a5)
	beq.s	.Exit
	bsr.s	Fs_StoDir
	bsr	FsI_SliStore
.Exit	rts

; ___________________________________________________________________
;
; 	Erreur dans le selecteur
; ___________________________________________________________________
;
Fs_Error
	move.l	Fs_Base(a5),a4
	move.l	Fs_Sp(a4),sp
; Arrete la tache de dialogue
	bsr	FsI_Stop
; Trouve le message...
	move.l	a6,-(sp)
	move.l	DosBase(a5),a6
	jsr	_LVOIoErr(a6)
	move.l	(sp)+,a6
	lea	FsErDisk(pc),a0
	moveq	#24,d1
.Loop	addq.l	#1,d1
	move.w	(a0)+,d2
	bmi.s	.No
	cmp.w	d0,d2
	bne.s	.Loop
	bra.s	.Ok
.No	moveq	#32,d1
.Ok	bsr	Fs_GetPuzzle
	move.l	a2,a0
	move.w	d1,d0
	Rjsr	L_GetMessage
	move.l	a0,-(sp)
; Efface tout!
	bsr	Fs_New
; Affiche le message
	bsr	Fs_GetPuzzle
	add.l	10(a0),a0		TROISIEME programme
	moveq	#0,d0			X
	moveq	#0,d1			Y
	moveq	#0,d2			Variable 0
	move.l	(sp)+,d3
	subq.w	#2,d3			Message d'erreur
	moveq	#0,d4			Rien!
	moveq	#0,d5			Ne pas copier le programme
	move.l	#256,d6			Buffer
	Rbsr	L_Dia_RunQuick
	bsr	FsI_ReStart
	tst.w	d0
	bne	Fs_BadAPI
	clr.w	Fs_Input(a4)
	bra	Fs_Loop

; ___________________________________________________________________
;
; 	Fermeture du s�lecteur de fichier / Sortie
; ___________________________________________________________________
;
;	A0/D0=	Message de retour
;
Fs_Close
	move.l	Fs_Sp(a4),sp
	movem.l	a0/d0,-(sp)
; Stocke le dernier directory
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Fs_Store
; Remet le requester
; ~~~~~~~~~~~~~~~~~~
	Rjsr	L_YesReq
; Enleve le patch erreurs
; ~~~~~~~~~~~~~~~~~~~~~~~
	clr.l	Fs_ErrPatch(a5)
; Referme le canal de dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.l	Fs_Channel(a4)
	beq.s	.NoChan
	cmp.l	#-1,Fs_Channel(a4)
	beq.s	.LowM
; Arret de la tache
	bsr	FsI_Quit
; Recupere les flags
	move.l	Fs_Variables(a4),a0
	move.b	FsV_Sort+3(a0),PI_FsSort(a5)
	move.b	FsV_Size+3(a0),PI_FsSize(a5)
	move.b	FsV_Store+3(a0),PI_FsStore(a5)
; Ferme le canal
.LowM	move.l	#Fs_ChannelN,d0
	Rbsr	L_Dia_CloseChannel
.NoChan
; Referme l'�cran
; ~~~~~~~~~~~~~~~
	tst.b	Fs_Opened(a4)
	beq.s	.PaFer
	move.l	Fs_AdEc(a4),a2
; Recupere la position de l'�cran
	move.w	EcAWX(a2),PI_FsDWx(a5)
	move.w	EcAWY(a2),PI_FsDWy(a5)
; Referme l'�cran
	bsr	Fs_DisAppear
.PaFer
; Detruit l'�cran
; ~~~~~~~~~~~~~~~
	tst.l	Fs_AdEc(a4)
	beq.s	.PaClo
	EcCalD	Del,EcFsel
	move.w	Fs_OldEc(a4),d1
	bmi.s	.PaClo
	EcCall	Active
.PaClo
; Remet la souris
; ~~~~~~~~~~~~~~~
	lea	Fs_LimSave(a4),a0
	lea	T_MouXMin(a5),a1
	moveq	#3,d0
.Mou	move.w	(a0)+,(a1)+
	dbra	d0,.Mou
; Efface les touches
; ~~~~~~~~~~~~~~~~~~
	SyCall	ClearKey
	clr.l	T_ClLast(a5)
; Efface le buffer de datas
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a4,a1
	move.l	#Fs_Long,d0
	SyCall	MemFree
	clr.l	Fs_Base(a5)
; On revient!
; ~~~~~~~~~~~
	movem.l	(sp)+,a0/d0		Recupere le r�sultat
	movem.l	(sp)+,d2-d7/a2-a6	Recupere les donnees
	lea	4*4(a3),a3		Depile les parametres...
	rts

; ___________________________________________________________________
;
;	STORE
; ___________________________________________________________________


; Stocke le directory courant (name1-name2) dans la liste
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Store
	tst.b	PI_FsStore(a5)		Store en route?
	beq.s	.Exit
	tst.b	Fs_DevFlag(a4)		Pas les devices
	bne.s	.Exit
	tst.l	BufFillF(a5)		Quelque chose � stocker?
	beq.s	.Exit
	bsr	Fs_FindStore		Deja defini?
	beq.s	.Skip
	bsr	Fs_DelStore		Si oui, on enleve!
.Skip	moveq	#Fs_MaxStore,d0
	bsr	Fs_DelLast		Pas plus de 10!
	lea	Fs_Liste(a5),a0		Base de la liste
	move.l	#16+128,d0		Liste + Nom
	Rjsr	L_Lst.New
	beq.s	.Exit
	addq.l	#8,a1			Adresse dans la liste
	move.l	BufFillF(a5),(a1)+	Poke la liste
	move.w	FillFNb(a5),(a1)+
	move.l	Fs_Variables(a4),a0
	move.l	FsV_PList(a0),(a1)+	Position dans la liste
	move.b	FillFSorted(a5),(a1)+
	clr.l	BufFillF(a5)		Enleve de DiskIo!
	clr.w	FillFNb(a5)
	clr.b	FillFSorted(a5)
	move.l	Name1(a5),a0		Copie le pathname
.Cop1	move.b	(a0)+,(a1)+
	bne.s	.Cop1
	move.l	Name2(a5),a0		Copy le filtre
.Cop2	move.b	(a0)+,(a1)+
	bne.s	.Cop2
.Exit	Rjsr	L_FillFFree		Enleve le lock!
	rts

; Branche le directory D0 sur le directory courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Branch
	move.l	d0,-(sp)		Enleve la liste precedente
	Rjsr	L_FillFFree
	move.l	(sp)+,d0
	move.l	d0,a0
	addq.l	#8,a0
	move.l	(a0)+,BufFillF(a5)	Branche la nouvelle
	move.w	(a0)+,FillFNb(a5)
	move.l	Fs_Variables(a4),a1	Position dans la liste
	move.l	(a0)+,FsV_PList(a1)
	move.b	(a0)+,FillFSorted(a5)
	clr.b	Fs_DirOn(a4)		Rien en route!
	move.l	d0,a1
	lea	Fs_Liste(a5),a0
	Rjsr	L_Lst.Del		Enleve la structure
	rts

; Trouve le directory courant (NAME1-NAME2) dans la liste
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_FindStore
	move.l	a2,-(sp)
	tst.b	PI_FsStore(a5)
	beq.s	.Exit
	move.l	Fs_Liste(a5),d0
	beq.s	.Exit
.Loop	move.l	d0,a2
	lea	8+4+2+4+1(a2),a1
	move.l	Name1(a5),a0		Regarde le pathname
.Chk1	move.b	(a1)+,d0
	Rbsr	L_MajD0
	move.b	d0,d1
	move.b	(a0)+,d0
	Rbsr	L_MajD0
	cmp.b	d0,d1
	bne.s	.Next
	tst.b	d0
	bne.s	.Chk1
	move.l	Name2(a5),a0		Regarde le filtre
.Chk2	move.b	(a1)+,d0
	Rbsr	L_MajD0
	move.b	d0,d1
	move.b	(a0)+,d0
	Rbsr	L_MajD0
	cmp.b	d0,d1
	bne.s	.Next
	tst.b	d0
	bne.s	.Chk2
	bra.s	.Found
.Next	move.l	(a2),d0			Encore un?
	bne.s	.Loop
.Exit	moveq	#0,d0
	bra.s	.Out
.Found	move.l	a2,d0
.Out	move.l	(sp)+,a2
	rts

; Detruit les elements de la liste >D0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_DelLast
	move.l	a2,-(sp)
	bsr	Fs_NumStore
	beq.s	.Exit
	move.l	d0,a2
.Loop	move.l	(a2),d0
	beq.s	.Exit
	bsr	Fs_DelStore
	bra.s	.Loop
.Exit	move.l	(sp)+,a2
	rts

; Retourne l'element numero D0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_NumStore
	moveq	#0,d1
	tst.w	d0
	beq.s	.Exit
	lea	Fs_Liste(a5),a0
	bra.s	.In
.Loop	move.l	(a0),d1
	move.l	d1,a0
	subq.w	#1,d0
	beq.s	.Exit
.In	tst.l	(a0)
	bne.s	.Loop
.Exit	move.l	d1,d0
	rts

; Compte le nombre d'elements
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_CptStore
	moveq	#0,d0
	move.l	Fs_Liste(a5),d1
	beq.s	.Exit
.Loop	move.l	d1,a0
	addq.w	#1,d0
	move.l	(a0),d1
	bne.s	.Loop
.Exit	rts

; Flush de tous les directory
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Flush
	bsr	Fs_DelStores		Enleve
	clr.b	PI_FsStore(a5)
	rts

; Detruit toute la liste des directory
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_End
Fs_DelStores
	move.l	Fs_Liste(a5),d0
	beq.s	.Exit
	bsr	Fs_DelStore
	bra.s	Fs_DelStores
.Exit	rts

; Enleve le directory D0 de la liste
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_DelStore
	move.l	a2,-(sp)
	move.l	d0,a2
	move.l	8(a2),d1
	bra.s	.In
.Loop	move.l	d1,a1
	move.l	(a1),d1
	move.w	4(a1),d0
	ext.l	d0
	SyCall	MemFree
.In	tst.l	d1
	bne.s	.Loop
	move.l	a2,a1
	lea	Fs_Liste(a5),a0
	Rjsr	L_Lst.Del
	move.l	(sp)+,a2
	rts


; ___________________________________________________________________
;
;	ERREURS
; ___________________________________________________________________

Fs_OoMem
Fs_EcWiErr
Fs_NoBank
Fs_BadAPI
Fs_IllDName
	bra	Fs_Cancel


; ___________________________________________________________________
;
;	DIFFERENTES ROUTINES
; ___________________________________________________________________

; Effacement de tous les noms de l'�cran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_New	Rjsr	L_FillFFree
	clr.b	Fs_DirOn(a4)
	move.w	#-1,Fs_Click(a4)
	move.l	Fs_Variables(a4),a0
	clr.l	FsV_PList(a0)
	bsr	FsI_AffF
	rts

; Remplissage des noms de fichier...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_First
	clr.b	Fs_DevFlag(a4)
	move.w	#-1,Fs_Click(a4)
	move.l	Fs_Variables(a4),a0	Flag sort
	tst.l	FsV_Sort(a0)
	sne	FillFSorted(a5)
	move.l	#-1,FsV_PosFirst(a0)	Position du premier fichier
	bsr	Fs_NomDir		Name >>> Name1 / Name2
	bsr	Fs_FindStore		Deja trouve?
	bne.s	.Dejalu
	Rjsr	L_LockGet			Find a lock
	Rjsr	L_FillFirst		Get names
	bsr	FsI_SliStore
	move.b	#1,Fs_DirOn(a4)
	rts
.Dejalu	bsr	Fs_Branch		Branche la liste!
	bsr	FsI_AffF
	bsr	FsI_SliStore
	rts

; Prend le nom de fichier suivant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Next	tst.b	Fs_DirOn(a4)
	beq.s	.Skip
	clr.b	Fs_DirOn(a4)
	Rjsr	L_FillNxt
	ble.s	.Fini
	move.b	#1,Fs_DirOn(a4)
.Fini	cmp.w	#1,d0
	beq.s	.Skip
; Si SORT, si le fichier poke est en DESSOUS, decale vers le bas!
	tst.b	FillFSorted(a5)
	beq.s	.NoS
	move.l	Fs_Variables(a4),a0
	move.l	FsV_PosFirst(a0),d0
	bmi.s	.NoS
	move.w	FillFPosPoke(a5),d1
	bmi.s	.NoS
	cmp.w	d1,d0
	bcs.s	.NoS
	addq.l	#1,FsV_PList(a0)
	addq.l	#1,FsV_PosFirst(a0)
.NoS	bsr	FsI_AffF
.Skip	rts

; Digere le pathname
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Nom source
;	>>>	Pathname=Name1
;	>>>	Filtre=Name2
Fs_NomDir
	movem.l	a2/d2-d7,-(sp)
	move.l	#Fs_ChannelN,d0		Demande la chaine PATH
	moveq	#Fs_PathN,d1
	moveq	#0,d2
	Rbsr	L_Dia_GetValue
	bne	Fs_BadAPI
	move.l	d1,a2
; Separe filtre et path
	move.l	Name1(a5),a0
	move.l	Name2(a5),a1
	clr.b	(a0)
	clr.b	(a1)
	moveq	#-1,d1
	move.l	a0,d6
	moveq	#0,d7
.NmD1	move.b	(a2)+,d0
	move.b	d0,(a0)+
	beq.s	.NmD5
	clr.b	(a0)
	cmp.b	#"*",d0
	beq.s	.NmD2
	cmp.b	#"?",d0
	bne.s	.NmD3
.NmD2	bset	#0,d7
.NmD3	cmp.b	#":",d0
	beq.s	.NmD4
	cmp.b	#"/",d0
	bne.s	.NmD1
.NmD4	btst	#0,d7
	bne.s	.NmD1
	move.l	a0,d6
	bra.s	.NmD1
.NmD5	tst.w	d7
	beq.s	.NmDx
	move.l	d6,a0
.NmD6	move.b	(a0)+,(a1)+
	bne.s	.NmD6
	move.l	d6,a0
	clr.b	(a0)
; V�rifie la pr�sence du / au d�but...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.NmDx	move.l	Name1(a5),a0
	tst.b	(a0)
	beq.s	.NmDx2
.NmDx1	tst.b	(a0)+
	bne.s	.NmDx1
	cmp.b	#":",-2(a0)
	beq.s	.NmDx2
	cmp.b	#"/",-2(a0)
	beq.s	.NmDx2
	move.b	#"/",-1(a0)
	clr.b	(a0)
.NmDx2
; Compute with default pathname
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Rjsr	L_Dsk.PathIt
	movem.l	(sp)+,a2/d2-d7
	rts

; Retourne l'adresse de la banque FSEL
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_GetPuzzle
	move.l	Sys_Resource(a5),a0
; Les graphiques
	move.l	a0,a1
	add.l	2(a0),a1
; Les messages
	move.l	a0,a2
	add.l	6(a0),a2
; Les Programmes
	add.l	10(a0),a0
; Ok!
	rts

; Insere le directory A0 � la suite du directory courant (NAME1-NAME2)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_NewDir
	move.l	a0,-(sp)
	bsr	Fs_New
	move.l	(sp)+,a2		En plus...
	move.l	Buffer(a5),a1		Path d'origine
	lea	512(a1),a1
	addq.w	#2,a1
	move.l	a1,d1
; Copie le debut
	move.l	Name1(a5),a0
	tst.b	(a0)			Vide>>> rien!
	beq.s	.Sk1
.Lp1	move.b	(a0)+,(a1)+
	bne.s	.Lp1
	subq.l	#1,a1
; Copie le nouveau nom
.Sk1	move.l	a2,a0
.Lp2	move.b	(a0)+,(a1)+
	bne.s	.Lp2
	subq.l	#1,a1
; Copie le filtre
	move.l	Name2(a5),a0
	tst.b	(a0)
	beq.s	.Sk3
	cmp.l	a1,d1			Vide?
	beq.s	.Lp3
	cmp.b	#"/",-1(a1)
	beq.s	.Lp3
	cmp.b	#":",-1(a1)
	beq.s	.Lp3
	move.b	#"/",(a1)+
.Lp3	move.b	(a0)+,(a1)+
	bne.s	.Lp3
	subq.l	#1,a1
; Envoie aux dialogues
.Sk3	move.l	Buffer(a5),a0		Met la taille de la chaine
	lea	512(a0),a0
	sub.l	d1,a1
	move.w	a1,(a0)
	cmp.w	#256,a1
	bcc	Fs_IllDName
	move.l	#Fs_ChannelN,d0		Update la zone PATHNAME
	moveq	#Fs_PathN,d1
	move.l	a0,d2
	Rbsr	L_Dia_Update
; Demande le nouveau directory
	move.w	#-1,Fs_Click(a4)
	bsr	Fs_First		Lis le directory
	rts

; Ouvre l'ecran du selecteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_ScOpen
	bsr	Fs_GetPuzzle
	move.l	a1,a0
	moveq	#EcFsel,d0
	ext.l	d1
	ext.l	d2
	moveq	#1,d3
	moveq	#-1,d4
	Rbsr	L_Dia_RScOpen
	rts

; Prend les entrees (a3)+
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_GetInputs
	move.l	Fs_Variables(a4),a1
	move.l	(a3)+,a0		Titre 1
	tst.b	1(a0)
	beq.s	.Pat2
	move.l	a0,FsV_Titre1(a1)
.Pat2	move.l	(a3)+,a0		Titre 0
	tst.b	1(a0)
	beq.s	.Pat1
	move.l	a0,FsV_Titre0(a1)
.Pat1	move.l	(a3)+,FsV_File(a1)	Nom par d�faut
; Compute path with actual path...
	move.l	(a3)+,a0
	move.l	Name1(a5),a1		Recopie dans NAME1
	move.b	(a0)+,d0
	lsl.w	#8,d0
	move.b	(a0)+,d0
	subq.w	#1,d0
	bmi.s	.Pat4
.Pat3	move.b	(a0)+,(a1)+
	dbra	d0,.Pat3
.Pat4	clr.b	(a1)+
	Rjsr	L_Dsk.PathIt		Insere le PATH
	move.l	Name1(a5),a0
	move.l	Buffer(a5),a1
	lea	512(a1),a1
	addq.l	#2,a1
.Pat5	move.b	(a0)+,(a1)+
	bne.s	.Pat5
	move.l	Buffer(a5),a0
	lea	512(a0),a0
	sub.l	a0,a1
	subq.l	#3,a1
	move.w	a1,(a0)
	move.l	Fs_Variables(a4),a1
	move.l	a0,FsV_Path(a1)
	rts

; Apparition de l'�cran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_Appear
	move.l	Fs_AdEc(a4),a2
	move.w	PI_FsDWx(a5),EcAWX(a2)
	bset	#1,EcAW(a2)
	move.w	PI_FsDVApp(a5),d7
	moveq	#1,d6
	move.w	PI_FsDWy(a5),d5
	move.w	EcTy(a2),d0
	lsr.w	#1,d0
	add.w	d0,d5
	Rjsr	L_AppCentre
	rts
; Disparition de l'ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_DisAppear
	move.l	Fs_AdEc(a4),a2
	move.w	PI_FsDVApp(a5),d7
	neg.w	d7
	move.w	EcTy(a2),d6
	lsr.w	#1,d6
	move.w	PI_FsDWy(a5),d5
	add.w	d6,d5
	Rjsr	L_AppCentre
	rts

; ___________________________________________________________________
;
;	TACHE DE DIALOGUE
; ___________________________________________________________________

; Boucle d'animation du selecteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsI_Entry
	move.l	TaskBase(pc),a5		Base des donnees
	move.l	Fs_Base(a5),a4
; Boucle d'attente
; ~~~~~~~~~~~~~~~~
FsI_Loop
	move.w	Fs_Command(a4),d2	Une commande?
	bne.s	.Com
; Animation de l'interface
	bsr	Fs_Multi		Multitache
	moveq	#1,d0
	Rbsr	L_Dia_AutoTest2
	move.l	#Fs_ChannelN,d0		Demande de resultat
	Rbsr	L_Dia_GetReturn
	tst.w	d0
	bne.s	.Quit
	move.w	d1,Fs_Input(a4)
	bne.s	.CWait
	SyCall	MouseKey		Touche de droite
	btst	#1,d1
	beq.s	FsI_Loop
	move.w	#20,Fs_Input(a4)	Faux bouton
; Une entree, attendre la reponse du selecteur
.CWait	move.w	#1,Fs_Waiting(a4)
.Wait	bsr	Fs_Multi
	tst.w	Fs_Waiting(a4)
	bne.s	.Wait
	bra.s	FsI_Loop
; Une commande du selecteur QUIT ou AFFF
.Com	bmi.s	.Quit
	clr.w	Fs_Command(a4)
	btst	#0,d2
	beq.s	.Sk1
	bsr	_AffF
.Sk1	btst	#1,d2
	beq.s	.Sk2
	bsr	_SliStore
.Sk2	btst	#2,d2
	bne.s	.CWait
	bra	FsI_Loop
; Sortie!
.Quit	move.w	#-1,Fs_Waiting(a4)
	rts
; Attente multitache
; ~~~~~~~~~~~~~~~~~~
Fs_Multi
	movem.l	a0-a1/a6/d0-d1,-(sp)
	move.l	T_GfxBase(a5),a6
	jsr	-270(a6)
	movem.l	(sp)+,a0-a1/a6/d0-d1
	rts
; Re-Affichage des noms de fichier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_AffF	movem.l	a2/d2,-(sp)
	move.w	FillFNb(a5),d0		Nombre de fichiers
	move.w	d0,Fs_ASize(a4)
	move.l	#Fs_ChannelN,d0		Change la taille de la liste
	moveq	#Fs_ListN,d1
	moveq	#0,d2
	Rbsr	L_Dia_GetZoneAd
	beq	Fs_BadAPI
	move.w	Fs_ASize(a4),Dia_LiMaxAct(a0)
	move.l	#Fs_ChannelN,d0		Re-actualise le slider, donc la liste
	moveq	#Fs_SliderN,d1
	move.w	Fs_ASize(a4),d4		Nouvelle global
	move.l	#EntNul,d3		Ancienne window
	move.l	Fs_Variables(a4),a2
	move.l	FsV_PList(a2),d2	Nouvelle position
	clr.l	FsV_AffFlag(a2)
	Rbsr	L_Dia_Update
	bne	Fs_BadAPI
	addq.l	#1,FsV_AffFlag(a2)
	movem.l	(sp)+,a2/d2
	rts
; Actualise le nombre de STORE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_SliStore
	movem.l	d2-d4,-(sp)
	bsr	Fs_CptStore
	move.l	d0,d4
	addq.w	#1,d4
	move.l	#Fs_ChannelN,d0
	moveq	#Fs_SliderS,d1
	moveq	#1,d3			Window = 1
	moveq	#0,d2
	Rbsr	L_Dia_Update
	bne	Fs_BadAPI
	movem.l	(sp)+,d2-d4
	rts


; ___________________________________________________________________
;
;	COMMUNICATION AVEC LA TACHE DE DIALOGUE
; ___________________________________________________________________

; Stoppe la tache de dialogue et revient
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsI_Stop
	tst.w	Fs_Waiting(a4)
	bne.s	.Skip
	bset	#2,Fs_Command+1(a4)
	clr.w	Fs_Input(a4)
.Loop	Rjsr	L_Sys_WaitMul
	tst.w	Fs_Waiting(a4)
	beq.s	.Loop
.Skip	rts
; Redemmare la tache de dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsI_ReStart
	clr.w	Fs_Waiting(a4)
	rts
; Provoque l'affichage du slider de store
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsI_SliStore
	bset	#1,Fs_Command+1(a4)
	rts
; Provoque l'affichage des fichiers.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsI_AffF
	bset	#0,Fs_Command+1(a4)
	rts
; Provoque la sortie de selecteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsI_Quit
	clr.w	Fs_Waiting(a4)
	bset	#7,Fs_Command(a4)
.Loop	Rjsr	L_Sys_WaitMul
	tst.w	Fs_Waiting(a4)
	beq.s	.Loop
	rts


; Retourne le nom du fichier aux dialogues...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_GetName
	move.l	d4,d0
	Rjsr	L_FillGet
	tst.w	d0
	beq.s	.Skip
	lea	6+4(a0),a1
	move.l	6(a0),d1
	move.l	Fs_Base(a5),a0
	move.l	Fs_Variables(a0),a0
	tst.l	FsV_Size(a0)
	bne.s	.Size
; Normal, sans taille...
; ~~~~~~~~~~~~~~~~~~~~~~
.Normal	move.l	a1,a0
.Long	tst.b	(a0)+
	bne.s	.Long
	sub.l	a1,a0
	move.l	a0,d1
	moveq	#1,d0
.Skip	rts
; Met la taille � la fin!
; ~~~~~~~~~~~~~~~~~~~~~~~
.Size	btst	#31,d1			Un assign/stored
	bne.s	.Normal
	cmp.b	#"*",(a1)		Un directory
	beq.s	.Normal
	movem.l	a2/d2-d4,-(sp)
	move.l	Buffer(a5),a2		Un endroit libre dans le buffer
	lea	896(a2),a2
	move.l	FsV_Tx(a0),d2		Taille fenetre
	move.l	a2,a0
.Copy1	move.b	(a1)+,(a0)+		Copie la premiere chaine
	bne.s	.Copy1
	subq.l	#1,a0
	lea	-8(a2,d2.w),a1		Termine avec des blancs
	bra.s	.Blin1
.Bllp1	move.b	#" ",(a0)+
.Blin1	cmp.l	a1,a0
	bcs.s	.Bllp1
	move.l	a1,a0
	move.b	#" ",(a0)+
	move.l	d1,d0			Met la taille
	Rjsr	L_LongToDec
	addq.l	#8,a1
	bra.s	.Blin2
.Bllp2	move.b	#" ",(a0)+
.Blin2	cmp.l	a1,a0
	bcs.s	.Bllp2
	move.l	a2,a1			Retourne la taille
	sub.l	a1,a0
	move.l	a0,d1
	movem.l	(sp)+,a2/d2-d4
	rts
; ___________________________________________________________________
;
;	LOW MEMORY FILE SELECTOR
; ___________________________________________________________________
;
Fs_LowMemory

; Recupere la palette de la banque par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Fs_GetPuzzle
	move.w	(a1),d0
	lsl.w	#2,d0
	lea	2+2+2(a1,d0.w),a1
	lea	LFs_Palette(pc),a0
	moveq	#31,d0
.Copy	move.w	(a1)+,(a0)+
	dbra	d0,.Copy
	lea	LFs_Palette(pc),a0
	move.w	2*2(a0),0*2(a0)
	move.w	3*2(a0),1*2(a0)

; Ouverture d'un tout petit ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	PI_FsDSx(a5),d2		Taille choisie
	ext.l	d2
	moveq	#64,d3			SY
	moveq	#1,d4			Nb de plans
	move.l	#$8000,d5
	moveq	#2,d6			Nb de couleurs
	moveq	#0,d7
	lea	LFs_Palette(pc),a1
	EcCalD	Cree,EcFsel
	bne	Fs_OoMem
	move.l	a0,Fs_AdEc(a4)
	bsr	Fs_Appear
	WiCalA	Print,Fs_COff(pc)
	move.b	#1,Fs_Opened(a4)

; Ouverture du canal de dialogue...
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Fs_GetPuzzle
	add.l	14(a0),a0		QUATRIEME programme
	move.l	#Fs_ChannelN,d0		Numero du canal
	move.l	#1024,d1		Buffer
	moveq	#32,d2			Nombre de variables
	moveq	#0,d3			Ne PAS recopier
	Rbsr	L_Dia_OpenChannel
	tst.l	d0			Une erreur?
	bne	Fs_OoMem
	move.l	#-1,Fs_Channel(a4)	Canal non multitache!
	move.l	a1,Fs_Variables(a4)
	bsr	Fs_GetInputs		Recupere les entrees utilisateur

; Dessin du fond / Appel du programme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	#Fs_ChannelN,d0
	moveq	#-1,d1
	moveq	#0,d2
	moveq	#0,d3
	Rbsr	L_Dia_RunProgram
	tst.l	d0
	bne	Fs_OoMem

; C'est fini!!!
; ~~~~~~~~~~~~~
	cmp.l	#1,d1
	beq	Fs_Ok
	bra	Fs_Cancel


; 		Arret du curseur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_COff		dc.b	27,"C0",0
		even

;		Structure FLUSH / END
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fs_FlushStructure	dc.l	0
			bra	Fs_Flush
Fs_EndStructure		dc.l	0
			bra	Fs_End

; 		Table des erreurs reconnues
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FsErDisk	dc.w 204,205,210,213,218,225,226,-1
;		EdT	80,<Directory not found>		204
;		EdT	81,<File not found>			205
;		EdT	82,<Illegal file name>			210
;		EdT	83,<Disc is not validated>		213
;		EdT	86,<Device not available>		218
;		EdT	92,<Not an AmigaDOS disc>		225
;		EdT	93,<No disc in drive>			226
;		EdT	94,<I/O error>

; Tache de dialogue
; ~~~~~~~~~~~~~~~~~
TaskBase	dc.l	0
TaskName	dc.b	"AMOSPro File Selector",0
		even

; Palette par defaut du selecteur low mem
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LFs_Palette	ds.w	32
		dc.l	0
		Even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DU SELECTEUR DE FICHIERS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	End_FSel
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CREATE TASK / Amiga.Lib
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	CreateTask
; - - - - - - - - - - - - -
	LINK	A6,#-$20
	MOVEM.L	D2-5/A2-3,-(A7)
	MOVE.L	8(A6),D3
	MOVE.B	$F(A6),D2
	MOVE.L	$10(A6),D4
	MOVE.L	$14(A6),D0
	ADDQ.L	#3,D0
	MOVEQ	#-4,D1
	AND.L	D1,D0
	MOVE.L	D0,$14(A6)
	lea	_CtData(pc),A0
	LEA	-$20(A6),A1
	MOVEQ	#7,D0
L7D065D6
	MOVE.L	(A0)+,(A1)+
	DBF	D0,L7D065D6
	MOVE.L	$14(A6),-4(A6)
	PEA	-$20(A6)
	BSR	L7D066BC
	MOVEA.L	D0,A3
	MOVE.L	A3,D5
	ADDQ.L	#4,A7
	BNE.S	L7D065FA
	MOVEQ	#0,D0
	BRA	L7D0666C
L7D065FA
	MOVEA.L	$10(A3),A2
	MOVE.L	$18(A3),$3A(A2)
	MOVE.L	$14(A6),D0
	ADD.L	$3A(A2),D0
	MOVE.L	D0,$3E(A2)
	MOVE.L	$3E(A2),$36(A2)
	MOVE.B	#1,8(A2)
	MOVE.B	D2,9(A2)
	MOVE.L	D3,$A(A2)
	PEA	$4A(A2)
	BSR	L7D066F8
	MOVE.L	A3,-(A7)
	PEA	$4A(A2)
	BSR	L7D06688
	CLR.L	-(A7)
	MOVE.L	D4,-(A7)
	MOVE.L	A2,-(A7)
	BSR	L7D066A0
	MOVEA.L	$4.w,A0
	CMPI.W	#$25,$14(A0)
	LEA	$18(A7),A7
	BCS.S	L7D0666A
	TST.L	D0
	BNE.S	L7D0666A
	MOVE.L	A3,-(A7)
	BSR	L7D066D0
	MOVEQ	#0,D0
	ADDQ.L	#4,A7
	BRA.S	L7D0666C
L7D0666A
	MOVE.L	A2,D0
L7D0666C
	MOVEM.L	-$38(A6),D2-5/A2-3
	UNLK	A6
	RTS

L7D06688
	MOVE.L	A6,-(A7)
	MOVEA.L	$4.w,A6
	MOVEM.L	8(A7),A0-A1
	JSR	-$F0(A6)
	MOVEA.L	(A7)+,A6
	RTS
L7D066A0
	MOVEM.L	A2-A3/A6,-(A7)
	MOVEA.L	$4.w,A6
	MOVEM.L	$10(A7),A1-A3
	JSR	-$11A(A6)
	MOVEM.L	(A7)+,A2-A3/A6
	RTS
L7D066BC
	MOVE.L	A6,-(A7)
	MOVEA.L	$4.w,A6
	MOVEA.L	8(A7),A0
	JSR	-$DE(A6)
	MOVEA.L	(A7)+,A6
	RTS
L7D066D0
	MOVE.L	A6,-(A7)
	MOVEA.L	$4.w,A6
	MOVEA.L	8(A7),A0
	JSR	-$E4(A6)
	MOVEA.L	(A7)+,A6
	RTS
L7D066F8
	MOVEA.L	4(A7),A0
	CLR.L	4(A0)
	MOVE.L	A0,8(A0)
	ADDQ.L	#4,A0
	MOVE.L	A0,-(A0)
	RTS
_CtData
	DC.L	0
	DC.L	0
	DC.L	0
	DC.L	2
	DC.L	$10001
	DC.L	$5C
	DC.L	$10000
	DC.L	0





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
	Lib_Def	LEd_Init
; - - - - - - - - - - - - -
	movem.l	a0-a3,-(sp)
	move.l	a0,a3
	move.l	a2,LEd_Buffer(a3)	Adresse buffer
	move.b	d0,LEd_Flags(a3)	Flags
	move.w	d2,LEd_Max(a3)		Longueur maxi ligne
	clr.w	LEd_Cur(a3)
	clr.w	LEd_Start(a3)
	moveq	#-1,d0			RAZ masques
	move.l	d0,LEd_Mask+0(a3)
	move.l	d0,LEd_Mask+4(a3)
	move.l	d0,LEd_Mask+8(a3)
	WiCall	XYCuWi
	move.w	d1,LEd_X(a3)
	move.w	d2,LEd_Y(a3)
	move.w	d3,LEd_Large(a3)	Largeur
	bpl.s	.Skip
	WiCall	SXSYCuWi		Trouve le maximum
	sub.w	LEd_X(a3),d1
	subq.w	#1,d1
	bmi.s	.Err
	btst	#8,d0			SCROLL ON? (Bit#0 de WiSys)
	beq.s	.PSc
	subq.w	#1,d1
	bmi.s	.Err
.PSc	move.w	d1,LEd_Large(a3)
.Skip	bclr	#2,LEd_Flags+1(a3)	Pas de curseur
	WiCalA	Print,.CuOff(pc)	Arret du vrai curseur!
	move.l	T_EcCourant(a5),a0	Stocke le numero d'ecran
	move.w	EcNumber(a0),LEd_Screen(a3)
	movem.l	(sp),a0-a3
	moveq	#-1,d0
	Rbsr	L_LEd_New		Affiche la ligne
	movem.l	(sp)+,a0-a3
	moveq	#0,d0
	rts
.Err	movem.l	(sp)+,a0-a3
	moveq	#-1,d0			Pas assez de place
	rts
.CuOff	dc.b	27,"C0",0


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
	Lib_Def	LEd_Loop
; - - - - - - - - - - - - -
	movem.l	a2-a3/d3,-(sp)
	move.l	a0,a3
	Rbsr	L_LEd_CuMarche

; Gestion des flags
; ~~~~~~~~~~~~~~~~~~~~~
.Loop	btst	#LEd_FMulti,LEd_Flags(a3)	Multitache?
	beq.s	.Sk1
	Rbsr	L_Dia_WaitMul
.Sk1	btst	#LEd_FTests,LEd_Flags(a3)	Tests AMOS
	beq.s	.Sk2
	Rjsr	L_Test_PaSaut
	move.w	T_Actualise(a5),d0		Control-C
	and.w	ActuMask(a5),d0
	bclr	#BitControl,d0
	beq.s	.Sk2
	move.w	d0,T_Actualise(a5)
	moveq	#0,d1
	moveq	#-1,d2
	bra	.Exit
.Sk2	SyCall	MouseKey			Tests de la souris
	and.w	#$03,d1
	beq.s	.Sk3
	btst	#LEd_FMouCur,LEd_Flags(a3)
	beq	.MOut
	SyCall	MouScrFront		Souris dans ecran de front
	cmp.w	LEd_Screen(a3),d0	Le bon ecran?
	bne.s	.MOut
	SyCall	XyWin			Dans la fenetre courante
	bmi.s	.MOut
	cmp.w	LEd_Y(a3),d2		Meme ligne Y
	bne.s	.MOut
	sub.w	LEd_X(a3),d1		Dans la ligne X
	bcs.s	.MOut
	add.w	LEd_Start(a3),d1	Dans le texte?
	cmp.w	LEd_Long(a3),d1
	bhi.s	.MOut
	cmp.w	LEd_Cur(a3),d1
	beq.s	.Sk3
	move.w	d1,LEd_Cur(a3)		On change!
	bra	.Print
.MOut	btst	#LEd_FMouse,LEd_Flags(a3)	Retourner la souris
	beq.s	.Sk3
	moveq	#0,d1
	moveq	#2,d2			Sortie, avec code souris (=2)
	bra	.Exit
.Sk3

; Prend la touche
; ~~~~~~~~~~~~~~~
	move.l	LEd_Buffer(a3),a2
	SyCall	Inkey
	move.l	d1,d0
	beq.s	.Rien
	swap	d0
	cmp.b	#$4F,d0			Fleche gauche
	beq	.G
	cmp.b	#$4E,d0			Fleche droite
	beq	.D
	cmp.b	#$41,d0			BackSpace
	beq	.Ba
	cmp.b	#$46,d0			Delete
	beq	.Dl
	cmp.b	#13,d1			Return
	beq.s	.Rt
	cmp.b	#32,d1
	bcc.s	.Lt
; Retour � l'appelant, avec le scancode eventuel en D1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Out	btst	#LEd_FKeys,LEd_Flags(a3)
	beq	.Loop
	moveq	#0,d2
	bra.s	.Exit
; Rien en retour. Retourner � l'appelant?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Rien	btst	#LEd_FOnce,LEd_Flags(a3)
	beq	.Loop
	moveq	#0,d2
.Exit	moveq	#0,d0
	move.w	LEd_Long(a3),d0
	move.l	LEd_Buffer(a3),a0
	clr.b	0(a0,d0.w)
	tst.w	d2
	movem.l	(sp)+,a2-a3/d3
	rts
; Return
; ~~~~~~
.Rt	moveq	#1,d2
	bra	.Exit
; Une lettre
; ~~~~~~~~~~
.Lt	btst	#LEd_FFilter,LEd_Flags(a3)	; Limiter les entrees?
	beq.s	.Ins
	tst.b	d1
	bmi.s	.Rien
	cmp.b	#32,d1
	bcs.s	.Rien
	move.w	d1,d0				; Oui, pointe les tables
	lsr.w	#3,d0
	lea	LEd_Mask-4(a3,d0.w),a0
	move.b	d1,d0
	and.w	#$07,d0
	neg.w	d0
	addq.w	#7,d0
	btst	d0,(a0)
	beq	.Rien
; Insere la lettre
.Ins	move.w	LEd_Cur(a3),d2
	move.w	LEd_Long(a3),d3
	cmp.w	LEd_Max(a3),d3
	bcs.s	.Cour
	cmp.w	d2,d3			; Remplace la derniere lettre
	bne	.Loop
	move.b	d1,-1(a2,d2.w)
	bra	.Print
.Cour	lea	0(a2,d2.w),a0		; Recopie la ligne
	lea	0(a2,d3.w),a1
	bra.s	.In
.Copy	move.b	-(a1),1(a1)		; Boucle de recopie
.In	cmp.l	a0,a1
	bhi.s	.Copy
.Poke	move.b	d1,(a0)
	addq.w	#1,LEd_Cur(a3)
	addq.w	#1,LEd_Long(a3)
	bra	.Print
; Fleche gauche
; ~~~~~~~~~~~~~
.G	move.w	LEd_Cur(a3),d2
	beq	.Loop
	move.w	d0,d1
	and.w	#$0300,d0		; Shifts
	bne.s	.Mg
	and.w	#$1800,d1		; Control
	bne.s	.Gl
	subq.w	#1,LEd_Cur(a3)		; Un caractere � gauche
	bra	.Print
.Gl	clr.w	LEd_Cur(a3)		; Debut ligne
	bra	.Print
.Mg	subq.w	#1,d2			; Mot gauche
	move.w	d2,LEd_Cur(a3)
	beq	.Print
	move.b	-1(a2,d2.w),d0
	Rbsr	L_LEd_Lettre
	bne.s	.Mg
	bra	.Print
; Fleche droite
; ~~~~~~~~~~~~~
.D	move.w	LEd_Cur(a3),d2
	move.w	LEd_Long(a3),d3
	cmp.w	d3,d2
	bcc	.Loop
	move.w	d0,d1
	and.w	#$0300,d0		; Shifts
	bne.s	.Md
	and.w	#$1800,d1		; Control
	bne.s	.Dd
	addq.w	#1,LEd_Cur(a3)		; A droite
	bra	.Print
.Dd	move.w	d3,LEd_Cur(a3)		; Droite de la ligne
	bra	.Print
.Md	addq.w	#1,d2
	move.w	d2,LEd_Cur(a3)
	cmp.w	d3,d2
	bcc	.Print
	move.b	-1(a2,d2.w),d0
	Rbsr	L_LEd_Lettre
	bne.s	.Md
	bra	.Print
; BackSpace
; ~~~~~~~~~
.Ba	and.w	#$0300,d1
	bne.s	.Cl
	tst.w	LEd_Cur(a3)
	beq	.Loop
	subq.w	#1,LEd_Cur(a3)
; Delete
; ~~~~~~
.Dl	and.w	#$0300,d1
	bne.s	.Cl
	move.w	LEd_Cur(a3),d2
	move.w	LEd_Long(a3),d3
	cmp.w	d3,d2
	bcc	.Loop
	lea	0(a2,d2.w),a0			Recopie du texte
	lea	0(a2,d3.w),a1
.DCopy	move.b	1(a0),(a0)+
	cmp.l	a1,a0
	bcs.s	.DCopy
	subq.w	#1,LEd_Long(a3)
	bset	#0,LEd_Flags+1(a3)
	bra	.Print
; Nettoyage ligne
; ~~~~~~~~~~~~~~~
.Cl	clr.w	LEd_Long(a3)
	clr.w	LEd_Cur(a3)
	bset	#1,LEd_Flags+1(a3)
; Impression de la ligne, retour � la boucle
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Print	Rbsr	L_LEd_Print
	btst	#LEd_FTests,LEd_Flags(a3)	Si flag TEST (INPUT)
	beq	.Loop
	Rjsr	L_Dia_Patch			Branchement au patch moniteur
	bra	.Loop

; Remplacement de la ligne editee par une autre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	Structure
;	A1=	Nouvelle ligne
;	D0=	Longueur (ou -1)
; - - - - - - - - - - - - -
	Lib_Def	LEd_New
; - - - - - - - - - - - - -
	movem.l	a2-a3/d2-d4,-(sp)
	move.l	a0,a3
	move.l	LEd_Buffer(a3),a2
	move.w	LEd_Max(a3),d2
	tst.w	d0
	beq.s	.Exit1
.Copy	move.b	(a1)+,(a2)+
	beq.s	.Exit
	subq.w	#1,d0
	beq.s	.Exit1
	subq.w	#1,d2
	bne.s	.Copy
	addq.l	#1,a2
.Exit	subq.l	#1,a2
.Exit1	clr.b	(a2)
	sub.l	LEd_Buffer(a3),a2
	move.w	a2,LEd_Long(a3)
	move.w	a2,LEd_Cur(a3)
	clr.w	LEd_Start(a3)
; Fin de l'impression...
	bset	#1,LEd_Flags+1(a3)
	Rbsr	L_LEd_Print
	move.l	a3,a0
	Rbsr	L_LEd_CuStoppe
	move.l	a3,a0
	movem.l	(sp)+,a2-a3/d2-d4
	rts

; 	Impression de la ligne courante
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	LEd_Print
; - - - - - - - - - - - - -
	move.l	LEd_Buffer(a3),a2
; Curseur on / off
	move.l	a3,a0
	Rbsr	L_LEd_CuStoppe
; Locate en debut de ligne
	move.w	LEd_X(a3),d1
	move.w	LEd_Y(a3),d2
	WiCall	Locate
; Effacement complet de la ligne?
	bclr	#1,LEd_Flags+1(a3)
	beq.s	.Pacl
	move.w	LEd_Large(a3),d2
.Eff	WiCalD	ChrOut,32
	subq.w	#1,d2
	bne.s	.Eff
	move.w	LEd_X(a3),d1
	move.w	LEd_Y(a3),d2
	WiCall	Locate
; Curseur � gauche
.Pacl	move.w	LEd_Cur(a3),d2
	cmp.w	LEd_Start(a3),d2
	bcc.s	.Pag
	move.w	d2,LEd_Start(a3)
; Curseur � droite
.Pag	move.w	LEd_Start(a3),d0
	add.w	LEd_Large(a3),d0
	cmp.w	d0,d2
	ble.s	.Pad
	move.w	d2,d0
	sub.w	LEd_Large(a3),d0
	move.w	d0,LEd_Start(a3)
	bset	#0,LEd_Flags+1(a3)
; Impression de la ligne
.Pad	move.w	LEd_Start(a3),d0
	lea	0(a2,d0.w),a1
	move.w	LEd_Long(a3),d1
	sub.w	d0,d1
	cmp.w	LEd_Large(a3),d1
	ble.s	.Pal
	bclr	#0,LEd_Flags+1(a3)
	move.w	LEd_Large(a3),d1
	addq.w	#1,d1
.Pal	WiCall	Print2
; Effacement de la droite de la ligne?
	bclr	#0,LEd_Flags+1(a3)
	beq.s	.Noe
	WiCalD	ChrOut,32
; Locate au curseur
.Noe	move.w	LEd_X(a3),d1
	add.w	LEd_Cur(a3),d1
	sub.w	LEd_Start(a3),d1
	move.w	LEd_Y(a3),d2
	WiCall	Locate
; Remet le curseur (vrai ou faux)
	move.l	a3,a0
	Rbsr	L_LEd_CuMarche
	rts

; Arret curseur ligne editee
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	LEd_CuStoppe
; - - - - - - - - - - - - -
	bclr	#2,LEd_Flags+1(a0)
	beq.s	.Pacu
	lea	LEd_CuOff(pc),a1
	btst	#LEd_FCursor,LEd_Flags(a0)
	bne.s	.Nor
	Rlea	L_LEd_FauCu,1
.Nor	WiCall	Print
.Pacu	rts
LEd_CuOff	dc.b	27,"C0",0
	even
; Remise en route curseur
; ~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	LEd_CuMarche
; - - - - - - - - - - - - -
	bset	#2,LEd_Flags+1(a0)
	bne.s	.Pacu
	lea	LEd_CuOn(pc),a1
	btst	#LEd_FCursor,LEd_Flags(a0)
	bne.s	.Nor
	Rlea	L_LEd_FauCu,1
.Nor	WiCall	Print
.Pacu	rts
LEd_CuOn	dc.b	27,"C1",0
	even
; - - - - - - - - - - - - -
	Lib_Def	LEd_FauCu
; - - - - - - - - - - - - -
LEd_FauCu	dc.b	27,"I1",27,"W2"," ",27,"W0",27,"I0",29,0
	even

;
; Ramene VRAI si D2 est une lettre ou un chiffre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	LEd_Lettre
; - - - - - - - - - - - - -
	cmp.b	#"0",d0
	bcs.s	.LFaux
	cmp.b	#"9",d0
	bls.s	.LVrai
	cmp.b	#"A",d0
	bcs.s	.LFaux
	cmp.b	#"Z",d0
	bls.s	.LVrai
	cmp.b	#"a",d0
	bcs.s	.LFaux
	cmp.b	#"z",d0
	bls.s	.LVrai
	cmp.b	#128,d0
	bcc.s	.LVrai
.LFaux:	moveq	#0,d0
	rts
.LVrai:	moveq	#-1,d0
	rts

; - - - - - - - - - - - - -
	Lib_Def	Dia_WaitMul
; - - - - - - - - - - - - -
	movem.l	a0-a1/a6/d0-d1,-(sp)
	move.l	T_GfxBase(a5),a6
	jsr	-270(a6)
	movem.l	(sp)+,a0-a1/a6/d0-d1
	rts


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
	Lib_Def	Dia_OpenChannel
; - - - - - - - - - - - - -
	movem.l	a2-a6/d2-d7,-(sp)
	addq.l	#1,d2
	movem.l	a0-a2/d0-d2,-(sp)
	lsl.l	#2,d2
	and.w	#$FFFE,d1
	add.l	d1,d2
	add.l	#Dia_Vars,d2
; Boite d�ja en route?
; ~~~~~~~~~~~~~~~~~~~~
	Rbsr	L_Dia_GetChannel
	bne	.Deja
; Un ecran ouvert?
; ~~~~~~~~~~~~~~~~
	tst.l	T_EcCourant(a5)
	beq	.Scnop
; Reserve la zone de memoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Cur_Dialogs(a5),a0
	move.l	d2,d0				Longueur buffer
	Rjsr	L_Lst.New
	beq	.Mem
	lea	8(a1),a4
	move.l	(sp)+,Dia_Channel(a4)		Numero de la boite
	move.l	(sp)+,d2			Longueur du buffer
	move.l	(sp)+,Dia_NVar(a4)		Nombre de variables
	move.l	(sp)+,a2			Adresse du programme
	move.l	(sp)+,Dia_Puzzle(a4)		Banque de puzzle
	move.l	(sp)+,Dia_Messages(a4)
	move.l	sp,Dia_Sp(a4)
; Branche la routine ClearVar
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Rjsr	L_Dia_ClearVar
; Stocke le Numero / Adresse de l'�cran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	T_EcCourant(a5),a0
	move.l	a0,Dia_Screen(a4)
	move.w	EcNumber(a0),Dia_ScreenNb(a4)
; Init des couleurs sliders
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	SlDInit(pc),a0
	lea	Dia_SlDefault(a4),a1
	moveq	#16-1,d0
.Se	move.b	(a0)+,(a1)+
	dbra	d0,.Se
; Copie le programme dans une espace memoire
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d0
	move.w	(a2)+,d0
	beq	.Synt
	move.l	a2,Dia_Programs(a4)
	clr.l	Dia_ProgLong(a4)
	sub.l	a6,a6
	btst	#0,d3
	beq.s	.PaCop
	move.l	d0,d1
	addq.l	#4,d0
	move.l	d0,Dia_ProgLong(a4)
	SyCall	MemFast
	beq	.Mem2
	move.l	a0,Dia_Programs(a4)
.Copy	move.b	(a2)+,(a0)+
	subq.w	#1,d1
	bne.s	.Copy
	clr.b	(a0)
.PaCop	move.l	Dia_Programs(a4),a6
; Trouve le debut et la fin du buffer interne
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	Dia_NVar(a4),d0
	lsl.l	#2,d0
	lea	Dia_Vars(a4),a3
	add.l	d0,a3
	move.l	a3,Dia_Labels(a4)
	move.l	a3,Dia_Buffer(a4)
	lea	0(a3,d2.l),a0
	move.l	a0,Dia_Pile(a4)
; Trouve les labels, defini les instructions supplementaires
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	clr.l	(a3)
	subq.l	#1,(a3)
.INext	Rbsr	L_Dia_Chr		Premiere lettre
	beq	.Quit
	cmp.b	#"[",d0
	beq.s	.INext
	cmp.b	#"]",d0
	beq.s	.INext
	move.b	d0,d1			Deuxieme lettre
	Rbsr	L_Dia_Chr
	beq	.Synt
	lsl.w	#8,d1
	move.b	d0,d1
	Rlea	L_Dia_Instr,0		Exploration des tokens
	move.l	a0,a1
	moveq	#Dia_NInstr-1,d0
	moveq	#-1,d7
.Find	addq.w	#1,d7
	cmp.w	(a0)+,d1
	dbeq	d0,.Find
	tst.w	d0
	bmi	.PaDef
; Un label?
	cmp.w	#28,d7			Label?
	beq	.Lab
	cmp.w	#35,d7			Instruction?
	beq	.Inst
; Saute les parametres
	move.w	Dia_NParam-Dia_Instr-2(a0),d2
	beq.s	.NoPar
; Appelle l'evaluation
.Param	bsr	.Evalue
	subq.w	#1,d2
	beq.s	.PFin
	cmp.b	#",",d0
	beq.s	.Param
	bne	.NPar
.PFin	cmp.b	#",",d2
	beq	.NPar
	bra	.INext
; Pas de parametres: un separateur
.NoPar	Rbsr	L_Dia_Chr
	beq	.Synt
	bcc	.INext
	bra	.Synt
; Fixe les bornes du buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~
.Quit	clr.l	(a3)
	subq.l	#1,(a3)+
	move.l	a3,Dia_Buffer(a4)
	move.l	a3,Dia_PBuffer(a4)
	clr.l	(a3)
; Ok, pas d'erreur!
; ~~~~~~~~~~~~~~~~~
	move.l	a4,a0
	lea	Dia_Vars(a4),a1
	movem.l	(sp)+,a2-a6/d2-d7
	moveq	#0,d0
	rts

; Erreur avant verification
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Scnop	moveq	#EDia_Screen,d0
	bra.s	.Err
.Mem	moveq	#EDia_OMem,d0
	bra.s	.Err
.Deja	moveq	#EDia_ChanAD,d0
.Err	moveq	#0,d1
	lea	3*4+3*4(sp),sp
	movem.l	(sp)+,a2-a6/d2-d7
	tst.l	d0
	rts
; Messages d'erreur pendant verification
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.NPar	moveq	#EDia_NPar,d2
	bra.s	.Err2
.ALab	moveq	#EDia_LabAD,d2
	bra.s	.Err2
.Mem2	sub.l	a6,a6
	moveq	#EDia_OMem,d2
	bra.s	.Err3
.Synt	moveq	#EDia_Syntax,d2
; Efface le buffer
.Err2	sub.l	Dia_Programs(a4),a6
.Err3	move.l	Dia_Sp(a4),sp
; Efface le buffer
	move.l	a4,a0
	Rbsr	L_Dia_CloseA0
; Pointe l'erreur en D1
	move.l	a6,d1
	move.l	d2,d0
	movem.l	(sp)+,a2-a6/d2-d7
	rts

; Definition d'une instruction
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Inst	Rbsr	L_Dia_Chr
	beq	.Synt
	bcc	.Synt
	bmi	.Synt
	move.b	d0,d1
	lsl.w	#8,d1
	Rbsr	L_Dia_Chr
	bcc	.Synt
	move.b	d0,d1
; Une instruction deja definie?
	Rlea	L_Dia_Instr,0		Exploration des tokens
	moveq	#Dia_NInstr-1,d0
.UFind	cmp.w	(a0)+,d1
	dbeq	d0,.UFind
	tst.w	d0
	bpl	.Synt
; Recherche dans le labels
	move.l	Dia_Labels(a4),a0
.IChk	move.w	(a0),d0
	addq.l	#6,a0
	bmi.s	.IOk
	cmp.w	d1,d0
	bne.s	.IChk
	bra	.ALab
; Poke dans les labels
.IOk	move.w	d1,(a3)+		Adresse dans la liste
	Rbsr	L_Dia_Chr
	cmp.b	#",",d0			Une virgule
	bne	.Synt
	Rbsr	L_Dia_Chr		Trouve le nombre de params
	beq	.Synt
	bcc	.Synt
	bpl	.Synt
	addq.l	#4,a3
	Rbsr	L_Dia_FChif
	move.l	(a3),d1			Jusqu'� 16 parametres
	cmp.l	#9,d1
	bhi	.Synt
	move.w	d1,(a3)+
	Rbsr	L_Dia_Chr		Saute le ;
	bcs	.Synt
	Rbsr	L_Dia_Chr		Adresse du debut de la routine
	cmp.b	#"[",d0
	bne	.Synt
	move.l	a6,d0
	sub.l	Dia_Programs(a4),d0
	move.w	d0,(a3)+
	clr.l	(a3)			Marque la fin des labels
	subq.l	#1,(a3)
	bra	.INext
; Trouve le numero du label
; ~~~~~~~~~~~~~~~~~~~~~~~~~
.Lab	Rbsr	L_Dia_Chr
	beq	.Synt
	bcc	.Synt
	bpl	.Synt
	addq.l	#4,a3
	Rbsr	L_Dia_FChif
	move.l	(a3),d1
	cmp.l	#65536,d1
	bcc	.Synt
	clr.l	(a3)
	subq.l	#1,(a3)
	move.l	Dia_Labels(a4),a0
.Chk	move.l	(a0),d0
	addq.l	#6,a0
	bmi.s	.Ok
	cmp.l	d1,d0
	bne.s	.Chk
	bra	.ALab
.Ok	move.l	d1,(a3)+
	Rbsr	L_Dia_Chr		Saute le separateur
	beq	.Synt
	bcs	.Synt
	move.l	a6,d0
	sub.l	Dia_Programs(a4),d0
	move.w	d0,(a3)+
	clr.l	(a3)
	subq.l	#1,(a3)
	bra	.INext
; Appel d'un UI: saute les parametres
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PaDef	Rbsr	L_Dia_Chr
	beq	.Synt
	bcc	.Dfin
	subq.l	#1,a6
.Dloop	bsr	.Evalue
	cmp.b	#",",d0
	beq.s	.Dloop
.Dfin	bra	.INext
; Evaluation d'une expression
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Evalue
.ELoop	Rbsr	L_Dia_Chr		Premiere lettre
	bmi.s	.Chiffre
	bvs.s	.Signe
	beq	.Synt
	bcc	.EFini
	Rlea	L_Dia_FTokens,1
	move.b	-64(a1,d0.w),d0		Offset des codes
	beq	.Synt
	lea	32(a1,d0.w),a1		Pointe la liste
	Rbsr	L_Dia_Chr
.FLoop	move.w	(a1)+,d1		Trouve le bon
	beq	.Synt
	cmp.b	d0,d1
	bne.s	.FLoop
	cmp.b	#'"',d1				"
	beq.s	.Guil
	cmp.b	#"'",d1				'
	beq.s	.Guil
	bra.s	.ELoop
.Signe	Rlea	L_Dia_FTokens,1
	lea	32(a1),a1
	bra.s	.FLoop
.Chiffre
	cmp.b	#"%",d0
	beq.s	.BLoop
	cmp.b	#"$",d0
	beq.s	.HLoop
.DLoop	Rbsr	L_Dia_ChrC
	bmi.s	.CFini
	cmp.b	#10,d0
	bcs.s	.DLoop
	bra.s	.CFini
.BLoop	Rbsr	L_Dia_ChrC
	bmi.s	.CFini
	cmp.b	#2,d0
	bcs.s	.BLoop
	bra.s	.CFini
.HLoop	Rbsr	L_Dia_ChrC
	bmi.s	.CFini
	cmp.b	#10,d0
	bcs.s	.HLoop
	add.b	#"0"-"A",d0
	bmi.s	.CFini
	cmp.b	#6,d0
	bcs.s	.HLoop
.CFini	subq.l	#1,a6
	bra	.ELoop
; Des guillemets
.Guil	move.b	(a6)+,d0
	cmp.b	#" ",d0
	bcs	.Synt
	cmp.b	d0,d1
	bne.s	.Guil
	bra	.ELoop
; On a fini l'expression
.EFini	rts
; Init des sliders par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
SlDInit	dc.b 0,0,0,1,4,4,4,1
	dc.b 0,0,0,1,3,3,3,1
	even


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Demande un espace buffer temporaire
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetBuffer
; - - - - - - - - - - - - -
	movem.l	a1/d1,-(sp)
	move.l	Dia_ABuffer(a4),a0
	addq.w	#1,d0
	and.w	#$FFFE,d0
	lea	0(a0,d0.w),a1
	move.l	a1,d1
	cmp.l	a3,d1
	Rbcc	L_Dia_OBuffer
	move.l	d1,Dia_ABuffer(a4)
	move.w	d0,(a0)
; Nettoyage
	lea	4(a0),a1
.C	clr.w	(a1)+
	cmp.l	a1,d1
	bhi.s	.C
	clr.l	(a1)
	movem.l	(sp)+,a1/d1
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Memorise la position du buffer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_SetBuffer
; - - - - - - - - - - - - -
	move.l	Dia_ABuffer(a4),Dia_PBuffer(a4)
	rts

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
	Lib_Def	Dia_Active
; - - - - - - - - - - - - -
	movem.l	a0-a1/d1,-(sp)
; Verifie la presence de l'�cran, du meme!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	Dia_ScreenNb(a4),d0
	lsl.w	#2,d0
	add.w	#T_EcAdr,d0
	move.l	0(a5,d0.w),d0
	cmp.l	Dia_Screen(a4),d0
	bne.s	.Err
; Stocke l'ancien ecran
; ~~~~~~~~~~~~~~~~~~~~~
	move.w	#-1,Dia_ScreenOld(a4)
	move.l	T_EcCourant(a5),a0
	move.w	EcNumber(a0),d0
	cmp.w	Dia_ScreenNb(a4),d0
	beq.s	.Deja
; Active le nouvel ecran
; ~~~~~~~~~~~~~~~~~~~~~~
	move.w	d0,Dia_ScreenOld(a4)
	move.w	Dia_ScreenNb(a4),d1
	EcCall	Active
; Stocke la fenetre active
; ~~~~~~~~~~~~~~~~~~~~~~~~
.Deja	WiCall	GAdr
	move.w	d1,Dia_WindOld(a4)
	move.w	d1,Dia_WindOn(a4)
; Ok!
; ~~~
	moveq	#0,d0
	bra.s	.Out
; Erreur, l'�cran est modifie
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Err	moveq	#EDia_Screen,d0
.Out	movem.l	(sp)+,a0-a1/d1
	rts

; ___________________________________________________________________________
;
;	Re-Active l'ancien ecran
; ___________________________________________________________________________
;
; IN
;	A4=	Boite de dialogue
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_ReActive
; - - - - - - - - - - - - -
	movem.l	d0-d1/a0-a1,-(sp)
; Une fenetre d'�dition en route?
	move.l	Dia_Edited(a4),d0
	ble.s	.Skip
	move.l	d0,a0
	Rbsr	L_Dia_EdActive
	bra.s	.Meme
; Remet la fenetre de fond
.Skip	move.w	Dia_WindOld(a4),d1
	cmp.w	Dia_WindOn(a4),d1
	beq.s	.Meme
	move.w	d1,Dia_WindOn(a4)
	WiCall	QWindow
; Remet l'ancien ecran
.Meme	move.w	Dia_ScreenOld(a4),d1
	bmi.s	.Deja
	EcCall	Active
	move.w	#-1,Dia_ScreenOld(a4)
.Deja	movem.l	(sp)+,d0-d1/a0-a1
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Branchement de la routine de fermeture CLEARVAR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_ClearVar
; - - - - - - - - - - - - -
	move.l	a2,-(sp)
	lea	.Struc(pc),a1
	lea	Sys_ClearRoutines(a5),a2
	SyCall	AddRoutine
	lea	.Struc1(pc),a1
	lea	Sys_DefaultRoutines(a5),a2
	SyCall	AddRoutine
	Rlea	L_Dia_AutoTest,0
	move.l	a0,GoTest_Dialog(a5)
	move.l	(sp)+,a2
	rts
.Struc	dc.l	0
	bra.s	.Clr
.Struc1	dc.l	0
.Clr	Rbra	L_Dia_CloseChannels

; ___________________________________________________________________________
;
;	Effacement de tous les canaux de dialogues
; ___________________________________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_CloseChannels
; - - - - - - - - - - - - -
.Loop	move.l	Cur_Dialogs(a5),a0
	move.l	(a0),d0
	beq.s	.Out
	move.l	d0,a0
	move.l	8(a0),d0
	Rbsr	L_Dia_CloseChannel
	bra.s	.Loop
.Out	rts

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
	Lib_Def	Dia_CloseChannel
; - - - - - - - - - - - - -
	movem.l	d2,-(sp)
; Efface une boite existante
	move.l	d0,d2
	Rbsr	L_Dia_EffChannel
; Efface la structure
	move.l	d2,d0
	Rbsr	L_Dia_GetChannel
	beq.s	.Err
	Rbsr	L_Dia_CloseA0
	moveq	#0,d0
.Out	movem.l	(sp)+,d2
	rts
.Err	moveq	#EDia_ChanND,d0
	bra.s	.Out
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Ferme le canal A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_CloseA0
; - - - - - - - - - - - - -
	pea	-8(a0)
	move.l	Dia_Programs(a0),d0
	beq.s	.Skip
	move.l	d0,a1
	move.l	Dia_ProgLong(a0),d0
	beq.s	.Skip
	SyCall	MemFree
.Skip	move.l	(sp)+,a1
	move.l	Cur_Dialogs(a5),a0
	Rjsr	L_Lst.Del
	rts

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
	Lib_Def	Dia_RunQuick
; - - - - - - - - - - - - -
	movem.l	d2-d7,-(sp)
; Trouve le bon numero de canal
	movem.l	d0/d1/a0/a1,-(sp)
	move.l	#65536,d7
.Find	move.l	d7,d0
	Rbsr	L_Dia_GetChannel
	beq.s	.Ok
	addq.l	#1,d7
	bra.s	.Find
.Ok	movem.l	(sp)+,d0/d1/a0/a1
; Ouvre le canal
	movem.l	d0-d1,-(sp)
	movem.l	d2-d4,-(sp)
	move.l	d7,d0
	move.l	d6,d1
	moveq	#16,d2
	move.l	d5,d3
	Rbsr	L_Dia_OpenChannel
	bne	.Err
; Met les variables d'entree
	move.l	(sp)+,(a1)+
	move.l	(sp)+,(a1)+
	move.l	(sp)+,(a1)+
; Demarre le programme
	move.l	d7,d0
	moveq	#-1,d1
	movem.l	(sp)+,d2-d3
	Rbsr	L_Dia_RunProgram
; Efface le canal
	movem.l	d0/d1,-(sp)
	move.l	d7,d0
	Rbsr	L_Dia_CloseChannel
	movem.l	(sp)+,d0/d1
.Out	movem.l	(sp)+,d2-d7
	tst.w	d0
	rts
.Err	lea	5*4(sp),sp
	bra.s	.Out

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
	Lib_Def	Dia_Update
; - - - - - - - - - - - - -
	movem.l	a2-a6/d2-d7,-(sp)
	move.l	d4,d5
	move.l	d3,d4
	move.l	d2,d3
	move.l	d1,d2
	Rbsr	L_Dia_GetChannel
	beq.s	.NF
	move.l	a0,a4
	moveq	#0,d0
	btst	#0,Dia_RFlags(a4)
	beq.s	.Out
	move.l	Dia_Pile(a4),a3
	Rbsr	L_Dia_Active
	bne.s	.NF
	Rbsr	L_Dia_ZUpdate
	Rbsr	L_Dia_ReActive
	move.w	Dia_Error(a4),d0	Pas d'erreur, revient directement
	beq.s	.Out
	move.w	Dia_ErrorPos(a4),d1	Position de l'erreur
	ext.l	d1
.Out	movem.l	(sp)+,d2-d7/a2-a6
	ext.l	d0
	rts
.NF	moveq	#EDia_ChanND,d0
	bra.s	.Out

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
	Lib_Def	Dia_RunProgram
; - - - - - - - - - - - - -
	movem.l	a2-a6/d2-d7,-(sp)
	move.l	d0,d5
	move.l	d1,d4
; Va effacer une boite d�ja en route
	Rbsr	L_Dia_EffChannel
; Trouve l'adresse du canal
	move.l	d5,d0
	Rbsr	L_Dia_GetChannel
	beq	.ChanNF
	move.l	a0,a4
; Base X / Base Y
	move.l	#EntNul,d0
	cmp.l	d0,d2
	beq.s	.Sk1
	move.l	d2,Dia_BaseX(a4)
.Sk1	cmp.l	d0,d3
	beq.s	.Sk2
	move.l	d3,Dia_BaseY(a4)
.Sk2
; Trouve l'adresse du programme
	move.l	Dia_Programs(a4),a6
	move.l	d4,d0
	bmi.s	.Palab
	Rbsr	L_Dia_GetLabel
	beq	.LabNF
	add.l	d0,a6
.Palab
; Prepare les piles
	move.l	Dia_Pile(a4),a3
	moveq	#0,d6
	moveq	#0,d7
	move.l	Dia_Buffer(a4),a0
	move.l	a0,Dia_PBuffer(a4)
	clr.l	(a0)
	clr.l	Dia_LastKey(a4)
	clr.l	Dia_Edited(a4)
	clr.l	Dia_LastZone(a4)
	clr.w	Dia_XA(a4)
	clr.w	Dia_YA(a4)
	clr.w	Dia_XB(a4)
	clr.w	Dia_YB(a4)
	clr.w	Dia_Return(a4)
	clr.w	Dia_Error(a4)
	clr.w	Dia_Users(a4)
	clr.l	Dia_PUsers(a4)
	clr.w	Dia_NPUsers(a4)
	clr.l	Dia_Release(a4)
; Active l'�cran
	Rbsr	L_Dia_Active
	bne.s	.Err
; Writing 0
	move.l	T_RastPort(a5),a1
	moveq	#0,d0
	move.w	d0,Dia_Writing(a4)
	GfxCa5	SetDrMd
; Appelle le programme
	bset	#0,Dia_RFlags(a4)	Faire un effacement
	bclr	#1,Dia_RFlags(a4)	Runne?
	bclr	#2,Dia_RFlags(a4)	Pas FREEZE
	Rbsr	L_Dia_ClearKey		Plus de touches
	Rbsr	L_Dia_Loop
	Rbsr	L_Dia_ClearKey		Plus de touches
	btst	#1,Dia_RFlags(a4)
	bne.s	.Runne
; Initialise pour les tests
	Rbsr	L_Dia_EdFirst		Premiere ligne EDIT
	Rbsr	L_Dia_Patch
	Rbsr	L_Dia_ReActive		Remet l'ancien ecran
	move.w	Dia_Error(a4),d0	Pas d'erreur, revient directement
	beq.s	.OOut
.Err	move.l	a4,a0			Erreur, efface ce qui a ete dessin�
	Rbsr	L_Dia_EffChanA0
	move.w	Dia_Error(a4),d0	Type de l'erreur
	ext.l	d0
.OErr	move.w	Dia_ErrorPos(a4),d1	Position de l'erreur
	ext.l	d1
.OOut	movem.l	(sp)+,d2-d7/a2-a6
	rts
; Un RUn a eu lieu!
; ~~~~~~~~~~~~~~~~~
.Runne		move.l	a4,a0			Efface le canal
	Rbsr	L_Dia_EffChanA0
	move.w	Dia_Return(a4),d1
	ext.l	d1
	move.w	Dia_Error(a4),d0
	beq.s	.OOut
	bra.s	.OErr
; Programme non trouve
; ~~~~~~~~~~~~~~~~~~~~
.LabNF	moveq	#EDia_LabND,d0
	bra.s	.OOut
; Channel not found
; ~~~~~~~~~~~~~~~~~
.ChanNF	moveq	#EDia_ChanND,d0
	bra.s	.OOut

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Trouve le label D0>>>D0
;	BEQ= 	pas trouve
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetLabel
; - - - - - - - - - - - - -
	move.l	Dia_Labels(a4),a0
.Loop	move.l	(a0)+,d1
	addq.l	#2,a0
	bmi.s	.Err
	cmp.l	d0,d1
	bne.s	.Loop
	moveq	#0,d0
	move.w	-2(a0),d0
	rts
.Err 	moveq	#0,d0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Freeze tous les canaux
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_FreezeChannels
; - - - - - - - - - - - - -
	move.l	d2,-(sp)
	move.l	d1,d2
	move.l	Cur_Dialogs(a5),a1
	bra.s	.In
.Loop	move.l	d0,a1
	lea	8(a1),a0
	Rbsr	L_Dia_FrzA0
.In	move.l	(a1),d0
	bne.s	.Loop
	movem.l	(sp)+,d2
	rts
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
	Lib_Def	Dia_FreezeChannel
; - - - - - - - - - - - - -
	move.l	d2,-(sp)
	move.l	d1,d2
	Rbsr	L_Dia_GetChannel
	beq.s	.NF
	Rbsr	L_Dia_FrzA0
.Ok	moveq	#0,d0
.Out	movem.l	(sp)+,d2
	rts
; Erreur
.NF	moveq	#EDia_ChanND,d0
	bra.s	.Out
; - - - - - - - - - - - - -
	Lib_Def	Dia_FrzA0
; - - - - - - - - - - - - -
	bclr	#2,Dia_RFlags(a0)
	tst.w	d2
	beq.s	.Skip
	bset	#2,Dia_RFlags(a0)
.Skip	rts

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
	Lib_Def	Dia_EffChannel
; - - - - - - - - - - - - -
	Rbsr	L_Dia_GetChannel
	beq.s	.Err
	Rbsr	L_Dia_EffChanA0
	moveq	#0,d0
	rts
.Err	moveq	#EDia_ChanND,d0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Efface une boite de dialogue dans un canal A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EffChanA0
; - - - - - - - - - - - - -
	movem.l	a2-a4/a6/d2-d7,-(sp)
	move.l	a0,a4
; Faut-il effacer?
	bclr	#0,Dia_RFlags(a4)
	beq	.PaEff
; Active l'ancien ecran
	Rbsr	L_Dia_Active
	bne	.PaEff
; Efface les fenetres / Zones r�serv�es par le programme
	clr.l	-(sp)
	move.l	Dia_Buffer(a4),a6
	bra.s	.EIn
; Une ligne d'edition
.ELoop	cmp.w	#Dia_EdMark,2(a6)	Une ligne d'edition
	bne.s	.PaEd
	move.l	a6,a0			Efface la fenetre
	Rbsr	L_Dia_EdEnd
; Un bouton
.PaEd	cmp.w	#Dia_BlMark,2(a6)
	bne.s	.PaBl
	move.l	a6,-(sp)
; Une liste
.PaBl	cmp.w	#Dia_LiMark,2(a6)
	bne.s	.PaLi
	move.l	a6,a2
	Rbsr	L_Dia_LiEnd
; Un Texte
.PaLi	cmp.w	#Dia_TxMark,2(a6)
	bne.s	.PaTx
	move.l	a6,a2
	Rbsr	L_Dia_TxEnd
; Suivant dans le buffer
.PaTx	add.w	(a6),a6
.EIn	tst.w	(a6)
	bne.s	.ELoop
; Efface les blocs dans l'ordre inverse
	bra.s	.BlIn
.BlLp	move.l	d0,a6
	moveq	#0,d1
	move.w	Dia_BlNumber(a6),d1
	move.l	#EntNul,d2
	move.l	d2,d3
	move.l	d3,d4
	move.l	d4,d5
	move.l	Buffer(a5),a1
	EcCall	BlPut
	moveq	#0,d1
	move.w	Dia_BlNumber(a6),d1
	EcCall	BlDel
.BlIn	move.l	(sp)+,d0
	bne.s	.BlLp
	Rbsr	L_Dia_Patch
; Remet l'ancien ecran
	clr.l	Dia_Edited(a4)
	Rbsr	L_Dia_ReActive
; Remet le buffer en place
.PaEff	movem.l	(sp)+,a2-a4/a6/d2-d7
	rts

; ___________________________________________________
;
;	Retourne l'adresse de la variable D0/D1
; ___________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_GetVariable
; - - - - - - - - - - - - -
	move.l	d2,-(sp)
	move.l	d1,d2
	Rbsr	L_Dia_GetChannel
	beq.s	.NFnd
	cmp.l	Dia_NVar(a0),d2
	bhi	.FCall
	lsl.w	#2,d2
	lea	Dia_Vars(a0),a0
	add.w	d2,a0
	moveq	#0,d0
	bra.s	.Out
.NFnd	moveq	#EDia_ChanND,d0
	bra.s	.Out
.FCall	moveq	#EDia_VarND,d0
.Out	movem.l	(sp)+,d2
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Retourne le numero de la zone sous D1/D2 dans le canal D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetZone
; - - - - - - - - - - - - -
	move.l	a4,-(sp)
	movem.l	d1/d2,-(sp)
	Rbsr	L_Dia_GetChannel
	movem.l	(sp)+,d1/d2
	beq	.NFnd
	move.l	a0,a4
	Rbsr	L_Dia_GetZ
	moveq	#-1,d1
	tst.l	d0
	beq.s	.Out
	move.l	d0,a0
	move.w	Dia_ZoNumber(a0),d1
	ext.l	d1
	moveq	#0,d0
.Out	move.l	(sp)+,a4
	rts
; Rien du tout!
.NFnd	moveq	#EDia_ChanND,d0
	bra.s	.Out

; ___________________________________________________
;
;	Retourne la valeur de la zone D1/D2, Channel D0
; ___________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_GetValue
; - - - - - - - - - - - - -
	Rbsr	L_Dia_GetZoneAd
	beq	.NFnd
	moveq	#0,d1
	moveq	#0,d2
; Un bouton normal / slider / liste
	cmp.w	#Dia_BtMark,2(a0)
	beq.s	.Po
	cmp.w	#Dia_LiMark,2(a0)
	beq.s	.Po
	cmp.w	#Dia_SlMark,2(a0)
	beq.s	.Po
	cmp.w	#Dia_TxMark,2(a0)
	beq.s	.Tx
	cmp.w	#Dia_EdMark,2(a0)
	beq.s	.Ed
.Ok	moveq	#0,d0
	rts
; Rien du tout!
.NFnd	moveq	#EDia_ChanND,d0
	rts
; Une valeur en chiffre, normale
.Po	move.l	Dia_ZoPos(a0),d1
	moveq	#0,d2
	bra.s	.Ok
; Un hyper texte
.Tx	moveq	#0,d2
	move.l	Dia_ZoPos(a0),d1	Vide!
	bne.s	.Po
	lea	Dia_TxBuffer(a0),a0
	move.l	a0,d1
	moveq	#2,d2
	bra.s	.Ok
; Une ligne d'�dition
.Ed	cmp.w	#Dia_DiLong,(a0)
	beq.s	.Di
; Un texte
	move.l	Dia_LEd+LEd_Buffer(a0),d1
	moveq	#2,d2
	bra.s	.Ok
; Un chiffre
.Di	move.l	Dia_LEd+LEd_Buffer(a0),a1
	move.b	(a1)+,d0
	cmp.b	#32,d0
	bcs.s	.Di2
	movem.l	a0/a3/a6/d7,-(sp)
	move.l	a1,a6
	lea	Dia_DiValue(a0),a3
	clr.l	(a3)+
	Rbsr	L_Dia_FChif
	movem.l	(sp)+,a0/a3/a6/d7
.Di2	move.l	Dia_DiValue(a0),d1
	moveq	#0,d2
	bra.s	.Ok

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; Retourne l'adresse de la zone Channel D0 / Zone D1 - D2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetZoneAd
; - - - - - - - - - - - - -
	movem.l	d2/d3,-(sp)
	move.l	d1,d3
	Rbsr	L_Dia_GetChannel
	beq.s	.NFnd
; Boucle d'exploration
	subq.w	#1,d2
	move.l	Dia_Buffer(a0),a1
	bra.s	.In
.Loop	cmp.w	#Dia_ZoMark,4(a1)
	bne.s	.Next
	cmp.w	Dia_ZoNumber(a1),d3
	beq.s	.Fnd
.Next	add.w	(a1),a1
.In	tst.w	(a1)
	bne.s	.Loop
.NFnd	moveq	#0,d0
	bra.s	.Out
; Trouve?
.Fnd	tst.w	d2
	ble.s	.Skip
	dbra	d2,.Next
.Skip	move.l	a1,d0
	move.l	d0,a0
.Out	movem.l	(sp)+,d2/d3
	rts


; __________________________________________
;
; Positionne les variables en fonction de D0
; __________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_SetVFlags
; - - - - - - - - - - - - -
	move.l	d2,-(sp)
	bra.s	.In
.Loop	lsr.w	#1,d0
	moveq	#0,d2
	addx.l	d2,d2
	move.l	d2,(a0)+
.In	dbra	d1,.Loop
	movem.l	(sp)+,d2
	rts

; ___________________________________________
;
; Recupere les memes variables
; ___________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_GetVFlags
; - - - - - - - - - - - - -
	move.w	d1,d0
	lsl.w	#2,d0
	add.w	d0,a0
	moveq	#0,d0
	bra.s	.Skip
.Loop	lsl.w	#1,d0
	tst.l	-(a0)
	beq.s	.Skip
	bset	#0,d0
.Skip	dbra	d1,.Loop
	rts

;
; Retourne le parametre d'une boite de dialogue
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; IN
;	D0=	Channel
; OUT
;	D0=	Erreur
;	D1=	Parametre
; - - - - - - - - - - - - -
	Lib_Def	Dia_GetReturn
; - - - - - - - - - - - - -
	Rbsr	L_Dia_GetChannel
	beq.s	.Err
	moveq	#-1,d1
	btst	#0,Dia_RFlags(a0)
	beq.s	.Out
	moveq	#0,d1
	move.w	Dia_Return(a0),d1
	clr.w	Dia_Return(a0)
.Out	moveq	#0,d0
	rts
.Err	moveq	#EDia_ChanND,d0
	rts

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
	Lib_Def	Dia_RScOpen
; - - - - - - - - - - - - -
	movem.l	a2/d2-d7,-(sp)
	subq.l	#8,sp
	move.l	d3,(sp)
	move.l	d2,d3			TY
	move.l	d1,d2			TX
	move.l	d0,d1			Numero
	move.l	a0,a1
	move.w	(a1),d0
	lsl.w	#2,d0
	lea	2(a1,d0.w),a1
	move.w	(a1)+,d6		Nombre couleurs
	ext.l	d6
	move.w	(a1)+,d5		Mode
	and.l	#$8004,d5
	tst.w	d4			Force l'interlaced?
	bmi.s	.Skm
	bclr	#2,d5
	tst.w	d4
	beq.s	.Skm
	bset	#2,d5
.Skm	cmp.l	#4096,d6
	bne.s	.ScOo0
	moveq	#6,d4
	or.w	#$0800,d5
	moveq	#64,d6
	bra.s	.ScOo2
* Nombre de couleurs-> plans
.ScOo0	moveq	#1,d4			* Nb de plans
	moveq	#2,d0
.ScOo1	cmp.l	d0,d6
	beq.s	.ScOo2
	lsl.w	#1,d0
	addq.w	#1,d4
	cmp.w	#7,d4
	bcs.s	.ScOo1
.ScOo2	EcCall	Cree
	bne.s	.Err
	move.l	a0,4(sp)
* Fait flasher la couleur
	move.l	(sp),d1			Efface le curseur
	bne.s	.Fl
	lea	.Cu0(pc),a1
	bra.s	.Prn
.Fl	moveq	#1,d0			Met le curseur
	cmp.w	EcNbCol(a0),d1
	bcc.s	.Err
	moveq	#46,d0
	Rjsr	L_Sys_GetMessage
	move.l	a0,a1
	EcCall	Flash
	bne.s	.Err
	move.l	(sp),d1
	lea	.Cu1(pc),a1
	add.b	#"0",d1
	move.b	d1,2(a1)
.Prn	WiCall	Print
	moveq	#0,d0
* Erreur
.Err	tst.l	(sp)+
	move.l	(sp)+,a0
	movem.l	(sp)+,a2/d2-d7
	tst.w	d0
	rts
.Cu0	dc.b	27,"C0",0
.Cu1	dc.b	27,"D0",0


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	LISTE DES INSTRUCTIONS DIALOGUE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_Instr
; - - - - - - - - - - - - -
Dia_Instr	dc.w	"EX"		0 EXit
		dc.w	"UN"		1 UNpack
		dc.w	"LI"		2 LIne
		dc.w	"BO"		3 BOx
		dc.w	"SI"		4 SIze
		dc.w	"BA"		5 BAse
		dc.w	"PU"		6 PUzzle
		dc.w	"SV"		7 Set Var
		dc.w	"IL"		8 Illegal
		dc.w	"PR"		9 PRint
		dc.w	"PO"		10 Print Outline
		dc.w	"IN"		11 INk
		dc.w	"SF"		12 SetFont
		dc.w	"SW"		13 SetWriting
		dc.w	"SL"		14 SetLine
		dc.w	"SP"		15 Set Pattern
		dc.w	"BU"		16 BUtton
		dc.w	"JP"		17 Jump
		dc.w	"RU"		18 Run
		dc.w	"BR"		19 Button Return
		dc.w	"ED"		20 Edit Ascii
		dc.w	"JS"		21 Jump to subroutine!
		dc.w	"RT"		22 Rts!
		dc.w	"BC"		23 Button Change
		dc.w	"KY"		24 Affectation touche
		dc.w	"SA"		25 Save
		dc.w	"DI"		26 Digit
		dc.w	"BL"		27 Block
		dc.w	"LA"		28 Label
		dc.w	"BQ"		29 Button Quit
		dc.w	"HS"		30 Slider Horizontal
		dc.w	"VS"		31 Slider Vertical
		dc.w	"AL"		32 Active List
		dc.w	"IL"		33 Inactive List
		dc.w	"ZC"		34 Zone change
		dc.w	"UI"		35 User Instruction
		dc.w	"GB"		36 Graphic Box
		dc.w	"GS"		37 Graphic Square
		dc.w	"GL"		38 Graphic Line
		dc.w	"IF"		39 If
		dc.w	"SZ"		40 SetZonevar
		dc.w	"XY"		41 XA YA XB YB
		dc.w	"NW"		42 NoWait
		dc.w	"VT"		43 VText
		dc.w	"VL"		44 VLine
		dc.w	"HT"		45 HyperText
		dc.w	"CA"		46 Call
		dc.w	"SM"		47 Screen Move
		dc.w	"GE"		48 Graphic Ellipse
		dc.w	"GP"		49 Graphic Plot
		dc.w	"SS"		50 Set Slider
Dia_NInstr	equ	 (*-Dia_Instr)/2
Dia_NParam	dc.w	0		0 EXit
		dc.w	3		1 UNpack X,Y,I
		dc.w	4		2 LIne X,Y,I,SX
		dc.w	5		3 BOx X,Y,I,SX,SY
		dc.w	2		4 SIze X,Y
		dc.w	2		5 BAse X,Y
		dc.w	1		6 PUzzle I
		dc.w	2		7 SetVar N,V
		dc.w	0		8 Illegal
		dc.w	4		9 Print X,Y,Text,Ink
		dc.w	5		10 POutline X,Y,Text,P0,P1
		dc.w	3		11 INk A,B,C
		dc.w	2		12 SetFont Font,Style
		dc.w	1		13 SetWrite Mode
		dc.w	1		14 SetLine Line
		dc.w	2		15 SetFill Pattern,Out
		dc.w	8		16 Button z,x,y,sx,sy,pos,min,max
		dc.w	1		17 Jump
		dc.w	2		18 Teste
		dc.w	1		19 Button Return
		dc.w	8		20 Edit n,x,y,sx,sxm,$$,pap,pen
		dc.w	1		21 Jump Sub
		dc.w	0		22 Rts
		dc.w	2		23 Button Change
		dc.w	2		24 Affectation touche
		dc.w	1		25 Save
		dc.w	8		26 Digit n,x,y,sx,var#,flag,pap,pen
		dc.w	1		27 Block 2
		dc.w	1		28 LAbel N
		dc.w	0		29 Button Quit
		dc.w	9		30 Slider H
		dc.w	9		31 Slider V
		dc.w	10		32 Active List
		dc.w	10		33 Inactive List
		dc.w	2		34 Zone Change
		dc.w	0		35 INstruction
		dc.w	4		36 Graphic Box x,y,x,y
		dc.w	4		37 Graphic Square x,y,x,y
		dc.w	4		38 Graphic Line x,y,x,y
		dc.w	1		39 IF
		dc.w	1		40 SetZonevar
		dc.w	4		41 XY
		dc.w	0		42 NoWait
		dc.w	4		43 HText x,y,t,pen
		dc.w	4		44 VLine x,y,i,y2
		dc.w 	10		45 HText z,x,y,sx,sy,txt,pos,flags,pap,pen;[]
		dc.w	1		46 CAll ad
		dc.w	0		47 Screen Move
		dc.w	4		48 Graphic Ellipse x,y,r1,r2
		dc.w	3		49 Graphic Plot x,y,i
		dc.w	8		50 Set Slider a,b,c,p,a,b,c,p

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	DEFINITIONS DES TOKENS FONCTIONS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_FTokens
; - - - - - - - - - - - - -
Dia_FTokens	IncBin	"bin/+Dialog_Funcs.bin"
		Even

* D�finition des fonctions
* ~~~~~~~~~~~~~~~~~~~~~~~~
*Dia_Foncs:
* BX		0 - Base X
* BY		1 - Base Y
* SX		2 - Size X
* SY		3 - Size Y
* ++		4 - Plus
* --		5 - Moins
* **		6 - Mul
* //		7 - Div
* NE		8 - NEg
* ME		9 - MEss
* ME		10- MEss VIDE!
* TW		11- Text Width
* TH		12- Text Height
* VA		13- Variable
* ""		14- String 1
* ''		15- String 2
* CX		16- Centre Text
* SW		17- Size of screen X
* SH		18- Size of screen Y
* MI		19- Minimum
* MA		20- Maximum
* BP		21- Button position
* ##		22- Number
* !!		23- Plus chaine
* MZ		24- Message ZERO
* XA		25- Position X2
* YA		26- Position Y2
* XB		27- Position X2
* YB		28- Position Y2
* AR		29- Array
* ZP		30- Zone Position
* P1		31- Param #
* P2		32- Param #
* P3		33- Param #
* P4		34- Param #
* P5		35- Param #
* P6		36- Param #
* P7		37- Param #
* P8		38- Param #
* P9		39- Param #
* ==		40- Equal
* \\		41- Different
* <<		42- Inferieur
* >>		43- Superieur
* &&		44- And
* ||		45- Or
* ZV		46- ZoneVar
* ZN		47- ZoneNumber
* TL		48- Text Length
* AS		49- Array Size
Dia_NFonc:	equ	50

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
	Lib_Def	Dia_Loop
; - - - - - - - - - - - - -
Dia_Loop
	movem.l	d2-d7/a2/a3/a6,-(sp)
; Sauvegarde de la pile
; ~~~~~~~~~~~~~~~~~~~~~
	move.l	sp,Dia_Sp(a4)
; Boucle d'appel
; ~~~~~~~~~~~~~~
.INext	tst.w	Dia_Error(a4)		Sort si erreur
	Rbne	L_Dia_Quit
	Rbsr	L_Dia_Chr		Premiere lettre
	Rbeq	L_Dia_Synt		DOIT finir par exit
	cmp.b	#"]",d0			Routine finie?
	Rbeq	L_Dia_Quit
	move.b	d0,d1			Deuxieme lettre
	Rbsr	L_Dia_Chr
	lsl.w	#8,d1
	move.b	d0,d1
	Rlea	L_Dia_Instr,0		Exploration des tokens
	move.l	a0,a1
	moveq	#Dia_NInstr-1,d0
.Find	cmp.w	(a0)+,d1
	dbeq	d0,.Find
	tst.w	d0
	bmi	.Inst
	move.l	a0,d0
	sub.l	a1,d0
	lsl.w	#1,d0
	pea	.Jumps-4(pc,d0.w)
	bra	.Params
; Appel des instructions
; ~~~~~~~~~~~~~~~~~~~~~~
.Jumps	Rbra	L_Dia_Quit		0
	bra	Dia_Unpack		1
	bra	Dia_Line		2
	bra	Dia_Box			3
	bra	Dia_Size		4
	bra	Dia_Base		5
	bra	Dia_Pu			6
	bra	Dia_SetVar		7
	bra	Dia_Illegal		8
	bra	Dia_Text		9
	bra	Dia_Outline		10
	bra	Dia_SInk		11
	bra	Dia_SFont		12
	bra	Dia_SWrite		13
	bra	Dia_SLine		14
	bra	Dia_SFill		15
	bra	Dia_Button		16
	bra	Dia_Jump		17
	bra	Dia_Run			18
	bra	Dia_BR			19
	bra	Dia_Edit		20
	bra	Dia_JSub		21
	bra	Dia_Rts			22
	bra	Dia_BChange		23
	bra	Dia_Key			24
	bra	Dia_Block		25
	bra	Dia_Digit		26
	bra	Dia_Block2		27
	bra	Dia_Label		28
	bra	Dia_BQuit		29
	bra	Dia_SliderH		30
	bra	Dia_SliderV		31
	bra	Dia_List		32
	bra	Dia_List		33
	bra	Dia_ZChange		34
	Rbra	L_Dia_Synt		35 Instruction!
	bra	Dia_GBox		36
	bra	Dia_GSquare		37
	bra	Dia_GLine		38
	bra	Dia_If			39
	bra	Dia_ZVar		40
	bra	Dia_XY			41
	bra	Dia_NWait		42
	bra	Dia_VText		43
	bra	Dia_VLine		44
	bra	Dia_HyperText		45
	bra	Dia_Call		46
	bra	Dia_SMove		47
	bra	Dia_GEllipse		48
	bra	Dia_GPlot		49
	bra	Dia_SetSlider		50

; Prend les parametres
; ~~~~~~~~~~~~~~~~~~~~
.Params	move.l	Dia_PBuffer(a4),Dia_ABuffer(a4)
	move.w	Dia_NParam-Dia_Instr-2(a0),d0
	beq.s	.NoPar
	cmp.w	#7,d0
	bcc.s	.Call
	move.w	d0,-(sp)
	clr.w	-(sp)
; Appelle l'evaluation
.Param	Rbsr	L_Dia_Evalue
; Pousse dans le bon registre
	move.w	(sp),d1
	jmp	.Move(pc,d1.w)
.Move	move.l	d0,d2
	bra.s	.Next
	move.l	d0,d3
	bra.s	.Next
	move.l	d0,d4
	bra.s	.Next
	move.l	d0,d5
	bra.s	.Next
	move.l	d0,d6
	bra.s	.Next
	move.l	d0,d7
; Un autre param?
.Next	addq.w	#4,(sp)
	subq.w	#1,2(sp)
	bne.s	.Param
	addq.l	#4,sp
	bra.s	.Call
; Pas de parametres
; ~~~~~~~~~~~~~~~~~
.NoPar	Rbsr	L_Dia_Chr
; Appelle l'instruction
; ~~~~~~~~~~~~~~~~~~~~~
.Call	move.l	(sp)+,a0
	jsr	(a0)
	move.l	Dia_PBuffer(a4),a0
	clr.l	(a0)
	bra	.INext

; Une instruction user!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Inst	move.l	Dia_Labels(a4),a0
.ULoop	move.w	(a0)+,d0
	addq.l	#4,a0
	Rbmi	L_Dia_Synt
	cmp.w	d0,d1
	bne.s	.ULoop
	cmp.w	#10,Dia_Users(a4)		Pas plus de 10 appels
	Rbcc	L_Dia_Fonc
; Pousse les anciennes valeurs
	move.l	a3,-(sp)
	move.l	Dia_Sp(a4),-(sp)
; Adresse dans le programme
	moveq	#0,d0
	move.w	-(a0),d0
	add.l	Dia_Programs(a4),d0
	move.l	d0,-(sp)
; Recupere les parametres
	move.w	-(a0),-(sp)			Nombre de parametres
	beq.s	.UNoPar
	move.w	(sp),-(sp)
.UPar	Rbsr	L_Dia_Evalue			Recupere les parametres
	move.l	d0,-(a3)
	subq.w	#1,(sp)
	bne.s	.UPar
	addq.l	#2,sp
	bra.s	.UCall
.UNoPar	Rbsr	L_Dia_Chr
; Branche au programme
.UCall	move.w	(sp)+,d1
	move.l	(sp),d0				A6
	exg	d0,a6
	move.l	d0,(sp)
	move.l	Dia_PUsers(a4),-(sp)		Position pile user
	move.l	a3,Dia_PUsers(a4)
	move.w	Dia_NPUsers(a4),-(sp)		Nombre params user
	move.w	d1,Dia_NPUsers(a4)
	addq.w	#1,Dia_Users(a4)
	bsr	Dia_Loop
	subq.w	#1,Dia_Users(a4)
	move.w	(sp)+,Dia_NPUsers(a4)
	move.l	(sp)+,Dia_PUsers(a4)
	move.l	(sp)+,a6
	move.l	(sp)+,Dia_Sp(a4)
	move.l	(sp)+,a3
	bra	.INext
; ________________________________________________________________________
;
; 	INSTRUCTIONS DBL
;

; GRAPHIC PLOT d2x,d3y,d4ink
; ~~~~~~~~~~~~
Dia_GPlot
	move.l	T_RastPort(a5),a1
	move.l	d4,d0				Set Ink A
	bmi.s	.Skip
	GfxCa5	SetAPen
.Skip	move.l	d2,d0
	move.l	d3,d1
	move.w	d0,Dia_XA(a4)
	move.w	d0,Dia_YA(a4)
	move.w	d1,Dia_XB(a4)
	move.w	d1,Dia_YB(a4)
	addq.w	#1,Dia_XB(a4)
	addq.w	#1,Dia_YB(a4)
	GfxCa5	WritePixel
	rts

; GRAPHIC ELLIPSE d2x,d3y,d4r1,d5r2
; ~~~~~~~~~~~~~~~
Dia_GEllipse
	move.l	T_RastPort(a5),a1
	move.l	d2,d0
	move.l	d3,d1
	move.l	d4,d2
	Rble	L_Dia_Fonc
	move.l	d5,d3
	Rble	L_Dia_Fonc
	move.w	d0,Dia_XA(a4)
	move.w	d0,Dia_YA(a4)
	move.w	d1,Dia_XB(a4)
	move.w	d1,Dia_YB(a4)
	add.l	Dia_BaseX(a4),d0
	add.l	Dia_BaseY(a4),d1
	GfxCa5	DrawEllipse
	rts

; GRAPHIC BOX d2x,d3y,d4x,d5y
; ~~~~~~~~~~~
Dia_GBox
	move.l	T_RastPort(a5),a1
	move.l	d2,d0
	move.l	d3,d1
	move.l	d4,d2
	move.l	d5,d3
	move.w	d0,Dia_XA(a4)
	move.w	d1,Dia_YA(a4)
	move.w	d2,Dia_XB(a4)
	move.w	d3,Dia_YB(a4)
	add.l	Dia_BaseX(a4),d0
	add.l	Dia_BaseY(a4),d1
	add.l	Dia_BaseX(a4),d2
	add.l	Dia_BaseY(a4),d3
	cmp.l	d0,d2
	Rble	L_Dia_Fonc
	cmp.l	d1,d3
	Rble	L_Dia_Fonc
	GfxCa5	RectFill
	rts
; GRAPHIC SQUARE d2x,d3y,d4x,d5y
; ~~~~~~~~~~~~~~
Dia_GSquare
	move.l	a2,-(sp)
	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	move.w	d4,Dia_XB(a4)
	move.w	d5,Dia_YB(a4)
	add.l	Dia_BaseX(a4),d2
	add.l	Dia_BaseY(a4),d3
	add.l	Dia_BaseX(a4),d4
	add.l	Dia_BaseY(a4),d5
	moveq	#16,d0
	Rbsr	L_Dia_GetBuffer
	move.l	T_RastPort(a5),a1
	move.w	d2,36(a1)
	move.w	d3,38(a1)
	addq.l	#4,a0
	move.l	a0,a2
	move.w	d2,(a2)+
	move.w	d5,(a2)+
	move.w	d4,(a2)+
	move.w	d5,(a2)+
	move.w	d4,(a2)+
	move.w	d3,(a2)+
	move.w	d2,(a2)+
	move.w	d3,(a2)+
	addq.w	#1,d3
	cmp.w	d5,d3
	bcc.s	.IBx1
	subq.w	#2,d3
.IBx1	moveq	#4,d0
	GfxCa5	PolyDraw
	move.l	(sp)+,a2
	rts
; GRAPHIC LINE d2x d3y d4x d5y
; ~~~~~~~~~~~~
Dia_GLine
	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	move.w	d4,Dia_XB(a4)
	move.w	d5,Dia_YB(a4)
	add.l	Dia_BaseX(a4),d2
	add.l	Dia_BaseY(a4),d3
	add.l	Dia_BaseX(a4),d4
	add.l	Dia_BaseY(a4),d5
	move.l	T_RastPort(a5),a1
	move.w	d2,36(a1)
	move.w	d3,38(a1)
	move.l	d4,d0
	move.l	d5,d1
	GfxCa5	RDraw
	rts

; SAVE d2BLOCK
; ~~~~~~~~~~~~
Dia_Block
	cmp.l	#"NobL",Dia_Magic(a5)
	beq.s	Dia_NoBloc
	moveq	#Dia_BlLong,d0
	Rbsr	L_Dia_GetBuffer
	move.w	#Dia_BlMark,2(a0)
	move.w	d2,4(a0)
	Rbsr	L_Dia_SetBuffer
Dia_Block2
	cmp.l	#"NobL",Dia_Magic(a5)
	beq.s	Dia_NoBloc
	move.l	d2,d1
	move.l	Dia_BaseX(a4),d2
	move.l	d2,d0
	and.w	#$FFF0,d2
	sub.l	d2,d0
	move.l	Dia_BaseY(a4),d3
	move.l	Dia_Sx(a4),d4
	add.l	d0,d4
	add.w	#15,d4
	and.w	#$FFF0,d4
	move.l	Dia_Sy(a4),d5
	moveq	#0,d6
	EcCall	BlGet
	Rbne	L_Dia_Synt
Dia_NoBloc
	rts

; KY d2CODE d3SHIFT
; ~~
Dia_Key
	moveq	#Dia_KyLong,d0
	Rbsr	L_Dia_GetBuffer
	move.w	#Dia_KyMark,2(a0)
	move.b	d2,Dia_KyCode(a0)
	move.b	d3,Dia_KyShift(a0)
	move.l	Dia_LastZone(a4),Dia_KyZone(a0)
	Rbsr	L_Dia_SetBuffer
	rts

; IF d2condition [instructions]
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_If	Rbsr	L_Dia_Chr
	cmp.b	#"[",d0
	Rbne	L_Dia_Synt
	tst.l	d2
	beq.s	.Skip
; Branche!
; ~~~~~~~~
	move.l	Dia_Sp(a4),-(sp)
	bsr	Dia_Loop
	move.l	(sp)+,Dia_Sp(a4)
; Saute l'instruction
; ~~~~~~~~~~~~~~~~~~~
.Skip	moveq	#1,d1
.Loop	move.b	(a6)+,d0
	Rbeq	L_Dia_Synt
	cmp.b	#"[",d0
	beq.s	.Plus
	cmp.b	#"]",d0
	bne.s	.Loop
	subq.w	#1,d1
	bne.s	.Loop
	rts
.Plus	addq.w	#1,d1
	bra.s	.Loop

; JSub d2NUMERO
; ~~~~
Dia_JSub
	move.l	a6,-(a3)
	move.l	#"MoiM",-(a3)
	bsr	Dia_Label
	rts
; RTs
; ~~~
Dia_Rts
	cmp.l	#"MoiM",(a3)+
	Rbne	L_Dia_Synt
	move.l	(a3)+,a6
	rts
; Jump d2NUMERO
; ~~~~
Dia_Jump
; Trouve un label D2
; ~~~~~~~~~~~~~~~~~~
Dia_Label
	move.l	d2,d0
	Rbsr	L_Dia_GetLabel
	move.l	Dia_Programs(a4),a6
	add.l	d0,a6
	rts

; Unpack Image d2X d3Y d4I
; ~~~~~~~~~~~~
Dia_Unpack
	move.l	d2,-(sp)
	move.l	Dia_Puzzle(a4),a0
	move.l	d4,d0
	add.l	Dia_PuzzleI(a4),d0
	Rbeq	L_Dia_Synt
	cmp.w	(a0),d0
	Rbhi	L_Dia_Synt
	lsl.w	#2,d0
	add.l	-4+2(a0,d0.w),a0
	move.l	Dia_Screen(a4),a1
	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	move.w	d2,Dia_XB(a4)
	move.w	d3,Dia_YB(a4)
	move.l	d2,d1
	move.l	d3,d2
	add.l	Dia_BaseX(a4),d1
	add.l	Dia_BaseY(a4),d2
	Rbsr	L_UnPack_Bitmap
	move.w	d0,Dia_PuzzleSx+2(a4)
	add.w	d0,Dia_XB(a4)
	swap	d0
	move.w	d0,Dia_PuzzleSy+2(a4)
	add.w	d0,Dia_YB(a4)
	move.l	(sp)+,d2
	rts
; Dessin d'une ligne d2X d3Y d4I d5X2
; ~~~~~~~~~~~~~~~~~~
Dia_Line
	movem.l	d2-d7,-(sp)
	and.l	#$FFFFFFF8,d2			Multiple de 8
	and.l	#$FFFFFFF8,d5
	move.l	d2,d6
	move.l	d5,d7
	sub.l	d2,d7
	Rble	L_Dia_Fonc
; Le corps de la ligne
	addq.l	#1,d4
.Loop	bsr	Dia_Unpack
	move.l	Dia_PuzzleSx(a4),d0
	beq.s	.Skip
	add.l	d0,d2
	sub.l	d0,d7
	cmp.l	d0,d7
	bge.s	.Loop
; La gauche
.Skip	move.l	d6,d2
	subq.l	#1,d4
	bsr	Dia_Unpack
; La droite
	move.l	d5,d2
	sub.l	Dia_PuzzleSx(a4),d2
	addq.l	#2,d4
	bsr	Dia_Unpack
	bra	Dia_BFin
; Dessin d'une ligne verticale d2X d3Y d4I d5Y2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_VLine
	movem.l	d2-d7,-(sp)
	and.l	#$FFFFFFF8,d2			Multiple de 8
	move.l	d3,d6
	move.l	d5,d7
	sub.l	d3,d7
	Rble	L_Dia_Fonc
; Le corps de la ligne
	addq.l	#1,d4
.Loop	bsr	Dia_Unpack
	move.l	Dia_PuzzleSy(a4),d0
	beq.s	.Skip
	add.l	d0,d3
	sub.l	d0,d7
	cmp.l	d0,d7
	bge.s	.Loop
; Le haut
.Skip	move.l	d6,d3
	subq.l	#1,d4
	bsr	Dia_Unpack
; Le bas
	move.l	d5,d3
	sub.l	Dia_PuzzleSy(a4),d3
	addq.l	#2,d4
	bsr	Dia_Unpack
	bra	Dia_BFin
; Dessin d'une boite d2X d3Y d4I d5X d6Y
; ~~~~~~~~~~~~~~~~~~
Dia_Box
	movem.l	d2-d7,-(sp)
	move.l	d3,-(sp)
	move.l	d6,d7
	sub.l	d3,d7
	Rble	L_Dia_Fonc
; Le haut
	addq.l	#3,d4
	bsr	Dia_Line
	move.l	Dia_PuzzleSy(a4),d0
	beq.s	.Ho
	sub.l	d0,d7
	ble.s	.Ho
; Screen copy du milieu
	movem.l	d2-d6,-(sp)
	move.l	d5,d6
.Copy	move.l	d2,d0
	move.l	d3,d1
	move.l	d6,d4
	add.l	Dia_PuzzleSy(a4),d3
	move.l	d3,d5
	Rbsr	L_Dia_ScCopy
	sub.l	Dia_PuzzleSy(a4),d7
	cmp.l	Dia_PuzzleSy(a4),d7
	bge.s	.Copy
	movem.l	(sp)+,d2-d6
; Le haut
.Ho	subq.l	#3,d4
	move.l	(sp)+,d3
	bsr	Dia_Line
; Le bas
	addq.l	#6,d4
	move.l	d6,d3
	sub.l	Dia_PuzzleSy(a4),d3
	bsr	Dia_Line
; Met les variables XX/YY
Dia_BFin
	movem.l	(sp)+,d2-d7
	move.w	d2,d0
	and.w	#$FFFFFFF8,d0
	move.w	d0,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	rts

; Set Size of the box d2SX d3SY
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_Size
	add.l	#15,d2			Multiple de 16
	and.l	#$FFFFFFF0,d2
	move.l	d2,Dia_Sx(a4)
	move.l	d3,Dia_Sy(a4)
	rts
; Set XA XB YA YB
; ~~~~~~~~~~~~~~~
Dia_XY	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	move.w	d4,Dia_XB(a4)
	move.w	d5,Dia_YB(a4)
	rts
; Set Base of the box d2X d3Y
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_Base
	and.l	#$FFFFFFF0,d2		Multiple de 16
	move.l	d2,Dia_BaseX(a4)
	move.l	d3,Dia_BaseY(a4)
	rts
; Set Puzzle Size d2IBase
; ~~~~~~~~~~~~~~~
Dia_Pu
	move.l	d2,Dia_PuzzleI(a4)
	rts
; Set Zone variable VAL
; ~~~~~~~~~~~~~~~~~~~~~
Dia_ZVar
	move.l	d2,Dia_NextZone(a4)
	rts
; Set Variable d2N,d3V
; ~~~~~~~~~~~~~~~~~~~~
Dia_SetVar
	move.l	d2,d0
	lsl.w	#2,d0
	lea	Dia_Vars(a4),a0
	move.l	d3,0(a0,d0.w)
	Rbsr	L_Dia_SetBuffer
	rts
; Call
; ~~~~~~~
Dia_Call
	movem.l	a2-a6/d2-d7,-(sp)
	move.l	d2,a0
	jsr	(a0)
	movem.l	(sp)+,a2-a6/d2-d7
	rts
; Move screen
; ~~~~~~~~~~~~~~~~~~~~~~
Dia_SMove
	movem.l	a2/d2,-(sp)
	move.l	T_EcCourant(a5),a2
	SyCall	XyMou
	sub.w	EcAWY(a2),d2
	move.w	d2,-(sp)
.MLoop	SyCall	XyMou
	sub.w	(sp),d2
	cmp.w	EcAWY(a2),d2
	beq.s	.MSkip
	move.w	d2,EcAWY(a2)
	bset	#2,EcAW(a2)
	EcCall	CopForce
	Rbsr	L_Dia_WaitMul
.MSkip	SyCall	MouseKey
	btst	#0,d1
	bne.s	.MLoop
	tst.w	(sp)+
	movem.l	(sp)+,a2/d2
	rts

; Debug
; ~~~~~~~
Dia_Illegal
	illegal
Dia_Rien
	rts
; Inks d2A d3B d4C
; ~~~~
Dia_SInk
	move.l	T_RastPort(a5),a1
	move.l	d4,d0			Ink C
	bmi.s	.Skip1
	move.b	d0,27(a1)
.Skip1	move.l	d3,d0			Ink B
	bmi.s	.Skip2
	GfxCa5	SetBPen
.Skip2	move.l	d2,d0			Ink A
	bmi.s	.Skip3
	GfxCa5	SetAPen
.Skip3	rts
; SET SLIDER A,B,C,P,A,B,C,P
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_SetSlider
	lea	Dia_SlDefault(a4),a2
	moveq	#7,d2
.Loop	Rbsr	L_Dia_Evalue
	move.b	d0,(a2)+
	dbra	d2,.Loop
	move.l	-8(a2),(a2)+
	move.l	-8(a2),(a2)+
	rts

; Print TEXT Outline d2X d3Y d4Text d5Ink1 d6Ink2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_Outline
; Writing 0
	move.l	T_RastPort(a5),a1
	moveq	#0,d0
	bsr	Dia_Write
; Imprime le fond
	subq.l	#1,d2
	bsr.s	Dia_Text
	moveq	#-1,d5
	addq.l	#2,d2
	bsr.s	Dia_Text
	subq.l	#1,d2
	subq.l	#1,d3
	bsr.s	Dia_Text
	addq.l	#2,d3
	bsr.s	Dia_Text
	subq.l	#1,d3
; Imprime le devant
	move.l	d6,d5
; Print TEXT d2X d3Y d4Text d5Ink
; ~~~~~~~~~~
Dia_Text
	move.l	T_RastPort(a5),a1
	move.l	d5,d0
	bmi.s	.SkipI
	GfxCa5	SetAPen
.SkipI	move.l	d2,d0
	move.w	d3,d1
	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	move.w	d2,Dia_XB(a4)
	move.w	d3,Dia_YB(a4)
	add.l	Dia_BaseX(a4),d0
	add.l	Dia_BaseY(a4),d1
	move.w	d0,36(a1)
	add.w	62(a1),d1
	move.w	d1,38(a1)
	move.l	d4,a0
	tst.b	(a0)+
	moveq	#0,d0
	move.b	(a0)+,d0
	beq.s	.Skip
	move.l	a6,-(sp)
	movem.l	a0/a1/d0,-(sp)		Imprime
	move.l	T_GfxBase(a5),a6
	jsr	Text(a6)
	movem.l	(sp)+,a0/a1/d0
	move.w	58(a1),d1		Hauteur
	add.w	d1,Dia_YB(a4)
	jsr	TextLength(a6)		Largeur
	add.w	d0,Dia_XB(a4)
	move.l	(sp)+,a6
.Skip	rts
; Print VTEXT d2X d3Y d4Text d5Ink
; ~~~~~~~~~~
Dia_VText
	movem.l	a6/d2-d4,-(sp)
	move.l	T_RastPort(a5),a1
	move.l	d5,d0
	bmi.s	.SkipI
	GfxCa5	SetAPen
.SkipI	move.l	d2,d0
	move.w	d3,d1
	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	move.w	d2,Dia_XB(a4)
	move.w	d3,Dia_YB(a4)
	add.l	Dia_BaseX(a4),d2
	add.l	Dia_BaseY(a4),d3
	add.w	62(a1),d3
	move.l	d4,a0
	tst.b	(a0)+
	moveq	#0,d4
	move.b	(a0)+,d4
	beq.s	.Skip
	move.l	T_GfxBase(a5),a6
.Loop	moveq	#1,d0			Imprime une lettre
	move.w	d2,36(a1)
	move.w	d3,38(a1)
	movem.l	a0/a1,-(sp)
	jsr	Text(a6)
	movem.l	(sp)+,a0/a1
	addq.l	#1,a0
	move.w	58(a1),d1
	add.w	d1,d3
	add.w	d1,Dia_XB(a4)
	subq.w	#1,d4
	bne.s	.Loop
	movem.l	(sp)+,a6/d2-d4
.Skip	rts
; SET FONT d2FONT d3STYLE
; ~~~~~~~~
Dia_SFont
	move.l	d2,d1
	bmi.s	.Skip1
	EcCall	SFont
.Skip1	tst.l	d3
	bmi.s	.Skip2
	move.l	T_RastPort(a5),a1
	move.b	d3,56(a1)
.Skip2	rts
; WRITING d2MODE
; ~~~~~~~
Dia_SWrite
	move.l	d2,d0
	bmi.s	.Skip
	bsr	Dia_Write
.Skip	rts

; Change le writing courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_Write
;	cmp.w	Dia_Writing(a4),d0
;	beq.s	.Skip
	move.w	d0,Dia_Writing(a4)
	move.l	T_RastPort(a5),a1
	GfxCa5	SetDrMd
.Skip	rts

; SET LINE d2PATTERN
; ~~~~~~~~
Dia_SLine
	tst.l	d2
	bmi.s	.Skip
	move.l	T_RastPort(a5),a1
	move.w	d2,34(a1)
.Skip	rts
; PATTERN d2PATTERN d3OUTLINE
; ~~~~~~~
Dia_SFill
	move.l	T_RastPort(a5),a1
	tst.l	d3
	bmi.s	.Skip
	bclr	#4,33(a1)
	tst.w	d3
	beq.s	.Skip
	bset	#4,33(a1)
.Skip	move.l	d2,d1
	EcCall	Pattern
	rts

; ________________________________________________________________________
;
;	Interface avec l'�diteur ligne
;
; DIgit	Numero,X,Y,Long,Valeur,Flag,Paper,Pen
; ~~~~~
Dia_Digit
	movem.l	a2/d2-d7,-(sp)
	moveq	#Dia_DiLong,d0
	Rbsr	L_Dia_GetBuffer
	move.l	a0,a2
	move.w	#Dia_EdMark,2(a2)
	move.l	a2,Dia_LastZone(a4)
; Numero / Position / Largeur
	bsr	Dia_EdDiRout
; Place pour le chiffre
	Rbsr	L_Dia_Evalue		Variable
	move.l	d0,Dia_DiValue(a2)
; Afficher le chiffre de depart?
	Rbsr	L_Dia_Evalue
	move.l	d0,d1
	sub.l	a0,a0
	btst	#0,d1
	beq.s	.Paa
	move.l	Dia_DiValue(a2),-(a3)
	moveq	#1,d7
	Rbsr	L_Dia_FDecimal
	move.l	(a3)+,a0
.Paa	move.w	#(1<<LEd_FMouCur)|(1<<LEd_FMouse)|(1<<LEd_FOnce)|(1<<LEd_FKeys)|(1<<LEd_FFilter),d0
	move.w	d4,d1			Largeur= largeur fenetre-1
	subq.w	#1,d1
	lea	Dia_DiBuffer(a2),a1
	pea	Dia_LEd+LEd_Mask(a2)
	bsr	Dia_Edit2
	move.l	(sp)+,a0
	move.l	#%00000000000101011111111111000000,(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	movem.l	(sp)+,a2/d2-d7
	rts

; EDit Numero X Y Long Max Chaine Paper Pen
; ~~~~
Dia_Edit
	movem.l	a2/d2-d7,-(sp)
	moveq	#Dia_EdLong,d0
	Rbsr	L_Dia_GetBuffer
	move.l	a0,a2
	move.w	#Dia_EdMark,2(a2)
	move.l	a2,Dia_LastZone(a4)
; Numero / Position / Longueur
	bsr	Dia_EdDiRout
; Longmax
	Rbsr	L_Dia_Evalue
	tst.l	d0
	Rbeq	L_Dia_Fonc
	cmp.l	#1024,d0
	Rbhi	L_Dia_Fonc
	move.w	d0,-(sp)
; Reserve le buffer
	addq.w	#6,d0
	Rbsr	L_Dia_GetBuffer
	move.w	#Dia_StMark,2(a0)
	pea	4(a0)
; Copie la chaine de d�part, finie par zero
	Rbsr	L_Dia_Evalue
	move.l	d0,a0
; Flags
	move.l	(sp)+,a1
	move.w	#(1<<LEd_FMouCur)|(1<<LEd_FMouse)|(1<<LEd_FOnce)|(1<<LEd_FKeys),d0
	move.w	(sp)+,d1
	bsr	Dia_Edit2
	movem.l	(sp)+,a2/d2-d7
	rts

; Entree pour DIGIT, A0=chaine D1=longmax D0=Flags A1=Buffer
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_Edit2
	movem.l	a1/d0/d1,-(sp)
; Recopie la chaine dans le buffer
	move.l	a0,d0
	beq.s	.Sk4
	moveq	#0,d0
	tst.b	(a0)+
	move.b	(a0)+,d0
	cmp.w	d1,d0
	bcc.s	.Sk3
	move.w	d0,d1
	bra.s	.Sk3
.Sk2	move.b	(a0)+,d0
	cmp.b	#32,d0
	bcs.s	.Sk3
	move.b	d0,(a1)+
.Sk3	dbra	d1,.Sk2
.Sk4	clr.b	(a1)
; Cree la fenetre
	moveq	#1,d5
	moveq	#0,d6
	moveq	#0,d7
	sub.l	a1,a1
	move.w	Dia_ZoNumber(a2),d1
	add.w	#1000,d1		Fenetres � partir de 1000
	move.w	d1,Dia_WindOn(a4)
	WiCall	WindOp
; Paper / Pen
	Rbsr	L_Dia_Evalue
	move.w	d0,d2
	Rbsr	L_Dia_Evalue
	move.b	d0,d3
; Reserve la zone / Initialise
	lea	.Init(pc),a1
	add.b	#"0",d2
	move.b	d2,.Paper-.Init(a1)
	add.b	#"0",d3
	move.b	d3,.Pen-.Init(a1)
	WiCall	Print
; Imprime le texte, curseur � la fin
	movem.l	(sp)+,a1/d0/d2
	moveq	#0,d1
	moveq	#-1,d3
	lea	Dia_LEd(a2),a0
	move.l	a1,a2
	Rbsr	L_LEd_Init
	Rbne	L_Dia_Fonc
	Rbsr	L_Dia_SetBuffer
	rts
; Chaine d'init
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Init	dc.b	27,"C0",27,"B"
.Paper	dc.b	"0",27,"P"
.Pen	dc.b	"0",27,"J",48+31,25,27,"V0",0
	even


; ________________________________________________________________________
;
;	ZONE D'HYPER-TEXTE
;
; HText Numero X Y Tx Ty Txt Pos Zones Paper Pen; []
; ~~~~~
Dia_HyperText
	movem.l	a2/d2-d7,-(sp)
	move.l	#Dia_TxLong,d0
	Rbsr	L_Dia_GetBuffer
	move.l	a0,a2
	move.w	#Dia_TxMark,2(a2)
; Numero / Position / Longueur
	bsr	Dia_EdDiRout
; Largeur / Hauteur
	move.w	d4,Dia_TxTx(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_TxTy(a2)
	move.w	d0,d5
	lsl.w	#3,d0
	move.w	d0,Dia_ZoSy(a2)
; Adresse du texte
	Rbsr	L_Dia_Evalue
	cmp.l	#1*1024,d0		*** Illegal
	Rble	L_Dia_Fonc
	move.l	d0,Dia_TxText(a2)
; Position dans le texte
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_TxPos(a2)
; Cree la fenetre
	moveq	#0,d6
	moveq	#0,d7
	sub.l	a1,a1
	move.w	Dia_ZoNumber(a2),d1
	add.w	#3000,d1		Fenetres � partir de 3000
	move.w	d1,Dia_WindOn(a4)
	WiCall	WindOp
	tst.w	d0
	Rbne	L_Dia_Fonc
; Nombre de zones/ligne
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_TxDispMax(a2)
	mulu	#Dia_TxDispZone,d0
	addq.l	#4+4,d0
	move.w	d0,Dia_TxDispSize(a2)
; Paper / Pen
	Rbsr	L_Dia_Evalue
	move.b	d0,d2
	Rbsr	L_Dia_Evalue
	move.b	d0,d3
; Initialise
	add.b	#"0",d2
	add.b	#"0",d3
	move.b	d2,Dia_TxPaper(a2)
	move.b	d3,Dia_TxPen(a2)
	lea	.Pp(pc),a0
	lea	Dia_TxPp(a2),a1
	move.l	(a0)+,(a1)
	move.l	(a0)+,4(a1)
	move.b	d2,2(a1)
	move.b	d3,5(a1)
	WiCall	Print
	lea	.Clw(pc),a1
	move.w	CurTab(a5),d0
	add.b	#"0",d0
	move.b	d0,6(a1)
	WiCall	Print
; Recupere la routine
	Rbsr	L_Dia_GetRout
	move.w	d1,Dia_ZoRChange(a2)
; Trouve tous les pointeurs sur les lignes / Compte les lignes
	lea	-16(a3),a0
	move.l	a0,d4
	move.l	Dia_TxText(a2),a0
	move.l	Dia_ABuffer(a4),a1
	lea	4(a1),a1
	move.l	a1,Dia_TxAdress(a2)
	moveq	#0,d1
	bra.s	.In
.NLine  sub.l	d2,d3
	move.b	d3,-1(a1)		Longueur de la ligne precedente
.In	move.l	a0,d2
	move.l	d2,d3
	cmp.l	d4,a1
	Rbcc	L_Dia_OBuffer
	addq.l	#1,d1
	move.l	a0,d0
	sub.l	Dia_TxText(a2),d0
	lsl.l	#8,d0
	move.l	d0,(a1)+
.CLoop	move.b	(a0)+,d0
	cmp.b	#32,d0
	bcc.s	.CLoop
	tst.b	d0
	beq.s	.CFini
	cmp.b	#27,d0
	beq.s	.CEsc
	cmp.b	#10,d0
	beq.s	.C10
	cmp.b	#13,d0
	bne.s	.CLoop
.C10	move.l	a0,d3
	subq.l	#1,d3
	cmp.b	#13,(a0)
	bne.s	.NLine
	addq.l	#1,a0
	bra.s	.NLine
.C13	move.l	a0,d3
	subq.l	#1,d3
	cmp.b	#10,(a0)
	bne.s	.NLine
	addq.l	#1,a0
	bra.s	.NLine
.CEsc	addq.l	#2,a0
	bra.s	.CLoop
.CFini	sub.l	d2,d3
	move.b	d3,-1(a1)
	move.w	d1,Dia_TxNLine(a2)
	move.l	d1,Dia_NextZone(a4)		Met dans la variable temporaire
	move.l	Dia_ABuffer(a4),a0
	move.l	a1,Dia_ABuffer(a4)
	clr.l	(a1)
	sub.l	a0,a1
	move.w	a1,(a0)
	move.w	#Dia_TaMark,2(a0)
; Reserve le buffer de travail
	move.w	Dia_TxTy(a2),d0
	mulu	Dia_TxDispSize(a2),d0
	addq.l	#4,d0
	Rbsr	L_Dia_GetBuffer
	move.w	#Dia_TdMark,2(a0)
	lea	4(a0),a0
	move.l	a0,Dia_TxDisplay(a2)
; Fixe le buffer!!!
	Rbsr	L_Dia_SetBuffer
; Affiche le contenu!
	Rbsr	L_Dia_TxDraw
; Fini
	movem.l	(sp)+,a2/d2-d7
	rts
	even
.Pp	dc.b	27,"B0",27,"P0",0
.Clw	dc.b	27,"C0",25,27,"T0",27,"V0",0
	even

; ________________________________________________________________________
;
;	ZONE DE LISTE
;
; AList Numero X Y Tx Ty Tab$ Pos Flags Paper Pen; []
; ~~~~~
Dia_List
	movem.l	a2/d2-d7,-(sp)
	moveq	#Dia_LiLong,d0
	Rbsr	L_Dia_GetBuffer
	move.l	a0,a2
	move.w	#Dia_LiMark,2(a2)
; Numero / Position / Longueur
	bsr	Dia_EdDiRout
; Largeur / Hauteur
	move.w	d4,Dia_LiTx(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_LiTy(a2)
	move.w	d0,d5
	lsl.w	#3,d0
	move.w	d0,Dia_ZoSy(a2)
; Tableau
	Rbsr	L_Dia_Evalue
	tst.l	d0
	beq.s	.ANul
	cmp.l	#1*1024,d0		*** Illegal
	Rble	L_Dia_Fonc
	move.l	d0,Dia_LiArray(a2)
	move.l	d0,a0
	cmp.b	#1,(a0)			Nombre de dimensions
	Rbne	L_Dia_Fonc
	cmp.b	#2,1(a0)		Taille variable=4!
	Rbne	L_Dia_Fonc
	moveq	#0,d0
	move.w	2(a0),d0
	cmp.w	#32768,d0
	Rbcc	L_Dia_Fonc
.ANul	move.w	d0,Dia_LiMaxAct(a2)
	move.w	d0,-(sp)
; Position dans tableau
	Rbsr	L_Dia_Evalue
	cmp.w	Dia_LiMaxAct(a2),d0
	bcs.s	.PNul
	move.w	Dia_LiMaxAct(a2),d0
.PNul	move.w	d0,Dia_LiPos(a2)
	move.w	#-1,Dia_LiActNumber(a2)
	move.l	#-1,Dia_ZoPos(a2)
; Cree la fenetre
	moveq	#0,d6
	moveq	#0,d7
	sub.l	a1,a1
	move.w	Dia_ZoNumber(a2),d1
	add.w	#2000,d1		Fenetres � partir de 1000
	move.w	d1,Dia_WindOn(a4)
	WiCall	WindOp
	tst.w	d0
	Rbne	L_Dia_Fonc
; Flags
	Rbsr	L_Dia_Evalue
	move.b	d0,Dia_ZoFlags+1(a2)
; Paper / Pen
	Rbsr	L_Dia_Evalue
	move.w	d0,d2
	Rbsr	L_Dia_Evalue
	move.b	d0,d3
; Initialise
	lea	Dia_WInit(pc),a1
	add.b	#"0",d2
	move.b	d2,Dia_WPaper-Dia_WInit(a1)
	add.b	#"0",d3
	move.b	d3,Dia_WPen-Dia_WInit(a1)
	WiCall	Print
; Fixe le buffer
	Rbsr	L_Dia_SetBuffer
; Recupere la routine
	Rbsr	L_Dia_GetRout
	move.w	d1,Dia_ZoRChange(a2)
; Trouve la longueur de l'affichage du tableau
	moveq	#0,d0
	move.w	(sp)+,d0
	move.l	Dia_ABuffer(a4),a0
	addq.l	#4,a0
	move.l	a0,a1
	Rbsr	L_Dia_DecToAsc
	sub.l	a1,a0
	move.w	a0,Dia_LiLArray(a2)
; Affiche le contenu!
	Rbsr	L_Dia_LiDraw
; Fini
	movem.l	(sp)+,a2/d2-d7
	rts
; Chaine d'init fenetre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_WInit	dc.b	27,"C0",27,"B"
Dia_WPaper	dc.b	"0",27,"P"
Dia_WPen	dc.b	"0",27,"J",48+31,25,27,"V0",0
		even

; Routine pour Ed et Digit
; ~~~~~~~~~~~~~~~~~~~~~~~~
Dia_EdDiRout
; Numero
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_ZoNumber(a2)
; Positions
	move.w	#Dia_ZoMark,Dia_ZoId(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_XA(a4)
	move.w	d0,Dia_XB(a4)
	add.l	Dia_BaseX(a4),d0
	and.l	#$FFF0,d0
	move.l	d0,d2
	move.w	d0,Dia_ZoX(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_YA(a4)
	move.w	d0,Dia_YB(a4)
	add.l	Dia_BaseY(a4),d0
	move.l	d0,d3
	move.w	d0,Dia_ZoY(a2)
; Largeur
	Rbsr	L_Dia_Evalue
	addq.w	#1,d0
	and.w	#$FFFE,d0
	Rbeq	L_Dia_Synt
	add.w	d0,Dia_XB(a4)
	addq.w	#8,Dia_YB(a4)
	move.w	d0,d4			Taille de la fenetre paire
	lsl.w	#3,d0
	move.w	d0,Dia_ZoSx(a2)
	move.w	#8,Dia_ZoSy(a2)
; Variable de zone
	move.l	Dia_NextZone(a4),Dia_ZoVar(a2)
	rts

; ________________________________________________________________________
;
; 	Interface avec les sliders
;
; SH Numero, X, Y, SX, SY, Pos, Fenetre, Total, Step;[]
; ~~
Dia_SliderH
	moveq	#0,d6
	bra.s	Dia_Slider
Dia_SliderV
	moveq	#-1,d6
Dia_Slider
	move.l	a2,-(sp)
	moveq	#Dia_SlLong,d0
	bsr	Dia_GetEntete
	move.w	#Dia_SlMark,2(a2)
	move.w	Dia_ZoX(a2),Dia_Sl+Sl_X(a2)
	move.w	Dia_ZoY(a2),Dia_Sl+Sl_Y(a2)
	move.w	Dia_ZoSx(a2),Dia_Sl+Sl_Sx(a2)
	subq.w	#1,Dia_Sl+Sl_Sx(a2)
	move.w	Dia_ZoSy(a2),Dia_Sl+Sl_Sy(a2)
	subq.w	#1,Dia_Sl+Sl_Sy(a2)
; Horizontal ou vertical?
	move.w	Dia_Sl+Sl_X(a2),Dia_Sl+Sl_Start(a2)
	move.w	Dia_Sl+Sl_Sx(a2),Dia_Sl+Sl_Size(a2)
	tst.w	d6
	beq.s	.PaV
	bset	#Sl_FlagVertical,Dia_Sl+Sl_Flags(a2)
	move.w	Dia_Sl+Sl_Y(a2),Dia_Sl+Sl_Start(a2)
	move.w	Dia_Sl+Sl_Sy(a2),Dia_Sl+Sl_Size(a2)
.PaV
; Position de depart
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_Sl+Sl_Position(a2)
	move.l	d0,Dia_ZoPos(a2)
; Taille de depart
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_Sl+Sl_Window(a2)
; Maximum de reference
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_Sl+Sl_Global(a2)
; +/- scrolling
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_Sl+Sl_Scroll(a2)
; Routines
	lea	Dia_RSlider(pc),a0
	move.l	a0,Dia_Sl+Sl_Routines(a2)
; Couleurs
	lea	Dia_SlDefault(a4),a1
	lea	Dia_Sl+Sl_Inactive(a2),a0
	moveq	#16-1,d0
.Co	moveq	#0,d1
	move.b	(a1)+,d1
	move.w	d1,(a0)+
	dbra	d0,.Co
; Fixe la position du buffer
	Rbsr	L_Dia_SetBuffer
; Routine de dessin
	Rbsr	L_Dia_GetRout
	move.w	d1,Dia_ZoRChange(a2)
; Dessine le slider
; ~~~~~~~~~~~~~~~~~
	moveq	#0,d0
	lea	Dia_Sl(a2),a0
	Rbsr	L_Sl_Draw
	move.w	Dia_Writing(a4),d0
	move.l	T_RastPort(a5),a1
	GfxCa5	SetDrMd
; Ok!
; ~~~
	move.l	(sp)+,a2
	rts

; Interface de dessin
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_RSlider
	bra	Dia_SlUpdate
	bra	Dia_SlChange
	bra	Dia_SlChange
	bra	Dia_SlChange
; Appelle la routine change
; ~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_SlChange
	move.l	a2,-(sp)
	lea	-Dia_Sl(a2),a2
	moveq	#0,d0
	move.w	Dia_Sl+Sl_Position(a2),d0
	move.l	d0,Dia_ZoPos(a2)
	Rbsr	L_Dia_ZoChange
	move.l	(sp)+,a2
	rts
; Update nul
; ~~~~~~~~~~
Dia_SlUpdate
	rts

; ________________________________________________________________________
;
; 	Interface avec les boutons
;
; Bouton Numero, X, Y, SX, SY, Pos, Min, Max;[] []
; ~~~~~~
Dia_Button
	move.l	a2,-(sp)
	moveq	#Dia_BtLong,d0
	bsr	Dia_GetEntete
	move.w	#Dia_BtMark,2(a2)
	move.l	a2,Dia_LastZone(a4)
; Position de depart
	Rbsr	L_Dia_Evalue
	move.l	d0,Dia_ZoPos(a2)
; Minimum et maximum
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_BtMin(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_BtMax(a2)
	clr.b	Dia_ZoFlags(a2)
; Fixe la position du buffer
	Rbsr	L_Dia_SetBuffer
; Routine de dessin du bouton
	Rbsr	L_Dia_GetRout
	move.w	d1,Dia_BtRDraw(a2)
; Routine de changement de position
	Rbsr	L_Dia_GetRout
	move.w	d1,Dia_ZoRChange(a2)
; Initialisation du bouton
	Rbsr	L_Dia_BtDraw
; Termine!
	move.l	(sp)+,a2
	rts

; Fabrique un entete de zone active - D0=longueur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D0=	longueur
Dia_GetEntete
	movem.w	d2/d3,-(sp)
	Rbsr	L_Dia_GetBuffer
	move.l	a0,a2
; Numero
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_ZoNumber(a2)
; X/Y/SX/SY
	move.w	#Dia_ZoMark,Dia_ZoId(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,d2
	add.l	Dia_BaseX(a4),d0
	move.w	d0,Dia_ZoX(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,d3
	add.l	Dia_BaseY(a4),d0
	move.w	d0,Dia_ZoY(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_ZoSx(a2)
	Rbsr	L_Dia_Evalue
	move.w	d0,Dia_ZoSy(a2)
; Variables internes
	move.w	d2,Dia_XA(a4)
	move.w	d3,Dia_YA(a4)
	add.w	Dia_ZoSx(a2),d2
	move.w	d2,Dia_XB(a4)
	add.w	Dia_ZoSy(a2),d3
	move.w	d3,Dia_YB(a4)
; Variable interne
	move.l	Dia_NextZone(a4),Dia_ZoVar(a2)
	movem.w	(sp)+,d2/d3
	rts

; Button Return d2V/F -> mis dans VAR-2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_BR	move.l	d2,Dia_Vars-2*4(a4)
	rts

; Button Quit
; ~~~~~~~~~~~
Dia_BQuit
	move.l	Dia_Vars-3*4(a4),d0		Dans un bouton?
	Rbeq	L_Dia_Synt
	move.l	d0,a0
	bset	#7,Dia_ZoFlags(a0)
	rts

; Button Nowait
; ~~~~~~~~~~~~~
Dia_NWait
	move.l	Dia_Vars-3*4(a4),d0		Dans un bouton?
	Rbeq	L_Dia_Synt
	move.l	d0,a0
	bset	#5,Dia_ZoFlags(a0)
	rts
; Button Change, d2NUMBER d3POSITION
; ~~~~~~~~~~~~~~
Dia_BChange
; Peut-on effectuer l'instruction?
	move.l	Dia_Vars-3*4(a4),d0		Si appel� en dehors de la parenth
	Rbeq	L_Dia_Synt
	move.l	d0,a0
	btst	#6,Dia_ZoFlags(a0)
	beq.s	.Ok
	rts
; Explore la liste
.Ok	movem.l	a2/d2/d3,-(sp)
	move.l	Dia_Buffer(a4),a2
.Loop	cmp.w	#Dia_BtMark,2(a2)		Un bouton?
	bne.s	.Next
	cmp.w	Dia_ZoNumber(a2),d2		Le bon?
	bne.s	.Next
	cmp.l	Dia_Vars-3*4(a4),a2		Pas celui-ci?
	beq.s	.Next
	bset	#6,Dia_ZoFlags(a2)		Marque le flag!
	cmp.l	Dia_ZoPos(a2),d3		Faut-il changer le bouton?
	beq.s	.Deja
	move.l	d3,Dia_ZoPos(a2)		Change le bouton
	Rbsr	L_Dia_BtDraw			Va le dessiner
	Rbsr	L_Dia_ZoChange			Va le changer
.Deja	bclr	#6,Dia_ZoFlags(a2)		Enleve le flag!
	bclr	#7,Dia_ZoFlags(a2)		Pas de sortie!
.Next	add.w	(a2),a2
	tst.w	(a2)
	bne.s	.Loop
	movem.l	(sp)+,a2/d2/d3
	rts

; ________________________________________________________________________
;
;	INTERFACE AVEC LES ZONES ACTIVES
;
; Zone Change, d2NUMBER d3POSITION
; ~~~~~~~~~~~
Dia_ZChange
	movem.l	d4-d5,-(sp)
; Peut-on effectuer l'instruction?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.l	Dia_Vars-3*4(a4)		Si appel� en dehors d'une parenth
	Rbeq	L_Dia_Synt
	move.l	#EntNul,d4
	move.l	d4,d5
	Rbsr	L_Dia_ZUpdate
	movem.l	(sp)+,d4-d5
	rts



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	INSTRUCTION RUn d3Flag d2Timer
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Dia_Run
; - - - - - - - - - - - - -
	move.b	d3,Dia_Flags(a4)
	move.l	d2,Dia_Timer(a4)
	move.l	T_VblCount(a5),Dia_TimerPos(a4)
	bset	#1,Dia_RFlags(a4)
; Nettoyage des entrees
; ~~~~~~~~~~~~~~~~~~~~~
	bclr	#0,d3			Nettoyer les touches
	beq.s	.Skip1
	SyCall	ClearKey
.Skip1	bclr	#1,d3			Nettoyer les clicks
	beq.s	.Skip2
	Rbsr	L_Dia_NoMKey
.Skip2
; Active la premiere fenetre
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	Rbsr	L_Dia_EdFirst
; Boucle d'attente
; ~~~~~~~~~~~~~~~~
.Loop	Rjsr	L_Sys_WaitMul
; Le timer
	move.l	Dia_Timer(a4),d0
	beq.s	.Skip
	move.l	T_VblCount(a5),d1
	sub.l	Dia_TimerPos(a4),d1
	cmp.l	d0,d1
	bcc.s	.Exit0
.Skip
; Control-C?
	move.w	T_Actualise(a5),d0
	bclr	#BitControl,d0
	beq.s	.PaC
	move.w	d0,T_Actualise(a5)
	bra.s	.Exit0
.PaC
; Un coup de test.
	Rbsr	L_Dia_Tests
	bne.s	.Exit
; Branchement au monitor?
	move.l	Patch_ScFront(a5),d0
	beq.s	.Loop
	move.l	d0,a0
	jsr	(a0)
	bra.s	.Loop
; Sortie...
; ~~~~~~~~~
.Exit0	clr.w	Dia_Return(a4)
; Remet l'ecran du moniteur
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
.Exit	tst.l	Patch_ScFront(a5)
	beq.s	.Exit1
	movem.l	a0/d0,-(sp)
	move.l	Patch_ScFront(a5),a0
	jsr	(a0)
	movem.l	(sp)+,a0/d0
.Exit1	tst.w	d0
	rts



; ________________________________________________________________________
;
; 	EVALUATION D'EXPRESSION
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_Evalue
; - - - - - - - - - - - - -
	move.l	d7,-(sp)
	moveq	#0,d7
.Loop	Rbsr	L_Dia_Chr		Premiere lettre
	bmi.s	.Chiffre
	bvs.s	.Signe
	beq.s	.Fini
	bcc.s	.Fini
	Rlea	L_Dia_FTokens,1
	move.b	-64(a1,d0.w),d0		Offset des codes
	lea	32(a1,d0.w),a1		Pointe la liste
	Rbsr	L_Dia_Chr
.FLoop	move.w	(a1)+,d1		Trouve le bon
	cmp.b	d0,d1
	bne.s	.FLoop
	lsr.w	#8,d1			Pointe la fonction
	jsr	.Jumps(pc,d1.w)		Appel!
	bra.s	.Loop
.Signe	Rlea	L_Dia_FTokens,1
	lea	32(a1),a1
	bra.s	.FLoop
.Chiffre
	Rbsr	L_Dia_FChif
	bra.s	.Loop
; On a fini l'expression
.Fini	cmp.w	#1,d7
	Rbne	L_Dia_Synt
	move.l	(a3)+,d0
	move.l	(sp)+,d7
	rts
.Jumps	bra	Dia_FBx			0
	bra	Dia_FBy			1
	bra	Dia_FSx			2
	bra	Dia_FSy			3
	bra	Dia_FPlus		4
	bra	Dia_FMoins		5
	bra	Dia_FMul		6
	bra	Dia_FDiv		7
	bra	Dia_FNeg		8
	bra	Dia_FMess		9
	bra	Dia_FMess		10
	bra	Dia_FTw			11
	bra	Dia_FTh			12
	bra	Dia_FVar		13
	bra	Dia_FString1		14
	bra	Dia_FString2		15
	bra	Dia_FCx			16
	bra	Dia_FSw			17
	bra	Dia_FSh			18
	bra	Dia_FMin		19
	bra	Dia_FMax		20
	bra	Dia_FBP			21
	Rbra	L_Dia_FDecimal		22
	bra	Dia_FPlusCh		23
	bra	Dia_FStZero		24
	bra	Dia_FXA			25
	bra	Dia_FYA			26
	bra	Dia_FXB			27
	bra	Dia_FYB			28
	bra	Dia_FArray		29
	bra	Dia_FZP			30
	bra	Dia_P1			31
	bra	Dia_P2			32
	bra	Dia_P3			33
	bra	Dia_P4			34
	bra	Dia_P5			35
	bra	Dia_P6			36
	bra	Dia_P7			37
	bra	Dia_P8			38
	bra	Dia_P9			39
	bra	Dia_FEq			40
	bra	Dia_FNEq		41
	bra	Dia_FInf		42
	bra	Dia_FSup		43
	bra	Dia_FAnd		44
	bra	Dia_FOr			45
	bra	Dia_FZVar		46
	bra	Dia_FZNum		47
	bra	Dia_FLTxt		48
	bra	Dia_FASize		49

; Prend une variable
; ~~~~~~~~~~~~~~~~~~
Dia_FVar
	tst.w	d7
	Rble	L_Dia_Synt
	move.l	(a3),d0
	lsl.w	#2,d0
	lea	Dia_Vars(a4),a0
	move.l	0(a0,d0.l),(a3)
	rts
; = ZV Une variable interne � une zone
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_FZVar
	addq.w	#1,d7
	move.l	Dia_Vars-3*4(a4),d0
	beq.s	.Ext
; Variable � l'interieur de la zone
	move.l	d0,a0
	move.l	Dia_ZoVar(a0),-(a3)
	rts
; Variable � l'exterieur de la zone
.Ext	move.l	Dia_NextZone(a4),-(a3)
	rts
; = ZN Numero de la zone
; ~~~~~~~~~~~~~~~~~~~~~~
Dia_FZNum
	move.l	Dia_Vars-3*4(a4),d0
	Rbeq	L_Dia_Synt
	move.l	d0,a0
	moveq	#0,d0
	move.w	Dia_ZoNumber(a0),d0
	move.l	d0,-(a3)
	addq.w	#1,d7
	rts

; =tableau pos ARray Retourne le contenu d'une variable dans une tableau
; ~~~~~~~~~~~~~~~~~~~
Dia_FArray
	cmp.w	#2,d7
	Rblt	L_Dia_NPar
	subq.w	#1,d7
	move.l	(a3)+,d1
	move.l	(a3),a0
	cmp.l	#1*1024,a0		*** Illegal
	Rble	L_Dia_Fonc
	cmp.b	#1,(a0)+		Nombre de dimensions
	Rbne	L_Dia_Fonc
	cmp.b	#2,(a0)+		Taille variable=4!
	Rbne	L_Dia_Fonc
	moveq	#0,d0
	move.w	(a0),d0
	cmp.l	d0,d1
	Rbcc	L_Dia_Fonc
	lsl.l	#2,d1
	lea	4(a0,d1.l),a0
	move.l	(a0),(a3)
	rts
; =tableau ArraySize Retourne la taille d'un tableau
; ~~~~~~~~~~~~~~~~~~
Dia_FASize
	cmp.w	#1,d7
	Rblt	L_Dia_NPar
	move.l	(a3),d0
	beq.s	.Nul
	move.l	d0,a0
	cmp.l	#1*1024,a0		*** Illegal
	Rble	L_Dia_Fonc
	cmp.b	#1,(a0)+		Nombre de dimensions
	Rbne	L_Dia_Fonc
	cmp.b	#2,(a0)+		Taille variable=4!
	Rbne	L_Dia_Fonc
	moveq	#0,d0
	move.w	(a0),d0
.Nul	move.l	d0,(a3)
	rts

; =Px
; ~~~
Dia_P1	moveq	#1,d0
	bra.s	Dia_P
Dia_P2	moveq	#2,d0
	bra.s	Dia_P
Dia_P3	moveq	#3,d0
	bra.s	Dia_P
Dia_P4	moveq	#4,d0
	bra.s	Dia_P
Dia_P5	moveq	#5,d0
	bra.s	Dia_P
Dia_P6	moveq	#6,d0
	bra.s	Dia_P
Dia_P7	moveq	#7,d0
	bra.s	Dia_P
Dia_P8	moveq	#8,d0
	bra.s	Dia_P
Dia_P9	moveq	#9,d0
Dia_P	move.l	Dia_PUsers(a4),d1
	Rbeq	L_Dia_Synt
	move.l	d1,a0
	move.w	Dia_NPUsers(a4),d1
	sub.w	d0,d1
	Rbcs	L_Dia_Synt
	lsl.w	#2,d1
	move.l	0(a0,d1.w),-(a3)
	addq.w	#1,d7
	rts

; X Size of display
; ~~~~~~~~~~~~~~~~~
Dia_FSw
	move.l	T_EcCourant(a5),a0
	moveq	#0,d0
	move.w	EcTx(a0),d0
	move.l	d0,-(a3)
	addq.w	#1,d7
	rts
; Y Size of display
; ~~~~~~~~~~~~~~~~~
Dia_FSh
	move.l	T_EcCourant(a5),a0
	moveq	#0,d0
	move.w	EcTy(a0),d0
	move.l	d0,-(a3)
	addq.w	#1,d7
	rts
; Operateur PLUS
; ~~~~~~~~~~~~~~
Dia_FPlus
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	add.l	d0,(a3)
	subq.w	#1,d7
	rts
; Operateur MOINS
; ~~~~~~~~~~~~~~~
Dia_FMoins
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	sub.l	d0,(a3)
	subq.w	#1,d7
	rts
; Operateur MULTIPLIE
; ~~~~~~~~~~~~~~~~~~~
Dia_FMul
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	move.l	(a3),d1
	muls	d0,d1
	move.l	d1,(a3)
	subq.w	#1,d7
	rts
; Operateur DIVISE
; ~~~~~~~~~~~~~~~~
Dia_FDiv
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	Rbeq	L_Dia_Synt
	move.l	(a3),d1
	ext.l	d1
	divs	d0,d1
	ext.l	d1
	move.l	d1,(a3)
	subq.w	#1,d7
	rts
; Operateur =
; ~~~~~~~~~~~
Dia_FEq
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	cmp.l	(a3),d0
	beq.s	Dia_Tr
	bne.s	Dia_Fa
; Operateur \
; ~~~~~~~~~~~
Dia_FNEq
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	cmp.l	(a3),d0
	bne.s	Dia_Tr
	beq.s	Dia_Fa
; Operateur <
; ~~~~~~~~~~~
Dia_FInf
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	move.l	(a3),d1
	cmp.l	d0,d1
	blt.s	Dia_Tr
	bra.s	Dia_Fa
; Operateur >
; ~~~~~~~~~~~
Dia_FSup
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	move.l	(a3),d1
	cmp.l	d0,d1
	bgt.s	Dia_Tr
; Faux / Vrai
; ~~~~~~~~~~~
Dia_Fa	clr.l	(a3)
	subq.w	#1,d7
	rts
Dia_Tr	move.l	#-1,(a3)
	subq.w	#1,d7
	rts
; Operateur &<
; ~~~~~~~~~~~
Dia_FAnd
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	and.l	d0,(a3)
	subq.w	#1,d7
	rts
; Operateur |
; ~~~~~~~~~~~
Dia_FOr
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	(a3)+,d0
	or.l	d0,(a3)
	subq.w	#1,d7
	rts
; Negatif
; ~~~~~~~
Dia_FNeg
	tst.w	d7
	Rbeq	L_Dia_Synt
	neg.l	(a3)
	rts
; Taille en X
; ~~~~~~~~~~~
Dia_FSx	move.l	Dia_Sx(a4),-(a3)
	addq.w	#1,d7
	rts
; Taille en Y
; ~~~~~~~~~~~
Dia_FSy	move.l	Dia_Sy(a4),-(a3)
	addq.w	#1,d7
	rts
; Positions apres fonctions X et Y
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_FXA	move.w	Dia_XA(a4),d0
	bra.s	Dia_FX
Dia_FYA	move.w	Dia_YA(a4),d0
	bra.s	Dia_FX
Dia_FXB	move.w	Dia_XB(a4),d0
	bra.s	Dia_FX
Dia_FYB	move.w	Dia_YB(a4),d0
Dia_FX	ext.l	d0
	move.l	d0,-(a3)
	addq.w	#1,d7
	rts
; Base en X
; ~~~~~~~~~
Dia_FBx	move.l	Dia_BaseX(a4),-(a3)
	addq.w	#1,d7
	rts
; Base en Y
; ~~~~~~~~~
Dia_FBy	move.l	Dia_BaseY(a4),-(a3)
	addq.w	#1,d7
	rts
; Message normal
; ~~~~~~~~~~~~~~
Dia_FMess
	tst.w	d7
	Rbeq	L_Dia_Synt
	move.l	Dia_Messages(a4),a0
	move.l	(a3),d0
	Rble	L_Dia_Fonc
	moveq	#0,d1
	bra.s	.MSkip
.MLoop	move.b	1(a0),d1
	Rbmi	L_Dia_Fonc
	lea	2(a0,d1.w),a0
.MSkip	subq.l	#1,d0
	bne.s	.MLoop
	move.l	a0,(a3)
	rts

; Largeur d'un texte
; ~~~~~~~~~~~~~~~~~~
Dia_FTw
	tst.w	d7
	Rbeq	L_Dia_Synt
	move.l	(a3),a0
	tst.b	(a0)+
	moveq	#0,d0
	move.b	(a0)+,d0
	beq.s	.Skip
	move.l	T_RastPort(a5),a1
	move.l	a6,-(sp)
	move.l	T_GfxBase(a5),a6
	jsr	TextLength(a6)
	move.l	(sp)+,a6
	ext.l	d0
.Skip	move.l	d0,(a3)
	rts
; Centrage d'un texte
; ~~~~~~~~~~~~~~~~~~~
Dia_FCx
	bsr	Dia_FTw
	move.l	(a3),d0
	move.l	Dia_Sx(a4),d1
	sub.l	d0,d1
	bpl.s	.Skip
	moveq	#0,d1
.Skip	lsr.l	#1,d1
	move.l	d1,(a3)
	rts
; Nombre de caracteres
; ~~~~~~~~~~~~~~~~~~~~
Dia_FLTxt
	cmp.w	#1,d7
	Rblt	L_Dia_Synt
	move.l	(a3),a0
	moveq	#0,d0
	tst.b	(a0)+
	move.b	(a0)+,d0
	move.l	d0,(a3)
	rts
; Hauteur d'un texte
; ~~~~~~~~~~~~~~~~~~
Dia_FTh
	move.l	T_RastPort(a5),a1
	move.w	58(a1),d0
	ext.l	d0
	move.l	d0,-(a3)
	addq.w	#1,d7
	rts

; Copie d'une chaine terminee par zero ADR LONG MZ
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dia_FStZero
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	move.l	a2,-(sp)
	Rbsr	L_Dia_StDebut
	subq.w	#1,d7
	move.l	(a3)+,d1	Longueur max
	move.l	(a3),a2
.Loop	move.b	(a2)+,d0
	cmp.b	#32,d0
	bcs.s	.Exit
	move.b	d0,(a0)+
	subq.w	#1,d1
	bgt.s	.Loop
.Exit	move.l	a1,(a3)
	move.l	(sp)+,a2
	Rbra	L_Dia_StFini
; Addition de chaines CH1 CH2 $
; ~~~~~~~~~~~~~~~~~~~
Dia_FPlusCh
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	Rbsr	L_Dia_StDebut
	subq.w	#1,d7
; Premiere chaine
	move.l	a2,-(sp)
	move.l	4(a3),a2
	moveq	#0,d0
	tst.b	(a2)+
	move.b	(a2)+,d0
	subq.w	#1,d0
	bmi.s	.Sk1
.Lp1	move.b	(a2)+,(a0)+
	dbra	d0,.Lp1
.Sk1
; Deuxieme chaine
	move.l	(a3)+,a2
	moveq	#0,d0
	tst.b	(a2)+
	move.b	(a2)+,d0
	subq.w	#1,d0
	bmi.s	.Sk2
.Lp2	move.b	(a2)+,(a0)+
	dbra	d0,.Lp2
.Sk2
; Stocke le r�sultat
	move.l	a1,(a3)
	move.l	(sp)+,a2
	Rbra	L_Dia_StFini
; Variable en STRING
; ~~~~~~~~~~~~~~~~~~
Dia_FString1
	moveq	#'"',d1
	bra.s	Dia_FStr
Dia_FString2
	moveq	#"'",d1
Dia_FStr
	Rbsr	L_Dia_StDebut
.Loop	move.b	(a6)+,d0
	cmp.b	#" ",d0
	bcs.s	.Fin
	cmp.b	d0,d1
	beq.s	.Fini
	move.b	d0,(a0)+
	bra.s	.Loop
.Fin	subq.l	#1,a6
.Fini	move.l	a1,-(a3)
	addq.w	#1,d7
	Rbra	L_Dia_StFini
; Fonction MIN
; ~~~~~~~~~~~~
Dia_FMin
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	subq.w	#1,d7
	move.l	(a3)+,d0
	cmp.l	(a3),d0
	bge.s	.Skip
	move.l	d0,(a3)
.Skip	rts
; Fonction MAX
; ~~~~~~~~~~~~
Dia_FMax
	cmp.w	#2,d7
	Rblt	L_Dia_Synt
	subq.w	#1,d7
	move.l	(a3)+,d0
	cmp.l	(a3),d0
	ble.s	.Skip
	move.l	d0,(a3)
.Skip	rts

; Button Position, prise dans VAR-1
; Zone Position, prise dans VAR-1
; ~~~~~~~~~~~~~
Dia_FZP
Dia_FBP	move.l	Dia_Vars-1*4(a4),-(a3)
	addq.w	#1,d7
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 				ROUTINES GESTIONS CHAINE / CONVERSIONS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FONCTION: Conversion en d�cimal CHIF #
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_FDecimal
; - - - - - - - - - - - - -
	cmp.w	#1,d7
	Rblt	L_Dia_Synt
	Rbsr	L_Dia_StDebut
	move.l	(a3),d0
	move.l	a1,(a3)
	Rbsr	L_Dia_DecToAsc
	Rbra	L_Dia_StFini

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Fixe le debut d'une chaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_StDebut
; - - - - - - - - - - - - -
	move.l	Dia_ABuffer(a4),a0
	clr.w	(a0)+			Longueur
	move.w	#Dia_StMark,(a0)+	Id
	move.l	a0,a1
	clr.w	(a0)+
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Calcule le buffer pour une chaine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_StFini
; - - - - - - - - - - - - -
	move.l	a0,d0
	move.l	a0,d1
	sub.l	a1,d1
	subq.b	#2,d1
	move.b	d1,1(a1)		Longueur de la chaine
	addq.l	#1,d0
	and.w	#$FFFE,d0
	move.l	d0,Dia_ABuffer(a4)	Position du buffer
	subq.l	#4,a1			Pointe le header
	move.l	d0,a0
	clr.l	(a0)			Plus rien!
	sub.l	a1,a0
	move.w	a0,(a1)			Longueur de la chaine
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES GESTIONS ZONE D'EDITION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la fenetre de dialogue A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EdEnd
; - - - - - - - - - - - - -
	Rbsr	L_Dia_EdOn
	Rbsr	L_Dia_WiZero
	WiCall	WinDel
	move.w	#-1,Dia_WindOn(a4)
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Active la ligne de dialogue A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EdActive
; - - - - - - - - - - - - -
	move.l	a0,-(sp)
	moveq	#0,d1
	move.w	Dia_ZoNumber(a0),d1
	add.w	#1000,d1
	Rbsr	L_Dia_WActive
	move.l	(sp)+,a0
	lea	Dia_LEd(a0),a0
	Rbra	L_LEd_CuMarche
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Fenetre dialogue A0 en route
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EdOn
; - - - - - - - - - - - - -
	movem.l	a0/a1/d0/d1,-(sp)
	moveq	#0,d1
	move.w	Dia_ZoNumber(a0),d1
	add.w	#1000,d1
	Rbsr	L_Dia_WActive
	movem.l	(sp)+,a0/a1/d0/d1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Inactive la fenetre A0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EdInactive
; - - - - - - - - - - - - -
	lea	Dia_LEd(a0),a0
	Rbra	L_LEd_CuStoppe
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Active s'il faut, la fenetre D1
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_WActive
; - - - - - - - - - - - - -
	cmp.w	Dia_WindOn(a4),d1
	beq.s	.Skip
	move.w	d1,Dia_WindOn(a4)
	WiCall	QWindow
.Skip	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Passe paper/pen � zero pour effacement
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_WiZero
; - - - - - - - - - - - - -
	lea	.Zero(pc),a1
	WiCall	Print
	rts
.Zero	dc.b	27,"J0",0
	even



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES DE GESTION DE L'HYPERTEXTE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Activation de la fenetre texte, A2=structure
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_TxActive
; - - - - - - - - - - - - -
	movem.l	a0-a1/d0-d1,-(sp)
	moveq	#0,d1
	move.w	Dia_ZoNumber(a2),d1
	add.w	#3000,d1
	Rbsr	L_Dia_WActive
	movem.l	(sp)+,a0-a1/d0-d1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la fenetre de texte A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_TxEnd
; - - - - - - - - - - - - -
	Rbsr	L_Dia_TxActive
	Rbsr	L_Dia_WiZero
	WiCall	WinDel
	move.w	#-1,Dia_WindOn(a4)
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affiche la page de texte
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_TxDraw
; - - - - - - - - - - - - -
	movem.l	d2/d3,-(sp)
; Boucle d'affichage
	move.w	Dia_TxPos(a2),d2
	move.w	Dia_TxTy(a2),d3
.ALoop	move.w	d2,d0
	Rbsr	L_Dia_TxAff
	addq.w	#1,d2
	subq.w	#1,d3
	bne.s	.ALoop
; Plus d'actif!
	clr.l	Dia_TxAct(a2)
	movem.l	(sp)+,d2/d3
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affiche la ligne #D0
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_TxAff
; - - - - - - - - - - - - -
	movem.l	d0-d7/a0-a6,-(sp)
	clr.w	-(sp)
; Locate
	moveq	#0,d3
	move.w	d0,d3
	moveq	#0,d1
	move.w	d0,d2
	sub.w	Dia_TxPos(a2),d2
	bmi	.Vide
	cmp.w	Dia_TxTy(a2),d2
	bcc	.Vide
	move.w	d2,d0
	mulu	Dia_TxDispSize(a2),d0
	move.l	Dia_TxDisplay(a2),a3
	add.w	d0,a3				Adresse des pointeurs
	clr.l	(a3)				Par defaut, rien!
	clr.w	4(a3)
	WiCall	Locate
; Paper / Ink + Effacement ligne
	lea	Dia_TxPp(a2),a1
	WiCall	Print
	WiCalD	ChrOut,26
; Trouve l'adresse de la ligne
	cmp.w	Dia_TxNLine(a2),d3
	bcc	.Vide
	move.w	d3,d0
	lsl.w	#2,d0
	move.l	Dia_TxAdress(a2),a0
	move.l	0(a0,d0.w),d6
	lsr.l	#8,d6
	add.l	Dia_TxText(a2),d6
	move.l	d6,a6				Adresse de la ligne!
	move.l	a6,(a3)+
	moveq	#0,d1
	move.b	3(a0,d0.w),d1			Longueur de la ligne
	lea	0(a6,d1.w),a0
	move.l	a0,d7				Fin de la ligne
	move.b	(a0),d5				Marque la fin
	clr.b	(a0)
; Affiche les segments de la ligne
.RLoop	move.l	a6,a1
.CLoop	move.b	(a1)+,d0
	beq.s	.CPrn
	cmp.b	#"{",d0
	bne.s	.CLoop
.CPrn	subq.l	#1,a1
; Imprime le segment, normal
	exg	a6,a1
	move.b	(a6),d4
	clr.b	(a6)
	moveq	#0,d1			Impression limitee
	moveq	#0,d2			Depuis le debut
	move.w	Dia_TxTx(a2),d3		Taille fenetre -1
	WiCall	Print3
	cmp.w	d3,d1
	bcc	.Ouf			Si droite atteinte
; Un segment actif?
	move.b	d4,(a6)
	cmp.b	#"{",d4
	bne	.Ouf
; Affichage d'un segment actif
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	a6,a1
	move.w	(sp),d0
	cmp.w	Dia_TxDispMax(a2),d0
	bcc	.AErr
	cmp.b	#"[",1(a1)
	bne	.AErr
	addq.l	#2,a1
; Saute la partie reponse
	move.b	Dia_TxPen(a2),4(a3)		Pen=paper
	move.b	Dia_TxPaper(a2),5(a3)		Paper=pen
	WiCall	XYCuWi
	move.b	d1,2(a3)			Position dans ecran
	move.l	a1,d0
	sub.l	d6,d0
	move.b	d0,6(a3)			Debut mot cl� dans texte
	bset	#31,d4
	cmp.b	#",",(a1)			Si pas de mot cl�
	bne.s	.ALoop
	bclr	#31,d4
.ALoop	move.b	(a1)+,d0
	beq	.AErr
	cmp.b	#",",d0
	beq.s	.Pp
	cmp.b	#"]",d0
	bne.s	.ALoop
	beq.s	.AFin
; Retrouve paper et pen
.Pp	bsr	.PaPe
	move.b	d1,4(a3)
	cmp.b	#",",d0
	bne	.AErr
	bsr	.PaPe
	move.b	d1,5(a3)
	cmp.b	#"]",d0
	bne	.AErr
; Cherche la fin de la parenthese }
.AFin	move.l	a1,a6
.FLoop	move.b	(a1)+,d0
	beq.s	.FFin
	cmp.b	#"}",d0
	bne.s	.FLoop
	subq.l	#1,a1
.FFin	move.l	a6,d0
	sub.l	d6,d0
	move.b	d0,(a3)				Debut dans le texte
	move.l	a1,d0
	sub.l	d6,d0
	move.b	d0,1(a3)			Fin dans le texte
; Change les couleurs
	move.l	a1,-(sp)
	lea	Dia_HtActive1(pc),a1
	move.b	4(a3),2(a1)
	move.b	5(a3),5(a1)
	WiCall	Print
	move.l	(sp)+,a1
; Imprime le texte actif
	exg.l	a1,a6
	move.b	(a6),d4
	clr.b	(a6)
	moveq	#0,d1				Impression limitee
	moveq	#0,d2
	move.w	Dia_TxTx(a2),d3
	WiCall	Print3
	move.b	d4,(a6)
	addq.w	#1,(sp)
	move.b	d1,3(a3)			Position dans ecran
	clr.b	7(a3)
	btst	#31,d4				Inactif?
	bne.s	.PaIna
	move.b	#1,2(a3)
	clr.b	3(a3)
.PaIna	lea	Dia_TxDispZone(a3),a3
	clr.w	(a3)
; Rebranche � la normale
	cmp.w	Dia_TxTx(a2),d1
	bcc	.Ouf
	cmp.b	#"}",d4
	bne.s	.Ouf
	addq.l	#1,a6
	lea	Dia_TxPp(a2),a1
	WiCall	Print
	bra	.RLoop
; Erreur dans la definition
.AErr	clr.l	(a3)
	clr.w	4(a3)
	lea	1(a6),a1
	bra	.CLoop
; Ouf, c'est fini!
; ~~~~~~~~~~~~~~~~
.Ouf	move.l	d7,a0
	move.b	d5,(a0)
.Vide	addq.l	#2,sp
	movem.l	(sp)+,d0-d7/a0-a6
	rts
; Petite saisie de chiffre decimal
.PaPe	moveq	#0,d2
	moveq	#0,d1
.PLoop	move.b	(a1)+,d0
	move.b	d0,d2
	sub.b	#"0",d2
	bcs.s	.POut
	cmp.b	#9,d2
	bhi.s	.POut
	mulu	#10,d1
	add.l	d2,d1
	bra.s	.PLoop
.POut	add.b	#"0",d1
	rts
Dia_HtActive1
	dc.b	27,"B0",27,"P0",0
	even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affiche le mot actif A0(pointeurs) D0=Y
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_TxAffActive
; - - - - - - - - - - - - -
	movem.l	a2-a4/d2-d4,-(sp)
	move.l	a0,a3
	moveq	#0,d2
	move.w	d0,d2
	mulu	Dia_TxDispSize(a2),d0
	move.l	Dia_TxDisplay(a2),a4
	move.l	0(a4,d0.w),a4
; Locate
	moveq	#0,d1
	move.b	2(a3),d1
	WiCall	Locate
	bne.s	.Out
; Change les couleurs
	lea	Dia_HtActive(pc),a1
	move.b	4(a3),2(a1)
	move.b	5(a3),5(a1)
	cmp.l	Dia_TxAct(a2),a3
	bne.s	.Pa
	move.b	5(a3),2(a1)
	move.b	4(a3),5(a1)
.Pa	WiCall	Print
; Imprime le texte actif
	moveq	#0,d0
	move.b	(a3)+,d0
	lea	0(a4,d0.w),a1
	move.b	(a3)+,d0
	lea	0(a4,d0.w),a4
	move.b	(a4),d4
	clr.b	(a4)
	moveq	#0,d1				Impression limitee
	moveq	#0,d2
	move.w	Dia_TxTx(a2),d3
	WiCall	Print3
	move.b	d4,(a4)
; Ca y est!
.Out	movem.l	(sp)+,a2-a4/d2-d4
	rts
Dia_HtActive
	dc.b	27,"B0",27,"P0",0
	even


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES GESTION LISTES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Activation de la fenetre liste A2=liste
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_LiActive
; - - - - - - - - - - - - -
	movem.l	a0-a1/d0-d1,-(sp)
	moveq	#0,d1
	move.w	Dia_ZoNumber(a2),d1
	add.w	#2000,d1
	Rbsr	L_Dia_WActive
	movem.l	(sp)+,a0-a1/d0-d1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Efface la fenetre de liste A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_LiEnd
; - - - - - - - - - - - - -
	Rbsr	L_Dia_LiActive
	Rbsr	L_Dia_WiZero
	WiCall	WinDel
	move.w	#-1,Dia_WindOn(a4)
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affichage du contenu d'une liste - A2=liste
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_LiDraw
; - - - - - - - - - - - - -
	movem.l	d2/d3,-(sp)
; Boucle d'affichage
	move.w	Dia_LiPos(a2),d2
	move.w	Dia_LiTy(a2),d3
.ALoop	move.w	d2,d0
	Rbsr	L_Dia_LiAff
	addq.w	#1,d2
	subq.w	#1,d3
	bne.s	.ALoop
; Ok!
	movem.l	(sp)+,d2/d3
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Affichage d'un des �l�ments de la liste - A2 structure - D0=#
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_LiAff
; - - - - - - - - - - - - -
	movem.l	d0-d6/a0-a1,-(sp)
; Locate
	moveq	#0,d4
	move.w	d0,d4
	moveq	#0,d1
	move.w	d0,d2
	sub.w	Dia_LiPos(a2),d2
	bmi	.Vide
	cmp.w	Dia_LiTy(a2),d2
	bcc	.Vide
	WiCall	Locate
; Paper / Ink + Effacement ligne
	lea	.Inv0(pc),a1
	cmp.w	Dia_LiActNumber(a2),d4
	bne.s	.Ina
	lea	.Inv1(pc),a1
.Ina	WiCall	Print
; Trop loin?
	move.l	Dia_LiArray(a2),d0
	beq	.Vide
	move.l	d0,a0
	cmp.w	2(a0),d4
	bcc	.Vide
; Imprime le numero, eventuellement
	moveq	#0,d6
	btst	#0,Dia_ZoFlags+1(a2)
	beq.s	.NoNum
	move.l	d4,d0
	btst	#1,Dia_ZoFlags+1(a2)
	beq.s	.Nm1
	addq.l	#1,d0
.Nm1	move.l	Dia_ABuffer(a4),a0
	addq.l	#4,a0
	move.l	a0,d2
	Rbsr	L_Dia_DecToAsc
	sub.l	a0,d2
	move.b	#" ",(a0)+
	move.b	#"-",(a0)+
	move.b	#" ",(a0)+
	clr.b	(a0)
	move.l	a0,d6
	sub.l	Dia_ABuffer(a4),d6
	subq.l	#2,d6
; Met des espaces au debut
	add.w	Dia_LiLArray(a2),d2
	ble.s	.Nm2
.NmL	WiCalD	ChrOut,32
	subq.w	#1,d2
	bne.s	.NmL
; Imprime le chiffre
.Nm2	move.l	Dia_ABuffer(a4),a1
	addq.l	#4,a1
	WiCall	Print
.NoNum
; Imprime la chaine
	move.l	Dia_LiArray(a2),a0
	moveq	#0,d0
	cmp.w	2(a0),d4
	bcc.s	.Vide
; Un tableau magique?
	cmp.l	#"MgIk",4(a0)
	bne.s	.Norm
	move.l	8(a0),a0
	jsr	(a0)
	bra.s	.Sui
; Un tableau normal
.Norm	lsl.l	#2,d4
	lea	6(a0,d4.l),a0
	move.l	(a0),d0
	cmp.l	#8*1024,d0
	ble	.Vide
	move.l	d0,a1
	move.b	(a1)+,d1
	lsl.w	#8,d1
	move.b	(a1)+,d1
.Sui	move.w	Dia_LiTx(a2),d2
	sub.w	d6,d2
	cmp.w	d2,d1
	ble.s	.Pa
	move.w	d2,d1
.Pa	WiCall	Print2
.Vide	movem.l	(sp)+,d0-d6/a0-a1
	rts
.Inv0	dc.b	27,"I0",26,0
.Inv1	dc.b	27,"I1",26,0


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DE GESTION SLIDERS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Dessine le slider A2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_SlDraw
; - - - - - - - - - - - - -
	move.l	a2,-(sp)
	moveq	#0,d0
	lea	Dia_Sl(a2),a0
	Rbsr	L_Sl_Draw
	move.w	Dia_Writing(a4),d0
	move.l	T_RastPort(a5),a1
	GfxCa5	SetDrMd
	move.l	(sp)+,a2
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINE DESSIN BOUTON
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_BtDraw
; - - - - - - - - - - - - -
	movem.l	a2/a6,-(sp)
	moveq	#0,d0
	move.w	Dia_BtRDraw(a2),d0
	beq	.Skip
; Adresse de la routine
	move.l	Dia_Programs(a4),a6
	add.l	d0,a6
; Sauve les donn�es
	move.l	Dia_BaseX(a4),-(sp)
	move.l	Dia_BaseY(a4),-(sp)
	move.l	Dia_Sx(a4),-(sp)
	move.l	Dia_Sy(a4),-(sp)
	move.w	Dia_XA(a4),-(sp)
	move.w	Dia_YA(a4),-(sp)
	move.w	Dia_XB(a4),-(sp)
	move.w	Dia_YB(a4),-(sp)
	move.l	Dia_Vars-1*4(a4),-(sp)
	move.l	Dia_Vars-2*4(a4),-(sp)
	move.l	Dia_Vars-3*4(a4),-(sp)
	move.l	Dia_Sp(a4),-(sp)
; Change XYSXSY
	moveq	#0,d0
	move.w	Dia_ZoSx(a2),d0
	move.l	d0,Dia_Sx(a4)
	move.w	Dia_ZoSy(a2),d0
	move.l	d0,Dia_Sy(a4)
	move.w	Dia_ZoX(a2),d0
	move.l	d0,Dia_BaseX(a4)
	move.w	Dia_ZoY(a2),d0
	move.l	d0,Dia_BaseY(a4)
; Appelle la routine
	move.l	Dia_ZoPos(a2),Dia_Vars-1*4(a4)
	move.l	a2,Dia_Vars-3*4(a4)
	Rbsr	L_Dia_Loop
; Restore les donn�es
	move.l	(sp)+,Dia_Sp(a4)
	move.l	(sp)+,Dia_Vars-3*4(a4)
	move.l	(sp)+,Dia_Vars-2*4(a4)
	move.l	(sp)+,Dia_Vars-1*4(a4)
	move.w	(sp)+,Dia_YB(a4)
	move.w	(sp)+,Dia_XB(a4)
	move.w	(sp)+,Dia_YA(a4)
	move.w	(sp)+,Dia_XA(a4)
	move.l	(sp)+,Dia_Sy(a4)
	move.l	(sp)+,Dia_Sx(a4)
	move.l	(sp)+,Dia_BaseY(a4)
	move.l	(sp)+,Dia_BaseX(a4)
; Retour
.Skip	movem.l	(sp)+,a2/a6
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ROUTINES DIVERSES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Delimite une routine []
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetRout
; - - - - - - - - - - - - -
	Rbsr	L_Dia_Chr
	cmp.b	#"[",d0
	Rbne	L_Dia_Synt
	move.l	a6,d1
	Rbsr	L_Dia_Chr
	Rbeq	L_Dia_Synt
	cmp.b	#"]",d0
	beq.s	.Zero
	sub.l	Dia_Programs(a4),d1
	Rbsr	L_Dia_EndP
	rts
.Zero	moveq	#0,d1
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve la fin d'une parenthese
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EndP
; - - - - - - - - - - - - -
	move.l	d1,-(sp)
	moveq	#1,d1
.Loop	move.b	(a6)+,d0
	Rbeq	L_Dia_Synt
	cmp.b	#"[",d0
	beq.s	.Plus
	cmp.b	#"]",d0
	bne.s	.Loop
	subq.w	#1,d1
	bne.s	.Loop
	move.l	(sp)+,d1
	rts
.Plus	addq.w	#1,d1
	bra.s	.Loop

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UPDATE LA ZONE D2
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_ZUpdate
; - - - - - - - - - - - - -
	movem.l	a2/d6,-(sp)
	move.l	Dia_Buffer(a4),a2
	move.l	#EntNul,d6
.Loop	cmp.w	#Dia_ZoMark,4(a2)		Une zone?
	bne.s	.Next
	cmp.w	Dia_ZoNumber(a2),d2		La bonne?
	bne.s	.Next
	cmp.l	Dia_Vars-3*4(a4),a2		Pas celui-ci?
	beq.s	.Next
	cmp.w	#Dia_EdMark,2(a2)
	beq	.Ed
	cmp.w	#Dia_BtMark,2(a2)
	beq	.Bt
	cmp.w	#Dia_SlMark,2(a2)
	beq	.Sl
	cmp.w	#Dia_LiMark,2(a2)
	beq	.Li
	cmp.w	#Dia_TxMark,2(a2)
	beq	.Tx
.Next	add.w	(a2),a2
	tst.w	(a2)
	bne.s	.Loop
	Rbsr	L_Dia_Patch
	movem.l	(sp)+,a2/d6
	rts
; Un bouton
; ~~~~~~~~~
.Bt	cmp.l	d3,d6
	beq.s	.BNul
	cmp.l	Dia_ZoPos(a2),d3		Faut-il change le bouton?
	beq.s	.BDeja
	move.l	d3,Dia_ZoPos(a2)		Change le bouton
.BNul	Rbsr	L_Dia_BtDraw			Va le dessiner
	Rbsr	L_Dia_ZoChange			Va le changer
.BDeja	bclr	#7,Dia_ZoFlags(a2)		Pas de sortie!
	bra.s	.Next
; Un slider
; ~~~~~~~~~
.Sl	cmp.l	d5,d6				Update global
	beq.s	.SNul1
	move.w	d5,Dia_Sl+Sl_Global(a2)
.SNul1	cmp.l	d4,d6				Update window
	beq.s	.SNul2
	move.w	d4,Dia_Sl+Sl_Window(a2)
.SNul2	cmp.l	d3,d6				Update position
	beq.s	.SNul3
	move.l	d3,Dia_ZoPos(a2)
	move.w	d3,Dia_Sl+Sl_Position(a2)
.SNul3	Rbsr	L_Dia_SlDraw
	Rbsr	L_Dia_ZoChange
	bclr	#7,Dia_ZoFlags(a2)		Pas de sortie!
	bra	.Next
; Une liste
; ~~~~~~~~~
.Li	cmp.l	d5,d6				Maximum Activable
	beq.s	.LNul1
	move.w	d5,Dia_LiMaxAct(a2)
.LNul1	cmp.l	d4,d6				Active courant
	beq.s	.LNul2
	move.w	Dia_LiActNumber(a2),d0
	bmi.s	.LAct
	move.w	#-1,Dia_LiActNumber(a2)
	Rbsr	L_Dia_LiActive
	Rbsr	L_Dia_LiAff
.LAct	move.w	d4,Dia_LiActNumber(a2)
	move.l	d4,Dia_ZoPos(a2)
.LNul2	cmp.l	d3,d6				Position de la liste
	beq.s	.LNul3
	move.w	d3,Dia_LiPos(a2)
.LNul3	Rbsr	L_Dia_LiActive
	Rbsr	L_Dia_LiDraw
	bclr	#7,Dia_ZoFlags(a2)		Pas de sortie!
	bra	.Next
; Un texte
; ~~~~~~~~
.Tx	cmp.l	d3,d6
	beq	.Next
	move.w	d3,Dia_TxPos(a2)
	Rbsr	L_Dia_TxActive
	Rbsr	L_Dia_TxDraw
	bclr	#7,Dia_ZoFlags(a2)
	bra	.Next
; Une zone de texte
; ~~~~~~~~~~~~~~~~~
.Ed	cmp.l	d3,d6
	beq	.Next
	cmp.w	#Dia_EdLong,(a2)
	bne.s	.Di
	cmp.l	#1*1024,d3			Illegal: type mismatch
	Rble	L_Dia_Fonc
	move.l	d3,a1
	bra.s	.EdCop
; Un chiffre?
; ~~~~~~~~~~~
.Di	move.l	d3,-(a3)
	moveq	#1,d7
	Rbsr	L_Dia_FDecimal
	move.l	(a3)+,a1
; Recopie le texte dans le buffer
.EdCop	move.l	a2,a0
	Rbsr	L_Dia_EdOn
	lea	Dia_LEd(a2),a0
	moveq	#0,d0
	tst.b	(a1)+
	move.b	(a1)+,d0
	Rbsr	L_LEd_New
	bra	.Next


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Routine CHANGE zone active (bouton / liste active / slider)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_ZoChange
; - - - - - - - - - - - - -
	move.l	a6,-(sp)
	move.l	Dia_ZoPos(a2),d0
	moveq	#0,d1
	move.w	Dia_ZoRChange(a2),d1
	beq	.Skip
; Sauve les donn�es
	move.l	Dia_BaseX(a4),-(sp)
	move.l	Dia_BaseY(a4),-(sp)
	move.l	Dia_Sx(a4),-(sp)
	move.l	Dia_Sy(a4),-(sp)
	move.w	Dia_XA(a4),-(sp)
	move.w	Dia_YA(a4),-(sp)
	move.w	Dia_XB(a4),-(sp)
	move.w	Dia_YB(a4),-(sp)
	move.l	Dia_Vars-1*4(a4),-(sp)
	move.l	Dia_Vars-2*4(a4),-(sp)
	move.l	Dia_Vars-3*4(a4),-(sp)
	move.l	Dia_Sp(a4),-(sp)
; Prepare les donn�es
	move.l	d0,Dia_Vars-1*4(a4)		Position du bouton
	move.l	d0,Dia_Vars-2*4(a4)
	move.l	a2,Dia_Vars-3*4(a4)		Adresse de la structure
	bclr	#7,Dia_ZoFlags(a2)		Pas de QUIT
	bclr	#5,Dia_ZoFlags(a2)		Pas de IMMEDIAT
; Appelle la routine
	move.l	Dia_Programs(a4),a6
	add.l	d1,a6
	Rbsr	L_Dia_Loop
; Retour de parametres
	move.l	Dia_Vars-2*4(a4),d0
; Restaure les donn�es
	move.l	(sp)+,Dia_Sp(a4)
	move.l	(sp)+,Dia_Vars-3*4(a4)
	move.l	(sp)+,Dia_Vars-2*4(a4)
	move.l	(sp)+,Dia_Vars-1*4(a4)
	move.w	(sp)+,Dia_YB(a4)
	move.w	(sp)+,Dia_XB(a4)
	move.w	(sp)+,Dia_YA(a4)
	move.w	(sp)+,Dia_XA(a4)
	move.l	(sp)+,Dia_Sy(a4)
	move.l	(sp)+,Dia_Sx(a4)
	move.l	(sp)+,Dia_BaseY(a4)
	move.l	(sp)+,Dia_BaseX(a4)
; Retour, D0= position de retour
.Skip	move.l	(sp)+,a6
	rts

;
; 	Trouve l'adresse du dialogue numero D0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	OUT	BEQ Pas trouve, BNE Trouve, D0=Flags / A1=Adresse
; - - - - - - - - - - - - -
	Lib_Def	Dia_GetChannel
; - - - - - - - - - - - - -
	move.l	Cur_Dialogs(a5),a0
	move.l	(a0),d1
	beq.s	.Nof
.Loop	move.l	d1,a1
	cmp.l	8(a1),d0
	beq.s	.Fnd
	move.l	(a1),d1
	bne.s	.Loop
.Nof	sub.l	a1,a1
	move.l	a1,a0
	rts
.Fnd	lea	8(a1),a0
	move.l	a0,a1
	move.w	#%00000,CCR
	rts


; ________________________________________________________________________
;
;	ROUTINES D'ANIMATION DE L'ARBRE
;
; Exploration de l'arbre par le test
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	Dia_AutoTest
; - - - - - - - - - - - - -
	moveq	#127,d0				Teste jusqu'� 127 boites
	Rbra	L_Dia_AutoTest2
; - - - - - - - - - - - - -
	Lib_Def	Dia_AutoTest2
; - - - - - - - - - - - - -
	movem.l	a2-a6/d2-d7,-(sp)
	move.l	d0,d7
	subq.w	#1,d7
; Explore les differentes boites
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Tests	move.l	Cur_Dialogs(a5),a4
	move.l	(a4),d0
	beq.s	.Out
.Loop	move.l	d0,a4
	lea	8(a4),a4
	btst	#0,Dia_RFlags(a4)		Canal effac�?
	beq.s	.Next
	btst	#2,Dia_RFlags(a4)		Canal FREEZE?
	bne.s	.Next
	Rbsr	L_Dia_Active			Active l'�cran
	bne.s	.Err
	move.l	Dia_Edited(a4),d0		Active la fenetre courante
	ble.s	.PaWi
	move.l	d0,a0
	Rbsr	L_Dia_EdActive
.PaWi	move.l	Dia_Pile(a4),a3
	Rbsr	L_Dia_Tests			Fait les tests
	beq.s	.Act
	move.l	a4,a0
	Rbsr	L_Dia_EffChanA0			Effacement du canal!
.Act	Rbsr	L_Dia_ReActive			Remet l'ancien ecran
	move.w	Dia_Error(a4),d0		Une erreur?
	bne.s	.Err
.Next	move.l	-8(a4),d0
	beq.s	.Out
	dbra	d7,.Loop
; Termin�!
; ~~~~~~~~
.Out	moveq	#0,d0				Pas d'erreur
.Out2	movem.l	(sp)+,a2-a6/d2-d7
	rts
; Une erreur: met le canal en FREEZE, pour eviter la boucle
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Err	bset	#2,Dia_RFlags(a4)
	bra.s	.Out2


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					UN TOUR DE L'ARBRE D'OBJETS
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_Tests
; - - - - - - - - - - - - -
	movem.l	a2-a3/d2-d7,-(sp)
; Flags a zero!
; ~~~~~~~~~~~~~
	clr.w	Dia_Exit(a4)
; Un bouton instantann�
; ~~~~~~~~~~~~~~~~~~~~~
	move.l	Dia_Release(a4),d0
	beq.s	.Pare
	move.l	d0,a2
	move.w	Dia_ZoNumber(a2),Dia_Return(a4)
	SyCall	MouseKey
	and.w	#$01,d1
	beq.s	.Re
	Rbsr	L_Dia_ZoChange
	bclr	#7,Dia_Flags(a2)
	beq	.TFini
	addq.w	#1,Dia_Exit(a4)
	bra	.TFini
.Re	clr.w	Dia_Return(a4)
	clr.l	Dia_Release(a4)
	Rbsr	L_Dia_BtDraw
	Rbsr	L_Dia_Patch
.Pare
; Une �dition?
; ~~~~~~~~~~~~~
	move.l	Dia_Edited(a4),d0
	beq.s	.NoEd
	cmp.l	#-1,d0
	beq.s	.NoEd
	bsr	.Timer
	move.l	d0,a2
	move.l	a2,a0
	Rbsr	L_Dia_EdActive
	lea	Dia_LEd(a2),a0
	Rbsr	L_LEd_Loop
	cmp.w	#1,d2			Return
	bne.s	.PaRet
	move.l	Dia_Edited(a4),a0	Renvoie la zone
	move.w	Dia_ZoNumber(a0),Dia_Return(a4)
	btst	#4,Dia_Flags(a4)	Passer avec RETURN?
	beq.s	.EdNxt
	bra	.Mouse
.PaRet	tst.l	d1			Une touche?
	beq	.Mouse
	Rbsr	L_Dia_Patch
	move.l	d1,d0
	swap	d0
	cmp.b	#$42,d0			Tab?
	bne.s	.EdK
; Trouve la prochaine fenetre � �diter
.EdNxt	move.l	a2,a0
	Rbsr	L_Dia_EdInactive
	move.l	Dia_Edited(a4),a0
	move.l	#-1,Dia_Edited(a4)
	Rbsr	L_Dia_EdNext
	Rbsr	L_Dia_ClearKey
	Rbsr	L_Dia_Patch
	bra	.Mouse
; Une touche?
; ~~~~~~~~~~~
.NoEd	move.l	T_ClLast(a5),d1
.EdK	move.l	d1,Dia_LastKey(a4)
	beq	.Mouse
	bsr 	.Timer
	btst	#2,Dia_Flags(a4)	Sortie forcee?
	beq.s	.PaEx
	addq.w	#1,Dia_Exit(a4)
.PaEx	move.b	d1,d2
	swap	d1
	cmp.b	#"a",d2			Garde la majuscule
	blt.s	.Mj
	cmp.b	#"z",d2
	bgt.s	.Mj
	sub.b	#$20,d2
.Mj	move.l	Dia_Buffer(a4),a2
; Boucle d'exploration
.KLoop	tst.w	(a2)
	beq	.ReEd
	cmp.w	#Dia_KyMark,2(a2)	Une definition de touche?
	bne	.KNext
	move.b	Dia_KyCode(a2),d0	Prend le code
	bmi.s	.KScan
	beq	.KNext
	cmp.b	d0,d2			ASCII egal?
	bne	.KNext
	bra.s	.KShf
.KScan	cmp.b	#-1,d0			-1>>> sortie directe
	beq	.KShf
	and.b	#$7f,d0
	cmp.b	d1,d0
	bne	.KNext
.KShf	ror.w	#8,d1			Les shifts
	move.b	Dia_KyShift(a2),d0
	move.b	d0,d2			Shift?
	and.b	#Shf,d2
	sne	d2
	move.b	d1,d3
	and.b	#Shf,d3
	sne	d3
	cmp.b	d2,d3
	bne.s	.KNextS
	move.b	d0,d2			Amiga?
	and.b	#Ami,d2
	sne	d2
	move.b	d1,d3
	and.b	#Ami,d3
	sne	d3
	cmp.b	d2,d3
	bne.s	.KNextS
	move.b	d0,d2			Control
	and.b	#Ctr,d2
	sne	d2
	move.b	d1,d3
	and.b	#Ctr,d3
	sne	d3
	cmp.b	d2,d3
	bne.s	.KNextS
	move.b	d0,d2			Alternate
	and.b	#Alt,d2
	sne	d2
	move.b	d1,d3
	and.b	#Alt,d3
	sne	d3
	cmp.b	d2,d3
	bne.s	.KNextS
.Pa4	move.l	Dia_KyZone(a2),d0
	beq.s	.Mouse
	Rbsr	L_Dia_ClearKey		Nettoyage du buffer
	moveq	#1,d4			Simule l'appui
	move.l	d0,d5			Pointe l'objet � activer
	moveq	#0,d6
	moveq	#0,d7
	bra	.MousIn
; Passe au suivant
.KNextS	ror.w	#8,d1
.KNext	add.w	(a2),a2
	bra	.KLoop
; Aucune touche active, TAB reactive une fenetre EDITt
.ReEd	cmp.l	#-1,Dia_Edited(a4)
	bne	.Mouse
	move.w	Dia_LastKey(a4),d0
	cmp.b	#$42,Dia_LastKey+1(a4)
	bne.s	.Mouse
.ReA	Rbsr	L_Dia_EdFirst
	Rbsr	L_Dia_ClearKey
	Rbsr	L_Dia_Patch
	bra	.TFini

; La souris?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Mouse
; Gestion des listes actives.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
	SyCall	MouseKey
	move.w	d1,d4
	and.w	#$01,d4
	Rbsr	L_Dia_GetMouseZone		Une zone?
	move.l	d0,d5
	move.w	d1,d6
	move.w	d2,d7
; Explore toute la liste
.MousIn	move.l	Dia_Buffer(a4),a2
.MLoop	cmp.w	#Dia_ZoMark,4(a2)
	bne.s	.MNext
	move.w	2(a2),d0
	cmp.w	#Dia_BtMark,d0
	beq	.MBt
	cmp.w	#Dia_EdMark,d0
	beq	.MEd
	cmp.w	#Dia_SlMark,d0
	beq	.MSl
	cmp.w	#Dia_LiMark,d0
	beq	.MLi
	cmp.w	#Dia_TxMark,d0
	beq	.MHy
.MNext	add.w	(a2),a2
	tst.w	(a2)
	bne.s	.MLoop
; Appui souris?
; ~~~~~~~~~~~~~
	tst.w	d4
	beq.s	.PaSou
	bsr	.Timer
	btst	#3,Dia_Flags(a4)
	beq.s	.PaSou
	addq.w	#1,Dia_Exit(a4)
.PaSou
; Fin de la boucle! Sortie= BNE!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.TFini	move.w	Dia_Exit(a4),d0
	movem.l	(sp)+,a2-a3/d2-d7
	rts


; Reset le timer si action
; ~~~~~~~~~~~~~~~~~~~~~~~~~
.Timer	tst.l	Dia_Timer(sp)
	beq.s	.Skt
	move.l	T_VblCount(a5),Dia_TimerPos(a4)
.Skt	rts

; Mousekey->	Bouton
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.MBt	tst.w	d4
	beq	.MNext
	cmp.l	a2,d5
	bne	.MNext
; Met le numero du bouton
	move.w	Dia_ZoNumber(a2),Dia_Return(a4)
; Passe au dessin suivant
	move.l	Dia_ZoPos(a2),d1
	addq.w	#1,d1
	cmp.w	Dia_BtMax(a2),d1
	ble.s	.PaSup
	move.w	Dia_BtMin(a2),d1
.PaSup	move.l	d1,Dia_ZoPos(a2)
; Appelle les routines
	Rbsr	L_Dia_BtDraw
	Rbsr	L_Dia_ZoChange
	Rbsr	L_Dia_Patch
	bclr	#5,Dia_ZoFlags(a2)
	bne.s	.PaW
	Rbsr	L_Dia_NoMKey
	cmp.l	Dia_ZoPos(a2),d0
	beq.s	.Ju
	move.l	d0,Dia_ZoPos(a2)
	Rbsr	L_Dia_BtDraw
	Rbsr	L_Dia_Patch
.Ju	bra.s	.BQuit
.PaW	move.l	d0,Dia_ZoPos(a2)
	move.l	a2,Dia_Release(a4)
; Positionne le flag EXIT
.BQuit	bclr	#7,Dia_ZoFlags(a2)
	beq	.MNext
	addq.w	#1,Dia_Exit(a4)
	bra	.MNext
; Mousekey->	Une edition
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.MEd	tst.w	d4
	beq	.MNext
	cmp.l	a2,d5
	bne	.MNext
; Change l'�dition courante
	move.l	Dia_Edited(a4),d0
	move.l	a2,Dia_Edited(a4)
	cmp.l	d0,a2			Active la nouvelle fenetre
	beq.s	.Bla
	cmp.l	#-1,d0
	beq.s	.Bla
	move.l	d0,a0
	Rbsr	L_Dia_EdInactive
	Rbsr	L_Dia_Patch
.Bla	bra	.MNext
; Mousekey->	Un slider
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.MSl	tst.w	d4
	beq	.MNext
	cmp.l	a2,d5
	bne	.MNext
	move.w	Dia_ZoNumber(a2),Dia_Return(a4)
	lea	Dia_Sl(a2),a0
	Rbsr	L_Sl_Clic
	move.l	T_RastPort(a5),a1
	move.w	Dia_Writing(a4),d0
	GfxCa5	SetDrMd
	bclr	#7,Dia_ZoFlags(a2)
	bra	.MNext
; Mousekey->	Une liste active
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.MLi	cmp.l	a2,d5
	bne.s	.LNon
; Coordonnees dans la fenetre
	move.w	d7,d2
	lsr.w	#3,d2
	add.w	Dia_LiPos(a2),d2
	cmp.w	Dia_LiMaxAct(a2),d2		Autorise?
	blt.s	.LDans
.LNon	moveq	#-1,d2
; Effacer une ancienne selection?
.LDans	move.w	Dia_LiActNumber(a2),d0
	bmi.s	.LAct
	cmp.w	d0,d2
	beq	.LKey
	btst	#2,Dia_ZoFlags+1(a2)		Flag appui sur mouse?
	beq.s	.LPak1
	tst.w	d4
	beq.s	.LKey
	tst.w	d2
	bmi.s	.LKey
.LPak1	move.w	#-1,Dia_LiActNumber(a2)
	move.l	#-1,Dia_ZoPos(a2)
	Rbsr	L_Dia_LiActive
	Rbsr	L_Dia_LiAff
	Rbsr	L_Dia_Patch
; Active la nouvelle
.LAct	btst	#2,Dia_ZoFlags+1(a2)		Flag appui sur mouse?
	beq.s	.LPak2
	tst.w	d4
	beq.s	.LKey
.LPak2	move.w	d2,d0
	bmi.s	.LKey
	move.w	d0,Dia_LiActNumber(a2)
	Rbsr	L_Dia_LiActive
	Rbsr	L_Dia_LiAff
	Rbsr	L_Dia_Patch
; Appui sur la souris?
.LKey	tst.w	d4
	beq	.MNext
	moveq	#0,d0
	move.w	d2,d0
	bmi	.MNext
	move.w	Dia_ZoNumber(a2),Dia_Return(a4)
	move.l	d0,Dia_ZoPos(a2)
	Rbsr	L_Dia_ZoChange
	Rbsr	L_Dia_Patch
	bclr	#5,Dia_ZoFlags(a2)
	bne.s	.LNow
	Rbsr	L_Dia_NoMKey
	Rbsr	L_Dia_Patch
.LNow	bra	.BQuit

; Mousekey->	Hypertexte
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.MHy	cmp.l	a2,d5
	bne.s	.HRien
; Trouve le mot actif dans cette fenetre
	move.w	d7,d2
	lsr.w	#3,d2
	move.w	d6,d1
	lsr.w	#3,d1
	move.l	Dia_TxDisplay(a2),a0
	move.w	d2,d0
	mulu	Dia_TxDispSize(a2),d0
	add.w	d0,a0
	move.l	(a0)+,d0
	bne.s	.HIn
	bra.s	.HRien
.HLoop	cmp.b	2(a0),d1
	bcs.s	.HNext
	cmp.b	3(a0),d1
	bcs.s	.HFound
.HNext	lea	Dia_TxDispZone(a0),a0
.HIn	tst.w	(a0)
	bne.s	.HLoop
.HRien	moveq	#0,d2
	bra.s	.HSuit
; Trouve!
.HFound	move.w	d2,d3
	move.l	a0,d2
; Effacer une ancienne selection?
.HSuit	move.l	Dia_TxAct(a2),d0
	beq.s	.HAct
	cmp.l	d0,d2
	beq.s	.HKey
	move.l	#-1,Dia_ZoPos(a2)
	Rbsr	L_Dia_TxActive
	move.l	Dia_TxAct(a2),a0
	move.w	Dia_TxYAct(a2),d0
	clr.l	Dia_TxAct(a2)
	Rbsr	L_Dia_TxAffActive
	Rbsr	L_Dia_Patch
; Active la nouvelle
.HAct	move.l	d2,Dia_TxAct(a2)
	beq.s	.HKey
	move.w	d3,Dia_TxYAct(a2)
	Rbsr	L_Dia_TxActive
	move.l	d2,a0
	move.w	d3,d0
	Rbsr	L_Dia_TxAffActive
	Rbsr	L_Dia_Patch
; Appui sur la souris?
.HKey	tst.w	d4
	beq	.MNext
	tst.l	d2
	beq	.MNext
	move.w	Dia_ZoNumber(a2),Dia_Return(a4)
	movem.l	a3/a6/d7,-(sp)
	mulu	Dia_TxDispSize(a2),d3
	move.l	Dia_TxDisplay(a2),a6
	move.l	0(a6,d3.w),a6
	move.l	d2,a0
	moveq	#0,d0
	move.b	6(a0),d0
	add.l	d0,a6
	Rbsr	L_Dia_Chr
	bpl.s	.Let
; Un chiffre
	Rbsr	L_Dia_FChif
	move.l	(a3)+,Dia_ZoPos(a2)
	clr.b	Dia_TxBuffer(a2)
	bra	.HX
; Un texte!
.Let	sub.l	#1,a6
	lea	Dia_TxBuffer(a2),a0
	lea	Dia_TxBufferEnd-2(a2),a1
.HCopy	move.b	(a6)+,d0
	cmp.l	a1,a0
	bcc.s	.HOut
	cmp.b	#",",d0
	beq.s	.HOut
	cmp.b	#"]",d0
	beq.s	.HOut
	move.b	d0,(a0)+
	bra.s	.HCopy
.HOut	clr.b	(a0)
	clr.l	Dia_ZoPos(a2)
.HX	movem.l	(sp)+,a3/a6/d7
	Rbsr	L_Dia_ZoChange
	Rbsr	L_Dia_Patch
	bclr	#5,Dia_ZoFlags(a2)
	bne	.MNext
	Rbsr	L_Dia_NoMKey
	Rbsr	L_Dia_Patch
	bra	.MNext

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Recherche une zone dans l'arbre
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetMouseZone
; - - - - - - - - - - - - -
	SyCall	XyMou			Coordonn�es du curseur
	moveq	#0,d3
	SyCall	XyScr			Dans l'�cran
	Rbra	L_Dia_GetZ
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Trouve la zone D1/D2 (coords ecran)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_GetZ
; - - - - - - - - - - - - -
	move.l	Dia_Buffer(a4),a0
	tst.w	(a0)
	beq.s	.Out
.Loop	cmp.w	#Dia_ZoMark,4(a0)
	bne.s	.Next
	lea	6(a0),a1
	move.w	6+0(a0),d0
	cmp.w	d0,d1			X
	bcs.s	.Next
	add.w	6+4(a0),d0		X+SX
	cmp.w	d0,d1
	bcc.s	.Next
	move.w	6+2(a0),d0		Y
	cmp.w	d0,d2
	bcs.s	.Next
	add.w	6+6(a0),d0		Y+SY
	cmp.w	d0,d2
	bcs.s	.Ok
.Next	move.w	(a0),d0
	add.w	d0,a0
	bne.s	.Loop
.Out	moveq	#0,d0
	rts
.Ok	sub.w	Dia_ZoX(a0),d1
	sub.w	Dia_ZoY(a0),d2
	move.l	a0,d0
	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Activation de la premiere fenetre d'�dition
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_EdFirst
; - - - - - - - - - - - - -
	move.l	Dia_Buffer(a4),a0
	clr.l	Dia_Edited(a4)
	Rbra	L_EdaLoop
; - - - - - - - - - - - - -
	Lib_Def	Dia_EdNext
; - - - - - - - - - - - - -
	add.w	(a0),a0
	Rbra	L_EdaLoop
; - - - - - - - - - - - - -
	Lib_Def	EdaLoop
; - - - - - - - - - - - - -
.Loop	tst.w	(a0)
	beq.s	Edano
	cmp.w	#Dia_EdMark,2(a0)
	beq.s	Edaed
	add.w	(a0),a0
	bra.s	.Loop
Edaed	move.l	a0,Dia_Edited(a4)
	Rbsr	L_Dia_EdActive
Edano	rts


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					ERREURS DE L'INTERPRETEUR
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_OBuffer
; - - - - - - - - - - - - -
	move.w	#EDia_OBuffer,Dia_Error(a4)
	Rbra	L_Dia_Er
; - - - - - - - - - - - - -
	Lib_Def	Dia_NPar
; - - - - - - - - - - - - -
	move.w	#EDia_NPar,Dia_Error(a4)
	Rbra	L_Dia_Er
; - - - - - - - - - - - - -
	Lib_Def	Dia_Fonc
; - - - - - - - - - - - - -
	move.w	#EDia_FCall,Dia_Error(a4)
	Rbra	L_Dia_Er
; - - - - - - - - - - - - -
	Lib_Def	Dia_Synt
; - - - - - - - - - - - - -
	move.w	#EDia_Syntax,Dia_Error(a4)
	Rbra	L_Dia_Er
; - - - - - - - - - - - - -
	Lib_Def	Dia_Er
; - - - - - - - - - - - - -
	move.l	a6,d0
	sub.l	Dia_Programs(a4),d0
	move.w	d0,Dia_ErrorPos(a4)
	Rbra	L_Dia_Quit
; - - - - - - - - - - - - -
	Lib_Def	Dia_Quit
; - - - - - - - - - - - - -
	move.l	Dia_Sp(a4),sp
	move.w	Dia_Error(a4),d0
	movem.l	(sp)+,d2-d7/a2/a3/a6
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHRGET rapide...
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_Chr
; - - - - - - - - - - - - -
Dia_Chr	moveq	#0,d0
.Loop	move.b	(a6)+,d0
	bmi.s	.Loop
	move.w	d0,a0
	move.b	.Codes(pc,d0.w),d0
	bmi.s	.Loop
	move.w	d0,CCR
	exg	d0,a0
	rts
.Codes	dc.b	%00100		*  0 " " Fin
	dc.b	$80+%00000	*  1 " "
	dc.b	$80+%00000	*  2 " "
	dc.b	$80+%00000	*  3 " "
	dc.b	$80+%00000	*  4 " "
	dc.b	$80+%00000	*  5 " "
	dc.b	$80+%00000	*  6 " "
	dc.b	$80+%00000	*  7 " "
	dc.b	$80+%00000	*  8 " "
	dc.b	$80+%00000	*  9 " "
	dc.b	$80+%00000	* 10 " "
	dc.b	$80+%00000	* 11 " "
	dc.b	$80+%00000	* 12 " "
	dc.b	$80+%00000	* 13 " "
	dc.b	$80+%00000	* 14 " "
	dc.b	$80+%00000	* 15 " "
	dc.b	$80+%00000	* 16 " "
	dc.b	$80+%00000	* 17 " "
	dc.b	$80+%00000	* 18 " "
	dc.b	$80+%00000	* 19 " "
	dc.b	$80+%00000	* 20 " "
	dc.b	$80+%00000	* 21 " "
	dc.b	$80+%00000	* 22 " "
	dc.b	$80+%00000	* 23 " "
	dc.b	$80+%00000	* 24 " "
	dc.b	$80+%00000	* 25 " "
	dc.b	$80+%00000	* 26 " "
	dc.b	$80+%00000	* 27 " " Escape
	dc.b	$80+%00000	* 28 " "
	dc.b	$80+%00000	* 29 " "
	dc.b	$80+%00000	* 30 " "
	dc.b	$80+%00000	* 31 " "

	dc.b	$80+%00000	* 32 " " 0
	dc.b	%00011		* 33 "!" 1
	dc.b	%00011		* 34 """ 2
	dc.b	%00011		* 35 "#" 3
	dc.b	%01001		* 36 "$" 4
	dc.b	%01001		* 37 "%" 5
	dc.b	%00011		* 38 "&" 6
	dc.b	%00011		* 39 "'" 7
	dc.b	$80+%00000	* 40 "(" 8
	dc.b	$80+%00000	* 41 ")" 9
	dc.b	%00011		* 42 "*" 10
	dc.b	%00011		* 43 "+" 11
	dc.b	%00000		* 44 "," 12
	dc.b	%00011		* 45 "-" 13
	dc.b	$80+%00000	* 46 "." 14
	dc.b	%00011		* 47 "/" 15
	dc.b	%01001		* 48 "0"
	dc.b	%01001		* 49 "1"
	dc.b	%01001		* 50 "2"
	dc.b	%01001		* 51 "3"
	dc.b	%01001		* 52 "4"
	dc.b	%01001		* 53 "5"
	dc.b	%01001		* 54 "6"
	dc.b	%01001		* 55 "7"
	dc.b	%01001		* 56 "8"
	dc.b	%01001		* 57 "9"
	dc.b	%00000		* 58 ":"
	dc.b	%00000		* 59 ";"
	dc.b	%00011		* 60 "<"
	dc.b	%00011		* 61 "="
	dc.b	%00011		* 62 ">"
	dc.b	%00011		* 63 "?"
	dc.b	%00011		* 64 "@"
	dc.b	%00001		* 65 "A"
	dc.b	%00001		* 66 "B"
	dc.b	%00001		* 67 "C"
	dc.b	%00001		* 68 "D"
	dc.b	%00001		* 69 "E"
	dc.b	%00001		* 70 "F"
	dc.b	%00001		* 71 "G"
	dc.b	%00001		* 72 "H"
	dc.b	%00001		* 73 "I"
	dc.b	%00001		* 74 "J"
	dc.b	%00001		* 75 "K"
	dc.b	%00001		* 76 "L"
	dc.b	%00001		* 77 "M"
	dc.b	%00001		* 78 "O"
	dc.b	%00001		* 79 "P"
	dc.b	%00001		* 80 "Q"
	dc.b	%00001		* 81 "R"
	dc.b	%00001		* 82 "S"
	dc.b	%00001		* 83 "T"
	dc.b	%00001		* 84 "U"
	dc.b	%00001		* 85 "V"
	dc.b	%00001		* 86 "K"
	dc.b	%00001		* 87 "W"
	dc.b	%00001		* 88 "X"
	dc.b	%00001		* 89 "Y"
	dc.b	%00001		* 90 "Z"
	dc.b	%00011		* 91 "["
	dc.b	%00011		* 92 "\"
	dc.b	%00011		* 93 "]"
	dc.b	%00011		* 94 "^"
	dc.b	%00011		* 95 " "
	dc.b	%00000		* 96 "`"
	dc.b	$80+%00000	* 97 "a"
	dc.b	$80+%00000	* 98 "b"
	dc.b	$80+%00000	* 99 "c"
	dc.b	$80+%00000	* 100 "d"
	dc.b	$80+%00000	* 101 "e"
	dc.b 	$80+%00000	* 102 "f"
	dc.b	$80+%00000	* 103 "g"
	dc.b	$80+%00000	* 104 "h"
	dc.b	$80+%00000	* 105 "i"
	dc.b	$80+%00000	* 106 "j"
	dc.b	$80+%00000	* 107 "k"
	dc.b	$80+%00000	* 108 "l"
	dc.b	$80+%00000	* 109 "m"
	dc.b	$80+%00000	* 110 "n"
	dc.b	$80+%00000	* 111 "o"
	dc.b	$80+%00000	* 112 "p"
	dc.b	$80+%00000	* 113 "q"
	dc.b	$80+%00000	* 114 "r"
	dc.b	$80+%00000	* 115 "s"
	dc.b	$80+%00000	* 116 "t"
	dc.b	$80+%00000	* 117 "u"
	dc.b	$80+%00000	* 118 "v"
	dc.b	$80+%00000	* 119 "w"
	dc.b	$80+%00000	* 120 "x"
	dc.b	$80+%00000	* 121 "y"
	dc.b	$80+%00000	* 122 "z"
	dc.b	$80+%00000	* 123 "{"
	dc.b	$80+%00011	* 124 "|"
	dc.b	$80+%00000	* 125 "}"
	dc.b	$80+%00000	* 126 "~"
	dc.b	$80+%00000	* 127 ""
	even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Prend un chiffre
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_FChif
; - - - - - - - - - - - - -
	moveq	#0,d1
	cmp.b	#"%",d0
	beq.s	.Bin
	cmp.b	#"$",d0
	beq.s	.Hex
	sub.b	#"0",d0
	move.b	d0,d1
	moveq	#0,d0
.Loop	Rbsr	L_Dia_ChrC
	bmi.s	.Fini
	cmp.b	#10,d0
	bcc.s	.Fini
	add.l	d1,d1
	move.l	d1,a0
	lsl.l	#2,d1
	add.l	a0,d1
	add.l	d0,d1
	bra.s	.Loop
.Fini	move.l	d1,-(a3)
	addq.l	#1,d7
	subq.l	#1,a6
	rts
; En binaire
; ~~~~~~~~~~
.Bin	Rbsr	L_Dia_ChrC
	bmi.s	.Fini
	cmp.b	#2,d0
	bcc.s	.Fini
	lsl.l	#1,d1
	or.b	d0,d1
	bra.s	.Bin
; En hexa
; ~~~~~~~
.Hex	Rbsr	L_Dia_ChrC
	bmi.s	.Fini
	cmp.b	#10,d0
	bcs.s	.Hx1
	add.b	#"0"-"A",d0
	bmi.s	.Fini
	cmp.b	#6,d0
	bcc.s	.Fini
	add.b	#10,d0
.Hx1	lsl.l	#4,d1
	or.b	d0,d1
	bra.s	.Hex

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					CHRGET SPECIAL CHIFFRES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_ChrC
; - - - - - - - - - - - - -
	move.b	(a6)+,d0
	sub.b	#"0",d0
	rts
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 					Conversion Decimal>>>ascii
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Def	Dia_DecToAsc
; - - - - - - - - - - - - -
	movem.l	a1/d0-d3,-(sp)
	tst.l	d0
	bpl.s	.Pos
	move.b	#"-",(a0)+
	neg.l	d0
.Pos	lea	.MDix(pc),a1
	move.l	(a1)+,d1
	moveq	#0,d3
.Loop   moveq	#-1,d2
.Lp	addq.b 	#1,d2
        sub.l 	d1,d0
        bcc.s 	.Lp
        add.l 	d1,d0
	move.l	(a1)+,d1
	beq.s	.Put
	tst.b	d2
	bne.s	.Put
	tst.w	d3
	beq.s	.Loop
.Put	add.b	#"0",d2
	move.b	d2,(a0)+
	addq.w	#1,d3
	tst.l	d1
	bne.s	.Loop
	movem.l	(sp)+,a1/d0-d3
	rts
; Multiples de 10
.MDix	dc.l 1000000000,100000000,10000000,1000000
        dc.l 100000,10000,1000,100,10,1,0


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
	Lib_Def	Bt_InitList
; - - - - - - - - - - - - -
	movem.l	a2/d2,-(sp)
	move.l	a0,a2
	move.w	d0,d2
.Loop	Rbsr	L_Bt_Init
	lea	Bt_Long(a2),a2
	move.l	a2,a0
	move.w	d2,d0
	tst.w	(a0)
	bne.s	.Loop
	movem.l	(sp)+,a2/d2
	rts

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
	Lib_Def	Bt_Init
; - - - - - - - - - - - - -
	movem.l	a2/d2-d5,-(sp)
	move.w	d0,d2
	move.l	a0,a2
	Rbsr	L_Bt_CPos
	tst.w	d2
	beq.s	.Skip
	Rbsr	L_Bt_CDraw
.Skip	move.w	Bt_Zone(a2),d1
	move.w	Bt_X(a2),d2
	move.w	Bt_Y(a2),d3
	moveq	#0,d0
	move.b	Bt_Dx(a2),d0
	add.w	d0,d2
	move.b	Bt_Dy(a2),d0
	add.w	d0,d3
	move.w	d2,d4
	move.w	d3,d5
	move.b	Bt_Sx(a2),d0
	add.w	d0,d4
	move.b	Bt_Sy(a2),d0
	add.w	d0,d5
	SyCall	SetZone
	movem.l	(sp)+,a2/d2-d5
	rts

; __________________________________________________
;
;	Fin d'un bouton
; __________________________________________________
;
;	A0=	Structure Bouton
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Bt_End
; - - - - - - - - - - - - -
	move.w	Bt_Zone(a0),d1
	SyCall	RazZone
	rts

; __________________________________________________
;
;	Gestion des boutons
; __________________________________________________
;
;	A0=	Premiere structure Bouton
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Bt_Gere
; - - - - - - - - - - - - -
	movem.l	d2/a2,-(sp)
	move.l	a0,a2
	SyCall	MouseKey
	move.w	d1,-(sp)
	and.w	#$03,d1
	beq.s	.BNone
	SyCall	GetZone
	swap	d1
.ELoop	cmp.w	Bt_Zone(a2),d1
	beq.s	.Down
	lea	Bt_Long(a2),a2
	tst.w	Bt_Number(a2)
	bne.s	.ELoop
.BNone	moveq	#0,d0
.BOut	move.w	(sp)+,d1
	movem.l	(sp)+,d2/a2
	tst.l	d0
	rts
.Down	bsr	RBt_Down
	bra.s	.BOut
; On appuie sur un bouton A2
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
RBt_Down
	btst	#Bt_FlagOnOf,Bt_Flags(a2)
	bne.s	.OOf
; One shot
	eor.w	#1,Bt_Pos(a2)
	Rbsr	L_Bt_CDraw
	Rbsr	L_Dia_NoMKey
	eor.w	#1,Bt_Pos(a2)
	Rbsr	L_Bt_CDraw
	Rbsr	L_Bt_CChange
	move.l	a2,d0
	rts
; On / Off
.OOf	eor.w	#1,Bt_Pos(a2)
	Rbsr	L_Bt_CDraw
	btst	#Bt_FlagNoWait,Bt_Flags(a2)
	bne.s	.WSkip2
	Rbsr	L_Dia_NoMKey
.WSkip2	bset	#Bt_FlagNew,Bt_Flags(a2)
	Rbsr	L_Bt_CChange
	move.l	a2,d0
	rts

; Appelle la routine de modification bouton
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	Bt_CChange
; - - - - - - - - - - - - -
	move.b	Bt_RChange(a2),d0
	Rbne	L_Bt_Call
	rts
; Ramene la position du bouton
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	Bt_CPos
; - - - - - - - - - - - - -
	move.b	Bt_RPos(a2),d0
	Rbne	L_Bt_Call
	rts
; Va dessiner un bouton
; ~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	Bt_CDraw
; - - - - - - - - - - - - -
	move.b	Bt_RDraw(a2),d0
	Rbne	L_Bt_Call
	rts
; Appelle la routine
; ~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	Bt_Call
; - - - - - - - - - - - - -
	move.l	Bt_Routines(a2),a0
	ext.w	d0
	lsl.w	#2,d0
	jmp	-4(a0,d0.w)

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
	Lib_Def	Sl_Init
; - - - - - - - - - - - - -
	movem.l	d2-d5/a2,-(sp)
	move.l	a0,a2
; Initialise les zones
	move.w	Sl_Zone(a2),d1
	beq.s	.SkipZ
	move.w	Sl_X(a2),d2
	move.w	Sl_Y(a2),d3
	move.w	d2,d4
	move.w	d3,d5
	add.w	Sl_Sx(a2),d4
	add.w	Sl_Sy(a2),d5
	sub.w	Sl_ZDx(a2),d2
	sub.w	Sl_ZDy(a2),d3
	add.w	Sl_ZDx(a2),d4
	add.w	Sl_ZDy(a2),d5
	SyCall	SetZone
.SkipZ
; Update les valeurs
	moveq	#0,d0
	Rbsr	L_Sl_Call
; X/Y
	move.w	Sl_X(a2),Sl_Start(a2)
	move.w	Sl_Sx(a2),Sl_Size(a2)
	btst	#Sl_FlagVertical,Sl_Flags(a2)
	beq.s	.Skip
	move.w	Sl_Y(a2),Sl_Start(a2)
	move.w	Sl_Sy(a2),Sl_Size(a2)
.Skip
; Va dessiner
	moveq	#0,d0
	move.l	a2,a0
	Rbsr	L_Sl_Draw
; Ok!
	moveq	#0,d0
	movem.l	(sp)+,d2-d5/a2
	rts

; Appel d'une routine slider
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
; - - - - - - - - - - - - -
	Lib_Def	Sl_Call
; - - - - - - - - - - - - -
	move.l	Sl_Routines(a2),d1
	bne.s	.Call
	rts
.Call	move.l	d1,a0
	jmp	0(a0,d0.w)

; __________________________________________________
;
;	Gestion d'un slider
; __________________________________________________
;
;	A0=	Structure slider
; _______________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Sl_Clic
; - - - - - - - - - - - - -
	movem.l	a2-a2/d2-d7,-(sp)
	move.l	a0,a2
; Active le slider
	moveq	#0,d0
	Rbsr	L_Sl_Call
; Souris
	bsr	Sl_XYMouEc
	move.w	d2,d0
	sub.w	Sl_Start(a2),d0
	cmp.w	Sl_Mouse1(a2),d0
	bcs	.Sl_Down
	cmp.w	Sl_Mouse2(a2),d0
	bcc	.Sl_Up
;
; POSITIONNE DIRECTEMENT
; ~~~~~~~~~~~~~~~~~~~~~~
.Direct	move.w	d0,d7
	sub.w	Sl_Mouse1(a2),d7
	move.l	a2,a0
	moveq	#-1,d0
	Rbsr	L_Sl_Draw
.loop	sub.w	d7,d2
	bpl.s	.skipu
	moveq	#0,d2
.skipu	sub.w	Sl_Start(a2),d2
	bpl.s	.skip0
	moveq	#0,d2
.skip0	move.w	Sl_Global(a2),d0
	addq.w	#1,d0
	mulu	d0,d2
	move.w	Sl_Size(a2),d1
	addq.w	#1,d1
	divu	d1,d2
	move.w	Sl_Global(a2),d0
	sub.w	Sl_Window(a2),d0
	bpl.s	.Skk
	moveq	#0,d0
.Skk	cmp.w	d0,d2
	ble.s	.skip1
	move.w	d0,d2
.skip1	cmp.w	Sl_Position(a2),d2
	beq.s	.skip
	move.w	d2,Sl_Position(a2)
	move.l	a2,a0
	moveq	#-1,d0
	Rbsr	L_Sl_Draw
	SyCall	MouseKey
	btst	#1,d1
	beq.s	.skip
	moveq	#3*4,d0
	Rbsr	L_Sl_Call
.skip	SyCall	MouseKey
	btst	#0,d1
	beq.s	.out
	Rbsr	L_Dia_WaitMul
	Rbsr	L_Dia_Patch
	bsr	Sl_XYMouEc
	bra.s	.loop
.out	moveq	#3*4,d0
	Rbsr	L_Sl_Call
	Rbsr	L_Dia_Patch
	bra	.End

;	sub.w	d7,d2
;	bpl.s	.skip0
;	moveq	#0,d2
;.skip0	move.w	Sl_Global(a2),d0
;	mulu	d0,d2
;	move.w	Sl_Window(a2),d1
;	divu	d1,d2
;	lsr.w	#3,d2
;	move.w	Sl_Global(a2),d0
;	sub.w	Sl_Window(a2),d0
;	addq.w	#1,d0
;	bmi.s	.skip
;	cmp.w	d0,d2
;	bcs.s	.skip1
;	move.w	d0,d2
;.skip1
;
; Slider-> Gauche/Haut
; ~~~~~~~~~~~~~~~~~~~~
.Sl_Down
	move.l	a2,a0
	moveq	#-1,d0
	Rbsr	L_Sl_Draw
	move.w	Sl_Position(a2),d0
	sub.w	Sl_Scroll(a2),d0
	bpl.s	.Sld0
	moveq	#0,d0
.Sld0	cmp.w	Sl_Position(a2),d0
	beq.s	.Sld1
	move.w	d0,Sl_Position(a2)
	moveq	#1*4,d0
	Rbsr	L_Sl_Call
	Rbsr	L_Dia_Patch
.Sld1	SyCall	MouseKey
	btst	#0,d1
	beq.s	.End
	btst	#1,d1
	bne.s	.Sl_Down
	Rbsr	L_Dia_WaitMul
	bra.s	.Sld1
;
; Slider-> Droite/Bas
; ~~~~~~~~~~~~~~~~~~~
.Sl_Up
	move.l	a2,a0
	moveq	#-1,d0
	Rbsr	L_Sl_Draw
	move.w	Sl_Position(a2),d0
	add.w	Sl_Scroll(a2),d0
	move.w	d0,d1
	add.w	Sl_Window(a2),d1
	cmp.w	Sl_Global(a2),d1
	bls.s	.Slu0
	move.w	Sl_Global(a2),d0
	sub.w	Sl_Window(a2),d0
	bpl.s	.Slu0
	moveq	#0,d0
.Slu0	cmp.w	Sl_Position(a2),d0
	beq.s	.Slu1
	move.w	d0,Sl_Position(a2)
	moveq	#2*4,d0
	Rbsr	L_Sl_Call
	Rbsr	L_Dia_Patch
.Slu1	SyCall	MouseKey
	btst	#0,d1
	beq.s	.End
	btst	#1,d1
	bne.s	.Sl_Up
	Rbsr	L_Dia_WaitMul
	bra.s	.Slu1
;
; Fin de la gestion
; ~~~~~~~~~~~~~~~~~
.End	move.l	a2,a0
	moveq	#0,d0
	Rbsr	L_Sl_Draw
	movem.l	(sp)+,a2-a2/d2-d7
	rts
; Routinette: XY Mouse dans l'ecran courant, dans le bon sens
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sl_XYMouEc
	SyCall	XyMou
	moveq	#0,d3
	SyCall	XyScr
	btst	#Sl_FlagVertical,Sl_Flags(a2)
	bne.s	.Skip
	move.w	d1,d2
.Skip	rts

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
	Lib_Def	Sl_Draw
; - - - - - - - - - - - - -
	movem.l	a2/d2-d7,-(sp)
	move.l	a0,a2
; Change les encres
	lea	Sl_Inactive(a2),a0
	tst.w	d0
	beq.s	.Skip
	lea	Sl_Active(a2),a0
.Skip	movem.w	(a0),d0-d7
	EcCall	SetSli
; Writing JAM2
	move.l	T_RastPort(a5),a1
	moveq	#1,d0
	GfxCa5	SetDrMd
; Dessine le slider
	lea	Sl_Sx(a2),a0
	movem.w	(a0),d1-d7
	btst	#Sl_FlagVertical,Sl_Flags(a2)
	bne.s	.Vert
; Slider horizontal
	EcCall	HorSli
	lea	Sl_Sx(a2),a2
	movem.w	(a2),d1-d7
	move.w	d1,d0
	bra.s	.Pour
; Slider vertical
.Vert	EcCall	VerSli
	lea	Sl_Sx(a2),a2
	movem.w	(a2),d1-d7
	move.w	d2,d0
; Calcule les pourcentages
.Pour	EcCall	PourSli
	add.w	d6,d7			Plus taille du centre
	move.w	d6,Sl_Mouse1(a2)
	move.w	d7,Sl_Mouse2(a2)
; Ok
	moveq	#0,d0
	movem.l	(sp)+,a2/d2-d7
	rts

; _________________________________________________
;
;	Attend le relachement de la souris
; _________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_NoMKey
; - - - - - - - - - - - - -
	movem.l	d0-d1/a0-a1,-(sp)
.Lp	Rbsr	L_Dia_WaitMul
	SyCall	MouseKey
	and.w	#$01,d1
	bne.s	.Lp
	movem.l	(sp)+,d0-d1/a0-a1
	rts

; _________________________________________________
;
;	Nettoyage du buffer clavier
; _________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_ClearKey
; - - - - - - - - - - - - -
	movem.l	d0-d1/a0-a1,-(sp)
	SyCall	ClearKey
	clr.l	T_ClLast(a5)
	movem.l	(sp)+,d0-d1/a0-a1
	rts
; _________________________________________________
;
;	Appel du patch
; _________________________________________________
;
; - - - - - - - - - - - - -
	Lib_Def	Dia_Patch
; - - - - - - - - - - - - -
	movem.l	d0/a0,-(sp)
	move.l	Patch_ScCopy(a5),d0
	beq.s	.Skip
	move.l	d0,a0
	jsr	(a0)
.Skip	movem.l	(sp)+,d0/a0
	rts

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
	Lib_Def	ScCopy
; - - - - - - - - - - - - -
	movem.l	d0-d7/a0-a4,-(sp)
	lea	EcCurrent(a0),a2
	move.l	a2,SccEcO(a5)
	lea	EcCurrent(a1),a2
	move.l	a1,SccEcD(a5)
	move.w	#$CC,d6
	Rjsr	L_Sco0
	movem.l	(sp)+,d0-d7/a0-a4
	rts
; - - - - - - - - - - - - -
	Lib_Def	Dia_ScCopy
; - - - - - - - - - - - - -
	movem.l	d0-d7/a0-a4,-(sp)
	move.l	T_EcCourant(a5),a0
	move.l	a0,a1
	lea	EcCurrent(a0),a2
	move.l	a2,SccEcO(a5)			Bitmaps
	move.l	a2,SccEcD(a5)
	move.l	Dia_BaseX(a4),d6
	add.l	d6,d0
	add.l	d6,d2
	add.l	d6,d4
	move.l	Dia_BaseY(a4),d6
	add.l	d6,d1
	add.l	d6,d3
	add.l	d6,d5
	move.w	#$CC,d6
	Rjsr	L_Sco0
	movem.l	(sp)+,d0-d7/a0-a4
	rts

; _____________________________________________________________________________
;
;								Unpacker
;______________________________________________________________________________
;
UAEc:	equ 0
UDEc:	equ 4
UITy:	equ 8
UTy:	equ 10
UTLine:	equ 12
UNPlan:	equ 14
USx	equ 16
USy	equ 18
UPile:	equ 20

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
	Lib_Def	UnPack_Screen
; - - - - - - - - - - - - -
	movem.l	a2-a6/d2-d7,-(sp)
	cmp.l	#SCCode,PsCode(a0)
	bne	.NoPac
	move.w	d0,d1
	moveq	#0,d2
	moveq	#0,d3
	moveq	#0,d4
	moveq	#0,d5
	move.w	PsTx(a0),d2
	move.w	PsTy(a0),d3
	move.w	PsNPlan(a0),d4
	move.w	PsCon0(a0),d5
	move.w	PsNbCol(a0),d6
	lea	PsPal(a0),a1
	move.l	a0,-(sp)
	EcCall	Cree
	move.l	a0,a1
	move.l	(sp)+,a0
	bne.s	.NoScreen
* Enleve le curseur
	movem.l	a0-a6/d0-d7,-(sp)
	lea	.CuCu(pc),a1
	WiCall	Print
	movem.l	(sp)+,a0-a6/d0-d7
* Change View/Offset
	move.w	PsAWx(a0),EcAWX(a1)
	move.w	PsAWy(a0),EcAWY(a1)
	move.w	PsAWTx(a0),EcAWTX(a1)
	move.w	PsAWTy(a0),EcAWTY(a1)
	move.w	PsAVx(a0),EcAVX(a1)
	move.w	PsAVy(a0),EcAVY(a1)
	move.b	#%110,EcAW(a1)
	move.b	#%110,EcAWT(a1)
	move.b	#%110,EcAV(a1)
* Unpack!
	lea	PsLong(a0),a0
	moveq	#0,d1
	moveq	#0,d2
	Rbsr	L_UnPack_Bitmap
	move.l	a1,a0
	bra.s	.Out
.NoPac
	moveq	#0,d0
	moveq	#0,d1
	bra.s	.Out
.NoScreen
	moveq	#0,d0
	moveq	#1,d1
.Out	movem.l	(sp)+,d2-d7/a2-a6
	rts
.CuCu	dc.b	27,"C0",0
	even

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
	Lib_Def	UnPack_Bitmap
; - - - - - - - - - - - - -
	movem.l	a0-a6/d1-d7,-(sp)
* Jump over SCREEN DEFINITION
	cmp.l	#SCCode,(a0)
	bne.s	dec0
	lea	PsLong(a0),a0
* Is it a packed bitmap?
dec0	cmp.l	#BMCode,(a0)
	bne	NoPac

* Parameter preparation
	lea	-UPile(sp),sp		* Space to work
	lea	EcCurrent(a1),a2
	move.l	a2,UAEc(sp)		* Bitmaps address
        move.w 	EcTLigne(a1),d7    	* d7--> line size
	move.w	EcNPlan(a1),d0		* How many bitplanes
	cmp.w	Pknplan(a0),d0
	bne	NoPac0
	move.w	d0,UNPlan(sp)
	move.w	Pktcar(a0),d6		* d6--> SY square

	lsr.w	#3,d1			* > number of bytes
        tst.l 	d1			* Screen address in X
        bpl.s 	dec1
        move.w 	Pkdx(a0),d1
dec1:   tst.l 	d2			* In Y
        bpl.s 	dec2
        move.w 	Pkdy(a0),d2
dec2:   move.w	Pktx(a0),d0
	move.w	d0,USx(sp)		* Taille en X
	add.w	d1,d0
	cmp.w	d7,d0
	bhi	NoPac0
	move.w	Pkty(a0),d0
	mulu	d6,d0
	move.w	d0,USy(sp)		* Taille en Y
	add.w	d2,d0
	cmp.w	EcTy(a1),d0
	bhi	NoPac0

	mulu	d7,d2			* Screen address
	ext.l	d1
	add.l	d2,d1
	move.l	d1,UDEc(sp)

	move.w	d6,d0			* Size of one line
        mulu 	d7,d0
        move 	d0,UTLine(sp)

        move.w 	Pktx(a0),a3		* Size in X
        subq.w	#1,a3
        move.w 	Pkty(a0),UITy(sp)	* in Y
        lea 	PkDatas1(a0),a4        	* a4--> bytes table 1
        move.l 	a0,a5
        move.l 	a0,a6
        add.l 	PkDatas2(a0),a5     	* a5--> bytes table 2
        add.l 	PkPoint2(a0),a6     	* a6--> pointer table

        moveq 	#7,d0
        moveq 	#7,d1
        move.b 	(a5)+,d2
        move.b 	(a4)+,d3
        btst 	d1,(a6)
        beq.s 	prep
        move.b 	(a5)+,d2
prep:   subq.w 	#1,d1

* Unpack!
dplan:  move.l 	UAEc(sp),a2
	addq.l	#4,UAEc(sp)
	move.l	(a2),a2
	add.l	UDEc(sp),a2
        move.w 	UITy(sp),UTy(sp)	* Y Heigth counter
dligne: move.l 	a2,a1
        move.w 	a3,d4
dcarre: move.l 	a1,a0
        move.w 	d6,d5   		* Square height
doctet1:subq.w 	#1,d5
        bmi.s 	doct3
        btst 	d0,d2
        beq.s 	doct1
        move.b 	(a4)+,d3
doct1:  move.b 	d3,(a0)
        add.w 	d7,a0
        dbra 	d0,doctet1
        moveq 	#7,d0
        btst 	d1,(a6)
        beq.s 	doct2
        move.b 	(a5)+,d2
doct2:  dbra 	d1,doctet1
        moveq 	#7,d1
        addq.l 	#1,a6
        bra.s 	doctet1
doct3:  addq.l	#1,a1           	* Other squares?
        dbra 	d4,dcarre
        add.w 	UTLine(sp),a2          	* Other square line?
        subq.w 	#1,UTy(sp)
        bne.s 	dligne
        subq.w 	#1,UNPlan(sp)
        bne.s 	dplan
* Finished!
	move.w	USy(sp),d0
	swap	d0
	move.w	USx(sp),d0
	lsl.w	#3,d0
        lea	UPile(sp),sp            * Restore the pile
	movem.l	(sp)+,a0-a6/d1-d7
	rts
NoPac0 	lea	UPile(sp),sp
NoPac	moveq	#0,d0
	movem.l	(sp)+,a0-a6/d1-d7
	rts

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
