///////////////////////////////
// XINPUT structs

#define XINPUT_DPAD_UP      0
#define XINPUT_DPAD_DOWN    1
#define XINPUT_DPAD_LEFT    2
#define XINPUT_DPAD_RIGHT   3
#define XINPUT_START        4
#define XINPUT_SELECT       5
#define XINPUT_LEFT_STICK   6
#define XINPUT_RIGHT_STICK  7
#define XINPUT_LEFT_BUTTON  8
#define XINPUT_RIGHT_BUTTON 9
#define XINPUT_A            12
#define XINPUT_B            13
#define XINPUT_X            14
#define XINPUT_Y            15

typedef struct _XINPUT_GAMEPAD
{
	WORD                                wButtons;
	byte                                bLeftTrigger;
	byte                                bRightTrigger;
	short                               sThumbLX; // short is signed in lite-C, unsigned in C++
	short                               sThumbLY;
	short                               sThumbRX;
	short                               sThumbRY;
} XINPUT_GAMEPAD;

typedef struct _XINPUT_STATE
{
	DWORD                               dwPacketNumber;
	XINPUT_GAMEPAD                      Gamepad;
} XINPUT_STATE;

XINPUT_STATE xinput_states[4]; // global variables/ objects <3
int ackXInputGamepadSlot = 0;
int ackXInputGamepadUse = 0;

///////////////////////////////
// ackXinput DLL prototypes

/*
* Call this function to check for connected gamepads as well as button and trigger states.
* 
* id: 		Gamepad id, from 0 to 3.
* state: 	Pointer to XINPUT_STATE object which saves all gamepad information.
*				state.dwPacketNumber increases when any button or trigger state has changed.
* 
* return:	1 if a gamepad in slot "id" was found.
*/
var ackXInputGetState(var id, XINPUT_STATE* state);

/*
* Force feedback.
* 
* id: 		Gamepad id, from 0 to 3.
* left: 	Left (low-frequency) vibration motor, from 0 to 100.
* right: 	Right (high-frequency) vibration motor, from 0 to 100.
* 
*/
void ackXInputSetVibration(var id, var left, var right);

///////////////////////////////
// ackXinput convenience functions

/*
* Retrieve button state. Includes the XBox 360 Digipad but not the analog triggers (LT and RT).
* 
* state: 	Pointer to XINPUT_STATE object which contains all gamepad information.
* bid: 		Button id, from 0 to 15.
* 
* return:	1 if button pressed, 0 otherwise.
*/
var ackXInputGetButtonState(XINPUT_STATE* state, var bid);

/*
* Retrieve analog trigger state.
* 
* state: 	Pointer to XINPUT_STATE object which contains all gamepad information.
* right: 	0 left trigger, 1 right trigger.
* 
* return: 	0-255, 255 fully pressed.
*/
var ackXInputGetTriggerState(XINPUT_STATE* state, var right);

/*
* Retrieve analog trigger state.
* 
* state: 	Pointer to XINPUT_STATE object which contains all gamepad information.
* right: 	0 left thumb, 1 right thumb.
* y: 		0 x axis, 1 y axis.
* 
* return: 	0-255, 255 fully pressed.
*/
var ackXInputGetThumbState(XINPUT_STATE* state, var right, var y);

/*
* Count connected XINPUT devices. Uses the global xinput_states[4] array.
* 
* return: 	Greater than 0 if gamepad found.
*/
var ackXInputEnumConnected();

/*
* Find first connected gamepad, if any.
* 
* return: 	-1 if no gamepad found, first active gamepad slot otherwise (0..3).
*/
var ackXInputGetGamepadNum();















///////////////////////////////
// ackXinput convenience functions - implementation

var ackXInputGetButtonState(XINPUT_STATE* state, var bid)
{
	var key;
	
	key = !!((state->Gamepad).wButtons&(1<<bid));
	
	return key;
}

var ackXInputGetTriggerState(XINPUT_STATE* state, var right)
{
	var trigger;
	
	if(right) trigger = (state->Gamepad).bRightTrigger;
	else trigger = (state->Gamepad).bLeftTrigger;
	//trigger = (trigger-127.5)*2;
	
	return trigger;
}

var ackXInputGetThumbState(XINPUT_STATE* state, var right, var y)
{
	var thumb;
	
	if(right)
	{
		if(y) thumb = (state->Gamepad).sThumbRY;
		else thumb = (state->Gamepad).sThumbRX;
	}
	else
	{
		if(y) thumb = (state->Gamepad).sThumbLY;
		else thumb = (state->Gamepad).sThumbLX;
	}
	
	thumb = (thumb&~(1<<15))-(thumb&(1<<15));
	thumb = thumb/128.50196;
	
	return thumb;
}

var ackXInputEnumConnected()
{
	var i,num = 0;
	
	for(i = 0; i < 4; i++)
	{
		if(ackXInputGetState(i,xinput_states[i]) > 0) num++;
	}
	
	return num;
}

var ackXInputGetGamepadNum()
{
	var i,num = -1;
	
	for(i = 0; i < 4; i++)
	{
		if(ackXInputGetState(i,xinput_states[i]) > 0) return i;
	}
	
	return -1;
}

var ackXInputGetState3()
{
	return (ackXInputGetState(0, xinput_states[0]));
}

var ackXInputGetButtonState3(var bid)
{
	return (ackXInputGetButtonState(xinput_states[ackXInputGamepadSlot],bid));
}

var ackXInputGetTriggerState3(var right)
{
	return (ackXInputGetTriggerState(xinput_states[ackXInputGamepadSlot],right));
}

var ackXInputGetThumbState3(var right, var y)
{
	return (ackXInputGetThumbState(xinput_states[ackXInputGamepadSlot],right, y));
}
