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

BMAP * options_bmp_inbetween_small = "options_inbetween_small.png";
BMAP * options_bmp_inbetween_large = "options_inbetween_large.png";

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

typedef struct slider_t
{
    uibutton_t * decrease;
    uibutton_t * increase;
    TEXT * label;
    TEXT * value;
    PANEL * inbetween;

    var lowerlimit, upperlimit;
    var step;
    var * target;
} slider_t ;

typedef struct checkbox_t
{
    bool * target;
    uibutton_t * button;
    TEXT * label;
} checkbox_t;

typedef struct selector_t
{
    uibutton_t * decrease;
    uibutton_t * increase;
    TEXT * label;
    TEXT * value;
    PANEL * inbetween;

    int * target;
    TEXT * options;
} selector_t ;

struct
{
    // COMMON
    uibutton_t * common_tab;
    uibutton_t * input_tab;

    uibutton_t * save;
    uibutton_t * cancel;

    selector_t resolution;

    checkbox_t fullscreen;
    checkbox_t vsync;

    selector_t fpslimit;
    selector_t anisotropy;

    // INPUT
    struct inputslot_t inputs[INPUT_MAX];

    slider_t hsensitivity;
    slider_t vsensitivity;

    checkbox_t hinvert;
    checkbox_t vinvert;
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

int options_selected_resolution;
int options_selected_fpslimit;
int options_selected_anisotropy;

void options_selector_click(uibutton_t * btn)
{
    selector_t * selector = btn->skill1;
    (*selector->target) = cycle(
        (*selector->target) + (int)(btn->skill2),
        0,
        selector->options->strings);
}

void options_selector_init(selector_t * selector, int * _target, TEXT * options, var x, var y, BMAP * inbetween, STRING * label, int group)
{
    selector->target = _target;
    selector->options = options;

    selector->decrease = uisystem_add_button(options_ui, x, y, options_bmp_less, options_selector_click);
    selector->increase = uisystem_add_button(options_ui, x + bmap_width(options_bmp_less) + 6 + bmap_width(inbetween) , y, options_bmp_more, options_selector_click);

    selector->decrease->skill1 = selector;
    selector->increase->skill1 = selector;

    selector->decrease->skill2 = -1;
    selector->increase->skill2 =  1;

    selector->decrease->group = group;
    selector->increase->group = group;

    selector->decrease->neighbour[UIDIR_RIGHT] = selector->increase;
    selector->increase->neighbour[UIDIR_LEFT] = selector->decrease;

    selector->inbetween = pan_create("", 503);
    selector->inbetween->bmap = inbetween;

    selector->label = txt_create(1, 504);
    selector->label->flags = ARIGHT | OUTLINE;
    selector->label->font = options_panel_font;
    str_cpy((selector->label->pstring)[0], label);

    selector->value = txt_create(1, 504);
    selector->value->flags = CENTER_X | LIGHT;
    selector->value->font = options_inputslot_font;
    vec_set(&selector->value->blue, vector(0,0,0));
    str_cpy((selector->value->pstring)[0], "~~");
}

void options_selector_update(selector_t * selector)
{
    selector->inbetween->pos_x = selector->decrease->pan->pos_x + bmap_width(selector->decrease->bmap) + 3;
    selector->inbetween->pos_y = selector->decrease->pan->pos_y;

    selector->label->pos_x = selector->decrease->pan->pos_x - 3;
    selector->label->pos_y = selector->decrease->pan->pos_y + (43 - selector->label->font->dy) / 2;

    selector->value->pos_x = selector->inbetween->pos_x + bmap_width(selector->inbetween->bmap) / 2;
    selector->value->pos_y = selector->decrease->pan->pos_y + (43 - selector->value->font->dy) / 2;

    str_cpy((selector->value->pstring)[0], (selector->options->pstring)[(*selector->target)]);

    if(is(selector->decrease->pan, SHOW))
    {
        set(selector->inbetween, SHOW);
        set(selector->label, SHOW);
        set(selector->value, SHOW);
    }
    else
    {
        reset(selector->inbetween, SHOW);
        reset(selector->label, SHOW);
        reset(selector->value, SHOW);
    }
}

void options_selector_hide(selector_t * selector)
{
    reset(selector->inbetween, SHOW);
    reset(selector->label, SHOW);
    reset(selector->value, SHOW);
}

void options_modify_val(uibutton_t * btn)
{
    slider_t * slider = btn->skill1;

    (*slider->target) = clamp(
        (*slider->target) + btn->skill2,
        slider->lowerlimit,
        slider->upperlimit);
}

void options_slider_init(slider_t * slider, var * _target, var min, var max, var step, var x, var y, BMAP * inbetween, STRING * label, int group)
{
    slider->target = _target;
    slider->lowerlimit = min;
    slider->upperlimit = max;
    slider->step = step;

    slider->decrease = uisystem_add_button(options_ui, x, y, options_bmp_less, options_modify_val);
    slider->increase = uisystem_add_button(options_ui, x + bmap_width(options_bmp_less) + 6 + bmap_width(inbetween) , y, options_bmp_more, options_modify_val);

    slider->decrease->skill1 = slider;
    slider->increase->skill1 = slider;

    slider->decrease->skill2 = -step;
    slider->increase->skill2 =  step;

    slider->decrease->group = group;
    slider->increase->group = group;

    slider->decrease->neighbour[UIDIR_RIGHT] = slider->increase;
    slider->increase->neighbour[UIDIR_LEFT] = slider->decrease;

    slider->inbetween = pan_create("", 503);
    slider->inbetween->bmap = inbetween;

    slider->label = txt_create(1, 504);
    slider->label->flags = ARIGHT | OUTLINE;
    slider->label->font = options_panel_font;
    str_cpy((slider->label->pstring)[0], label);

    slider->value = txt_create(1, 504);
    slider->value->flags = CENTER_X | LIGHT;
    slider->value->font = options_inputslot_font;
    vec_set(&slider->value->blue, vector(0,0,0));
    str_printf((slider->value->pstring)[0], "%.1f", (double)*_target);
}

void options_slider_update(slider_t * slider)
{
    slider->inbetween->pos_x = slider->decrease->pan->pos_x + bmap_width(slider->decrease->bmap) + 3;
    slider->inbetween->pos_y = slider->decrease->pan->pos_y;

    slider->label->pos_x = slider->decrease->pan->pos_x - 3;
    slider->label->pos_y = slider->decrease->pan->pos_y + (43 - slider->label->font->dy) / 2;

    slider->value->pos_x = slider->inbetween->pos_x + bmap_width(slider->inbetween->bmap) / 2;
    slider->value->pos_y = slider->decrease->pan->pos_y + (43 - slider->value->font->dy) / 2;

    str_printf((slider->value->pstring)[0], "%.1f", (double)*slider->target);


    if(is(slider->decrease->pan, SHOW))
    {
        set(slider->inbetween, SHOW);
        set(slider->label, SHOW);
        set(slider->value, SHOW);
    }
    else
    {
        reset(slider->inbetween, SHOW);
        reset(slider->label, SHOW);
        reset(slider->value, SHOW);
    }
}

void options_slider_hide(slider_t * slider)
{
    reset(slider->inbetween, SHOW);
    reset(slider->label, SHOW);
    reset(slider->value, SHOW);
}


void options_toggle_val(uibutton_t * btn)
{
    checkbox_t * cb = btn->skill1;
    (*cb->target) = !(*cb->target);
}

void options_checkbox_init(checkbox_t * cb, bool * _target, var x, var y, STRING * label, int group)
{
    cb->target = _target;

    cb->button = uisystem_add_button(options_ui, x, y, options_bmp_inputslot, options_toggle_val);

    cb->button->skill1 = cb;

    cb->button->group = group;

    cb->label = txt_create(1, 504);
    cb->label->flags = ARIGHT | OUTLINE;
    cb->label->font = options_panel_font;
    str_cpy((cb->label->pstring)[0], label);
}

void options_checkbox_update(checkbox_t * cb)
{
    cb->label->pos_x = cb->button->pan->pos_x - 3;
    cb->label->pos_y = cb->button->pan->pos_y + (43 - cb->label->font->dy) / 2;

    if(*cb->target)
        cb->button->bmap = options_bmp_checkbox;
    else
        cb->button->bmap = options_bmp_inputslot;

    if(is(cb->button->pan, SHOW))
    {
        set(cb->label, SHOW);
    }
    else
    {
        reset(cb->label, SHOW);
    }
}

void options_checkbox_hide(checkbox_t * cb)
{
    reset(cb->label, SHOW);
}

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

    set(options_txt_fps_limit, SHOW);
    set(options_txt_anisotropic, SHOW);

    uisystem_show_group(options_ui, OPTIONGROUP_COMMON);
    uisystem_hide_group(options_ui, OPTIONGROUP_INPUT);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        reset(optionbutton.inputs[i].description, SHOW);
    }
    optionbutton.input_tab->neighbour[UIDIR_DOWN] = optionbutton.resolution.increase;
    optionbutton.common_tab->neighbour[UIDIR_DOWN] = optionbutton.resolution.decrease;

    optionbutton.save->neighbour[UIDIR_UP] = optionbutton.anisotropy.increase;
    optionbutton.cancel->neighbour[UIDIR_UP] = optionbutton.anisotropy.increase;
}

void options_select_input()
{
    reset(optionbutton.common_tab->pan, LIGHT);
    set(optionbutton.input_tab->pan, LIGHT);

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

    optionbutton.save->neighbour[UIDIR_UP] = optionbutton.vsensitivity.increase;
    optionbutton.cancel->neighbour[UIDIR_UP] = optionbutton.vsensitivity.increase;
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

TEXT * options_resolutions =
{
    string = ("1280x720", "1920x1080", "1920x1200", "2560x1440");
}
TEXT * options_fpslimits_text =
{
    string = ("30 FPS", "60 FPS", "90 FPS", "120 FPS", "144 FPS", "Unlimited");
}
TEXT * options_anisotropy_text =
{
    string = ("None", "1x", "2x", "3x", "4x", "5x", "6x", "7x");
}

void options_init()
{
    int i;

    options_ui = uisystem_new(502);

    optionbutton.save   = uisystem_add_button(options_ui, bmap_width(options_bmp_pane) - 165, bmap_height(options_bmp_pane) - 58, options_bmp_btn_save,   options_save);
    optionbutton.cancel = uisystem_add_button(options_ui, bmap_width(options_bmp_pane) - 327, bmap_height(options_bmp_pane) - 58, options_bmp_btn_cancel, options_cancel);
    optionbutton.common_tab = uisystem_add_button(options_ui, 0,   -40, options_bmp_tab_common, options_select_common);
    optionbutton.input_tab  = uisystem_add_button(options_ui, 192, -40, options_bmp_tab_input,  options_select_input);

    vec_set(optionbutton.common_tab->pan->blue, vector(34, 61, 208));
    vec_set(optionbutton.input_tab->pan->blue, vector(34, 61, 208));

    optionbutton.save->neighbour[UIDIR_LEFT] = optionbutton.cancel;
    optionbutton.cancel->neighbour[UIDIR_RIGHT] = optionbutton.save;

    optionbutton.input_tab->neighbour[UIDIR_LEFT] = optionbutton.common_tab;
    optionbutton.common_tab->neighbour[UIDIR_RIGHT] = optionbutton.input_tab;


    options_selector_init(&optionbutton.resolution, &options_selected_resolution, options_resolutions, 190, 10, options_bmp_inbetween_large, "Resolution", OPTIONGROUP_COMMON);

    options_checkbox_init(&optionbutton.fullscreen, &options_settings_copy.fullscreen, 190, 60, "Fullscreen", OPTIONGROUP_COMMON);
    options_checkbox_init(&optionbutton.vsync, &options_settings_copy.vsync, 190, 110, "V-Sync", OPTIONGROUP_COMMON);

    options_selector_init(&optionbutton.fpslimit, &options_selected_fpslimit, options_fpslimits_text, 190, 160, options_bmp_inbetween_large, "FPS Limit", OPTIONGROUP_COMMON);

    options_selector_init(&optionbutton.anisotropy, &options_settings_copy.anisotropy, options_anisotropy_text, 190, 210, options_bmp_inbetween_large, "Anisotropic Filter", OPTIONGROUP_COMMON);

    optionbutton.resolution.decrease->neighbour[UIDIR_UP] = optionbutton.common_tab;
    optionbutton.resolution.decrease->neighbour[UIDIR_DOWN] = optionbutton.fullscreen.button;

    optionbutton.resolution.increase->neighbour[UIDIR_UP] = optionbutton.common_tab;
    optionbutton.resolution.increase->neighbour[UIDIR_DOWN] = optionbutton.fullscreen.button;

    optionbutton.fullscreen.button->neighbour[UIDIR_UP] = optionbutton.resolution.decrease;
    optionbutton.fullscreen.button->neighbour[UIDIR_DOWN] = optionbutton.vsync.button;

    optionbutton.vsync.button->neighbour[UIDIR_UP] = optionbutton.fullscreen.button;
    optionbutton.vsync.button->neighbour[UIDIR_DOWN] = optionbutton.fpslimit.decrease;

    optionbutton.fpslimit.increase->neighbour[UIDIR_UP] = optionbutton.vsync.button;
    optionbutton.fpslimit.decrease->neighbour[UIDIR_UP] = optionbutton.vsync.button;

    optionbutton.fpslimit.increase->neighbour[UIDIR_DOWN] = optionbutton.anisotropy.increase;
    optionbutton.fpslimit.decrease->neighbour[UIDIR_DOWN] = optionbutton.anisotropy.decrease;

    optionbutton.anisotropy.increase->neighbour[UIDIR_UP] = optionbutton.fpslimit.increase;
    optionbutton.anisotropy.decrease->neighbour[UIDIR_UP] = optionbutton.fpslimit.decrease;

    optionbutton.anisotropy.increase->neighbour[UIDIR_DOWN] = optionbutton.cancel;
    optionbutton.anisotropy.decrease->neighbour[UIDIR_DOWN] = optionbutton.cancel;

    { // initialize and interconnect all input slots

        options_slider_init(&optionbutton.hsensitivity, &options_settings_copy.hsensitivity, 1, 20, 1, 193, 328, options_bmp_inbetween_small, "Horiz. Sensitivity", OPTIONGROUP_INPUT);
        options_slider_init(&optionbutton.vsensitivity, &options_settings_copy.vsensitivity, 1, 20, 1, 193, 374, options_bmp_inbetween_small, "Vert. Sensitivity", OPTIONGROUP_INPUT);

        options_checkbox_init(&optionbutton.hinvert, &options_settings_copy.invertX, 600, 328, "Invert Horiz.", OPTIONGROUP_INPUT);
        options_checkbox_init(&optionbutton.vinvert, &options_settings_copy.invertY, 600, 374, "Invert. Vert.", OPTIONGROUP_INPUT);

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
        }

        optionbutton.inputs[middle - 1].slot[0]->neighbour[UIDIR_DOWN] = optionbutton.hsensitivity.decrease;
        optionbutton.inputs[middle - 1].slot[1]->neighbour[UIDIR_DOWN] = optionbutton.hsensitivity.decrease;
        optionbutton.inputs[middle - 1].slot[2]->neighbour[UIDIR_DOWN] = optionbutton.hsensitivity.increase;
        optionbutton.inputs[middle - 1].slot[3]->neighbour[UIDIR_DOWN] = optionbutton.hsensitivity.increase;

        optionbutton.hsensitivity.decrease->neighbour[UIDIR_UP] = optionbutton.inputs[middle - 1].slot[0];
        optionbutton.hsensitivity.increase->neighbour[UIDIR_UP] = optionbutton.inputs[middle - 1].slot[3];

        optionbutton.hsensitivity.decrease->neighbour[UIDIR_DOWN] = optionbutton.vsensitivity.decrease;
        optionbutton.hsensitivity.increase->neighbour[UIDIR_DOWN] = optionbutton.vsensitivity.increase;

        optionbutton.vsensitivity.decrease->neighbour[UIDIR_UP] = optionbutton.hsensitivity.decrease;
        optionbutton.vsensitivity.increase->neighbour[UIDIR_UP] = optionbutton.hsensitivity.increase;
        optionbutton.vsensitivity.decrease->neighbour[UIDIR_DOWN] = optionbutton.cancel;
        optionbutton.vsensitivity.increase->neighbour[UIDIR_DOWN] = optionbutton.cancel;
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

    uisystem_update(options_ui);

    var dy = 0;

    options_selector_update(&optionbutton.resolution);
    options_selector_update(&optionbutton.fpslimit);
    options_selector_update(&optionbutton.anisotropy);

    options_slider_update(&optionbutton.hsensitivity);
    options_slider_update(&optionbutton.vsensitivity);

    options_checkbox_update(&optionbutton.fullscreen);
    options_checkbox_update(&optionbutton.vsync);

    options_checkbox_update(&optionbutton.hinvert);
    options_checkbox_update(&optionbutton.vinvert);


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

    reset(options_txt_fps_limit, SHOW);
    reset(options_txt_anisotropic, SHOW);

    int i;
    for(i = 0; i < INPUT_MAX; i++)
    {
        reset(optionbutton.inputs[i].description, SHOW);
    }

    options_selector_hide(&optionbutton.resolution);
    options_selector_hide(&optionbutton.fpslimit);
    options_selector_hide(&optionbutton.anisotropy);

    options_slider_hide(&optionbutton.hsensitivity);
    options_slider_hide(&optionbutton.vsensitivity);

    options_checkbox_hide(&optionbutton.fullscreen);
    options_checkbox_hide(&optionbutton.vsync);

    options_checkbox_hide(&optionbutton.hinvert);
    options_checkbox_hide(&optionbutton.vinvert);

    uisystem_hide_all(options_ui);
}
