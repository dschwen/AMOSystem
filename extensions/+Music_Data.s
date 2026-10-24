;---------------------------------------------------------------------
;	The data of the Music extension (src/ext/music.cpp), described
;	for tools/gen_layout.py: the extension's data zone (MusicData,
;	the address =Mu Base gives; its first four bytes are the VU
;	meters), an envelope, a wave, a music voice and the music's
;	table, a sample interrupt.
;---------------------------------------------------------------------

; An envelope (ADSR) of a voice
		RsReset
EnvNb:		rs.w	1		Steps left in this part
EnvDVol:	rs.w	1		Volume of the voice
EnvVol:		rs.l	1		Current volume, 16.16
EnvDelta:	rs.l 	1		Step of the volume, 16.16
EnvAd:		rs.l	1		Next part
EnvDeb:		rs.l	1		First part
EnvLong:	equ 	__RS

; A wave, in a list
LWave:		equ 	256+128+64+32+16+8+4+2
LNoise:		equ 	LWave
		RsReset
WaveNext:	rs.l	1
WaveNb:		rs.w	1
WaveEnv:	rs.w	16*2
WaveDeb:	rs.b	LWave
WaveLong:	equ 	__RS

; A voice of a music
		RsReset
VoiAdr		rs.l	1
VoiDeb		rs.l	1
VoiInst		rs.l	1
VoiDPat		rs.l	1
VoiPat		rs.l	1
VoiCpt		rs.w	1
VoiRep		rs.w	1
VoiNote		rs.w	1
VoiDVol		rs.w	1
VoiVol		rs.w	1
VoiEffect	rs.l	1
VoiValue	rs.w	1
VoiPToTo	rs.w	1
VoiPTone	rs.b	1
VoiVib		rs.b	1
VoiLong		equ 	__RS

; A music being played: its four voices, then
		RsReset
MuVoices	rs.b	VoiLong*4
MuCpt		rs.w	1
MuTempo		rs.w	1
MuStart		rs.w	1
MuStop		rs.w	1
MuLong		equ 	__RS

; The interrupt of a voice playing a sample: an exec Interrupt, then
		RsReset
Sami_is		rs.b	22
Sami_bit	rs.w	1
Sami_dma	rs.w	1
Sami_reg	rs.w	1
Sami_adr	rs.l	1
Sami_long	rs.l	1
Sami_pos	rs.l	1
Sami_rpos	rs.l	1
Sami_radr	rs.l	1
Sami_rlong	rs.l	1
Sami_dvol	rs.w	1
Sami_old	rs.l	1
Sami_intl	equ	__RS

; The data zone
		RsReset
MuVu		rs.b	4		VU meters
MuBase		rs.l	1		Music being played
MusAdr		rs.l	1
MusClock	rs.l	1		Clock (PAL / NTSC)
WaveBase	rs.l	1		The waves
Waves		rs.w	4		Wave of each voice (-n: sample n)
EnvOn		rs.w	1		Envelopes running
EnvBase		rs.b	EnvLong*4	Envelopes
SamBank		rs.w	1		Sample bank
BSeed		rs.w	1		Random seed
Noise		rs.w	1		Voices playing noise
PNoise		rs.w	1		Position in the noise
TempoBase	rs.w	1		100 (PAL) / 120 (NTSC)
MusBank		rs.l	1		Music bank
MusCheck	rs.l	1		Its check sum
BankInst	rs.l	1		Instruments
BankSong	rs.l	1		Songs
BankPat		rs.l	1		Patterns
MuNumber	rs.w	1		Musics playing (3 at most)
MuVolume	rs.w	1		Music volume
MuDMAsk		rs.w	1		Voices of the music
MuReStart	rs.w	1		Voices to give back to the music
MuChip		rs.l	4		Audio registers of each voice
FoEnd		rs.w	1		Empty pattern ($8000)
MuBuffer	rs.b	MuLong*3	The musics
Sami_int	rs.b	Sami_intl*4	Sample interrupts
Sami_oldena	rs.w	1
Sami_empty	rs.l	1
Sami_bits	rs.w	1
Sami_handad	rs.l	1		Sample interrupt code
Sami_flag	rs.w	1		Installed
SamLoops	rs.w	1		Voices looping their sample
Amaps		rs.b	4		Narrator: audio channels
TranBase	rs.l	1		translator.library
WriteIo		rs.l	1		narrator.device
WritePort	rs.l	1
ReadIo		rs.b	92		Its mouths
ReadPort	rs.l	1
Med_Bank	rs.w	1		MED bank
Med_Base	rs.l	1		medplayer.library
Med_Adr		rs.l	1		Module playing
Med_On		rs.b	1
Med_Midi	rs.b	1
Track_Bank	rs.w	1		Tracker bank
mt_on		rs.b	1		Tracker playing
Track_Loop	rs.b	1
Track_Stop	rs.b	1
mt_pad		rs.b	1
mt_speed	rs.b	1
mt_counter	rs.b	1
mt_pattpos	rs.w	1
mt_songpos	rs.b	1
mt_break	rs.b	1
mt_dmacon	rs.w	1
mt_samplestarts	rs.l	31
mt_voice	rs.w	14*4		The tracker's voices
mt_data		rs.l	1		Module playing
mt_mulu		rs.l	32		Its sample headers
MusicDataLong	equ	__RS
