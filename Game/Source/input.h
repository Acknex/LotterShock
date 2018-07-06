#ifndef INPUT_H
#define INPUT_H

#include "ackXinput.h"

#define INPUT_TYPE_NONE     0
#define INPUT_TYPE_KEYBOARD 1
#define INPUT_TYPE_GAMEPAD  2
#define INPUT_TYPE_AXIS     3

// THESE ARE BUTTONS
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
#define INPUT_SHOW_MAP 13
#define INPUT_MAX 14

// THESE ARE AXIS
#define INPUT_AXIS_LOOK_HORIZONTAL 0
#define INPUT_AXIS_LOOK_VERTICAL   1
#define INPUT_AXIS_MOUSE_X         2
#define INPUT_AXIS_MOUSE_Y         3
#define INPUT_AXIS_MOUSEWHEEL      4
#define INPUT_AXIS_LEFT_STICK_X    5
#define INPUT_AXIS_LEFT_STICK_Y    6
#define INPUT_AXIS_RIGHT_STICK_X   7
#define INPUT_AXIS_RIGHT_STICK_Y   8
#define INPUT_AXIS_LEFT_TRIGGER    9
#define INPUT_AXIS_RIGHT_TRIGGER   10
#define INPUT_AXIS_MAX             11

typedef struct
{
    int type;
    int index;
    float deadZone;
    bool inverted;
} input_config_t;

typedef struct
{
	int down;
    int justPressed;

    char cinfo[128];

    input_config_t configs[4];

    // analog input
    var value;
    float sensitivity;
} INPUT;

INPUT input[INPUT_MAX];

void input_update();

//! adds a button input
void input_add(int inputID, int inputType, int value);

//! adds a button input for an axis
void input_add_axis(int inputID, int axis, float deadZone, bool inverted);

void input_init();

bool input_down(int id);

bool input_hit(int id);

bool input_any();

var input_axis(int id);

#endif // INPUT_H
