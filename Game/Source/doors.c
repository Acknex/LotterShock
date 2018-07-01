action keycard() {
	framework_setup(my, SUBSYSTEM_KEYCARDS);
}

void keycard_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_KEYCARDS) {
		ptr->pan += time_step * 10;
		
				
		if (player) {
			var trace = c_trace(ptr.x, player.x, IGNORE_ME);
			if ((trace > 0) && (trace < 100)) {
				if (you == player) {
					
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
// skill22: WAIT_TIME
// skill23: DOOR_Z_START
action door() {
	my.DOOR_STATE = 0;
	my.DOOR_Z_START = my.z;
	my.WAIT_TIME = 50;
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
						} else {
							ent_playsound(ptr, snd_keypad_no, 100);
						}
					}
				}
			break;
			case 1:
				if (ptr.z < ptr.DOOR_Z_START + DOOR_Z_MAX) {
					ptr.z +=10 * time_step;
	
					if (ptr.z >= ptr.DOOR_Z_START + DOOR_Z_MAX) {
						ptr.DOOR_STATE = 3;
						ptr.z = ptr.DOOR_Z_START + DOOR_Z_MAX;
						ptr.WAIT_TIME = 50;
					}
				}
			break;
			case 2:
				if (ptr.z > ptr.DOOR_Z_START) {
					ptr.z -=10 * time_step;
					
					if (ptr.z <= ptr.DOOR_Z_START) {
						ptr.DOOR_STATE = 0;
						ptr.DOOR_Z_START = ptr.DOOR_Z_START;
					}
				}			
			break;
			case 3:
				if (ptr.WAIT_TIME > 0) {
					ptr.WAIT_TIME -=1 * time_step;
					
					if (ptr.WAIT_TIME <= 0) {
						ptr.DOOR_STATE = 2;
						ptr.WAIT_TIME = 400;
					}
				}
			break;
		}
	}
}