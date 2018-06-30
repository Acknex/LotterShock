#ifndef DOORS_H
#define DOORS_H

// keycards
#define KEYCARD_KEY_ID skill1

// keypads
#define KEYPAD_KEY_ID skill1
#define KEYPAD_ID skill2
#define KEYPAD_EXPECTED_KEY skill3
#define KEYPAD_KEY_ID_TO_ENABLE skill4

// doors
#define DOOR_REQUIRED_KEY_ID skill1
#define DOOR_STATE skill21
#define MOVEMENT_STAGE skill22

// keys
#define KEYS_MAX 5
#define RED_KEY 0
#define BLUE_KEY 1
#define GREEN_KEY 2
#define YELLOW_KEY 3
#define BLACK_KEY 4

SOUND* snd_beep = "Beep.wav";
SOUND* snd_keypad_yes = "keypadYes.wav";
SOUND* snd_keypad_no = "keypadNo.wav";
SOUND* snd_gate = "gate.wav";

STRING* keypad_input = "";
int current_keypad_id = 0;

int keys[KEYS_MAX] = {0, 0, 0, 0, 0};

/*void Keycard();
void keycard_update();*/

void Door();
void doors_update();

void keypad();
void keypad_update();

#endif