#include "enemy.h"
#include "movement.h"

#define bulletSpeed skill20
#define bulletLifeTime skill21
#define bulletDamage skill22

#define ENEMY_BULLETLIFETIME 96
#define ENEMY_BULLETSPEED 25

void ENEMY__projectileEvent();

action enemy_projectile()
{
	my->emask |= ENABLE_IMPACT | ENABLE_SHOOT | ENABLE_ENTITY;
	my->bulletSpeed = ENEMY_BULLETSPEED;
	my->event = ENEMY__projectileEvent;
	my->bulletLifeTime = ENEMY_BULLETLIFETIME;
	vec_set(my->blue, vector(0,0,255));
	set(my, LIGHT);
	my->bulletDamage = DAMAGE_DEFAULT;
   framework_setup(my, SUBSYSTEM_ENEMY_PROJECTILE);
}

void ENEMY_setDamage(ENTITY* ent, var damage)
{
	ent->bulletDamage = damage;
}

void ENEMY__projectileEvent()
{
	if ((event_type == EVENT_IMPACT) || (event_type == EVENT_SHOOT))
	{
		my->SK_ENTITY_DEAD = 1;
	}
}

void ENEMY_UpdateProjectile()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_PROJECTILE)
	{
		var vFlags = IGNORE_ME | IGNORE_PASSABLE | ACTIVATE_SHOOT;
		ptr->bulletLifeTime -= time_step;
		VECTOR* to = vector(-ptr->bulletSpeed,0,0);
		vec_rotate (to, ptr->pan);
		vec_scale (to, time_step);
		vec_add(to, ptr->x);
		
		var vDist = c_trace(ptr.x, to, vFlags);
		if((vDist == 0) && (ptr->bulletLifeTime > 0)) 
		{
			
			VECTOR* vecMove = vector(-ptr->bulletSpeed, 0, 0);
			vec_rotate(vecMove, ptr->pan);
			vec_scale(vecMove, time_step);
			vec_add(ptr->x, vecMove);
		}
		else
		{
			ptr->SK_ENTITY_DEAD = 1;
		}
		if(SCAN_IsPlayerNear(ptr, 150))
		{
			playerAddHealth(-ptr->bulletDamage);
			ptr->SK_ENTITY_DEAD = 1;
		}
	}
}
