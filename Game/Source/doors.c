action Keykard() {
	framework_setup(my, SUBSYSTEM_USABLES);
}

void keycard_init() {
	ent_create("keycard.mdl", vector(0,0,0), keycard);
}

int keycard_lvl = 0;

void keycard_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_USABLES) {
		ptr->pan += time_step;
		
		if (c_trace(ptr.x, player.x, IGNORE_ME) > 0) {
			if (you == player) {
				
				if (player) {
					// add to inventory
					if (ptr.KEYCARD_LVL > keycard_lvl) {
						keycard_lvl = ptr.KEYCARD_LVL;
					}
					
					// remove me
					ptr->SK_ENTITY_DEAD = 1;
				}
			}
		}
	}
}

// skill1: KEY_ID
// skill2: KEYPAD_ID
// skill3: EXPECTED_KEY
action keypad() {	
	my.emask = ENABLE_CLICK | ENABLE_TOUCH | ENABLE_RELEASE;
	my.event = keypad_event;
}

void keypad_event() {
	switch (event_type) {
		
		case EVENT_CLICK:
			ent_playsound(me, snd_beep, 100);
			
			if (my.KEYPAD_ID != current_keypad_id) {
				str_cpy(keypad_input, "");
				current_keypad_id = my.KEYPAD_ID;
			}
			
			if ((my.KEYPAD_KEY_ID >= 0) && (my.KEYPAD_KEY_ID <= 9)) {
				str_cat(keypad_input, str_for_num(NULL, my.KEYPAD_KEY_ID));	
				printf("%s", _chr(keypad_input));
			}
			
			if (my.KEYPAD_KEY_ID == 10) {
				str_cpy(keypad_input, "");
			}
			
			if (my.KEYPAD_KEY_ID == 11) {
				if (str_cmp(keypad_input, str_for_num(NULL, my.EXPECTED_KEY)) == 1) {
					printf("Richtig");
				} else {
					printf("Falsch");
				}
				str_cpy(keypad_input, "");
			}
		break;
		case EVENT_TOUCH:
			my.skin = 2;
		break;
		case EVENT_RELEASE:
			my.skin = 1;
		break;
	}
}