

#define HUD_ELEMENT_COUNT 1

#define HUD_BORDER_PADDING 30
#define HUD_Z_LAYER 2

#define HUD_BARS_XPADDING 3

BMAP * hud_health_label = "health_label.png";
BMAP * hud_bar_background = "hud_bar_background.png";
BMAP * hud_healthbar = "healthbar.png";
BMAP * hud_ammobar = "ammobar.png";

PANEL* HUD_HP_label =
{
	bmap = hud_health_label;
	flags = TRANSLUCENT;
	layer = 2;
}
PANEL* HUD_HP_bars =
{
	bmap = hud_bar_background;
	flags = TRANSLUCENT;
	layer = 2;
}

PANEL* HUD_Ammo_label =
{
	bmap = hud_health_label;
	flags = TRANSLUCENT;
	layer = 2;
}
PANEL* HUD_Ammo_bars =
{
	bmap = hud_bar_background;
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
	bar->pos_y = screen_size.y - label->size_y - HUD_BORDER_PADDING;
	
	pan_setwindow  (bar, 0, 0,0, 0, bmap_height(hud_healthbar), hud_healthbar, 0,0);
	
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
	hud_show_bar(HUD_HP_label, HUD_HP_bars, 0);
}

void hud_update()
{
	//TODO!
	var PLAYER_MAXHEALTH = 50;
	var PLAYER_HEALTH = 30;
	//
	
    hud_update_bar(HUD_HP_bars, hud_healthbar, PLAYER_HEALTH, PLAYER_MAXHEALTH);
}