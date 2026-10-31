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
; .200000............2002........................| AMOSPro Version 2
; .200002........................................| Loader general
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
;
		Include "+AMOS_Includes.s"
		Include "+Version.s"
; ______________________________________________________________________________


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; The boot, the libraries' loader, the end and the system routines are
; C++ (src/main). Left here: the entry, the stack of its own and the way
; out (they move the stack), the installation's data and the tables of
; the routines AMOSPro gives the libraries.

		XREF	_amos_task
		XREF	_amos_bridge

; CCall of src/glue.i (which cannot be included with the library's macros)
BCall		MACRO
		move.l	$4.w,a0
		move.l	276(a0),-(sp)		ThisTask
		move.l	_amos_bridge,a0
		cmpm.l	(a0)+,(sp)+
		beq.s	.Main\@
		lea	24(a0),a0
.Main\@	movem.l	d6-d7/a3-a6,(a0)
		jsr	\1
		move.l	$4.w,a0
		move.l	276(a0),-(sp)
		move.l	_amos_bridge,a0
		cmpm.l	(a0)+,(sp)+
		beq.s	.Back\@
		lea	24(a0),a0
.Back\@	movem.l	(a0),d6-d7/a3-a6
		ENDM
		XREF	_main_cold_start,_main_init_fin,_main_the_end
		XREF	Edit_Load,Edit_Free,Mon_Load,Mon_Free,RamFast,RamFast2,RamChip,RamChip2
		XREF	RamFree,ResTempBuffer,Math_Close,Sys_WaitMul,Def_GetMessage,Sys_GetMessage
		XREF	GetMessage,Sys_AddPath,Sys_UnCode,MemMaximum,MemDelBanks,Sys_ClearCache
		XREF	WOption,ReCop,AskDir,AskDir2,InCloseEditor,InKillEditor,InMonitor,Bnk.Change
		XREF	D_Open,D_OpenD1,D_Close,D_Read,D_Write,D_Seek

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	FIN DE L'INITIALISATION : ENLEVE LE HUNK INIT!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	SECTION	"E",CODE
; - - - - - - - - - - - - -

Hunk_1	jmp	Cold_Start

Init_Fin
; Enleve le hunk init
; ~~~~~~~~~~~~~~~~~~~
	lea	Hunk_1-8(pc),a0
; The program to run, or the editor (main/boot.cpp)

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
;	SORTIE GENERALE (main/boot.cpp)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TheEnd	BCall	_main_the_end

; void main_get_out(message): the way out, the Workbench message (if any)
; replied
		xdef	_main_get_out
_main_get_out
	move.l	4(sp),d7
Get_Out	move.l	SaveSp(pc),a7
; The CLI's stack back, AMOSPro's freed
.OldStack
	tst.l	d7
.PaMe	moveq	#0,d0


; Ne pas changer la position du nom, apres dos.library!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		even
		dc.l	UserSecu-DosName
DosName		dc.b 	"dos.library",0
UserReg
_main_user_reg	dc.w	14
		dc.b	"R"^$73		0
		dc.b	"E"^$73		1
		dc.b	"G"^$73		2
		dc.b	"I"^$73		3
		dc.b	"S"^$73		4
		dc.b	"T"^$73		5
		dc.b	"R"^$73		6
		dc.b	"A"^$73		7
		dc.b	"T"^$73		8
		dc.b	"I"^$73		9
		dc.b	"O"^$73		10
		dc.b	"N"^$73		11
		dc.b	" "^$73		12
		dc.b	"#"^$73		13
UserName
_main_user_name	dc.w	14
		dc.b	"N"^$A5		0
		dc.b	"o"^$A5		1
		dc.b	"t"^$A5		2
		dc.b	" "^$A5		3
		dc.b	"I"^$A5		4
		dc.b	"n"^$A5		5
		dc.b	"s"^$A5		6
		dc.b	"t"^$A5		7
		dc.b	"a"^$A5		8
		dc.b	"l"^$A5		9
		dc.b	"l"^$A5		10
		dc.b	"e"^$A5		11
		dc.b	"d"^$A5		12
		dc.b	"!"^$A5		13
		ds.b	32-14+4
		even

		xdef	_main_user_reg,_main_user_name

******************************************************************
		dc.b	$17,$09,$19,$92
UserSecu	ds.b	34
******************************************************************

SaveSp		dc.l 	0
OwnStack_Size	equ	64*1024
OwnStack	dc.l	0,0,0			StackSwapStruct
OwnStack_A0	dc.l	0
OwnStack_D0	dc.l	0
OwnStack_On	dc.w	0

;		Marqueur de version
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
VersionN:	dc.b	" Version "
		Version
		dc.b	0,"$VER: "
		Version


;	IFNE	Debug
PreBug 	btst	#6,$BFE001
BugBug	movem.l	d0-d2/a0-a2,-(sp)
.Ll	move.w	#$FF0,$DFF180
.L0	move.w	#10000,d1
.L1	move.w	d0,$DFF180
.Ill	EcCalD	AMOS_WB,0
;	ENDC


; Inclusion de la verification
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Include	"+Verif.s"

; Charge le moniteur et l'editeur en mode debug
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	IFNE	Debug=2
Mon_Debug	Include	"+Monitor.s"
Edit_Debug	Include "+Edit.s"
	ENDC
	IFEQ	Debug=2
Mon_Debug
Edit_Debug
	ENDC


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	CHARGEMENT / INITIALISATION - Partie amovible!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	SECTION	"I",CODE

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	CHARGEMENT / INITIALISATION - Partie amovible!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	SECTION	"I",CODE
; - - - - - - - - - - - - -
Hunk_2
Cold_Start
; - - - - - - - - - - - - -
	move.l	sp,SaveSp

; A stack of its own: the C++ needs more than a CLI's default 4K (it
; overflowed into the memory below). Given back in Get_Out.
.NoStack
	move.l	OwnStack_A0,a0

; The main task, for the C++ code (src/core/call.s)

; The start (main/boot.cpp), with the command line and the stack of the
; interpreter (BasSp)

		even

;		Adresses des routines accessibles aux extensions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_main_amos_jmps,_main_ext_jmps
_main_amos_jmps
AMOSJmps	Include	"+Internal_Jumps.s"
; The extensions': slot, list
_main_ext_jmps
ExtJmps		dc.w	1
		dc.l	MusicJmps
		dc.w	2
		dc.l	CompactJmps
		dc.w	3
		dc.l	RequestJmps
		dc.w	6
		dc.l	IOPortsJmps
		dc.w	0
MusicJmps	Include	"+Music_Jumps.s"
CompactJmps	Include	"+Compact_Jumps.s"
RequestJmps	Include	"+Request_Jumps.s"
IOPortsJmps	Include	"+IO_Ports_Jumps.s"

