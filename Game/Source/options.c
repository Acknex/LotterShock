#include "global.h"
#include "options.h"
#include "input.h"
#include "ui.h"
#include "settings.h"

#include <acknex.h>

BMAP * options_bmp_pane = "options_pane.png";
BMAP * options_bmp_tab_common = "options_tab_common.png";
BMAP * options_bmp_tab_input = "options_tab_input.png";
BMAP * options_bmp_btn_cancel = "options_cancel.png";
BMAP * options_bmp_btn_save = "options_save.png";
BMAP * options_bmp_inputslot = "option_inputslot.png";

PANEL * options_pan_pane =
{
    bmap = options_bmp_pane;
    layer = 501;
}

PANEL * options_pan_background =
{
    bmap = ui_bmp_background;
    layer = 500;
}

FONT * options_panel_font = "Arial#26b";

int options_current_tab;

bool options_done;

uisystem_t * options_ui;

struct inputslot_t
{
    INPUT * target;
    uibutton_t * slot[4];
    TEXT * description;
};

struct
{
    uibutton_t * common_tab;
    uibutton_t * input_tab;

    uibutton_t * save;
    uibutton_t * cancel;

    struct inputslot_t inputs[INPUT_MAX];
} optionbutton;

#define OPTIONGROUP_COMMON 1
#define OPTIONGROUP_INPUT  2

bool options_wants_close()
{
    return options_done;
}

void options_cancel()
{
    options_done = 1;
}

void options_save()
{
    error("save here!");
    options_done = 1;
}

void options_select_common()
{
    set(optionbutton.common_tab->pan, LIGHT);
    reset(optionbutton.input_tab->pan, LIGHT);

    uisystem_show_group(options_ui, OPTIONGROUP_COMMON);
    uisystem_hide_group(options_ui, OPTIONGROUP_INPUT);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        reset(optionbutton.inputs[i].description, SHOW);
    }
    optionbutton.input_tab->neighbour[UIDIR_DOWN] = optionbutton.save;
    optionbutton.common_tab->neighbour[UIDIR_DOWN] = optionbutton.save;

    optionbutton.save->neighbour[UIDIR_UP] = optionbutton.input_tab;
    optionbutton.cancel->neighbour[UIDIR_UP] = optionbutton.input_tab;
}

void options_select_input()
{
    reset(optionbutton.common_tab->pan, LIGHT);
    set(optionbutton.input_tab->pan, LIGHT);

    uisystem_hide_group(options_ui, OPTIONGROUP_COMMON);
    uisystem_show_group(options_ui, OPTIONGROUP_INPUT);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        set(optionbutton.inputs[i].description, SHOW);
    }

    optionbutton.input_tab->neighbour[UIDIR_DOWN] = optionbutton.inputs[0].slot[0];
    optionbutton.common_tab->neighbour[UIDIR_DOWN] = optionbutton.inputs[0].slot[0];

    optionbutton.save->neighbour[UIDIR_UP] = optionbutton.inputs[INPUT_MAX - 1].slot[0];
    optionbutton.cancel->neighbour[UIDIR_UP] = optionbutton.inputs[INPUT_MAX - 1].slot[0];
}

void options_setup_input(uibutton_t * btn)
{
    error("change input here!");
}

void options_init_slots(int from, int to, int x_offset, int y_offset)
{
    int i, j;
    for(i = from; i < to; i++)
    {
        optionbutton.inputs[i].description = txt_create(1, 503);
        optionbutton.inputs[i].description->font = options_panel_font;
        str_cpy((optionbutton.inputs[i].description->pstring)[0], input[i].cinfo);
        optionbutton.inputs[i].description->flags = OUTLINE | ARIGHT;
        optionbutton.inputs[i].description->size_x = 400;
        optionbutton.inputs[i].description->size_y = 43;

        for(j = 0; j < 4; j++)
        {
            optionbutton.inputs[i].slot[j] = uisystem_add_button(options_ui, x_offset + 46 * j, y_offset + 46 * (i - from), options_bmp_inputslot, options_setup_input);
            optionbutton.inputs[i].slot[j]->group = OPTIONGROUP_INPUT;
        }
    }
    // connect all input slots
    for(i = from; i < to; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(j > 0)
                optionbutton.inputs[i].slot[j]->neighbour[UIDIR_LEFT] = optionbutton.inputs[i].slot[j - 1];
            if(j < 3)
                optionbutton.inputs[i].slot[j]->neighbour[UIDIR_RIGHT] = optionbutton.inputs[i].slot[j + 1];
            if(i > from)
                optionbutton.inputs[i].slot[j]->neighbour[UIDIR_UP] = optionbutton.inputs[i - 1].slot[j];
            if(i < (to - 1))
                optionbutton.inputs[i].slot[j]->neighbour[UIDIR_DOWN] = optionbutton.inputs[i + 1].slot[j];
        }
    }
}

void options_init()
{
    options_ui = uisystem_new(502);

    optionbutton.save   = uisystem_add_button(options_ui, 990, 537, options_bmp_btn_save,   options_save);
    optionbutton.cancel = uisystem_add_button(options_ui, 828, 537, options_bmp_btn_cancel, options_cancel);
    optionbutton.common_tab = uisystem_add_button(options_ui, 0,   -40, options_bmp_tab_common, options_select_common);
    optionbutton.input_tab  = uisystem_add_button(options_ui, 192, -40, options_bmp_tab_input,  options_select_input);

    vec_set(optionbutton.common_tab->pan->blue, vector(34, 61, 208));
    vec_set(optionbutton.input_tab->pan->blue, vector(34, 61, 208));

    optionbutton.save->neighbour[UIDIR_LEFT] = optionbutton.cancel;
    optionbutton.cancel->neighbour[UIDIR_RIGHT] = optionbutton.save;

    optionbutton.input_tab->neighbour[UIDIR_LEFT] = optionbutton.common_tab;
    optionbutton.common_tab->neighbour[UIDIR_RIGHT] = optionbutton.input_tab;

    { // initialize and interconnect all input slots

        int middle = INPUT_MAX / 2;

        options_init_slots(0, middle, bmap_width(options_pan_pane->bmap) / 2 - 187, 6);

        options_init_slots(middle, INPUT_MAX, bmap_width(options_pan_pane->bmap) - 187, 6);

        int i;
        // connect left with right part
        for(i = 0; i < middle; i++)
        {
            int other = minv(middle + i, INPUT_MAX - 1);
            if(other < INPUT_MAX)
            {
                optionbutton.inputs[i].slot[3]->neighbour[UIDIR_RIGHT] = optionbutton.inputs[other].slot[0];
                if((other - middle) == i)
                    optionbutton.inputs[other].slot[0]->neighbour[UIDIR_LEFT] = optionbutton.inputs[i].slot[3];
            }
        }

        // connect tops and bottoms
        for(i = 0; i < 4; i++)
        {
            optionbutton.inputs[0].slot[i]->neighbour[UIDIR_UP] = optionbutton.input_tab;
            optionbutton.inputs[middle].slot[i]->neighbour[UIDIR_UP] = optionbutton.input_tab;

            optionbutton.inputs[INPUT_MAX - 1].slot[i]->neighbour[UIDIR_DOWN] = optionbutton.save;
            optionbutton.inputs[middle - 1].slot[i]->neighbour[UIDIR_DOWN] = optionbutton.save;

        }
    }
}

void options_open()
{
    int i;

    set(options_pan_pane, SHOW);
    set(options_pan_background, SHOW);
    options_current_tab = 0;
    options_done = false;

    uisystem_show_all(options_ui);
    options_select_common();
}

void options_update()
{
    int i;

    options_pan_background->scale_x = screen_size.x / bmap_width(options_pan_background->bmap);
    options_pan_background->scale_y = screen_size.y / bmap_height(options_pan_background->bmap);

    options_pan_pane->pos_x = (screen_size.x - bmap_width(options_pan_pane->bmap)) / 2;
    options_pan_pane->pos_y = (screen_size.y - bmap_height(options_pan_pane->bmap)) / 2;

    options_ui->pos_x = options_pan_pane->pos_x;
    options_ui->pos_y = options_pan_pane->pos_y;

    uisystem_update(options_ui);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {

        optionbutton.inputs[i].description->pos_x = optionbutton.inputs[i].slot[0]->pan->pos_x - 3;
        optionbutton.inputs[i].description->pos_y = optionbutton.inputs[i].slot[0]->pan->pos_y + (46 - optionbutton.inputs[i].description->font->dy) / 2;
    }

    if(input_hit(INPUT_NAVBACK))
        options_done = true;
}

void options_close()
{
    int i;
    reset(options_pan_background, SHOW);
    reset(options_pan_pane, SHOW);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        reset(optionbutton.inputs[i].description, SHOW);
    }

    uisystem_hide_all(options_ui);
}
