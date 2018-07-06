 #include "input.h"
#include "ackXinput.h"
#include "weapons.h"
#include "dmgsys.h"
#include "movement.h"
#include "settings.h"
#include "environmentals.h"

#include <windows.h>
#include <acknex.h>

//////////////////////////////

VECTOR mickey_smoothed;

var input_states[INPUT_AXIS_MAX];

INPUT input[INPUT_MAX];

bool input_down(int id)
{
    if(id < 0 || id >= INPUT_MAX) error("input_down: unknown button!");
    return input[id].down;
}

bool input_hit(int id)
{
    if(id < 0 || id >= INPUT_MAX) error("input_hit: unknown button!");
    return input[id].justPressed;
}

bool input_any()
{
	int i;
    for(i = 0; i <= INPUT_MAX; ++i)
    {
		if(input[i].down)
			return true;
    }
	return false;
}

var input_axis(int id)
{
    if(id < 0 || id >= INPUT_AXIS_MAX) error("input_axis: unknown axis!");
    return input_states[id];
}

void input_add(int inputID, int inputType, int value)
{
    int k;
    if(inputType == INPUT_TYPE_AXIS)
        error("wrong type!");

    INPUT *pinput = &input[inputID]; // no error checks!
    for(k = 0; k < 4; k++)
    {
        input_config_t * cfg = &pinput->configs[k];
        if(cfg->type != INPUT_TYPE_NONE)
            continue;
        cfg->type = inputType;
        cfg->index = value;
        break;
    }
}

void input_add_axis(int inputID, int axisid, float deadZone, bool inverted)
{
    int k;

    INPUT *pinput = &input[inputID]; // no error checks!
    for(k = 0; k < 4; k++)
    {
        input_config_t * cfg = &pinput->configs[k];
        if(cfg->type != INPUT_TYPE_NONE)
            continue;
        cfg->type = INPUT_TYPE_AXIS;
        cfg->index = axisid;
        cfg->deadZone = deadZone;
        cfg->inverted = inverted;
        break;
    }
}


void input_init()
{
#ifdef DEBUG
    // Remap debug console
    on_grave = on_tab;
    on_tab = NULL;
#endif

    int i,k;

    memset(input, 0, sizeof(INPUT) * INPUT_MAX);
    for(i = 0; i < INPUT_MAX; i++)
    {
        INPUT *pinput = &input[i];
        pinput->down = 0;
        pinput->justPressed = 0;
        for(k = 0; k < 4; k++)
        {
            pinput->configs[k].type = INPUT_TYPE_NONE;
            pinput->configs[k].deadZone = 0;
        }
        pinput->deadZone = 0.1;
        pinput->value = 0.0;
        pinput->sensitivity = 1.0;
    }
    //*/

    strcpy(input[INPUT_BLOCK].cinfo,"Block");
    strcpy(input[INPUT_UP].cinfo,"Walk (Forward)");
    strcpy(input[INPUT_DOWN].cinfo,"Walk (Back)");
    strcpy(input[INPUT_LEFT].cinfo,"Walk (Left)");
    strcpy(input[INPUT_RIGHT].cinfo,"Walk (Right)");
    strcpy(input[INPUT_JUMP].cinfo,"Jump");
    strcpy(input[INPUT_USE].cinfo,"Use");
    strcpy(input[INPUT_ATTACK].cinfo,"Attack");
    strcpy(input[INPUT_WEAPON_UP].cinfo,"Next Weapon");
    strcpy(input[INPUT_WEAPON_DOWN].cinfo,"Previous Weapon");
    strcpy(input[INPUT_CROUCH].cinfo,"Crouch");
    strcpy(input[INPUT_NAVBACK].cinfo,"Navigate Back");
    strcpy(input[INPUT_MORPHBALL].cinfo,"Toggle Morphball");
    strcpy(input[INPUT_SHOW_MAP].cinfo, "Toggle Map");


    //////////////////////////////
    // configurable stuff

    // Keyboard Mapping
    input_add(INPUT_UP,         INPUT_TYPE_KEYBOARD, key_for_str("w") );
    input_add(INPUT_UP,         INPUT_TYPE_KEYBOARD, 72);
    input_add(INPUT_DOWN,       INPUT_TYPE_KEYBOARD, key_for_str("s"));
    input_add(INPUT_DOWN,       INPUT_TYPE_KEYBOARD, 80);
    input_add(INPUT_LEFT,       INPUT_TYPE_KEYBOARD, key_for_str("a"));
    input_add(INPUT_LEFT,       INPUT_TYPE_KEYBOARD, 75);
    input_add(INPUT_RIGHT,      INPUT_TYPE_KEYBOARD, key_for_str("d"));
    input_add(INPUT_RIGHT,      INPUT_TYPE_KEYBOARD, 77);
    input_add(INPUT_JUMP,       INPUT_TYPE_KEYBOARD, 57);
    input_add(INPUT_USE,        INPUT_TYPE_KEYBOARD, key_for_str("e"));
    input_add(INPUT_USE,        INPUT_TYPE_KEYBOARD, 28);
    input_add(INPUT_ATTACK,     INPUT_TYPE_KEYBOARD, 280);
    input_add(INPUT_BLOCK,      INPUT_TYPE_KEYBOARD, 281);
    input_add(INPUT_NAVBACK,    INPUT_TYPE_KEYBOARD, key_for_str("esc"));
    input_add(INPUT_CROUCH,     INPUT_TYPE_KEYBOARD, key_for_str("ctrl"));
    input_add(INPUT_MORPHBALL,  INPUT_TYPE_KEYBOARD, key_for_str("shiftl"));
    input_add(INPUT_SHOW_MAP,   INPUT_TYPE_KEYBOARD, 15); // TAB

    // input_add(INPUT_WEAPON_UP,  INPUT_TYPE_KEYBOARD, key_for_str("q"));  // mickey.z
    // input_add(INPUT_WEAPON_DOWN,INPUT_TYPE_KEYBOARD, key_for_str("e"));

    // Controller Mapping:
    input_add(INPUT_UP,         INPUT_TYPE_GAMEPAD, XINPUT_DPAD_UP );
    input_add(INPUT_DOWN,       INPUT_TYPE_GAMEPAD, XINPUT_DPAD_DOWN );
    input_add(INPUT_LEFT,       INPUT_TYPE_GAMEPAD, XINPUT_DPAD_LEFT );
    input_add(INPUT_RIGHT,      INPUT_TYPE_GAMEPAD, XINPUT_DPAD_RIGHT );
    input_add(INPUT_USE,        INPUT_TYPE_GAMEPAD, XINPUT_X );
    input_add(INPUT_ATTACK,     INPUT_TYPE_GAMEPAD, XINPUT_B );
    input_add(INPUT_BLOCK,      INPUT_TYPE_GAMEPAD, XINPUT_Y );
    input_add(INPUT_JUMP,       INPUT_TYPE_GAMEPAD, XINPUT_A );
    input_add(INPUT_NAVBACK,    INPUT_TYPE_GAMEPAD, XINPUT_START );
    input_add(INPUT_SHOW_MAP,   INPUT_TYPE_GAMEPAD, XINPUT_SELECT );
    input_add(INPUT_CROUCH,     INPUT_TYPE_GAMEPAD, XINPUT_LEFT_STICK );
    input_add(INPUT_MORPHBALL,  INPUT_TYPE_GAMEPAD, XINPUT_RIGHT_STICK );

    input_add(INPUT_WEAPON_UP,  INPUT_TYPE_GAMEPAD, XINPUT_RIGHT_BUTTON);
    input_add(INPUT_WEAPON_DOWN,INPUT_TYPE_GAMEPAD, XINPUT_LEFT_BUTTON);

    input_add_axis(INPUT_WEAPON_UP,   INPUT_AXIS_MOUSEWHEEL, 0.0, false);
    input_add_axis(INPUT_WEAPON_DOWN, INPUT_AXIS_MOUSEWHEEL, 0.0, true);

    input_add_axis(INPUT_LEFT,  INPUT_AXIS_LEFT_STICK_X, 0.3, true);
    input_add_axis(INPUT_RIGHT, INPUT_AXIS_LEFT_STICK_X, 0.3, false);

    input_add_axis(INPUT_UP,    INPUT_AXIS_LEFT_STICK_Y, 0.3, false);
    input_add_axis(INPUT_DOWN,  INPUT_AXIS_LEFT_STICK_Y, 0.3, true);

    input_add_axis(INPUT_ATTACK, INPUT_AXIS_RIGHT_TRIGGER, 0.3, false);
    input_add_axis(INPUT_BLOCK,  INPUT_AXIS_LEFT_TRIGGER,  0.3, false);

    int slot = ackXInputGetGamepadNum();
    if(slot >= 0)
    {
        ackXInputGamepadSlot = slot;
        ackXInputGamepadUse = 1;
    }
    else ackXInputGamepadUse = 0;
}

var input_deadzone(var value, var deadzone)
{
    if(abs(value) < deadzone)
        return 0.0;
    return value;
}

void input_update()
{
    int i,k;

    mickey_smoothed.x += (mickey.x - mickey_smoothed.x) * 0.75;
    mickey_smoothed.y += (mickey.y - mickey_smoothed.y) * 0.75;

    if(!mickey.x && abs(mickey_smoothed.x) < 0.01)
        mickey_smoothed.x = 0;

    if(!mickey.y && abs(mickey_smoothed.y) < 0.01)
        mickey_smoothed.y = 0;
  

    for(i = 0; i < INPUT_AXIS_MAX; i++)
        input_states[i] = 0;

    input_states[INPUT_AXIS_MOUSE_X]    = mickey_smoothed.x;
    input_states[INPUT_AXIS_MOUSE_Y]    = mickey_smoothed.y;
    input_states[INPUT_AXIS_MOUSEWHEEL] = mickey.z / 120.0;

    if(ackXInputGamepadUse)
    {
        ackXInputGetState3();

        input_states[INPUT_AXIS_LEFT_STICK_X]  = ackXInputGetThumbState3(0, 0) / 255.0;
        input_states[INPUT_AXIS_LEFT_STICK_Y]  = ackXInputGetThumbState3(0, 1) / 255.0;
        input_states[INPUT_AXIS_RIGHT_STICK_X] = ackXInputGetThumbState3(1, 0) / 255.0;
        input_states[INPUT_AXIS_RIGHT_STICK_Y] = ackXInputGetThumbState3(1, 1) / 255.0;
        input_states[INPUT_AXIS_LEFT_TRIGGER]  = ackXInputGetTriggerState3(0) / 255.0;
        input_states[INPUT_AXIS_RIGHT_TRIGGER] = ackXInputGetTriggerState3(1) / 255.0;
    }

    { // refresh combined axis manually

        var input_axis_hori = 0;
        var input_axis_vert = 0;

        input_axis_hori += mickey_smoothed.x / 40.0;
        input_axis_vert -= mickey_smoothed.y / 40.0;

        input_axis_hori += input_deadzone(input_states[INPUT_AXIS_RIGHT_STICK_X], 0.3) * time_step;
        input_axis_vert += input_deadzone(input_states[INPUT_AXIS_RIGHT_STICK_Y], 0.3) * time_step;

        input_axis_hori *= settings.input_sensitivity;
        input_axis_vert *= settings.input_sensitivity;

        input_states[INPUT_AXIS_LOOK_HORIZONTAL] = input_axis_hori;
        input_states[INPUT_AXIS_LOOK_VERTICAL]   = input_axis_vert;
    }

    for(i = 0; i < INPUT_MAX; i++)
    {
        INPUT *pinput = &input[i];

        char prevDown = pinput->down;

        pinput->down = 0;
        pinput->value = 0;

        for(k = 0; k < 4; k++)
        {
            input_config_t * cfg = &pinput->configs[k];
            switch(cfg->type)
            {
                case INPUT_TYPE_KEYBOARD:
                    if(key_pressed(cfg->index))
                        pinput->down = 1;
                    break;

                case INPUT_TYPE_GAMEPAD:
                    if(!ackXInputGamepadUse)
                        break;

                    if(ackXInputGetButtonState3(cfg->index))
                        pinput->down = 1;

                    break;

                case INPUT_TYPE_AXIS:
                {
                    var val = input_deadzone(input_states[cfg->index], cfg->deadZone);
                    if(cfg->inverted)
                        val = -val;

                    pinput->value += maxv(0.0, val);
                    break;
                }
            }
        }

        pinput->value *= pinput->sensitivity;
        pinput->down |= (pinput->value > 0.3);

        if(pinput->down && !prevDown)
            pinput->justPressed = 1;
        else
            pinput->justPressed = 0;
    }
}
