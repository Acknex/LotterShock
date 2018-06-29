#ifndef GLOBAL_H
#define GLOBAL_H

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

#endif // GLOBAL_H
