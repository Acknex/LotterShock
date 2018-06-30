

#define HUD_ELEMENT_COUNT 1

#define HUD_BORDER_PADDING 30
#define HUD_Z_LAYER 2

#define HUD_BARS_XPADDING 3

BMAP * hud_health_label = "health_label.png";
BMAP * hud_healthbar_background = "healthbar_background.png";
BMAP * hud_healthbar = "healthbar.png";
PANEL* HUD_HP_label =
{
	bmap = hud_health_label;
	flags = TRANSLUCENT;
	layer = 2;
}
PANEL* HUD_HP_bars =
{
	bmap = hud_healthbar_background;
	flags = TRANSLUCENT;
	layer = 2;
}

PANEL *HUD_elements[HUD_ELEMENT_COUNT];


void hud_init()
{
	HUD_elements [0] = HUD_HP_label;
	HUD_elements [1] = HUD_HP_bars;
}

void hud_show()
{
    HUD_elements [0]->pos_x = HUD_BORDER_PADDING;
    HUD_elements [0]->pos_y = screen_size.y - HUD_elements[0].size_y - HUD_BORDER_PADDING;
    
    HUD_elements [1]->pos_x = HUD_BORDER_PADDING + HUD_elements[0].size_x +3;
    HUD_elements [1]->pos_y = screen_size.y - HUD_elements[0].size_y - HUD_BORDER_PADDING;
    
    pan_setwindow  (HUD_elements [1], 0, 0,0, 0,bmap_height(hud_healthbar), hud_healthbar, 0,0);
    
    set(HUD_elements [0], SHOW);
    set(HUD_elements [1], SHOW);
}

void hud_update()
{
	//TODO!
	var PLAYER_MAXHEALTH = 50;
	var PLAYER_HEALTH = 30;
	//
	
	var HUD_HEALTH_SIZE = bmap_width(hud_healthbar_background)-HUD_BARS_XPADDING*2;
	HUD_HEALTH_SIZE *= PLAYER_HEALTH / PLAYER_MAXHEALTH;
	
	var HUD_BARS_YPADDING = (bmap_height(hud_healthbar_background)-bmap_height(hud_healthbar))/2;
	
	pan_setwindow  (	HUD_elements [1], 1, 
							HUD_BARS_XPADDING,HUD_BARS_YPADDING, 
							HUD_HEALTH_SIZE ,bmap_height(hud_healthbar), 
							hud_healthbar, 
							0,0);
    
}