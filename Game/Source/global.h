#ifndef GLOBAL_H
#define GLOBAL_H

// boolstuff!
typedef int bool;
#define true  1
#define false 0

#define LEVEL_FILE "this-is-the-only-level.wmb"

//! Markiert die Entity als "TOT" und sorgt dafür, dass sie zu Ende des Frames gelöscht wird.
#define SK_ENTITY_DEAD skill89

//! Definiert, welches Subsystem die Entity besitzt.
#define SK_SUBSYSTEM   skill99

#define SUBSYSTEM_FRAMEWORK     1
#define SUBSYSTEM_USABLES       2
#define SUBSYSTEM_CREDITS       3
#define SUBSYSTEM_COLLECTIBLES  4
#define SUBSYSTEM_HUD           5
#define SUBSYSTEM_JOURNAL       6
#define SUBSYSTEM_INPUT         7
#define SUBSYSTEM_MUSIC         8
#define SUBSYSTEM_WEAPONS       9

#define SUBSYSTEM_ENEMY_LERCHE  100
#define SUBSYSTEM_ENEMY_WARLOCK 101
#define SUBSYSTEM_ENEMY_SPUTNIK 103
#define SUBSYSTEM_ENEMY_EYE     104



#define SUBSYSTEM_LOOP(ptr,system) for(ptr = ent_next(NULL); ptr != NULL; ptr = ent_next(ptr)) if(ptr->SK_SUBSYSTEM == (system))

#endif // GLOBAL_H
