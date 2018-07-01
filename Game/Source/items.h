#ifndef ITEMS_H
#define ITEMS_H

#define ITEM_MEDIPACK 0
#define ITEM_KEYCARD 1
#define ITEM_AMMUNITION 2

#define MEDIPACK_HEALTH_PLUS 10

SOUND *medipack_snd = "gothealth.wav";

void collectibles_update();

void Medipack();
void medipack_init();
void keycard();

#endif