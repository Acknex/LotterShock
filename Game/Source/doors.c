action Keycard() {
	framework_setup(my, SUBSYSTEM_USABLES);
}

void keycard_init() {
	ent_create("keycard.mdl", vector(0,0,0), Keycard);
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
	framework_setup(my, SUBSYSTEM_USABLES);
	set(me, PASSABLE);
}

void keycard_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_USABLES) {
		
		draw_text("asd", 10, 10, COLOR_RED);
		if (mouse_ent == ptr) {
			ptr.skin = 2;
			if (input_hit(INPUT_USE)) {
				ent_playsound(ptr, snd_beep, 100);
				
				if (ptr.KEYPAD_ID != current_keypad_id) {
					str_cpy(keypad_input, "");
					current_keypad_id = ptr.KEYPAD_ID;
				}
				
				if ((ptr.KEYPAD_KEY_ID >= 0) && (ptr.KEYPAD_KEY_ID <= 9)) {
					str_cat(keypad_input, str_for_num(NULL, ptr.KEYPAD_KEY_ID));	
				}
				
				if (ptr.KEYPAD_KEY_ID == 10) {
					str_cpy(keypad_input, "");
				}
				
				if (ptr.KEYPAD_KEY_ID == 11) {
					if (str_cmp(keypad_input, str_for_num(NULL, ptr.EXPECTED_KEY)) == 1) {
						ent_playsound(ptr, snd_keypad_yes, 100);
					} else {
						ent_playsound(ptr, snd_keypad_no, 100);
					}
					str_cpy(keypad_input, "");
				}
			}
		} else {
			ptr->skin = 1;
		}
	
	}
}