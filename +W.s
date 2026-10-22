
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
 	btst	#6,$BFE001
	bne.s	.Ll
	move.w	#20,d0
.L0	move.w	#10000,d1
.L1	move.w	d0,$DFF180
	dbra	d1,.L1
	dbra	d0,.L0
	btst	#6,$BFE001
	beq.s	.Ill
	movem.l	(sp)+,d0-d2/a0-a2
	rts
.Ill	moveq	#0,d1
	bsr	TAMOSWb
	movem.l	(sp)+,d0-d2/a0-a2
	illegal
	rts

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
BobAd:
*******
	move.l	T_BbDeb(a5),d0
	beq.s	AdBb1
AdBb0:	move.l	d0,a1
	cmp.w	BbNb(a1),d1
	beq.s	AdBb2
	bcs.s	AdBb1
	move.l	BbNext(a1),d0
	bne.s	AdBb0
AdBb1	moveq	#1,d0
AdBb2	rts

BobPut:	jmp	_w_bob_put
BobAct:	jmp	_w_bob_update

***********************************************************
*	RETOURNEUR DE SPRITES!

******* Initialisation: fabrique la table
******* Fin, libere la memoire
RbEnd	rts

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
EcGet:	move.w	d1,d0
	lsl.w	#2,d0
	lea	T_EcAdr(a5),a0
	add.w	d0,a0
	move.l	(a0),d0
	rts
EcGE:	moveq	#0,d0
	rts

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
;-----> Wait mouse key
WaitMK:	bsr	MBout
	cmp.w	#2,d1
	bne.s	WaitMk
Att:	bsr	MBout
	cmp.w	#0,d1
	bne.s	Att
	rts

;-----> OWN BLITTER
OwnBlit:movem.l	d0/d1/a0/a1/a6,-(sp)
	move.l	GfxBase(pc),a6
	jsr	_LVOOwnBlitter(a6)		OwnBlitter
	movem.l	(sp)+,d0/d1/a0/a1/a6
;-----> Wait blitter fini
BlitWait
	move.l	a6,-(sp)
	move.l	GfxBase(pc),a6
	jsr	_LVOWaitBlit(a6)
	move.l	(sp)+,a6
	rts
;-----> DISOWN BLITTER
DOwnBlit
	movem.l	d0/d1/a0/a1/a6,-(sp)
	move.l	GfxBase(pc),a6
	jsr	DisownBlitter(a6)
	movem.l	(sp)+,d0/d1/a0/a1/a6
	rts

;-----> Position du faisceau
PosVbl:	move.l 	$004(a6),d0
	lsr.l 	#8,d0
	and.w 	#$1FF,d0
	rts

******* RESERVATION MEMOIRE
		xdef	_w_fast_mm
_w_fast_mm
FastMm	movem.l	a0/d1,-(sp)
	move.l	#CLEAR|PUBLIC,d1
	bsr	WMemReserve
	move.l	a0,d0
	movem.l	(sp)+,a0/d1
	rts
		xdef	_w_fast_mm2
_w_fast_mm2
FastMm2	movem.l	a0/d1,-(sp)
	move.l	#PUBLIC,d1
	bsr	WMemReserve
	move.l	a0,d0
	movem.l	(sp)+,a0/d1
	rts
		xdef	_w_chip_mm
_w_chip_mm
ChipMm	movem.l	a0/d1,-(sp)
	move.l	#CLEAR|PUBLIC|CHIP,d1
	bsr	WMemReserve
	move.l	a0,d0
	movem.l	(sp)+,a0/d1
	rts
		xdef	_w_chip_mm2
_w_chip_mm2
ChipMm2	movem.l	a0/d1,-(sp)
	move.l	#PUBLIC|CHIP,d1
	bsr	WMemReserve
	move.l	a0,d0
	movem.l	(sp)+,a0/d1
	rts
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
******* CHRGET pour les animations
* 	SAUTE LES MINUSCULES! GENIAL!
AniChr	moveq	#0,d0
AniCh0:	move.b	(a0)+,d0
	beq.s	AnChX
	cmp.b	#33,d0
	bcs.s	AniCh1
	cmp.b	#"Z",d0
	bhi.s	AniCh1
AnChX:	rts
AniCh1:	cmp.b	#"|",d0
	beq.s	AnChX
	cmp.b	#"!",d0
	beq.s	AnChX
	cmp.b	#27,d0
	bne.s	AniCh0
	addq.l	#2,a0
	bra.s	AniCh0

******* Conversion DECIMAL/HEXA-> HEXA
AniLong	moveq	#1,d3
	bsr	AniChr
	cmp.b	#"-",d0
	bne.s	Adh0
	subq.w	#1,d3
	bsr	AniChr
Adh0:	cmp.b	#"$",d0			* Hexa????
	beq.s	Adhh0
	sub.b	#"0",d0
	bcs	AniE1
	cmp.b	#10,d0
	bcc	AniE1
	move.l	d0,d1
	subq.w	#1,d3
Adh1:   bsr 	AniChr
	sub.b	#"0",d0
	bcs.s	Adh2
	cmp.b 	#10,d0
        bcc.s 	Adh2
	add.l	d1,d1
	move.l	d1,d2
	lsl.l	#2,d1
	add.l	d2,d1
	add.l	d0,d1
	bra.s	Adh1
Adh2:   subq.l 	#1,a0
        tst 	d3
	beq.s	AdhX
	bpl	AniE1
	neg.l	d1
AdhX:	move.l	d1,d0
	rts
* En Hexa!
Adhh0	bsr	AniChr
	bsr	Tohh
	bmi	AniE1
	move.l	d0,d1
	subq.w	#1,d3
Adhh1:  bsr 	AniChr
	bsr	Tohh
	bmi.s	Adh2
	lsl.l	#4,d1
	add.l	d0,d1
	bra.s	Adhh1
Tohh	sub.b	#"0",d0
	bcs.s	TohhE
	cmp.b	#10,d0
	bcs.s	TohhX
	sub.b	#"A"-"0",d0
	bcs.s	TohhE
	cmp.b	#6,d0
	bcc.s	TohhE
	add.b	#10,d0
TohhX	tst.b	d0
	rts
TohhE	moveq	#-1,d0
	rts

	IFEQ	EZFlag
***************************** FORMAT D'UNE SEQUENCE
NbInterne:	equ 10
AmPrev:		equ 0
AmNext:		equ 4
AmLong:		equ 8
AmNb:		equ 10
AmPos:		equ 12
AmAuto:		equ 16
AmAct:		equ 20
AmBit:		equ 24
AmCpt:		equ 26
AmDeltX:	equ 28
AmDeltY:	equ 32
AmVirgX:	equ 36
AmVirgY:	equ 38
AmFin:		equ 40
AmAJsr:		equ 44
AmAAd:		equ 48
AmAALoop:	equ 52
AmACLoop:	equ 56
AmACpt:		equ 58
AmIRegs:	equ 60
AmStart:	equ 60+NbInterne*2

***********************************************************
*	TOKENISATEUR AMAL
******* ENTREE
*	D3==> Type 0=Anim / 1= Move / 2= Move X / 3= Move Y
*	A1==> Chaine
*	A2/D2==> Buffer tokenisation
*	D1==> Buffer variables
******* SORTIE
*	D0==> ERREUR?
*	A0==> Longueur chaine
TokAMAL:
********
	movem.l	d1-d7/a1-a6,-(sp)
	move.l	sp,T_AMALSp(a5)
	move.l	d7,-(sp)
	move.l	d7,T_AmBank(a5)

	move.l	a1,a0			* Debut chaine!
	move.l	d1,a1
	move.l	a2,d7			* Bas du buffer
	lea	0(a2,d2.w),a5		* Fin du buffer
	moveq	#0,d6
	move.l	d6,a6
	moveq	#0,d5			* Pas d'autotest
	moveq	#0,d4			* Pas d'erreur
* Init de la table des labels
	move.l	a1,a3
	moveq	#26/2,d0
	moveq	#-1,d1
InA0:	move.l	d1,(a3)+
	dbra	d0,InA0
* Table des FOR/NEXT
	lea	26*2+26*4(a1),a3	* 256 Octets de plus!
	move.l	#-1,(a3)+
	lea	AmJumps(pc),a4		* Table des decalages
* Stos type?
	tst.w	d3
	bne	AniStos

* Boucle d'interpretation
AniLoop	cmp.l	a5,a2
	bcc	AniTrop
	bsr	AniChr
	beq	AnPasse2
	move.w	d0,d1
	bsr	AniChr
	cmp.b	#":",d0
	beq	AnLab
	subq.l	#1,a0
	cmp.b	#"J",d1
	beq	AnJmp
	cmp.b	#"L",d1
	beq	AnLet
	cmp.b	#"M",d1
	beq	AnMove
	cmp.b	#"F",d1
	beq	AnFor
	cmp.b	#"N",d1
	beq	AnNext
	cmp.b	#"I",d1
	beq	AnIf
	cmp.b	#"W",d1
	beq	AnWait
	cmp.b	#"P",d1
	beq	AnPose
	cmp.b	#"E",d1
	beq	AnStop
	cmp.b	#"A",d1
	beq	AnAni
	cmp.b	#"D",d1
	beq	AnDirect
	cmp.b	#"X",d1
	beq	AnExit
	cmp.b 	#")",d1
	beq	AnAutOf
	bra	AniLoop
******* Trop long! Marque et boucle!
AniTrop:sub.l	d7,a2
	add.l	a2,a6
	move.l	d7,a2
	bra	AniLoop

******* AUto test
AnAutOn:addq.l	#1,a0
	bsr	AniChr
	tst.w	d5
	bne	AniE5
	cmp.b	#"(",d0
	bne	AniE1
	bsr	AniChr
	cmp.b	#")",d0
	beq.s	AnAu1
	subq.l	#1,a0
	move.w	$60/2(a4),(a2)+
	move.l	a2,d5
	sub.l	d7,d5
	swap	d5
	move.w	#1,d5
	clr.w	(a2)+
	bra	AniLoop
AnAu1:	move.w	$64/2(a4),(a2)+
	bra	AniLoop
******* FIN AUTOTEST / EXIT
AnExit:	tst.w	d5
	beq	AniE6
	move.w	$68/2(a4),(a2)+
	clr.w	(a2)+
	bra	AniLoop
AnAutOf	tst.w	d5
	beq	AniE6
	move.w	$68/2(a4),(a2)+
	move.l	a2,d0
	sub.l	d7,d0
	swap	d5
	sub.w	d5,d0
	move.l	a0,-(sp)
	move.l	d7,a0
	move.w	d0,0(a0,d5.w)
	move.l	(sp)+,a0
	moveq	#0,d5
	bra	AniLoop
******* DIRECT label EXTERNE!
AnDirect:
	tst.w	d5
	beq	AniE6
	move.w	$6C/2(a4),(a2)+
	bsr	AniChr
	sub.b	#"A",d0
	bcs	AniE1
	cmp.b	#26,d0
	bhi	AniE1
	lsl.w	#1,d0
	move.l	a2,d1
	sub.l	d7,d1
	move.w	d1,26*2(a1,d6.w)
	move.w	d0,26*2+2(a1,d6.w)
	addq.w	#4,d6
	move.w	#-1,26*2(a1,d6.w)
	clr.w	(a2)+
	bra	AniLoop

******* Un LABEL!
AnLab:	sub.b	#"A",d1
	bcs	AniE1
	cmp.b	#26,d1
	bhi	AniE1
	lsl.w 	#1,d1
	tst.w	0(a1,d1.w)
	bpl	AniE8
	move.l	a2,d0
	sub.l	d7,d0
	or.w	d5,d0
	move.w	d0,0(a1,d1.w)
	bra	AniLoop
******* Un BRANCHEMENT
AnJmp:	move.w	$1C/2(a4),(a2)+
	bsr	AniChr
	sub.b	#"A",d0
	bcs	AniE1
	cmp.b	#26,d0
	bhi	AniE1
	lsl.w	#1,d0
	move.l	a2,d1
	sub.l	d7,d1
	or.w	d5,d1
	move.w	d1,26*2(a1,d6.w)
	move.w	d0,26*2+2(a1,d6.w)
	addq.w	#4,d6
	move.w	#-1,26*2(a1,d6.w)
	clr.w	(a2)+
	bra	AniLoop

*******	For RA=deb To end
AnFor:	tst.w	d5
	bne	AniE9
	move.w	$28/2(a4),(a2)+
	bsr	AniReg
	beq	AniE1
	cmp.w	#$000C/2,d1
	bne	AniE1
	swap	d2
	move.w	d2,-(sp)		* Poke le debut
	bsr	AniChr
	cmp.b	#"=",d0
	bne	AniE1
	bsr	AniExp
	bsr	AniChr			* Poke le TO
	cmp.b	#"T",d0
	bne	AniE1
	bsr	AniExp
	move.l	a2,(a3)+
	move.w	(sp)+,d2		* Poke la variable
	move.w	d2,(a3)+
	move.w	d2,(a2)+
	clr.w	(a2)+			* Place pour le TO!
	bra	AniLoop
******* NEXT RA
AnNext:	tst.w	d5
	bne	AniE9
	move.w	$2C/2(a4),(a2)+
	bsr	AniReg
	beq	AniE1
	cmp.w	#$000C/2,d1
	bne	AniE1
	swap	d2
	cmp.w	-(a3),d2
	bne	AniE2
	move.l	-(a3),d0
	sub.l	a2,d0
	move.w	d0,(a2)+
	bra	AniLoop

******* LET
AnLet:	move.w	$20/2(a4),(a2)+
	bsr	AniReg
	beq	AniE1
	add.w	#$0030/2,d1
	movem.l	d1-d3,-(sp)
	bsr	AniChr
	cmp.b	#"=",d0
	bne	AniE1
	bsr	AniExp
	movem.l	(sp)+,d1-d3
	move.w	0(a4,d1.w),(a2)
	move.l	d2,2(a2)
	add.w	d3,a2
	bra	AniLoop

******* TESTS
AnIf:	move.w	$24/2(a4),(a2)+
	bsr	AniExp
	bsr	AniChr
	cmp.b	#"J",d0
	beq	AnJmp
	cmp.b	#"D",d0
	beq	AnDirect
	cmp.b	#"X",d0
	beq	AnExit
	bra	AniE1

******* MOVE DeltaX,DeltaY,Nbstep
AnMove:	tst.w	d5
	bne	AniE9
	move.w	$18/2(a4),(a2)+
	bsr	AniExp
	bsr	AniChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniExp
	bsr	AniChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniExp
	bra	AniLoop


******* PAUSE
AnPose:	cmp.b 	#"L",(a0)
	beq.s	AnPlay
	move.w	$0014/2(a4),(a2)+
	bra	AniLoop
******* END
AnStop:	move.w	$0000/2(a4),(a2)+
	bra	AniLoop
******* WAIT
AnWait:	tst.w	d5
	bne	AniE9
	move.w	$0010/2(a4),(a2)+
	bra	AniLoop
******* PLay Exp
AnPlay:	addq.l	#1,a0
	tst.l	(sp)
	beq	AniE10
	move.w	$00C0/2(a4),(a2)+
	bsr	AniExp
	bra	AniLoop
******* Anim
AnAni:	cmp.b	#"U",(a0)
	beq	AnAutOn
	move.w	$CC/2(a4),(a2)+
	bsr	AniExp
	move.l	a2,-(sp)
	clr.w	(a2)+
	bsr	AniChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniChr
	cmp.b	#"(",d0
	bne	AniE1
AnAni1	bsr	AniExp
	bsr	AniChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniExp
	bsr	AniChr
	cmp.b	#")",d0
	bne	AniE1
	bsr	AniChr
	cmp.b	#"(",d0
	beq.s	AnAni1
	subq.l	#1,a0
	clr.w	(a2)+
	move.l	(sp),d0
	move.l	a2,(sp)
	exg	d0,a2
	sub.l	a2,d0
	move.w	d0,(a2)
	move.l	(sp)+,a2
	bra	AniLoop

***********************************************************
*	COMPATIBLE STOS!
AniStos	move.l	a0,-(sp)
	move.l	(sp)+,a0
	cmp.w	#1,d3
	bne.s	AnMve
******* ANIMATION!
	move.w	$04/2(a4),(a2)+
	bsr	StChr
	cmp.b	#"(",d0
	bne	AniE1
AnSt0	bsr	AniLong
	move.w	d0,(a2)+
	bsr	StChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniLong
	move.w	d0,(a2)+
	bmi	AniE1
	bsr	StChr
	cmp.b	#")",d0
	bne	AniE1
AnSt1:	bsr	StChr
	beq.s	AnSt2
	cmp.b	#"L",d0
	beq.s	AnSt3
	cmp.b	#"(",d0
	beq.s	AnSt0
	bra	AniE1
AnSt2	move.w	#-1,(a2)+
	bra	AnPasse2
AnSt3	move.w	#-2,(a2)+
	bra	AnPasse2
* Mouvement X ou Y!
AnMve:	move.w	$08/2(a4),d0
	cmp.w	#2,d3
	beq.s	AnMv1
	move.w	$0C/2(a4),d0
AnMv1	move.w	d0,(a2)+
	move.l	a2,a3
	move.w	#$8000,(a2)+		* Debut
	clr.w	(a2)+			* Loop/End
	move.w	#$8000,(a2)+		* End
	bsr	StChr
	beq	AniE1
	cmp.b	#"(",d0
	beq.s	AnMv2
	subq.l	#1,a0
	bsr	AniLong
	move.w	d0,(a3)
	bsr	StChr
	cmp.b	#"(",d0
	bne	AniE1
AnMv2	bsr	AniLong
	move.w	d0,(a2)+
	ble	AniE1
	bsr	StChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniLong
	move.w	d0,(a2)+
	bsr	StChr
	cmp.b	#",",d0
	bne	AniE1
	bsr	AniLong
	move.w	d0,(a2)+
	bmi	AniE1
	bsr	StChr
	cmp.b	#")",d0
	bne	AniE1
	bsr	StChr
	cmp.b	#"(",d0
	beq.s	AnMv2
	clr.w	(a2)+
	tst.b	d0
	beq	AnPasse2
	cmp.b	#"L",d0
	bne.s	AnMv3
	move.w	#-1,2(a3)
	bra.s	AnMv4
AnMv3	cmp.b	#"E",d0
	bne	AniE1
AnMv4	bsr	StChr
	beq	AnPasse2
	subq.l	#1,a0
	bsr	AniLong
	move.w	d0,4(a3)
	bra	AnPasse2
******* Chrget pour stos type!
StChr:	moveq	#0,d0
StChr1	move.b	(a0)+,d0
	beq.s	StChr2
	cmp.b	#32,d0
	beq.s	StChr1
	cmp.b	#"a",d0
	bcs.s	StChr2
	cmp.b	#"z",d0
	bhi.s	StChr2
	sub.b	#32,d0
StChr2	rts

***********************************************************
* 	DEUXIEME PASSE: affecte les labels!
AnPasse2:
	move.w	$00/2(a4),(a2)+
	sub.l	d7,a2
	exg.l	d7,a2
	lea	26*2(a1),a0
	moveq	#0,d0
AnLoop2	move.w	(a0)+,d0
	bmi.s	AniX
	move.w	d0,d2
	and.w	#$0001,d2
	and.w	#$FFFE,d0
	move.w	(a0)+,d1
	move.w	0(a1,d1.w),d1
	bmi	Aniee3
	move.w	d1,d3
	and.w	#$0001,d3
	and.w	#$FFFE,d1
	cmp.w	d2,d3
	bne	Aniee4
	sub.w	d0,d1
	move.w	d1,0(a2,d0.w)
	bra.s	AnLoop2
******* Tout est fini!
AniX:	addq.l	#4,sp
	cmp.l	#0,a6
	bne.s	AniX2
	move.l	d7,a0
	movem.l	(sp)+,d1-d7/a1-a6
	moveq	#0,d0
	rts
AniX2:	move.l	a6,a0
	add.l	d7,a0
	movem.l	(sp)+,d1-d7/a1-a6	* -1-> Pas assez de place!
	moveq	#-1,d0			* A0= longueur necessaire!
	rts
Aniee3:	lea	0(a2,d0.w),a0
	bra.s	Anie3
Aniee4:	lea	0(a2,d0.w),a0
	bra.s	Anie4
	ENDC

******* ERREUR!
AniE10:	addq.w	#1,d4			* PLay only with bank
AniE9:	addq.w	#1,d4			* Not authorised during autotest
AniE8:	addq.w	#1,d4			* Label already def
AniE7:	addq.w	#1,d4			* String too long
AniE6:	addq.w	#1,d4			* Autotest not opened
AniE5:	addq.w	#1,d4			* Autotest already on
AniE4:	addq.w	#1,d4			* Jump To/Within AUTOTEST
AniE3:	addq.w	#1,d4			* Label not defined
AniE2:	addq.w	#1,d4			* Next /For
AniE1:	move.l	W_Base(pc),a5
	move.l	T_AMALSp(a5),d1
	move.l	d1,sp
	addq.w	#1,d4			* Syntax error
	move.l	d4,d0
	movem.l	(sp)+,d1-d7/a1-a6
	sub.l	a1,a0			* Offset de l'erreur
	tst.w	d0
	rts

	IFEQ	EZFlag

***********************************************************
*	ROUTINES INTERPRETATION
******* Prend X/Y/A/ ou REG
AniReg:	bsr	AniChr
AniR:	moveq	#$00/2,d1
	cmp.b	#"A",d0
	beq.s	AniRx
	moveq	#$04/2,d1
	cmp.b	#"X",d0
	beq.s	AniRx
	moveq	#$08/2,d1
	cmp.b	#"Y",d0
	beq.s	AniRx
	cmp.b	#"R",d0
	bne.s	AniRe
* R
	moveq	#$0C/2,d1
	bsr	AniChr
	sub.b	#"0",d0
	bcs	AniRe
	cmp.b	#NbInterne,d0
	bcc.s	AniR0
	addq.w	#1,d0
	neg.w	d0
	bra.s	AniR1
AniR0:	sub.b	#"A"-"0",d0
	bcs	AniRe
	cmp.b	#26,d0
	bhi	AniRe
AniR1:	lsl.w	#1,d0
	move.w	d0,d2
	swap 	d2
	moveq	#4,d3
	rts
* Autres
AniRx:	moveq	#2,d3
	rts
* Pas un registre
AniRe:	subq.l	#1,a0
	moveq	#0,d3
	rts

******* Operande!
AniOpe:	bsr	AniChr
	cmp.b	#"K",d0
	beq.s	AniMou
	cmp.b	#"J",d0
	beq.s	AniJoy
	cmp.b	#"O",d0
	beq.s	AniOn
	cmp.b	#"S",d0
	beq	AniSCol
	cmp.b 	#"B",d0
	beq	AniBCol
	cmp.b 	#"C",d0
	beq	AniCol
	cmp.b	#"Z",d0
	beq	AniHaz
	cmp.b	#"V",d0
	beq	AniVu
	cmp.b	#"-",d0
	beq.s	AniOc
	cmp.b	#"$",d0
	beq.s	AniOc
	cmp.b	#"0",d0
	bcs.s	AniO1
	cmp.b	#"9",d0
	bhi.s	AniO1
* Chiffre!
AniOc:	subq.l	#1,a0
	bsr	AniLong
	move.w	$70/2(a4),(a2)+
	move.w	d0,(a2)+
	rts
* =On
AniOn:	move.w	$50/2(a4),(a2)+
	rts
* Saisie du joystick!
AniJoy:	bsr	AniChr
	move.w	$80/2(a4),d1
	cmp.b	#"0",d0
	beq.s	AniO0
	move.w	$84/2(a4),d1
	cmp.b	#"1",d0
	bne	AniE1
AniO0:	move.w	d1,(a2)+
	rts
* Saisie touches souris!
AniMou:	bsr	AniChr
	move.w	$88/2(a4),d1
	cmp.b	#"1",d0
	beq.s	AniO0
	move.w	$8C/2(a4),d1
	cmp.b	#"2",d0
	beq.s	AniO0
	bra	AniE1
* Autres registres
AniO1:	bsr	AniR
	beq	AniE1
	cmp.w	#$04/2,d1
	beq.s	AniO4
	cmp.w	#$08/2,d1
	beq.s	AniO4
* Un registre normal
AniO2:	add.w	#$0040/2,d1
	bra	AniO5
* X-Y  MOUSE/SCREEN/HARD
AniO4:	bsr	AniChr
	subq.l	#1,a0
	cmp.b	#"S",d0
	beq.s	AniO4a
	cmp.b	#"H",d0
	beq.s	AniO4b
	cmp.b	#"M",d0
	bne.s	AniO2
	add.w	#$0070/2,d1
	addq.l	#1,a0
	bra	AniO5
AniO4a:	add.w	#$0050/2,d1
	bra.s	AniO4c
AniO4b	add.w	#$00C0/2,d1
AniO4c:	addq.l	#1,a0
	move.w	0(a4,d1.w),(a2)+
AniPp2:	bsr	AniChr
	cmp.b	#"(",d0
	bne	AniRe
	bra.s	AniP2
* = Col(n)
AniCol:	move.w	$005C/2(a4),(a2)+
AniPp1:	bsr	AniChr
	cmp.b	#"(",d0
	bne	AniRe
	bsr	AniExp
	bsr	AniChr
	cmp.b	#")",d0
	bne	AniRe
	rts
* = Spr Col(n,x,y)
AniSCol	move.w	$00B8/2(a4),(a2)+
	bra.s	AniCo
* = Bob Col(n,x,y)
AniBCol:move.w	$00BC/2(a4),(a2)+
AniCo:	bsr	AniChr
	cmp.b	#"C",d0
	bne	AniRe
AniPp3:	bsr	AniChr
	cmp.b	#"(",d0
	bne	AniRe
	bsr	AniExp
	bsr	AniChr
	cmp.b	#",",d0
	bne	AniRe
AniP2:	bsr	AniExp
	bsr	AniChr
	cmp.b	#",",d0
	bne	AniRe
	bsr	AniExp
	bsr	AniChr
	cmp.b	#")",d0
	bne	AniRe
	rts
* Poke!
AniO5:	move.w	0(a4,d1.w),(a2)
	move.l	d2,2(a2)
	add.w	d3,a2
	rts
* =H(xx)
AniHaz	move.w	$D0/2(a4),(a2)+
	bra	AniPp1
* =V(xx)
AniVu	move.w	$D4/2(a4),(a2)+
	bra	AniPp1

******* EXPRESSION
AniExp:	bsr	AniOpe
AniE0:	bsr	AniChr
	moveq	#$0090/2,d1
	cmp.b	#"=",d0
	beq.s	AniEx1
	addq.w	#4,d1
	cmp.b	#"<",d0
	bne.s	AniEx0
	cmp.b	#">",(a0)
	bne.s	AniEx1
	subq.w	#2,d1
	addq.l	#1,a0
	bra.s	AniEx1
AniEx0:	addq.w	#2,d1
	cmp.b	#">",d0
	beq.s	AniEx1
	addq.w	#2,d1
	cmp.b	#"+",d0
	beq.s	AniEx1
	addq.w	#2,d1
	cmp.b	#"-",d0
	beq.s	AniEx1
	addq.w	#2,d1
	cmp.b	#"/",d0
	beq.s	AniEx1
	addq.w	#2,d1
	cmp.b	#"*",d0
	beq.s	AniEx1
	addq.w	#2,d1
	cmp.b	#"|",d0
	beq.s	AniEx1
	addq.w	#2,d1
	cmp.b	#"&",d0
	beq.s	AniEx1
	cmp.b	#"!",d0
	beq.s	AniXor
* Fin de l'expression
	move.w	$7c/2(a4),(a2)+
	subq.l	#1,a0
	rts
* Special pour XOR
AniXor	moveq	#$00D8/2,d1
* Cherche le deuxieme operande
AniEx1	move.w	0(a4,d1.w),-(sp)
	bsr	AniOpe
	move.w	(sp)+,(a2)+
	bra	AniE0

***********************************************************
*	INITIALISATION AMAL
		xdef	_w_amal_init
_w_amal_init
AMALInit:
********
	clr.l	T_AmDeb(a5)
	clr.l	T_AmChaine(a5)
	move.w	#$1234,T_AmSeed(a5)
***********************************************************
*	CLEAR AMAL
ClrAMAL:
*******
	bsr	DAllAMAL
	lea	T_AmRegs(a5),a0
	moveq	#26/2-1,d0
ClAm:	clr.l	(a0)+
	dbra	d0,ClAm
	moveq	#0,d0
	rts

***********************************************************
*	SYNCHRO: D1= on/off
SyncO:	move.w	d1,T_SyncOff(a5)
	moveq	#0,d0
	rts
Sync:	tst	T_SyncOff(a5)
	beq.s	Sync1
	movem.l	d2-d7/a2-a6,-(sp)
	bsr	Animeur
	movem.l	(sp)+,d2-d7/a2-a6
Sync1:	rts

***********************************************************
*	RAMENE L'ADRESSE UN REGISTRE
*	D1= -1-> generaux / # du mouvement
*	D2= type du mouvement
*	D3= numero du registre
RegAMAL:
*******
	lea	T_AmRegs(a5),a0
	lsl.w	#1,d3
	tst.w	d1
	bmi.s	RgA3
* Registres internes
	move.l	T_AmDeb(a5),d0
	beq.s	RgA1
	lsl.w	#2,d1
	add.w	d2,d1
RgA0	move.l	d0,a1
	cmp.w	AmNb(a1),d1
	beq.s	RgA2
	bcs.s	RgA1
	move.l	AMNext(a1),d0
	bne.s	RgA0
RgA1	moveq	#-1,d0
	rts
RgA2:	lea	AmIRegs+NbInterne*2-2(a1),a0
	neg.w	d3
RgA3:	add.w	d3,a0
	moveq	#0,d0
	rts

***********************************************************
*	AMAL PLAY
*	D1= Debut
*	D2= Fin
*	D3= Speed (R0)
*	D4= Direction (R1)
SetPlay:
*******
	move.l	T_AmDeb(a5),d0
	beq.s	StPx
	lsl.w	#2,d1
	lsl.w	#2,d2
	move.l	#EntNul,d5
StP1:	move.l	d0,a1
	move.w	AmNb(a1),d0
	cmp.w	d2,d0
	bhi.s	StPx
	cmp.w	d1,d0
	bcs.s	StP3
	cmp.l	d5,d3
	beq.s	StP2
	move.w	d3,AmIRegs+NbInterne*2-2(a1)
StP2:	cmp.l	d5,d4
	beq.s	StP3
	move.w	d4,AmIRegs+NbInterne*2-4(a1)
StP3:	move.l	AMNext(a1),d0
	bne.s	StP1
StPx:	moveq	#0,d0
	rts

***********************************************************
*	CREATION / REMPLACEMENT D'UNE ANIMATION
*	A1=	Chaine a tokeniser
*	A2/D2=	Buffer de tokenisation
*	D1=	Buffer variables
*	D3= 	0-> Amal / 1-> Anim / 2-> X / 3-> Y
*	D4= 	0-> Sprite / 1-> Bobs / 2-> Ecrans / 3-> Adresse
*	D5= 	# De l'objet
*	D6= 	# du Canal
CreAMAL:
********
******* Va tokeniser!
	bsr	TokAMAL
	tst.w	d0
	beq.s	CreA0
	bpl	CreAmE
* Pas assez de place! Reserve et RE-tokenise!
	add.w	#AmStart+4,a0
	move.l	a0,d0
	move.l	d0,d2
	bsr	FastMm
	beq	CreME
	move.l	d0,a2
	move.w	d2,AmLong(a2)
	sub.l	#AmStart+4,d2
	lea	AmStart(a2),a2
	bsr	TokAMAL
	lea	-AmStart(a2),a2
	sub.l	a0,a0
* Ok!
CreA0:	move.l	a0,d1
	move.w	d6,d2

******* Insere dans la liste
	clr.l	T_AmChaine(a5)
	clr.l	T_AmFreeze(a5)
******* ID complet!
	lsl.w	#2,d2
	add.w	d3,d2

******* Efface l'ancienne - SI PRESENT -
	move.l	a1,a0
	move.l	T_AmDeb(a5),d0
	beq.s	CreAm1
CreAm0:	move.l	d0,a1
	cmp.w	AmNb(a1),d2
	beq.s	CreAm2
	bcs.s	CreAm5
	move.l	AmNext(a1),d0
	bne.s	CreAm0
* Met a la fin!
	bsr	ResAMAL
	bne	CreAmE
	move.l	a1,AmPrev(a0)
	move.l	a0,AmNext(a1)
	bra.s	CreAm10
* Au tout debut
CreAm1:	bsr	ResAMAL
	bne	CreAmE
	move.l	a0,T_AmDeb(a5)
	bra.s	CreAm10
* Remplace l'ancienne chaine
CreAm2:	move.l	AmPrev(a1),d6
	move.l	AmNext(a1),d7
	moveq	#0,d0
	move.w	AmLong(a1),d0
	bsr	FreeMm
	bsr	ResAMAL
	move.w	d0,-(sp)
	move.l	d6,AmPrev(a0)
	move.l	d7,AmNext(a0)
	beq.s	CreAm3
	move.l	d7,a1
	move.l	a0,AmPrev(a1)
CreAm3:	tst.l	d6
	bne.s	CreAm4
	move.l	a0,T_AmDeb(a5)
	bra.s	CreAm4a
CreAm4:	move.l	d6,a1
	move.l	a0,AmNext(a1)
CreAm4a	move.w	(sp)+,d0
	beq.s	CreAm10
	bra.s	CreAmE
* Insere la nouvelle
CreAm5:	bsr	ResAMAL
	bne	CreAmE
	move.l	AmPrev(a1),d0
	move.l	a0,AmPrev(a1)
	move.l	d0,AmPrev(a0)
	bne.s	CreAm6
	move.l	T_AmDeb(a5),d3
	move.l	a0,T_AmDeb(a5)
	bra.s	CreAm7
CreAm6:	move.l	d0,a2
	move.l	AmNext(a2),d3
	move.l	a0,AmNext(a2)
CreAm7:	move.l	d3,AmNext(a0)
******* Pas d'erreur!
CreAm10	move.l	T_AmDeb(a5),T_AmChaine(a5)
	moveq	#0,d0
	rts
******* Erreur!
CreAmE:	tst.w	d0
	rts
CreME:	moveq	#-1,d0
	bra.s	CreAME

******* CREATION DE LA TABLE!
ResAMAL:tst.l	d1
	beq.s	ResDeja
* Reserve la memoire pour la table, et copie le buffer!
	move.l	a2,a0
	add.w	#AmStart+4,d1
	moveq	#0,d0
	move.w	d1,d0
	bsr	FastMm
	beq	ResAErr
	move.l	d0,a2
	move.w	d1,AmLong(a2)
	move.w	d2,AmNb(a2)
	move.l	a1,-(sp)		* Copie la table
	lea	AmStart(a2),a1
	move.l	a1,AmPos(a2)
	sub.w	#AmStart+4,d1
	lsr.w	#2,d1
ResAc:	move.l	(a0)+,(a1)+
	dbra	d1,ResAc
	move.l	(sp)+,a1
	bra.s	ResPas
* Table deja reservee!
ResDeja	move.w	d2,AmNb(a2)
	lea	AmStart(a2),a0
	move.l	a0,AmPos(a2)

* Calcule l'adresse ACT
ResPas:	move.l	a2,a0
	cmp.w	#6,d4
	beq	ResRain
	cmp.w	#5,d4
	beq	ResAdd
	subq.w	#1,d4
	bmi.s	ResAs
	beq.s	ResAb

******* Creation ECRANS
	lsl.w	#2,d5
	lea	T_EcAdr(a5),a2
	move.l	0(a2,d5.w),d5
	beq.s	ResAee
	move.l	d5,a2
	subq.w	#2,d4
	bmi.s	ResAe3
	beq.s	ResAe2
* Ecrans OFFSET
	add.w	#EcAV,a2
	bra.s	ResAe4
* Ecrans TAILLE
ResAe2:	add.w	#EcAWT,a2
	bra.s	ResAe4
* Ecrans position
ResAe3:	add.w	#EcAW,a2
ResAe4:	move.l	a2,AmAct(a0)
	move.w	#$8000+BitEcrans,AmBit(a0)
	moveq	#0,d0
	rts
* Erreur! Screen not opened
ResAee:	moveq	#-3,d0
	bra.s	ResAEx

******* Creation BOBS
ResAb:	move.w	d5,d1
	movem.l	a0/a1,-(sp)
	bsr	BobAd
	bne.s	ResAbe
	lea	BbAct(a1),a2
	movem.l	(sp)+,a0/a1
	move.l	a2,AmAct(a0)
	move.w	#$8000+BitBobs,AmBit(a0)
	moveq	#0,d0
	rts
* Erreur! BOB not defined
ResAbe:	movem.l	(sp)+,a0/a1
	moveq	#-24,d0
	bra.s	ResAEx

******* Creation SPRITES
ResAs:	lsl.w	#3,d5
	lea	T_HsTAct(a5),a2
	add.w	d5,a2
	move.l	a2,AmAct(a0)
	move.w	#$8000+BitSprites,AmBit(a0)
	moveq	#0,d0
	rts

******* Creation ADRESSE
ResAdd:	move.l	d5,AmAct(a0)
	move.l	d4,d0
	swap	d0
	and.w	#$00FF,d0
	add.w	#$8000,d0
	move.w	d0,AmBit(a0)
	moveq	#0,d0
	rts

******* Creation RAINBOW
ResRain	lea	T_RainTable(a5),a2
	mulu	#RainLong,d5
	lea	RnAct(a2,d5.w),a2
	move.l	a2,AmAct(a0)
	move.w	#$8000+BitEcrans,AmBit(a0)
	moveq	#0,d0
	rts

******* Erreur memoire!
ResAErr	moveq	#-1,d0
	rts
******* Efface la table si ERREUR!
ResAex:	move.l	d0,-(sp)
	move.l	a0,a1
	moveq	#0,d0
	move.w	AmLong(a1),d0
	bsr	FreeMm
	move.l	(sp)+,d0
	rts

***********************************************************
*	ARRET D'UN CANAL avec une adresse ACTUALISATION
*	A0= Adresse ACTUALISATION
		xdef	_w_dad_amal
_w_dad_amal
DAdAMAL:
********
	movem.l	d1-d7/a0-a2,-(sp)
	move.l	T_AmDeb(a5),d6
	clr.l	T_AmChaine(a5)
	clr.l	T_AmFreeze(a5)
	move.l	d6,d5
	beq	MvOx
	move.l	a0,d7
DAdAM1:	move.l	d5,a1
	cmp.l	AmAct(a1),d7
	beq.s	DAdAM2
	move.l	AmNext(a1),d5
	bne.s	DAdAM1
	bra	MvOx
DAdAM2:	move.l	AmNext(a1),d5
	bsr	DAMAL
	tst.l	d5
	bne.s	DAdAM1
	bra	MvOx

***********************************************************
*	ADRESSE D'UN CANAL: D1= Numero!
*	D6= AmDeb!
AdAMAL:
*******
	move.l	d6,d5
	beq.s	AdAmal1
	lsl.w	#2,d1
AdAmal0	move.l	d5,a1
	move.w	AmNb(a1),d0
	and.w	#$FFFC,d0
	cmp.w	d0,d1
	beq.s	AdAmal2
	bcs.s	AdAmal3
	move.l	AMNext(a1),d5
	bne.s	AdAmal0
AdAmal1 rts
AdAmal2 move.l	AmNext(a1),d5
	move.w	AmNb(a1),d7
	and.w	#$0003,d7
	moveq	#-1,d0
	rts
AdAmNx:	tst.l	d5
	bne.s	AdAmal0
	rts
AdAmal3 moveq	#0,d0
	rts

***********************************************************
*	=AMAL ADRESSE
*	D1= #
TAmAd:	move.l	T_AmDeb(a5),d6
	bsr	AdAmal
	move.l	a1,d1
	tst.w	d0
	rts
***********************************************************
*	=MOVON
*	D1= #
TMovon	move.l	T_AmDeb(a5),d6
	bsr	AdAmal
	beq.s	TMvo1
TMvo0	cmp.w	#2,d7
	beq.s	TMvo3
	cmp.w	#3,d7
	beq.s	TMvo3
	bsr	AdAmNx
	bne.s	TMvo0
TMvo1	moveq	#0,d1
	rts
TMvo3	tst.w	AmBit(a1)
	bmi.s	TMvo1
	tst.l	AmAJsr(a1)
	beq.s	TMvo1
TMvo2	moveq	#-1,d1
	rts
***********************************************************
*	= CHANAN
*	D1= #
RChan	move.l	T_AmDeb(a5),d6
	bsr	AdAmal
	beq.s	RChan1
	tst.w	AmBit(a1)
	bpl.s	RChan2
RChan1	addq.l	#4,sp
	moveq	#0,d1
RChan2	rts
TChanA	bsr	RChan
	tst.l	AmAJsr(a1)
	beq.s	TMvo1
	bne.s	TMvo2
TChanM	bsr	RChan
	tst.l	AmPos(a1)
	beq.s	TMvo1
	bne.s	TMvo2

***********************************************************
*	OFF/ON/FREEZE
*	D1= -1 -> TOUS / D1=#
*	D2= Bit a 1--> A Changer
*	D3= -1-> OFF / 0-> Freeze / 1-> On
MvOAMAL:
*******
	movem.l	d1-d7/a0-a2,-(sp)
	move.l	T_AmDeb(a5),d6
	clr.l	T_AmFreeze(a5)
	clr.l	T_AmChaine(a5)

	tst.w	d1
	bmi.s	MvOAll
	bsr	AdAMAL
	beq.s	MvOx
******* Efface
MvO1:	btst	d7,d2
	beq.s	MvO2
	bsr	OnOfFrz
MvO2:	bsr	AdAMNx
	bne.s	MvO1

* FINI!
MvOx:	move.l	d6,T_AmDeb(a5)
	move.l	d6,T_AmChaine(a5)
	movem.l	(sp)+,d1-d7/a0-a2
	moveq	#0,d0
	rts

******* ARRETE TOUT!
MvOAll:	move.l	d6,d5
	beq.s	MvOx
MvOA1:	move.l	d5,a1
	move.l	AmNext(a1),d5
	move.w	AmNb(a1),d7
	and.w	#$0003,d7
	btst	d7,d2
	beq.s	MvOA2
	bsr	OnOfFrz
MvOA2:	tst.l	d5
	bne.s	MvOA1
	bra	MvOx

******************************************* ENLEVE / MET / FREEZE
OnOfFrz	tst.w	d3
	bmi.s	DAMAL
	beq.s	OoF
* On!
	and.w	#$7FFF,AmBit(a1)
	rts
* Freeze
OoF:	or.w	#$8000,AmBit(a1)
	rts
****************************************** Enleve!
*	D6= AmDeb!
DAMAL:	movem.l	d0-d3,-(sp)
	move.l	AmNext(a1),d3
	move.l	AmPrev(a1),d2
	beq.s	DAMAL3
	move.l	d2,a0
	move.l	d3,AmNext(a0)
	bra.s	DAMAL4
DAMAL3:	move.l	d3,d6
DAMAL4:	tst.l	d3
	beq.s	DAMAL5
	move.l	d3,a0
	move.l	d2,AmPrev(a0)
DAMAL5:	move.w	AmLong(a1),d0
	ext.l	d0
	bsr	FreeMm
	movem.l	(sp)+,d0-d3
	rts

***********************************************************
*	EFFACEMENT TOUS CANAUX
DAllAMAL:
		xdef	_w_amal_end
_w_amal_end
AMALEnd:
********
	moveq	#-1,d1
	moveq	#%1111,d2
	moveq	#-1,d3
	bra	MvOAMAL

***********************************************************
*	FREEZE AMAL
FrzAMAL:
********
	tst.l	T_AmFreeze(a5)
	bne.s	FrzA
	move.l	T_AmChaine(a5),T_AmFreeze(a5)
	clr.l	T_AmChaine(a5)
FrzA:	moveq	#0,d0
	rts
**********************
*	UNFREEZE AMAL
********
UFrzAMAL:
	tst.l	T_AmChaine(a5)
	bne.s	UFrzA
	move.l	T_AmFreeze(a5),d0
	beq.s	FrzA
	move.l	d0,T_AmChaine(a5)
UFrzA:	clr.l	T_AmFreeze(a5)
	bra.s	FrzA


******* BRANCHEMENT AUX FONCTIONS
AmJumps	dc.w 	AmStop-AmJumps			* 00
	dc.w	AmAnim-AmJumps			* 04
	dc.w	AmMvtX-AmJumps			* 08
	dc.w 	AmMvtY-AmJumps			* 0C
	dc.w	AmWait-AmJumps			* 10
	dc.w	AmPose-AmJumps			* 14
	dc.w	AmMove-AmJumps			* 18
	dc.w	AmJump-AmJumps			* 1C
	dc.w	AmLet-AmJumps			* 20
	dc.w	AmIf-AmJumps			* 24
	dc.w	AmFor-AmJumps			* 28
	dc.w	AmNxt-AmJumps			* 2C
	dc.w	AmAEg-AmJumps			* 30
	dc.w	AmXEg-AmJumps			* 34
	dc.w	AmYEg-AmJumps			* 38
	dc.w	AmREg-AmJumps			* 3C
	dc.w	AmEgA-AmJumps			* 40
	dc.w	AmEgX-AmJumps			* 44
	dc.w	AmEgY-AmJumps			* 48
	dc.w	AmEgR-AmJumps			* 4C
	dc.w	AmOn-AmJumps			* 50
	dc.w	AmXS-AmJumps			* 54
	dc.w	AmYS-AmJumps			* 58
	dc.w	AmCol-AmJumps			* 5C
	dc.w	AmAOn-AmJumps			* 60
	dc.w	AmAOff-AmJumps			* 64
	dc.w	AmAExit-AmJumps			* 68
	dc.w	AmDirect-AmJumps		* 6C
	dc.w	AmChif-AmJumps			* 70
	dc.w	AmXMou-AmJumps			* 74
	dc.w	AmYMou-AmJumps			* 78
	dc.w	AmSExp-AmJumps			* 7C
	dc.w	AmJ0-AmJumps			* 80
	dc.w	AmJ1-AmJumps			* 84
	dc.w	AmM1-AmJumps			* 88
	dc.w	AmM2-AmJumps			* 8C
	dc.w	AmEg-AmJumps			* 90
	dc.w	AmDif-AmJumps			* 94
	dc.w	AmInf-AmJumps			* 98
	dc.w	AmSup-AmJumps			* 9C
	dc.w	AmPlus-AmJumps			* A0
	dc.w	AmMoins-AmJumps			* A4
	dc.w	AmDiv-AmJumps			* A8
	dc.w	AmMult-AmJumps			* AC
	dc.w	AmOr-AmJumps			* B0
	dc.w	AmAnd-AmJumps			* B4
	dc.w	AmSCol-AmJumps			* B8
	dc.w	AmBCol-AmJumps			* BC
	dc.w	AmPlay-AmJumps			* C0
	dc.w	AmXH-AmJumps			* C4
	dc.w	AmYH-AmJumps			* C8
	dc.w	AmAni-AmJumps			* CC
	dc.w 	AmHaz-AmJumps			* D0
	dc.w	AmVu-AmJumps			* D4
	dc.w	AmXor-AmJumps			* D8

***********************************************************
*	ANIMEUR / DEPLACEUR
		xdef	_w_animator
_w_animator
Animeur:
********
	lea	AMJumps(pc),a4
	move.l	T_AMChaine(a5),d7
	beq.s	AMX
	move.l	d7,d0
	clr.l	T_AMChaine(a5)
	move.w	T_Actualise(a5),d5

******* Depart animation!
AmRE:	move.l	d0,a6
	move.w	AmBit(a6),d4
	bmi.s	AmL2

******* Boucle de l'autotest
	move.l	AmAuto(a6),d0
	beq.s	AmL1
	move.l	d0,a3
	moveq	#20,d6
	move.w	(a3)+,d0
	jsr	0(a4,d0.w)

******* Boucle normale
AmL1:	move.l	AmPos(a6),d0
	beq.s	AmRet
	move.l	d0,a3
	moveq	#10,d6
	move.w	(a3)+,d0
	jsr	0(a4,d0.w)

******* Appel de l'animation?
AmRet:	move.l	AmAJsr(a6),d0
	beq.s	AmL2
	move.l	d0,a0
	jsr	(a0)

******* Un Autre?
AmL2:	move.l	AmNext(a6),d0
	bne.s	AmRE

* Rebranche les inters
	move.w	d5,T_Actualise(a5)
	move.l	d7,T_AmChaine(a5)
AmX:	rts

******* MOVE x,y,step
AmMove:	subq.w	#1,AmCpt(a6)
	bmi.s	AmMvI
	beq.s	AmMvX
* Un cran
	move.l	AmAct(a6),a0
	move.w	2(a0),d0
	move.w	d0,d2
	swap	d0
	move.w	AmVirgX(a6),d0
	move.w	4(a0),d1
	move.w	d1,d3
	swap	d1
	move.w	AmVirgY(a6),d1
	add.l	AmDeltX(a6),d0
	add.l	AmDeltY(a6),d1
AmMv0:	move.w	d0,AmVirgX(a6)
	move.w	d1,AmVirgY(a6)
	swap	d0
	swap	d1
	cmp.w	d2,d0
	beq.s	AmMv1
	move.w	d0,2(a0)
	bset	#1,(a0)
	bset	d4,d5
AmMv1:	cmp.w	d3,d1
	beq.s	AmMv2
	move.w	d1,4(a0)
	bset	#2,(a0)
	bset	d4,d5
AmMv2:	rts
* Mvt suivant
AmMvX:	move.l	AmFin(a6),a3
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
* Init! Calcule les pentes
AmMvI:	lea	-2(a3),a0
	move.l	a0,AmPos(a6)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.l	a3,AmFin(a6)
	tst.w	d3
	bgt.s	AmMi1
	moveq	#1,d3
AmMi1:	move.w	d3,AmCpt(a6)
	move.w	(sp)+,d1
	move.w	(sp)+,d0
	moveq	#0,d2
	ext.l	d0
	bpl.s	AmMi2
	neg.l	d0
	bset	#0,d2
AmMi2:	ext.l	d1
	bpl.s	AmMi3
	neg.l	d1
	bset	#1,d2
AmMi3:	lsl.l	#8,d0
	lsl.l	#8,d1
	divu	d3,d0
	bvc.s	AmMi4
	moveq	#0,d0
AmMi4:	divu	d3,d1
	bvc.s	AmMi5
	moveq	#0,d1
AmMi5:	btst	#0,d2
	beq.s	AmMi6
	neg.w	d0
AmMi6:	btst	#1,d2
	beq.s	AmMi7
	neg.w	d1
AmMi7:	ext.l	d0
	ext.l	d1
	lsl.l	#8,d0
	lsl.l	#8,d1
	move.l	d0,AmDeltX(a6)
	move.l	d1,AmDeltY(a6)
	move.l	AmAct(a6),a0
	move.w	2(a0),d2
	swap	d2
	move.w	#$8000,d2
	move.w	4(a0),d3
	swap	d3
	move.w	#$8000,d3
	add.l	d2,d0
	add.l	d3,d1
	bra	AmMv0

******* PLAY n
AmPlay:	subq.w	#1,AmCpt(a6)
	bmi	AmPli
	bne	AmX
AmPl0:	move.w	AmIRegs+NbInterne*2-4(a6),d2
	bmi	AmMvX
	move.l	AmAct(a6),a0
* Pas suivant en X
	move.l	AmDeltX(a6),a1
	move.b	(a1),d0
	beq	AmMvX
	bmi	AmPWx
	lsl.b	#1,d0
	asr.b	#1,d0
	ext.w	d0
	bset	#1,(a0)
	bset	d4,d5
	tst.w	d2
	beq.s	AmPx1
	add.w	d0,2(a0)
	addq.l	#1,AmDeltX(a6)
	bra.s	AmPy
AmPx1:	sub.w	d0,2(a0)
	subq.l	#1,AmDeltX(a6)
	bra.s	AmPy
* Attente en X!
AmPWx:	subq.w	#1,AmVirgX(a6)
	beq.s	Ampwx1
	bpl.s	Ampy
	and.w	#$7f,d0
	beq.s	Ampwx1
	move.w	d0,AmVirgX(a6)
	bra.s	AmPy
Ampwx1:	tst.w	d2
	bne.s	Ampwx2
	subq.l	#1,AmDeltX(a6)
	bra.s	AmPy
Ampwx2:	addq.l	#1,AmDeltX(a6)
* Pas suivant en Y
AmPy:	move.l	AmDeltY(a6),a1
	move.b	(a1),d0
	beq	AmMvX
	bmi	AmPWy
	lsl.b	#1,d0
	asr.b	#1,d0
	ext.w	d0
	bset	#2,(a0)
	bset	d4,d5
	tst.w	d2
	beq.s	AmPy1
	add.w	d0,4(a0)
	addq.l	#1,AmDeltY(a6)
	bra.s	Ampwy3
AmPy1:	sub.w	d0,4(a0)
	subq.l	#1,AmDeltY(a6)
	bra.s	Ampwy3
* Attente en Y!
AmPWy:	subq.w	#1,AmVirgY(a6)
	beq.s	Ampwy1
	bpl.s	Ampwy3
	and.w	#$7f,d0
	beq.s	Ampwy1
	move.w	d0,AmVirgY(a6)
	bra.s	Ampwy3
Ampwy1:	tst.w	d2
	bne.s	Ampwy2
	subq.l	#1,AmDeltY(a6)
	bra.s	Ampwy3
Ampwy2:	addq.l	#1,AmDeltY(a6)
Ampwy3:	move.w	AmIRegs+NbInterne*2-2(a6),AmCpt(a6)
	rts
******* Initialisation!
AmPli:	lea	-2(a3),a0
	move.l	a0,AmPos(a6)
	bsr	AmEvalue
	move.l	a3,AmFin(a6)
	move.l	T_AmBank(a5),d0
	beq	AmMvX
	move.l	d0,a0
	cmp.w	4(a0),d3
	bhi	AmMvX
	lsl.w	#1,d3
	beq	AmMvX
	move.w	4+2-2(a0,d3.w),d3
	beq	AmMvX
	lsl.w	#1,d3
	lea	4(a0,d3.w),a0
	move.w	(a0)+,AmIRegs+NbInterne*2-2(a6)
	move.w	#1,AmIRegs+NbInterne*2-4(a6)
	move.w	(a0)+,d0
	lea	-4+1(a0,d0.w),a1
	move.l	a1,AmDeltY(a6)
	addq.l	#1,a0
	move.l	a0,AmDeltX(a6)
	clr.w	AmVirgX(a6)
	clr.w	AmVirgY(a6)
	bra	AmPl0

******* ANIM
AmAni:	bsr	AmEvalue		* Initialise l'animation!
	move.w	d3,AmACLoop(a6)
	move.w	#1,AmACpt(a6)
	lea	2(a3),a0
	move.l	a0,AmAAd(a6)
	move.l	a0,AmAALoop(a6)
	lea	AmDoAni(pc),a0
	move.l	a0,AmAJsr(a6)
	add.w	(a3),a3
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
AmDoAni	subq.w	#1,AmACpt(a6)		* Fait l'animation
	bne.s	AmDAX
	move.l	AmAAd(a6),a3
AmDA0	tst.w	(a3)
	beq.s	AmDA1
	bsr	AmEvalue
	move.l	AmAct(a6),a0
	move.w	d3,6(a0)
	bset	#0,(a0)
	bset	d4,d5
	bsr	AmEvalue
	move.w	d3,AmACpt(a6)
	move.l	a3,AmAAd(a6)
AmDAX	rts
AmDA1	tst.w	AmACLoop(a6)
	beq	AmDA2
	subq.w	#1,AmACLoop(a6)
	beq.s	AmDA3
AmDA2	move.l	AmAALoop(a6),a3
	bra.s	AmDA0
AmDA3	clr.l	AmAJsr(a6)
	rts

******* ST-ANIMATION
AmAnim:	move.w	#1,AmACpt(a6)
	move.l	a3,AmAAd(a6)
	move.l	a3,AmAALoop(a6)
	lea	StAni(pc),a0
	move.l	a0,AmAJsr(a6)
	clr.l	AmPos(a6)
	rts
StAni:	subq.w	#1,AmACpt(a6)
	bne.s	StA1
	move.l	AmAAd(a6),a3
StA0	move.w	(a3)+,d0
	bmi.s	StA2
	move.l	AmAct(a6),a0
	move.w	d0,6(a0)
	bset	#0,(a0)
	bset	d4,d5
	move.w	(a3)+,AmACpt(a6)
	beq.s	StA3
	move.l	a3,AmAAd(a6)
StA1	rts
StA2	cmp.w	#-1,d0
	beq.s	StA3
	move.l	AmAALoop(a6),a3
	bra.s	StA0
StA3	clr.l	AmAJsr(a6)
	rts

******* ST-MOUVEMENT EN X
AmMvtX:	move.w	#1,AmACpt(a6)
	move.l	a3,AmAALoop(a6)
	lea	StMvX(pc),a0
	move.l	a0,AmAJsr(a6)
	clr.l	AmPos(a6)
	moveq	#1,d1
	move.l	AmAct(a6),a0
	lea	2(a0),a1
	bra	StML
******* ST-MOUVEMENT EN Y
AmMvtY:	move.w	#1,AmACpt(a6)
	move.l	a3,AmAALoop(a6)
	lea	StMvY(pc),a0
	move.l	a0,AmAJsr(a6)
	clr.l	AmPos(a6)
	moveq	#2,d1
	move.l	AmAct(a6),a0
	lea	4(a0),a1
	bra	StML
* Entree MOVE X
StMvX	subq.w	#1,AmACpt(a6)
	bne.s	StMXx
	moveq	#1,d1
	move.l	AmAct(a6),a0
	lea	2(a0),a1
	bra.s	StM0
* Entree MOVE Y
StMvY	subq.w	#1,AmACpt(a6)
	bne.s	StMXx
	moveq	#2,d1
	move.l	AmAct(a6),a0
	lea	4(a0),a1
* Fait le mouvement
StM0	move.l	AmAAd(a6),a3
	move.w	(a3)+,AmACpt(a6)
	move.w	(a1),d0
	add.w	(a3)+,d0
	move.w	d0,(a1)
	bset	d1,(a0)
	bset	d4,d5
	cmp.w	AmDeltY(a6),d0		* Condition?
	beq.s	StM2
	subq.w	#1,AmDeltX(a6)		* Encore un mouvement?
	beq.s	StM1
StMXx:	rts
* Mouvement suivant
StM1	addq.l	#2,a3
	tst.w	(a3)
	bne.s	StM4
* Condition realisee: on boucle?
StM2:	move.l	AmAALoop(a6),a3
	tst.w	2(a3)
	beq.s	StMF
StML:	move.w	(a3)+,d0		* Reinitialisation
	cmp.w	#$8000,d0
	beq.s	StM3
	move.w	d0,(a1)
	bset	d1,(a0)
	bset	d4,d5
StM3:	addq.l	#2,a3
	move.w	(a3)+,AmDeltY(a6)
StM4:	move.l	a3,AmAAd(a6)
	move.w	4(a3),AmDeltX(a6)
	rts
StMF:	clr.l	AmAJsr(a6)
	rts

******* AUTOTEST ON
AmAOn:	move.w	(a3)+,d0
	move.l	a3,AmAuto(a6)
	lea	-2(a3,d0.w),a3
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
******* AUTOTEST OFF
AmAOff:	clr.l	AmAuto(a6)
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
******* AUTOTEST EXIT
AmAExit:rts
******* DIRECT label
AmDirect:
	move.w	(a3)+,d0
	lea	-2(a3,d0.w),a0
	move.l	a0,AmPos(a6)
	clr.w	AmCpt(a6)
	rts

******* STOP GENERAL!
AmStop:	clr.l	AmPos(a6)
	clr.l	AmAuto(a6)
	rts
******* WAIT VBL
AmPose:	move.l	a3,AmPos(a6)
	rts
******* WAIT
AmWait:	clr.l	AmPos(a6)
	rts
******* RIEN
AmRien:	move.w	(a3)+,d0
	jmp	0(a4,d0.w)

******* LET
AmLet:	bsr	AmEvalue
	move.w	(a3)+,d0
	jsr	0(a4,d0.w)
	move.w	d3,(a0)
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)

******* IF
AmIf:	bsr	AmEvalue
	tst.w	d3
	bne.s	AmIfV
	addq.l	#4,a3
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
AmIfV:	move.w	(a3)+,d0
	jmp	0(a4,d0.w)

******* FOR
AmFor:	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	lea	T_AmRegs(a5),a0
	move.w	(a3)+,d0
	bpl.s	AmFr0
	lea	AmIRegs+NbInterne*2(a6),a0
AmFr0:	move.w	(sp)+,0(a0,d0.w)
	move.w	d3,(a3)+
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
******* NEXT
AmNxt:	move.w	(a3)+,d0
	lea	-2(a3,d0.w),a1
	lea	T_AmRegs(a5),a0
	move.w	(a1)+,d0
	bpl.s	AnNx0
	lea	AmIRegs+NbInterne*2(a6),a0
AnNx0:	add.w	d0,a0
	addq.w	#1,(a0)
	move.w	(a1)+,d0
	cmp.w	(a0),d0
	bge.s	AmNx1
* Sortie, on continue
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
* On reste dans la boucle
AmNx1:	move.l	a1,AmPos(a6)
	rts

******* JUMP
AmJump:	add.w	(a3),a3
	subq.w	#1,d6
	beq.s	AmJp1
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)
AmJp1	lea	AmRet(pc),a0
	cmp.l	(sp),a0
	bne.s	AmJp2
	move.l	a3,AmPos(a6)
AmJp2	rts

******* A=
AmAEg:	move.l	AmAct(a6),a0
	bset	#0,(a0)
	bset	d4,d5
	addq.l	#6,a0
	rts
******* X=
AmXEg:	move.l	AmAct(a6),a0
	bset	#1,(a0)
	bset	d4,d5
	addq.l	#2,a0
	rts
******* Y=
AmYEg:	move.l	AmAct(a6),a0
	bset	#2,(a0)
	bset	d4,d5
	addq.l	#4,a0
	rts
******* REG=
AmREg:	move.w	(a3)+,d0
	bmi.s	AmREg0
	lea	T_AmRegs(a5),a0
	add.w	d0,a0
	rts
AmREg0:	lea	AmIRegs+NbInterne*2(a6),a0
	add.w	d0,a0
	rts

******* EVALUATION D'EXPRESSION
AmEvalue:
	move.w	(a3)+,d0
	jsr	0(a4,d0.w)
	move.w	d2,d3
AmELoop	move.w	(a3)+,d0
	jsr	0(a4,d0.w)
	move.w	(a3)+,d0
	jmp	0(a4,d0.w)

* =XH(Ec,X)
AmXH:	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d1
	bsr	AmEcTo
	tst.w	EcCon0(a0)
	bpl.s	AmXH0
	asr.w	#1,d2
AmXH0:	add.w	EcWX(a0),d2
	move.w	(sp)+,d3
	rts
* =YH(Ec,Y)
AmYH:	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d1
	bsr	AmEcTo
	add.w	EcWY(a0),d2
	sub.w	#EcYBase,d2
	move.w	(sp)+,d3
	rts
* =XS(Ec,X)
AmXS:	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d1
	bsr	AmEcTo
	sub.w	EcWx(a0),d2
	btst	#7,EcCon0(a0)
	beq.s	AmXs1
	asl.w	#1,d2
AmXs1:	add.w	EcVx(a0),d2
	move.w	(sp)+,d3
	rts
* =YS(e,X)
AmYS:	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d1
	bsr	AmEcTo
	add.w	#EcYBase,d2
	sub.w	EcWy(a0),d2
	add.w	EcVy(a0),d2
	move.w	(sp)+,d3
	rts
* Routine: Ecto
AmEcTo	move.w	d3,d2
	and.w	#$0007,d1
	lsl.w	#2,d1
	lea	T_EcAdr(a5),a0
	move.l	0(a0,d1.w),d1
	beq.s	AmEc1
	move.l	d1,a0
	rts
AmEc1:	addq.l	#4,sp
	moveq	#-1,d2
	move.w	(sp)+,d3
	rts
* = Bob Coll
AmBCol:	move.w	T_SyncOff(a5),d0
	beq.s	AmbbX
	movem.l	d3-d7/a0-a2,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d2
	move.w	(sp)+,d1
	bsr	BbColl
	movem.l	(sp)+,d3-d7/a0-a2
	move.w	d0,d2
	rts
* = Spr Coll
AmSCol:	move.w	T_SyncOff(a5),d0
	beq.s	AmbbX
	movem.l	d3-d7/a0-a2,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d2
	move.w	(sp)+,d1
	bsr	SpColl
	movem.l	(sp)+,d3-d7/a0-a2
	move.w	d0,d2
	rts
AmBbx:	moveq	#0,d2
	rts
* = Col(n)
AmCol:	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	d3,d1
	bsr	GetCol
	move.w	d0,d2
	move.w	(sp)+,d3
	rts
* = Haz(n)
AmHaz:	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	T_AmSeed(a5),d2
	mulu	#$3171,d2
	add.w	Circuits+$06,d2
	addq.w	#1,d2
	move.w	d2,T_AmSeed(a5)
	lsr.l	#8,d2
	and.w	d3,d2
	move.w	(sp)+,d3
	rts
* =Vu(n)
AmVu	move.w	d3,-(sp)
	bsr	AmEvalue
	move.w	(sp)+,d3
	move.l	ExtAdr+0*16(a5),d0
	beq.s	AmVu0
	move.l	d0,a0
	cmp.w	#4,d2
	bcc.s	AmVu0
	add.w	d2,a0
	move.b	(a0),d2
	clr.b	(a0)
	rts
AmVu0	moveq	#0,d2
	rts
* =A
AmEgA:	move.l	AmAct(a6),a0
	move.w	6(a0),d2
	rts
* =X
AmEgX:	move.l	AmAct(a6),a0
	move.w	2(a0),d2
	rts
* =Y
AmEgY:	move.l	AmAct(a6),a0
	move.w	4(a0),d2
	rts
* =RA
AmEgR:	move.w	(a3)+,d0
	bmi.s	AEgR0
	lea	T_AmRegs(a5),a0
	move.w	0(a0,d0.w),d2
	rts
AEgR0:	lea	AmIRegs+NbInterne*2(a6),a0
	move.w	0(a0,d0.w),d2
	rts
* =On
AmOn:	tst.w	AmCpt(a6)
	bmi.s	AmO0
	beq.s	AmO0
AmOnM:	moveq	#-1,d2
	rts
AmO0:	moveq	#0,d2
	rts
AmO1:	moveq	#1,d2
	rts

* =Chiffre
AmChif:	move.w	(a3)+,d2
	rts
* =XMouse
AmXMou:	move.w	T_XMouse(a5),d2
	rts
* =YMouse
AmYMou:	move.w	T_Ymouse(a5),d2
	rts
* =Joy0
AmJ0:	moveq	#0,d1
	bsr	ClJoy
	move.w	d1,d2
	rts
* =Joy1
AmJ1:	moveq	#1,d1
	bsr	ClJoy
	move.w	d1,d2
	rts
* = Mousekey1
AmM1:	moveq	#0,d2
	btst	#6,CiaAPrA
	bne.s	AmMx
	moveq	#-1,d2
AmMx:	rts
* = Mousekey 2
AmM2:	moveq	#0,d2
	btst	#10,$DFF016
	beq.s	AmMx
	moveq	#-1,d2
	rts

******* OPERATEURS

* Fin de l'evaluation
AmSExp:	addq.l	#4,sp
	rts
* =
AmEg:	cmp.w	d3,d2
	bne.s	AmEF
AmEV:	moveq	#-1,d3
	bra	AmELoop
AmEF:	moveq	#0,d3
	bra	AmELoop
* <
AmInf:	cmp.w	d2,d3
	blt.s	AmEV
	bra.s	AmEF
* >
AmSup:	cmp.w	d2,d3
	bgt.s	AmEv
	bra.s	AmEF
* <>
AmDif:	cmp.w	d2,d3
	bne.s	AmEV
	bra.s	AmEF
* +
AmPlus:	add.w	d2,d3
	bra	AmELoop
* -
AmMoins	sub.w	d2,d3
	bra	AmELoop
* /
AmDiv:	tst.w	d2
	beq	AmELoop
	ext.l	d3
	divs	d2,d3
	bra	AmELoop
* *
AmMult:	ext.l	d3
	muls	d2,d3
	bra	AmELoop
* |
AmOr:	or.w	d2,d3
	bra	AmELoop
* !
AmXor:	eor.w	d2,d3
	bra	AmELoop
* &
AmAnd:	and.w	d2,d3
	bra	AmELoop
	ENDC


; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ROUTINES AU MILIEU!
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

; IceStart: W_Base and the contexts of the C++ code (AMOSPro's, see
; src/core/call.s), then src/w/startup.cpp, like StartAll, EndAll, IceEnd
IceStart
	lea	W_Base(pc),a0
	move.l	a5,(a0)
	move.l	T_CBridge(a5),_amos_bridge
	jmp	_w_ice_start
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
	bne.s	.Wb
	move.l	Old_LoadView(pc),-(sp)
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
WMemInit
WMemEnd
WMemCheck
	rts
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
	move.l	a1,-(sp)
	jsr	_w_vbl
	addq.l	#4,sp
	movem.l	(sp)+,d2-d7/a2-a4
	lea	$DFF000,a0
	moveq	#0,d0
	rts

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
	move.l	W_Base(pc),a5
	jsr	_w_input
	move.l	(sp)+,a5
	rts
WSend_FakeEvent	jmp	_w_send_fake_event

; Clear Key: src/w/keyboard.cpp
ClVide	jmp	_w_clear_key

; KEY WAIT, retourne BNE si des touches en attente
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ClKWait	moveq	#0,d0
	move.w	T_ClQueue(a5),d1
	cmp.w	T_ClTete(a5),d1
	rts

; Inkey: src/w/keyboard.cpp
ClInky	jmp	_w_inkey

; Change KEY MAP A1
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ClKeyM	rts

; Key Speed, Key Shift, Key State, Put Key, function keys: src/w/keyboard.cpp
TKSpeed	jmp	_w_key_speed
ClSh:	jmp	_w_key_shift
ClInst:	jmp	_w_key_state
ClPutK:	jmp	_w_put_key
ClFFk:	jmp	_w_set_function_key
ClGFFk:	jmp	_w_get_function_key

; RETOUR L'ETAT DU FLAG DISC
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TGetDisc
	move.w	T_DiscIn(a5),d0
	ext.l	d0
	rts

;	Gestion cyclique hors interruptions
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Cyclic tests outside the interrupts: src/w/input.cpp
WTest_Cyclique	jmp	_w_test_cyclic


; Error "illegal function call" of the zone functions
PErr7:	moveq	#16,d0
	rts



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
	move.l	W_Base(pc),a5
	jsr	_w_auto_req
	move.l	(sp)+,a5
	rts
		xdef	_w_easy_req_entry
_w_easy_req_entry
EasyReq	move.l	a5,-(sp)
	move.l	W_Base(pc),a5
	jsr	_w_easy_req
	move.l	(sp)+,a5
	rts

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
		IncBin	"bin/+AMOSPro_Mouse.abk"

		even
