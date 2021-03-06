#include "weapons.h"
#include "movement.h"
#include "global.h"
#include "environmentals.h"
#include "doors.h"

#define HUD_ELEMENT_COUNT 1

#define HUD_BORDER_PADDING 6
#define HUD_Z_LAYER 2

#define HUD_BARS_XPADDING 2
#define HUD_DIST_BETWEEN_BARS 0

#define HUD_BARS_MAX_ALPHA 50

#define HUD_AMMO_COLOR vector(32,200,255)

BMAP * hud_bar_background_bmap = "hud_bar_background.png";
//BMAP * hud_bar_background_bmap = "gunbg.png";
BMAP * hud_health_label_bmap = "health_label.png";

BMAP * hud_ammobar_bmap = "ammobar.png";

FONT *HUD_font = "HUD_font.png";
BMAP *HUD_font_bmap = "HUD_font.png";
BMAP *HUD_fadenkreuz_def_bmap = "fadenkreuz.png";
BMAP *HUD_fadenkreuz_act_bmap = "fadenkreuz_green.png";

#define PRAGMA_BIND "label_sword.png"
#define PRAGMA_BIND "label_shotgun.png"
#define PRAGMA_BIND "label_cellgun.png"
#define PRAGMA_BIND "label_flamethrower.png"
#define PRAGMA_BIND "icon_keycard4.png"
#define PRAGMA_BIND "icon_keycard2.png"
#define PRAGMA_BIND "icon_keycard.png"
#define PRAGMA_BIND "icon_keycard3.png"
#define PRAGMA_BIND "icon_keycard1.png"


PANEL *HUD_background =
{
	bmap = "interfache_bg.png";
	flags = TRANSLUCENT;
	layer = 2;
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
	layer = 0;
}

TEXT* HUD_Ammo_infotext =	
{
	layer = 5;
	font = "Console#20b";
    blue = 255;
    green = 255;
    red = 255;
	string ("asd");
    flags = CENTER_X | CENTER_Y | LIGHT;
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

BMAP *hud_lotterhead_bmap = "emohealthsmall.png";
var hud_lotterhead_shift = 0;
PANEL* HUD_Head =
{
	//bmap = "lotterindahouse.png";
	window(0,0, 0,0, hud_lotterhead_bmap, NULL, NULL);
	flags = TRANSLUCENT | LIGHT;
	layer = 3;
	scale_x = 1.1;
	scale_y = 1.1;
}

PANEL *hud_weapon_icon[4];
PANEL *hud_keycard_icon[KEYS_MAX];

void hud_init()
{
	hud_weapon_icon[WEAPON_SWORD-1] = pan_create("bmap = label_sword.png;",2);
	hud_weapon_icon[WEAPON_SHOTGUN-1] = pan_create("bmap = label_shotgun.png",2);
	hud_weapon_icon[WEAPON_CELLGUN-1] = pan_create("bmap = label_cellgun.png",2);
	hud_weapon_icon[WEAPON_FLAMETHROWER-1] = pan_create("bmap = label_flamethrower.png",2);
	
   //KEY 0 is unused -> start with 1
	hud_keycard_icon[BLACK_KEY] = pan_create("bmap = icon_keycard4.png",2);
	hud_keycard_icon[GREEN_KEY] = pan_create("bmap = icon_keycard2.png",2);
	hud_keycard_icon[RED_KEY] = pan_create("bmap = icon_keycard.png",2);
	hud_keycard_icon[YELLOW_KEY] = pan_create("bmap = icon_keycard3.png",2);
	hud_keycard_icon[BLUE_KEY] = pan_create("bmap = icon_keycard1.png",2);
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
}

void hud_update_bar(PANEL *bar, BMAP *source, var current_value, var max_value)
{
	var bar_size = bar->size_x-HUD_BARS_XPADDING*2;
	bar_size *= current_value / max_value;
	
	var HUD_BARS_YPADDING = (bar->size_y - bmap_height(source))/2;
	
	pan_setwindow  (	bar, 1, 
							HUD_BARS_XPADDING,HUD_BARS_YPADDING, 
							bar_size ,bmap_height(source)*bar->scale_y, 
							source, 
							0,0);
}

void hud_applyLayout()
{
	pan_setwindow(HUD_Head,1, 0,0,bmap_width(hud_lotterhead_bmap)/4, bmap_height(hud_lotterhead_bmap),hud_lotterhead_bmap, hud_lotterhead_shift, NULL);
	HUD_Head->pos_x = (screen_size.x- bmap_width(hud_lotterhead_bmap)*HUD_Head.scale_x/4)/2;
	HUD_Head->pos_y = screen_size.y - bmap_height(hud_lotterhead_bmap)*HUD_Head.scale_y - HUD_BORDER_PADDING;
	set(HUD_Head, SHOW);
	
	
	HUD_background->pos_x = (screen_size.x-hud_sizex(HUD_background))/2;
	HUD_background->pos_y = screen_size.y-hud_sizey(HUD_background);
	set(HUD_background, SHOW);
	
	
	HUD_crosshair->pos_x = (screen_size.x - hud_sizex(HUD_crosshair)) /2;
	HUD_crosshair->pos_y = (screen_size.y - hud_sizey(HUD_crosshair)) /2;
	HUD_crosshair->alpha = 10;
	set(HUD_crosshair, SHOW);
	
	var desired_height = 38;
	var desired_width = 78;
	
	var pos_center = screen_size.x/2; 
	var pos_right = 38 +38;
	var pos_rright = 126 +38;
	
	var pos_left = pos_center-pos_right;
	var pos_lleft = pos_center-pos_rright;
	
	pos_right = pos_center+pos_right;
	pos_rright = pos_center+pos_rright;
	
	int i;
	for(i=0; i<4; ++i)
	{
		hud_weapon_icon[i]->scale_x = desired_width/hud_weapon_icon[i]->size_x;
		hud_weapon_icon[i]->scale_y = desired_height/hud_weapon_icon[i]->size_y;
		
	 	hud_weapon_icon[i]->pos_x = pos_lleft - hud_sizex(hud_weapon_icon[i])/2;
	 	hud_weapon_icon[i]->pos_y = screen_size.y - hud_sizey(hud_weapon_icon[i])-HUD_BORDER_PADDING;
	}
	HUD_Ammo_bars->scale_x = desired_width/HUD_Ammo_bars->size_x;
	HUD_Ammo_bars->scale_y = desired_height/HUD_Ammo_bars->size_y;
	
	hud_place_bar(HUD_Ammo_bars, pos_left - hud_sizex(HUD_Ammo_bars)/2);
	
	HUD_Ammo_infotext->pos_x = HUD_Ammo_bars->pos_x + hud_sizex(HUD_Ammo_bars)/2;
	HUD_Ammo_infotext->pos_y = HUD_Ammo_bars->pos_y + hud_sizey(HUD_Ammo_bars)/2;
	
	
	HUD_HP_text->pos_x = pos_rright-hud_sizex(HUD_HP_text)/2;
	HUD_HP_text->pos_y = screen_size.y - hud_sizey(HUD_HP_text)-HUD_BORDER_PADDING;
	
	
	HUD_HP_infotext->pos_x = pos_right;
	HUD_HP_infotext->pos_y = screen_size.y - hud_sizey(HUD_HP_text)/2-HUD_BORDER_PADDING;
	
   //KEY 0 is unused -> start with 1
	for(i = 0; i<KEYS_MAX ;++i)
	{
		hud_keycard_icon[i]->pos_x = HUD_BORDER_PADDING;
		hud_keycard_icon[i]->pos_y = screen_size.y - hud_sizey(hud_keycard_icon[i])*(i+1) - HUD_BORDER_PADDING;
	}
}

void hud_show()
{
	hud_applyLayout();
	
	set(HUD_Head, SHOW);
	set(HUD_background, SHOW);
	
	set(HUD_Ammo_bars, SHOW);
	set(HUD_HP_text, SHOW);
	set(HUD_HP_infotext, SHOW);
	
	set(HUD_crosshair, SHOW);
	
   settings_register_signal(hud_applyLayout);
}

void hud_hide()
{
	reset(HUD_Head, SHOW);
	reset(HUD_HP_infotext, SHOW);
	reset(HUD_HP_text, SHOW);
	reset(HUD_crosshair, SHOW);
	reset(HUD_HP_label, SHOW);
	reset(HUD_Ammo_bars, SHOW);
	reset(HUD_Ammo_infotext, SHOW);
	reset(HUD_background, SHOW);
	reset(HUD_gunframe, SHOW);
	
	int i;
	for(i = 0; i<4; ++i)
		reset(hud_weapon_icon[i], SHOW);
	for(i = 0; i<KEYS_MAX; ++i)
		reset(hud_keycard_icon[i], SHOW);
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
    bool crosshairTargetsMob = false;
    if(mouse_ent != NULL)
    {
        highlightCrosshair = mouse_ent.INTERACTIBLE;
        if (mouse_ent.SK_SUBSYSTEM >= 1000)
           crosshairTargetsMob = true;
           
#ifdef DEBUG
        VECTOR * color = COLOR_GREEN;
        if((mouse_ent->emask & DYNAMIC) == 0)
            color = COLOR_RED;
        draw_text(
            str_printf(NULL, "[%s]", mouse_ent.type),
            screen_size.x / 2 + 10,
            screen_size.y / 2 + 10,
            color);
#endif
    }

    if(highlightCrosshair)
    {
        HUD_crosshair.bmap = HUD_fadenkreuz_act_bmap;
        HUD_crosshair->alpha = 40;
        //set(HUD_crosshair, LIGHT);
    }    
    else
    {
        //reset(HUD_crosshair, LIGHT);
        HUD_crosshair.bmap = HUD_fadenkreuz_def_bmap;
        HUD_crosshair->alpha = 10;
    }    
   
   
   //KEY 0 is unused -> start with 1
	int i;
	for(i=0; i<KEYS_MAX ; ++i)
		if(keys[i])
			set(hud_keycard_icon[i], SHOW);

	var player_maxhealth = playerGetMaxHealth();
	var player_health = playerGetHealth();
	
	STRING *infostring = str_printf(NULL, "%d", (int)floor(player_health+0.999));
	str_cpy((HUD_HP_infotext->pstring)[0], infostring);
	//
	//hud_update_bar(HUD_HP_bars, hud_healthbar_bmap, player_health, player_maxhealth);
	
	var player_healthratio = player_health/player_maxhealth;
	if(player_healthratio < 0.5)
	{
		var hp_flicker_frequency = 50/(1.+2*player_healthratio);
	   
		var hp_flicker_brightness = 60*(player_healthratio);
		
		/*
		var minalpha = 50*player_healthratio;
		var varalpha = 50-minalpha;
		HUD_HP_infotext.alpha = minalpha + varalpha*(1-hud_flicker_coefficient(hp_flicker_frequency));
		HUD_HP_text.alpha = HUD_HP_infotext.alpha;
		*/
		
		//HUD_HP_infotext.red = (127-2*hp_flicker_brightness)*hud_flicker_coefficient(hp_flicker_frequency)+ 2*hp_flicker_brightness;
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
	
	int mode = player_healthratio*4;
	hud_lotterhead_shift = (bmap_width(hud_lotterhead_bmap)/4)*mode;
	
	
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
