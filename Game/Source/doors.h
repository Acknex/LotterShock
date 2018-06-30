#ifndef DOORS_H
#define DOORS_H

#define KEYCARD_LVL skill1
#define KEYPAD_KEY_ID skill1
#define KEYPAD_ID skill2
#define EXPECTED_KEY skill3

SOUND* snd_beep = "Beep.wav";

STRING* keypad_input = "";
int current_keypad_id = 0;

void keycard();
void keycard_init();
void keycard_update();

void Keypad();
void keypad_event();

#endif