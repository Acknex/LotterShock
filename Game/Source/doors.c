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
action keypad() {
	
	my.emask |= ENABLE_CLICK;
	my.event = keypad_event;
	set(me, PASSABLE);

	while(me) {
		if (mouse_ent == me) {
			my.skin = 2;
		} else {
			my.skin = 1;
		}
		wait(1);
	}
}

void keypad_event() {
	if (event_type == EVENT_CLICK) {
		ent_playsound(me, snd_beep, 100);
		
		if (my.KEYPAD_ID != current_keypad_id) {
			str_trunc(keypad_input, str_len(keypad_input));
		}
		
		if ((my.KEYPAD_KEY_ID >= 0) || (my.KEYPAD_KEY_ID >= 9)) {	
			str_cat(keypad_input, str_for_num(NULL, my.KEYPAD_KEY_ID));	
			printf("%s", _chr(keypad_input));
		}
		
		if (my.KEYPAD_KEY_ID == 10) {
			str_trunc(keypad_input, str_len(keypad_input));
		}
		
		if (my.KEYPAD_KEY_ID == 11) {
			if (str_cmp(keypad_input, "1234") == 1) {
				printf("Richtig");
			} else {
				printf("Falsch");
			}
		}
	}
}