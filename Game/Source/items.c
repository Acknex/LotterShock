//keycards
#include "doors.h"
//ammo
#include "weapons.h"

bool itemCollectible_condition(ENTITY *item)
{
	switch(item->SUBSYSTEM_PARAMETER)
	{
		case ITEM_MEDIPACK:
			return playerGetHealth() < playerGetMaxHealth();
		case ITEM_AMMUNITION:
			int id = item->SUBSYSTEM_skill_a;
			return weapons_get_ammo(id) < weapons_get_max_ammo(id);
	}
	return true;
}


void itemCollectible_effect(ENTITY *item)
{
	switch(item->SUBSYSTEM_PARAMETER)
	{
		case ITEM_MEDIPACK:
			// increase player health
			playerAddHealth(MEDIPACK_HEALTH_PLUS);
			ent_playsound(item, medipack_snd, 100);
			break;
			
		case ITEM_KEYCARD:
			// add to inventory
			keys[item.KEYCARD_KEY_ID] = 1;
			ent_playsound(player, snd_beep, 100);
			break;
			
		case ITEM_AMMUNITION:
			// add to inventory
			weapons_add_ammo(item->SUBSYSTEM_skill_a, item->SUBSYSTEM_skill_b);
			ent_playsound(player, snd_beep, 100);
			break;
	}
	// remove me
	item->SK_ENTITY_DEAD = 1;
}



void collectibles_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_COLLECTIBLES) {
		ptr->pan += time_step * 10;
		
		if (player) {
			if (itemCollectible_condition(ptr)) {
				var trace = c_trace(ptr.x, player.x, IGNORE_ME);
				if ((trace > 0) && (trace < 100)) {
					if (you == player) {
						itemCollectible_effect(ptr);
					}
				}
			}
		}
	}
}

action Medipack() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_MEDIPACK;
}

void medipack_init() {
	ent_create("medipack.mdl", vector(0,0,0), Medipack);
}

action keycard() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_KEYCARD;
}

void item_ammo(int ammoType, var amount)
{
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_AMMUNITION;
	my->SUBSYSTEM_skill_a = ammoType;
	my->SUBSYSTEM_skill_b = amount;
}

action shotgun_ammo()
{
	item_ammo(WEAPON_SHOTGUN, 6);
	
}
action cellgun_ammo()
{
	item_ammo(WEAPON_CELLGUN, 30);
}
action flamethrower_ammo()
{
	item_ammo(WEAPON_FLAMETHROWER, 75);
}