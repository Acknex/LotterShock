#include "input.h"
#include "ackXinput.h"
#include "weapons.h"
#include "dmgsys.h"
#include "movement.h"

#include <windows.h>

//////////////////////////////

SOUND * input_snd_cheat_unlocked = "snd_jingle.ogg";
SOUND * input_snd_cheat_tap = "snd_button_tap.wav";

bool input_down(int id)
{
    return input[id].down;
}

bool input_hit(int id)
{
    return input[id].justPressed;
}

var input_axis(int id)
{
    return input[id].value;
}

void input_add(int inputID, int inputType, int value)
{
    int k;

    INPUT *pinput = &input[inputID]; // no error checks!
    for(k = 0; k < 4; k++)
    {
        switch(inputType)
        {
            case INPUT_TYPE_KEYBOARD:
            if(pinput->scanCodes[k] == -1)
            {
                pinput->scanCodes[k] = value;
                return;
            }
            break;

            case INPUT_TYPE_GAMEPAD:
            if(pinput->gamepadKeys[k] == -1)
            {
                pinput->gamepadKeys[k] = value;
                return;
            }
            break;

            case INPUT_TYPE_MOUSEAXIS:
            pinput->useAxis = value;
            break;

            default:
            error("inputAdd: wow! error!");
        }
    }
}

void input_add_axis(int inputID, var * value, float scale, float deadZone)
{
    int k;

    INPUT *pinput = &input[inputID]; // no error checks!
    for(k = 0; k < 4; k++)
    {
        if(pinput->axes[k].value == NULL)
        {
            pinput->axes[k].value = value;
            pinput->axes[k].deadZone = deadZone;
            pinput->axes[k].scale = scale;
            return;
        }
    }
}

#define INPUT_CHEAT_COUNT 5

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
    memset(cheatcodes, 0, sizeof(cheatcode_t) * INPUT_CHEAT_COUNT);

    if(on_message != input_proc_message)
    {
        input_original_proc_message = on_message;	// store the original message loop
        on_message = input_proc_message; // and replace it by your own
    }

    cheatcodes[0].text = "iddqd";
    cheatcodes[0].trigger = input_cheat_indestructible;

    cheatcodes[1].text = "idkfa";
    cheatcodes[1].trigger = input_cheat_all_weapons;

    cheatcodes[2].text = "idfa";
    cheatcodes[2].trigger = input_cheat_all_die;

    cheatcodes[3].text = "idspispopd";
    cheatcodes[3].trigger = input_cheat_clipmode;

    cheatcodes[4].text = "idnoclip";
    cheatcodes[4].trigger = input_cheat_wallhack;

    int i,k;

    memset(input, 0, sizeof(INPUT) * INPUT_MAX);
    for(i = 0; i < INPUT_MAX; i++)
    {
        INPUT *pinput = &input[i];
        pinput->down = 0;
        pinput->justPressed = 0;
        for(k = 0; k < 4; k++) (pinput->scanCodes)[k] = -1;
        for(k = 0; k < 4; k++) (pinput->gamepadKeys)[k] = -1;
        pinput->useAxis = -1;
        pinput->factor = 0;

        for(k = 0; k < 4; k++)
        {
            pinput->axes[k].value = NULL;
            pinput->axes[k].deadZone = 0;
        }
        pinput->deadZone = 0.1;
        pinput->positiveValue = true;
        pinput->value = 0.0;
        pinput->sensitivity = 1.0;
    }
    //*/

    input[INPUT_DOWN].positiveValue = false;
    input[INPUT_LEFT].positiveValue = false;

    // TODO: Controller + Mouse Sensitivity
    input[INPUT_LOOK_HORIZONTAL].sensitivity = 2.0;
    input[INPUT_LOOK_VERTICAL].sensitivity   = 2.0;

    strcpy(input[INPUT_UP].cinfo,"UP");
    strcpy(input[INPUT_DOWN].cinfo,"DOWN");
    strcpy(input[INPUT_LEFT].cinfo,"LEFT");
    strcpy(input[INPUT_RIGHT].cinfo,"RIGHT");
    strcpy(input[INPUT_JUMP].cinfo,"JUMP");
    strcpy(input[INPUT_USE].cinfo,"USE");
    strcpy(input[INPUT_ATTACK].cinfo,"ATTACK");
    strcpy(input[INPUT_WEAPON_UP].cinfo,"WEAPON_UP");
    strcpy(input[INPUT_WEAPON_DOWN].cinfo,"WEAPON_DOWN");
    strcpy(input[INPUT_NAVBACK].cinfo,"NAVBACK");
    strcpy(input[INPUT_CROUCH].cinfo,"CROUCH");
    strcpy(input[INPUT_MORPHBALL].cinfo,"ENTMORPHBALL");


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

    input_add(INPUT_WEAPON_UP,  INPUT_TYPE_KEYBOARD, key_for_str("q"));  // mickey.z
    input_add(INPUT_WEAPON_DOWN,INPUT_TYPE_KEYBOARD, key_for_str("e"));

    // Controller Mapping:
    input_add(INPUT_UP,         INPUT_TYPE_GAMEPAD, 0  ); //! dpad up
    input_add(INPUT_DOWN,       INPUT_TYPE_GAMEPAD, 1  ); //! dpad down
    input_add(INPUT_LEFT,       INPUT_TYPE_GAMEPAD, 2  ); //! dpad left
    input_add(INPUT_RIGHT,      INPUT_TYPE_GAMEPAD, 3  ); //! dpad right
    input_add(INPUT_USE,        INPUT_TYPE_GAMEPAD, 14 ); //! X
    input_add(INPUT_ATTACK,     INPUT_TYPE_GAMEPAD, 13 ); //! B
    input_add(INPUT_BLOCK,      INPUT_TYPE_GAMEPAD, 15 ); //! Y
    input_add(INPUT_JUMP,       INPUT_TYPE_GAMEPAD, 12 ); //! A
    input_add(INPUT_NAVBACK,    INPUT_TYPE_GAMEPAD, 4  ); //! start
    input_add(INPUT_NAVBACK,    INPUT_TYPE_GAMEPAD, 5  ); //! select
    input_add(INPUT_CROUCH,     INPUT_TYPE_GAMEPAD, 6  ); //! left stick
    input_add(INPUT_MORPHBALL,  INPUT_TYPE_GAMEPAD, 7  ); //! right stick

    input_add(INPUT_WEAPON_UP,  INPUT_TYPE_GAMEPAD, 9); //! right shoulder
    input_add(INPUT_WEAPON_DOWN,INPUT_TYPE_GAMEPAD, 8); //! left shoulder


    input_add_axis(INPUT_LOOK_HORIZONTAL, &mouse_force.x, 3.0, 0.0);
    input_add_axis(INPUT_LOOK_VERTICAL,   &mouse_force.y, 3.0, 0.0);

    input_add_axis(INPUT_LOOK_HORIZONTAL, &input_states.rightStick.x, 1.0 / 255.0, 0.3);
    input_add_axis(INPUT_LOOK_VERTICAL,   &input_states.rightStick.y, 1.0 / 255.0, 0.3);

    input_add_axis(INPUT_LEFT,  &input_states.leftStick.x, 1.0 / 255.0, 0.3);
    input_add_axis(INPUT_RIGHT, &input_states.leftStick.x, 1.0 / 255.0, 0.3);

    input_add_axis(INPUT_UP,    &input_states.leftStick.y, 1.0 / 255.0, 0.3);
    input_add_axis(INPUT_DOWN,  &input_states.leftStick.y, 1.0 / 255.0, 0.3);

    input_add_axis(INPUT_ATTACK, &input_states.rightTrigger, 1.0 / 255.0, 0.3);
    input_add_axis(INPUT_BLOCK,  &input_states.leftTrigger,  1.0 / 255.0, 0.3);

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
            pinput->factor = 1;
            if(pinput->scanCodes[k] != -1)
            {
                if(key_pressed(pinput->scanCodes[k]))
                    pinput->down = 1;
            }
            if(ackXInputGamepadUse)
            {
                if(pinput->gamepadKeys[k] != -1)
                {
                    if(ackXInputGetButtonState3(pinput->gamepadKeys[k]))
                        pinput->down = 1;
                }
            }
            if(pinput->axes[k].value != NULL)
            {
                var val = (*(pinput->axes[k].value)) * pinput->axes[k].scale;
                var ded = pinput->axes[k].deadZone;

                val = sign(val) * clamp(maxv(0, (abs(val) - ded)) / (1.0 - ded), 0.0, 1.0);

                pinput->value += val;
            }
        }

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
