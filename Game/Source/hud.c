#include "weapons.h"

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


PANEL* HUD_crosshair =
{
	bmap = "fadenkreuz.png";
	flags = TRANSLUCENT;
	layer = 2;
}

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

PANEL *hud_weapon_icon[4];

void hud_init()
{
	hud_weapon_icon[WEAPON_SWORD-1] = pan_create("bmap = label_sword.png;",2);
	hud_weapon_icon[WEAPON_SHOTGUN-1] = pan_create("bmap = label_shotgun.png",2);
	hud_weapon_icon[WEAPON_CELLGUN-1] = pan_create("bmap = label_cellgun.png",2);
	hud_weapon_icon[WEAPON_FLAMETHROWER-1] = pan_create("bmap = label_flamethrower.png",2);
}

void hud_place_label(PANEL *label, var offsetY)
{
	label->pos_x = HUD_BORDER_PADDING;
	label->pos_y = screen_size.y - label.size_y - HUD_BORDER_PADDING - offsetY;
	
	set(label, SHOW);
}

void hud_place_bar(PANEL* label, PANEL *bar, var offsetY) 
{
	
	bar->pos_x = HUD_BORDER_PADDING + label->size_x +3;
	bar->pos_y = screen_size.y - label->size_y - HUD_BORDER_PADDING -offsetY;
	
	pan_setwindow  (bar, 0, 0,0, 0, bmap_height(hud_healthbar_bmap), hud_healthbar_bmap, 0,0);
	
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
	hud_place_label(HUD_HP_label, 0);
	
	int i;
	for(i = 0; i<4; ++i)
		hud_place_label(hud_weapon_icon[i], distance_between_bars);
	
	
	hud_place_bar(HUD_HP_label, HUD_HP_bars, 0);
	hud_place_bar(hud_weapon_icon[0], HUD_Ammo_bars, distance_between_bars);
	
	HUD_crosshair->pos_x = (screen_size.x - HUD_crosshair.size_x) /2;
	HUD_crosshair->pos_y = (screen_size.y - HUD_crosshair.size_y) /2;
	HUD_crosshair->alpha = 10;
	set(HUD_crosshair, SHOW);
}

void hud_show_weapon_icon(int id)
{
	int i;
	for(i = 0; i<4; ++i)
		reset(hud_weapon_icon[i], SHOW);
	if(id >= 0)
		set(hud_weapon_icon[id], SHOW);
}

void hud_update()
{
	//TODO!
	var PLAYER_MAXHEALTH = 50;
	var PLAYER_HEALTH = 30;
	//
   hud_update_bar(HUD_HP_bars, hud_healthbar_bmap, PLAYER_HEALTH, PLAYER_MAXHEALTH);
    
    
    
	int weaponID = 1;//weapons_get_current();
	
	hud_show_weapon_icon(weaponID-1);
	if(weaponID > 0)
	{
		var ammo = 20;//weapons_get_ammo();
		var max_ammo = 100;//weapons_get_max_ammo();
		
		hud_update_bar(HUD_Ammo_bars, hud_ammobar_bmap, ammo, max_ammo);
	}
}