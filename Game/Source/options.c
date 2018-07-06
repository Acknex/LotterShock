#include "global.h"
#include "options.h"
#include "input.h"
#include "ui.h"
#include "settings.h"

#include <acknex.h>
#include <d3d9.h>



BMAP * options_bmp_pane = "options_pane.png";
BMAP * options_bmp_tab_common = "options_tab_common.png";
BMAP * options_bmp_tab_input = "options_tab_input.png";
BMAP * options_bmp_btn_cancel = "options_cancel.png";
BMAP * options_bmp_btn_save = "options_save.png";
BMAP * options_bmp_inputslot = "option_inputslot.png";
BMAP * options_bmp_xbox = "options_xbox.png";
BMAP * options_bmp_assignwindow = "options_assignwait.png";
BMAP * options_bmp_axis = "options_axis.png";

FONT * options_panel_font = "Arial#26b";

FONT * options_inputslot_font = "Impact#24b";

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

PANEL * options_pan_assignwindow =
{
    bmap = options_bmp_assignwindow;
    layer = 510;
}

TEXT * options_txt_brush =
{
    string ( "hi" );
    font = options_inputslot_font;
    flags = LIGHT | CENTER_X | CENTER_Y;
    red = 0;
    green = 0;
    blue = 0;
}

int options_current_tab;

bool options_done;

uisystem_t * options_ui;

struct inputslot_t
{
    INPUT * target;
    uibutton_t * slot[4];
    BMAP * content[4];
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

struct
{
    var timer;
    int inputid;
    int slot;
    bool isClear;
} options_assign_key;

#define OPTIONGROUP_COMMON 1
#define OPTIONGROUP_INPUT  2

INPUT options_input_copy[INPUT_MAX];

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

    memcpy(input, options_input_copy, sizeof(INPUT) * INPUT_MAX);

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
    options_assign_key.timer = 5 * 16;
    options_assign_key.inputid = btn->skill1;
    options_assign_key.slot = btn->skill2;
    options_assign_key.isClear = false;
    set(options_pan_assignwindow, SHOW);
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
            optionbutton.inputs[i].slot[j]->skill1 = i;
            optionbutton.inputs[i].slot[j]->skill2 = j;


            optionbutton.inputs[i].content[j] = bmap_createblack(37, 37, 8888);
            pan_setwindow(optionbutton.inputs[i].slot[j]->pan, 0, 3, 3, 37, 37, optionbutton.inputs[i].content[j], &nullvector->x, &nullvector->y);
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

    memcpy(options_input_copy, input, sizeof(INPUT) * INPUT_MAX);

    uisystem_show_all(options_ui);
    options_select_common();
}

STRING * options_tempstring = "#64";

void options_update()
{
    int i;

    options_pan_background->scale_x = screen_size.x / bmap_width(options_pan_background->bmap);
    options_pan_background->scale_y = screen_size.y / bmap_height(options_pan_background->bmap);

    options_pan_pane->pos_x = (screen_size.x - bmap_width(options_pan_pane->bmap)) / 2;
    options_pan_pane->pos_y = (screen_size.y - bmap_height(options_pan_pane->bmap)) / 2;

    options_ui->pos_x = options_pan_pane->pos_x;
    options_ui->pos_y = options_pan_pane->pos_y;

    options_pan_assignwindow->pos_x = (screen_size.x - bmap_width(options_pan_assignwindow->bmap)) / 2;
    options_pan_assignwindow->pos_y = (screen_size.y - bmap_height(options_pan_assignwindow->bmap)) / 2;

    if(is(options_pan_assignwindow, SHOW))
    {
        if(options_assign_key.timer <= 0)
        {
            reset(options_pan_assignwindow, SHOW);
        }
        else
        {
            options_assign_key.timer -= time_step;

            input_config_t cfg;
            memcpy(&cfg, &options_input_copy[options_assign_key.inputid].configs[options_assign_key.slot], sizeof(input_config_t));

            bool wasset = false;

            if(key_any)
            {
                cfg.type = INPUT_TYPE_KEYBOARD;
                cfg.index = key_lastpressed;
                wasset = true;
            }

            for(i = 0; i < 16; i++)
            {
                if(ackXInputGetButtonState3(i))
                {
                    cfg.type = INPUT_TYPE_GAMEPAD;
                    cfg.index = i;
                    wasset = true;
                }
            }
            for(i = 2; i < INPUT_AXIS_MAX; i++)
            {
                if(abs(input_axis(i)) > 0.3)
                {
                    cfg.type = INPUT_TYPE_AXIS;
                    cfg.index = i;
                    cfg.inverted = (input_axis(i) < 0);
                    cfg.deadZone = 0.3;
                    wasset = true;
                }
            }

            if(options_assign_key.isClear)
            {
                if(wasset)
                {
                    memcpy(&options_input_copy[options_assign_key.inputid].configs[options_assign_key.slot], &cfg, sizeof(input_config_t));
                    snd_play(ui_accept_snd, 100, 0);
                    reset(options_pan_assignwindow, SHOW);
                }
            }
            else
            {
                if(!wasset)
                    options_assign_key.isClear = true;
            }

            return;
        }
    }

    uisystem_update(options_ui);

    int i, j;
    for(i = 0; i < INPUT_MAX; i++)
    {
        optionbutton.inputs[i].description->pos_x = optionbutton.inputs[i].slot[0]->pan->pos_x - 3;
        optionbutton.inputs[i].description->pos_y = optionbutton.inputs[i].slot[0]->pan->pos_y + (46 - optionbutton.inputs[i].description->font->dy) / 2;

        for(j = 0; j < 4; j++)
        {
            input_config_t * cfg = &options_input_copy[i].configs[j];

            // Render the input icon
            bmap_rendertarget(optionbutton.inputs[i].content[j], 0, 0);

            LPDIRECT3DDEVICE9 pd3dDev;
            pd3dDev = (LPDIRECT3DDEVICE9)draw_begin();
            if(pd3dDev)
            {
                pd3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(255, 255, 255, 0), 1.0, 0);
            }

            switch(cfg->type)
            {
            case INPUT_TYPE_NONE:
                // Nothing to do here
                break;

            case INPUT_TYPE_KEYBOARD:
                str_for_key((options_txt_brush->pstring)[0], cfg->index);
                str_upr((options_txt_brush->pstring)[0]);
                options_txt_brush->pos_x = 37 / 2.0;
                options_txt_brush->pos_y = 37 / 2.0;
                draw_obj(options_txt_brush);
                break;

            case INPUT_TYPE_GAMEPAD:
                draw_quad(
                    options_bmp_xbox,
                    vector(0, 0, 0),
                    vector(37 * cfg->index, 0, 0),
                    vector(37, 37, 0),
                    NULL,
                    NULL,
                    100,
                    0);
                break;

            case INPUT_TYPE_AXIS:
                if(cfg->inverted)
                    draw_quad(NULL, vector(0,0,0),NULL,vector(37,37,0),NULL, vector(192,192,255), 100, 0);
                else
                    draw_quad(NULL, vector(0,0,0),NULL,vector(37,37,0),NULL, vector(255,192,192), 100, 0);
                draw_quad(
                    options_bmp_axis,
                    vector(0, 0, 0),
                    vector(37 * cfg->index, 0, 0),
                    vector(37, 37, 0),
                    NULL,
                    NULL,
                    100,
                    0);

                break;

            }
            bmap_rendertarget(NULL, 0, 0);
        }
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
