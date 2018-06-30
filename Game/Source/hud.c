

#define HUD_ELEMENT_COUNT 1

#define HUD_BORDER_PADDING 30
#define HUD_Z_LAYER 2

#define HUD_BARS_XPADDING 3
#define HUD_DIST_BETWEEN_BARS 8

BMAP * hud_bar_background_bmap = "hud_bar_background.png";

BMAP * hud_health_label_bmap = "health_label.png";
BMAP * hud_healthbar_bmap = "healthbar.png";

BMAP * hud_ammo_label_bmap = "ammo_label.png";
BMAP * hud_ammobar_bmap = "ammobar.png";

PANEL* HUD_HP_label =
{
	bmap = hud_health_label_bmap;
	flags = TRANSLUCENT;
	layer = 2;
}

PANEL* HUD_HP_bars =
{
	bmap = hud_bar_background_bmap;
	flags = TRANSLUCENT;
	layer = 2;
}

PANEL* HUD_Ammo_label =
{
	bmap = hud_ammo_label_bmap;
	flags = TRANSLUCENT;
	layer = 2;
}
PANEL* HUD_Ammo_bars =
{
	bmap = hud_bar_background_bmap;
	flags = TRANSLUCENT;
	layer = 2;
}


void hud_init()
{
}

void hud_show_bar(PANEL *label, PANEL *bar, var offsetY) 
{
	label->pos_x = HUD_BORDER_PADDING;
	label->pos_y = screen_size.y - label.size_y - HUD_BORDER_PADDING- offsetY;
	
	bar->pos_x = HUD_BORDER_PADDING + label->size_x +3;
	bar->pos_y = screen_size.y - label->size_y - HUD_BORDER_PADDING -offsetY;
	
	pan_setwindow  (bar, 0, 0,0, 0, bmap_height(hud_healthbar_bmap), hud_healthbar_bmap, 0,0);
	
	set(label, SHOW);
	set(bar, SHOW);
}

void hud_update_bar(PANEL *bar, BMAP *source, var current_value, var max_value)
{
	var bar_size = bar->size_x-HUD_BARS_XPADDING*2;
	bar_size *= current_value / max_value;
	
	var HUD_BARS_YPADDING = (bar->size_y-bmap_height(source))/2;
	
	pan_setwindow  (	bar, 1, 
							HUD_BARS_XPADDING,HUD_BARS_YPADDING, 
							bar_size ,bmap_height(source), 
							source, 
							0,0);
}


void hud_show()
{
	var distance_between_bars = HUD_HP_label->size_y + HUD_DIST_BETWEEN_BARS;
	hud_show_bar(HUD_HP_label, HUD_HP_bars, 0);
	hud_show_bar(HUD_Ammo_label, HUD_Ammo_bars, distance_between_bars);
}

void hud_update()
{
	//TODO!
	var PLAYER_MAXHEALTH = 50;
	var PLAYER_HEALTH = 30;
	//
	
    hud_update_bar(HUD_HP_bars, hud_healthbar_bmap, PLAYER_HEALTH, PLAYER_MAXHEALTH);
    hud_update_bar(HUD_Ammo_bars, hud_ammobar_bmap, PLAYER_HEALTH, PLAYER_MAXHEALTH);
}