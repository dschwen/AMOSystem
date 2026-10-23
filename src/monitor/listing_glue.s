; ---------------------------------------------------------------------
;	The monitor's listing window: monitor/listing.cpp
; ---------------------------------------------------------------------

		Include	"glue.i"
		Include	"w/wglue.i"

		XREF	_w_go
		XREF	_mon_centre_click
		XREF	_mon_slider_horizontal_click
		XREF	_mon_slider_vertical_click
		XREF	_mon_listing_centre
		XREF	_mon_listing_show
		XREF	_mon_listing_new
		XREF	_mon_listing_untokenise
		XREF	_mon_separation_drag
		XREF	_mon_listing_screen
		XREF	_mon_listing_size
		XREF	_mon_listing_click
		XREF	_mon_choose
		XREF	_mon_mouse_position
		XREF	_mon_split
		XREF	_mon_no_init

		SECTION	.text,CODE

		xdef	_mon_centre_click_entry
_mon_centre_click_entry WEntry _mon_centre_click,l
		xdef	_mon_slider_horizontal_click_entry
_mon_slider_horizontal_click_entry WEntry _mon_slider_horizontal_click,l
		xdef	_mon_slider_vertical_click_entry
_mon_slider_vertical_click_entry WEntry _mon_slider_vertical_click,l
		xdef	_mon_listing_centre_entry
_mon_listing_centre_entry WEntry _mon_listing_centre,l
		xdef	_mon_listing_show_entry
_mon_listing_show_entry WEntry _mon_listing_show,l
		xdef	_mon_listing_new_entry
_mon_listing_new_entry WEntry _mon_listing_new,l
		xdef	_mon_listing_untokenise_entry
_mon_listing_untokenise_entry WEntry _mon_listing_untokenise,l
		xdef	_mon_separation_drag_entry
_mon_separation_drag_entry WEntry _mon_separation_drag,l
		xdef	_mon_listing_screen_entry
_mon_listing_screen_entry WEntry _mon_listing_screen,l
		xdef	_mon_listing_size_entry
_mon_listing_size_entry WEntry _mon_listing_size,l
		xdef	_mon_listing_click_entry
_mon_listing_click_entry WEntry _mon_listing_click,l
		xdef	_mon_choose_entry
_mon_choose_entry WEntry _mon_choose,l
		xdef	_mon_mouse_position_entry
_mon_mouse_position_entry WEntry _mon_mouse_position,l
		xdef	_mon_split_entry
_mon_split_entry WEntry _mon_split,l
		xdef	_mon_no_init_entry
_mon_no_init_entry WEntry _mon_no_init,l
