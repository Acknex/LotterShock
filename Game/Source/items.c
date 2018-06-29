action medipack() {
	framework_setup(my, SUBSYSTEM_MEDIPACK);
}

void medipack_init() {
	ent_create("medipack.mdl", vector(0,0,0), medipack);
}


void medipack_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_MEDIPACK) {
		ptr->pan += time_step;
		
		if (c_trace(ptr.x, player.x, IGNORE_ME) > 0)) {
			if (you == player) {
				
				if (player) {
					// increase player health
					player->health +=MEDIPACK_HEALTH_PLUS;
					
					// remove me
					ptr->SK_ENTITY_DEAD = 1;
				}
			}
		}
	}
}