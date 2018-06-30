action Medipack() {
	framework_setup(my, SUBSYSTEM_MEDIPACK);
}

void medipack_init() {
	ent_create("medipack.mdl", vector(0,0,0), Medipack);
}


void medipack_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_MEDIPACK) {
		ptr->pan += time_step;
		
		var trace = c_trace(ptr.x, player.x, IGNORE_ME);
		if ((trace > 0) && (trace < 100)) {
			if (you == player) {
				
				if (player) {
					// increase player health
					//player->health +=MEDIPACK_HEALTH_PLUS;
					
					// remove me
					ptr->SK_ENTITY_DEAD = 1;
				}
			}
		}
	}
}