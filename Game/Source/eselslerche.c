#include "global.h"
#include "framework.h"
#include "eselslerche.h"
#include "splatter.h"
#include "scan.h"
#include "enemy_hit.h"
#include "gib.h"
#include "particle.h"

#define EL_RUNSPEED skill1
#define EL_TURNSPEED skill2
#define EL_ANIMSPEED skill3
#define EL_EXPLODEDIST skill4
#define EL_ACTIVEDIST skill5

#define EL_ANIMSTATE skill21
#define EL_ANIMSTATELIM skill22
#define EL_STATE skill23
#define EL_RUNSPEEDCUR skill24
#define EL_EXPLODESTATE skill25
#define EL_HITDIR skill30
#define EL_RAMPAGE skill31


#define EL_WALKANIM "walk"
#define EL_WAITANIM "stand"
#define EL_DIEANIM "dance"
#define EL_TURNANIM "turn"
#define EL_HITANIM "asleepfall"

#define EL_STATE_INACTIVE 0
#define EL_STATE_WAIT 1
#define EL_STATE_RUN 2
#define EL_STATE_EXPLODE 3
#define EL_STATE_DIE 4
#define EL_STATE_DEAD 5
#define EL_STATE_HIT 6

BMAP* EL_bmapSplatter = "splatter_green_01.png";

// uses: EL_RUNSPEED, EL_TURNSPEED, EL_ANIMSPEED, EL_EXPLODEDIST, EL_ACTIVEDIST
action Eselslerche()
{
    framework_setup(my, SUBSYSTEM_ENEMY_LERCHE);
}

void ESELSLERCHE_Init()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_LERCHE)
   {
   	//TODO: useful default values
   	if(ptr->EL_RUNSPEED == 0) ptr->EL_RUNSPEED = 12;
   	if(ptr->EL_TURNSPEED == 0) ptr->EL_TURNSPEED = 10;
   	if(ptr->EL_ANIMSPEED == 0) ptr->EL_ANIMSPEED = 5;
   	if(ptr->EL_EXPLODEDIST == 0) ptr->EL_EXPLODEDIST = 300;
   	if(ptr->EL_ACTIVEDIST == 0) ptr->EL_ACTIVEDIST = 5000;
		ptr->HEALTH = 50;
		ENEMY_HIT_init(ptr);
		vec_scale(&ptr->scale_x, 2);
		set(ptr, SHADOW);
	}	
}

void ESELSLERCHE_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_LERCHE)
	{
		if (player != NULL)
    	{
    		DEBUG_VAR(ptr->EL_STATE, 50);
    		DEBUG_VAR(ptr->EL_ACTIVEDIST, 95);

			ptr->EL_ANIMSTATE += ptr->EL_ANIMSPEED * time_step;
			ptr->EL_ANIMSTATELIM = clamp(ptr->EL_ANIMSTATE, 0, 100);
			ptr->EL_ANIMSTATE = cycle(ptr->EL_ANIMSTATE, 0, 100);

			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				ptr->event = NULL;
				ptr->EL_STATE = EL_STATE_HIT;
				SPLATTER_explode(10, &ptr->x, 200, EL_bmapSplatter);
			}
			
			switch(ptr->EL_STATE)    	
			{
				case EL_STATE_INACTIVE:
				{
					ESELSLERCHE__inactive(ptr);
					break;
				}

				case EL_STATE_WAIT:
				{
					ESELSLERCHE__wait(ptr);
					break;
				}

				case EL_STATE_RUN:
				{
					ESELSLERCHE__run(ptr);
					break;
				}

				case EL_STATE_EXPLODE:
				{
					ESELSLERCHE__explode(ptr);
					break;
				}

				case EL_STATE_DIE:
				{
					ESELSLERCHE__die(ptr);
					break;
				}

				case EL_STATE_HIT:
				{
					ESELSLERCHE__hit(ptr);
					break;
				}

				default:
				{
					break;
				}

			}	
		}

		if (ptr->EL_STATE != EL_STATE_EXPLODE)
		{
			VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 10);
			VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
			me = ptr;
			var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON;
			c_trace(from, to, mode);
			if(HIT_TARGET)
				ptr->z = hit.z - ptr->min_z;
		}
	}	
}

var ESELSLERCHE__turnToPlayer(ENTITY* ptr)
{
	ANGLE vecAngle;
	VECTOR vecTemp;
	vec_set(&vecTemp, &player->x);
	vec_sub(&vecTemp, &ptr->x);
	vec_to_angle(&vecAngle, &vecTemp);

	if (ang(ptr->pan) < vecAngle.pan - 5)
	{
		ptr->pan = minv(vecAngle.pan, ang(ptr->pan + ptr->EL_TURNSPEED * time_step));
		return 0;
	}	
	if (ang(ptr->pan) > vecAngle.pan + 5)
	{
		ptr->pan = maxv(vecAngle.pan, ang(ptr->pan - ptr->EL_TURNSPEED * time_step));
		return 0;
	}	
//	if (integer(ang(ptr->pan)) == integer(vecAngle.pan))
		return 1;
//	else
//		return 0;
}

void ESELSLERCHE__inactive(ENTITY* ptr)
{
	/* transitions */
	if(SCAN_IsPlayerNear(ptr, ptr->EL_ACTIVEDIST))
	{
		ent_animate(ptr, EL_WAITANIM, ptr->EL_ANIMSTATE, ANM_CYCLE);
		if (SCAN_IsPlayerInSight(ptr, ptr->EL_ACTIVEDIST, 90) || SCAN_IsPlayerNear(ptr, ptr->EL_ACTIVEDIST * 0.3))
		{
			ptr->EL_STATE = EL_STATE_WAIT;
		}
	}
}

void ESELSLERCHE__wait(ENTITY* ptr)
{
	ent_animate(ptr, EL_TURNANIM, ptr->EL_ANIMSTATE, ANM_CYCLE);

	/* transitions */
	if (ESELSLERCHE__turnToPlayer(ptr) != 0)
	{
		ptr->EL_ANIMSTATE = 0;
		ptr->EL_RUNSPEEDCUR = 0;
		ptr->EL_STATE = EL_STATE_RUN;
	}
	else if(!SCAN_IsPlayerNear(ptr, ptr->EL_ACTIVEDIST + 100))
	{
		ptr->EL_STATE = EL_STATE_INACTIVE;
	}
	else
	{
	}
}

void ESELSLERCHE__run(ENTITY* ptr)
{
	ptr->EL_RUNSPEEDCUR = minv(ptr->EL_RUNSPEEDCUR + 4*time_step, ptr->EL_RUNSPEED);
	ESELSLERCHE__turnToPlayer(ptr);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, vector(ptr->EL_RUNSPEEDCUR, 0, 0), nullvector, mode);
	ent_animate(ptr, EL_WALKANIM, ptr->EL_ANIMSTATE, ANM_CYCLE);

	/* transitions */
	if (SCAN_IsPlayerInSight(ptr, ptr->EL_EXPLODEDIST, 360))
	{
		ptr->EL_STATE = EL_STATE_EXPLODE;
		set(ptr, PASSABLE);
	}
	else if (
		!SCAN_IsPlayerInSight(ptr, ptr->EL_ACTIVEDIST, 90) 
		&& (!SCAN_IsPlayerNear(ptr, ptr->EL_ACTIVEDIST + 100))
		&& !ptr->EL_RAMPAGE
	)
	{
		ptr->EL_STATE = EL_STATE_WAIT;
		ptr->EL_ANIMSTATE = 0;
	}
	else
	{
		
	}

}

void ESELSLERCHE__explode(ENTITY* ptr)
{
	
	ptr->EL_EXPLODESTATE += time_step;
	if (ptr->EL_EXPLODESTATE < 1)
	{
		vec_sub(&ptr->scale_x, vector(time_step, time_step, time_step));
	}
	else
	{
		vec_add(&ptr->scale_x, vec_scale(vector(time_step, time_step, time_step),2));
	}
	
	/* transitions */
	if(ptr->EL_EXPLODESTATE >= 2.5)
	{
		set(ptr, PASSABLE|INVISIBLE);
		var i;
		for ( i = 0; i < 5; i++)
		{
			GIB_Spawn(&ptr->x);
		}
		SPLATTER_explode(40, &ptr->x, 600, EL_bmapSplatter);
		PARTICLE_explode(50, &ptr->x);
		ptr->EL_STATE = EL_STATE_DEAD;
		ptr->SK_ENTITY_DEAD = 1;
	}
}

void ESELSLERCHE__die(ENTITY* ptr)
{
	var animState;
	animState = clamp(ptr->EL_ANIMSTATE, 0, 50);
	ent_animate(ptr, EL_DIEANIM, ptr->EL_ANIMSTATE, 0);

	/* transitions */
	if(animState >= 50)
	{
		ptr->EL_STATE = EL_STATE_DEAD;
		set(ptr, PASSABLE);
	}
}

void ESELSLERCHE__hit(ENTITY* ptr)
{
	var animState;
	animState = clamp(ptr->EL_ANIMSTATE, 0, 90);
	var animMirror;
	if (animState <=45)
		animMirror = animState;
	else
		animMirror = 90 - animState;
	ent_animate(ptr, EL_HITANIM, animMirror, 0);
	
	VECTOR dir;
	vec_set(&dir, ptr->DAMAGE_VEC);
	vec_scale(&dir, 1.2*time_step);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, nullvector, dir, mode);

	/* transitions */
	if (ptr->HEALTH <= 0)
	{
		ptr->EL_STATE = EL_STATE_DIE;
	}
	else if (animState >= 90)
	{
		ptr->EL_STATE = EL_STATE_RUN;			
		ptr->EL_RAMPAGE = 1;
		ptr->event = ENEMY_HIT_event;
		ptr->EL_ANIMSTATE = 0;
		ptr->DAMAGE_VEC = nullvector;
	}
	else
	{
		
	}
}

