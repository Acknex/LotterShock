#ifndef INPUT_H
#define INPUT_H

#include "ackXinput.h"

#define INPUT_TYPE_NONE     0
#define INPUT_TYPE_KEYBOARD 1
#define INPUT_TYPE_GAMEPAD  2
#define INPUT_TYPE_AXIS     3



typedef struct
{
    int type;
    int button;
    var * pvalue;
    float deadZone;
    float scale;
    bool time_sensitive;
} input_config_t;


typedef struct
{
	int down;
    int justPressed;

    char cinfo[128];

    input_config_t configs[4];

    // analog input
    var value;
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

#define INPUT_SHOW_MAP 15

#define INPUT_MAX 16

INPUT input[INPUT_MAX];

bool input_cheats_enabled = false;

void input_update();

void input_add(int inputID, int inputType, int value);

void input_add_axis(int inputID, var * value, float scale, float deadZone, bool time_sensitive);

void input_init();

bool input_down(int id);

bool input_hit(int id);

bool input_any();

var input_axis(int id);

#endif // INPUT_H
