#ifndef INPUT_H
#define INPUT_H

#include "ackXinput.h"

typedef struct
{
    var * value;
    float deadZone;
    float scale;
    bool time_sensitive;
} input_axis_t;

typedef struct
{
	int down;
    int justPressed;
	int scanCodes[4];
	int gamepadKeys[4];
    int useAxis;
	float factor;
	char cinfo[32];
    EVENT fnc;

    // analog input
    var value;
    input_axis_t axes[4];
    bool positiveValue;
    float deadZone;
    float sensitivity;
} INPUT;

#define INPUT_UP 0
#define INPUT_DOWN 1
#define INPUT_LEFT 2
#define INPUT_RIGHT 3
#define INPUT_JUMP 4
#define INPUT_USE 5
#define INPUT_ATTACK 6
#define INPUT_WEAPON_UP 7
#define INPUT_WEAPON_DOWN 8
#define INPUT_NAVBACK 9
#define INPUT_BLOCK 10
#define INPUT_CROUCH 11
#define INPUT_MORPHBALL 12

#define INPUT_LOOK_HORIZONTAL 13
#define INPUT_LOOK_VERTICAL 14

#define INPUT_MAX 15

#define INPUT_TYPE_KEYBOARD 0
#define INPUT_TYPE_GAMEPAD 1
#define INPUT_TYPE_MOUSEAXIS 2

INPUT input[INPUT_MAX];

bool input_cheats_enabled = false;

void input_update();

void input_add(int inputID, int inputType, int value);

void input_add_axis(int inputID, var * value, float scale, float deadZone, bool time_sensitive);

void input_init();

bool input_down(int id);

bool input_hit(int id);

var input_axis(int id);

#endif // INPUT_H
