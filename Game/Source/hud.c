#include "weapons.h"
#include "movement.h"

#define HUD_ELEMENT_COUNT 1

#define HUD_BORDER_PADDING 30
#define HUD_Z_LAYER 2

#define HUD_BARS_XPADDING 3
#define HUD_DIST_BETWEEN_BARS 0

#define HUD_BARS_MAX_ALPHA 50

#define HUD_AMMO_COLOR vector(32,200,255)

BMAP * hud_bar_background_bmap = "hud_bar_background.png";

BMAP * hud_health_label_bmap = "health_label.png";
BMAP * hud_healthbar_bmap = "healthbar.png";

//BMAP * hud_ammo_label_bmap = "ammo_label.png";
BMAP * hud_ammobar_bmap = "ammobar.png";


PANEL* HUD_crosshair =
{
	bmap = "fadenkreuz.png";
	flags = TRANSLUCENT;
	layer = 2;
	scale_x = 0.5;
	scale_y = 0.5;
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
	flags = TRANSLUCENT | LIGHT;
	layer = 2;
}
/*
PANEL* HUD_Ammo_label =
{
	bmap = hud_ammo_label_bmap;
	flags = TRANSLUCENT;
	layer = 2;
}*/
PANEL* HUD_Ammo_bars =
{
	bmap = hud_bar_background_bmap;
	flags = TRANSLUCENT;
	layer = 2;
}

TEXT* HUD_Ammo_infotext =	
{
	layer = 3;
	font = "Console#20b";
	blue = 0;
	green = 0;
	red = 0;
	string ("asd");
	flags = CENTER_X | CENTER_Y ;
} 

PANEL *hud_weapon_icon[4];

void hud_init()
{
	hud_weapon_icon[WEAPON_SWORD-1] = pan_create("bmap = label_sword.png;",2);
	hud_weapon_icon[WEAPON_SHOTGUN-1] = pan_create("bmap = label_shotgun.png",2);
	hud_weapon_icon[WEAPON_CELLGUN-1] = pan_create("bmap = label_cellgun.png",2);
	hud_weapon_icon[WEAPON_FLAMETHROWER-1] = pan_create("bmap = label_flamethrower.png",2);
	/*
	int i;
	for(i=0; i<4; ++i)
	{
		hud_weapon_icon[i].scale_x = 0.8;
		hud_weapon_icon[i].scale_y = 0.8;
	}*/
}

void hud_place_label(PANEL *label, var offsetY)
{
	label->pos_x = HUD_BORDER_PADDING;
	label->pos_y = screen_size.y - label.size_y*label.scale_y - HUD_BORDER_PADDING - offsetY;
	label->alpha = HUD_BARS_MAX_ALPHA;
	
	set(label, SHOW);
}

void hud_place_bar(PANEL* label, PANEL *bar, var offsetY) 
{
	var label_bar_ydiff = label->size_y*label->scale_y - bar->size_y;
	
	bar->pos_x = HUD_BORDER_PADDING + label->size_x*label->scale_x +3;
	bar->pos_y = screen_size.y - label->size_y*label->scale_y - HUD_BORDER_PADDING -offsetY + label_bar_ydiff/2;
	bar->alpha = HUD_BARS_MAX_ALPHA;
	
	pan_setwindow  (bar, 0, 0,0, 0, bmap_height(hud_healthbar_bmap), hud_healthbar_bmap, 0,0);
	
	set(bar, SHOW);
}

void hud_update_bar(PANEL *bar, BMAP *source, var current_value, var max_value)
{
	var bar_size = bar->size_x-HUD_BARS_XPADDING*2;
	bar_size *= current_value / max_value;
	
	var HUD_BARS_YPADDING = (bar->size_y*bar->scale_y - bmap_height(source))/2;
	
	pan_setwindow  (	bar, 1, 
							HUD_BARS_XPADDING,HUD_BARS_YPADDING, 
							bar_size ,bmap_height(source)*bar->scale_y, 
							source, 
							0,0);
}


void hud_show()
{
	var ammo_YOffset = HUD_HP_bars->size_y*HUD_HP_bars->scale_y;
	var distance_between_bars = ammo_YOffset + HUD_DIST_BETWEEN_BARS;
	
	int i;
	for(i = 0; i<4; ++i)
	{
		hud_place_label(hud_weapon_icon[i], distance_between_bars);
		vec_set(hud_weapon_icon[i].blue, HUD_AMMO_COLOR);
		set(hud_weapon_icon[i], LIGHT);
	}
	
	hud_place_bar(hud_weapon_icon[0], HUD_Ammo_bars, distance_between_bars);
	
	
	var desiredWidth = hud_weapon_icon[0]->size_x*hud_weapon_icon[0]->scale_x + 3 + HUD_Ammo_bars->size_x*HUD_Ammo_bars->scale_x;
	HUD_HP_bars.scale_x = desiredWidth / HUD_HP_bars.size_x;
	HUD_HP_bars.pos_x = HUD_BORDER_PADDING;
	HUD_HP_bars.pos_y = screen_size.y - HUD_HP_bars.size_y*HUD_HP_bars.scale_y - HUD_BORDER_PADDING;
	pan_setwindow  (HUD_HP_bars, 0, 0,0, 0, bmap_height(hud_healthbar_bmap), hud_healthbar_bmap, 0,0);
	set(HUD_HP_bars, SHOW);
	
	HUD_crosshair->pos_x = (screen_size.x - HUD_crosshair.size_x*HUD_crosshair.scale_x) /2;
	HUD_crosshair->pos_y = (screen_size.y - HUD_crosshair.size_y*HUD_crosshair.scale_x) /2;
	HUD_crosshair->alpha = 10;
	set(HUD_crosshair, SHOW);
	
	HUD_Ammo_infotext->pos_x = HUD_Ammo_bars->pos_x + HUD_Ammo_bars->size_x*HUD_Ammo_bars->scale_x/2;
	HUD_Ammo_infotext->pos_y = HUD_Ammo_bars->pos_y + HUD_Ammo_bars->size_y*HUD_Ammo_bars->scale_y/2;
}

void hud_hide()
{
	reset(HUD_crosshair, SHOW);
	reset(HUD_HP_label, SHOW);
	reset(HUD_HP_bars, SHOW);
	//reset(HUD_Ammo_label, SHOW);
	reset(HUD_Ammo_bars, SHOW);
	reset(HUD_Ammo_infotext, SHOW);
	
	int i;
	for(i = 0; i<4; ++i)
		reset(hud_weapon_icon[i], SHOW);
}

void hud_show_weapon_icon(int id)
{
	int i;
	for(i = 0; i<4; ++i)
		reset(hud_weapon_icon[i], SHOW);
	if(id >= 0)
		set(hud_weapon_icon[id], SHOW);
}

var hud_flicker_coefficient(var frequency)
{
	return (1.+cos(total_ticks*frequency)) /2;
}

void hud_show_ammobar()
{
	set(HUD_Ammo_bars, SHOW);
	set(HUD_Ammo_infotext, SHOW);
}

void hud_hide_ammobar()
{
	reset(HUD_Ammo_bars, SHOW);
	reset(HUD_Ammo_infotext, SHOW);
}

void hud_update()
{
	var player_maxhealth = playerGetHealth();
	var player_health = playerGetMaxHealth();
	//
   hud_update_bar(HUD_HP_bars, hud_healthbar_bmap, player_health, player_maxhealth);
   
   var player_healthratio = player_health/player_maxhealth;
   if(player_healthratio < 0.3)
   {
	   var hp_flicker_frequency = 50/(1.+2*player_healthratio);
	   
	   var hp_flicker_brightness = 60;
	   
		HUD_HP_bars.red = (255-2*hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency)+ 2*hp_flicker_brightness;
		HUD_HP_bars.green = (255-hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency) + hp_flicker_brightness;
		HUD_HP_bars.blue = (255-hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency) + hp_flicker_brightness;
	}
	
	
	int weaponID = weapons_get_current();
	
	hud_show_weapon_icon(weaponID-1);
	if(weaponID > 0)
	{
		int ammo = weapons_get_ammo();
		int max_ammo = weapons_get_max_ammo();
		
		if(max_ammo != 0)
		{
			hud_show_ammobar();
			hud_update_bar(HUD_Ammo_bars, hud_ammobar_bmap, ammo, max_ammo);
			
			STRING *infostring = str_printf(NULL, "%d / %d", ammo, max_ammo);
			str_cpy((HUD_Ammo_infotext->pstring)[0], infostring);
			
			if(ammo == 0)
				HUD_Ammo_bars.alpha = HUD_BARS_MAX_ALPHA * hud_flicker_coefficient(20);
			else
				HUD_Ammo_bars.alpha = HUD_BARS_MAX_ALPHA;
		}
		else
			hud_hide_ammobar();
		
	}
	else
		hud_hide_ammobar();
}