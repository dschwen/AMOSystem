; ---------------------------------------------------------------------
;	The Music extension: ext/music.cpp, ext/music_play.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"
		Include	"+Music_Data.s"

		SECTION	.text,CODE

		LibCall	Ext_Music_InSay2,_ext_say2,2,None
		LibCall	Ext_Music_InSay1,_ext_say1,1,None
		LibCall	Ext_Music_InSetTalk,_ext_set_talk,4,None
		LibCall	Ext_Music_FnMouthWidth,_ext_fn_mouth_width,0,Int
		LibCall	Ext_Music_FnMouthHeight,_ext_fn_mouth_height,0,Int
		LibCall	Ext_Music_InBell0,_ext_bell0,0,None
		LibCall	Ext_Music_InBell1,_ext_bell1,1,None
		LibCall	Ext_Music_InBoom,_ext_boom,0,None
		LibCall	Ext_Music_InShoot,_ext_shoot,0,None
		LibCall	Ext_Music_InVolume1,_ext_volume1,1,None
		LibCall	Ext_Music_InVolume2,_ext_volume2,2,None
		LibCall	Ext_Music_InTalkStop,_ext_talk_stop,0,None
		LibCall	Ext_Music_InPlay2,_ext_play2,2,None
		LibCall	Ext_Music_InPlay3,_ext_play3,3,None
		LibCall	Ext_Music_InPlayOff0,_ext_play_off0,0,None
		LibCall	Ext_Music_InPlayOff1,_ext_play_off1,1,None
		LibCall	Ext_Music_InSamBank,_ext_sam_bank,1,None
		LibCall	Ext_Music_InSamLoopOn1,_ext_sam_loop_on1,1,None
		LibCall	Ext_Music_InSamLoopOn0,_ext_sam_loop_on0,0,None
		LibCall	Ext_Music_InSamLoopOff1,_ext_sam_loop_off1,1,None
		LibCall	Ext_Music_InSamLoopOff0,_ext_sam_loop_off0,0,None
		LibCall	Ext_Music_InNoiseTo,_ext_noise_to,1,None
		LibCall	Ext_Music_InSampleTo,_ext_sample_to,2,None
		LibCall	Ext_Music_InSamPlay1,_ext_sam_play1,1,None
		LibCall	Ext_Music_InSamPlay2,_ext_sam_play2,2,None
		LibCall	Ext_Music_InSamPlay3,_ext_sam_play3,3,None
		LibCall	Ext_Music_InSamRaw,_ext_sam_raw,4,None
		LibCall	Ext_Music_InSload,_ext_sload,3,None
		LibCall	Ext_Music_InWave,_ext_wave,2,None
		LibCall	Ext_Music_InSetWave,_ext_set_wave,2,None
		LibCall	Ext_Music_InDelWave,_ext_del_wave,1,None
		LibCall	Ext_Music_InSetEnvel,_ext_set_envel,4,None
		LibCall	Ext_Music_InMusicOff,_ext_music_off,0,None
		LibCall	Ext_Music_InMusicStop,_ext_music_stop,0,None
		LibCall	Ext_Music_InMvolume,_ext_mvolume,1,None
		LibCall	Ext_Music_InVoice,_ext_voice,1,None
		LibCall	Ext_Music_InMusic,_ext_music,1,None
		LibCall	Ext_Music_InTempo,_ext_tempo,1,None
		LibCall	Ext_Music_FnVuMeter,_ext_fn_vumeter,1,Int
		LibCall	Ext_Music_FnMusicBase,_ext_fn_mubase,0,Int
		LibCall	Ext_Music_InLedOn,_ext_led_on,0,None
		LibCall	Ext_Music_InLedOf,_ext_led_off,0,None
		LibCall	Ext_Music_FnSamSwapped,_ext_fn_sam_swapped,1,Int
		LibCall	Ext_Music_InSamSwap,_ext_sam_swap,3,None
		LibCall	Ext_Music_InSamStop0,_ext_sam_stop0,0,None
		LibCall	Ext_Music_InSamStop1,_ext_sam_stop1,1,None
		LibCall	Ext_Music_InTrackLoad,_ext_track_load,2,None
		LibCall	Ext_Music_InTrackStop,_ext_track_stop,0,None
		LibCall	Ext_Music_InTrackLoopOn,_ext_track_loop_on,0,None
		LibCall	Ext_Music_InTrackLoopOff,_ext_track_loop_off,0,None
		LibCall	Ext_Music_InTrackPlay0,_ext_track_play0,0,None
		LibCall	Ext_Music_InTrackPlay1,_ext_track_play1,1,None
		LibCall	Ext_Music_InTrackPlay2,_ext_track_play2,2,None
		LibCall	Ext_Music_InMouthRead,_ext_mouth_read,0,None
		LibCall	Ext_Music_InTalkMisc,_ext_talk_misc,2,None
		LibCall	Ext_Music_InSsave,_ext_ssave,3,None
		LibCall	Ext_Music_InMedLoad,_ext_med_load,2,None
		LibCall	Ext_Music_InMedStop,_ext_med_stop,0,None
		LibCall	Ext_Music_InMedPlay0,_ext_med_play0,0,None
		LibCall	Ext_Music_InMedPlay1,_ext_med_play1,1,None
		LibCall	Ext_Music_InMedPlay2,_ext_med_play2,2,None
		LibCall	Ext_Music_InMedMidiOn,_ext_med_midi_on,0,None
		LibCall	Ext_Music_InMedCont,_ext_med_cont,0,None

; MusInit, from the extension's cold start
		RoutineCall Ext_Music_MusInit,_ext_music_init
		RegRoutine Ext_Music_Mus_Cold,_ext_music_cold

; The routines handed to AMOS: Default, End, the check of the banks
		RoutineCall _music_default,_ext_music_default
		RoutineCall _music_end,_ext_music_end
		RoutineCall _music_bank_check,_ext_music_bank_check

; The audio interrupts of the samples: exec calls them with a1 = is_Data
		xdef	_music_sami_entry
_music_sami_entry
		move.l	a1,-(sp)
		jsr	_ext_music_sami
		addq.l	#4,sp
		rts

; uint32_t music_lvo(uint32_t *r, uint32_t base, int32_t lvo): the
; library function with d0, d1, a0, a1 from r; d0 back (also in r[0])
		xdef	_music_lvo
_music_lvo	movem.l	d2-d7/a2-a6,-(sp)
		move.l	4+44(sp),a2
		move.l	8+44(sp),a6
		move.l	12+44(sp),d2
		movem.l	(a2),d0-d1/a0-a1
		jsr	0(a6,d2.l)
		move.l	4+44(sp),a2
		move.l	d0,(a2)
		movem.l	(sp)+,d2-d7/a2-a6
		rts

; GoSam with registers, for the editor (+Edit.s, editor/config.cpp): it
; calls the address 4 bytes before the data zone. d1 voices, a1 sample,
; d2 length, d3 frequency.
		XREF	_ext_music_go_sam
		RegRoutine _music_go_sam,_ext_music_go_sam_reg

; The data zone (described by +Music_Data.s), after its entry for the
; editor
		SECTION	music_data,DATA
music_jump	jmp	_music_go_sam
		bra.w	music_jump
		xdef	_music_data
_music_data	ds.b	MusicDataLong
