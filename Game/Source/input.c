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

SOUND * input_snd_cheat_unlocked = "snd_jingle.ogg";
SOUND * input_snd_cheat_tap = "snd_button_tap.wav";

VECTOR mickey_smoothed;

bool input_down(int id)
{
    return input[id].down;
}

bool input_hit(int id)
{
    return input[id].justPressed;
}

bool input_any()
{
	int i;
	for(i = 0; i<=INPUT_MORPHBALL; ++i)
		if(input[i].down)
			return true;
	return false;
}

var input_axis(int id)
{
    return input[id].value;
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
        cfg->button = value;
        break;
    }
}

void input_add_axis(int inputID, var * value, float scale, float deadZone, bool time_sensitive)
{
    int k;

    INPUT *pinput = &input[inputID]; // no error checks!
    for(k = 0; k < 4; k++)
    {
        input_config_t * cfg = &pinput->configs[k];
        if(cfg->type != INPUT_TYPE_NONE)
            continue;
        cfg->type = INPUT_TYPE_AXIS;

        cfg->pvalue = value;
        cfg->deadZone = deadZone;
        cfg->scale = scale;
        cfg->time_sensitive = time_sensitive;
        break;
    }
}

#define INPUT_CHEAT_COUNT 7

typedef struct cheatcode_t
{
    int position;
    char const * text;
    void * trigger;
} cheatcode_t;

cheatcode_t cheatcodes[INPUT_CHEAT_COUNT];

void input_reset_cheats()
{
    int i;
    for(i = 0; i < INPUT_CHEAT_COUNT; i++)
    {
        cheatcodes[i].position = 0;
    }
}

void input_put_cheat_char(char c)
{
    if(!input_cheats_enabled)
    {
        input_reset_cheats();
        return;
    }

    int i;
    bool any = false;
    for(i = 0; i < INPUT_CHEAT_COUNT; i++)
    {
        if((cheatcodes[i].text)[cheatcodes[i].position] == c)
        {
            cheatcodes[i].position += 1;
            if((cheatcodes[i].text)[cheatcodes[i].position] == '\0')
            {
                void (*trigger)();
                trigger = cheatcodes[i].trigger;
                if(trigger == NULL)
                    error("input(cheats): cheat code does not have a trigger!");
                trigger();

                // cheat war erfolgreich: alle cheateingaben zurücksetzen
                input_reset_cheats();
                snd_play(input_snd_cheat_unlocked, 100, 0);
                break;
            }
            else
            {
                any = true;
            }
        }
        else
        {
            cheatcodes[i].position = 0;
        }
    }
    if(any)
        snd_play(input_snd_cheat_tap, 20, random(200) - 100);
}

LRESULT CALLBACK input_original_proc_message(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK input_proc_message(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   if (WM_CHAR == message)
   {
       input_put_cheat_char((char)wParam);
   }
   return input_original_proc_message(hwnd,message,wParam,lParam);
}

void input_cheat_indestructible()
{
    movement_cheat_invincibility = !movement_cheat_invincibility;
}

void input_cheat_all_weapons()
{
    weapons_add(WEAPON_SWORD);
    weapons_add(WEAPON_SHOTGUN);
    weapons_add(WEAPON_CELLGUN);
    weapons_add(WEAPON_FLAMETHROWER);
}

void input_cheat_weapons_and_keys()
{
    weapons_add(WEAPON_SWORD);
    weapons_add(WEAPON_SHOTGUN);
    weapons_add(WEAPON_CELLGUN);
    weapons_add(WEAPON_FLAMETHROWER);
    var i;
    for(i = 0; i < 5; ++i)
    {
        keys[i] = 1;
    }
    story_enginesEnabled = 1;
}

void input_cheat_suicide()
{
    playerAddHealth(-999);
}

void input_cheat_all_die()
{
    ENTITY * it;
    for(it = ent_next(NULL); it != NULL; it = ent_next(it))
    {
        if(it->SK_SUBSYSTEM < 1000)
            continue;

        it->DAMAGE_HIT += 25000;
    }
}

void input_cheat_wallhack()
{
    ENTITY * it;
    for(it = ent_next(NULL); it != NULL; it = ent_next(it))
    {
        if(it->SK_SUBSYSTEM < 1000)
            continue;

        toggle(it, ZNEAR);
    }
}

void input_cheat_clipmode()
{
    movement_cheat_clipmode = !movement_cheat_clipmode;
}

typedef struct inputstate_t
{
    VECTOR leftStick;
    VECTOR rightStick;
    var leftTrigger;
    var rightTrigger;
} inputstate_t;

inputstate_t input_states;

void input_init()
{
#ifdef DEBUG
    // Remap debug console
    on_grave = on_tab;
    on_tab = NULL;
#endif

    memset(cheatcodes, 0, sizeof(cheatcode_t) * INPUT_CHEAT_COUNT);

    if(on_message != input_proc_message)
    {
        input_original_proc_message = on_message;	// store the original message loop
        on_message = input_proc_message; // and replace it by your own
    }

    cheatcodes[0].text = "iddqd";
    cheatcodes[0].trigger = input_cheat_indestructible;

    cheatcodes[1].text = "idfa";
    cheatcodes[1].trigger = input_cheat_all_weapons;

    cheatcodes[2].text = "idkill";
    cheatcodes[2].trigger = input_cheat_all_die;

    cheatcodes[3].text = "idfly";
    cheatcodes[3].trigger = input_cheat_clipmode;

    cheatcodes[4].text = "idnoclip";
    cheatcodes[4].trigger = input_cheat_wallhack;

    cheatcodes[5].text = "idkfa";
    cheatcodes[5].trigger = input_cheat_weapons_and_keys;

    cheatcodes[6].text = "idseppuku";
    cheatcodes[6].trigger = input_cheat_suicide;

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
        pinput->positiveValue = true;
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
    strcpy(input[INPUT_LOOK_HORIZONTAL].cinfo,"Camera (Horizontal)");
    strcpy(input[INPUT_LOOK_VERTICAL].cinfo,"Camera (Vertical)");
    strcpy(input[INPUT_SHOW_MAP].cinfo, "Toggle Map");


    //////////////////////////////
    // configurable stuff

    // Keyboard Mapping
    input_add(INPUT_UP,         INPUT_TYPE_KEYBOARD, key_for_str("w") );
    input_add(INPUT_UP,         INPUT_TYPE_KEYBOARD, 72);
    input_add(INPUT_DOWN,       INPUT_TYPE_KEYBOARD, key_for_str("s"));
    input_add(INPUT_LEFT,       INPUT_TYPE_KEYBOARD, key_for_str("a"));
    input_add(INPUT_RIGHT,      INPUT_TYPE_KEYBOARD, key_for_str("d"));
    input_add(INPUT_JUMP,       INPUT_TYPE_KEYBOARD, 57);
    input_add(INPUT_USE,        INPUT_TYPE_KEYBOARD, key_for_str("f"));
    input_add(INPUT_ATTACK,     INPUT_TYPE_KEYBOARD, 280);
    input_add(INPUT_BLOCK,      INPUT_TYPE_KEYBOARD, 281);
    input_add(INPUT_NAVBACK,    INPUT_TYPE_KEYBOARD, key_for_str("esc"));
    input_add(INPUT_CROUCH,     INPUT_TYPE_KEYBOARD, key_for_str("ctrl"));
    input_add(INPUT_MORPHBALL,  INPUT_TYPE_KEYBOARD, key_for_str("shiftl"));
    input_add(INPUT_SHOW_MAP,   INPUT_TYPE_KEYBOARD, 15); // TAB

    input_add(INPUT_WEAPON_UP,  INPUT_TYPE_KEYBOARD, key_for_str("q"));  // mickey.z
    input_add(INPUT_WEAPON_DOWN,INPUT_TYPE_KEYBOARD, key_for_str("e"));

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

    input_add_axis(INPUT_WEAPON_UP,  &mickey.z, 1.0, 0.0, false);
    input_add_axis(INPUT_WEAPON_DOWN,&mickey.z, 1.0, 0.0, false);

    input_add_axis(INPUT_LOOK_HORIZONTAL, &mickey_smoothed.x, 1.0/40.0, 0.0, false);
    input_add_axis(INPUT_LOOK_VERTICAL,   &mickey_smoothed.y, -1.0/40.0, 0.0, false);

	input_add_axis(INPUT_LOOK_HORIZONTAL, &input_states.rightStick.x, 1.0 / 255.0, 0.3, true);	
	input_add_axis(INPUT_LOOK_VERTICAL,   &input_states.rightStick.y, 1.0 / 255.0, 0.3, true);

    input_add_axis(INPUT_LEFT,  &input_states.leftStick.x, 1.0 / 255.0, 0.3, false);
    input_add_axis(INPUT_RIGHT, &input_states.leftStick.x, 1.0 / 255.0, 0.3, false);

    input_add_axis(INPUT_UP,    &input_states.leftStick.y, 1.0 / 255.0, 0.3, false);
    input_add_axis(INPUT_DOWN,  &input_states.leftStick.y, 1.0 / 255.0, 0.3, false);

    input_add_axis(INPUT_ATTACK, &input_states.rightTrigger, 1.0 / 255.0, 0.3, false);
    input_add_axis(INPUT_BLOCK,  &input_states.leftTrigger,  1.0 / 255.0, 0.3, false);

    // axis configuration
    input[INPUT_DOWN].positiveValue = false;
    input[INPUT_LEFT].positiveValue = false;

    input[INPUT_WEAPON_UP].positiveValue   = true;
    input[INPUT_WEAPON_DOWN].positiveValue = false;

    // TODO: Controller + Mouse Sensitivity
    input[INPUT_LOOK_HORIZONTAL].sensitivity = settings.input_sensitivity;
    input[INPUT_LOOK_VERTICAL].sensitivity   = settings.input_sensitivity;

    int slot = ackXInputGetGamepadNum();
    if(slot >= 0)
    {
        ackXInputGamepadSlot = slot;
        ackXInputGamepadUse = 1;
    }
    else ackXInputGamepadUse = 0;
}


void input_update()
{
    int i,k;

	mickey_smoothed.x += (mickey.x-mickey_smoothed.x)*0.75;
	mickey_smoothed.y += (mickey.y-mickey_smoothed.y)*0.75;
	if(!mickey.x && abs(mickey_smoothed.x) < 0.01) mickey_smoothed.x = 0;
	if(!mickey.y && abs(mickey_smoothed.y) < 0.01) mickey_smoothed.y = 0;
  
    if(ackXInputGamepadUse)
    {
        ackXInputGetState3();

        input_states.leftStick.x = ackXInputGetThumbState3(0, 0);
        input_states.leftStick.y = ackXInputGetThumbState3(0, 1);
        input_states.rightStick.x = ackXInputGetThumbState3(1, 0);
        input_states.rightStick.y = ackXInputGetThumbState3(1, 1);
        input_states.leftTrigger = ackXInputGetTriggerState3(0);
        input_states.rightTrigger = ackXInputGetTriggerState3(1);
    }
    else
    {
        vec_zero(input_states.leftStick);
        vec_zero(input_states.rightStick);
        input_states.leftTrigger = 0;
        input_states.rightTrigger = 0;
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
                    if(key_pressed(cfg->button))
                        pinput->down = 1;
                    break;

                case INPUT_TYPE_GAMEPAD:
                    if(!ackXInputGamepadUse)
                        break;

                    if(ackXInputGetButtonState3(cfg->button))
                        pinput->down = 1;

                    break;

                case INPUT_TYPE_AXIS:
                {
                    var val = (*(cfg->pvalue)) * cfg->scale;
                    var ded = cfg->deadZone;

                    if(abs(val) < ded)
                        val = 0;

                    if(cfg->time_sensitive)
                        pinput->value += val * time_step;
                    else
                        pinput->value += val;
                    break;
                }
            }
        }

        pinput->value *= pinput->sensitivity;

        if(pinput->positiveValue)
        {
            pinput->down |= (pinput->value > 0.3);
        }
        else
        {
            pinput->down |= (pinput->value < -0.3);
        }

        if(pinput->down && !prevDown)
            pinput->justPressed = 1;
        else
            pinput->justPressed = 0;
    }
}
