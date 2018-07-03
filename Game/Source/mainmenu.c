#include "mainmenu.h"
#include "input.h"

#define MAINMENU_ITEM_COUNT 4

#define MAINMENU_BORDER_PADDING 16
#define MAINMENU_FADE_SPEED 25


BMAP * mainmenu_start_bmap = "mainmenu_start.png";
BMAP * mainmenu_credits_bmap = "mainmenu_credits.png";
BMAP * mainmenu_exit_bmap = "mainmenu_exit.png";
BMAP * mainmenu_bestiary_bmap = "mainmenu_bestiary.png";

PANEL * mainmenu_start_pan =
{
    bmap = mainmenu_start_bmap;
    flags = TRANSLUCENT;
    layer = 2;
}

PANEL * mainmenu_credits_pan =
{
    bmap = mainmenu_credits_bmap;
    flags = TRANSLUCENT;
        layer = 2;
}

PANEL * mainmenu_exit_pan =
{
    bmap = mainmenu_exit_bmap;
    flags = TRANSLUCENT;
    layer = 2;
}

PANEL * mainmenu_bestiary_pan =
{
    bmap = mainmenu_bestiary_bmap;
    flags = TRANSLUCENT;
    layer = 2;
}

PANEL * mainmenu_selection_pan =
{
    bmap = mainmenu_selection_bmap;
    flags = TRANSLUCENT;
    layer = 3;
}

PANEL * mainmenu_items[MAINMENU_ITEM_COUNT];


PANEL* mainmenue_title =
{
	bmap = "Menuetitle.png";
	flags = TRANSLUCENT;
	layer = 2;
}

int mainmenu_selection;

int mainmenu_response;

int mainmenu_get_response()
{
    return mainmenu_response;
}

void mainmenu_init()
{
    mainmenu_items[0] = mainmenu_start_pan;
    mainmenu_items[1] = mainmenu_credits_pan;
    mainmenu_items[2] = mainmenu_bestiary_pan;
    mainmenu_items[3] = mainmenu_exit_pan;
}

void mainmenu_open()
{
    int i;
    for(i = 0; i < MAINMENU_ITEM_COUNT; i++)
    {
        set(mainmenu_items[i], SHOW);
        mainmenu_items[i]->alpha = 0;
        mainmenu_items[i]->pos_x = screen_size.x - 192 - MAINMENU_BORDER_PADDING;
        mainmenu_items[i]->pos_y = screen_size.y - MAINMENU_BORDER_PADDING - (MAINMENU_ITEM_COUNT - i) * (64 + MAINMENU_BORDER_PADDING);
    }
    set(mainmenu_selection_pan, SHOW);
    mainmenu_selection_pan->alpha = 0;
    mouse_mode = 4;
    mainmenu_selection = 0;
    mainmenu_response = MAINMENU_RESPONSE_STILLACTIVE;
    
    mainmenue_title->pos_x = (screen_size.x-mainmenue_title.size_x) /2;
´   mainmenue_title->pos_y = 40;
    set(mainmenue_title, SHOW);
    
	fog_color = 2;
	camera.fog_end = 20000.0;
    level_load("menue.wmb");
    wait_for(level_load);
    wait(3);
}

void mainmenu_update()
{	
    int i;

    if(input_hit(INPUT_DOWN) && mainmenu_selection < (MAINMENU_ITEM_COUNT-1))
    {
        mainmenu_selection += 1;
        snd_play(mainmenu_swap_snd, 100, 0);
    }
    if(input_hit(INPUT_UP) && mainmenu_selection > 0)
    {
        mainmenu_selection -= 1;
        snd_play(mainmenu_swap_snd, 100, 0);
    }

    for(i = 0; i < MAINMENU_ITEM_COUNT; i++)
    {
        mainmenu_items[i]->alpha = clamp(
            mainmenu_items[i]->alpha + MAINMENU_FADE_SPEED * time_step,
            0,
            100);

        if(mainmenu_items[i]->alpha < 70)
            break;

        if(mouse_panel == mainmenu_items[i])
        {
            if(mainmenu_selection != i)
                snd_play(mainmenu_swap_snd, 100, 0);
            mainmenu_selection = i;
        }
    }
    if(i == 3)
    {
        mainmenu_selection_pan->alpha = clamp(mainmenu_selection_pan->alpha + MAINMENU_FADE_SPEED * time_step, 0, 100);
    }
    mainmenu_selection_pan->pos_x = mainmenu_items[mainmenu_selection]->pos_x;
    mainmenu_selection_pan->pos_y = mainmenu_items[mainmenu_selection]->pos_y;

    var attack = input_hit(INPUT_ATTACK) && (!mouse_left || (mouse_left && (mouse_panel != NULL)));
    if(input_hit(INPUT_USE) || attack || input_hit(INPUT_JUMP))
    {
        snd_play(mainmenu_accept_snd, 100, 0);
        switch(mainmenu_selection)
        {
        case 0: mainmenu_response = MAINMENU_RESPONSE_START; break;
        case 1: mainmenu_response = MAINMENU_RESPONSE_CREDITS; break;
        case 2: mainmenu_response = MAINMENU_RESPONSE_BESTIARY; break;
        case 3: mainmenu_response = MAINMENU_RESPONSE_EXIT; break;
        }
    }
}

void mainmenu_close()
{
    int i;
    for(i = 0; i < MAINMENU_ITEM_COUNT; i++)
    {
        reset(mainmenu_items[i], SHOW);
    }
    reset(mainmenu_selection_pan, SHOW);
    reset(mainmenue_title, SHOW);
}

action menuelevel_anchor()
{
	var camera_distance = vec_dist(&my->x, camera.x);
	while(1)
	{
		if(str_cmp(level_name, "menue.wmb"))
		{
			camera->x = sin(total_ticks)*camera_distance;
			camera->y = cos(total_ticks)*camera_distance;
			
			VECTOR diff;
			vec_set(&diff, &my.x);
			vec_sub(&diff, camera->x);
			vec_to_angle(camera->pan, &diff);
		}
		wait(1);
	}
}

//Henrik,s blöder code 
action act_TFL1()

{



	{

//	 set(my, FLAG1);
   }
	if(my->string1 == NULL)
	{
	error("an animationobject has no animation set");
	return;

	}

	if(str_cmpi(my->string1, ""))

	{
	error("an animationobject has no animation set");
	return;

	}

	

	if(my->skill[0] == 0)

	my->skill[0] = 1;

	if(my->skill[1] == 0)

	my->skill[1] = 10;

	while(1)

	{

		my->skill[99] = (my->skill[99] +my->skill[0]*time_step) %100;
   	ent_animate(my, my->string1, my->skill[99], ANM_CYCLE);
		my->pan -= my->skill[0.5]*time_step;
		wait(1);

	}

}


action act_rotator()
{
	my->alpha = 100;
	if(my->skill[0] == 0)
		my->skill[0] = 10;

	while(1)
	{
		my->pan -= my->skill[0]*time_step;
		wait(1);
	}
}

action act_lightflares()
{
	my->alpha = 100;

	my->flags |= (BRIGHT|TRANSLUCENT);

   
	my.emask &= ~DYNAMIC;
}

action act_animation()
{
	if(my->string1 == NULL)
	{
		error("an animationobject has no animation set");
		return;
	}

	if(str_cmpi(my->string1, ""))
	{
		error("an animationobject has no animation set");
		return;
	}
	if(my->skill[0] == 0)
		my->skill[0] = 1;

	while(1)
	{
		my->skill[1] = (my->skill[1] +my->skill[0]*time_step) %100;
		ent_animate(my, my->string1, my->skill[1], ANM_CYCLE);
		wait(1);
	}
}
