#include "global.h"
#include "options.h"
#include "input.h"
#include "ui.h"
#include "settings.h"

#include <acknex.h>

#define OPTIONS_TAB_COUNT 2

BMAP * options_bmp_pane = "options_pane.png";
BMAP * options_bmp_tab_common = "options_tab_common.png";
BMAP * options_bmp_tab_input = "options_tab_input.png";

PANEL * options_pan_tab_common =
{
    bmap = options_bmp_tab_common;
    red = 208;
    green = 61;
    blue = 34;
    layer = 510;
}

PANEL * options_pan_tab_input =
{
    bmap = options_bmp_tab_input;
    red = 208;
    green = 61;
    blue = 34;
    layer = 510;
}

PANEL * options_pan_pane =
{
    bmap = options_bmp_pane;
    layer = 500;
}

PANEL * options_tab_panels[OPTIONS_TAB_COUNT];

int options_current_tab;

bool options_done;

bool options_wants_close()
{
    return options_done;
}

void options_init()
{
    options_tab_panels[0] = options_pan_tab_common;
    options_tab_panels[1] = options_pan_tab_input;
}

void options_open()
{
    int i;

    set(options_pan_pane, SHOW);
    for(i = 0; i < OPTIONS_TAB_COUNT; i++)
    {
        set(options_tab_panels[i], SHOW);
    }
    options_current_tab = 0;
    options_done = false;
}

void options_update()
{
    int i;

    options_pan_pane->pos_x = (screen_size.x - bmap_width(options_pan_pane->bmap)) / 2;
    options_pan_pane->pos_y = (screen_size.y - bmap_height(options_pan_pane->bmap)) / 2;

    for(i = 0; i < OPTIONS_TAB_COUNT; i++)
    {
        options_tab_panels[i]->pos_x = options_pan_pane->pos_x + 176 * i;
        options_tab_panels[i]->pos_y = options_pan_pane->pos_y - bmap_height(options_tab_panels[i]->bmap) + 3;

        if(options_current_tab == i)
        {
            if(options_tab_panels[i]->layer != 511)
                layer_sort(options_tab_panels[i], 511);
        }
        else
        {
            if(options_tab_panels[i]->layer != 510)
                layer_sort(options_tab_panels[i], 510);
        }
    }

    if(input_hit(INPUT_NAVBACK))
        options_done = true;
}

void options_close()
{
    int i;
    reset(options_pan_pane, SHOW);
    for(i = 0; i < OPTIONS_TAB_COUNT; i++)
    {
        reset(options_tab_panels[i], SHOW);
    }

}
