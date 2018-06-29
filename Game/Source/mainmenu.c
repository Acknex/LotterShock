#include "mainmenu.h"

#define MAINMENU_ITEM_COUNT 3
#define MAINMENU_BORDER_PADDING 16

BMAP * mainmenu_start_bmap = "mainmenu_start.png";
BMAP * mainmenu_credits_bmap = "mainmenu_credits.png";
BMAP * mainmenu_exit_bmap = "mainmenu_exit.png";
BMAP * mainmenu_selection_bmap = "mainmenu_selection.png";

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

PANEL * mainmenu_selection_pan =
{
    bmap = mainmenu_selection_bmap;
    flags = TRANSLUCENT;
    layer = 3;
}

PANEL * mainmenu_items[MAINMENU_ITEM_COUNT];

int mainmenu_selection;

void mainmenu_init()
{
    mainmenu_items[0] = mainmenu_start_pan;
    mainmenu_items[1] = mainmenu_credits_pan;
    mainmenu_items[2] = mainmenu_exit_pan;
}

void mainmenu_open()
{
    int i;
    for(i = 0; i < 3; i++)
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
}

void mainmenu_update()
{
    int i;

    mainmenu_selection_pan->alpha = 0;
    for(i = 0; i < 3; i++)
    {
        mainmenu_items[i]->alpha = clamp(
            mainmenu_items[i]->alpha + 10 * time_step,
            0,
            100);
        mainmenu_selection_pan->alpha = maxv(
            mainmenu_selection_pan->alpha,
            mainmenu_items[i]->alpha);

        DEBUG_VAR(mainmenu_items[i]->alpha, 16 * i);

        if(mainmenu_items[i]->alpha < 70)
            break;

        if(mouse_panel == mainmenu_items[i])
            mainmenu_selection = i;
    }

    mainmenu_selection_pan->pos_x = mainmenu_items[mainmenu_selection]->pos_x;
    mainmenu_selection_pan->pos_y = mainmenu_items[mainmenu_selection]->pos_y;
}

void mainmenu_close()
{
    int i;
    for(i = 0; i < 3; i++)
    {
        reset(mainmenu_items[i], SHOW);
    }
}
