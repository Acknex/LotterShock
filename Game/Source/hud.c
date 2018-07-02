#include "weapons.h"
#include "movement.h"
#include "global.h"
#include "environmentals.h"

#define HUD_ELEMENT_COUNT 1

#define HUD_BORDER_PADDING 30
#define HUD_Z_LAYER 2

#define HUD_BARS_XPADDING 3
#define HUD_DIST_BETWEEN_BARS 0

#define HUD_BARS_MAX_ALPHA 50

#define HUD_AMMO_COLOR vector(32,200,255)

BMAP * hud_bar_background_bmap = "hud_bar_background.png";
BMAP * hud_health_label_bmap = "health_label.png";

BMAP * hud_ammobar_bmap = "ammobar.png";

FONT *HUD_font = "HUD_font.png";
BMAP *HUD_font_bmap = "HUD_font.png";

PANEL *HUD_background =
{
	bmap = "interfache_bg.png";
	flags = TRANSLUCENT;
	layer = 1;
}

PANEL *HUD_gunframe =
{
	bmap = "gunbg.png";
	flags = TRANSLUCENT;
	layer = 2;
}

PANEL* HUD_crosshair =
{
	bmap = "fadenkreuz.png";
    flags = TRANSLUCENT;
	layer = 2;
	scale_x = 0.5;
	scale_y = 0.5;
    red = 0;
    green = 255;
    blue = 0;
}

PANEL* HUD_HP_label =
{
	bmap = hud_health_label_bmap;
	flags = TRANSLUCENT;
	layer = 3;
}
PANEL* HUD_Ammo_bars =
{
	bmap = hud_bar_background_bmap;
	flags = TRANSLUCENT;
	layer = 4;
}

TEXT* HUD_Ammo_infotext =	
{
	layer = 5;
	font = "Console#20b";
	blue = 0;
	green = 0;
	red = 0;
	string ("asd");
	flags = CENTER_X | CENTER_Y ;
} 

PANEL* HUD_HP_text =
{
	bmap = "HP.png";
	blue = 255;
	green = 255;
	red = 255;
	flags = TRANSLUCENT | LIGHT;
	layer = 3;
}
TEXT* HUD_HP_infotext =	
{
	layer = 4;
	font = HUD_font;//"Console#50b";
	blue = 255;
	green = 255;
	red = 255;
	
	string ("100");
	flags = CENTER_X | CENTER_Y | LIGHT;
} 

PANEL* HUD_Head =
{
	bmap = "lotterindahouse.png";
	flags = TRANSLUCENT | LIGHT;
	layer = 3;
}

PANEL *hud_weapon_icon[4];

void hud_init()
{
	hud_weapon_icon[WEAPON_SWORD-1] = pan_create("bmap = label_sword.png;",2);
	hud_weapon_icon[WEAPON_SHOTGUN-1] = pan_create("bmap = label_shotgun.png",2);
	hud_weapon_icon[WEAPON_CELLGUN-1] = pan_create("bmap = label_cellgun.png",2);
	hud_weapon_icon[WEAPON_FLAMETHROWER-1] = pan_create("bmap = label_flamethrower.png",2);
}


var hud_sizex(PANEL *p)
{
	return p->size_x*p->scale_x;
}
 
var hud_sizey(PANEL *p)
{
	return p->size_y*p->scale_y;
}

void hud_place_bar(PANEL *bar, var offsetX) 
{
	bar->pos_x = offsetX;
	bar->pos_y = screen_size.y - bar->size_y*bar->scale_y - HUD_BORDER_PADDING;
	bar->alpha = HUD_BARS_MAX_ALPHA;
	
	pan_setwindow  (bar, 0, 0,0, 0, bmap_height(hud_ammobar_bmap), hud_ammobar_bmap, 0,0);
	
	set(bar, SHOW);
}

void hud_update_bar(PANEL *bar, BMAP *source, var current_value, var max_value)
{
	var bar_size = bar->size_x-HUD_BARS_XPADDING*2;
	bar_size *= current_value / max_value;
	
	var HUD_BARS_YPADDING = (bar->size_y - bmap_height(source))/2;
	
	pan_setwindow  (	bar, 1, 
							HUD_BARS_XPADDING,HUD_BARS_YPADDING, 
							bar_size ,bmap_height(source), 
							source, 
							0,0);
}


void hud_show()
{
	
	HUD_Head->pos_x = (screen_size.x- hud_sizex(HUD_Head))/2;
	HUD_Head->pos_y = screen_size.y - hud_sizey(HUD_Head) - HUD_BORDER_PADDING;
	//set(HUD_Head, SHOW);
	
	var SEGMENT_DISTANCE = 20;
	
	var space_left = HUD_Head->pos_x - HUD_BORDER_PADDING;
	space_left /= 2;
	
	var lb_leftElement = HUD_BORDER_PADDING;
	var lb_rightElement = HUD_BORDER_PADDING+space_left;
	
	var ub_leftElement = lb_rightElement-SEGMENT_DISTANCE;
	var ub_rightElement = HUD_Head->pos_x-SEGMENT_DISTANCE;
	
	var leftElementShift = -20;
	lb_leftElement += leftElementShift;
	ub_leftElement += leftElementShift;
	
	var center_leftElement = (ub_leftElement+lb_leftElement)/2;
	var center_rightElement = (ub_rightElement+lb_rightElement)/2;
	
	var cc_digitSize = (ub_leftElement-lb_leftElement)/5;
	var cc_left = lb_leftElement + 1.5*cc_digitSize;
	var cc_right = ub_leftElement - 0.8*cc_digitSize;
	
	var element_width = space_left - SEGMENT_DISTANCE;
	
	var height_right = hud_sizey(hud_weapon_icon[0]) + hud_sizey(HUD_Ammo_bars);
	var height_left = hud_sizey(HUD_HP_text);
	var height_total = maxv(height_right, height_left);
	
	var center_all = screen_size.y - height_total/2 - HUD_BORDER_PADDING;
	
	var desired_background_height = height_total+2*HUD_BORDER_PADDING;
	var scale_rightx = element_width / hud_weapon_icon[0].size_x;
	var scale_righty = (desired_background_height-2*HUD_BORDER_PADDING) / hud_weapon_icon[0].size_x;
	var scale_right = minv(scale_rightx, scale_righty);
	
	int i;
	for(i=0; i<4; ++i)
	{
		hud_weapon_icon[i]->scale_x = scale_right;
		hud_weapon_icon[i]->scale_y = scale_right;
		
	 	hud_weapon_icon[i]->pos_x = center_rightElement - hud_sizex(hud_weapon_icon[i])/2;
	 	hud_weapon_icon[i]->pos_y = center_all - height_right*scale_right/2;
	}
	
	HUD_Ammo_bars->scale_x = scale_right*1.5;
	HUD_Ammo_bars->scale_y = scale_right*1.3;
	
	hud_place_bar(HUD_Ammo_bars, center_rightElement - hud_sizex(HUD_Ammo_bars)/2);
	HUD_Ammo_bars->pos_y = center_all + height_right/2 - hud_sizey(HUD_Ammo_bars);
	
	
	HUD_gunframe->scale_x = scale_right;
	HUD_gunframe->scale_y = scale_right;
	
	HUD_gunframe->pos_x = center_rightElement - hud_sizex(HUD_gunframe)/2;
	HUD_gunframe->pos_y = center_all - hud_sizey(HUD_gunframe)/2 + 10;
	set(HUD_gunframe, SHOW);
	
	
	
	
	HUD_HP_text->pos_x = cc_right - hud_sizex(HUD_HP_text)/2;
	HUD_HP_text->pos_y = center_all - hud_sizey(HUD_HP_text)/2;
	set(HUD_HP_text, SHOW);
	
	
	HUD_HP_infotext->pos_x = cc_left;
	HUD_HP_infotext->pos_y = center_all;
	set(HUD_HP_infotext, SHOW);
	
	
	
	
	HUD_background->scale_y = desired_background_height/HUD_background->size_y;
	HUD_background->scale_x = HUD_background->scale_y;
	
	
	var background_ub = (screen_size.x+hud_sizex(HUD_Head))/2 +HUD_BORDER_PADDING;
	var background_overshoot = hud_sizex(HUD_background) - background_ub;
	
	HUD_background->pos_x = -background_overshoot;
	HUD_background->pos_y = screen_size.y-hud_sizey(HUD_background);
	set(HUD_background, SHOW);
	
	
	HUD_crosshair->pos_x = (screen_size.x - hud_sizex(HUD_crosshair)) /2;
	HUD_crosshair->pos_y = (screen_size.y - hud_sizey(HUD_crosshair)) /2;
	HUD_crosshair->alpha = 10;
	set(HUD_crosshair, SHOW);
	
	HUD_Ammo_infotext->pos_x = HUD_Ammo_bars->pos_x + hud_sizex(HUD_Ammo_bars)/2;
	HUD_Ammo_infotext->pos_y = HUD_Ammo_bars->pos_y + hud_sizey(HUD_Ammo_bars)/2;
}

void hud_hide()
{
	reset(HUD_Head, SHOW);
	reset(HUD_HP_infotext, SHOW);
	reset(HUD_HP_text, SHOW);
	reset(HUD_crosshair, SHOW);
	reset(HUD_HP_label, SHOW);
	//reset(HUD_HP_bars, SHOW);
	//reset(HUD_Ammo_label, SHOW);
	reset(HUD_Ammo_bars, SHOW);
	reset(HUD_Ammo_infotext, SHOW);
	reset(HUD_background, SHOW);
	reset(HUD_gunframe, SHOW);
	
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
    bool highlightCrosshair = false;
    if(mouse_ent != NULL)
    {
        if(mouse_ent.SK_SUBSYSTEM == SUBSYSTEM_DOORS)
            highlightCrosshair = true;
        if(mouse_ent.SK_SUBSYSTEM == SUBSYSTEM_ENVIRONMENT && mouse_ent.ENVIRONMENTALS_TYPE == ENVIRONMENTAL_ENGINE_TERMINAL)
            highlightCrosshair = true;

#ifdef DEBUG
        draw_text(
            str_printf(NULL, "[%s]", mouse_ent.type),
            screen_size.x / 2 + 10,
            screen_size.y / 2 + 10,
            COLOR_GREEN);
#endif
    }

    if(highlightCrosshair)
        set(HUD_crosshair, LIGHT);
    else
        reset(HUD_crosshair, LIGHT);

	var player_maxhealth = playerGetMaxHealth();
	var player_health = playerGetHealth();
	
	STRING *infostring = str_printf(NULL, "%d", (int)player_health);
	str_cpy((HUD_HP_infotext->pstring)[0], infostring);
	//
	//hud_update_bar(HUD_HP_bars, hud_healthbar_bmap, player_health, player_maxhealth);
	
	var player_healthratio = player_health/player_maxhealth;
	if(player_healthratio < 0.8)
	{
		var hp_flicker_frequency = 50/(1.+player_healthratio);
	   
		var hp_flicker_brightness = 30*(player_healthratio);
		
		var minalpha = 50*player_healthratio;
		var varalpha = 50-minalpha;
		HUD_HP_infotext.alpha = minalpha + varalpha*(1-hud_flicker_coefficient(hp_flicker_frequency));
		HUD_HP_text.alpha = HUD_HP_infotext.alpha;
		
		HUD_HP_infotext.red = (255-2*hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency)+ 2*hp_flicker_brightness;
		HUD_HP_infotext.green = (255-hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency) + hp_flicker_brightness;
		HUD_HP_infotext.blue = (255-hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency) + hp_flicker_brightness;
		
		vec_set(HUD_HP_text.blue, HUD_HP_infotext.blue);
	}
	else
	{
		HUD_HP_infotext.alpha = 50;
		HUD_HP_text.alpha = 50;
		
		vec_set(HUD_HP_infotext.blue, vector(255,255,255));
		vec_set(HUD_HP_text.blue, HUD_HP_infotext.blue);
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
