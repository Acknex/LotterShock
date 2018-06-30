#include <acknex.h>
#include <default.c>

#define PRAGMA_PATH "../Game/Assets"

BMAP* cursor = "cursor.bmp";

action touch() {
	while(me) {
		if (mouse_ent == me) {
			my.skin = 2;
		} else {
			my.skin = 1;
		}
		wait(1);
	}
}

void main() {
	level_load("");
	wait(1);
	
	mouse_mode = 4;
	ENTITY* keypad1Btn = ent_create("keypadkey1.mdl", vector(-7, 0.5, 5), touch);
	ENTITY* keypad2Btn = ent_create("keypadkey2.mdl", vector(-2, 0.5, 5), touch);
	ENTITY* keypad3Btn = ent_create("keypadkey3.mdl", vector(3, 0.5, 5), touch);
	
	ENTITY* keypad4Btn = ent_create("keypadkey4.mdl", vector(-7, 0.5, 1), touch);
	ENTITY* keypad5Btn = ent_create("keypadkey5.mdl", vector(-2, 0.5, 1), touch);
	ENTITY* keypad6Btn = ent_create("keypadkey6.mdl", vector(3, 0.5, 1), touch);
	
	ENTITY* keypad7Btn = ent_create("keypadkey7.mdl", vector(-7, 0.5, -3), touch);
	ENTITY* keypad8Btn = ent_create("keypadkey8.mdl", vector(-2, 0.5, -3), touch);
	ENTITY* keypad9Btn = ent_create("keypadkey9.mdl", vector(3, 0.5, -3), touch);
	
	ENTITY* keypadCBtn = ent_create("keypadkeyC.mdl", vector(-7, 0.5, -7), touch);
	ENTITY* keypad0Btn = ent_create("keypadkey0.mdl", vector(-2, 0.5, -7), touch);
	ENTITY* keypadOKBtn = ent_create("keypadkeyOK.mdl", vector(3, 0.5, -7), touch);
	
	vec_set(camera.x, vector(-1, -58, 0));
	vec_set(camera.pan, vector(90, 0, 0));
	
	mouse_map = cursor;	
}