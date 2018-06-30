action keycard() {
	framework_setup(my, SUBSYSTEM_KEYCARDS);
}

int keycard_lvl = 0;

void keycard_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_KEYCARDS) {
		ptr->pan += time_step * 10;
		
		var trace = c_trace(ptr.x, player.x, IGNORE_ME);
		if ((trace > 0) && (trace < 100)) {
			if (you == player) {
				
				if (player) {
					
					ent_playsound(player, snd_beep, 100);
					
					// add to inventory
					keys[ptr.KEYCARD_KEY_ID] = 1;
					
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
	framework_setup(my, SUBSYSTEM_KEYPADS);
	set(me, PASSABLE);
}

void keypad_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_KEYPADS) {
		
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
					if (str_cmp(keypad_input, str_for_num(NULL, ptr.KEYPAD_EXPECTED_KEY)) == 1) {
						ent_playsound(ptr, snd_keypad_yes, 100);
						keys[ptr.KEYPAD_KEY_ID_TO_ENABLE] = 1;
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


// skill1: KEY_ID
// skill21: DOOR_STATE (0 = NONE, 1 = OPENING, 2 = CLOSING, 3 = WAITING)
// skill22: MOVEMENT_STAGE
action door() {
	my.DOOR_STATE = 0;
	my.MOVEMENT_STAGE = 0;
	framework_setup(my, SUBSYSTEM_DOORS);
}

void doors_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_DOORS) {
		
		switch(ptr.DOOR_STATE) {
			case 0:
				if (mouse_ent == ptr) {
					if (input_hit(INPUT_USE)) {
						
						if (keys[ptr.DOOR_REQUIRED_KEY_ID] == 1) {
							ent_playsound(ptr, snd_gate, 100);
							ptr.DOOR_STATE = 1;
							ptr.MOVEMENT_STAGE = 400;
						} else {
							ent_playsound(ptr, snd_keypad_no, 100);
						}
					}
				}
			break;
			case 1:
				if (ptr.MOVEMENT_STAGE > 0) {
					ptr.MOVEMENT_STAGE -=10 * time_step;
					ptr.z +=10 * time_step;
					
					if (ptr.MOVEMENT_STAGE <= 0) {
						ptr.DOOR_STATE = 3;
						ptr.MOVEMENT_STAGE = 50;
					}
				}
			break;
			case 2:
				if (ptr.MOVEMENT_STAGE > 0) {
					ptr.MOVEMENT_STAGE -=10 * time_step;
					ptr.z -=10 * time_step;
					
					if (ptr.MOVEMENT_STAGE <= 0) {
						ptr.DOOR_STATE = 0;
						ptr.MOVEMENT_STAGE = 0;
					}
				}			
			break;
			case 3:
				if (ptr.MOVEMENT_STAGE > 0) {
					ptr.MOVEMENT_STAGE -=1 * time_step;
					
					if (ptr.MOVEMENT_STAGE <= 0) {
						ptr.DOOR_STATE = 2;
						ptr.MOVEMENT_STAGE = 400;
					}
				}
			break;
		}
	}
}