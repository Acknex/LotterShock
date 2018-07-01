//keycards
#include "doors.h"
//ammo
#include "weapons.h"

SOUND* snd_catch = "catch.wav";

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
			snd_play(medipack_snd, 100, 0);
			break;
			
		case ITEM_KEYCARD:
			// add to inventory
			keys[item.KEYCARD_KEY_ID] = 1;
			snd_play(snd_catch, 100, 0);
			break;
			
		case ITEM_AMMUNITION:
			// add to inventory
			weapons_add_ammo(item->SUBSYSTEM_skill_a, item->SUBSYSTEM_skill_b);
			snd_play(snd_catch, 100, 0);
			break;
			
		case ITEM_WEAPON:
			// unlock
			weapons_add(item->SUBSYSTEM_skill_a);
			snd_play(snd_catch, 100, 0);
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

/**********
MEDIPACK

	condition: player hurt
**********/
action Medipack() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_MEDIPACK;
}

void medipack_init() {
	ent_create("medipack.mdl", vector(0,0,0), Medipack);
}

/**********
KEY
	condition: none
**********/
action keycard() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_KEYCARD;
}

/**********
AMMO
	condition: ammo not full
**********/

void item_ammo(int ammoType, var amount)
{
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_AMMUNITION;
	my->SUBSYSTEM_skill_a = ammoType;
	my->SUBSYSTEM_skill_b = amount;
}
action ammo_shotgun()
{
	item_ammo(WEAPON_SHOTGUN, 6);
	
}
action ammo_cellgun()
{
	item_ammo(WEAPON_CELLGUN, 30);
}
action ammo_flamethrower()
{
	item_ammo(WEAPON_FLAMETHROWER, 75);
}

/**********
WEAPONS
	condition: none
**********/

void item_weapon(int weaponType)
{
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_WEAPON;
	my->SUBSYSTEM_skill_a = weaponType;
}

action weapon_sword()
{
	item_weapon(WEAPON_SWORD);
}
action weapon_shotgun()
{
	item_weapon(WEAPON_SHOTGUN);
}
action weapon_cellgun()
{
	item_weapon(WEAPON_CELLGUN);
}
action weapon_flamethrower()
{
	item_weapon(WEAPON_FLAMETHROWER);
}
