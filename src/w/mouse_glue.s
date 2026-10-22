; ---------------------------------------------------------------------
;	Mouse, coordinates, zones (SyCall): w/mouse.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_w_sy_mouse_key,_w_sy_mouse_click,_w_sy_mouse_change,_w_sy_mouse_store
		XREF	_w_sy_mouse_recall,_w_sy_mouse_hide,_w_sy_mouse_show,_w_sy_mouse_xy
		XREF	_w_sy_xy_screen,_w_sy_xy_hard,_w_sy_xy_window,_w_sy_screen_at,_w_sy_mouse_screen
		XREF	_w_sy_mouse_set,_w_sy_mouse_limit,_w_sy_mouse_limit_screen,_w_sy_zones_reserve
		XREF	_w_sy_zone_reset,_w_sy_zone_set,_w_sy_zone_graphic,_w_sy_zone_hard
		XREF	_w_sy_zone_mouse,_w_sy_hardcol_set,_w_sy_hardcol_get

		SECTION	.text,CODE

		xdef	_w_mouse_key,_w_mouse_click,_w_mouse_change,_w_mouse_store
		xdef	_w_mouse_recall,_w_mouse_hide,_w_mouse_show,_w_mouse_xy
		xdef	_w_xy_screen,_w_xy_hard,_w_xy_window,_w_screen_at,_w_mouse_screen
		xdef	_w_mouse_set,_w_mouse_limit,_w_mouse_limit_screen,_w_zones_reserve
		xdef	_w_zone_reset,_w_sy_set_z,_w_zone_graphic,_w_zone_hard
		xdef	_w_zone_mouse,_w_hardcol_set,_w_hardcol_get
_w_mouse_key	WEntry	_w_sy_mouse_key,l
_w_mouse_click	WEntry	_w_sy_mouse_click,l
_w_mouse_change	WEntry	_w_sy_mouse_change,l
_w_mouse_store	WEntry	_w_sy_mouse_store,l
_w_mouse_recall	WEntry	_w_sy_mouse_recall,l
_w_mouse_hide	WEntry	_w_sy_mouse_hide,l
_w_mouse_show	WEntry	_w_sy_mouse_show,l
_w_mouse_xy	WEntry	_w_sy_mouse_xy,l
_w_xy_screen	WEntry	_w_sy_xy_screen,l
_w_xy_hard	WEntry	_w_sy_xy_hard,l
_w_xy_window	WEntry	_w_sy_xy_window,l
_w_screen_at	WEntry	_w_sy_screen_at,l
_w_mouse_screen	WEntry	_w_sy_mouse_screen,l
_w_mouse_set	WEntry	_w_sy_mouse_set,l
_w_mouse_limit	WEntry	_w_sy_mouse_limit,l
_w_mouse_limit_screen WEntry _w_sy_mouse_limit_screen,l
_w_zones_reserve WEntry	_w_sy_zones_reserve,l
_w_zone_reset	WEntry	_w_sy_zone_reset,l
_w_sy_set_z	WEntry	_w_sy_zone_set,l
_w_zone_graphic	WEntry	_w_sy_zone_graphic,l
_w_zone_hard	WEntry	_w_sy_zone_hard,l
_w_zone_mouse	WEntry	_w_sy_zone_mouse,l
_w_hardcol_set	WEntry	_w_sy_hardcol_set,l
_w_hardcol_get	WEntry	_w_sy_hardcol_get,l
