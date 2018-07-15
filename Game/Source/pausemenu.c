#include "pausemenu.h"
#include "input.h"
#include "global.h"
#include "framework.h"
#include "ui.h"
#include "options.h"

#include <acknex.h>

#define PAUSEMENU_BUTTONS 3

BMAP * pausemenu_bmp_continue   = "pause_continue.png";
BMAP * pausemenu_bmp_options    = "pause_options.png";
BMAP * pausemenu_bmp_quit       = "pause_quit.png";

PANEL * pausemenu_pan_background =
{
    bmap = ui_bmp_background;
    layer = 100;
}


uisystem_t * pausemenu_ui;
uibutton_t * pausemenu_buttons[PAUSEMENU_BUTTONS];

int pausemenu_response;
bool pausemenu_shows_options;

int pausemenu_get_response()
{
    return pausemenu_response;
}

void pausemenu_reset_response()
{
    pausemenu_response = PAUSEMENU_RESPONSE_NONE;
}

void pausemenu_rsp_continue()
{
    pausemenu_response = PAUSEMENU_RESPONSE_CONTINUE;
}

void pausemenu_rsp_options()
{
    pausemenu_shows_options = true;
    options_open();
}

void pausemenu_rsp_quit()
{
    pausemenu_response = PAUSEMENU_RESPONSE_QUIT;
}

void pausemenu_init()
{
    pausemenu_ui = uisystem_new(101);
    pausemenu_buttons[0] = uisystem_add_button(pausemenu_ui, 0, 0, pausemenu_bmp_continue, pausemenu_rsp_continue);
    pausemenu_buttons[1] = uisystem_add_button(pausemenu_ui, 0, 0, pausemenu_bmp_options, pausemenu_rsp_options);
    pausemenu_buttons[2] = uisystem_add_button(pausemenu_ui, 0, 0, pausemenu_bmp_quit, pausemenu_rsp_quit);

    int i;
    for(i = 0; i < PAUSEMENU_BUTTONS; i++)
    {
        if(i > 0)
            pausemenu_buttons[i]->neighbour[UIDIR_UP] = pausemenu_buttons[i - 1];
        if(i < (PAUSEMENU_BUTTONS-1))
            pausemenu_buttons[i]->neighbour[UIDIR_DOWN] = pausemenu_buttons[i + 1];
    }

}

void pausemenu_open()
{
    int i;
    set(pausemenu_pan_background, SHOW);
    uisystem_show_all(pausemenu_ui);
    for(i = 0; i < PAUSEMENU_BUTTONS; i++)
    {
        pausemenu_buttons[i]->pos_x = -bmap_width(pausemenu_buttons[i]->bmap);
        pausemenu_buttons[i]->pos_y = 16 + 50 * i;
    }
    pausemenu_reset_response();
    pausemenu_shows_options = false;
    freeze_mode = 1; // stop all particle effects
}

void pausemenu_update()
{
    int i;
    pausemenu_pan_background->scale_x = screen_size.x / bmap_width(pausemenu_pan_background->bmap);
    pausemenu_pan_background->scale_y = screen_size.y / bmap_height(pausemenu_pan_background->bmap);

    if(pausemenu_shows_options)
    {
        options_update();
        if(options_wants_close())
        {
            options_close();
            pausemenu_shows_options = false;
        }
        return;
    }

    if(input_hit(INPUT_NAVBACK))
    {
        snd_play(ui_accept_snd, 100, 0);
        pausemenu_response = PAUSEMENU_RESPONSE_CONTINUE;
        return;
    }

    for(i = 0; i < PAUSEMENU_BUTTONS; i++)
    {
        pausemenu_buttons[i]->pos_x = minv(pausemenu_buttons[i]->pos_x + 50 * time_step, 0);
    }

    uisystem_update(pausemenu_ui);
}

void pausemenu_close()
{
    int i;
    reset(pausemenu_pan_background, SHOW);
    uisystem_hide_all(pausemenu_ui);
    freeze_mode = 0;
}
