
;
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;
;		LIBRAIRIE GRAPHIQUE AMOS
;
;
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		OPT	P-		(C++ code is in other hunks; LoadSeg relocates)

***************************************************************************
		IFND	EZFlag
EZFlag		equ 	0
		ENDC
***************************************************************************

		IncDir	"includes/"
		Include "exec/types.i"
		Include "exec/interrupts.i"
		Include "graphics/gfx.i"
		Include "graphics/layers.i"
		Include "graphics/clip.i"
		Include "hardware/intbits.i"
		Include "devices/input.i"
		Include "devices/inputevent.i"

		Include	"+Debug.s"
		Include	"+AMOS_Includes.s"

; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Version	MACRO
	dc.b	0,"$VER: 2.00",0
	even
	ENDM
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		XREF	_amos_bridge
		XREF	_w_wi_in,_w_wopen,_w_widela
		XREF	_w_cls,_w_set_clip,_w_paint,_w_pattern
		XREF	_w_slider_h,_w_slider_v,_w_slider_knob,_w_slider_set
		XREF	_w_set_colour,_w_get_colour,_w_set_palette,_w_colour_back
		XREF	_w_flash_off,_w_flash,_w_shift,_w_shift_off,_w_fade,_w_fade_off,_w_fade_stop
		XREF	_w_get_fonts,_w_free_fonts,_w_get_font,_w_set_font,_w_close_font
		XREF	_w_rain_set,_w_rain_do,_w_rain_var,_w_rain_del,_w_rain_hide
		XREF	_w_cop_swap,_w_cop_swap_lists,_w_cop_reset,_w_cop_wait,_w_cop_move,_w_cop_movel,_w_cop_base
		XREF	_w_screen_copy
		XREF	_w_vbl
		XREF	_w_input,_w_send_fake_event,_w_test_cyclic
		XREF	_w_tamoswb,_w_joy,_w_request_on_off
		XREF	_w_bob_col,_w_sprite_col,_w_get_col
		XREF	_w_ice_start,_w_ice_end,_w_start_all,_w_end_all,_w_copper_on_off,_w_sc_reset,_w_sc_end
		XREF	_w_add_vbl,_w_rem_vbl,_w_wvbl,_w_clear_key
		XREF	_w_copper_make,_w_copper_force
		XREF	_w_tabk1,_w_tabk2,_w_tabk3,_w_tabk4
		XREF	_w_auto_req,_w_easy_req
		XREF	_w_mem_fast_clear,_w_mem_fast,_w_mem_chip_clear,_w_mem_chip,_w_mem_reserve,_w_mem_free
		XREF	_w_add_flush_routine,_w_add_routine,_w_mem_flush,_w_call_routines
		XREF	_w_inkey,_w_key_speed,_w_key_shift,_w_key_state,_w_put_key,_w_set_function_key,_w_get_function_key
		XREF	_w_hardcol_set,_w_hardcol_get,_w_mouse_key,_w_mouse_click,_w_mouse_recall,_w_mouse_store,_w_mouse_change
		XREF	_w_mouse_hide,_w_mouse_show,_w_mouse_xy,_w_xy_screen,_w_xy_hard,_w_xy_window,_w_mouse_screen,_w_screen_at
		XREF	_w_mouse_set,_w_mouse_limit_screen,_w_mouse_limit,_w_zones_reserve,_w_zone_reset,_w_sy_set_z
		XREF	_w_zone_graphic,_w_zone_hard,_w_zone_mouse
		XREF	_w_sprite_buffer,_w_sprite_priority,_w_sprite_bank,_w_sprite_off,_w_sprites_off,_w_sprite_xy
		XREF	_w_sprite,_w_sprites_stop,_w_sprites_restart,_w_sprites_update,_w_sprite_set,_w_sprite_unset,_w_sprites_show
		XREF	_w_hot_spot,_w_get_bob,_w_bob_xy,_w_paste,_w_bob_set,_w_bob_off,_w_bobs_off,_w_bob_limit
		XREF	_w_priority,_w_bobs_screen_off,_w_bob_put,_w_bob_update,_w_flip,_w_mask,_w_bob_erase,_w_bob_show
		XREF	_w_cblock_get,_w_cblock_put,_w_cblock_delete,_w_cblocks_delete,_w_block_get,_w_block_delete
		XREF	_w_blocks_delete,_w_block_put,_w_block_flip
		XREF	_w_sc_open,_w_sc_close,_w_sc_close_range,_w_sc_to_front,_w_sc_to_back,_w_sc_view,_w_sc_offset
		XREF	_w_sc_hide,_w_sc_clone,_w_sc_double,_w_sc_swap,_w_sc_swap_all,_w_sc_dual,_w_sc_dual_priority
		XREF	_w_sc_activate,_w_sc_current,_w_sc_address,_w_sc_free_number,_w_sc_max_raw,_w_sc_ntsc
		XREF	_w_menu_start,_w_menu_stop
		xdef	_w_debut
_w_debut
WDebut	bra	Startall	Cold start
	bra	Endall		End
	dc.l	L_Trappe	Length of data zone
	bra	IceStart	Access of System functions
	bra	IceEnd		Stop access of system functions
	dc.b	"P110"		Version Pro 1.10

BugBug	movem.l	d0-d2/a0-a2,-(sp)
.Ll	move.w	#$FF0,$DFF180
.L0	move.w	#10000,d1
.L1	move.w	d0,$DFF180
.Ill	moveq	#0,d1

		Version

	IFEQ	EZFlag
***********************************************************
*	COLLISIONS HARD

******* Hardware collisions: src/w/mouse.cpp
HColSet	jmp	_w_hardcol_set
HColGet	jmp	_w_hardcol_get
	ENDC

***********************************************************
*	COLLISIONS SOFT: src/w/collision.cpp
BbColl:	jmp	_w_bob_col
SpColl:	jmp	_w_sprite_col
GetCol:	jmp	_w_get_col

******* Bobs: src/w/bobs.cpp
SpotH:	jmp	_w_hot_spot
GetBob:	jmp	_w_get_bob

; BbInit / BbEnd: src/w/bobs.cpp

BobXY:	jmp	_w_bob_xy
TPatch	jmp	_w_paste
BobSet:	jmp	_w_bob_set
BobOff:	jmp	_w_bob_off
BobSOff:	jmp	_w_bobs_off
BobLim:	jmp	_w_bob_limit
TPrio	jmp	_w_priority

***********************************************************
*	ENLEVE LES BOBS D'UN ECRAN!
*	A0= Ecran
		xdef	_w_bb_ec_off
_w_bb_ec_off
BbEcOff:	jmp	_w_bobs_screen_off

***********************************************************
*	ADRESSE D'UN BOB: D1= Numero!

BobPut:	jmp	_w_bob_put
BobAct:	jmp	_w_bob_update

***********************************************************
*	RETOURNEUR DE SPRITES!

******* Initialisation: fabrique la table
******* Fin, libere la memoire

RevTrap	jmp	_w_flip
Masque:	jmp	_w_mask

BobEff:	jmp	_w_bob_erase
BobAff	jmp	_w_bob_show

******************************************************************
*	Screen copy a0,d0,d1,d4,d5 to a1,d2,d3,d6
*
*	a0 Origin Bit Map Struc.   a1 Destination Bit Map Struc.
*	d0 Origin X (16 a factor!) d2 Destination X (16 a factor!)
*	d1 Origin Y		   d3 Destination Y
*		  	d4 Width  X (Must be multiple of 16!)
*			d5 height Y
*			d6 Minterm
*
*	If minterm is $CC and d0,d2,d4 are on word boundaries
*	then blit is done and result is 0 otherwise not done
*	and result is -1.
*
*	Uses only A and D channels for blit,
*	therefore twice as fast as normal screen copy!
*

WScCpy:	jmp	_w_screen_copy		src/w/drawing.cpp


;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* SCREENS
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------

; EcInit / EcEnd / EcRaz: src/w/screens.cpp
EcEnd:	jmp	_w_sc_end
EcRaz:	jmp	_w_sc_reset


******* Jumps to screen functions
		xdef	_w_ec_in
_w_ec_in
EcIn:	bra	EcRaz		;Raz:
	bra	EcCopper	;CopMake:
	bra	EcCopper	;*
	bra	EcCree		;Cree:
	bra	EcDel		;Del:
	bra	EcFirst		;First:
	bra	EcLast		;Last:
	bra	EcMarch		;Active:
	bra	EcForceCop	;CopForce:
	bra	EcView		;AView:
	bra	EcOffs		;OffSet:
	bra	EcEnd		;Visible:
	bra	EcDAll		;DelAll:
	bra	EcGCol		;GCol:
	bra	EcSCol		;SCol:
	bra	EcSPal		;SPal:
	bra	EcSColB		;SColB:
	bra	FlStop		;FlRaz:
	bra	FlStart		;Flash:
	bra	ShStop		;ShRaz:
	bra	ShStart		;Shift:
	bra	EcHide		;EHide:
	bra	MakeCBloc	;CBlGet:
	bra	DrawCBloc	;CBlPut:
	bra	FreeCBloc	;CBlDel:
	bra	RazCBloc	;CBlRaz:
	bra	EcLibre		;Libre:
	bra	EcCClo		;CCloEc:
	bra	EcCrnt		;Current:
	bra	EcDouble	;Double:
	bra	ScSwap		;SwapSc:
	bra	ScSwapS		;SwapScS:
	bra	EcAdres		;AdrEc:
	bra	Duale		;SetDual:
	bra	DualP		;PriDual:
	bra	EcCls		;ClsEc:
	bra	SPat		;Pattern:
	bra	TGFonts		;GFonts:
	bra	TFFonts		;FFonts:
	bra	TGFont		;GFont:
	bra	TSFont		;SFont:
	bra	TSClip		;SetClip:
	bra	MakeBloc	;- BlGet:		Routine blocs normaux
	bra	DelBloc		;-BlDel:
	bra	RazBloc		;-BlRaz:
	bra	DrawBloc	;-BlPut:
	bra	SliVer		;- VerSli:		Slider vertical
	bra	SliHor		;- HorSli:		Slider horizontal
	bra	SliSet		;- SetSli:		Set slider params
	bra	StaMn		;- MnStart:	Sauve l'ecran
	bra	StoMn		;- MnStop:		Remet l'ecran
	bra	TRDel		;- RainDel:	Delete RAINBOW
	bra	TRSet		;- RainSet:	Set RAINBOW
	bra	TRDo		;- RainDo:		Do RAINBOW
	bra	TRHide		;- RainHide:	Hide / Show RAINBOW
	bra	TRVar		;- RainVar:	Var RAINBOW
	bra	FadeTOn		;- FadeOn:		Fade
	bra	FadeTOf		;- FadeOf:		Fade Off
	bra	TCopOn		;- CopOnOff:	Copper ON/OFF
	bra	TCopRes		;- CopReset:	Copper RESET
	bra	TCopSw		;- CopSwap:	Copper SWAP
	bra	TCopWt		;- CopWait:	Copper WAIT
	bra	TCopMv		;- CopMove:	Copper MOVE
	bra	TCopMl		;- CopMoveL:	Copper MOVEL
	bra	TCopBs		;- CopBase:	Copper BASE ADDRESS
	bra	TAbk1		;- AutoBack1:	Autoback 1
	bra	TAbk2		;- AutoBack2:	Autoback 2
	bra	TAbk3		;- AutoBack3:	Autoback 3
	bra	TAbk4		;- AutoBack4:	Autoback 4
	bra	TPaint		;- SuPaint:	Super paint!
	bra	RevBloc		;- BlRev:		Retourne le bloc
	bra	RevTrap		;- DoRev:		Retourne dans la banque
	bra	TAmosWB		;- AMOS_WB		AMOS/WorkBench
	bra	WScCpy		;- ScCpyW		New_W_2.s
	bra	TMaxRaw		;- MaxRaw		Maximum raw number
	bra	TNTSC		;- NTSC		NTSC?
	bra	SliPour		;- PourSli		Calculs slider

***********************************************************
*	Instructions de gestion des ecrans
***********************************************************

******* Screen management: src/w/screens.cpp
TMaxRaw	jmp	_w_sc_max_raw
TNTSC	jmp	_w_sc_ntsc

ScSwap:	jmp	_w_sc_swap
ScSwapS	jmp	_w_sc_swap_all
EcCClo:	jmp	_w_sc_clone
EcDouble:	jmp	_w_sc_double
Duale:	jmp	_w_sc_dual
DualP:	jmp	_w_sc_dual_priority

EcCree:	jmp	_w_sc_open

;	Sauve les contenu du rasport de l'ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Ec_Push / Ec_Pull: src/w/drawing.cpp

EcView:	jmp	_w_sc_view
EcFirst:	jmp	_w_sc_to_front
EcLast:	jmp	_w_sc_to_back
EcDel:	jmp	_w_sc_close


EcDAll:	jmp	_w_sc_close_range
EcOffs:	jmp	_w_sc_offset
EcHide:	jmp	_w_sc_hide

***********************************************************
*	GESTION DE L'AUTOBACK!
******* Autoback: src/w/autoback.cpp
TAbk1:	jmp	_w_tabk1
TAbk2	jmp	_w_tabk2
TAbk3	jmp	_w_tabk3
TAbk4:	jmp	_w_tabk4

***********************************************************
*	CLS
*	D1= Couleur
*	D2= X
*	D3= Y
*	D4= X2
*	D5= Y2
EcCls:	jmp	_w_cls			src/w/drawing.cpp

;	Active l'ecran D1 - si pas ecran CLONE!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EcMarch	jmp	_w_sc_activate

;	Routine d'activation de l'ecran A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Ec_Active: src/w/screens.cpp

******* RETURNS CURRENT USERS SCREEN ADDRESS
EcCrnt	jmp	_w_sc_current

******* ADRESSE ECRAN D1
EcAdres	jmp	_w_sc_address

******* Colours: src/w/colours.cpp
EcSCol:	jmp	_w_set_colour
EcGCol:	jmp	_w_get_colour
EcSPal	jmp	_w_set_palette
EcSColB	jmp	_w_colour_back


******* Rainbows: src/w/rainbow.cpp
TRHide	jmp	_w_rain_hide
TRDo	jmp	_w_rain_do
TRVar	jmp	_w_rain_var
TRSet	jmp	_w_rain_set
TRDel	jmp	_w_rain_del

***********************************************************
*-----*	Ss programme ---> adresse d'un ecran

******* Trouve le premier ecran libre
EcLibre:	jmp	_w_sc_free_number

;	SET CLIP ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TSClip:	jmp	_w_set_clip		src/w/drawing.cpp


;	Change le clip rectangle dans l'ecran
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Ec_SetClip: src/w/drawing.cpp

******* PAINT: src/w/drawing.cpp
TPaint	jmp	_w_paint

******* SET PATTERN ecran courant!
SPat:	jmp	_w_pattern		src/w/drawing.cpp

******* Efface le pattern de l'ecran courant
; EffPat: src/w/drawing.cpp

******* Fonts: src/w/fonts.cpp
TGFonts	jmp	_w_get_fonts
TFFonts	jmp	_w_free_fonts
TGFont:	jmp	_w_get_font
TSFont	jmp	_w_set_font
CFont:	jmp	_w_close_font

	IFEQ	EZFlag
******************************************************************
*	MARCHE / ARRET de menus
; Menus: src/w/fonts.cpp
StaMn:	jmp	_w_menu_start
StoMn	jmp	_w_menu_stop
	ENDC

******************************************************************
*	SLIDERS

******* Sliders: src/w/drawing.cpp
SliHor	jmp	_w_slider_h
SliVer	jmp	_w_slider_v
SliPour	jmp	_w_slider_knob
SliSet:	jmp	_w_slider_set


; Flash Off, Flash: src/w/colours.cpp
FlStop:	jmp	_w_flash_off
FlStart	jmp	_w_flash


	IFEQ	EZFlag
******* Shift: src/w/colours.cpp
ShStart	jmp	_w_shift


ShStop:	jmp	_w_shift_off

; The interrupt parts of shift, fade and flash: src/w/vbl.cpp
	ENDC

******* Fade: src/w/colours.cpp
FadeTOf	jmp	_w_fade_off
FaStop	jmp	_w_fade_stop
FadeTOn	jmp	_w_fade




***********************************************************
*	FABRIQUE LA COPPER LISTE A PARTIR DES ECRANS
*
******* ACTUALISATION DES ECRANS
		xdef	_w_ec_force_cop
_w_ec_force_cop
EcForceCop:	jmp	_w_copper_force		src/w/copperlist.cpp
EcCopper:	jmp	_w_copper_make

; CpInit / CpEnd: src/w/copper.cpp


***********************************************************
*	GESTION DIRECTE COPPER
***********************************************************
	IFEQ	EZFlag
; Copper On/Off: src/w/copper.cpp
TCopOn	jmp	_w_copper_on_off

******* The user copper list: src/w/copper.cpp
TCopSw	jmp	_w_cop_swap
TCpSw	jmp	_w_cop_swap_lists
TCopRes	jmp	_w_cop_reset
TCopWt	jmp	_w_cop_wait
TCopMv	jmp	_w_cop_move
TCopMl	jmp	_w_cop_movel
TCopBs	jmp	_w_cop_base
	ENDC

;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* DIVERS
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------

******* RESERVATION MEMOIRE
		xdef	_w_fast_mm
_w_fast_mm
FastMm	movem.l	a0/d1,-(sp)
		xdef	_w_fast_mm2
_w_fast_mm2
FastMm2	movem.l	a0/d1,-(sp)
		xdef	_w_chip_mm
_w_chip_mm
ChipMm	movem.l	a0/d1,-(sp)
		xdef	_w_chip_mm2
_w_chip_mm2
ChipMm2	movem.l	a0/d1,-(sp)
		xdef	_w_free_mm
_w_free_mm
FreeMm	bra	WMemFree


;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* AMAL
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------
******* AMAL: src/w/amal.cpp (amal_glue.s for the entries)
TokAMAL		jmp	_w_amal_tok
CreAMAL		jmp	_w_amal_cre
MvOAMAL		jmp	_w_amal_mvo
DAllAMAL	jmp	_w_amal_dall
Animeur		jmp	_w_amal_animate
RegAMAL		jmp	_w_amal_reg
ClrAMAL		jmp	_w_amal_clr
FrzAMAL		jmp	_w_amal_frz
UFrzAMAL	jmp	_w_amal_ufrz
SyncO		jmp	_w_amal_set_sync
Sync		jmp	_w_amal_sync
SetPlay		jmp	_w_amal_play_set
TMovon		jmp	_w_amal_movon
TChanA		jmp	_w_amal_chan_a
TChanM		jmp	_w_amal_chan_m
		XREF	_w_amal_tok,_w_amal_cre,_w_amal_mvo,_w_amal_dall,_w_amal_animate,_w_amal_reg,_w_amal_clr
		XREF	_w_amal_frz,_w_amal_ufrz,_w_amal_set_sync,_w_amal_sync,_w_amal_play_set,_w_amal_movon
		XREF	_w_amal_chan_a,_w_amal_chan_m



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES AU MILIEU!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; IceStart: W_Base and the contexts of the C++ code (AMOSPro's, see
; src/core/call.s), then src/w/startup.cpp, like StartAll, EndAll, IceEnd
IceStart
	lea	W_Base(pc),a0
StartAll	jmp	_w_start_all
EndAll		jmp	_w_end_all
IceEnd		jmp	_w_ice_end


* Librairies
DevName		dc.b	"input.device",0
ConName		dc.b	"console.device",0
LayName		dc.b	"layers.library",0
TopazName	dc.b	"topaz.font",0
Switcher	dc.b	"_Switcher AMOS_",0
TaskName	dc.b	" AMOS",0
		even
***********************************************************

; 	Patch sur LOADVIEW si AMOS TO FRONT si AA
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_w_amos_loadview,_w_old_loadview
_w_amos_loadview
AMOS_LoadView
	tst.b	0.l			T_AMOSHere, modifie lors du patch...
.Wb	rts
_w_old_loadview
Old_LoadView	dc.l	0		Ici et pas ailleurs...

; AMOS / WORKBENCH
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1=0	> 	Workbench
;	D1>0	>	AMOS
;	D1<0	>	Rien (trouver valeur)
;	Retour D1= AMOS ici(-1), WB ici (0)
; AMOS / Workbench: src/w/system.cpp
TAMOSWb	jmp	_w_tamoswb




; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ZONE DE DONNE CENTRALE (BEARK)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


;		Zone de donnee externe
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_w_con_io
_w_con_io
; (OpConsole clears Lio+Lmsg bytes: the original's 32+8 overran into the
; buffer that followed)
ConIo		ds.b 	Lio+Lmsg
ConEssai	ds.b	32
W_Base		ds.l	1
		xdef	_w_gfx_base_var
_w_gfx_base_var
GfxBase		ds.l	1
		even
; Table de retournement bobs


;		Caracteres speciaux des fontes AMOS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_w_def_font
_w_def_font
Def_Font	IncBin	"bin/+WFont.bin"
		even

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* SYSTEME : SOURIS / CLAVIER / INTER VBL
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------


******* Table des sauts
		xdef	_w_sy_in
_w_sy_in
SyIn:	bra 	ClInky			;0 -Inkey:
	bra	ClVide			;1 -ClearKey:
	bra 	ClSh			;2 -Shifts:
	bra	ClInst			;3 -Instant:
	bra	ClKeyM			;4 -KeyMap:
	bra	ClJoy			;5 -Joy:
	bra	ClPutK			;6 -PutKey:
	bra	MHide			;7 -Hide:
	bra	MShow			;8 -Show:
	bra	MChange			;9 -ChangeM:	 ChMouse
	bra	MXy			;10-XyMou:	 XY Mouse
	bra	CXyHard			;11-XyHard:	 Conversion SCREEN-> HARD
	bra	CXyScr			;12-XyScr:	 Conversion HARD-> SCREEN
	bra	MBout			;13-MouseKey:
	bra	MSetAb			;14-SetM:
	bra	GetSIn			;15-ScIn:	 Get screen IN
	bra	CXyWi			;16-XyWin:	 Conversion SCREEN-> WINDOW courante
	bra	MLimA			;17-LimitM:	 Limit mouse
	bra	SyZoHd			;18-ZoHd:	 Zone coordonnees HARD
	bra	SyResZ			;19-ResZone:	 Reserve des zones
	bra	SyRazZ			;20-RazZone:	 Effacement zones
	bra	SySetZ			;21-SetZone:	 Set zone
	bra	SyMouZ			;22-GetZone:	 Zone souris!
	bra	WVbl			;23-WaitVbl:
	bra	HsSet			;24-SetHs:	 Affiche un hard sprite
	bra	HsUSet			;25-USetHs:	 Efface un hard sprite
	bra	ClFFk			;26-SetFunk:
	bra	ClGFFk			;27-GetFunk:
	bra	HsAff			;28-AffHs:	 Recalcule les hard sprites
	bra	HsBank			;29-SetSpBank:	 Fixe la banque de sprites
	bra	HsNXYA			;30-NXYAHs:	 Instruction sprite
	bra	HsXOff			;31-XOffHs:	 Sprite off n
	bra	HsOff			;32-OffHs:	 All sprite off
	bra	HsAct			;33-ActHs:	 Actualisation HSprite
	bra	HsSBuf			;34-SBufHs:	 Set nombre de lignes
	bra	HsStAct			;35-StActHs:	 Arrete les HS sans deasctiver!
	bra	HsReAct			;36-ReActHs:	 Re-Active tous!
	bra	MStore			;37-StoreM:	 Stocke etat souris / Show on
	bra	MRecall			;38-RecallM:	 Remet la souris
	bra	HsPri			;39-PriHs:	 Priorites SPRITES/PLAYFIELD
	bra	TokAMAL			;40-AMALTok:	 Tokenise AMAL
	bra	CreAMAL			;41-AMALCre:	 Demarre AMAL
	bra	MvOAMAL			;42-AMALMvO:	 On/Off/Freeze AMAL
	bra	DAllAMAL		;43-AMALDAll:	 Enleve TOUT!
	bra	Animeur			;44-AMAL:	 Un coup d'animation
	bra	RegAMAL			;45-AMALReg:	 Registre!
	bra	ClrAMAL			;46-AMALClr:	 Clear
	bra	FrzAMAL			;47-AMALFrz:	 FREEZE all
	bra	UFrzAMAL		;48-AMALUFrz:	 UNFREEZE all
	bra	BobSet			;49-SetBob:	 Entree set bob
	bra	BobOff			;50-OffBob:	 Arret bob
	bra	BobSOff			;51-OffBobS:	 Arret tous bobs
	bra 	BobAct			;52-ActBob:	 Actualisation bobs
	bra	BobAff			;53-AffBob:	 Affichage bobs
	bra	BobEff			;54-EffBob:	 Effacement bobs
	bra	ChipMM			;55-SyChip:	 Reserve CHIP
	bra	FastMM			;56-SyFast:	 Reserve FAST
	bra	BobLim			;57-LimBob:	 Limite bobs!
	bra	SyZoGr			;58-ZoGr:	 Zone coord graphiques
	bra	GetBob			;59-SprGet:	 Saisie graphique
	bra	Masque			;60-MaskMk:	 Calcul du masque
	bra	SpotH			;61-SpotHot:	 Fixe le point chaud
	bra	BbColl			;62-ColBob:	 Collisions bob
	bra	GetCol			;63-ColGet:	 Fonction collision
	bra	SpColl			;64-ColSpr:	 Collisions sprites
	bra	SyncO			;65-SetSync:	 Synchro on/off
	bra	Sync			;66-Synchro:	 Synchro step
	bra	SetPlay			;67-PlaySet:	 Set play direction...
	bra	BobXY			;68-XYBob:	 Get XY Bob
	bra	HsXY			;69-XYSp:	 Get XY Sprite
	bra	BobPut			;70-PutBob:	 Put Bob!
	bra	TPatch			;71-Patch:	 Patch icon/bob!
	bra	MRout			;72-MouRel:	 Souris relachee
	bra	MLimEc			;73-LimitMEc:	 Limit mouse ecran
	bra	FreeMM			;74-SyFree:	 Libere mem
	bra	HColSet			;75-SetHCol:	 Set HardCol
	bra	HColGet			;76-GetHCol:	 Get HardCol
	bra	TMovon			;77-MovOn:	 Movon!
	bra	TKSpeed			;78-KeySpeed:	 Key speed
	bra	TChanA			;79-ChanA:	 =ChanAn
	bra	TChanM			;80-ChanM:	 =ChanMv
	bra	TPrio			;81-SPrio:	 Set priority
	bra	TGetDisc		;82-GetDisc:	 State of disc drive
	bra	Add_VBL			;83-RestartVBL	 Restart VBL
	bra	Rem_VBL			;84-StopVBL	 Stop VBL
	bra	ClKWait			;85-KeyWaiting	 (P) Une touche en attente?
	bra	WMouScrFront		;86-MouScrFront	 (P) Coordonnees souris dans ecran front
	bra	WMemReserve		;87-MemReserve	 (P) Reservation memoire secure
	bra	WMemFree		;88-MemFree	 (P) Liberation memoire secure
	bra	WMemCheck		;89-MemCheck	 (P) Verification memoire
	bra	WMemFastClear		;90-MemFastClear (P)
	bra	WMemChipClear		;91-MemChipClear
	bra	WMemFast		;92-MemFast
	bra	WMemChip		;93-MemChip
	bra	WSend_FakeEvent		;94-Send_FakeEvent	Envoi d'un faux event souris
	bra	WTest_Cyclique		;95-Test_Cyclique	Tests cyclique AMOS
	bra	WAddFlushRoutine	;96-AddFlushRoutine	Ajoute une routine FLUSH
	bra	WMemFlush		;97-MemFlush	 	Force un memory FLUSH
	bra	WAddRoutine		;98-AddRoutine		Ajoute une routine
	bra	WCallRoutines		;99-CallRoutines 	Appelle les routines
	bra	WRequest_OnOff		;100-Set Requester	Change le requester

; ___________________________________________________________________
;
;	RESERVATION / LIBERATION MEMOIRE CENTRALISEE / DEBUGGAGE
; ___________________________________________________________________
;
		RsReset
Mem_Length	rs.l	1
Mem_Pile	rs.l	8
Mem_Header	equ	__Rs
Mem_Border	equ	128
Mem_Code	equ	$AA
MemList_Size	equ	1024*8

; Memory and routine lists: src/w/memory.cpp
WMemFastClear	jmp	_w_mem_fast_clear
WMemFast	jmp	_w_mem_fast
WMemChipClear	jmp	_w_mem_chip_clear
WMemChip	jmp	_w_mem_chip
WMemReserve	jmp	_w_mem_reserve
WMemFree	jmp	_w_mem_free
WMemCheck	jmp	_w_nothing_entry
WAddFlushRoutine	jmp	_w_add_flush_routine
WAddRoutine	jmp	_w_add_routine
WMemFlush	jmp	_w_mem_flush
WCallRoutines	jmp	_w_call_routines

***********************************************************
*	Gestion de la souris
***********************************************************

; VblInit, Add_VBL, Rem_VBL: src/w/vbl.cpp
Add_VBL	jmp	_w_add_vbl
Rem_VBL	jmp	_w_rem_vbl

******* Entree des interruptions
; The VBL server (is_Data in a1): src/w/vbl.cpp
		xdef	_w_vbl_in
_w_vbl_in
VblIn:	movem.l	d2-d7/a2-a4,-(sp)

; Wait Vbl: src/w/support.cpp
WVbl	jmp	_w_wvbl

******* Traitement de la souris

***********************************************************
*	BOUTONS DE LA SOURIS
***********************************************************
******* Mouse: src/w/mouse.cpp
MBout:	jmp	_w_mouse_key
MRout:	jmp	_w_mouse_click
MRecall:jmp	_w_mouse_recall
MStore:	jmp	_w_mouse_store
MChange:jmp	_w_mouse_change
MHide:	jmp	_w_mouse_hide
MShow:	jmp	_w_mouse_show

**********************************************************
*	COORDONNEES
**********************************************************
MXy:	jmp	_w_mouse_xy
CXyScr	jmp	_w_xy_screen
CXyHard	jmp	_w_xy_hard
CXyWi:	jmp	_w_xy_window
WMouScrFront	jmp	_w_mouse_screen
GetSIn:	jmp	_w_screen_at
MSetAb:	jmp	_w_mouse_set
MLimEc	jmp	_w_mouse_limit_screen
MLimA:	jmp	_w_mouse_limit



**********************************************************
*	JOYSTICK / d1= # de port
**********************************************************
ClJoy:	jmp	_w_joy

******* Zones: src/w/mouse.cpp
SyResZ:	jmp	_w_zones_reserve
SyRazZ:	jmp	_w_zone_reset
SySetZ:	jmp	_w_sy_set_z
SyZoGr:	jmp	_w_zone_graphic
SyZoHd:	jmp	_w_zone_hard
SyMouZ:	jmp	_w_zone_mouse

;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* SPRITES HARDWARE
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------
; HsInit, HsEnd and the sprite buffers: src/w/sprites.cpp

******* Sprites: src/w/sprites.cpp
HsSBuf:	jmp	_w_sprite_buffer



HsPri:	jmp	_w_sprite_priority


HsBank:	jmp	_w_sprite_bank
HsXOff:	jmp	_w_sprite_off
HsOff:	jmp	_w_sprites_off
HsXY:	jmp	_w_sprite_xy
HsNxya:	jmp	_w_sprite
HsStAct:	jmp	_w_sprites_stop
HsReAct:	jmp	_w_sprites_restart
HsAct:	jmp	_w_sprites_update
HsSet:	jmp	_w_sprite_set
HsUSet:	jmp	_w_sprite_unset
HsAff:	jmp	_w_sprites_show


;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* BLOCS
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------
******* Blocks: src/w/bobs.cpp
MakeCBloc:	jmp	_w_cblock_get
DrawCBloc:	jmp	_w_cblock_put
FreeCBloc:	jmp	_w_cblock_delete
RazCBloc:	jmp	_w_cblocks_delete
MakeBloc:	jmp	_w_block_get
DelBloc:	jmp	_w_block_delete
RazBloc	jmp	_w_blocks_delete
DrawBloc:	jmp	_w_block_put
RevBloc	jmp	_w_block_flip


;-----------------------------------------------------------------
; **** *** **** ****
; *     *  *  * *	******************************************
; ****  *  *  * ****	* TRAPPE FENETRES
;    *  *  *  *    *	******************************************
; ****  *  **** ****
;-----------------------------------------------------------------


; console.device, input.device: src/w/keyboard.cpp

; Input handler, branche sur la chaine des inputs.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; The input handler (a0 the events, d0 returned): src/w/input.cpp
		xdef	_w_io_handler_entry
_w_io_handler_entry
IoHandler
	move.l	a5,-(sp)
WSend_FakeEvent	jmp	_w_send_fake_event

; Clear Key: src/w/keyboard.cpp
ClVide	jmp	_w_clear_key

; KEY WAIT, retourne BNE si des touches en attente
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ClKWait	jmp	_w_key_waiting_entry

; Inkey: src/w/keyboard.cpp
ClInky	jmp	_w_inkey

; Change KEY MAP A1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ClKeyM	jmp	_w_nothing_entry

; Key Speed, Key Shift, Key State, Put Key, function keys: src/w/keyboard.cpp
TKSpeed	jmp	_w_key_speed
ClSh:	jmp	_w_key_shift
ClInst:	jmp	_w_key_state
ClPutK:	jmp	_w_put_key
ClFFk:	jmp	_w_set_function_key
ClGFFk:	jmp	_w_get_function_key

; RETOUR L'ETAT DU FLAG DISC
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TGetDisc	jmp	_w_get_disc_entry

;	Gestion cyclique hors interruptions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Cyclic tests outside the interrupts: src/w/input.cpp
WTest_Cyclique	jmp	_w_test_cyclic


; Error "illegal function call" of the zone functions



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	AMOS REQUESTER ROUTINES
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Req_WY		equ	100
T_Tit		equ	0
T_Text		equ	T_Tit+80
T_Gad		equ	T_Text+320
T_Size		equ	T_Gad+128

;	Set request ON / OFF
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
WRequest_OnOff	jmp	_w_request_on_off

; System requesters: src/w/requester.cpp (a5 the data zone for it)
		xdef	_w_auto_req_entry
_w_auto_req_entry
AutoReq	move.l	a5,-(sp)
		xdef	_w_easy_req_entry
_w_easy_req_entry
EasyReq	move.l	a5,-(sp)

		xdef	_w_req_pal
_w_req_pal
Req_Pal		Incbin	"bin/ReqPic.bin"
		even


***********************************************************




***********************************************************
*		CODE AMOS HERE?
BufCode		dc.w	0
		dc.b	"I"+$60
		dc.b	"S"+$60
		dc.b	" "+$60
		dc.b	"A"+$60
		dc.b	"M"+$60
		dc.b	"O"+$60
		dc.b	"S"+$60
		dc.b	" "+$60
		dc.b	"H"+$60
		dc.b	"E"+$60
		dc.b	"R"+$60
		dc.b	"E"+$60
		dc.b 	0


	IFNE	EzFlag
TokAMAL
CreAMAL
MvOAMAL
DAllAMAL
Animeur
RegAMAL
ClrAMAL
FrzAMAL
UFrzAMAL
SpColl
SyncO
Sync
SetPlay
HColSet
HColGet
TMovon
TChanA
TChanM

ShStop
ShStart
MakeCBloc
DrawCBloc
FreeCBloc
RazCBloc
Duale
DualP
StaMn
StoMn
TCopOn
TCopRes
TCopSw
TCopWt
TCopMv
TCopMl
TCopBs

DAdAMAL
AMALInit
AMALEnd
ShInit
	rts

	ENDC

;		Banque MOUSE.ABK par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		SECTION	"m",DATA_C
		xdef	_w_default_mouse
_w_default_mouse
		IncBin	"bin/+AMOSPro_Mouse.abk"

		even
