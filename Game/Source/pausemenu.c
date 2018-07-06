#include "pausemenu.h"
#include "input.h"
#include "global.h"
#include "framework.h"
#include "ui.h"

#include <acknex.h>

#define PAUSEMENU_PAN_COUNT 3

BMAP * pausemenu_bmp_continue   = "pause_continue.png";
BMAP * pausemenu_bmp_options    = "pause_options.png";
BMAP * pausemenu_bmp_quit       = "pause_quit.png";
BMAP * pausemenu_bmp_background = "pause_background.png";

PANEL * pausemenu_pan_background =
{
    bmap = pausemenu_bmp_background;
    layer = 100;
}

PANEL * pausemenu_pan_continue =
{
    bmap = pausemenu_bmp_continue;
    layer = 101;
    red = 208;
    green = 61;
    blue = 34;
}

PANEL * pausemenu_pan_options =
{
    bmap = pausemenu_bmp_options;
    layer = 101;
    red = 208;
    green = 61;
    blue = 34;
}

PANEL * pausemenu_pan_quit =
{
    bmap = pausemenu_bmp_quit;
    layer = 101;
    red = 208;
    green = 61;
    blue = 34;
}

int pausemenu_response;

int pausemenu_selection;

int pausemenu_get_response()
{
    return pausemenu_response;
}

PANEL * pausemenu_panels[PAUSEMENU_PAN_COUNT];

void pausemenu_init()
{
    pausemenu_panels[0] = pausemenu_pan_continue;
    pausemenu_panels[1] = pausemenu_pan_options;
    pausemenu_panels[2] = pausemenu_pan_quit;
}

void pausemenu_open()
{
    int i;
    set(pausemenu_pan_background, SHOW);
    for(i = 0; i < PAUSEMENU_PAN_COUNT; i++)
    {
        set(pausemenu_panels[i], SHOW);
        pausemenu_panels[i]->pos_x = -bmap_width(pausemenu_panels[i]->bmap);
        pausemenu_panels[i]->pos_y = 16 + 50 * i;
    }
    pausemenu_response = 0;
    pausemenu_selection = 0;
}

void pausemenu_update()
{
    int i;
    int prevsel = pausemenu_selection;

    pausemenu_pan_background->scale_x = screen_size.x / bmap_width(pausemenu_pan_background->bmap);
    pausemenu_pan_background->scale_y = screen_size.y / bmap_height(pausemenu_pan_background->bmap);

    if(input_hit(INPUT_NAVBACK))
    {
        snd_play(ui_accept_snd, 100, 0);
        pausemenu_response = PAUSEMENU_RESPONSE_CONTINUE;
        return;
    }

    if(input_hit(INPUT_DOWN) && pausemenu_selection < (PAUSEMENU_PAN_COUNT-1))
    {
        pausemenu_selection += 1;
    }
    if(input_hit(INPUT_UP) && pausemenu_selection > 0)
    {
        pausemenu_selection -= 1;
    }

    for(i = 0; i < PAUSEMENU_PAN_COUNT; i++)
    {
        pausemenu_panels[i]->pos_x = minv(pausemenu_panels[i]->pos_x + 50 * time_step, 0);

        if(pausemenu_panels[i] == mouse_panel && mouse_moving)
            pausemenu_selection = i;

        if(i == pausemenu_selection)
            set(pausemenu_panels[i], LIGHT);
        else
            reset(pausemenu_panels[i], LIGHT);
    }

    if(pausemenu_selection != prevsel)
    {
        snd_play(ui_swap_snd, 100, 0);
    }

    var attack = input_hit(INPUT_ATTACK) && (!mouse_left || (mouse_left && (mouse_panel != NULL)));
    if(input_hit(INPUT_USE) || attack || input_hit(INPUT_JUMP))
    {
        snd_play(ui_accept_snd, 100, 0);
        switch(pausemenu_selection)
        {
        case 0: pausemenu_response = PAUSEMENU_RESPONSE_CONTINUE; break;
        case 1: pausemenu_response = PAUSEMENU_RESPONSE_OPTIONS; break;
        case 2: pausemenu_response = PAUSEMENU_RESPONSE_QUIT; break;
        error("pausemenu_update: invalid menu selection!");
        }
    }
}

void pausemenu_close()
{
    int i;
    reset(pausemenu_pan_background, SHOW);
    for(i = 0; i < PAUSEMENU_PAN_COUNT; i++)
    {
        reset(pausemenu_panels[i], SHOW);
    }
}
