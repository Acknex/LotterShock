#include "global.h"
#include "framework.h"
#include "sputnik.h"
#include "splatter.h"
#include "scan.h"
#include "enemy_hit.h"
#include "gib.h"
#include "particle.h"

#define SPUTNIK_RUNSPEED skill1
#define SPUTNIK_TURNSPEED skill2
#define SPUTNIK_ATTACKSPEED skill3
#define SPUTNIK_ATTACKRANGE skill4
#define SPUTNIK_ACTIVEDIST skill10

#define SPUTNIK_ANIMSTATE skill21
#define SPUTNIK_ANIMSTATEATK skill22
#define SPUTNIK_STATE skill23
#define SPUTNIK_RUNSPEEDCUR skill24
#define SPUTNIK_SOUNDHANDLE skill25
#define SPUTNIK_DIDATTACK skill26


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "die"


#define SPUTNIK_STATE_INACTIVE 0
#define SPUTNIK_STATE_WAIT 1
#define SPUTNIK_STATE_FOLLOW 2
#define SPUTNIK_STATE_ATTACK 3
#define SPUTNIK_STATE_DIE 4
#define SPUTNIK_STATE_DEAD 5


BMAP* SPUTNIK_bmapSplatter[5];

SOUND* sputnik_snd_hit1 = "eselslerche_hit1.wav";
SOUND* sputnik_snd_hit2 = "eselslerche_hit2.wav";

SOUND* sputnik_snd_death1 = "eselslerche_death1.wav";
SOUND* sputnik_snd_death2 = "eselslerche_death2.wav";

SOUND* sputnik_snd_idle1 = "eselslerche_idle1.wav";
SOUND* sputnik_snd_idle2 = "eselslerche_idle2.wav";

SOUND* sputnik_snd_spot1 = "eselslerche_spot1.wav";
SOUND* sputnik_snd_spot2 = "eselslerche_spot2.wav";


#define SPUTNIK_RUNSPEED skill1
#define SPUTNIK_TURNSPEED skill2
#define SPUTNIK_ATTACKSPEED skill3
#define SPUTNIK_ACTIVEDIST skill10
action Sputnik()
{
   framework_setup(my, SUBSYSTEM_ENEMY_SPUTNIK);
	if(my->SPUTNIK_RUNSPEED == 0) my->SPUTNIK_RUNSPEED = 12;
	if(my->SPUTNIK_TURNSPEED == 0) my->SPUTNIK_TURNSPEED = 20;
	if(my->SPUTNIK_ATTACKSPEED == 0) my->SPUTNIK_ATTACKSPEED = 5;
	if(my->SPUTNIK_ATTACKRANGE == 0) my->SPUTNIK_ATTACKRANGE = 20;
	if(my->SPUTNIK_ACTIVEDIST == 0) my->SPUTNIK_ACTIVEDIST = 5000;
	my->HEALTH = 80;
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 2);
	set(my, SHADOW);
	c_setminmax(me);
}

void SPUTNIK_GlobalInit()
{
	SPUTNIK_bmapSplatter[0] = bmap_create("splatter_green_01.png");
	SPUTNIK_bmapSplatter[1] = bmap_create("splatter_green_02.png");
	SPUTNIK_bmapSplatter[2] = bmap_create("splatter_green_03.png");
	SPUTNIK_bmapSplatter[3] = bmap_create("splatter_green_04.png");
	SPUTNIK_bmapSplatter[4] = bmap_create("splatter_green_05.png");
}

void SPUTNIK_Init()
{
}

void SPUTNIK_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_SPUTNIK)
	{
		if (player != NULL)
    	{
			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				if (ptr->SPUTNIK_SOUNDHNDLE != 0)
				{
					switch(integer(random(2)))
					{
						case 0: ptr->SPUTNIK_SOUNDHANDLE = snd_play(eselslerche_snd_hit1, 100, 0); break;
						case 1: ptr->SPUTNIK_SOUNDHANDLE = snd_play(eselslerche_snd_hit2, 100, 0); break;
					}
				}
				SPLATTER_splat(&ptr->x, vector(73.0/255.0,159.0/255.0,0.0));
				SPLATTER_explode(10, &ptr->x, 200, SPUTNIK_bmapSplatter, 5);
			}
			
			if (snd_playing(ptr->SPUTNIK_SOUNDHANDLE) == 0)
			{
				ptr->SPUTNIK_SOUNDHANDLE = 0;
			}
			
			switch(ptr->SPUTNIK_STATE)    	
			{
				case SPUTNIK_STATE_INACTIVE:
				{
					SPUTNIK__inactive(ptr);
					break;
				}

				case SPUTNIK_STATE_WAIT:
				{
					SPUTNIK__wait(ptr);
					break;
				}

				case SPUTNIK_STATE_FOLLOW:
				{
					sputnik__follow(ptr);
					break;
				}

				case SPUTNIK_STATE_DIE:
				{
					SPUTNIK__die(ptr);
					break;
				}

				default:
				{
					break;
				}
			}	
		}
	
		if (ptr->SPUTNIK_STATE != SPUTNIK_STATE_DIE && ptr->SPUTNIK_STATE != SPUTNIK_STATE_DEAD)
		{
			VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 100);
			VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
			me = ptr;
			var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON | USE_BOX;
			c_trace(from, to, mode);
			if(HIT_TARGET)
				ptr->z = hit.z - ptr->min_z;
		}
	}	
}

var SPUTNIK__turnToPlayer(ENTITY* ptr)
{
	ANGLE vecAngle;
	VECTOR vecTemp;
	vec_set(&vecTemp, &player->x);
	vec_sub(&vecTemp, &ptr->x);
	vec_to_angle(&vecAngle, &vecTemp);

	if (ang(ptr->pan) < vecAngle.pan - 5)
	{
		ptr->pan = minv(vecAngle.pan, ang(ptr->pan + ptr->SPUTNIK_TURNSPEED * time_step));
		return 0;
	}	
	if (ang(ptr->pan) > vecAngle.pan + 5)
	{
		ptr->pan = maxv(vecAngle.pan, ang(ptr->pan - ptr->SPUTNIK_TURNSPEED * time_step));
		return 0;
	}	
	return 1;
}

void SPUTNIK__inactive(ENTITY* ptr)
{
	/* transitions */
	if(SCAN_IsPlayerNear(ptr, ptr->SPUTNIK_ACTIVEDIST))
	{
    	ptr->SPUTNIK_ANIMSTATE += 10 * time_step;
		ent_animate(ptr, SPUTNIK_WAITANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);
		if (SCAN_IsPlayerInSight(ptr, ptr->SPUTNIK_ACTIVEDIST, 150) || SCAN_IsPlayerNear(ptr, ptr->SPUTNIK_ACTIVEDIST * 0.3))
		{
			ptr->SPUTNIK_STATE = SPUTNIK_STATE_WAIT;
		}
	}
}

void SPUTNIK__wait(ENTITY* ptr)
{
  	ptr->SPUTNIK_ANIMSTATE += 10 * time_step;
	ent_animate(ptr, SPUTNIK_WAITANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);
		
	/* transitions */
	if (SPUTNIK__turnToPlayer(ptr) != 0)
	{
		ptr->SPUTNIK_RUNSPEEDCUR = 0;
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_RUN;
		switch(integer(random(2)))
		{
			case 0: snd_play(sputnik_snd_spot1, 100, 0); break;
			case 1: snd_play(sputnik_snd_spot2, 100, 0); break;
		}
	}
	else if(!SCAN_IsPlayerNear(ptr, ptr->SPUTNIK_ACTIVEDIST + 100))
	{
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_INACTIVE;
	}
}

void SPUTNIK__run(ENTITY* ptr)
{
	ptr->SPUTNIK_RUNSPEEDCUR = minv(ptr->SPUTNIK_RUNSPEEDCUR + ptr->SPUTNIK_RUNSPEED*0.25*time_step, ptr->SPUTNIK_RUNSPEED);
	SPUTNIK__turnToPlayer(ptr);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, vector(ptr->SPUTNIK_RUNSPEEDCUR, 0, 0), nullvector, mode);
	
	// Player is near enough to attack
	if (SCAN_IsPlayerInSight(ptr, ptr->SPUTNIK_ATTACKRANGE, 360))
	{
		ptr->SPUTNIK_ANIMSTATEATK = (ptr->SPUTNIK_ATTACKSPEED * time_step) % 100;
		ptr->SPUTNIK_ANIMSTATE = 0;
		ent_animate(ptr, SPUTNIK_ATTACKANIM, ptr->SPUTNIK_ANIMSTATEATK, ANM_CYCLE);
		
		if (ptr->SPUTNIK_ANIMSTATEATK > 50)
		{
			if (ptr->SPUTNIK_DIDATTACK == 0)
			{
				// Spieler Angreifen
			}
			ptr->SPUTNIK_DIDATTACK = 1;
		}
		else
		{
			ptr->SPUTNIK_DIDATTACK = 0;
		}
	}
	else if ( !SCAN_IsPlayerInSight(ptr, ptr->SPUTNIK_ACTIVEDIST, 90) && !SCAN_IsPlayerNear(ptr, ptr->SPUTNIK_ACTIVEDIST + 100) ) // Player is to far away
	)
	{
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_WAIT;
	}
	else // Still following
	{
		ptr->SPUTNIK_DIDATTACK = 0;
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->SPUTNIK_ANIMSTATE *= ptr->SPUTNIK_RUNSPEED * time_step;
		ent_animate(ptr, SPUTNIK_WALKANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);		
	}

}

void SPUTNIK__die(ENTITY* ptr)
{
	var animState;
	animState = clamp(ptr->SPUTNIK_ANIMSTATE, 0, 50);
	ent_animate(ptr, SPUTNIK_DIEANIM, ptr->SPUTNIK_ANIMSTATE, 0);
	/* transitions */
	if(animState >= 50)
	{
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_DEAD;
		set(ptr, PASSABLE);
	}
}


#ifdef SPUTNIK_TEST
void sputnik_spawn_startup()
{
	
	wait(-10);
	while(1)
	{
		ENTITY* ptr = ent_create("sputnik.mdl", vector(5910,-6050,250), Sputnik);
		wait(-10);
	}
}
#endif