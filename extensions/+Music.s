
;---------------------------------------------------------------------
;    **   **   **  ***   ***   ****     **    ***  **  ****
;   ****  *** *** ** ** **     ** **   ****  **    ** **  **
;  **  ** ** * ** ** **  ***   *****  **  **  ***  ** **
;  ****** **   ** ** **    **  **  ** ******    ** ** **
;  **  ** **   ** ** ** *  **  **  ** **  ** *  ** ** **  **
;  **  ** **   **  ***   ***   *****  **  **  ***  **  ****
;---------------------------------------------------------------------
; Music extension source code for AMOSPro, Last change 20/07/1993
; By Francois Lionet
; AMOS, AMOSPro AMOS Compiler (c) Europress Software 1990-1993
; To be used with AMOSPro 2.0 and over
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
; NOTE: every chapter is preceded by "+++", so just look for this sign
; with your editor to go from one important paragraph to another
;
; +++
; This listing explains how to create an extension for AMOSPro V2.0
; The format of the extension have changed between V1.x and V2.x,
; but V2.x still accepts old-format extensions.
; So if you do not want to rewrite your code, just don't, it will work
; with the interpreter and the compiler.
;
; Several advantages of rewriting your code:
;	- Faster under both interpreter and compiler
;	- Reserved variables can be created in extensions
;	- Mathematical functions can be created in extensions
;	- You can call any AMOSPro function using special macros
;	  from within your extension.
;
; Drawbacks:
;	- Your function have to save D6/D7 registers
;
; To know how to rewrite your code, just read the text at the end of this
; file. Just know that this music extension has been turned from the old
; format (V1.x) to the new one, in one afternoon. You just need a good
; editor to perform the job. All the rest of the text only explain how to
; write a V2.x extension.
;
; --------------------------------------------------------------------------
;
; +++ What's an extension?
;
; An extension to AMOS is a machine language program that adds new
; instructions to the already huge AMOS instruction set. This system is
; designed to be as powerfull as AMOS itself: the extension includes its
; own token list, its own routines. It can even access some main AMOS
; routines via special macros. It has a total access to the internal AMOS
; data zone, and to the graphic library functions.
;
; To produce your own extension, I suggest you copy and rename this
; file, and remove the used code. This way you will not forgive one line.
; Also keep in mind that you can perfectly call AMOS from within MONAM2,
; and set some ILLEGAL instructions where you want to debug. To flip back to
; MONAM2 display, just press AMIGA-A.
;
; I have designed the extension system so that one only file works with
; both AMOS interpretor and compiler.
; 	- The extension is more a compiler library than a one chunk program:
;	it is done so that the compiler can pick one routine here and there
;	to cope with the program it is compiling.
;	- AMOSPro extension loader works a little like the compiler, exept
;	that all instructions are loaded and relocated.
;	- The AMOSPro (V2.x) instruction set is in fact a big extension,
;	called "AMOSPro.Lib", in the APSystem folder
;
; This code was assembled with GENAM3 on a A3000 25 Mhz machine, but a
; A500 can do it very well!
; The assembled program must be ONE CHUNK only, you must not link the
; the symbol table with it. Also be sure that you program is totally
; relocatable (see later) : if not it will add a relocation chunk to
; the output code, and your extension will simply crash AMOS on loading
; (and the compiler too!).
;
; +++
; Before assembling your code, you must run one AMOSro Compiled program that
; explores your source code, and creates the label table for the assembler.
; This program is called "Library_Digest". Here is my script file to assemble
; the music extension :
;
; Library_Digest +Music.s
; Genam FROM +Music.s TO APro:APSystem/AMOSPro_Music.Lib
;
; Library_Digest loads +music.s and creates two ascii files:
;	+Music_Size.s contains the number of functions in the extension
;	+Music_Labels.s contains the list of labels and their value.
;
; Here we go now!
;
; +++
; Here comes the number of the extension in the list of extensions in
; AMOSPro_Interpretor_Config program (minus one).
; This number is used later to reference the extension in internal AMOS
; tables...
;
ExtNb		equ	1-1

;---------------------------------------------------------------------
;		+++
;		Include the files automatically calculated by
;		Library_Digest.AMOS
;---------------------------------------------------------------------
		Include	"+Music_Size.s"
		Include	"+Music_Labels.s"

; +++ You must include this file, it will decalre everything for you.
		Include	"+AMOS_Includes.s"

; +++ This one is only for the current version number.
		Include	"+Version.s"

; A usefull macro to find the address of data in the extension's own
; datazone (see later)...
Dlea		MACRO
		move.l	ExtAdr+ExtNb*16(a5),\2
		add.w	#\1-MB,\2
		ENDM

; Another macro to load the base address of the datazone...
Dload		MACRO
		move.l	ExtAdr+ExtNb*16(a5),\1
		ENDM

; ---------------------------------------------------------------------------
; The data of the extension is described in +Music_Data.s, for its C++.

; +++
; All the above did not produce any byte of code. Here is the real beginning
; of the program. It MUST begin by a small table of pointers so that both
; AMOS and the compiler know where to get their data...
; Please remark that everything is relocatable...

; +++ First, a pointer to the token list
Start	dc.l	C_Tk-C_Off
;
; +++ Then, a pointer to the first library function
	dc.l	C_Lib-C_Tk
;
; +++ Then to the title
	dc.l	C_Title-C_Lib
;
; +++ From title to the end of the program
	dc.l	C_End-C_Title
; +++
; An important flag. Imagine a program does not call your extension, the
; compiler will NOT copy any routine from it in the object program. For
; certain extensions, like MUSIC, COMPACT, it is perfect.
; But for the REQUEST extension, even if it is not called, the first routine
; MUST be called, otherwise AMOS requester will not work!
; So, a value of 0 indicates to copy if needed only,
; A value of -1 forces the copy of the first library routine...
	dc.w	0

; +++ This magic code tells AMOSPro that this extensions uses the new format
	dc.b	"AP20"


;---------------------------------------------------------------------
;	+++ TABLE OF POINTERS TO THE LIBRARY
;
;	The following macros automatically create the necessary pointers
; to the library. It uses the informations created by "Library_Digest"
; You are free to add/remove a function in the middle of the extension
; without having to care about the numbers of the function.
;---------------------------------------------------------------------
		MCInit
C_Off
		REPT	Lib_Size
		MC
		ENDR


;---------------------------------------------------------------------
; 	+++ TOKEN TABLE
;
;
; This table is the crucial point of the extension! It tells
; everything the tokenisation process needs to know. You have to
; be carefull when writing it!
;
; The format is simple:
;	dc.w	Number of instruction,Number of function
;	dc.b	"instruction nam","e"+$80,"Param list",-1[or -2]
;
;	(1) Number of instruction / function
;		You must state the one that is needed for this token.
;               I suggest you keep the same method of referencing the
;		routines than mine: L_name, this label being defined
;		in the main program.
;		A -1 means take no routine is called (example a
;		instruction only will have a -1 in the function space...)
;
;	(2) Instruction name
;		It must be finished by the letter plus $80.
;		- You can SET A MARK in the token table with a "!" before
;		the name. See later
;		-Using a $80 ALONE as a name definition, will force AMOS
;		to point to the previous "!" mark...
;
;	(3) Param list
;		This list tells AMOS everything about the instruction.
;
;	- First character:
;		The first character defines the TYPE on instruction:
;			I--> instruction
;			0--> function that returns a integer
;			1--> function that returns a float
;			2--> function that returns a string
;			V--> reserved variable. In that case, you must
;				state the type int-float-string
;	- If your instruction does not need parameters, then you stop
;	- Your instruction needs parameters, now comes the param list
;			Type,TypetType,Type...
;		Type of the parameter:
;			0--> integer
;			1--> float or double
;			2--> string
;			3--> integer OR string. The only way to check the type
;			     is to check the adress (UGLY, but safe for integer
;			     up to 512 which cannot ever be a string's address)
;			4--> Integer OR float/double. You must then write TWO
;			     routines. The first one being called when the param
;			     is an integer, and being the one pointed to by the
;                            token table. The second one being called when the
;                            parameter is a float/double.
;			     This system is used for functions like SGN.
;			5--> Angle. This parameter will always be a float/double,
;			     in radians, even if DEGREE has been set.
;		Comma or "t" for TO
;
;	(4) End of instruction
;			"-1" states the end of the instruction
;			"-2" tells AMOS that another parameter list
;			     can be accepted. if so, MUST follow the
;			     complete instruction definition as explained
;			     but with another param list.
;	If so, you can use the "!" and $80 facility not to rewrite the
;	full name of the instruction...See SAM LOOP ON instruction for an
;	example...
;
;
;	+++ You _MUST_ leave this keyword in the source, in upper case:
;	Library_Digest uses it to detect the start of the library.
;
;	TOKEN_START

; 	+++ The next two lines needs to be unchanged...
C_Tk:	dc.w 	1,0
	dc.b 	$80,-1

; Now the real tokens...
	dc.w	L_Nul,L_FnMusicBase
	dc.b	"mubas","e"+$80,"0",-1
	dc.w	L_Nul,L_FnVuMeter
	dc.b	"vumete","r"+$80,"00",-1
	dc.w	L_InVoice,L_Nul
	dc.b	"voic","e"+$80,"I0",-1
	dc.w	L_InMusicOff,L_Nul
	dc.b	"music of","f"+$80,"I",-1
	dc.w	L_InMusicStop,L_Nul
	dc.b	"music sto","p"+$80,"I",-1
	dc.w	L_InTempo,L_Nul
	dc.b	"temp","o"+$80,"I0",-1
	dc.w	L_InMusic,L_Nul
	dc.b	"musi","c"+$80,"I0",-1

	dc.w	L_InNoiseTo,L_Nul
	dc.b	"noise t","o"+$80,"I0",-1
	dc.w	L_InBoom,L_Nul
	dc.b 	"boo","m"+$80,"I",-1
	dc.w	L_InShoot,L_Nul
	dc.b	"shoo","t"+$80,"I",-1
	dc.w	L_InSamBank,L_Nul
	dc.b	"sam ban","k"+$80,"I0",-1
	dc.w	L_InSamLoopOn0,L_Nul
	dc.b	"!sam loop o","n"+$80,"I",-2
	dc.w	L_InSamLoopOn1,L_Nul
	dc.b	$80,"I",-1
	dc.w	L_InSamLoopOff0,L_Nul
	dc.b	"sam loop of","f"+$80,"I",-2
	dc.w	L_InSamLoopOff1,L_Nul
	dc.b	$80,"I0",-1
	dc.w	L_InSampleTo,L_Nul
	dc.b	"sampl","e"+$80,"I0t0",-1
	dc.w 	L_InSamPlay1,L_Nul
	dc.b 	"!sam pla","y"+$80,"I0",-2
	dc.w	L_InSamPlay2,L_Nul
	dc.b	$80,"I0,0",-2
	dc.w 	L_InSamPlay3,L_Nul
	dc.b	$80,"I0,0,0",-1
	dc.w 	L_InSamRaw,L_Nul
	dc.b 	"sam ra","w"+$80,"I0,0,0,0",-1
	dc.w	L_InBell0,L_Nul
	dc.b 	"!bel","l"+$80,"I",-2
	dc.w 	L_InBell1,L_Nul
	dc.b	$80,"I0",-1
	dc.w	L_InPlayOff0,L_Nul
	dc.b	"!play of","f"+$80,"I",-2
	dc.w	L_InPlayOff1,L_Nul
	dc.b	$80,"I0",-1
	dc.w 	L_InPlay2,L_Nul
	dc.b	"!pla","y"+$80,"I0,0",-2
	dc.w	L_InPlay3,L_Nul
	dc.b	$80,"I0,0,0",-1
	dc.w 	L_InSetWave,L_Nul
	dc.b 	"set wav","e"+$80,"I0,2",-1
	dc.w	L_InDelWave,L_Nul
	dc.b	"del wav","e"+$80,"I0",-1
	dc.w	L_InSetEnvel,L_Nul
	dc.b	"set enve","l"+$80,"I0,0t0,0",-1
	dc.w	L_InMvolume,L_Nul
	dc.b	"mvolum","e"+$80,"I0",-1
	dc.w 	L_InVolume1,L_Nul
	dc.b	"!volum","e"+$80,"I0",-2
	dc.w 	L_InVolume2,L_Nul
	dc.b	$80,"I0,0",-1
	dc.w	L_InWave,L_Nul
	dc.b 	"wav","e"+$80,"I0t0",-1
	dc.w	L_InLedOn,L_Nul
	dc.b	"led o","n"+$80,"I",-1
	dc.w	L_InLedOf,L_Nul
	dc.b	"led of","f"+$80,"I",-1
	dc.w	L_InSay1,L_Nul
	dc.b	"!sa","y"+$80,"I2",-2
	dc.w	L_InSay2,L_Nul
	dc.b	$80,"I2,0",-1
	dc.w	L_InSetTalk,L_Nul
	dc.b	"set tal","k"+$80,"I0,0,0,0",-1
	dc.w	L_InSload,L_Nul
	dc.b	"sloa","d"+$80,"I0t0,0",-1
	dc.w	L_Nul,L_FnSamSwapped
	dc.b	"sam swappe","d"+$80,"00",-1
	dc.w	L_InSamSwap,L_Nul
	dc.b	"sam swa","p"+$80,"I0t0,0",-1
	dc.w 	L_InSamStop0,L_Nul
	dc.b	"!sam sto","p"+$80,"I",-2
	dc.w	L_InSamStop1,L_Nul
	dc.b	$80,"I0",-1

	dc.w	L_InTrackStop,L_Nul
	dc.b	"track sto","p"+$80,"I",-1
	dc.w	L_InTrackLoopOn,L_Nul
	dc.b	"track loop o","n"+$80,"I",-1
	dc.w	L_InTrackLoopOff,L_Nul
	dc.b	"track loop o","f"+$80,"I",-1
	dc.w	L_InTrackPlay0,L_Nul
	dc.b	"!track pla","y"+$80,"I",-2
	dc.w	L_InTrackPlay1,L_Nul
	dc.b	$80,"I0",-2
	dc.w	L_InTrackPlay2,L_Nul
	dc.b	$80,"I0,0",-1
	dc.w	L_InTrackLoad,L_Nul
	dc.b	"track loa","d"+$80,"I2,0",-1

	dc.w	L_Nul,L_FnMouthWidth
	dc.b	"mouth widt","h"+$80,"0",-1
	dc.w	L_Nul,L_FnMouthHeight
	dc.b	"mouth heigh","t"+$80,"0",-1
	dc.w	L_InMouthRead,L_Nul
	dc.b	"mouth rea","d"+$80,"I",-1
	dc.w	L_InTalkStop,L_Nul
	dc.b	"talk sto","p"+$80,"I",-1
	dc.w	L_InTalkMisc,L_Nul
	dc.b	"talk mis","c"+$80,"I0,0",-1

	dc.w	L_InSsave,L_Nul
	dc.b	"ssav","e"+$80,"I0,0t0",-1

	dc.w	L_InMedLoad,L_Nul
	dc.b	"med loa","d"+$80,"I2,0",-1
	dc.w	L_InMedPlay0,L_Nul
	dc.b	"!med pla","y"+$80,"I",-2
	dc.w	L_InMedPlay1,L_Nul
	dc.b	$80,"I0",-2
	dc.w	L_InMedPlay2,L_Nul
	dc.b	$80,"I0,0",-1
	dc.w	L_InMedStop,L_Nul
	dc.b	"med sto","p"+$80,"I",-1
	dc.w	L_InMedCont,L_Nul
	dc.b	"med con","t"+$80,"I",-1
	dc.w	L_InMedMidiOn,L_Nul
	dc.b	"med midi o","n"+$80,"I",-1

;	+++ You must also leave this keyword untouched, just before the zeros.
;	TOKEN_END

;	+++ The token table must end by this
	dc.w 	0
	dc.l	0


;
; Now come the big part, the library.
;
; The beginning of each routine is defined with macros. NB: in the following text
; a space in inserted in the macro name, so that it is not detected by
; "Library_Digest".
;
;	Lib_ Def	Function_Name_No_Parameter
; or
;	Lib_ Par Function_Name_With_Parameter
;
; Those two macro have the same function:
;	- Create an entry in the library offset table, so that AMOSPro locates
;	the function,
;	- Are detected by Library_Digest: it then creates a label in the "_Label.s"
;	file (see above) of the following form, by appending a "L_" to the name:
;	"L_Function_Name_..." This label must be used in the extension to reference
;	the routine.
;
; Differences between Lib_ Def and Lib_ Par
;	- Lib_ Def must be used for internal routines (ie, not instructions or
;	functions)
;	- Lib_ Par must be used for instructions or functions: it reserved a space
;	before the routine if used by the interpreter, to call the parameter
;	calculation routines. Well this is internal, you don't have to care
;	about it, just use "Lib_ Par" for routines referenced in the token
;	table...
;
; BSR and JSR
;	- You cannot directly call other library routines from one routine
;	by doing a BSR, but I have defined special macros (in +CEQU.S file)
;	to allow you to easily do so. Here is the list of available macros:
;
;	Rbsr	L_Routine	does a simple BSR to the routine
;	Rbra	L_Routine	as a normal BRA
;	Rbeq	L_Routine	as a normal Beq
;	Rbne	L_Routine	...
;	Rbcc	L_Routine
;	Rbcs	L_Routine
;	Rblt	L_Routine
;	Rbge	L_Routine
;	Rbls	L_Routine
;	Rbhi	L_Routine
;	Rble	L_Routine
;	Rbpl	L_Routine
;	Rbmi	L_Routine
;
; I remind you that you can only use this to call an library routine
; from ANOTHER routine. You cannot do a call WITHIN a routine, or call
; the number of the routine your calling from...
; The compiler (and AMOSPro extension loading part) will manage to find
; the good addresses in your program from the offset table.
;
; You can also call some main AMOSPro.Lib routines, to do so, use the
; following macros:
;	Rjsr	L_Routine
;	Rjmp	L_Routine
;
; Here is the list of the most usefull routines from the AMOSPro.Lib
;
;	Rjsr	L_Error
; ~~~~~~~~~~~~~~~~~~~~~
;	Jump to normal error routine. See end of listing
;
;	Rjsr	L_ErrorExt
; ~~~~~~~~~~~~~~~~~~~~~~~~
;	Jump to specific error routine. See end of listing.
;
; 	Rjsr	L_Test_PaSaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Perform one AMOSPro updating procedure, update screens, sprites,
;	bobs etc. You should use it for wait loops. Does not jump to
;	automatic calls.
;
;	Rjsr	L_Test_Normal
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Same as before, but with automatic function jump.
;
;	Rjsr	L_WaitRout
; ~~~~~~~~~~~~~~~~~~~~~~~~
;	Wait for D3 VBL with tests.
;	See play instruction.
;
;	Rjsr	L_GetEc
; ~~~~~~~~~~~~~~~~~~~~~
;	Get screen address: In: D1.l= number, Out: A0=address
;
;	Rjsr	L_Demande
; ~~~~~~~~~~~~~~~~~~~~~~~
;	Ask for string space.
;	D3.l is the length to ask for. Return A0/A1 point to free space.
;	Poke your string there, add the length of it to A0, EVEN the
;	address to the highest multiple of two, and move it into
;	HICHAINE(a5) location...
;
;	Rjsr	L_RamChip
; ~~~~~~~~~~~~~~~~~~~~~
;	Ask for PUBLIC|CLEAR|CHIP ram, size D0, return address in D0, nothing
;	changed, Z set according to the success.
;
;	Rjsr	L_RamChip2
; ~~~~~~~~~~~~~~~~~~~~~~
;	Same for PUBLIC|CHIP
;
;	Rjsr	L_RamFast
; ~~~~~~~~~~~~~~~~~~~~~
;	Same for PUBLIC|CLEAR
;
;	Rjsr	L_RamFast2
; ~~~~~~~~~~~~~~~~~~~~~~~~
;	Same for PUBLIC
;
;	Rjsr	L_RamFree
; ~~~~~~~~~~~~~~~~~~~~~~~
;	Free memory A1/D0
;
;	Rjsr	L_Bnk.OrAdr
; ~~~~~~~~~~~~~~~~~~~~~~~~~
;	Find whether a number is a address or a	memory bank number
;	IN: 	D0.l= number
;	OUT: 	D0/A0= number or start(number)
;
;	Rjsr	L_Bnk.GetAdr
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Find the start of a memory bank.
;	IN:	D0.l=	Bank number
;	OUT:	A0=	Bank address
;		D0.w=	Bank flags
;		Z set if bank not defined.
;
;	Rjsr	L_Bnk.GetBobs
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Returns the address of the bob's bank
;	IN:
;	OUT:	Z 	Set if not defined
;		A0=	address of bank
;
;	Rjsr	L_Bnk.GetIcons
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Returns the address of the icons bank
;	IN:
;	OUT:	Z 	Set if not defined
;		A0=	address of bank
;
;	Rjsr	L_Bnk.Reserve
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Reserve a memory bank.
;	IN:	D0.l	Number
;		D1	Flags
;		D2	Length
;		A0	Name of the bank (8 bytes)
;	OUT:	Z 	Set inf not successfull
;		A0	Address of bank
;	FLAGS:
;		Bnk_BitData		Data bank
;		Bnk_BitChip		Chip bank
;		Example:	Bset	#Bnk_BitData|Bnk_BitChip,d1
;	NOTE: 	you should call L_Bnk.Change after reserving/erasing a bank.
;
;	Rjsr	L_Bnk.Eff
; ~~~~~~~~~~~~~~~~~~~~~~~
;	Erase one memory bank.
;	IN:	D0.l	Number
;	OUT:
;
;	Rjsr	L_Bnk.EffA0
; ~~~~~~~~~~~~~~~~~~~~~~~~~
;	Erase a bank from its address.
;	IN:	A0	Start(bank)
;	OUT:
;
;	Rjsr	L_Bnk.EffTemp
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Erase all temporary banks
;	IN:
;	OUT:
;
;	Rjsr	L_Bnk.EffAll
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Erase all banks
;	IN:
;	OUT:
;
;	Rjsr	L_Bnk.Change
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Inform the extension, the bob handler that something has changed
;	in the banks. You should use this function after every bank
;	reserve / erase.
;	IN:
;	OUT:
;
; 	Rjsr	L_Dsk.PathIt
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Add the current AMOS path to a file name.
;	IN:	(Name1(a5)) contains the name, finished by zero
;	OUT:	(Name1(a5)) contains the name with new path
;	Example:
;		move.l	Name1(a5),a0
;		move.l	#"Kiki",(a0)+
;		clr.b	(a0)
;		Rjsr	L_Dsk.PathIt
;		... now I load in the current directory
;
;	Rjsr	 L_Dsk.FileSelector
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	Call the file selector.
;	IN:	12(a3)	Path+filter
;		8(a3)	Default name
;		4(a3)	Title 2
;		0(a3)	Title 1
;		All strings must be in AMOS string format:
;			dc.w	Length
;			dc.b 	"String"
;	OUT:	D0.w	Length of the result. 0 if no selection
;		A0	Address of first character of the result.
;
;
; How does it work?
; Having a look at the +CEQU file, you see that I use special codes
; to show the compiler that it has to copy the asked routine and relocate
; the branch. Some remarks:
;	- The size of a Rbsr is 4 bytes, like the normal branch, it does
; not change the program (you can make some jumps over it)
;	- Although I have coded the signal, and put a lot a security,
; a mischance may lead to the compiler thinking there is a Rbsr where
; there is nothing than normal data. The result may be disastrous! So if
; you have BIG parts of datas in which you do not make any special calls,
; you can put before it the macro: RDATA. It tells the compiler that
; the following code, up to the end of the library routine (up to the next
; L(N) label) is normal data: the compiler will not check for Rbranches...
; Up to now, I have not been forced to do so, but if something goes wrong,
; try that!
;

; +++ Remember!
;	- Your code must be (pc), TOTALLY relocatable, check carefully your
;  	code!
;	- Never perform a BSR or a JSR from one function to another: it
;	_will_ crash once compiled. Use the special macros instead.
;	- Each individual routine of the library can be up to 32K

;---------------------------------------------------------------------
; +++ This macro initialise the library counter, and is also detected by
;     Library_Digest
;
	Lib_Ini	0
;---------------------------------------------------------------------

; +++ Start of the library (for the header)
C_Lib

******************************************************************
*		COLD START
*
; The first routine of the library will perform all initialisations in the
; booting of AMOS.
;
; I have put here all the music datazone, and all the interrupt routines.
; I suggest you put all you C-Code here too if you have some...

; ALL the following code, from L0 to L1 will be copied into the compiled
; program (if any music is used in the program) at once. All RBSR, RBRA etc
; will be detected and relocated. AMOSPro extension loader does the same.
; The length of this routine (and of any routine) must not exceed 32K

; - - - - - - - - - - - - -
	Lib_Int	Mus_Cold		src/ext/music.cpp

	Lib_Int	MusInit
	Lib_Empty			MOff
	Lib_Empty			OpNar
	Lib_Empty			NarInit
	Lib_Int	InSay2
	Lib_Int	InSay1
	Lib_Empty			Say
	Lib_Int	InSetTalk
	Lib_Int	FnMouthWidth
	Lib_Int	FnMouthHeight
	Lib_Int	InBell0
	Lib_Int	InBell1
	Lib_Int	InBoom
	Lib_Int	InShoot
	Lib_Empty			Shout
	Lib_Int	InVolume1
	Lib_Int	InVolume2
	Lib_Empty			Vol
	Lib_Int	InTalkStop
	Lib_Int	InPlay2
	Lib_Int	InPlay3
	Lib_Empty			GoBel
	Lib_Empty			GoShot
	Lib_Empty			VPlay
	Lib_Int	InPlayOff0
	Lib_Int	InPlayOff1
	Lib_Empty			PlOf
	Lib_Empty			StopDma
	Lib_Empty			DmaWait
	Lib_Int	InSamBank
	Lib_Int	InSamLoopOn1
	Lib_Int	InSamLoopOn0
	Lib_Int	InSamLoopOff1
	Lib_Int	InSamLoopOff0
	Lib_Empty			SL0
	Lib_Int	InNoiseTo
	Lib_Int	InSampleTo
	Lib_Empty			ISmt
	Lib_Int	InSamPlay1
	Lib_Int	InSamPlay2
	Lib_Int	InSamPlay3
	Lib_Int	InSamRaw
	Lib_Empty			GoSam
	Lib_Empty			GetSam
	Lib_Int	InSload
	Lib_Empty			SPlay
	Lib_Empty			SPl0
	Lib_Int	InWave
	Lib_Int	InSetWave
	Lib_Int	InDelWave
	Lib_Int	InSetEnvel
	Lib_Empty			RazWave
	Lib_Empty			NoWave
	Lib_Empty			NeWave
	Lib_Empty			WaveAd
	Lib_Empty			WaveDel
	Lib_Empty			Sami_stop
	Lib_Empty			EnvOff
	Lib_Empty			MuIntE
	Lib_Empty			MuInit
	Lib_Int	InMusicOff
	Lib_Int	InMusicStop
	Lib_Int	InMvolume
	Lib_Empty			MVol
	Lib_Int	InVoice
	Lib_Empty			VOnOf
	Lib_Int	InMusic
	Lib_Int	InTempo
	Lib_Int	FnVuMeter
	Lib_Int	FnMusicBase
	Lib_Int	InLedOn
	Lib_Int	InLedOf
	Lib_Empty			Sami_install
	Lib_Empty			Sami_remove
	Lib_Empty			IOOMem
	Lib_Empty			IFonc
	Lib_Empty			WNDef
	Lib_Empty			SNDef
	Lib_Empty			BNSam
	Lib_Empty			STSho
	Lib_Empty			W0Res
	Lib_Empty			MnRes
	Lib_Empty			MNDef
	Lib_Int	FnSamSwapped
	Lib_Int	InSamSwap
	Lib_Int	InSamStop0
	Lib_Int	InSamStop1
	Lib_Int	InTrackLoad
	Lib_Empty			TrackCheck
	Lib_Int	InTrackStop
	Lib_Int	InTrackLoopOn
	Lib_Int	InTrackLoopOff
	Lib_Int	InTrackPlay0
	Lib_Int	InTrackPlay1
	Lib_Int	InTrackPlay2
	Lib_Empty			IDError
	Lib_Int	InMouthRead
	Lib_Int	InTalkMisc
	Lib_Int	InSsave
	Lib_Int	InMedLoad
	Lib_Empty			MedCheck
	Lib_Int	InMedStop
	Lib_Int	InMedPlay0
	Lib_Int	InMedPlay1
	Lib_Int	InMedPlay2
	Lib_Empty			MedOpen
	Lib_Int	InMedMidiOn
	Lib_Empty			MedClose
	Lib_Int	InMedCont
	Lib_Empty			
	Lib_Empty			
	Lib_End
; - - - - - - - - - - - - -


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	+++ TITLE OF THE EXTENSION!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C_Title	dc.b	"AMOSPro Music extension V "
	Version
	dc.b	0,"$VER: "
	Version
	dc.b	0
	Even


; +++ END OF THE EXTENSION
C_End	dc.w	0
	even



; -----------------------------------------------------------------------
; +++ 	CONVERTING YOUR EXTENSION TO AMOSPRO V2.0 FORMAT
;
;	An old style extension will perfectly work under AMOSPro V2.0.
;
;	You may anyway, want to convert you extension to the new format to
; to benefit from the new facilities. To do so:
;
;	- Add the code after the header of the extension
;	- Change the includes to the V2.00 includes
;	- Remove the library pointer list and replace it with the macros
;	- Replace "-1" in the DC.W of the token list by "L_Nul"
;	- For each routine :
;		* Make sure D6/D7 are preserved. If really it is too difficult
;		  to preserve D6/D7, just call via a RJsr, L_SaveRegs at the
;		  beginning of the routine, and L_LoadRegs at the end. See
;		  my code.
;		* Grab the last parameter from D3 and not any more from the
;		  pile (simplest way is to push D3 in A3 just before your code)
;		* Replace the label definition of the routine with the new
;		  Lib_ Def or Lib_ Par macros.
;		* Enventually, for function, use "Ret_" macros to return the
;		  parameters.
;
