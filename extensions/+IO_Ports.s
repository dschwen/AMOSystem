
;---------------------------------------------------------------------
;    **   **   **  ***   ***   ****     **    ***  **  ****
;   ****  *** *** ** ** **     ** **   ****  **    ** **  **
;  **  ** ** * ** ** **  ***   *****  **  **  ***  ** **
;  ****** **   ** ** **    **  **  ** ******    ** ** **
;  **  ** **   ** ** ** *  **  **  ** **  ** *  ** ** **  **
;  **  ** **   **  ***   ***   *****  **  **  ***  **  ****
;---------------------------------------------------------------------
; Serial / Parallel / Printer extension source code
; By Fran�ois Lionet
; AMOS, AMOSPro, AMOS Compiler (c) Europress Software 1990-1992
; To be used with AMOSPro V2.0 and over
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
;
; Please refer to the _Music.s file for more informations...
;
;---------------------------------------------------------------------

ExtNb		equ	6-1

;		AMIGA's includes
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		INCDIR		"includes/"
		INCLUDE		"Exec/Types.I"
		INCLUDE		"Exec/Exec.I"
		INCLUDE 	"Devices/Serial.I"
		INCLUDE		"Devices/PrtBase.I"
		INCLUDE		"Devices/Printer.I"
		INCLUDE		"Devices/PrtGfx.I"
		INCLUDE		"Devices/Parallel.I"

;---------------------------------------------------------------------
;		Include the files automatically calculated by
;		Library_Digest.AMOS
;---------------------------------------------------------------------
 		Include	"+AMOS_Includes.s"
		Include	"+Version.s"
		Include	"+IO_Ports_Size.s"
		Include	"+IO_Ports_Labels.s"

; 		Number of serial channels allowed
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NSerial		equ	4


Dlea		MACRO
		move.l	ExtAdr+ExtNb*16(a5),\2
		add.w	#\1-DT,\2
		ENDM
Dload		MACRO
		move.l	ExtAdr+ExtNb*16(a5),\1
		ENDM


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

;---------------------------------------------------------------------
;		TOKEN TABLE
;---------------------------------------------------------------------
;		TOKEN_START

C_Tk
		dc.w 	1,0
		dc.b 	$80,-1
		dc.w	L_InSerialOpen2,L_Nul
		dc.b	"!serial ope","n"+$80,"I0,0",-2
		dc.w	L_InSerialOpen5,L_Nul
		dc.b	$80,"I0,0,0,0,0",-1
		dc.w	L_InSerialClose0,L_Nul
		dc.b	"!serial clos","e"+$80,"I",-2
		dc.w	L_InSerialClose1,L_Nul
		dc.b	$80,"I0",-1
		dc.w	L_InSerialSpeed,L_Nul
		dc.b	"serial spee","d"+$80,"I0,0",-1
		dc.w	L_Nul,L_FnSerialCheck
		dc.b	"serial chec","k"+$80,"00",-1
		dc.w 	L_InSerialSend,L_Nul
		dc.b	"serial sen","d"+$80,"I0,2",-1
		dc.w 	L_InSerialSpeed,L_Nul
		dc.b	"serial spee","d"+$80,"I0,0",-1
		dc.w 	L_InSerialBits,L_Nul
		dc.b	"serial bit","s"+$80,"I0,0,0",-1
		dc.w 	L_InSerialX,L_Nul
		dc.b	"serial ","x"+$80,"I0,0",-1
		dc.w 	L_InSerialBuf,L_Nul
		dc.b	"serial bu","f"+$80,"I0,0",-1
		dc.w 	L_InSerialParity,L_Nul
		dc.b	"serial parit","y"+$80,"I0,0",-1
		dc.w	L_Nul,L_FnSerialGet
		dc.b	"serial ge","t"+$80,"00",-1
		dc.w	L_Nul,L_FnSerialInput
		dc.b	"serial input","$"+$80,"20",-1
		dc.w	L_InSerialFast,L_Nul
		dc.b	"serial fas","t"+$80,"I0",-1
		dc.w	L_InSerialSlow,L_Nul
		dc.b	"serial slo","w"+$80,"I0",-1
		dc.w	L_Nul,L_FnSerialError
		dc.b	"serial erro","r"+$80,"00",-1
		dc.w	L_InSerialOut,L_Nul
		dc.b	"serial ou","t"+$80,"I0,0,0",-1
		dc.w	L_Nul,L_FnSerialStatus
		dc.b	"serial statu","s"+$80,"00",-1
		dc.w	L_Nul,L_FnSerialBase
		dc.b	"serial bas","e"+$80,"00",-1
		dc.w	L_InSerialAbort,L_Nul
		dc.b	"serial abor","t"+$80,"I0",-1

; Printer commands
; ~~~~~~~~~~~~~~~~
		dc.w   	L_InPrinterOpen,L_Nul
		dc.b	"printer ope","n"+$80,"I",-1
		dc.w	L_InPrinterClose,L_Nul
		dc.b	"printer clos","e"+$80,"I",-1
		dc.w	L_InPrinterSend,L_Nul
		dc.b	"printer sen","d"+$80,"I2",-1
		dc.w	L_InPrinterOut,L_Nul
		dc.b	"printer ou","t"+$80,"I0,0",-1
		dc.w	L_InPrinterDump0,L_Nul
		dc.b	"!printer dum","p"+$80,"I",-2
		dc.w	L_InPrinterDump4,L_Nul
		dc.b	$80,"I0,0t0,0",-2
		dc.w	L_InPrinterDump7,L_Nul
		dc.b	$80,"I0,0t0,0,0,0,0",-1
		dc.w	L_InPrinterAbort,L_Nul
		dc.b	"printer abor","t"+$80,"I",-1
		dc.w	L_Nul,L_FnPrinterCheck
		dc.b 	"printer chec","k"+$80,"0",-1
		dc.w	L_Nul,L_FnPrinterOnline
		dc.b	"printer onlin","e"+$80,"0",-1
		dc.w	L_Nul,L_FnPrinterBase
		dc.b	"printer bas","e"+$80,"0",-1
		dc.w	L_Nul,L_FnPrinterError
		dc.b	"printer erro","r"+$80,"0",-1

; Parallel commands
; ~~~~~~~~~~~~~~~~~
		dc.w   	L_InParallelOpen,L_Nul
		dc.b	"parallel ope","n"+$80,"I",-1
		dc.w	L_InParallelClose,L_Nul
		dc.b	"parallel clos","e"+$80,"I",-1
		dc.w	L_InParallelSend,L_Nul
		dc.b	"parallel sen","d"+$80,"I2",-1
		dc.w	L_InParallelOut,L_Nul
		dc.b	"parallel ou","t"+$80,"I0,0",-1
		dc.w	L_InParallelAbort,L_Nul
		dc.b	"parallel abor","t"+$80,"I",-1
		dc.w	L_Nul,L_FnParallelCheck
		dc.b 	"parallel chec","k"+$80,"0",-1
		dc.w	L_Nul,L_FnParallelStatus
		dc.b	"parallel statu","s"+$80,"0",-1
		dc.w	L_Nul,L_FnParallelBase
		dc.b	"parallel bas","e"+$80,"0",-1
		dc.w	L_Nul,L_FnParallelError
		dc.b	"parallel erro","r"+$80,"0",-1
		dc.w	L_Nul,L_FnParallelInput1
		dc.b	"!parallel input","$"+$80,"20",-2
		dc.w	L_Nul,L_FnParallelInput2
		dc.b	$80,"20,0",-2
;		TOKEN_END
		dc.w 	0
		dc.l	0			Important!


;---------------------------------------------------------------------
	Lib_Ini	0
;---------------------------------------------------------------------

C_Lib

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	COLD START
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Int	IOCold		src/ext/ioports.cpp

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Leave one empty routine here!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_Empty
; - - - - - - - - - - - - -

; The routines are in AMOSPro: src/ext/ioports.cpp
	Lib_Int	InSerialOpen2
	Lib_Int	InSerialOpen5
	Lib_Int	InSerialClose1
	Lib_Int	InSerialClose0
	Lib_Int	InSerialSend
	Lib_Int	InSerialOut
	Lib_Int	FnSerialGet
	Lib_Int	FnSerialInput
	Lib_Int	InSerialSpeed
	Lib_Int	InSerialBits
	Lib_Int	InSerialParity
	Lib_Int	InSerialX
	Lib_Int	InSerialBuf
	Lib_Int	InSerialFast
	Lib_Int	InSerialSlow
	Lib_Int	FnSerialCheck
	Lib_Int	FnSerialError
	Lib_Int	FnSerialStatus
	Lib_Int	FnSerialBase
	Lib_Int	InSerialAbort
	Lib_Empty			GetSerA1
	Lib_Empty			GetSerial
	Lib_Empty			Stpar
	Lib_Int	FnPrinterBase
	Lib_Int	FnPrinterCheck
	Lib_Int	InPrinterOpen
	Lib_Int	InPrinterClose
	Lib_Int	FnPrinterError
	Lib_Int	InPrinterSend
	Lib_Int	InPrinterOut
	Lib_Int	InPrinterAbort
	Lib_Int	FnPrinterOnline
	Lib_Int	InPrinterDump0
	Lib_Int	InPrinterDump4
	Lib_Int	InPrinterDump7
	Lib_Empty			Dump3a
	Lib_Empty			Dump2a
	Lib_Empty			Dump
	Lib_Empty			GetScr
	Lib_Int	FnParallelBase
	Lib_Int	FnParallelCheck
	Lib_Int	InParallelOpen
	Lib_Int	InParallelClose
	Lib_Int	InParallelSend
	Lib_Int	InParallelOut
	Lib_Int	InParallelAbort
	Lib_Int	FnParallelStatus
	Lib_Int	FnParallelError
	Lib_Int	FnParallelInput1
	Lib_Int	FnParallelInput2
	Lib_Empty			ParInput
	Lib_Empty			IOFonc

	Lib_Empty
	Lib_Empty
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Finish the library
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	Lib_End
; - - - - - - - - - - - - -

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 		TITLE OF THE EXTENSION
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C_Title	dc.b	"AMOSPro IO Devices Extension V "
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
