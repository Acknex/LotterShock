#include "mainmenu.h"
#include "input.h"
#include "global.h"
#include "framework.h"
#include "ui.h"
#include "options.h"
#include "settings.h"
#include <acknex.h>

#define MAINMENU_ITEM_COUNT 5

#define MAINMENU_BORDER_PADDING 16
#define MAINMENU_FADE_SPEED 25

SOUND * mainmenu_stupid = "aiaiaiai.ogg";

BMAP * mainmenu_start_bmap = "mainmenu_start.png";
BMAP * mainmenu_credits_bmap = "mainmenu_credits.png";
BMAP * mainmenu_bestiary_bmap = "mainmenu_bestiary.png";
BMAP * mainmenu_options_bmap = "mainmenu_options.png";
BMAP * mainmenu_exit_bmap = "mainmenu_exit.png";

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

PANEL * mainmenu_bestiary_pan =
{
    bmap = mainmenu_bestiary_bmap;
    flags = TRANSLUCENT;
    layer = 2;
}

PANEL * mainmenu_options_pan =
{
    bmap = mainmenu_options_bmap;
    flags = TRANSLUCENT;
    layer = 2;
}

PANEL * mainmenu_exit_pan =
{
    bmap = mainmenu_exit_bmap;
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

var mainmenu_cameradist;

bool mainmenu_shows_optionsmenu;

int mainmenu_get_response()
{
    return mainmenu_response;
}

void mainmenu_resize()
{
    int i;
    for(i = 0; i < MAINMENU_ITEM_COUNT; i++)
    {
        mainmenu_items[i]->pos_x = screen_size.x - 192 - MAINMENU_BORDER_PADDING;
        mainmenu_items[i]->pos_y = screen_size.y - MAINMENU_BORDER_PADDING - (MAINMENU_ITEM_COUNT - i) * (64 + MAINMENU_BORDER_PADDING);
    }

    mainmenue_title->pos_x = (screen_size.x-mainmenue_title.size_x) /2;
    mainmenue_title->pos_y = 40;
}

void mainmenu_init()
{
    mainmenu_items[0] = mainmenu_start_pan;
    mainmenu_items[1] = mainmenu_credits_pan;
    mainmenu_items[2] = mainmenu_bestiary_pan;
    mainmenu_items[3] = mainmenu_options_pan;
    mainmenu_items[4] = mainmenu_exit_pan;

    settings_register_signal(mainmenu_resize);
}

void mainmenu_open()
{
    mainmenu_resize();
    set(mainmenu_selection_pan, SHOW);
    mainmenu_selection_pan->alpha = 0;
    mainmenu_selection = 0;
    mainmenu_response = MAINMENU_RESPONSE_STILLACTIVE;

    set(mainmenue_title, SHOW);


    int i;
    for(i = 0; i < MAINMENU_ITEM_COUNT; i++)
    {
        set(mainmenu_items[i], SHOW);
        mainmenu_items[i]->alpha = 0;
    }

    fog_color = 2;
    camera.fog_end = 20000.0;
    level_load("menue.wmb");
    mainmenu_cameradist = vec_dist(nullvector, camera.x);
}

void mainmenu_update()
{
    int i;

    if(mainmenu_shows_optionsmenu)
    {
        options_update();
        if(options_wants_close())
        {
            options_close();
            mainmenu_shows_optionsmenu = false;
        }
        return;
    }

    if(input_hit(INPUT_DOWN) && mainmenu_selection < (MAINMENU_ITEM_COUNT-1))
    {
        mainmenu_selection += 1;
        snd_play(ui_swap_snd, 100, 0);
    }
    if(input_hit(INPUT_UP) && mainmenu_selection > 0)
    {
        mainmenu_selection -= 1;
        snd_play(ui_swap_snd, 100, 0);
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
                snd_play(ui_swap_snd, 100, 0);
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
        if((mouse_panel == mainmenue_title) && (mouse_left))
        {
            snd_play(mainmenu_stupid, 100, 0);
        }
        else
        {
            snd_play(ui_accept_snd, 100, 0);
            switch(mainmenu_selection)
            {
            case 0: mainmenu_response = MAINMENU_RESPONSE_START; break;
            case 1: mainmenu_response = MAINMENU_RESPONSE_CREDITS; break;
            case 2: mainmenu_response = MAINMENU_RESPONSE_BESTIARY; break;
            case 3:
                mainmenu_shows_optionsmenu = true;
                options_open();
                return;
            case 4: mainmenu_response = MAINMENU_RESPONSE_EXIT; break;
            }
        }
    }

    // rotate the camera a bit
    camera->x = sin(total_ticks) * mainmenu_cameradist;
    camera->y = cos(total_ticks) * mainmenu_cameradist;

    VECTOR temp;
    vec_set(temp, camera->x);
    vec_scale(temp, -1);
    vec_to_angle(camera->pan, temp);

    SUBSYSTEM_LOOP(you, SUBSYSTEM_MAINMENU)
    {
        you->pan -= you->skill[0] * time_step;
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


// skill1: Rotatespeed
action MainmenuRotator()
{
    framework_setup(me, SUBSYSTEM_MAINMENU);

    my->alpha = 100;
    if(my->skill1 == 0)
        my->skill1 = 10;
}

action MainmenuLightflare()
{
   my->alpha = 100;
   my->flags |= (BRIGHT|TRANSLUCENT);
   my.emask &= ~DYNAMIC;
}
/*

+//Henrik,s bl<F6>der code ^M
+action act_TFL1()^M
+^M
+{^M
+^M
+^M
+^M
+       {^M
+^M
+//      set(my, FLAG1);^M
+   }^M
+       if(my->string1 == NULL)^M
+       {^M
+       error("an animationobject has no animation set");^M
+       return;^M
+^M
+       }^M
+^M
+       if(str_cmpi(my->string1, ""))^M
+^M
+       {^M
+       error("an animationobject has no animation set");^M
+       return;^M
+^M
+       }^M
+^M
+       ^M
+^M
+       if(my->skill[0] == 0)^M
+^M
+       my->skill[0] = 1;^M
+^M
+       if(my->skill[1] == 0)^M
+^M
+       my->skill[1] = 10;^M
+^M
+       while(1)^M
+^M
+       {^M
+^M
+               my->skill[99] = (my->skill[99] +my->skill[0]*time_step) %100;^M
+       ent_animate(my, my->string1, my->skill[99], ANM_CYCLE);^M
+               my->pan -= my->skill[0.5]*time_step;^M
+               wait(1);^M
+^M
+       }^M
+^M
+}^M
+^M
+^M

+^M
+action act_animation()^M
+{^M
+       if(my->string1 == NULL)^M
+       {^M
+               error("an animationobject has no animation set");^M
+               return;^M
+       }^M
+^M
+       if(str_cmpi(my->string1, ""))^M
+       {^M
+               error("an animationobject has no animation set");^M
+               return;^M
+       }^M
+       if(my->skill[0] == 0)^M
+               my->skill[0] = 1;^M
+^M
+       while(1)^M
+       {^M
+               my->skill[1] = (my->skill[1] +my->skill[0]*time_step) %100;^M
+               ent_animate(my, my->string1, my->skill[1], ANM_CYCLE);^M
+               wait(1);^M
+       }^M
+}^M

*/
