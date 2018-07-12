#ifndef GLOBAL_H
#define GLOBAL_H

// boolstuff!
typedef int bool;
#define true  1
#define false 0

#define LEVEL_FILE "gamelevel.wmb"

#define REGION_SERVERROOM "Serverroom"

//! Markiert die Entity als "TOT" und sorgt dafür, dass sie zu Ende des Frames gelöscht wird.
#define SK_ENTITY_DEAD skill89

//! Definiert, welches Subsystem die Entity besitzt.
#define SK_SUBSYSTEM   skill99

//skill21-39: reserved for specific use
//skill40-49 reserved for shader
//skill50-67 reserved for common use
#define HEALTH skill50
#define DAMAGE_HIT skill51
#define DAMAGE_VEC skill52
//#define DAMAGE_VEC skill53
//#define DAMAGE_VEC skill54
#define INTERACTIBLE skill55
#define SUBSYSTEM_PARAMETER skill56
#define SUBSYSTEM_skill_a skill57
#define SUBSYSTEM_skill_b skill58
#define SUBSYSTEM_skill_c skill59

#define SUBSYSTEM_FRAMEWORK         1
#define SUBSYSTEM_USABLES           2
#define SUBSYSTEM_CREDITS           3
#define SUBSYSTEM_COLLECTIBLES      4
#define SUBSYSTEM_HUD               5
#define SUBSYSTEM_JOURNAL           6
#define SUBSYSTEM_INPUT             7
#define SUBSYSTEM_MUSIC             8
#define SUBSYSTEM_WEAPONS           9
#define SUBSYSTEM_PROJECTILE        10
#define SUBSYSTEM_DOORS             11
#define SUBSYSTEM_KEYPADS           12
#define SUBSYSTEM_ENVIRONMENT       13
#define SUBSYSTEM_BESTIARY          14
#define SUBSYSTEM_ENEMY_PROJECTILE  15
#define SUBSYSTEM_ENEMY_GIB         16
#define SUBSYSTEM_MAINMENU          17
#define SUBSYSTEM_FLESH             18


#define SUBSYSTEM_ENEMY_LERCHE  1000
#define SUBSYSTEM_ENEMY_WARLOCK 1001
#define SUBSYSTEM_ENEMY_SPUTNIK 1003
#define SUBSYSTEM_ENEMY_EYE     1004
#define SUBSYSTEM_ENEMY_TURRET  1005
#define SUBSYSTEM_ENEMY_SKULL   1006

#define FX_WAITTIME				640

#define SUBSYSTEM_LOOP(ptr,system) for(ptr = ent_next(NULL); ptr != NULL; ptr = ent_next(ptr)) if(ptr->SK_SUBSYSTEM == (system))


/* global enemy settings */
#define HEALTH_SKULL		125
#define HEALTH_LERCHE	50
#define HEALTH_SPUTNIK	80
#define HEALTH_EYE		150
#define HEALTH_TURRET	100

#define DAMAGE_SKULL		(10-random(5))
#define DAMAGE_LERCHE	(30-random(15))
#define DAMAGE_SPUTNIK	(10-random(10))
#define DAMAGE_EYE		(30-random(15))
#define DAMAGE_TURRET	(10-random(5))
#define DAMAGE_DEFAULT  (10-random(5))

/* do not modify unless you really know what you do! */
#define GROUP_PLAYER 3
#define GROUP_ENEMY 10

#endif // GLOBAL_H
