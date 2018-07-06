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

BMAP * options_bmp_less = "options_less.png";
BMAP * options_bmp_more = "options_more.png";

BMAP * options_bmp_checkbox = "option_inputslot_checked.png";

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

TEXT * options_txt_resolution =
{
    string("Resolution");
    font = options_panel_font;
    flags = ARIGHT | OUTLINE;
    layer = 503;
}

TEXT * options_txt_fullscreen =
{
    string("Fullscreen");
    font = options_panel_font;
    flags = ARIGHT | OUTLINE;
    layer = 503;
}

TEXT * options_txt_vsync =
{
    string("V-Sync");
    font = options_panel_font;
    flags = ARIGHT | OUTLINE;
    layer = 503;
}

TEXT * options_txt_fps_limit =
{
    string("FPS Limit");
    font = options_panel_font;
    flags = ARIGHT | OUTLINE;
    layer = 503;
}

TEXT * options_txt_anisotropic =
{
    string("Anisotropic Filter");
    font = options_panel_font;
    flags = ARIGHT | OUTLINE;
    layer = 503;
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

    uibutton_t * prevResolution;
    uibutton_t * nextResolution;

    uibutton_t * fullscreen;
    uibutton_t * vsync;

    uibutton_t * fpslimits[5];
    uibutton_t * anisotropy[5];

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

settings_t options_settings_copy;

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
    memcpy(input, options_input_copy, sizeof(INPUT) * INPUT_MAX);
    memcpy(settings, options_settings_copy, sizeof(settings_t));

    settings_save();

    settings_send_signal();

    options_done = 1;
}

void options_select_common()
{
    set(optionbutton.common_tab->pan, LIGHT);
    reset(optionbutton.input_tab->pan, LIGHT);

    set(options_txt_resolution, SHOW);
    set(options_txt_fullscreen, SHOW);
    set(options_txt_vsync, SHOW);
    set(options_txt_fps_limit, SHOW);
    set(options_txt_anisotropic, SHOW);

    uisystem_show_group(options_ui, OPTIONGROUP_COMMON);
    uisystem_hide_group(options_ui, OPTIONGROUP_INPUT);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        reset(optionbutton.inputs[i].description, SHOW);
    }
    optionbutton.input_tab->neighbour[UIDIR_DOWN] = optionbutton.nextResolution;
    optionbutton.common_tab->neighbour[UIDIR_DOWN] = optionbutton.prevResolution;

    optionbutton.save->neighbour[UIDIR_UP] = optionbutton.anisotropy[4];
    optionbutton.cancel->neighbour[UIDIR_UP] = optionbutton.anisotropy[4];
}

void options_select_input()
{
    reset(optionbutton.common_tab->pan, LIGHT);
    set(optionbutton.input_tab->pan, LIGHT);

    reset(options_txt_resolution, SHOW);
    reset(options_txt_fullscreen, SHOW);
    reset(options_txt_vsync, SHOW);
    reset(options_txt_fps_limit, SHOW);
    reset(options_txt_anisotropic, SHOW);

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

void options_toggle_fullscreen()
{
    options_settings_copy.fullscreen = !options_settings_copy.fullscreen;
}

void options_toggle_vsync()
{
    options_settings_copy.vsync = !options_settings_copy.vsync;
}

void options_set_fps_limit(uibutton_t * btn)
{
    options_settings_copy.fps_limit = btn->skill1;
}

void options_set_anisotropy(uibutton_t * btn)
{
    options_settings_copy.anisotropy = btn->skill1;
}

void options_init()
{
    int i;

    options_ui = uisystem_new(502);

    optionbutton.save   = uisystem_add_button(options_ui, 622, 342, options_bmp_btn_save,   options_save);
    optionbutton.cancel = uisystem_add_button(options_ui, 460, 342, options_bmp_btn_cancel, options_cancel);
    optionbutton.common_tab = uisystem_add_button(options_ui, 0,   -40, options_bmp_tab_common, options_select_common);
    optionbutton.input_tab  = uisystem_add_button(options_ui, 192, -40, options_bmp_tab_input,  options_select_input);

    vec_set(optionbutton.common_tab->pan->blue, vector(34, 61, 208));
    vec_set(optionbutton.input_tab->pan->blue, vector(34, 61, 208));

    optionbutton.save->neighbour[UIDIR_LEFT] = optionbutton.cancel;
    optionbutton.cancel->neighbour[UIDIR_RIGHT] = optionbutton.save;

    optionbutton.input_tab->neighbour[UIDIR_LEFT] = optionbutton.common_tab;
    optionbutton.common_tab->neighbour[UIDIR_RIGHT] = optionbutton.input_tab;

    /* INPUT
     * settings.input_sensitivity = 13;
     */
    /* COMMON
     * --------------------------------------------
     * Resolution:  [ settings.resolution_x, settings.resolution_y ]
     * Fullscreen:  [ 'X', ' ' ]
     * V-Sync:      [ 'X', ' ' ]
     * FPS Limit:   [ 30, 60, 90, 144, free ]
     * Anisotropic: [ 0, 1, 2, 3, 4] -> 0, 1, 3, 5, 7
     * --------------------------------------------
     * Master Volume: 100
     *
     */

    optionbutton.prevResolution = uisystem_add_button(options_ui, 190, 10, options_bmp_less, NULL);
    optionbutton.prevResolution->group = OPTIONGROUP_COMMON;

    optionbutton.nextResolution = uisystem_add_button(options_ui, 350, 10, options_bmp_more, NULL);
    optionbutton.nextResolution->group = OPTIONGROUP_COMMON;

    optionbutton.fullscreen = uisystem_add_button(options_ui, 190, 60, options_bmp_inputslot, options_toggle_fullscreen);
    optionbutton.fullscreen->group = OPTIONGROUP_COMMON;

    optionbutton.vsync = uisystem_add_button(options_ui, 190, 110, options_bmp_inputslot, options_toggle_vsync);
    optionbutton.vsync->group = OPTIONGROUP_COMMON;

    for(i = 0; i < 5; i++)
    {
        optionbutton.fpslimits[i] = uisystem_add_button(options_ui, 190 + 46 * i, 160, options_bmp_inputslot, options_set_fps_limit);
        optionbutton.fpslimits[i]->group = OPTIONGROUP_COMMON;
    }

    optionbutton.fpslimits[0]->skill1 = 30;
    optionbutton.fpslimits[1]->skill1 = 60;
    optionbutton.fpslimits[2]->skill1 = 90;
    optionbutton.fpslimits[3]->skill1 = 144;
    optionbutton.fpslimits[4]->skill1 = 999;

    for(i = 0; i < 5; i++)
    {
        optionbutton.anisotropy[i] = uisystem_add_button(options_ui, 190 + 46 * i, 210, options_bmp_inputslot, options_set_anisotropy);
        optionbutton.anisotropy[i]->group = OPTIONGROUP_COMMON;
    }

    optionbutton.anisotropy[0]->skill1 = 0;
    optionbutton.anisotropy[1]->skill1 = 1;
    optionbutton.anisotropy[2]->skill1 = 3;
    optionbutton.anisotropy[3]->skill1 = 5;
    optionbutton.anisotropy[4]->skill1 = 7;

    optionbutton.prevResolution->neighbour[UIDIR_UP] = optionbutton.common_tab;
    optionbutton.prevResolution->neighbour[UIDIR_DOWN] = optionbutton.fullscreen;
    optionbutton.prevResolution->neighbour[UIDIR_RIGHT] = optionbutton.nextResolution;

    optionbutton.nextResolution->neighbour[UIDIR_UP] = optionbutton.common_tab;
    optionbutton.nextResolution->neighbour[UIDIR_DOWN] = optionbutton.fullscreen;
    optionbutton.nextResolution->neighbour[UIDIR_LEFT] = optionbutton.prevResolution;

    optionbutton.fullscreen->neighbour[UIDIR_UP] = optionbutton.prevResolution;
    optionbutton.fullscreen->neighbour[UIDIR_DOWN] = optionbutton.vsync;

    optionbutton.vsync->neighbour[UIDIR_UP] = optionbutton.fullscreen;
    optionbutton.vsync->neighbour[UIDIR_DOWN] = optionbutton.fpslimits[0];

    for(i = 0; i < 5; i++)
    {
        optionbutton.anisotropy[i]->neighbour[UIDIR_UP] = optionbutton.fpslimits[i];
        optionbutton.anisotropy[i]->neighbour[UIDIR_DOWN] = optionbutton.cancel;

        optionbutton.fpslimits[i]->neighbour[UIDIR_UP] =  optionbutton.vsync;
        optionbutton.fpslimits[i]->neighbour[UIDIR_DOWN] = optionbutton.anisotropy[i];

        if(i > 0)
        {
            optionbutton.anisotropy[i]->neighbour[UIDIR_LEFT] = optionbutton.anisotropy[i - 1];
            optionbutton.fpslimits[i]->neighbour[UIDIR_LEFT] = optionbutton.fpslimits[i - 1];
        }

        if(i < 4)
        {
            optionbutton.anisotropy[i]->neighbour[UIDIR_RIGHT] = optionbutton.anisotropy[i + 1];
            optionbutton.fpslimits[i]->neighbour[UIDIR_RIGHT] = optionbutton.fpslimits[i + 1];
        }
    }

    { // initialize and interconnect all input slots

        int middle = INPUT_MAX / 2;

        options_init_slots(0, middle, 190, 6);

        options_init_slots(middle, INPUT_MAX, 600, 6);

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
    memcpy(options_settings_copy, settings, sizeof(settings_t));

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
            options_input_copy[options_assign_key.inputid].configs[options_assign_key.slot].type = INPUT_TYPE_NONE;
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

    if(options_settings_copy.fullscreen)
        optionbutton.fullscreen->bmap = options_bmp_checkbox;
    else
        optionbutton.fullscreen->bmap = options_bmp_inputslot;

    if(options_settings_copy.vsync)
        optionbutton.vsync->bmap = options_bmp_checkbox;
    else
        optionbutton.vsync->bmap = options_bmp_inputslot;

    for(i = 0; i < 5; i++)
    {
        if(options_settings_copy.anisotropy == optionbutton.anisotropy[i]->skill1)
            optionbutton.anisotropy[i]->bmap = options_bmp_checkbox;
        else
            optionbutton.anisotropy[i]->bmap = options_bmp_inputslot;

        if(options_settings_copy.fps_limit == optionbutton.fpslimits[i]->skill1)
            optionbutton.fpslimits[i]->bmap = options_bmp_checkbox;
        else
            optionbutton.fpslimits[i]->bmap = options_bmp_inputslot;
    }

    uisystem_update(options_ui);

    var dy = (43 - options_txt_resolution->font->dy) / 2;

    options_txt_resolution->pos_x = optionbutton.prevResolution->pan->pos_x - 3;
    options_txt_resolution->pos_y = optionbutton.prevResolution->pan->pos_y + dy;

    options_txt_fullscreen->pos_x = optionbutton.fullscreen->pan->pos_x - 3;
    options_txt_fullscreen->pos_y = optionbutton.fullscreen->pan->pos_y + dy;

    options_txt_vsync->pos_x = optionbutton.vsync->pan->pos_x - 3;
    options_txt_vsync->pos_y = optionbutton.vsync->pan->pos_y + dy;

    options_txt_fps_limit->pos_x = optionbutton.fpslimits[0]->pan->pos_x - 3;
    options_txt_fps_limit->pos_y = optionbutton.fpslimits[0]->pan->pos_y + dy;

    options_txt_anisotropic->pos_x = optionbutton.anisotropy[0]->pan->pos_x - 3;
    options_txt_anisotropic->pos_y = optionbutton.anisotropy[0]->pan->pos_y + dy;

    int i, j;
    for(i = 0; i < INPUT_MAX; i++)
    {
        optionbutton.inputs[i].description->pos_x = optionbutton.inputs[i].slot[0]->pan->pos_x - 3;
        optionbutton.inputs[i].description->pos_y = optionbutton.inputs[i].slot[0]->pan->pos_y + dy;

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
                if(cfg->index > 0 && cfg->index < 256)
                {
                    str_for_key((options_txt_brush->pstring)[0], cfg->index);
                    str_upr((options_txt_brush->pstring)[0]);
                }
                else
                {
                    str_for_num((options_txt_brush->pstring)[0], cfg->index);
                }
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

    reset(options_txt_resolution, SHOW);
    reset(options_txt_fullscreen, SHOW);
    reset(options_txt_vsync, SHOW);
    reset(options_txt_fps_limit, SHOW);
    reset(options_txt_anisotropic, SHOW);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        reset(optionbutton.inputs[i].description, SHOW);
    }

    uisystem_hide_all(options_ui);
}
