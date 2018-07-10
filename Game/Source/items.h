#ifndef ITEMS_H
#define ITEMS_H

#define ITEM_MEDIPACK 0
#define ITEM_KEYCARD 1
#define ITEM_AMMUNITION 2
#define ITEM_WEAPON 3
#define ITEM_DOUBLEJUMP 4
#define ITEM_HAZMAT 5

#define ITEM_NOROTATE skill1
#define AMMO_AMOUNT skill2
#define HEALTH_AMOUNT skill2

#define MEDIPACK_HEALTH_PLUS 10

SOUND *medipack_snd = "gothealth.wav";

void collectibles_update();

void Medipack();
void medipack_init();
void keycard();
void doublejump();
void hazmat();
void battery();

void ammo_shotgun();
void ammo_cellgun();
void ammo_flamethrower();

void weapon_sword();
void weapon_shotgun();
void weapon_cellgun();
void weapon_flamethrower();

#endif