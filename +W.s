
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
*	COLLISIONS SOFT

******* Routine collision:
*	A3/A2= 	descripteur sprite concerne
*	D0/D1=	bob dest   DX/DY
*	D2=	Image!
*	D4/D5= 	bob source DX/DY
*	D6/D7=	bob source FX/FY
*	A4=	banque!
******* RETOUR: BNE-> Pas coll
*******		BEQ-> Coll
ColRout:
* Pointe le descripteur
	and.w	#$3FFF,d2
	beq	ColRF
	cmp.w	(a4),d2
	bhi	ColRF
	lsl.w	#3,d2
	lea	-8+2(a4,d2.w),a0
	move.l	4(a0),d2
	ble	ColRF
	move.l	d2,a1
	move.l	(a0),d2
	beq	ColRF
	move.l	d2,a0
* Prend les coordonnees
	move.w	6(a0),d2
	lsl.w	#2,d2
	asr.w	#2,d2
	sub.w	d2,d0
	sub.w	8(a0),d1
	move.w	(a0),d2
	lsl.w	#4,d2
	move.w	2(a0),d3
	add.w	d0,d2
	add.w	d1,d3
* Croisement?
	cmp.w	d4,d2
	ble	ColRF
	cmp.w	d6,d0
	bge	ColRF
	cmp.w	d5,d3
	ble	ColRF
	cmp.w	d7,d1
	bge	ColRF
* Verifie avec le blitter!
	movem.l	d4-d7/a2/a3,-(sp)
	cmp.w	d0,d4		* Met le plus a gauche en D0
	bge.s	ColR1
	exg.l	d0,d4
	exg.l	d1,d5
	exg.l	d2,d6
	exg.l	d3,d7
	exg.l	a1,a3
	exg.l	a0,a2
ColR1:	cmp.w	d5,d1
	bge.s	ColR5
	move.w	d5,-(sp)
	sub.w	d1,(sp)
	clr.w	-(sp)
ColR2:	cmp.w	d3,d7
	bge.s	ColR3
	move.w	d7,-(sp)
	bra.s	ColR4
ColR3:	move.w	d3,-(sp)
ColR4:	sub.w	d5,(sp)
	bra.s	ColR7a
ColR5:	clr.w	-(sp)
	move.w	d1,-(sp)
	sub.w	d5,(sp)
	cmp.w	d3,d7
	bge.s	ColR6
	move.w	d7,-(sp)
	bra.s	ColR7
ColR6:	move.w	d3,-(sp)
ColR7:	sub.w	d1,(sp)

ColR7a:	move.w	d4,d1
	sub.w	d0,d1
	cmp.w	d2,d6
	bge.s	ColR8
	move.w	d6,d3
	bra.s	ColR9
ColR8:	move.w	d2,d3
ColR9:	sub.w	d4,d3

	move.w	d1,d0
	lsl.w	#8,d0
	lsl.w	#4,d0
	move.w	d0,BltCon1(a6)
	move.w	#%0000110011000000,BltCon0(a6)
	lsr.w	#4,d3
	tst.w	d0
	beq.s	ColRA
	addq.w	#1,d3
ColRA:	move.w	d3,d4
	move.w	(sp)+,d0
	ble.s	ColRF0
	lsl.w	#6,d0
	or.w	d0,d4
	lsl.w	#1,d3
	move.w	(a0),d0
	lsl.w	#1,d0
;	addq.w	#2,d0
	move.w	d0,d6
	sub.w	d3,d0
	move.w	d0,BltModA(a6)
	move.w	(a2),d0
	lsl.w	#1,d0
;	addq.w	#2,d0
	move.w	d0,d5
	sub.w	d3,d0
	move.w	d0,BltModB(a6)

	mulu	(sp)+,d5
	lea	4(a3,d5.w),a3
	move.l	a3,BltAdB(a6)
	move.w	d1,d0
	lsr.w	#4,d0
	lsl.w	#1,d0
	mulu	(sp)+,d6
	add.w	d0,d6
	lea	4(a1,d6.w),a1
	move.l	a1,BltAdA(a6)
	move.w	#-1,BltMaskD(a6)
	and.w	#$000F,d1
	lsl.w	#1,d1
	lea	MCls(pc),a1
	move.w	0(a1,d1.w),BltMaskG(a6)

	move.w	d4,BltSize(a6)
	movem.l	(sp)+,d4-d7/a2/a3

ColRW:	bsr	BlitWait
	btst	#13,DmaConR(a6)
	rts
* Yapa collision!
ColRF:	moveq	#1,d0
	rts
* Yapa special!
ColRF0:	addq.l	#4,sp
ColRF1:	movem.l	(sp)+,d4-d7/a2/a3
	moveq	#1,d0
	rts

***********************************************************
*	BOB COLLIDE
*	D1= Numero du bob
*	D2= Debut a explorer (Bit 31===> TO SPRITE)
*	D3= Fin a explorer
BbColl:	movem.l	a2-a6/d2-d7,-(sp)
	lea	Circuits,a6
	bsr	OwnBlit
	lea	T_TColl(a5),a0
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	moveq	#0,d7
	move.l	T_SprBank(a5),d0
	beq	BbColX
	move.l	d0,a4
	bsr	BobAd
	bne	BbColX

* Coordonnees du bob a tester!
	move.l	a1,a2
	tst.b	BbAct(a2)
	bmi	BbColX
	move.w	d1,a0
	move.l	BbEc(a2),d1
	move.w	BbX(a2),d4
	move.w	BbY(a2),d5
	move.w	BbI(a2),d0
	and.w	#$3FFF,d0
	beq	BbColX
	cmp.w	(a4),d0
	bhi	BbColX
	lsl.w	#3,d0
	lea	-8+2(a4,d0.w),a2
	move.l	4(a2),d0
	ble	BbColX
	move.l	d0,a3
	move.l	(a2),d0
	beq	BbColX
	move.l	d0,a2
	move.w	6(a2),d0
	lsl.w	#2,d0
	asr.w	#2,d0
	sub.w	d0,d4
	sub.w	8(a2),d5
	move.w	d4,d6
	move.w	d5,d7
	move.w	(a2),d0
	lsl.w	#4,d0
	add.w	d0,d6
	add.w	2(a2),d7
	btst	#31,d2
	bne	GoToSp

	exg.l	a0,a5
	move.l	T_BbDeb(a0),d0
	lea	T_TColl(a0),a0
******* Explore la table des bobs!
BbCol1:	move.l	d0,a1
	move.w	BbNb(a1),d0
	cmp.w	d2,d0
	bcs.s	BbColN
	cmp.w	d3,d0
	bhi.s	BbColX
	cmp.w	a5,d0
	beq.s	BbColN
	cmp.l	BbEc(a1),d1
	bne.s	BbColN
	tst.b	BbAct(a1)
	bmi.s	BbColN
	movem.l	d0-d3/a0/a1,-(sp)
	move.w	BbX(a1),d0
	move.w	BbY(a1),d1
	move.w	BbI(a1),d2
	bsr	ColRout
	movem.l	(sp)+,d0-d3/a0/a1
	bne.s	BbColN
	swap	d2
	and.w	#$00FF,d0
	move.w	d0,d2
	lsr.w	#3,d0
	and.w	#$7,d2
	bset	d2,0(a0,d0.w)
	bset	#31,d7
	swap	d2
BbColN:	move.l	BbNext(a1),d0
	bne.s	BbCol1

******* Fini!
BbColX:	bsr	DOwnBlit
	btst	#31,d7
	bne.s	BbColT
	moveq	#0,d0
	bra.s	BbColXx
BbColT	moveq	#-1,d0
BbColXx	movem.l	(sp)+,a2-a6/d2-d7
	rts

******* Conversion---> HARD
GoToSp:	movem.w	d2-d3,-(sp)
	move.l	d1,a0
	sub.w	d4,d6
	sub.w	d5,d7
	move.w	d4,d1
	move.w	d5,d2
	bsr	CXyS
	move.w	d1,d4
	move.w	d2,d5
	add.w	d4,d6
	add.w	d5,d7
	movem.w	(sp)+,d2-d3
	moveq	#-1,d1
	bra	BbToSp

	IFEQ	EZFlag
***********************************************************
*	SPRITE COLLIDE
*	D1= Numero du sprite
*	D2= Debut a explorer
*	D3= Fin a explorer
SpColl:	movem.l	a2-a6/d2-d7,-(sp)
	lea	Circuits,a6
	bsr	OwnBlit
	lea	T_TColl(a5),a0
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	clr.l	(a0)+
	moveq	#0,d7
	move.l	T_SprBank(a5),d0
	beq	BbColX
	move.l	d0,a4

* Coordonnees du sprite a tester!
	cmp.w	#64,d1
	bcc	BbColX
	lea	T_HsTAct(a5),a2
	move.w	d1,d0
	lsl.w	#3,d0
	add.w	d0,a2
	move.w	2(a2),d4
	move.w	4(a2),d5
	move.w	6(a2),d0
	and.w	#$3FFF,d0
	beq	BbColX
	cmp.w	(a4),d0
	bhi	BbColX
	lsl.w	#3,d0
	lea	-8+2(a4,d0.w),a2
	move.l	4(a2),d0
	ble	BbColX
	move.l	d0,a3
	move.l	(a2),d0
	beq	BbColX
	move.l	d0,a2
	move.w	6(a2),d0
	lsl.w	#2,d0
	asr.w	#2,d0
	sub.w	d0,d4
	sub.w	8(a2),d5
	move.w	d4,d6
	move.w	d5,d7
	move.w	(a2),d0
	lsl.w	#4,d0
	add.w	d0,d6
	add.w	2(a2),d7
	btst	#31,d2
	bne	SpToBb
	ENDC

BbToSp:	cmp.w	#64,d3
	bcc	BbColX
	cmp.w	d2,d3
	bcs	BbColX
	lea	T_TColl(a5),a0
	lea	T_HsTAct(a5),a1
	move.w	d2,d0
	lsl.w	#3,d2
	add.w	d2,a1
	move.w	d3,a5
******* Explore la table des sprites!
SpCol1:	cmp.w	d1,d0
	beq.s	SpColN
	tst.w	(a1)
	bmi.s	SpColN
	move.w	6(a1),d2
	ble.s	SpColN
	movem.l	d0/d1/a0/a1,-(sp)
	move.w	2(a1),d0
	move.w	4(a1),d1
	bsr	ColRout
	movem.l	(sp)+,d0/d1/a0/a1
	bne.s	SpColN
	move.w	d0,d2
	move.w	d0,d3
	lsr.w	#3,d2
	and.w	#$7,d3
	bset	d3,0(a0,d2.w)
	bset	#31,d7
SpColN:	addq.w	#1,d0
	addq.l	#8,a1
	cmp.w	a5,d0
	bls.s	SpCol1
	bra	BbColX

******* Fin de SPRITE/BOB
SpToBb:	move.l	T_BbDeb(a5),d0
	lea	T_TColl(a5),a5
SbCol1:	move.l	d0,a1
	move.w	BbNb(a1),d0
	cmp.w	d2,d0
	bcs	SbColN
	cmp.w	d3,d0
	bhi	BbColX
	tst.b	BbAct(a1)
	bmi	SbColN
	movem.l	d0/d2/d3/a1,-(sp)
	move.l	BbEc(a1),a0
	move.w	BbX(a1),d1
	move.w	BbY(a1),d2
	bsr	CXyS
	move.w	d1,d0
	move.w	d2,d1
	move.w	BbI(a1),d2
	bsr	ColRout
	movem.l	(sp)+,d0/d2/d3/a1
	bne.s	SbColN
	and.w	#$00FF,d0
	move.w	d0,d1
	lsr.w	#3,d0
	and.w	#$7,d1
	bset	d1,0(a5,d0.w)
	bset	#31,d7
SbColN:	move.l	BbNext(a1),d0
	bne.s	SbCol1
	bra	BbColX

***********************************************************
*	=COLL(n) ramene la collision d'un bob/sprite
GetCol:	lea	T_TColl(a5),a0
	tst.l	d1
	bmi.s	GetC2
	and.w	#$FF,d1
	move.w	d1,d0
	lsr.w	#3,d0
	and.w	#$7,d1
	btst	d1,0(a0,d0.w)
	bne.s	GetC1
GetC0:	moveq	#0,d0
	rts
GetC1:	moveq	#-1,d0
	rts
* Ramene le premier en collision
GetC2:	neg.l	d1
	cmp.l	#255,d1
	bcc.s	GetC0
	move.w	d1,d0
	lsr.w	#3,d0
	add.w	d0,a0
	move.l	d1,d0
	and.w	#7,d1
.loop	btst	d1,(a0)
	bne.s	.found
	addq.w	#1,d0
	addq.w	#1,d1
	cmp.w	#8,d1
	bcs.s	.loop
	moveq	#0,d1
	addq.l	#1,a0
	cmp.w	#256,d0
	bcs.s	.loop
	bra.s	GetC0
.found	rts

******* Bobs: src/w/bobs.cpp
SpotH:	jmp	_w_hot_spot
GetBob:	jmp	_w_get_bob

***********************************************************
*	INITIALISATION BOBS / D0= Nombre de bobs!
BbInit:
********
	clr.l	T_BbDeb(a5)
* Efface ce qui etait reserve
	move.w	d0,-(sp)
	bsr	BbEnd
	move.w	(sp)+,d1
* Reserve la memoire pour les tables priorites
	move.w	d1,T_BbMax(a5)
	ext.l	d1
	lsl.w	#2,d1
	move.l	d1,d0
	bsr	FastMm
	beq	GFatal
	move.l	d0,T_BbPrio(a5)
	move.l	d1,d0
	bsr	FastMm
	beq	GFatal
	move.l	d0,T_BbPrio2(a5)
	moveq	#0,d0
	rts

***********************************************************
*	FIN DES BOBS
BbEnd:
*******
	move.w	T_BbMax(a5),d1
	ext.l	d1
	lsl.l	#2,d1
	move.l	T_BbPrio(a5),d0
	beq.s	BOBE1
	move.l	d0,a1
	move.l	d1,d0
	bsr	FreeMm
BOBE1:	move.l	T_BbPrio2(a5),d0
	beq.s	BOBE2
	move.l	d0,a1
	move.l	d1,d0
	bsr	FreeMm
BOBE2:	moveq	#0,d0
	rts

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
RbInit	rts				(src/w/bobs.cpp reverses the bytes itself)
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

***********************************************************
*	DEMARRAGE A FROID DES ECRANS
*	D0= taille memoire pour liste copper
***********************************************************
EcInit:
* Reserve la memoire pour liste copper ecrans
	move.l	#EcTCop,d0
	bsr	FastMm
	beq	GFatal
	move.l	d0,T_EcCop(a5)
* Petit buffer en CHIP pour les operations graphiques
	move.l	#256,d0
	bsr	ChipMm
	beq	GFatal
	move.l	d0,T_ChipBuf(a5)
* Taille affichage par defaut
	move.w	#311+EcYBase,T_EcYMax(a5)	PAL
	move.l	$4.w,a0
	cmp.b	#50,530(a0)			VBlankFrequency=50?
	beq.s	.NoNTSC
	move.w	#261+EcYBase,T_EcYMax(a5)	NTSC!
.NoNTSC
* Autre inits
	bsr	EcRaz
	bsr	EcCopper
	tst.b	T_AMOSHere(a5)
	beq.s	.Skip
	lea	Circuits,a6
	clr.w	CopJmp1(a6)
	move.w	#$82A0,DmaCon(a6)
.Skip
; Installe le vecteur
	lea	EcIn(pc),a0
	move.l	a0,T_EcVect(a5)
	moveq	#0,d0
	rts

**********************************************************
*	ARRET FINAL DES ECRANS
**********************************************************
EcEnd:	moveq	#0,d1
	moveq	#EcMax-1,d2
	bsr	EcDAll
	bsr	RazCBloc
	bsr	RazBloc
	moveq	#-1,d1
	bsr	TrDel
* Efface la memoire du buffer CHIP
	move.l	T_ChipBuf(a5),d0
	beq.s	.skip
	move.l	d0,a1
	move.l	#256,d0
	bsr	FreeMm
.skip
* Efface la memoire liste copper
	move.l	T_EcCop(a5),d0
	beq.s	EcEnd1
	move.l	d0,a1
	move.l	#EcTCop,d0
	bsr	FreeMm
EcEnd1:	rts


******* Jumps to screen functions
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
		xdef	_w_ec_push
_w_ec_push
Ec_Push	movem.l	a0-a1/d0,-(sp)
	tst.w	T_PaPeek(a5)
	bne.s	.Pasave
	move.l	T_RastPort(a5),d0
	beq.s	.Pasave
	move.l	d0,a0
	lea	T_EcSave(a5),a1
	move.b	25(a0),(a1)+		0 EcInkA(a1)
	move.b	26(a0),(a1)+		1 EcInkB(a1)
	move.b	27(a0),(a1)+		2 EcOutL(a1)
	move.b	28(a0),(a1)+		3 EcMode(a1)
	move.w	32(a0),(a1)+		4 EcCont(a1)
	move.w	34(a0),(a1)+		6 EcLine(a1)
	move.w	36(a0),(a1)+		8 EcX(a1)
	move.w	38(a0),(a1)+		10 EcY(a1)
	move.l	8(a0),(a1)+		12 EcPat
	move.b	29(a0),(a1)+		16 EcPatY
	addq.l	#1,a1
	lea	52(a0),a0
	moveq	#14-1,d0		18 Fonte
.Loop	move.b	(a0)+,(a1)+
	dbra	d0,.Loop
; Sauve le clip rectangle
	move.l	T_EcCourant(a5),a0
	move.w	EcClipX0(a0),(a1)+	32
	move.w	EcClipY0(a0),(a1)+	34
	move.w	EcClipX1(a0),(a1)+	36
	move.w	EcClipY1(a0),(a1)+	38
.Pasave	movem.l	(sp)+,a0-a1/d0
	rts

;	Restore les modes graphiques de l'ecran courant
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_w_ec_pull
_w_ec_pull
Ec_Pull	movem.l	a0-a2/d0-d3,-(sp)
	tst.w	T_PaPeek(a5)
	bne.s	.Papull
	lea	T_EcSave(a5),a2
	move.l	T_RastPort(a5),d0
	beq.s	.Papull
	move.l	d0,a1
; Change le RASTPORT
; ~~~~~~~~~~~~~~~~~~
	moveq	#0,d0			Ink A
	move.b	(a2)+,d0
	GfxA5	SetAPen
 	moveq	#0,d0			Ink B
	move.b	(a2)+,d0
	GfxA5	SetBPen
	move.b	(a2)+,27(a1)		OutL
	moveq	#0,d0
	move.b	(a2)+,d0
	GfxA5	SetDrMd			Draw Mode
	move.w	(a2)+,32(a1)		Cont
	move.w	(a2)+,34(a1)		Line
	move.w	(a2)+,36(a1)		X
	move.w	(a2)+,38(a1)		Y
	move.l	(a2)+,8(a1)		EcPat
	move.b	(a2)+,29(a1)		EcPatY
	addq.l	#1,a2
	lea	52(a1),a1		Fonte
	moveq	#14-1,d0
.Loop	move.b 	(a2)+,(a1)+
	dbra	d0,.Loop
; Restore le clip rectangle
; ~~~~~~~~~~~~~~~~~~~~~~~~~
	move.w	(a2)+,d0
	move.w	(a2)+,d1
	move.w	(a2)+,d2
	move.w	(a2)+,d3
	bsr	Ec_SetClip
.Papull	movem.l	(sp)+,a0-a2/d0-d3
	rts

EcView:	jmp	_w_sc_view
EcFirst:	jmp	_w_sc_to_front
EcLast:	jmp	_w_sc_to_back
EcDel:	jmp	_w_sc_close

******* Initialisation des ecrans
EcRaz:	lea	T_EcAdr(a5),a0
	moveq	#EcMax-1,d0
EcR1:	clr.l	(a0)+
	dbra	d0,EcR1
	move.l	#-1,T_EcPri(a5)
	lea	T_CopMark(a5),a0
	move.w	#CopML/4-1,d0
EcR2:	clr.l	(a0)+
	dbra	d0,EcR2
	move.w	#1,T_EcYAct(a5)
	moveq	#0,d0
	rts

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

******* Table des masques
MCls:	dc.w	%1111111111111111
	dc.w 	%0111111111111111
	dc.w 	%0011111111111111
	dc.w	%0001111111111111
	dc.w	%0000111111111111
	dc.w	%0000011111111111
	dc.w	%0000001111111111
	dc.w	%0000000111111111
	dc.w	%0000000011111111
	dc.w	%0000000001111111
	dc.w	%0000000000111111
	dc.w	%0000000000011111
	dc.w	%0000000000001111
	dc.w	%0000000000000111
	dc.w	%0000000000000011
	dc.w	%0000000000000001
MCls2:	dc.w	%0000000000000000
	dc.w	%0000000000000001
	dc.w	%0000000000000011
	dc.w	%0000000000000111
	dc.w	%0000000000001111
	dc.w	%0000000000011111
	dc.w	%0000000000111111
	dc.w	%0000000001111111
	dc.w	%0000000011111111
	dc.w	%0000000111111111
	dc.w	%0000001111111111
	dc.w	%0000011111111111
	dc.w	%0000111111111111
	dc.w	%0001111111111111
	dc.w 	%0011111111111111
	dc.w 	%0111111111111111

;	Active l'ecran D1 - si pas ecran CLONE!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EcMarch	jmp	_w_sc_activate

;	Routine d'activation de l'ecran A0
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_w_ec_active
_w_ec_active
Ec_Active
	move.l	a0,T_EcCourant(a5)
	move.l	a0,d0
	beq.s	.Skip
	move.l	Ec_RastPort(a0),T_RastPort(a5)
	move.l	a1,-(sp)
	lea	WRastPort(pc),a1
	move.l	Ec_RastPort(a0),(a1)
	move.l	(sp)+,a1
.Skip	rts

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
		xdef	_w_ec_set_clip
_w_ec_set_clip
Ec_SetClip
	movem.l	d2-d4/a4/a6,-(sp)
	move.l	T_EcCourant(a5),a4
	tst.l	Ec_Region(a4)
	bne.s	.Deja

; Faut-il creer un cliprect?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	EcClipX0(a4),a0
	cmp.w	(a0)+,d0
	bne.s	.Nou
	cmp.w	(a0)+,d1
	bne.s	.Nou
	cmp.w	(a0)+,d2
	bne.s	.Nou
	cmp.w	(a0)+,d3
	beq.s	.Exit

; Installe la clipping region
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~
.Nou	movem.w	d0-d3,-(sp)			Sauve
	movem.w	d0-d3,-(sp)			Pour structure
	subq.w	#1,4(sp)
	subq.w	#1,6(sp)
	move.l	T_GfxBase(a5),a6
	jsr	_LVONewRegion(a6)		Prend une nouvelle region
	move.l	d0,Ec_Region(a4)
	beq.s	.Out				Out of memory
	move.l	sp,a1
	move.l	d0,a0
	jsr	_LVOOrRectRegion(a6)
	tst.l	d0
	beq.s	.Out
	move.l	T_LayBase(a5),a6		Installe le CLIP
	move.l	Ec_Layer(a4),a0
	move.l	Ec_Region(a4),a1
	jsr	_LVOInstallClipRegion(a6)
	addq.l	#8,sp
	movem.w	(sp)+,d0-d3

; Poke dans les structures
; ~~~~~~~~~~~~~~~~~~~~~~~~
.Deja	lea	EcClipX0(a4),a0
	movem.w	d0-d3,(a0)
	subq.w	#1,d2
	subq.w	#1,d3
	move.l	Ec_Layer(a4),a0			Layer
	move.l	8(a0),a0			ClipRect
	movem.w	d0-d3,16(a0)			Poke les nouveaux!
	bra.s	.Exit

.Out	addq.l	#8,sp
	movem.w	(sp)+,d0-d3
.Exit	movem.l	(sp)+,d2-d4/a4/a6
	rts

******* PAINT: src/w/drawing.cpp
TPaint	jmp	_w_paint

******* SET PATTERN ecran courant!
SPat:	jmp	_w_pattern		src/w/drawing.cpp

******* Efface le pattern de l'ecran courant
		xdef	_w_eff_pat
_w_eff_pat
EffPat:	movem.l	a0-a1/d0-d2,-(sp)
	move.l	T_EcCourant(a5),a0
	move.l	EcPat(a0),d0
	beq.s	EffPx
	move.l	d0,a1
	move.w	EcPatL(a0),d0
	ext.l	d0
	clr.l	EcPat(a0)
	clr.w	EcPatL(a0)
	clr.b	EcPatY(a0)
	bsr	FreeMm
EffPx:	move.l	T_RastPort(a5),a0
	clr.l	8(a0)
	clr.b	29(a0)
	movem.l	(sp)+,a0-a1/d0-d2
	rts

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;         INITIALISATION DU FLASHEUR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FlInit: clr 	T_nbflash(a5)
        move 	#lflash*FlMax-1,d0
        lea	T_tflash(a5),a0
razfl1: clr.b 	(a0)+
        dbra 	d0,razfl1
        rts

; Flash Off, Flash: src/w/colours.cpp
FlStop:	jmp	_w_flash_off
FlStart	jmp	_w_flash


	IFEQ	EZFlag
******* Shift: src/w/colours.cpp
ShStart	jmp	_w_shift

***********************************************************
*	Initialisation de la table des shifts
***********************************************************
ShInit:	lea	T_TShift(a5),a0
	move.w	#LShift/2-1,d0
ShI:	clr.w	(a0)+
	dbra	d0,ShI
	rts

ShStop:	jmp	_w_shift_off

***********************************************************
*	INTERRUPTIONS SHIFTER
***********************************************************
Shifter:lea	T_TShift(a5),a0
	tst.w	(a0)
	beq.s	ShfX
	subq.w	#1,(a0)
	bne.s	ShfX
* Shifte!
	move.w	2(a0),(a0)
	addq.l	#4,a0
	move.l	(a0)+,a1
	move.w	EcNumber(a1),d0
	lsl.w	#7,d0
	lea	0(a3,d0.w),a4
	lea	EcPal(a1),a1
	move.w	(a0)+,d0
	move.w	(a0)+,d1
	move.w	d0,d2
	move.w	d1,d3
	tst.b	(a0)+
	bne.s	Shf6
* En montant!
	move.w	0(a1,d3.w),d5
Shf5:	move.w	-2(a1,d3.w),0(a1,d3.w)
	subq.w	#2,d3
	cmp.w	d2,d3
	bne.s	Shf5
	bra.s	Shf8
* En descendant
Shf6:	move.w	0(a1,d2.w),d5
Shf7:	move.w	2(a1,d2.w),0(a1,d2.w)
	addq.w	#2,d2
	cmp.w	d2,d3
	bne.s	Shf7
* Poke dans les listes copper les couleurs D0-D1
Shf8:	tst.b	(a0)+			* Rotation???
	beq.s	Shf8a
	move.w	d5,0(a1,d2.w)
Shf8a:	move.w	d0,d2
	lsl.w	#1,d2
Shf9:	move.w	0(a1,d0.w),d3
	move.l	a4,a2
	cmp.w	#PalMax*2,d0
	bcs.s	ShfC
	lea	64(a2),a2
ShfC:	move.l	(a2)+,d4
	beq.s	ShfB
ShfA:	move.l	d4,a0
	move.w	d3,2(a0,d2.w)
	move.l	(a2)+,d4
	bne.s	ShfA
ShfB:	addq.w	#2,d0
	addq.w	#4,d2
	cmp.w	d1,d0
	bls.s	Shf9
* Fini!
ShfX	rts
	ENDC

******* Fade: src/w/colours.cpp
FadeTOf	jmp	_w_fade_off
FaStop	jmp	_w_fade_stop
FadeTOn	jmp	_w_fade

***********************************************************
*	INTERRUPTIONS FADEUR
*	Attention! Change A3!!!
***********************************************************
FadeI	tst.w	T_FadeFlag(a5)
	beq.s	FadX
	subq.w	#1,T_FadeCpt(a5)
	beq.s	Fad0
FadX	rts
* Fade!
Fad0	move.w	T_FadeVit(a5),T_FadeCpt(a5)
	move.l	T_FadePal(a5),a1
	move.l	T_FadeCop(a5),d3
	move.w	T_FadeNb(a5),d7
	lea	T_FadeCol(a5),a2
	moveq	#0,d6
* Boucle
Fad1	move.w	(a2)+,d5
	bmi.s	FadN0
	moveq	#0,d4
	moveq	#0,d0
	move.b	(a2)+,d0
	cmp.b	(a2)+,d0		* R
	beq.s	Fad4
	bhi.s	Fad2
	addq.w	#1,d0
	bra.s	Fad3
Fad2	subq.w	#1,d0
Fad3	addq.w	#1,d4
	move.b	d0,-2(a2)
Fad4	moveq	#0,d1
	move.b	(a2)+,d1
	cmp.b	(a2)+,d1		* G
	beq.s	Fad7
	bhi.s	Fad5
	addq.w	#1,d1
	bra.s	Fad6
Fad5	subq.w	#1,d1
Fad6	addq.w	#1,d4
	move.b	d1,-2(a2)
Fad7	moveq	#0,d2
	move.b	(a2)+,d2
	cmp.b	(a2)+,d2		* B
	beq.s	FadA
	bhi.s	Fad8
	addq.w	#1,d2
	bra.s	Fad9
Fad8	subq.w	#1,d2
Fad9	addq.w	#1,d4
	move.b	d2,-2(a2)
* Calcule la couleur
FadA	tst.w	d4
	beq.s	FadN1
	addq.w	#1,d6
	lsl.w	#4,d0
	or.w	d1,d0
	lsl.w	#4,d0
	or.w	d2,d0
* Poke dans l'ecran
	move.w	d0,0(a1,d5.w)
* Poke dans les listes copper
	lsl.w	#1,d5
	move.l	d3,a3
	cmp.w	#PalMax*4,d5
	bcs.s	FadC
	lea	64(a3),a3
FadC	move.l	(a3)+,d1
	beq.s	FadN
FadB	move.l	d1,a4
	move.w	d0,2(a4,d5.w)
	move.l	(a3)+,d1
	bne.s	FadB
* Couleur suivante
FadN	dbra	d7,Fad1
	move.w	d6,T_FadeFlag(a5)
	rts
* Rien dans cette couleur
FadN0	addq.l	#6,a2
	dbra	d7,Fad1
	move.w	d6,T_FadeFlag(a5)
	rts
* Plus rien maintenant
FadN1	move.w	#-1,-8(a2)
	bra.s	FadN


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       INTERRUPTIONS FLASHEUR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
FlInt:  tst.b	T_NbFlash+1(a5)		Autorisee?
	bne.s	FlShXX
	move.b 	T_NbFlash(a5),d7	Nombre en route
	beq.s	FlShXX
	addq.b	#1,T_NbFlash+1(a5)	Inhibe!
        lea	T_tflash-lflash+2(a5),a0
FlShLL	lea	lflash-2(a0),a0
FlShL	move.w 	(a0)+,d0
        beq.s 	FlShLL
; Flashe!
	sub.w 	#1,(a0)			* Compteur
        bne.s 	FlShN
	lea	2(a0),a1
	move.l	(a1)+,a2		* Adresse de l'ecran
	add.w	(a1)+,a1		* Pointe
	move.w	(a1)+,(a0)
	bne.s	Flsh4
	lea	6(a0),a1
	clr.w	(a1)+
	move.w	(a1)+,(a0)
FlSh4:	addq.w	#4,6(a0)		* Pointe le suivant
	move.w	(a1),d2
	move.w	d2,EcPal-2(a2,d0.w)	* Change dans la definition
	lsl.w	#1,d0
	move.w	EcNumber(a2),d1
	lsl.w	#7,d1
	lea	0(a3,d1.w),a2
	cmp.w	#PalMax*4+4,d0
	bcs.s	FlSh5
	lea	64(a2),a2
FlSh5:	move.l	(a2)+,d1		* Change toutes les definitions
	beq.s	FlShN
FlSh6:	move.l	d1,a1
	move.w	d2,2-4(a1,d0.w)
	move.l	(a2)+,d1
	bne.s	FlSh6
; Encore un actif?
FlShN	subq.b	#1,d7
	bne.s	FlShLL
; Fini!
FlShX	subq.b	#1,T_NbFlash+1(a5)	Retabli les interruptions
FlShXX	rts

***********************************************************
*	FABRIQUE LA COPPER LISTE A PARTIR DES ECRANS
*
******* ACTUALISATION DES ECRANS
		xdef	_w_ec_force_cop
_w_ec_force_cop
EcForceCop:				* Entree en forcant le calcul
	addq.w	#1,T_EcYAct(a5)
EcCopper:				* Entree normale
	movem.l	d1-d7/a1-a6,-(sp)

* Suite actualisation
	move.w	T_EcYAct(a5),d7
	lea	T_EcPri(a5),a0
	move.l	(a0)+,d0
	beq	EcA6
	bmi	EcActX
EcAct0:	move.l	d0,a1
* Changement en WY
	move.b	EcAW(a1),d0
	beq.s	EcA2
	btst	#2,d0
	beq.s	EcAct1
	move.w	EcAWY(a1),d1
	add.w	#EcYBase,d1
	bpl.s	EcAa
	moveq	#0,d1
EcAa:	move.w	d1,EcWY(a1)
	addq.w	#1,d7
* Changement en WX
EcAct1:	btst	#1,d0
	beq.s	EcAct2
	move.w	EcAWX(a1),d1
	and.w	#$FFF0,d1
	move.w	d1,EcWX(a1)
* Changement en WTY
EcAct2:	clr.w	EcAW(a1)
EcA2:	move.b	EcAWT(a1),d0
	beq.s	EcA4
	btst	#2,d0
	beq.s	EcAct3
	move.w	EcAWTY(a1),d1
	beq.s	EcAct3
	cmp.w	EcTY(a1),d1
	bcs.s	EcAc
EcAg:	move.w	EcTY(a1),d1
EcAc:	btst	#2,EcCon0+1(a1)
	beq.s	EcA2a
	lsr.w	#1,d1
EcA2a:	move.w	d1,EcWTy(a1)
	addq.w	#1,d7
* Changement en WTX
EcAct3:	btst	#1,d0
	beq.s	EcAct4
	move.w	EcAWTX(a1),d1
	and.w	#$FFF0,d1
	beq.s	EcAct4
	move.w	EcTx(a1),d2
	tst.w	EcCon0(a1)
	bpl.s	EcAe
	lsr.w	#1,d2
EcAe:	cmp.w	d2,d1
	bcs.s	EcAf
	move.w	d2,d1
EcAf:	move.w	d1,EcWTx(a1)
* Changement en OY
EcAct4:	clr.w	EcAWT(a1)
EcA4:	move.b	EcAV(a1),d0
	beq.s	EcA6
	btst	#2,d0
	beq.s	EcAct5
	move.w	EcAVY(a1),EcVY(a1)
* Changement en OX
EcAct5:	btst	#1,d0
	beq.s	EcAct6
	move.w	EcAVX(a1),EcVX(a1)
* Encore un ecran?
EcAct6:	clr.w	EcAV(a1)
EcA6:	move.l	(a0)+,d0
	beq.s	EcA6
	bpl	EcAct0
EcActX:

******* Calcul des nouvelles priorites?
	move.l	T_EcCop(a5),a1

;-----> Si pas de changement Y/TY/Priorite, pas de calcul
	tst.w	d7
	beq	PaDecoup
	clr.w	T_EcYAct(a5)

;-----> Decoupe les ecrans en tranches
	lea	T_EcBuf(a5),a3
	moveq	#0,d2		;Limite en haut
MkD0:	lea	T_EcPri(a5),a2
	move.w	#10000,d3		;Limite en bas
	moveq	#-1,d5
	moveq	#0,d1

MkD1:	addq.w	#4,d1
	move.l	(a2)+,d0
	bmi.s	MkD3
	beq.s	MkD1
	move.l	d0,a0
	tst.b	EcFlags(a0)
	bmi.s	MkD1
	move.w	EcWY(a0),d0
	subq.w	#1,d0
	cmp.w	d2,d0
	bls.s	MkD2
	cmp.w	d3,d0
	bcc.s	MkD2
	move.w	d0,d3
	move.w	d3,d4
	add.w	EcWTy(a0),d4
	addq.w	#1,d4
	move.w	d1,d5
	bra.s	MkD1
MkD2:	add.w	EcWTy(a0),d0
	addq.w	#1,d0
	cmp.w	d2,d0
	bls.s	MkD1
	cmp.w	d3,d0
	bcc.s	MkD1
	move.w	d0,d3
	move.w	d1,d5
	bset 	#15,d5
	bra.s	MkD1

MkD3:	cmp.w	#-1,d5			;Fini?
	beq.s	MkD5
	cmp.w	#EcYStrt-1,d2		;Passe le haut de l'ecran?
	bcc.s	MkD3a
	cmp.w	#EcYStrt-1,d3
	bcs.s	MkD3a
	move.w	#EcYStrt-1,(a3)+	;Marque le haut de l'ecran
	clr.w	(a3)+
	move.w	#$8000,(a3)+

MkD3a:
MkD3b:
	move.w	d3,(a3)+
	move.w	d4,(a3)+
	move.w	d5,(a3)+
; Re-Explore la liste en cas d'egalite SI DEBUT DE FENETRE
	and.w	#$7fff,d5
	lea	T_EcPri(a5),a2
	moveq	#0,d1

MkD4:	addq.w	#4,d1
	move.l	(a2)+,d0
	bmi.s	MkD4a
	beq.s	MkD4
	move.l	d0,a0
	tst.b	EcFlags(a0)
	bmi.s	MkD4
	move.w	EcWY(a0),d0
	subq.w	#1,d0
	cmp.w	d0,d3
	bne.s	MkD4
	cmp.w	d5,d1
	beq.s	MkD4
	move.w	d3,(a3)+
	move.w	d3,d4
	addq.w	#1,d4
	add.w	EcWTy(a0),d4
	move.w	d4,(a3)+
	move.w	d1,(a3)+
	bra.s	MkD4
; Remonte la limite
MkD4a:	move.w	d3,d2
	bra	MkD0
; Fin de la liste
MkD5:	move.w	#-1,(a3)+

PaDecoup:

;-----> Analyse de la table / creation de la liste
	clr.w	T_InterInter(a5)
	lea	T_EcPri(a5),a2
	lea	T_EcBuf(a5),a3
MkA1:	move.w	(a3),d0
	bmi	MkAFin
	move.w	2(a3),d1
	move.w	4(a3),d2
	bmi	MkA4
; Debut d'une fenetre: doit-on l'afficher?
	lea	T_EcBuf(a5),a0
MkA2:	cmp.l	a3,a0
	bcc	MkA8
	tst.w	4(a0)
	bmi.s	MkA3
	cmp.w	(a0),d0
	bcs.s	MkA3
	cmp.w	2(a0),d0
	bcc.s	MkA3
	cmp.w	4(a0),d2
	bcc	MkA10
MkA3:	lea	6(a0),a0
	bra.s	MkA2

; Fin d'une fenetre: doit-on en reafficher une autre?
MkA4:	and.w	#$7FFF,d2
	cmp.w	#$100,d2		;Si fin de l'ecran --> marque!
	beq	MkA9a

	clr.w	d3
MkA4a:	addq.w	#6,d3			;Cherche UN DEBUT devant
	cmp.w	0(a3,d3.w),d0
	bne.s	MkA4b
	tst.w	4(a3,d3.w)
	bmi.s	MkA4a
	lea	0(a3,d3.w),a3		;Va faire le debut!
	bra	MkA1

MkA4b:	lea	T_EcBuf(a5),a0		;Cherche la fenetre a reafficher
	move.w	#1000,d3
MkA5:	cmp.l	a3,a0
	bcc.s	MkA7
	tst.w	4(a0)
	bmi.s	MkA6
	cmp.w	(a0),d0
	bcs.s	MkA6
	cmp.w	2(a0),d0
	bcc.s	MkA6
	cmp.w	4(a0),d3
	bcs.s	MkA6
	move.w	4(a0),d3
MkA6:	lea	6(a0),a0
	bra.s	MkA5
MkA7:	cmp.w	#1000,d3
	beq.s	MkA9
	cmp.w	d2,d3
	bls.s	MkA10
	move.w	d3,d2
; Peut creer la fenetre
MkA8:	move.l	-4(a2,d2.w),a0
	move.w	(a3),d0
	cmp.w	#EcYStrt-1,d0		* Sort en haut?
	bcs.s	MkA10
	move.w	T_EcYMax(a5),d1		* Sort en bas?
	subq.w	#2,d1
	cmp.w	d1,d0
	bcc.s	MkA10
	move.w	d0,(a1)+
	move.l	a0,(a1)+
	btst	#2,EcCon0+1(a0)
	beq.s	MkA10
	move.w	#%100,T_InterInter(a5)
	bra.s	MkA10
; Fin normale de la fenetre
MkA9:	tst.w	d2
	beq.s	MkA10
	move.w	(a3),d0
MkA9a:	cmp.w	#EcYStrt-1,d0
	bcs.s	MkA10
	move.w	T_EcYMax(a5),d1
	subq.w	#1,d1
	cmp.w	d1,d0
	bcc.s	MkA11
	neg.w	d0
	move.w	d0,(a1)+
; Passe a une autre
MkA10:	lea	6(a3),a3
	bra	MkA1
; C'est la fin
MkA11	neg.w	d1
	move.w	d1,(a1)+
* Marque la fin des ecrans
MkAFin:	clr.w	(a1)

*******	Fabrique la liste copper
* 	Avec RAINBOW ou non!
* Plus de screen swap
	clr.w	T_Cop255(a5)
	clr.w	T_InterBit(a5)
	clr.l	T_SwapList(a5)
* Nettoie les marqueurs
	clr.l	T_CopMark+CopL1*0(a5)
	clr.l	T_CopMark+CopL1*0+64(a5)
	clr.l	T_CopMark+CopL1*1(a5)
	clr.l	T_CopMark+CopL1*1+64(a5)
	clr.l	T_CopMark+CopL1*2(a5)
	clr.l	T_CopMark+CopL1*2+64(a5)
	clr.l	T_CopMark+CopL1*3(a5)
	clr.l	T_CopMark+CopL1*3+64(a5)
	clr.l	T_CopMark+CopL1*4(a5)
	clr.l	T_CopMark+CopL1*4+64(a5)
	clr.l	T_CopMark+CopL1*5(a5)
	clr.l	T_CopMark+CopL1*5+64(a5)
	clr.l	T_CopMark+CopL1*6(a5)
	clr.l	T_CopMark+CopL1*6+64(a5)
	clr.l	T_CopMark+CopL1*7(a5)
	clr.l	T_CopMark+CopL1*7+64(a5)
	clr.l	T_CopMark+CopL1*8(a5)
	clr.l	T_CopMark+CopL1*8+64(a5)
	clr.l	T_CopMark+CopL1*9(a5)
	clr.l	T_CopMark+CopL1*9+64(a5)
	clr.l	T_CopMark+CopL1*10(a5)
	clr.l	T_CopMark+CopL1*10+64(a5)
	clr.l	T_CopMark+CopL1*11(a5)
	clr.l	T_CopMark+CopL1*11+64(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*0(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*1(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*2(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*3(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*4(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*5(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*6(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*7(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*8(a5)
	clr.l	T_CopMark+CopL1*12+CopL2*9(a5)
* Copper manuel???
	tst.w	T_CopON(a5)
	beq	PasCop
* Liste copper logique
	move.l	T_CopLogic(a5),a1
	lea	64+4(a1),a1		* Saute les sprites
* Rainbow?
	tst.w	T_RainBow(a5)
	bne.s	CopBow
******* Fabrique NORMAL
MCop0	move.l	T_EcCop(a5),a2
MCop1	move.w	(a2)+,d0
	beq.s	MCopX
	bmi.s	MCop2
* Debut d'un ecran
	move.l	(a2)+,a0
	bsr	EcCopHo
	bra.s	MCop1
* Fin d'un ecran
MCop2	neg.w	d0
	bsr	EcCopBa
	bra.s	MCop1
* Fin de la liste
MCopX:	subq.l	#2,a2
	cmp.l	T_EcCop(a5),a2
	bne.s	.Skip
	move.w	T_EcYMax(a5),d0
	subq.w	#1,d0
	bsr	EcCopBa
.Skip	move.l	#$FFFFFFFE,(a1)+
*******	Swappe les listes
MCopSw	move.l	T_CopLogic(a5),a0
	move.l	T_CopPhysic(a5),a1
	move.l	a1,T_CopLogic(a5)
	move.l	a0,T_CopPhysic(a5)
* Poke dans le copper, si AMOS est la!
	tst.b	T_AMOSHere(a5)
	beq.s	PasCop
	move.l	a0,Circuits+Cop1Lc
	move.w	T_InterInter(a5),T_InterBit(a5)
* Fini!
PasCop	movem.l	(sp)+,d1-d7/a1-a6
	moveq	#0,d0
	rts

******* Actualise les RAINBOWS
CopBow	lea	T_RainTable(a5),a0
	moveq	#0,d4
	moveq	#NbRain-1,d6
	moveq	#0,d5
RainA1	tst.w	RnLong(a0)
	beq.s	RainA5
	addq.w	#1,d5
	tst.w	RnI(a0)
	bmi.s	RainA5
	addq.w	#1,d4
	move.b	RnAct(a0),d7
	beq.s	RainA5
	clr.b	RnAct(a0)
* Taille en Y
	bclr	#0,d7
	beq.s	RainA2
	move.w	RnI(a0),d0
	move.w	d0,RnTY(a0)
	bset	#2,d7
* Position en Y
RainA2	bclr	#2,d7
	beq.s	RainA4
	clr.l	RnDY(a0)
	move.w	RnY(a0),d1
	cmp.w	#28,d1
	bcc.s	RainA3
	moveq	#28,d1
RainA3	move.w	RnTy(a0),d0
	add.w	#EcYBase,d1
	move.w	d1,RnDY(a0)
	add.w	d0,d1
	move.w	d1,RnFY(a0)
* Position de la base
RainA4	bclr	#1,d7
	beq.s	RainA5
	move.w	RnX(a0),d0
	lsl.w	#1,d0
	cmp.w	RnLong(a0),d0
	bcc.s	RainA5
	lsr.w	#1,d0
	move.w	d0,RnBase(a0)
* Rainbow suivant
RainA5	lea	RainLong(a0),a0
	dbra	d6,RainA1
* Securite!
	move.w	d5,T_RainBow(a5)
	tst.w	d4
	beq	MCop0

******* Fabrique la liste
	move.l	T_EcCop(a5),a2
	move.w	#EcYBase,d0
	moveq	#-1,d3
	moveq	#-1,d4
	moveq	#0,d7
Rain1	move.w	(a2)+,d1
	beq	Rain3
	bmi.s	Rain2
* Debut d'un ecran
	bsr	Rain
	move.l	(a2)+,a0
	movem.l	d0/d3-d7,-(sp)
	bsr	EcCopHo
	movem.l	(sp)+,d0/d3-d7
	clr.w	d3
	tst.w	d4
	bmi.s	Rain1e
	cmp.w	#PalMax*4,d4
	bcs.s	Rain1d
	lea	64(a4),a4
Rain1d	move.l	(a4),a0
	move.w	2(a0,d4.w),d3
	bclr	#31,d3
Rain1e	cmp.w	d7,d0
	bcc.s	Rain1a
	move.w	(a3)+,2(a0,d4.w)
	cmp.l	a6,a3
	bcs.s	Rain1a
	move.l	d6,a3
Rain1a	addq.w	#1,d0
	move.w	(a2),d1
	bpl.s	Rain1b
	neg.w	d1
Rain1b	cmp.w	d0,d1
	beq.s	Rain1
	cmp.w	d7,d0
	bcc.s	Rain2a
	move.w	d5,(a1)+
	move.w	(a3)+,(a1)+
	bra.s	Rain1c
* Fin d'un ecran
Rain2	neg.w	d1
	bsr	Rain
	bsr	EcCopBa
	tst.w	d4
	bne.s	Rain1b
	move.w	T_EcFond(a5),d3
	cmp.w	d7,d0
	bcc.s	Rain2a
	move.l	a1,a0			* Recherche la couleur
Rain1z	cmp.w	#$0180,-(a0)
	bne.s	Rain1z
	move.w	(a3)+,2(a0)
Rain1c	cmp.l	a6,a3
	bcs.s	Rain2a
	move.l	d6,a3
Rain2a	addq.w	#1,d0
	bra	Rain1
* Fin des ecrans
Rain3	subq.l	#2,a2
	cmp.l	T_EcCop(a5),a2
	bne.s	.Skip
	move.w	T_EcYMax(a5),d0
	subq.w	#1,d0
	bsr	Rain
	bsr	EcCopBa
.Skip	move.l	#$FFFFFFFE,(a1)+
	bra	MCopSw
******* Fabrique le rainbow ---> Y=D1
RainD1	move.w	d0,d2
	sub.w	#EcYBase,d2
	cmp.w	#256,d2			* Attente -> ligne -> D0
	bcs.s	RainD2
	tst.w	T_Cop255(a5)
	bne.s	RainD2
	move.w	#$FFE1,(a1)+
	move.w	#$FFFE,(a1)+
	addq.w	#1,T_Cop255(a5)
RainD2	lsl.w	#8,d2
	or.w	#$03,d2
	move.w	d2,(a1)+
	move.w	#$FFFE,(a1)+
	move.w	d5,(a1)+		* Change la couleur
	move.w	(a3)+,(a1)+
	cmp.l	a6,a3
	bcs.s	RainD3
	move.l	d6,a3
RainD3	addq.w	#1,d0
* Entree!
Rain	cmp.w	d7,d0
	bcc.s	RainNx
RainD0	cmp.w	d1,d0
	bcs.s	RainD1
RainDX	move.w	d1,d0
	rts
******* Trouve le rainbow comprenant D0
RainNX	tst.l	d3
	bmi.s	RainN0
	tst.w	d3			* Si RIEN au dessus
	bpl.s	Rain0a
	move.w	#Color00,(a1)+		* Couleur 0 d'office!
	move.w	T_EcFond(a5),(a1)+
	bset	#31,d3
	bra.s	RainN0
Rain0a	move.w	d5,(a1)+
	move.w	d3,(a1)+
	bset	#31,d3
RainN0	lea	T_RainTable(a5),a0	* Cherche le 1er
	moveq	#NbRain-1,d2
RainN1	cmp.w	(a0),d0
	bcs.s	RainN2
	cmp.w	RnFY(a0),d0
	bcs.s	RainN5
RainN2	lea	RainLong(a0),a0
	dbra	d2,RainN1
	lea	T_RainTable(a5),a0	* Trouve le 1er plus bas
	moveq	#0,d7
	moveq	#NbRain-1,d2
	move.w	d1,d6
RainN3	cmp.w	RnFY(a0),d0
	bcc.s	RainN4
	cmp.w	(a0),d1
	bcs.s	RainN4
	cmp.w	(a0),d6
	bcs.s	RainN4
	move.w	(a0),d6
	move.l	a0,d7
RainN4	lea	RainLong(a0),a0
	dbra	d2,RainN3
	tst.l	d7
	beq	RainDX
	move.l	d7,a0
	move.w	(a0),d0
* Debut d'un RainBow
RainN5	move.w	d0,d5
	sub.w	(a0),d5
	add.w	RnBase(a0),d5
	lsl.w	#1,d5
	move.l	RnBuf(a0),d6
	move.l	d6,a3
	move.l	a3,a6
	add.w	RnLong(a0),a6
	add.w	d5,a3
	cmp.l	a6,a3
	bcs.s	RainD7
RainN6	sub.w	RnLong(a0),a3
	cmp.l	a6,a3
	bcc.s	RainN6
RainD7	move.w	RnFY(a0),d7
* Nouvelle couleur
	move.w	d4,d2
	move.w	RnColor(a0),d4
	move.w	d4,d5
	lsl.w	#2,d4
	lsl.w	#1,d5
	add.w	#Color00,d5
* Reprend la couleur!
	tst.w	d3
	bmi.s	RainD9
	cmp.w	d4,d2
	beq.s	RainD9
	move.l	(a4),a0
	move.w	2(a0,d4.w),d3
RainD9	bclr	#31,d3
	bra	RainD0

******* Creation de la ligne COPPER de definition d'un ecran!
*	D0=	Y ecran
*	A0= 	Adresse de l'ecran
EcCopHo
* Decalage PHYSIQUE dans la fenetre
	move.w	d0,d1
	sub.w	EcWY(a0),d1
	addq.w	#1,d1
	btst	#2,EcCon0+1(a0)		* par 2 si entrelace!
	beq.s	MkC4a
	lsl.w	#1,d1
MkC4a
* Attend jusqu'a la ligne D0
	move.w	d0,d2
	sub.w	#EcYBase,d2
	bsr	WaitD2
* Preparation image
	move.w	#DmaCon,(a1)+		;Arret DMA
	move.w	#$0100,(a1)+
* Debut de la palette
	move.l	a1,-(sp)
	moveq	#PalMax-1,d3
	move.w	#Color00,d2
	lea	EcPal(a0),a4
MkC5:	move.w	d2,(a1)+
	addq.w	#2,d2
	move.w	(a4)+,(a1)+
	dbra	d3,MkC5

	IFEQ	EZFlag
* Dual playfield???
	move.w	EcDual(a0),d2
	bne	CreeDual
PluDual:
	ENDC

* Ecran normal!
	add.w	EcVY(a0),d1		* Decalage ecran
	mulu	EcTLigne(a0),d1
	move.w	EcVx(a0),d2
	lsr.w	#4,d2
	lsl.w	#1,d2
	add.w	d2,d1
	move.l	a1,d3
* Poke les adresses des bitplanes
	moveq	#EcPhysic,d2
	move.w	EcNPlan(a0),d6
	subq.w	#1,d6
	move.w	#Bpl1PtH,d7
MkC0:	move.l	0(a0,d2.w),d5
	add.l	d1,d5
	move.w	d7,(a1)+
	addq.w	#2,d7
	swap	d5
	move.w	d5,(a1)+
	move.w	d7,(a1)+
	addq.w	#2,d7
	swap	d5
	move.w	d5,(a1)+
	addq.l	#4,d2
	dbra	d6,MkC0
* Marque les adresses SCREEN SWAP
	move.w	EcNumber(a0),d2		* Marque les adresse
	cmp.w	#10,d2			* Si ecran utilisateur!
	bcc.s	MrkC2
	lsl.w	#6,d2
	lea	CopL1*EcMax+T_CopMark(a5),a4
	add.w	d2,a4
MrkC1	tst.l	(a4)
	addq.l	#8,a4
	bne.s	MrkC1
	clr.l	(a4)
	move.l	d3,-8(a4)		* Adresse dans liste
	move.l	d1,-4(a4)		* Decalage
MrkC2:
* Calcule les valeurs non plantantes!
	move.w	#465+16,d3
	tst.w	EcCon0(a0)
	bpl.s	MkC1c
	move.w	#465,d3
MkC1c	move.w	EcWX(a0),d1
	addq.w	#1,d1
	move.w	EcWTx(a0),d2
	move.w	d1,d6
	add.w	d2,d6
	cmp.w	d3,d6
	bcs.s	MkC1a
	sub.w	d3,d6
	add.w	#16,d6
	sub.w	d6,d2
	bra.s	MkC1b
MkC1a:	cmp.w	#176,d6
	bhi.s	MkC1b
	sub.w	#176,d6
	sub.w	d6,d1
MkC1b:	move.w	d1,EcWXr(a0)
	move.w	d2,EcWTxr(a0)
* Calcul et poke DIW Start/Stop
	move.w	#DiwStrt,(a1)+		;DiwStrt Y = 0
	move.w	d1,(a1)
	or.w	#$0100,(a1)+
	move.w	#DiwStop,(a1)+		;DiwStop Y = 311
	add.w	d2,d1
	and.w	#$00ff,d1
	or.w	#$3700,d1
	move.w	d1,(a1)+
* Calcul des valeurs modulo ---> d4
	move.w	EcTLigne(a0),d4
	move.w	EcWTxr(a0),d5
	lsr.w	#3,d5
	btst	#7,EcCon0(a0)
	bne.s	MkC2a
	lsr.w	#1,d5
MkC2a:	lsl.w	#1,d5
	sub.w	d5,d4
	bpl.s	MkC2
	clr.w	d4
MkC2:
* Calcul DDF Start/Stop---> D1/D2
	move.w	EcWXr(a0),d1
	move.w	EcWTxr(a0),d2
	move.w	EcVX(a0),d6
	btst	#7,EcCon0(a0)
	bne.s	MkCH
* Lowres
	sub.w	#17,d1
	lsr.w	#1,d1
	and.w	#$FFF8,d1
	lsr.w	#1,d2
	subq.w	#8,d2
	add.w	d1,d2
	and.w	#15,d6			;Scrolling?
	beq.s	MkC3
	subq.w	#8,d1
	subq.w	#2,d4
	neg.w	d6
	add.w	#16,d6
	bra.s	MkC3
* Hires
MkCH:	sub.w	#9,d1
	lsr.w	#1,d1
	and.w	#$FFFC,d1
	lsr.w	#1,d2
	subq.w	#8,d2
	add.w	d1,d2
	and.w	#15,d6			;Scrolling?
	lsr.w	#1,d6
	beq.s	MkC3
	subq.w	#4,d1
	subq.w	#4,d4
	neg.w	d6
	addq.w	#8,d6
MkC3:	move.w	d6,d5
	lsl.w	#4,d5
	or.w	d6,d5
* Poke les valeurs
	move.w	#DdfStrt,(a1)+
	move.w	d1,(a1)+
	move.w	#DdfStop,(a1)+
	move.w	d2,(a1)+
* Interlace?
	move.w	EcCon0(a0),d1
	btst	#2,d1
	beq.s	MkCi1
	move.w	EcTx(a0),d2
	lsr.w	#3,d2
	add.w	d2,d4
MkCi1	move.w	#Bpl1Mod,(a1)+
	move.w	d4,(a1)+
	move.w	#Bpl2Mod,(a1)+
	move.w	d4,(a1)+
* Registres de controle
	move.w	#BplCon0,(a1)+
	or.w	T_InterInter(a5),d1
	move.w	d1,(a1)+
	move.w	#BplCon1,(a1)+
	move.w	d5,(a1)+
	move.w	#BplCon2,(a1)+
	move.w	EcCon2(a0),(a1)+
* Reactive le DMA au debut de la fenetre
FiniCop	move.l	(sp)+,d4
	addq.w	#1,d0
	move.w	(a2),d1
	bpl.s	FiCp1
	neg.w	d1
FiCp1	cmp.w	d0,d1
	beq.s	MkC9
	move.w	d0,d2
	sub.w	#EcYBase,d2
	bsr	WaitD2
	move.w	#DmaCon,(a1)+
	move.w	#$8300,(a1)+
* Fin de la palette!
	move.l	a1,d3
	moveq	#32-PalMax-1,d1
	move.w	#Color00+PalMax*2,d2
	lea	EcPal+PalMax*2(a0),a4
MkC7:	move.w	d2,(a1)+
	addq.w	#2,d2
	move.w	(a4)+,(a1)+
	dbra	d1,MkC7
* Adresse de la 2ieme palette
	move.w	EcNumber(a0),d2
	lsl.w	#7,d2
	lea	T_CopMark+64(a5),a4
	add.w	d2,a4
MkC8:	tst.l	(a4)+
	bne.s	MkC8
	clr.l	(a4)
	sub.l	#4*PalMax,d3
	move.l	d3,-(a4)
* Adresse de la 1ere palette dans la liste copper
MkC9	move.w	EcNumber(a0),d2		* Marque les adresse
	lsl.w	#7,d2
	lea	T_CopMark(a5),a4
	add.w	d2,a4
MkC10:	tst.l	(a4)+
	bne.s	MkC10
	clr.l	(a4)
	move.l	d4,-(a4)
* Fini!
	rts

	IFEQ	EZFlag
******* Creation liste copper pour ecrans DUAL PLAYFIED!
CreeDual:
* Adresse du deuxieme ecran
	move.l	a2,-(sp)
	lsl.w	#2,d2
	lea	T_EcAdr(a5),a2
	move.l	-4(a2,d2.w),d2		* On a efface le deuxieme!
	bne.s	CrDu1
	move.l	(sp)+,a2
	clr.w	EcDual(a0)		* Transforme en ecran simple!
	move.w	EcCon0(a0),d2
	and.w	#%1000101111111111,d2
	move.w	EcNPlan(a0),d7
	lsl.w	#8,d7
	lsl.w	#4,d7
	or.w	d7,d2
	move.w	d2,EcCon0(a0)
	bra	PluDual
CrDu1:	move.l	d2,a2
* Adresses bitplanes PAIRS!
	move.w	d1,-(sp)
	add.w	EcVY(a0),d1		* Decalage ecran
	mulu	EcTLigne(a0),d1
	move.w	EcVx(a0),d2
	lsr.w	#4,d2
	lsl.w	#1,d2
	add.w	d2,d1
	move.l	a1,d3
	moveq	#EcPhysic,d2
	move.w	EcNPlan(a0),d6
	subq.w	#1,d6
	move.w	#Bpl1PtH,d7
MkDC1:	move.l	0(a0,d2.w),d5
	add.l	d1,d5
	move.w	d7,(a1)+
	addq.w	#2,d7
	swap	d5
	move.w	d5,(a1)+
	move.w	d7,(a1)+
	addq.w	#2,d7
	swap	d5
	move.w	d5,(a1)+
	addq.l	#4,d2
	addq.w	#4,d7
	dbra	d6,MkDC1
	move.w	EcNumber(a0),d2		* Marque les adresses
	cmp.w	#8,d2
	bcc.s	MrkDC2
	lsl.w	#6,d2
	lea	CopL1*EcMax+T_CopMark(a5),a4
	add.w	d2,a4
MrkDC1	tst.l	(a4)
	addq.l	#8,a4
	bne.s	MrkDC1
	clr.l	(a4)
	move.l	d3,-8(a4)
	move.l	d1,-4(a4)
* Adresses bitplanes IMPAIRS!
MrkDC2	move.w	(sp)+,d1
	add.w	EcVY(a2),d1		* Decalage ecran
	mulu	EcTLigne(a2),d1
	move.w	EcVx(a2),d2
	lsr.w	#4,d2
	lsl.w	#1,d2
	add.w	d2,d1
	move.l	a1,d3
	moveq	#EcPhysic,d2
	move.w	EcNPlan(a2),d6
	subq.w	#1,d6
	move.w	#Bpl1PtH+4,d7
MkdC12:	move.l	0(a2,d2.w),d5
	add.l	d1,d5
	move.w	d7,(a1)+
	addq.w	#2,d7
	swap	d5
	move.w	d5,(a1)+
	move.w	d7,(a1)+
	addq.w	#2,d7
	swap	d5
	move.w	d5,(a1)+
	addq.l	#4,d2
	addq.w	#4,d7
	dbra	d6,MkdC12
	move.w	EcNumber(a2),d2		* Marque les adresses
	cmp.w	#8,d2
	bcc.s	MrkDC4
	lsl.w	#6,d2
	lea	CopL1*EcMax+T_CopMark(a5),a4
	add.w	d2,a4
MrkDC3	tst.l	(a4)
	addq.l	#8,a4
	bne.s	MrkDC3
	clr.l	(a4)
	move.l	d3,-8(a4)
	move.l	d1,-4(a4)
MrkDC4
* Calcule les valeurs non plantantes!
	move.w	#465+16,d3
	tst.w	EcCon0(a0)
	bpl.s	MkdC1c
	sub.w	#16,d3
MkdC1c	move.w	EcWX(a0),d1
	addq.w	#1,d1
	move.w	EcWTx(a0),d2
	move.w	d1,d6
	add.w	d2,d6
	cmp.w	d3,d6
	bcs.s	MkdC1a
	sub.w	d3,d6
	add.w	#16,d6
	sub.w	d6,d2
	bra.s	MkdC1b
MkdC1a:	cmp.w	#176,d6
	bhi.s	MkdC1b
	sub.w	#176,d6
	sub.w	d6,d1
MkdC1b:	move.w	d1,EcWXr(a0)
	move.w	d2,EcWTxr(a0)
	move.w	#DiwStrt,(a1)+		;DiwStrt Y = 0
	move.w	d1,(a1)
	or.w	#$0100,(a1)+
	move.w	#DiwStop,(a1)+		;DiwStop Y = 311
	add.w	d2,d1
	and.w	#$00ff,d1
	or.w	#$3700,d1
	move.w	d1,(a1)+
* Calcul des valeurs modulo ---> D4/D5
	move.w	EcTLigne(a0),d4
	move.w	EcTLigne(a2),d5
	move.w	EcWTxr(a0),d6
	move.w	EcWTxr(a2),d7
	lsr.w	#3,d6
	lsr.w	#3,d7
	btst	#7,EcCon0(a0)
	bne.s	MkdC2
	lsr.w	#1,d6
	lsr.w	#1,d7
MkdC2:	lsl.w	#1,d6
	lsl.w	#1,d7
	sub.w	d6,d4
	bpl.s	MkdC2a
	clr.w	d4
MkdC2a:	sub.w	d7,d5
	bpl.s	MkdC2b
	clr.w	d5
MkdC2b:
* Calcul DDF Start/Stop---> D1/D2
	move.w	EcVX(a0),d6
	move.w	EcVX(a2),d7
	move.w	d6,d1
	and.w	#15,d1
	bne.s	Mkd2d
	and.w	#$FFF0,d7
Mkd2d:	move.w	d7,d1
	and.w	#15,d1
	bne.s	Mkd2e
	and.w	#$FFF0,d6
Mkd2e:	move.w	EcWXr(a0),d1
	move.w	EcWTxr(a0),d2
	btst	#7,EcCon0(a0)
	bne.s	MkdCH
* Lowres
	sub.w	#17,d1
	lsr.w	#1,d1
	and.w	#$FFF8,d1
	lsr.w	#1,d2
	subq.w	#8,d2
	add.w	d1,d2
	and.w	#15,d6
	and.w	#15,d7
	beq.s	MkdC3
	subq.w	#8,d1
	subq.w	#2,d4
	subq.w	#2,d5
	neg.w	d6
	add.w	#16,d6
	neg.w	d7
	add.w	#16,d7
	bra.s	MkdC3
* Hires
MkdCH:	sub.w	#9,d1
	lsr.w	#1,d1
	and.w	#$FFFC,d1
	lsr.w	#1,d2
	subq.w	#8,d2
	add.w	d1,d2
	and.w	#15,d6
	and.w	#15,d7
	lsr.w	#1,d6
	lsr.w	#1,d7
	beq.s	MkdC3
	subq.w	#4,d1
	subq.w	#4,d4
	subq.w	#4,d5
	neg.w	d6
	addq.w	#8,d6
	neg.w	d7
	addq.w	#8,d7
MkdC3:	lsl.w	#4,d7
	or.w	d7,d6
* Poke les valeurs
	move.w	#DdfStrt,(a1)+
	move.w	d1,(a1)+
	move.w	#DdfStop,(a1)+
	move.w	d2,(a1)+
	move.w	#Bpl1Mod,(a1)+
	move.w	d4,(a1)+
	move.w	#Bpl2Mod,(a1)+
	move.w	d5,(a1)+
* Registres de controle
	move.w	#BplCon0,(a1)+
	move.w	EcCon0(a0),d1
	or.w	T_InterInter(a5),d1
	move.w	d1,(a1)+
	move.w	#BplCon1,(a1)+
	move.w	d6,(a1)+
	move.w	#BplCon2,(a1)+
	move.w	EcCon2(a0),(a1)+
* Fini! Retourne au programme normal
	move.l	(sp)+,a2
	bra	FiniCop
	ENDC

******* Cree la ligne COPPER de fin de fenetre
EcCopBa	move.w	d0,d2
	sub.w	#EcYBase,d2
	bsr	WaitD2
	move.w	#DmaCon,(a1)+
	move.w	#$0100,(a1)+
	move.w	#Color00,(a1)+
	move.w	T_EcFond(a5),(a1)+
	rts

******* Attente copper jusqu'a la ligne D2
WaitD2:	cmp.w	#256,d2
	bcs.s	WCop
	tst.w	T_Cop255(a5)
	bne.s	WCop
	move.w	#$FFDF,(a1)+
	move.w	#$FFFE,(a1)+
	addq.w	#1,T_Cop255(a5)
WCop:	lsl.w	#8,d2
	or.w	#$03,d2
	move.w	d2,(a1)+
	move.w	#$FFFE,(a1)+
	rts

***********************************************************
*	INITIALISATION GENERALE LISTE COPPERS
*	D0= longueur des listes (physic et logic)
CpInit:
* Reserve la memoire pour les listes
	move.l	d0,T_CopLong(a5)
	bsr	ChipMm
	beq	GFatal
	move.l	d0,T_CopLogic(a5)
	move.l	d0,a0
	move.l	T_CopLong(a5),d0
	bsr	ChipMm
	beq	GFatal
	move.l	d0,T_CopPhysic(a5)
	move.l	d0,a1
* Copper en ROUTE!
	move.w	#-1,T_CopON(a5)
* Marque les offsets des listes sprites au debut...
HsCop	move.l	#$1003FFFE,(a0)+		Legere attente!
	move.l	-4(a0),(a1)+
	move.w	#$120,d0
CpI1:	move.w	d0,(a0)+
	move.w	d0,(a1)+
	addq.w	#2,d0
	addq.l	#2,a0
	addq.l	#2,a1
	cmp.w	#$13e,d0
	bls.s	CpI1
	moveq	#0,d0
	rts

***********************************************************
*	LIBERATION DES LISTES COPPER
CpEnd:	move.l	T_CopLogic(a5),d0
	beq.s	CpE1
	move.l	d0,a1
	move.l	T_CopLong(a5),d0
	bsr	FreeMm
CpE1:	move.l	T_CopPhysic(a5),d0
	beq.s	CpE2
	move.l	d0,a1
	move.l	T_CopLong(a5),d0
	bsr	FreeMm
CpE2:	rts


***********************************************************
*	GESTION DIRECTE COPPER
***********************************************************
	IFEQ	EZFlag
******* COPPER ON/OFF
TCopOn	tst.w	d1
	bne.s	ICpo1
* Copper OFF -> Hide!
	tst.w	T_CopON(a5)
	beq.s	ICpoX
	clr.w	T_CopON(a5)
	bsr	EcForceCop			* RAZ des pointeurs
	clr.l	T_HsChange(a5)			* Plus de HS!
	move.w	#-1,T_MouShow(a5)		* Plus de souris
	move.l	T_CopLogic(a5),T_CopPos(a5)	* Init!
	bsr	WVbl
	bra	TCopSw
* Copper ON -> Recalcule!
ICpo1	tst.w	T_CopON(a5)
	bne.s	ICpoX
	bsr	WVbl
	move.l	T_CopLogic(a5),a0		* Remet les listes sprites
	move.l	a0,a1
	bsr	HsCop
	bsr	TCpSw
	bsr	WVbl
	move.l	T_CopLogic(a5),a0
	move.l	a0,a1
	bsr	HsCop
	bsr	TCpSw
	bsr	WVbl
	move.w	#-1,T_CopON(a5)		* Remet!
	bsr	HsAff
	clr.w	T_MouShow(a5)
	bsr	EcForceCop		* Recalcule les listes
	bsr	WVbl
ICpoX	moveq	#0,d0
	rts

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

; Icestart: branch system functions only.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IceStart
	move.l	a6,-(sp)

	clr.w	T_WVersion(a5)		Par defaut
	cmp.l	#"V2.0",d0		Le magic
	bne.s	.Nomagic
	move.w	d1,T_WVersion(a5)	La version d'AMOS
.Nomagic

	lea	W_Base(pc),a0
	move.l	a5,(a0)
; The contexts of the C++ code are AMOSPro's (src/core/call.s)
	move.l	T_CBridge(a5),_amos_bridge
	lea	SyIn(pc),a0
	move.l	a0,T_SyVect(a5)
	bsr	WMemInit

; Recherche et stoppe les programmes AMOS lanc�s... (si AMOSPro V2.0)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	cmp.w	#$0200,T_WVersion(a5)
	bcs.s	.No20
	move.l	$4.w,a6
	jsr	Forbid(a6)
	lea	TaskName(pc),a1
	jsr	FindTask(a6)
	tst.l	d0
	beq.s	.skip
	move.l	d0,a0
	move.l	10(a0),a1
	move.b	#"S",(a1)		* STOP!!!
	move.l	a1,T_Stopped(a5)
.skip	jsr	Permit(a6)
; Change son propre nom...
	sub.l	a1,a1
	jsr	FindTask(a6)
	move.l	d0,a0
	move.l	d0,T_MyTask(a5)
	move.l	10(a0),T_OldName(a5)
	lea	TaskName(pc),a1
	move.l	a1,10(a0)
	move.l	a5,$58(a0)		Adresse des datas...
; Fini!
.No20	move.l	(sp)+,a6
	move.l	#"W2.0",d0		Retourne des magic
	move.w	#$0200,d1
	rts

; Normal cold start: start default system
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
StartAll
	movem.l	a0-a6/d1-d7,-(sp)
	move.l	sp,T_GPile(a5)

	move.l	a2,-(sp)			Palette par defaut
	move.l	a0,-(sp)

; Attend que l'autre AMOS soit arrete! (si v2.0)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	cmp.w	#$0200,T_WVersion(a5)
	bcs.s	.No20_a
	movem.l	a0-a3/d0-d3,-(sp)
	move.l	T_Stopped(a5),d0
	beq.s	.Wait2
	move.l	d0,a2
	move.w	#50*5,d3
.Wait1	move.l	T_GfxBase(a5),a6
	jsr	-270(a6)
	cmp.b	#"S",(a2)
	bne.s	.Wait2
	dbra	d3,.Wait1
.GoEnd	bra	GFatal
.Wait2	movem.l	(sp)+,a0-a3/d0-d3
.No20_a

; Sauve les flags du DMA
; ~~~~~~~~~~~~~~~~~~~~~~
	move.b	d0,T_AMOSHere(a5)
	move.w	Circuits+DmaConR,T_OldDma(a5)
	bset	#7,T_OldDma(a5)

; Amiga-A
; ~~~~~~~
	tst.l	d1
	bne.s	.Skip
	move.l	#$00406141,d1
.Skip	move.b	d1,T_AmigA_Ascii1(a5)
	lsr.l	#8,d1
	move.b	d1,T_AmigA_Ascii2(a5)
	lsr.w	#8,d1
	move.b	d1,T_AmigA_Shifts(a5)

; Mouse.Abk (si v2.0)
; ~~~~~~~~~~~~~~~~~~~
	cmp.w	#2,T_WVersion(a5)
	bcs.s	.No20_b
	move.l	a1,d0
	bne.s	.MSkip
	move.l	WDebut-4(pc),d0			Prend le HUNK suivant
	lsl.l	#2,d0
	move.l	d0,a1
	addq.l	#4,a1
.MSkip	cmp.l	#"AmSp",(a1)+
	bne	GFatal
	move.w	(a1)+,d1
	cmp.w	#4,d1
	bcs	GFatal
	move.l	a1,T_MouBank(a5)
; Pointe la palette pour l'ouverture des ecrans
	subq.w	#1,d1
.MLoop	move.w	(a1)+,d0
	mulu	(a1)+,d0
	mulu	(a1)+,d0
	lsl.l	#1,d0
	lea	4(a1,d0.l),a1
	dbra	d1,.MLoop
	move.w	#-1,(a1)		Stoppe la mouse.abk
	lea	16*2(a1),a1		Pointe couleurs 16-32
	lea	16*2(a2),a2		Couleurs 16-32 de default palette
	moveq	#15,d0
.PCopy	move.w	(a1)+,(a2)+
	dbra	d0,.PCopy
.No20_b

; Ouverture de la fonte systeme 8x8
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	lea	TopazName(pc),a1	Topaz force si <2.0
	cmp.w	#$0200,T_WVersion(a5)
	bcs.s	.Sfont
	move.l	a3,d0			Sinon, fonte dans a3
	beq.s	.Sfont
	tst.b	(a3)
	beq.s	.Sfont
	move.l	a3,a1
.Sfont	lea	-8(sp),sp
	move.l	sp,a0
	move.l	a1,(a0)
	move.w	#8,4(a0)
	move.w	#$0041,6(a0)
	move.l	T_GfxBase(a5),a6
	jsr	_LVOOpenFont(a6)
	move.l	d0,T_DefaultFont(a5)
	bne.s	.fOk
	lea	TopazName(pc),a1
	move.l	sp,a0			On ressaie avec topaz
	move.l	a1,(a0)
	move.w	#8,4(a0)
	move.w	#$0041,6(a0)
	jsr	_LVOOpenFont(a6)
	move.l	d0,T_DefaultFont(a5)
	beq	GFatal			???
.fOk	addq.l	#8,sp

; Graphic library
; ~~~~~~~~~~~~~~~
	lea	GfxBase(pc),a1
	move.l	T_GfxBase(a5),a0
	move.l	a0,(a1)
	clr.b	T_WFlags(a5)			Flag AA
	btst	#2,236(a0)
	beq.s	.PaAA
	bset	#WFlag_AA,T_WFlags(a5)
	bset	#WFlag_LoadView,T_WFlags(a5)
.PaAA	move.l	$4.w,a0				Kickstart >= V39?
	cmp.w	#39,$14(a0)			Si oui, on fait un LoadView(0)
	bcs.s	.Pa39
	bset	#WFlag_LoadView,T_WFlags(a5)
.Pa39
	IFNE	Debug=2				Si debug
	bclr	#WFlag_LoadView,T_WFlags(a5)	AMIGA-A normal...
	ENDC


; Ma propre tache
; ~~~~~~~~~~~~~~~
	sub.l	a1,a1
	move.l	$4.w,a6
	jsr	FindTask(a6)
	move.l	d0,T_MyTask(a5)
	move.l	d0,a0

; Ouverture du layer.library
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d0
	lea	LayName(pc),a1
	move.l	$4.w,a6
	jsr	OpenLib(a6)
	move.l	d0,T_LayBase(a5)
	beq	GFatal

; Branche l'input.device
; ~~~~~~~~~~~~~~~~~~~~~~
	bsr	ClInit
; Open Console Device
; ~~~~~~~~~~~~~~~~~~~
	lea	ConIo(pc),a1
	bsr	OpConsole
; Branche le input_handler
; ~~~~~~~~~~~~~~~~~~~~~~~~
	lea	T_IoDevice(a5),a1
	bsr	OpInput
	lea	T_Interrupt(a5),a0
	lea	IoHandler(pc),a1
	move.l	a1,IS_CODE(a0)
	clr.l	IS_DATA(a0)
	move.b	#100,ln_pri(a0)
	lea	T_IoDevice(a5),a1
	move.l	a0,io_Data(a1)
	move.w	#IND_ADDHANDLER,io_command(a1)
	jsr	_LVODoIo(a6)
	move.w	#-1,T_DevHere(a5)

; Parametres par defaut des ecrans
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	(sp),a0
	move.w	#129,T_DefWX(a5)
	move.w	#129-16,T_DefWX2(a5)
	move.w	10(a0),T_DefWY(a5)
	move.w	10(a0),T_DefWY2(a5)
	subq.w	#8,T_DefWY2(a5)

	lea 	Circuits,a6
	move.l	(sp),a0
	move.l	16(a0),d0
	bsr	HsInit			Hard sprites
	move.l	(sp),a0
	move.w	8(a0),d0
	bsr	BbInit			Bobs
	bsr	RbInit			Retourneur de bobs
	move.l	(sp),a0
	move.l	12(a0),d0
	bsr	CpInit			Copper
	bsr	EcInit			Ecrans
	bsr	SyInit			Systeme
	bsr	VBLInit			Interruptions VBL
	bsr	WiInit			Windows

;	Si AA, change le vecteur LOADVIEW
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	btst	#WFlag_LoadView,T_WFlags(a5)	Si LoadView en route
	beq.s	.NoLoadView
	lea	AMOS_LoadView(pc),a0
	lea	T_AMOSHere(a5),a1		Adresse du test
	move.l	a1,2(a0)			>>> dans le source...
	move.l	a0,d0				Nouvelle fonction
	move.w	#-222,a0			LOADVIEW
	move.l	T_GfxBase(a5),a1		Librairie
	move.l	$4.w,a6
	jsr	-420(a6)			Set function
	lea	Old_LoadView(pc),a0		Ancien vecteur
	move.l	d0,(a0)
	bsr	Sys_ClearCache			Nettoie les caches!
.NoLoadView

; 	Branche le requester
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	cmp.w	#$0200,T_WVersion(a5)
	bcs.s	.No20_c
	move.l	4(sp),a0			Palette par defaut
	bsr	WRequest_Start
; 	Fabrique la fonte par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	Wi_MakeFonte
	bne	GFatal
; 	Envoie le signal a l'AMOS Switcher
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	T_MyTask(a5),a0
	move.l	a5,$58(a0)
	moveq	#Switcher_Signal,d3
	bsr	Send_Switcher
.No20_c

; 	Tout fini: AMOS to front ?
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.b	T_AMOSHere(a5)
	beq.s	.Pafr
	clr.b	T_AMOSHere(a5)
	moveq	#1,d1
	bsr	TAMOSWb
.Pafr
; Pas d'erreur
; ~~~~~~~~~~~~
	moveq	#0,d0
	bra.s	GFini
; 	Erreur: EFFACE TOUT, et revient!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GFatal	bsr.s	EndAll
	moveq	#-1,d0
GFini	move.l	T_GPile(a5),a7
	movem.l	(sp)+,a0-a6/d1-d7
	rts


;							Arret general
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EndAll	lea	Circuits,a6

;	Remet l'ecran du workbench
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	moveq	#0,d1
	bsr	TAMOSWb
	moveq	#2,d0
	bsr	WVbl_d0

; Empeche le switcher de fonctionner (si 2.0)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	cmp.w	#$0200,T_WVersion(a5)
	bcs.s	.No20_a
	moveq	#Switcher_Signal+1,d3
	bsr	Send_Switcher
; 	Plus de requester
; ~~~~~~~~~~~~~~~~~~~~~~~
	bsr	WRequest_Stop
;	Efface la fonte
; ~~~~~~~~~~~~~~~~~~~~~
	bsr	Wi_DelFonte
.No20_a

;	Si AA, remet le vecteur LOADVIEW
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	btst	#WFlag_LoadView,T_WFlags(a5)	Si LoadView en route
	beq.s	.NoLoadView
	move.l	Old_LoadView(pc),d0		Ancienne fonction
	beq.s	.NoLoadView
	move.w	#-222,a0			LOADVIEW
	move.l	T_GfxBase(a5),a1		Librairie
	move.l	$4.w,a6
	jsr	-420(a6)			Set function
.NoLoadView

;	Debranche l'input.device
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	tst.w	T_DevHere(a5)
	beq.s	.skip0
	lea	T_Interrupt(a5),a0
	lea	T_IoDevice(a5),a1
	move.l	a0,io_Data(a1)
	move.w	#IND_REMHANDLER,Io_Command(a1)
	move.l	$4.w,a6
	jsr	_LVODoIo(a6)
	lea	T_IoDevice(a5),a1
	bsr	ClInput
.Skip0

; 	Arret des toutes les fonctions AMOS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	TFFonts
;	bsr	WiEnd			Rts!
	bsr	SyEnd
	bsr	EcEnd
	bsr	RbEnd
	bsr	BbEnd
	bsr	HsEnd
	bsr	VBLEnd
	bsr	CpEnd

;	Ferme les librairies
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	$4.w,a6
	move.l	T_FntBase(a5),d0		diskfont.library
	beq.s	.Lib0
	move.l	d0,a1
	jsr 	CloseLib(a6)
.Lib0	move.l	T_LayBase(a5),d0		layer.library
	beq.s	.Lib1
	move.l	d0,a1
	jsr 	CloseLib(a6)
.Lib1	move.l	T_GfxBase(a5),d0		graphics.library
	beq.s	.Lib2
	move.l	d0,a6				Ferme la fonte par defaut
	move.l	T_DefaultFont(a5),d0
	beq.s	.Lib2
	move.l	d0,a1
	jsr	_LVOCloseFont(a6)
.Lib2
	moveq	#0,d0
	rts

; Fin d'access au fonctions systeme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IceEnd
; Relance l'ancien AMOS (si 2.0)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	cmp.w	#$0200,T_WVersion(a5)
	bcs.s	.No20_a
	move.l	T_Stopped(a5),d0
	beq.s	.Skup
	move.l	d0,a0
	move.b	#" ",(a0)
; Remet son ancien nom
.Skup	move.l	T_MyTask(a5),d0
	beq.s	.Skiip
	move.l	d0,a0
	move.l	T_OldName(a5),10(a0)
.Skiip
.No20_a
; Enleve la gestion memoire si definie
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	bsr	WMemEnd			Plus de memory checking!
	rts

;	Envoie un signal � l'AMOS_Switcher (D3= signal)
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Send_Switcher
	lea	Switcher(pc),a1
	move.l	$4.w,a6
	jsr	_LVOFindTask(a6)
	tst.l	d0
	beq.s	.PaSwi
	move.l	d0,a1
	moveq	#0,d0
	bset	d3,d0
	jsr	_LVOSignal(a6)
.PaSwi	rts

;	Fabrique la fonte par defaut
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Wi_MakeFonte
	movem.l	a2/d2-d7,-(sp)
	moveq	#0,d1			Ecran 16x8, 2 couleur
	moveq	#16,d2
	moveq	#8,d3
	moveq	#1,d4
	moveq	#0,d5
	moveq	#2,d6
	lea	Wi_MakeFonte(pc),a1
	bsr	EcCree
	bne	.Error
; Boucle de creation
	move.l	#8*256,d0
	SyCall	MemFastClear
	beq	.Error
	move.l	a0,T_JeuDefo(a5)
	move.l	a0,a2
	lea	32*8(a2),a0
	moveq	#32,d2
	move.w	#128,d3
	bsr	.CreeFont
	lea	160*8(a2),a0
	move.w	#160,d2
	move.w	#256,d3
	bsr	.CreeFont
; Poke les caracteres specifiques
	lea	Def_Font(pc),a0
	move.l	a2,a1
	moveq	#(8*32)/4-1,d0
.Copy1	move.l	(a0)+,(a1)+
	dbra	d0,.Copy1
	lea	128*8(a2),a1
	moveq	#(8*32)/4-1,d0
.Copy2	move.l	(a0)+,(a1)+
	dbra	d0,.Copy2
; A y est!
	moveq	#0,d0
	bra.s	.Out
; Erreur
.Error	moveq	#1,d0
; Sortie!
.Out	move.l	d0,-(sp)
	moveq	#0,d0
	bsr	EcDel
	move.l	(sp)+,d0
	movem.l	(sp)+,a2/d2-d7
	rts
; Saisit les caracteres d2-d3
.CreeFont
	movem.l	d2/d3/a2/a3/a6,-(sp)
	move.l	a0,a2
	move.l	T_EcCourant(a5),a3
.Car	move.l	T_RastPort(a5),a1		Le rastport
	move.w	#0,36(a1)			Curseur en 0,0
	move.w	#6,38(a1)
	moveq	#1,d0				Un caractere
	lea	.COut(pc),a0
	move.b	d2,(a0)
	move.l	T_GfxBase(a5),a6		La fonction
	jsr	_LVOText(a6)
	move.l	EcLogic(a3),a0			Boucle de recopie
	move.w	EcTligne(a3),d0
	ext.l	d0
	moveq	#7,d1
.Loop	move.b	(a0),(a2)+
	add.l	d0,a0
	dbra	d1,.Loop
	addq.w	#1,d2
	cmp.w	d3,d2
	bcs.s	.Car
	movem.l	(sp)+,d2/d3/a2/a3/a6
	rts
.COut	dc.w	0

;	Effacement du jeu de caracteres
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Wi_DelFonte
	move.l	T_JeuDefo(a5),d0
	beq.s	.Sip
	move.l	d0,a1
	move.l	#8*256,d0
	SyCall	MemFree
	clr.l	T_JeuDefo(a5)
.Sip	rts

***********************************************************
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
AMOS_LoadView
	tst.b	0.l			T_AMOSHere, modifie lors du patch...
	bne.s	.Wb
	move.l	Old_LoadView(pc),-(sp)
.Wb	rts
Old_LoadView	dc.l	0		Ici et pas ailleurs...

; AMOS / WORKBENCH
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D1=0	> 	Workbench
;	D1>0	>	AMOS
;	D1<0	>	Rien (trouver valeur)
;	Retour D1= AMOS ici(-1), WB ici (0)
TAMOSWb
	tst.w	d1
	beq	.ToWB
	bmi	.Return

; 	Back to AMOS
; ~~~~~~~~~~~~~~~~~~
.ToAMOS	tst.b	T_AMOSHere(a5)
	bne	.Return
	move.b	#-1,T_AMOSHere+1(a5)		Code interdisant les requester

; Load View(0) + WaitTOF si AA
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	btst	#WFlag_LoadView,T_WFlags(a5)
	beq.s	.PaAA0
	movem.l	d0-d3/a0-a2/a6,-(sp)

	move.l	T_IntBase(a5),a6		Ouvre un ecran LOWRES

	btst	#WFlag_WBClosed,T_WFlags(a5)	Si WB ferme, le referme!
	beq.s	.NoWB
	jsr	-78(a6)
.NoWB
	lea	NewScreen(pc),a0
	jsr	OpenScreen(a6)
	move.l	d0,T_IntScreen(a5)

	move.l	T_GfxBase(a5),a6		WaitTOF
	jsr	-$10e(a6)
	jsr	-$10e(a6)

	sub.l	a1,a1
	move.l	T_GfxBase(a5),a6
	jsr	-$de(a6)			LoadView(a0)
	jsr	-$10e(a6)			WaitTOF
	jsr	-$10e(a6)			WaitTOF
	move.w	$dff07c,d0
	cmp.b	#$f8,d0				AA Chipset?
	bne.s	.NoBug
	move.w	#0,$dff1fc			Sprite resolution
	move.w	#%0000110000000000,$dff106	Sprite width / DualPF palette
.NoBug

	movem.l	(sp)+,d0-d3/a0-a2/a6
.PaAA0
	lea	Circuits,a0			Remet les circuits
	move.w	#$8080,JoyTest(a0)
	move.l 	T_CopPhysic(a5),$80(a0)
	clr.w 	$88(a0)
	move.b	#-1,T_AMOSHere(a5)		AMOS en front!
	clr.b	T_AMOSHere+1(a5)		Flip termine!
	bra.s	.Return

; 	Goto workbench
; ~~~~~~~~~~~~~~~~~~~~
.ToWB	tst.b	T_AMOSHere(a5)
	beq.s	.Return
	clr.b	T_AMOSHere(a5)			AMOS en fond!
	move.b	#-1,T_AMOSHere+1(a5)		Code interdisant les requesters

	move.w	T_OldDma(a5),$Dff096		Remet les chips
	move.l	T_GfxBase(a5),a0
	move.l 	38(a0),$dff080
	clr.w	$dff088

; Efface l'ecran si AA
; ~~~~~~~~~~~~~~~~~~~~
	btst	#WFlag_LoadView,T_WFlags(a5)
	beq.s	.PaAA1
	movem.l	d0-d3/a0-a2/a6,-(sp)
	move.l	T_IntBase(a5),a6		Close Screen
	btst	#WFlag_WBClosed,T_WFlags(a5)	Si WB ferme, le rouvre!
	beq.s	.NoBW
	jsr	-210(a6)			Reopen workbench
.NoBW	move.l	T_IntScreen(a5),a0		Close screen
	jsr	CloseScreen(a6)
	move.l	T_GfxBase(a5),a6
	jsr	-$10e(a6)			WaitTOF
	jsr	-$10e(a6)			WaitTOF
	movem.l	(sp)+,d0-d3/a0-a2/a6
.PaAA1
	clr.b	T_AMOSHere+1(a5)		Flip termine!

; Retourne l'etat actuel
; ~~~~~~~~~~~~~~~~~~~~~~
.Return	move.b	T_AMOSHere(a5),d1
	ext.w	d1
	ext.l	d1
	moveq	#0,d0
	rts

;	Clear CPU Caches
; ~~~~~~~~~~~~~~~~~~~~~~
Sys_ClearCache
	movem.l	a0-a1/a6/d0-d1,-(sp)
	move.l	$4.w,a6
	cmp.w	#37,$14(a6)			A partir de V37
	bcs.s	.Exit
	jsr	-$27c(a6)			CacheClearU
.Exit	movem.l	(sp)+,a0-a1/a6/d0-d1
	rts



; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	ZONE DE DONNE CENTRALE (BEARK)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


;		Zone de donnee externe
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ConIo		ds.b 	32+8
LConBuffer	equ	64
ConBuffer	ds.b 	LConBuffer
ConEssai	ds.b	32
W_Base		ds.l	1
GfxBase		ds.l	1
WRastPort	ds.l	1
		even
; Table de retournement bobs

NTx:		equ 480
NTy:		equ 12
NNp:		equ 1
NewScreen:	dc.w 0,0,NTx,NTy,NNp
		dc.b 1,0
		dc.w %0010000000000000,%00000110
		dc.l 0,0,0,0
		ds.b	16
		even

;		Caracteres speciaux des fontes AMOS
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

***********************************************************
*	DEMARRAGE A FROID DU SYSTEME
***********************************************************
SyInit:
	bsr	AMALInit
	moveq	#0,d0
	rts
***********************************************************
*	ARRET FINAL DU SYSTEME
***********************************************************
SyEnd:	bsr	AMALEnd
	moveq	#0,d0
	rts

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
; 	Initialisation / Fin du requester
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

;	Demarrage du requester (a0)= default palette
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WRequest_Start
; Copy default palette
	lea	8*2(a0),a0
	lea	Req_Pal+8*2(pc),a1
	moveq	#24-1,d0
CPal	move.w	(a0)+,(a1)+
	dbra	d0,CPal
; Branch the requester
	lea	AutoReq(pc),a2
	lea	EasyReq(pc),a3
	bsr.s	SetJump
	move.l	a2,T_PrevAuto(a5)
	move.l	a3,T_PrevEasy(a5)
	move.w	#-1,T_ReqFlag(a5)	        * Default is AMOS request
	rts

;	Arret du requester
; ~~~~~~~~~~~~~~~~~~~~~~~~
WRequest_Stop
	tst.l	T_PrevAuto(a5)
	beq.s	.Skip
	move.l	T_PrevAuto(a5),a2
	move.l	T_PrevEasy(a5),a3
	bsr.s	SetJump
.Skip	rts

; Branche A2/a3 sur easy/auto request
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SetJump	movem.l	a0/a1/a6,-(sp)
	move.l	$4.w,a6
; AutoRequest
	move.l	a2,d0
	lea	-$15c,a0
	move.l	T_IntBase(a5),a1
	jsr	SetFunction(a6)
	move.l	d0,a2
; EasyRequest, seulement si wb>=2.0
	move.l	$4.w,a0
	cmp.w	#36,$14(a0)
	bcs.s	.skip
	move.l	a3,d0
	lea	-$1d4-(20*6),a0
	move.l	T_IntBase(a5),a1
	jsr	SetFunction(a6)
	move.l	d0,a3
.skip	movem.l	(sp)+,a0/a1/a6
	rts

******* Table des sauts
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

******* Branche les interruptions
VblInit:
* Init couleurs
	bsr	FlInit
	bsr	ShInit
* Init souris
	move.l	T_MouBank(a5),a0
	move.l	a0,T_MouDes(a5)
	move.w	2(a0),T_MouTY(a5)
	clr.w	T_MouXOld(a5)
	clr.w	T_MouYOld(a5)
	move.w	#-1,T_MouShow(a5)
* Branche les interruptions!
	bsr	Add_VBL
	rts

******* Branche PROPREMENT les interruptions VBL...
Add_VBL
	move.l	a6,-(sp)
	lea	T_VBL_Is(a5),a1
	move.b	#NT_INTERRUPT,Ln_Type(a1)
	move.b	#100,Ln_Pri(a1)
	clr.l	Ln_Name(a1)
	move.l	a5,IS_DATA(a1)
	lea	VBLIn(pc),a0
	move.l	a0,IS_CODE(a1)
	move.l	$4.w,a6
	move.l	#INTB_VERTB,d0
	jsr	_LVOAddIntServer(a6)
	move.l	(sp)+,a6
	rts
******* Debranche PROPREMENT les interruptions VBL...
Rem_VBL
VBLEnd
	move.l	a6,-(sp)
	lea	T_VBL_Is(a5),a1
	tst.l	IS_CODE(a1)
	beq.s	.skip
	move.l	$4.w,a6
	move.l	#INTB_VERTB,d0
	jsr	_LVORemIntServer(a6)
.skip	move.l	(sp)+,a6
	rts

******* Entree des interruptions
VblIn:	movem.l	d2-d7/a2-a4,-(sp)
	lea	Circuits,a6
	move.l	a1,a5
* Fait les switch entrelaces
	tst.w	T_InterBit(a5)
	beq.s	SIntX2
	lea	T_InterList(a5),a5
	move.l	(a5)+,d0
	beq.s	SIntX
	move.w	$4(a6),d6
SInt0	move.l	d0,a4			* Adresse ecran
	move.l	(a5)+,a3		* Adresse marqueur
	move.w	EcNPlan(a4),d5		* D5 Nb de plans
	subq.w	#1,d5
	move.l	(a3)+,d2
	beq.s	SInt4
SInt1	move.l	(a3)+,a0
	tst.w	d6
	bmi.s	SInt2
	move.w	EcTx(a4),d0
	lsr.w	#3,d0
	add.w	d0,a0
SInt2	move.w	d5,d1
	move.l	d2,a2
	lea	EcPhysic(a4),a1
SInt3	move.l	(a1)+,d0
	add.l	a0,d0
	move.w	d0,6(a2)
	swap	d0
	move.w	d0,2(a2)
	addq.l	#8,a2
	dbra	d1,SInt3
	move.l	(a3)+,d2
	bne.s	SInt1
SInt4	move.l	(a5)+,d0
	bne.s	SInt0
SIntX	move.l	W_Base(pc),a5
SIntX2

* Fait les screen swaps
	lea	T_SwapList(a5),a0
	move.l	(a0),d0
	beq.s	SSwpX
	clr.l	(a0)+
SSwp1:	move.l	a0,a1
	move.l	d0,a4
	move.w	(a1)+,d1		* Nb de plans
	move.l	(a4)+,d0
	beq.s	SSwp4
SSwp2:	move.l	(a4)+,d3		* Decalage
	move.l	d0,a3
	move.l	a1,a2
	move.w	d1,d2
SSwp3:	move.l	(a2)+,d0
	add.l	d3,d0
	move.w	d0,6(a3)
	swap	d0
	move.w	d0,2(a3)
	addq.l	#8,a3
	dbra	d2,SSwp3
	move.l	(a4)+,d0
	bne.s	SSwp2
SSwp4:	lea	SwapL-4(a0),a0
	move.l	(a0)+,d0
	bne.s	SSwp1
SSwpX:

* Change l'adresses des sprites hard
	move.l	T_HsChange(a5),d0
	beq.s	VblPaHs
	clr.l	T_HsChange(a5)
	bsr	HsPCop
VblPaHs:

* Marque le VBL
	addq.l	#1,T_VBLCount(a5)
	addq.l	#1,T_VBLTimer(a5)
	subq.w	#1,T_EveCpt(a5)
	bset	#BitVBL,T_Actualise(a5)

* Appelle les autres routines
	lea	VblRout(a5),a4
	move.l	(a4)+,d0
	beq.s	VblPaCa
VblCall	move.l	d0,a0
	jsr	(a0)
	move.l	(a4)+,d0
	bne.s	VblCall
VblPaCa
* Affiche la souris
	bsr	MousInt
* Couleurs
	lea	T_CopMark(a5),a3

	IFEQ	EZFlag
	bsr	Shifter
	ENDC

	bsr	FlInt
	bsr	FadeI

	IFEQ	EZFlag
* Animations
	move.w	T_SyncOff(a5),d0
	bne.s	PaSync
	bsr	Animeur
PaSync:
	ENDC

	movem.l	(sp)+,d2-d7/a2-a4
	lea	$DFF000,a0
	moveq	#0,d0
	rts

******* WAIT VBL D0, multitache
WVbl_D0	movem.l	d0-d1/a0-a1/a6,-(sp)
	move.w	d0,-(sp)
.Lp	move.l	T_GfxBase(a5),a6
	jsr	_LVOWaitTOF(a6)
	subq.w	#1,(sp)
	bne.s	.Lp
	addq.l	#2,sp
	movem.l	(sp)+,d0-d1/a0-a1/a6
	rts

******* WAIT VBL
		xdef	_w_wvbl
_w_wvbl
WVbl:	move.l	T_VblCount(a5),d0
WVbl1:	cmp.l	T_VblCount(a5),d0
	beq.s	WVbl1
	moveq	#0,d0
	rts

******* Traitement de la souris
MousInt:tst.b	T_AMOSHere(a5)
	beq	MouF

	move.w	T_MouseX(a5),d0
	move.w	T_MouseY(a5),d1

; Limite la souris
MouV7	cmp.w	T_MouXMin(a5),d0
	bge.s	Mou5
	move.w	T_MouXMin(a5),d0
Mou5:	cmp.w	T_MouXMax(a5),d0
	ble.s	Mou6
	move.w	T_MouXMax(a5),d0
Mou6:	cmp.w	T_MouYMin(a5),d1
	bge.s	Mou7
	move.w	T_MouYMin(a5),d1
Mou7:	cmp.w	T_MouYMax(a5),d1
	ble.s	Mou8
	move.w	T_MouYMax(a5),d1
Mou8:	move.w	d0,T_MouseX(a5)
	move.w	d1,T_MouseY(a5)
	lsr.w	#1,d0
	lsr.w	#1,d1
	move.w	d0,T_XMouse(a5)
	move.w	d1,T_YMouse(a5)

; Poke les mots de control, si SHOW
	move.w	T_MouShow(a5),d2
	bmi.s	MouF
	sub.w	T_MouHotX(a5),d0
	sub.w	T_MouHotY(a5),d1
	move.l	T_HsPhysic(a5),a0		;Adresse du dessin
	move.l	T_HsLogic(a5),a1
	move.l	T_HsInter(a5),a2
	ror.w	#1,d0
	move.b	d1,d2
	lsl.w	#8,d2
	move.b	d0,d2
	move.w	d2,(a0)
	move.w	d2,(a1)
	move.w	d2,(a2)
	clr.w	d2
	btst	#8,d1
	beq.s	Mou10
	bset	#8+2,d2
Mou10:	add.w	T_MouTy(a5),d1
	move.b	d1,d2
	ror.w	#8,d2
	btst	#8,d1
	beq.s	Mou11
	bset	#1,d2
Mou11:	btst	#15,d0
	beq.s	Mou12
	bset	#0,d2
Mou12:	move.w	d2,2(a0)
	move.w	d2,2(a1)
	move.w	d2,2(a2)
MouF:	rts

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

; Screen to hardware coordinates of screen a0, for the collisions (which
; AMAL may test in the VBL): d1/d2
CXyS:	tst.w	EcCon0(a0)
	bpl.s	CXyS0
	asr.w	#1,d1
CXyS0:	add.w	EcWX(a0),d1
	ext.l	d1
	btst	#2,EcCon0+1(a0)
	beq.s	CXyS3
	asr.w	#1,d2
CXyS3	add.w	EcWY(a0),d2
	sub.w	#EcYBase,d2
	ext.l	d2
	moveq	#0,d0
	rts


**********************************************************
*	JOYSTICK / d1= # de port
**********************************************************
ClJoy:	tst.b	T_AMOSHere(a5)
	beq.s	JoyNo
	moveq	#6,d0			;# du bit de FEU
	add.w	d1,d0
	lea	Circuits,a0
	lsl.w	#1,d1
	move.w	10(a0,d1.w),d2
; Prend le bouton
	clr.w	d1
	btst	d0,CiaAPrA
	bne.s	Joy1
	bset	#4,d1
; Teste les directions
Joy1:	lea	JoyTab(pc),a0
	lsl.b	#6,d2
	lsr.w 	#6,d2
	and.w	#$000F,d2
	or.b	0(a0,d2.w),d1
Joy2	moveq	#0,d0
	rts
JoyNo	moveq	#0,d1
	bra.s	Joy2
JoyTab:	dc.b 	%0000,%0010,%1010,%1000,%0001,%0000,%0000,%1001
	dc.b 	%0101,%0000,%0000,%0000,%0100,%0110,%0000,%0000

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
***********************************************************
*	INITIALISATION SPRITES HARDWARE / D0= Nbre lignes
HsInit:	movem.l	d1-d7/a1-a6,-(sp)
	move.w	d0,-(sp)
* Reserve la table
	moveq	#HsNb,d0
	mulu	#HsLong,d0
	addq.l	#4,d0
	bsr	FastMm
	beq	GFatal
	addq.l	#4,d0
	move.l	d0,T_HsTable(a5)
* Va reserver les buffers
	move.w	(sp)+,d0
	bsr	HsRBuf
	bne	GFatal
* Pas d'erreur
HsOk:	movem.l	(sp)+,d1-d7/a1-a6
	moveq	#0,d0
	rts

******* Sprites: src/w/sprites.cpp
HsSBuf:	jmp	_w_sprite_buffer

******* Reserve le buffer des colonnes
		xdef	_w_hs_rbuf
_w_hs_rbuf
HsRBuf:	clr.l	T_HsBuffer(a5)
	clr.w	T_HsPMax(a5)
	clr.w	T_HsTCol(a5)
	move.w	d0,T_HsNLine(a5)
	mulu	#4*8,d0
	move.l	d0,d1
	mulu	#3,d0
	move.l	d0,T_HsTBuf(a5)
	bsr	ChipMm
	beq.s	HsRBe
	move.l	d0,T_HsBuffer(a5)
	move.l	d0,T_HsPhysic(a5)
	add.l	d1,d0
	move.l	d0,T_HsLogic(a5)
	add.l	d1,d0
	move.l	d0,T_HsInter(a5)
* Calcule les colonnes
	lsr.l	#3,d1
	move.w	d1,T_HsTCol(a5)
	lsr.w	#2,d1
	subq.w	#2,d1
	move.w	d1,T_HsPMax(a5)
* Ok!
	moveq	#0,d0
	rts
* Erreur!
HsRbe:	moveq	#1,d0
	rts

******* Efface le buffer des colonnes
		xdef	_w_hs_ebuf
_w_hs_ebuf
HsEBuf:	tst.l	T_HsBuffer(a5)
	beq.s	HsEb1
	clr.w	T_HsPMax(a5)
	clr.w	T_HsTCol(a5)
	move.l	T_HsBuffer(a5),a1
	clr.l	T_HsBuffer(a5)
	move.l	T_HsTBuf(a5),d0
	bsr	FreeMm
HsEb1:	rts

******* Poke l'adresse D0 dans les listes copper
HsPCop:	move.w	T_HsTCol(a5),d1
	ext.l	d1
	move.l	T_CopLogic(a5),a0
	move.l	T_CopPhysic(a5),a1
	addq.l	#4,a0
	addq.l	#4,a1
	moveq	#7,d2
HsPc1:	swap	d0
	move.w	d0,2(a0)
	move.w	d0,2(a1)
	swap	d0
	move.w	d0,6(a0)
	move.w	d0,6(a1)
	add.l	d1,d0
	lea	8(a0),a0
	lea	8(a1),a1
	dbra	d2,HsPc1
	rts

HsPri:	jmp	_w_sprite_priority

***********************************************************
*	ARRET SPRITES HARDWARE
HsEnd:	movem.l	d1-d7/a1-a6,-(sp)
	move.l	T_HsTable(a5),d0
	beq.s	HsE1
	move.l	d0,a1
	subq.l	#4,a1
	moveq	#HsNb,d0
	mulu	#HsLong,d0
	addq.l	#4,d0
	bsr	FreeMm
HsE1:	bsr	HsEBuf
	bra	HsOk

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


; OPEN CONSOLE.DEVICE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OpConsole
	lea	ConIo(pc),a1
	moveq	#(Lio+Lmsg)/2-1,d0
.Clean	clr.w	(a1)+
	dbra	d0,.Clean
	move.l	$4.w,a6
	lea	ConName(pc),a0
	lea	ConIo(pc),a1
	moveq	#-1,d0			Console #= -1
	moveq	#0,d1
	jsr	OpenDev(a6)
	rts

; OPEN INPUT.DEVICE
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A1-> 	pointeur sur zone libre!!!
		xdef	_w_open_input
_w_open_input
OpInput
	move.l	a1,-(sp)
* Clean
	moveq	#(Lio+Lmsg)/2-1,d0
OpInp1	clr.w	(a1)+
	dbra	d0,OpInp1
* Creates port
	sub.l	a1,a1
	move.l	$4.w,a6
	jsr	FindTask(a6)
	move.l	(sp),a1
	lea	Lio(a1),a1
	move.l	d0,$10(a1)
	jsr	AddPort(a6)
* Open device
	lea	DevName(pc),a0
	move.l	(sp),a1
	moveq	#0,d0
	moveq	#0,d1
	jsr	OpenDev(a6)
	move.l	(sp)+,a1
	lea	Lio(a1),a0
	move.l	a0,14(a1)
	rts

; CLOSE input.device
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		xdef	_w_close_input
_w_close_input
ClInput	move.l	a1,-(sp)
; Close device
	move.l	$4.w,a6
	jsr	CloseDev(a6)
; Close port
	move.l	(sp)+,a1
	lea	Lio(a1),a1
	jsr	RemPort(a6)
	rts

; Input handler, branche sur la chaine des inputs.
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IoHandler
	move.l	a5,-(sp)
	move.l	W_Base(pc),a5
; Si inhibe, laisse TOUT passer!
	tst.w	T_Inhibit(a5)
	bne.s	I_Inhibit
; Continue...
	move.b	T_AMOSHere(a5),d4		Si AMOS pas la,
	ext.w	d4
	bne.s	.Skip
	bset	#WFlag_Event,T_WFlags(a5)	Marque des faux events!
.Skip	move.l	a0,d0
	move.l	a0,d2
	moveq	#0,d3
IeLoop	move.b	Ie_Class(a0),d1
	cmp.b	#IeClass_RawMouse,d1
	beq.s	IeMous
	cmp.b	#IeClass_Rawkey,d1
	beq	IeKey
	cmp.b	#IeClass_DiskInserted,d1
	beq.s	IeDIn
	cmp.b	#IeClass_DiskRemoved,d1
	beq.s	IeDOut
IeLp1	move.l	d2,d3
	move.l	(a0),d2
IeLp2	move.l	d2,a0
	bne.s	IeLoop
IeLpX	move.l	(sp)+,a5
	rts
I_Inhibit
	move.l	(sp)+,a5
	move.l	a0,d0
	rts
; Disc inserted
IeDIn	bset	#WFlag_Event,T_WFlags(a5)
	move.w	#-1,T_DiscIn(a5)
	bra.s	IeLp1
; Disc removed
IeDOut	bset	#WFlag_Event,T_WFlags(a5)
	clr.w	T_DiscIn(a5)
	bra.s	IeLp1
; Evenement Mouse, fait le mouvement!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IeMous	tst.w	d4
	beq.s	IeLp1
	bset	#WFlag_Event,T_WFlags(a5)		Flag: un event!
	cmp.l	#Fake_Code,ie_X(a0)	Un faux evenement?
	beq.s	IeFake
IeNof	move.w	T_MouYOld(a5),d1	* Devenir des MOUSERAW
	and.w	#$0003,d1		* 0-> Normal
	beq.s	.norm			* 1-> Trash
	subq.w	#2,d1			* 2-> Tout passe
	bmi.s	IeTrash			* 3-> Mouvements seuls
	beq.s	IeLp1
; Mode key only>>> prend les touches, laisse passer les mouvements
	move.w	ie_Qualifier(a0),T_MouXOld(a5)
	and.w	#%1000111111111111,ie_Qualifier(a0)
	move.w	ie_Code(a0),d1
	and.w	#$7f,d1
	cmp.w	#IECODE_LBUTTON,d1
	beq.s	.ski1
	cmp.w	#IECODE_RBUTTON,d1
	bne.s	IeLp1
.ski1	move.w	#IECODE_NOBUTTON,ie_Code(a0)
	bra.s	IeLp1
; Mode normal>>> prend et met a la poubelle
.norm	move.w	ie_Qualifier(a0),d1
	move.w	d1,T_MouXOld(a5)
	btst	#IEQUALIFIERB_RELATIVEMOUSE,d1
	beq.s	IeTrash
	move.w	ie_X(a0),d1
	add.w	d1,T_MouseX(a5)
	move.w	ie_Y(a0),d1
	add.w	d1,T_MouseY(a5)
; Event to trash!
IeTrash	tst.l	d3
	beq.s	IeTr1
	move.l	d3,a1
	move.l	(a0),d2
	move.l	d2,(a1)
	bra	IeLp2
IeTr1	move.l	(a0),d0
	move.l	d0,a0
	bne	IeLoop
	move.l	(sp)+,a5
	rts
; Faux evenement clavier...
; ~~~~~~~~~~~~~~~~~~~~~~~~~
IeFake	cmp.w	#IEQUALIFIER_RELATIVEMOUSE,ie_Qualifier(a0)
	bne	IeNof
	clr.l	ie_X(a0)			Plus de decalage
	bra	IeLp1				On laisse passer
; Event clavier: prend le caractere au vol
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
IeKey	bset	#WFlag_Event,T_WFlags(a5)
	bsr	Cla_Event
	bne.s	.IeKy1
.IeKy0	tst.w	d4		Event to trash ou non
	bne.s	IeTrash
	bra	IeLp1
; AMIGA-A pressed
; ~~~~~~~~~~~~~~~
.IeKy1	tst.w	T_NoFlip(a5)
	bne.s	.IeKy0
	btst	#WFlag_LoadView,T_WFlags(a5)
	bne.s	.AA
; Appel de TAMOSWb, rapide...
	movem.l	a0-a1/d0-d1,-(sp)
	moveq	#0,d1
	tst.w	d4
	bne.s	.Ska
	moveq	#1,d1
.Ska	bsr	TAMOSWb
	movem.l	(sp)+,a0-a1/d0-d1
	bra	IeTrash
; Marque pour TESTS CYCLIQUES
.AA	bset	#WFlag_AmigaA,T_WFlags(a5)
	bra	IeTrash

; 	Gestion des evenements clavier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	A0=	EVENT KEY
;	D4=	Flag AMOS / WB
Cla_Event
	movem.l	a0-a1/d0-d3,-(sp)
	move.w	Ie_Code(a0),d0
	bclr	#7,d0
	bne	.ClaI2
; Appui sur une touche
; ~~~~~~~~~~~~~~~~~~~~
	cmp.b	#$68,d0				Shifts>>> pas stockes
	bcc.s	.RawK
	cmp.b	#$40,d0
	bcs.s	.RawK
	cmp.b	#$60,d0
	bcc	.Cont
; Conversion a la main des codes speciaux
	lea	Cla_Special-$40(pc),a1
	move.b	0(a1,d0.w),d1
	bpl	.Rien
	cmp.b	#$FF,d1
	beq.s	.RawK
; Une touche de fonction AMOS?
	moveq	#0,d1				Ascii nul
	move.b	Ie_Qualifier+1(a0),d2		Les shifts
	btst	#6,d2
	beq.s	.FFk1
	lea	T_TFF1(a5),a1			Touches 1-10
	bra.s	.FFk2
.FFk1	btst	#7,d2				Pas AMIGA>>> Touche normale
	beq.s	.Rien
	lea	T_TFF2(a5),a1			Touches 11-20
.FFk2	move.w	d0,d2
	sub.w	#$50,d2
	mulu	#FFkLong,d2
	lea	0(a1,d2.w),a1
	tst.b	(a1)
	beq.s	.Rien
	bsr	ClPutK
	moveq	#0,d2
	bra	.ClaIX
; Appel de RAWKEYCONVERT et stockage si AMOS present
.RawK	move.b	Ie_Qualifier+1(a0),d2		Prend CONTROL
	and.b	#%11110111,Ie_Qualifier+1(a0)	Plus de CONTROL
	movem.l	a0/a2/a6,-(sp)
	lea	ConIo(pc),a6			Structure IO
	move.l	20(a6),a6			io_device
	lea	ConBuffer(pc),a1			Buffer de sortie
	sub.l	a2,a2				Current Keymap
	moveq	#LConBuffer,d1			Longueur du buffer
	jsr	-$30(a6)			RawKeyConvert
	move.w	d0,d3
	movem.l	(sp)+,a0/a2/a6
	move.b	d2,Ie_Qualifier+1(a0)		Remet CONTROL
	move.w	Ie_Code(a0),d0
	moveq	#0,d1
	subq.w	#1,d3
	bmi.s	.Rien
	lea	ConBuffer(pc),a1			Une seule touche
	move.b	(a1),d1
.Rien	move.b	Ie_Qualifier+1(a0),d2		Les shifts!
; Amiga-A?
.A	move.b	d2,d3
	and.b	T_AmigA_Shifts(a5),d3
	cmp.b	T_AmigA_Shifts(a5),d3
	bne.s	.AAA
	cmp.b	T_AmigA_Ascii1(a5),d1
	beq.s	.AA
	cmp.b	T_AmigA_Ascii2(a5),d1
	bne.s	.AAA
.AA	moveq	#-1,d2
	bra.s	.ClaI1
; AMOS Not here: stop!
.AAA	tst.w	d4
	beq.s	.Cont
; Est-ce un CONTROL-C?
	btst	#3,d2
	beq.s	.Sto
	cmp.b	#"C",d1
	beq.s	.C
	cmp.b	#"c",d1
	bne.s	.Sto
.C	bset	#BitControl,T_Actualise(a5)
	bra.s	.Cont
; Stocke dans le buffer
.Sto	bsr	Cla_Stocke			On stocke!
; Change la table
.Cont	moveq	#0,d2
.ClaI1	move.w	d0,d1
	and.w	#$0007,d0
	lsr.w	#3,d1
	lea	T_ClTable(a5),a0
	bset	d0,0(a0,d1.w)
.ClaIX	tst.w	d2
	movem.l	(sp)+,a0-a1/d0-d3
	rts
; Relachement d'une touche
; ~~~~~~~~~~~~~~~~~~~~~~~~
.ClaI2	move.w	d0,d1
	and.w	#$0007,d0
	lsr.w	#3,d1
	lea	T_ClTable(a5),a1
	bclr	d0,0(a1,d1.w)
.ClaIF	moveq	#0,d0
	movem.l	(sp)+,a0-a1/d0-d3
	rts

; Table des touches $40->$5f
; ~~~~~~~~~~~~~~~~~~~~~~~~~~
Cla_Special
	dc.b	$ff,$08,$09,$0d,$0d,$1b,$00,$00		$40>$47
	dc.b	$00,$00,$ff,$00,$1e,$1f,$1c,$1d		$48>$4f
	dc.b	$fe,$fe,$fe,$fe,$fe,$fe,$fe,$fe		$50>$57
	dc.b	$fe,$fe,$ff,$ff,$ff,$ff,$ff,$00		$58>$5f

; Stocke D0/D1/D2 dans le buffer clavier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	D0	Rawkey
;	D1	Ascii
;	D2	Shifts
Cla_Stocke
	movem.l	a0/d3,-(sp)
	lea	T_ClBuffer(a5),a0
	move.w	T_ClTete(a5),d3
	addq.w	#3,d3
	cmp.w	#ClLong,d3
	bcs.s	.ClS11
	clr.w	d3
.ClS11	cmp.w	T_ClQueue(a5),d3
	beq.s	.ClS12
	move.w	d3,T_ClTete(a5)
	move.b	d2,0(a0,d3.w)
	move.b	d0,1(a0,d3.w)
	move.b	d1,2(a0,d3.w)
.ClS12	move.b	d2,-4(a0)
	move.b	d0,-3(a0)
	move.b	d1,-1(a0)
.ClSFin	movem.l	(sp)+,a0/d3
	rts

; Envoi d'un faux event souris au systeme
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
WSend_FakeEvent
	movem.l	d0-d1/a1/a6,-(sp)
	lea	Fake_Event(pc),a0
	move.b	#IECLASS_RAWMOUSE,ie_Class(a0)
	clr.b	ie_SubClass(a0)
	move.w	#IECODE_NOBUTTON,ie_Code(a0)
	move.w	#IEQUALIFIER_RELATIVEMOUSE,ie_Qualifier(a0)
	move.l	#Fake_Code,ie_X(a0)
	lea	T_IoDevice(a5),a1
	move.l	a0,io_Data(a1)
	move.w	#IND_WRITEEVENT,io_Command(a1)
	move.l	#22,io_Length(a1)
	move.l	$4.w,a6
	jsr	_LVODoIO(a6)
	movem.l	(sp)+,d0-d1/a1/a6
	rts
Fake_Code	equ	$789A789A
; Faux evenement souris
Fake_Event	dc.l	0				0
		dc.b	IeClass_RawMouse		4
		dc.b	0				5
		dc.w	IECODE_NOBUTTON			6
		dc.w	0				8
		dc.w	0				10
		dc.w	0				12
		dc.l	0				14 Time Stamp
		dc.l	0				18   "    "

; Initialisation / Vide du buffer clavier
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ClInit
ClVide	move.w	T_ClTete(a5),T_ClQueue(a5)
	clr.b	T_ClFlag(a5)
	moveq	#0,d0
	rts

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
WTest_Cyclique

; Verifie AMOS / WB si AA
; ~~~~~~~~~~~~~~~~~~~~~~~
	bclr	#WFlag_AmigaA,T_WFlags(a5)
	beq.s	.NoFlip
	moveq	#0,d1
	tst.b	T_AMOSHere(a5)
	bne.s	.Wb
	moveq	#1,d1
.Wb	bsr	TAMOSWb
.NoFlip
; Envoi des faux messages au WB, en cas de blanker
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	move.l	$4.w,a0
	cmp.w	#36,$14(a0)
	bcs.s	.Noev
	subq.w	#1,T_FakeEventCpt(a5)
	bpl.s	.Noev
	move.w	#50*2,T_FakeEventCpt(a5)
	tst.b	T_AMOSHere(a5)
	beq.s	.Noev
	bsr	WSend_FakeEvent
.Noev
; Verifie l'inhibition
; ~~~~~~~~~~~~~~~~~~~~
	move.l	T_MyTask(a5),a0
	move.l	10(a0),a0
	cmp.b	#"S",(a0)
	bne.s	.Skip
	bsr	AMOS_Stopped
.Skip	rts

;	Cet AMOS est inhibe par un premier!
; ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AMOS_Stopped
	movem.l	a0-a6/d0-d7,-(sp)
	move.l	a0,a4
; Fait revenir le WB / Stoppe les interrupts
	move.w	#-1,T_Inhibit(a5)
	moveq	#-1,d1
	bsr	TAMOSWb
	move.w	d1,d7
	moveq	#0,d1
	bsr	TAMOSWb
; Stoppe les interrupts
	bsr	Rem_VBL
; Arrete le son
	move.w	#$000F,$Dff096
; Change le "S"top en "W"
	move.b	#"W",(a4)
; Attend qu'il se retransforme en " "
.Wait	move.l	T_GfxBase(a5),a6
	jsr	-270(a6)
	cmp.b	#"W",(a4)
	beq.s	.Wait
; Ramene le programme..
	bsr	Add_VBL
	tst.w	d7
	beq.s	.Skip
	moveq	#1,d1
	bsr	TAMOSWb
.Skip	clr.w	T_Inhibit(a5)
	movem.l	(sp)+,a0-a6/d0-d7
	rts

***********************************************************
*	DEMARRAGE A FROID DES FENETRES
***********************************************************
WiInit:	lea	_w_wi_in,a0
	move.l	a0,T_WiVect(a5)
	rts

; The window functions are in C++ (src/w/windows.cpp): WiInit points
; T_WiVect at their table there.

***********************************************************
*	ARRET FINAL DES FENETRES
***********************************************************
WiEnd:	rts

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
WRequest_OnOff
	move.w	d0,T_ReqFlag(a5)
	rts

; System requesters: src/w/requester.cpp (a5 the data zone for it)
AutoReq	move.l	a5,-(sp)
	move.l	W_Base(pc),a5
	jsr	_w_auto_req
	move.l	(sp)+,a5
	rts
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
