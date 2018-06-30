//////////////////////////////
// input.h
//////////////////////////////

#include "ackXinput.h"

//////////////////////////////

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
} INPUT;

#undef INPUT_MAX

#define INPUT_UP 0
#define INPUT_DOWN 1
#define INPUT_LEFT 2
#define INPUT_RIGHT 3
#define INPUT_JUMP 4
#define INPUT_USE 5
#define INPUT_ATTACK 6
#define INPUT_WEAPON_UP 7
#define INPUT_WEAPON_DOWN 8
#define INPUT_MAX 9
INPUT input[INPUT_MAX];

#define INPUT_TYPE_KEYBOARD 0
#define INPUT_TYPE_GAMEPAD 1
#define INPUT_TYPE_MOUSEAXIS 2

void inputUpdate()
{
	int i,k;

	if(ackXInputGamepadUse) ackXInputGetState3();
	
	for(i = 0; i < INPUT_MAX; i++)
	{
		INPUT *pinput = &input[i];
		char prevDown = pinput->down;
		pinput->down = 0;
		for(k = 0; k < 4; k++) 
		{
		pinput->factor = 1;
			if(pinput->scanCodes[k] != -1) if(key_pressed(pinput->scanCodes[k])) pinput->down = 1;
			if(ackXInputGamepadUse)
			{
				if(pinput->gamepadKeys[k] != -1 && ackXInputGetButtonState3(pinput->gamepadKeys[k])) pinput->down = 1;
			}
			if(pinput->useAxis != -1)
			{
				/*if(pinput->useAxis == 0)
				{
					if(i == INPUT_LEFT) // fucking shit this is hacknex!
					pinput->factor = 1;
				}*/
			}
		}
		if(pinput->down && !prevDown) pinput->justPressed = 1;
		else pinput->justPressed = 0;
	}
}

void inputAdd(int inputID, int inputType, int value)
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

void inputInit()
{
	int i,k;
	

/*typedef struct
{
	char down;
	char justPressed;
	var scanCodes[4];
	var gamepadKeys[4];
	var useAxis;
	float factor;
	char cinfo[32];
	EVENT fnc;
} INPUT;
*/
///*
	
	memset(input, 0, sizeof(INPUT) * INPUT_MAX);
	for(i = 0; i < INPUT_MAX; i++)
	{
		INPUT *pinput = &input[i];
		pinput->down = 0;
		pinput->justPressed = 0;
		for(k = 0; k < 4; k++)
		{
			(pinput->scanCodes)[k] = -1;
			// error(str_printf(NULL, "%d, %d", i, k));
		}
		for(k = 0; k < 4; k++) (pinput->gamepadKeys)[k] = -1;
		pinput->useAxis = -1;
		pinput->factor = 0;
	}
	//*/
	
	
	strcpy(input[INPUT_UP].cinfo,"UP");
	strcpy(input[INPUT_DOWN].cinfo,"DOWN");
	strcpy(input[INPUT_LEFT].cinfo,"LEFT");
	strcpy(input[INPUT_RIGHT].cinfo,"RIGHT");
	strcpy(input[INPUT_JUMP].cinfo,"JUMP");
	strcpy(input[INPUT_USE].cinfo,"USE");
	strcpy(input[INPUT_ATTACK].cinfo,"ATTACK");
	strcpy(input[INPUT_WEAPON_UP].cinfo,"WEAPON_UP");
	strcpy(input[INPUT_WEAPON_DOWN].cinfo,"WEAPON_DOWN");
	
	//////////////////////////////
	// configurable stuff
	
	inputAdd(INPUT_UP,INPUT_TYPE_KEYBOARD,key_for_str("w") ); 
	inputAdd(INPUT_UP,INPUT_TYPE_KEYBOARD,72); 
	inputAdd(INPUT_DOWN,INPUT_TYPE_KEYBOARD,key_for_str("s")); 
	inputAdd(INPUT_LEFT,INPUT_TYPE_KEYBOARD,key_for_str("a")); 
	inputAdd(INPUT_RIGHT,INPUT_TYPE_KEYBOARD,key_for_str("d")); 
	inputAdd(INPUT_JUMP,INPUT_TYPE_KEYBOARD,57); 
	inputAdd(INPUT_USE,INPUT_TYPE_KEYBOARD,key_for_str("e")); 
	inputAdd(INPUT_ATTACK,INPUT_TYPE_KEYBOARD,280); 
	//inputAdd(INPUT_WEAPON_UP,INPUT_TYPE_KEYBOARD,key_for_str(“w”));  // mickey.z
	//inputAdd(INPUT_WEAPON_DOWN,INPUT_TYPE_KEYBOARD,key_for_str(“w”)); 

	int slot = ackXInputGetGamepadNum();
	if(slot >= 0)
	{
		ackXInputGamepadSlot = slot;
		ackXInputGamepadUse = 1;
	}
	else ackXInputGamepadUse = 0;
}

//////////////////////////////
// input.h
//////////////////////////////