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

uisystem_t * mainmenu_ui;
uibutton_t * mainmenu_items[MAINMENU_ITEM_COUNT];


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


void mainmenu_start_game() { mainmenu_response = MAINMENU_RESPONSE_START; }
void mainmenu_start_credits() { mainmenu_response = MAINMENU_RESPONSE_CREDITS; }
void mainmenu_start_bestiary() { mainmenu_response = MAINMENU_RESPONSE_BESTIARY; }
void mainmenu_show_options() {
    mainmenu_shows_optionsmenu = true;
    options_open();
}
void mainmenu_quit_game() { mainmenu_response = MAINMENU_RESPONSE_EXIT; }

void mainmenu_init()
{
    mainmenu_ui = uisystem_new(2);
    set(mainmenu_ui, UISYSTEM_SELECTOR);
    mainmenu_ui->selector->bmap = mainmenu_selection_bmap;

    mainmenu_items[0] = uisystem_add_button(mainmenu_ui, 0, 0, mainmenu_start_bmap, mainmenu_start_game);
    mainmenu_items[1] = uisystem_add_button(mainmenu_ui, 0, 0, mainmenu_credits_bmap, mainmenu_start_credits);
    mainmenu_items[2] = uisystem_add_button(mainmenu_ui, 0, 0, mainmenu_bestiary_bmap, mainmenu_start_bestiary);
    mainmenu_items[3] = uisystem_add_button(mainmenu_ui, 0, 0, mainmenu_options_bmap, mainmenu_show_options);
    mainmenu_items[4] = uisystem_add_button(mainmenu_ui, 0, 0, mainmenu_exit_bmap, mainmenu_quit_game);

    int i;
    for(i = 0; i < MAINMENU_ITEM_COUNT; i++)
    {
        if(i > 0)
            mainmenu_items[i]->neighbour[UIDIR_UP] = mainmenu_items[i - 1];
        if(i < (MAINMENU_ITEM_COUNT-1))
            mainmenu_items[i]->neighbour[UIDIR_DOWN] = mainmenu_items[i + 1];
    }

    settings_register_signal(mainmenu_resize);
}

void mainmenu_open()
{
    mainmenu_resize();
    mainmenu_response = MAINMENU_RESPONSE_STILLACTIVE;

    set(mainmenue_title, SHOW);
    uisystem_show_all(mainmenu_ui);

    fog_color = 2;
    camera.fog_end = 20000.0;
    level_load("menue.wmb");
    mainmenu_cameradist = vec_dist(nullvector, camera.x);

    music_start("Media/intro.mp3", 1.0, false);
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

    uisystem_update(mainmenu_ui);

    var attack = input_hit(INPUT_ATTACK) && (!mouse_left || (mouse_left && (mouse_panel != NULL)));
    if(input_hit(INPUT_USE) || attack || input_hit(INPUT_JUMP))
    {
        if((mouse_panel == mainmenue_title) && (mouse_left))
        {
            snd_play(mainmenu_stupid, 100, 0);
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
    uisystem_hide_all(mainmenu_ui);
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
