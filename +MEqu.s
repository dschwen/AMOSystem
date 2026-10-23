;_____________________________________________________________________________
;
;	The monitor's data (Mon_Base): included by +Monitor.s, and read by
;	tools/gen_layout.py for the C++ (struct MonData)
;_____________________________________________________________________________
Tt_ChannelN	equ	$BBCCDDEE
Inf_Size	equ	88*100		; 100 lignes max
		RsReset
Inf_Sly		rs.w	1
Inf_Slp1	rs.w	1
Inf_Slp2	rs.w	1
Inf_Pos		rs.l	1
Inf_YPos	rs.w	1
Inf_AlertOn	rs.w	1
Inf_NLigne	rs.w	1
Inf_Buf		rs.l	1

Tt_Dialogs	rs.l	1
Tt_CurBanks	rs.l	1
Tt_CurDialogs	rs.l	1
Tt_Channel	rs.l	1
Tt_Variables	rs.l	1
Tt_Resource	rs.l	1
Tt_Program	rs.l	1
Tt_JError	rs.l	1
Tt_PrgHelp	rs.l	1
Tt_Init		rs.w	1
Tt_View		rs.w	1
Tt_ViewTX	rs.w	1
Tt_ViewTY	rs.w	1
Tt_ViewNP	rs.w	1
Tt_ViewAd	rs.l	1
Tt_Active	rs.w	1
Tt_Accel	rs.w	1
Tt_Ecran	rs.w	1
Tt_YMark	rs.w	1
Tt_X1Mark	rs.w	1
Tt_X2Mark	rs.w	1
Tt_YInfo	rs.w	1
Tt_X1Info	rs.w	1
Tt_X2Info	rs.w	1
Tt_XNext	rs.w	1
Tt_YNext	rs.w	1
Tt_PLine	rs.l	1
Tt_ALine	rs.l	1
Tt_EvPile	rs.l	1
Tt_EvDebut	rs.l	1
Tt_InfOn	rs.w	1
Tt_DCurLine	rs.l	1
Tt_Run		rs.w	1
Tt_MapAdr	rs.l	1
Tt_MapLong	rs.l	1
Tt_MKey		rs.w	1
Tt_MFlag	rs.w	1
Tt_Adress	rs.l	1
Tt_Locked	rs.w	1
Tt_CurLigne	rs.l	1
Tt_DebProc	rs.l	1
Tt_NotHere	rs.b	1
		rs.b	1
Tt_BufT		rs.l	1
Tt_Ligne	rs.l	1

Ttt_BufE	rs.l	1
Ttt_XPos	rs.w	1
Ttt_YPos	rs.w	1
Ttt_Sy		rs.w	1
Ttt_Tlh		rs.w	1
Ttt_Mode	rs.w	1
Ttt_Tlb		rs.w	1
ttsl_hp1	rs.w	1
ttsl_hp2	rs.w	1
ttsl_vp1	rs.w	1
ttsl_vp2	rs.w	1

Step_Data	rs.l	10
Step_Break	rs.l	1
Step_Stop	rs.w	1
Step_Mess	rs.l	1
Step_Mode	rs.w	1

Hlp_Map		rs.l	1
Hlp_LMap	rs.l	1
Hlp_Texte	rs.l	1
Hlp_LTexte	rs.l	1

ViewScrols	rs.w	4*8
Tt_DzLong	equ	__RS
