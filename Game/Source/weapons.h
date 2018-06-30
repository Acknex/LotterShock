#ifndef WEAPONS_H
#define WEAPONS_H

#define WEAPON_SWORD        1
#define WEAPON_SHOTGUN      2
#define WEAPON_CELLGUN      3
#define WEAPON_FLAMETHROWER 4

void weapons_init();

void weapons_open();

void weapons_update();

void weapons_close();

//! Adds the weapon with the given ID to the arsenal
void weapons_add(int id);

//! Erects the sword and makes it longer.
void weapons_erect_sword();

#endif // WEAPONS_H
