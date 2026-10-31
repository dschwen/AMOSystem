;---------------------------------------------------------------------
;    **   **   **  ***   ***   ****     **    ***  **  ****
;   ****  *** *** ** ** **     ** **   ****  **    ** **  **
;  **  ** ** * ** ** **  ***   *****  **  **  ***  ** **
;  ****** **   ** ** **    **  **  ** ******    ** ** **
;  **  ** **   ** ** ** *  **  **  ** **  ** *  ** ** **  **
;  **  ** **   **  ***   ***   *****  **  **  ***  **  ****
;---------------------------------------------------------------------
; AMOSPro Picture compactor extension source code,
; By Fran�ois Lionet
; AMOS, AMOSPro and AMOS Compiler (c) Europress Software 1990-1992
; To be used with AMOS1.3 and over
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
; Please refer to the _Music.s file for more informations
;---------------------------------------------------------------------

ExtNb		equ	2-1

;---------------------------------------------------------------------
;		Include the files automatically calculated by
;		Library_Digest.AMOS
;---------------------------------------------------------------------
		Include	"+Compact_Size.s"
		Include	"+Compact_Labels.s"
 		Include	"+AMOS_Includes.s"
		Include	"+Version.s"
Start		dc.l	C_Tk-C_Off
		dc.l	C_Lib-C_Tk
		dc.l	C_Title-C_Lib
		dc.l	C_End-C_Title
		dc.w	0
		dc.b	"AP20"

;---------------------------------------------------------------------
;		Creates the pointers to functions
;---------------------------------------------------------------------
		MCInit
C_Off
		REPT	Lib_Size
		MC
		ENDR

***********************************************************
* 		COMPACTOR TOKENS

;		TOKEN_START
C_Tk		dc.w 	1,0
		dc.b 	$80,-1
		dc.w 	L_InPack2,L_Nul
		dc.b 	"!pac","k"+$80,"I0t0",-2
		dc.w	L_InPack6,L_Nul
		dc.b	$80,"I0t0,0,0,0,0",-1
		dc.w 	L_InSPack2,L_Nul
		dc.b 	"!spac","k"+$80,"I0t0",-2
		dc.w	L_InSPack6,L_Nul
		dc.b	$80,"I0t0,0,0,0,0",-1
		dc.w	L_InUnpack1,L_Nul
		dc.b	"!unpac","k"+$80,"I0",-2
		dc.w 	L_InUnpack2,L_Nul
		dc.b	$80,"I0t0",-2
		dc.w 	L_InUnpack3,L_Nul
		dc.b	$80,"I0,0,0",-1
;		TOKEN_END
		dc.w 	0
		dc.l	0			Important!


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	THE LIBRARY
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

;---------------------------------------------------------------------
	Lib_Ini	0
;---------------------------------------------------------------------

C_Lib

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	COLD START
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Compact_Cold		src/ext/compact.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ONE EMPTY SPACE
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	PACK Screen,Bank#
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; The routines are in AMOSPro: src/ext/compact.cpp
	Lib_Int	InPack2
	Lib_Int	InPack6
	Lib_Int	InSPack2
	Lib_Int	InSPack6
	Lib_Int	InUnpack1
	Lib_Int	InUnpack3
	Lib_Empty			UPack
	Lib_Int	InUnpack2
	Lib_Empty			ResBank
	Lib_Empty			PacPar
	Lib_Empty			GetSize
	Lib_Empty			Pack
	Lib_Empty			JFoncall
	Lib_Empty			JScnop
	Lib_Empty			JOOfMem
	Lib_Empty			NoPac
	Lib_Empty			NoScr



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ERRORS: First routine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Custom			src/ext/compact.cpp
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	ERRORS: Second routine
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	Custom2			src/ext/compact.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Finish the library
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_End
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	TITLE OF THE EXTENSION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C_Title		dc.b 	"AMOSPro Picture Compactor V "
		Version
		dc.b	0,"$VER: "
		Version
		dc.b	0
		Even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;		END OF THE EXTENSION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C_End		dc.w	0
		even
