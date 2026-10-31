; ---------------------------------------------------------------------
;	The Compact extension: ext/compact.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	Ext_Compact_InPack2,_ext_pack2,2,None
		LibCall	Ext_Compact_InPack6,_ext_pack6,6,None
		LibCall	Ext_Compact_InSPack2,_ext_spack2,2,None
		LibCall	Ext_Compact_InSPack6,_ext_spack6,6,None
		LibCall	Ext_Compact_InUnpack1,_ext_unpack1,1,None
		LibCall	Ext_Compact_InUnpack2,_ext_unpack2,2,None
		LibCall	Ext_Compact_InUnpack3,_ext_unpack3,3,None

; The cold start, the errors
		RegRoutine Ext_Compact_Compact_Cold,_ext_compact_cold
		RegRoutine Ext_Compact_Custom,_ext_compact_custom
		RegRoutine Ext_Compact_Custom2,_ext_compact_custom2
