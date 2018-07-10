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
			if(item->HEALTH_AMOUNT > 0)
				playerAddHealth(item->HEALTH_AMOUNT);
			else
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
			
		case ITEM_DOUBLEJUMP:
			// unlock
			playerHasDoubleJump += 1;
			snd_play(snd_catch, 100, 0);
			break;
			
		case ITEM_HAZMAT:
			// unlock
			playerHasHazmat = 1;
			snd_play(snd_catch, 100, 0);
			break;
	}
	// remove me
	item->SK_ENTITY_DEAD = 1;
}



void collectibles_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_COLLECTIBLES) {
		
		if(ptr->skill1 != 1) ptr->pan += time_step * 10;
		
		if (player) {
			if (itemCollectible_condition(ptr)) {
				var distance = vec_dist(ptr.x, player.x);
				if (distance < 120) 
					itemCollectible_effect(ptr);
			}
		}
	}
}

/**********
MEDIPACK

	condition: player hurt
**********/
// uses: ITEM_NOROTATE, HEALTH_AMOUNT
action Medipack() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_MEDIPACK;
	set(my, PASSABLE);
}

void medipack_init() {
	ent_create("medipack.mdl", vector(0,0,0), Medipack);
}

/**********
KEY
	condition: none
**********/
// uses: ITEM_NOROTATE, KEYCARD_KEY_ID
action keycard() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_KEYCARD;
	set(my, PASSABLE);
}
/**********
DOUBLEJUMP
	condition: none
**********/
// uses: ITEM_NOROTATE
action doublejump() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_DOUBLEJUMP;
	set(my, PASSABLE);
}
/**********
HAZMAT SUIT
	condition: none
**********/
// uses: ITEM_NOROTATE
action hazmat() {
	framework_setup(my, SUBSYSTEM_COLLECTIBLES);
	my->SUBSYSTEM_PARAMETER = ITEM_HAZMAT;
	set(my, PASSABLE);
}

/**********
POWER CORE BATTERY
	condition: none
**********/
action battery() {
	// TODO: please fill with code
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
	set(my, PASSABLE);
}
// uses: ITEM_NOROTATE, AMMO_AMOUNT
action ammo_shotgun()
{
	if(my->AMMO_AMOUNT > 0)
		item_ammo(WEAPON_SHOTGUN, my->AMMO_AMOUNT);
	else
		item_ammo(WEAPON_SHOTGUN, 6);
	
}
// uses: ITEM_NOROTATE, AMMO_AMOUNT
action ammo_cellgun()
{
	if(my->AMMO_AMOUNT > 0)
		item_ammo(WEAPON_CELLGUN, my->AMMO_AMOUNT);
	else
		item_ammo(WEAPON_CELLGUN, 30);
}
// uses: ITEM_NOROTATE, AMMO_AMOUNT
action ammo_flamethrower()
{
	if(my->AMMO_AMOUNT > 0)
		item_ammo(WEAPON_FLAMETHROWER, my->AMMO_AMOUNT);
	else
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
	set(my, PASSABLE);
}

// uses: ITEM_NOROTATE
action weapon_sword()
{
	item_weapon(WEAPON_SWORD);
}
// uses: ITEM_NOROTATE
action weapon_shotgun()
{
	item_weapon(WEAPON_SHOTGUN);
}
// uses: ITEM_NOROTATE
action weapon_cellgun()
{
	item_weapon(WEAPON_CELLGUN);
}
// uses: ITEM_NOROTATE
action weapon_flamethrower()
{
	item_weapon(WEAPON_FLAMETHROWER);
}
