#include "global.h"
#include "framework.h"
#include "sputnik.h"
#include "splatter.h"
#include "scan.h"
#include "enemy_hit.h"
#include "gib.h"
#include "particle.h"
#include "ang.h"

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
#define SPUTNIK_SPLATTERTHRESHOLD skill27


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "Die"


#define SPUTNIK_STATE_INACTIVE 0
#define SPUTNIK_STATE_WAIT 1
#define SPUTNIK_STATE_FOLLOW 2
#define SPUTNIK_STATE_ATTACK 3
#define SPUTNIK_STATE_DIE 4
#define SPUTNIK_STATE_DEAD 5
#define SPUTNIK_STATE_HIT 6

#define SPUTNIK_FEET 30


BMAP* SPUTNIK_bmapSplatter[5];

//SOUND* sputnik_snd_hit1 = "eselslerche_hit1.wav";
//SOUND* sputnik_snd_hit2 = "eselslerche_hit2.wav";

SOUND* sputnik_snd_death = "sputnik_death.wav";

//SOUND* sputnik_snd_idle1 = "eselslerche_idle1.wav";
//SOUND* sputnik_snd_idle2 = "eselslerche_idle2.wav";

//SOUND* sputnik_snd_spot1 = "eselslerche_spot1.wav";
//SOUND* sputnik_snd_spot2 = "eselslerche_spot2.wav";

SOUND* sputnik_snd_attack1 = "sputnik_attack1.wav";
SOUND* sputnik_snd_attack2 = "sputnik_attack2.wav";
SOUND* sputnik_snd_attack3 = "sputnik_attack3.wav";


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
	if(my->SPUTNIK_ATTACKRANGE == 0) my->SPUTNIK_ATTACKRANGE = 300;
	if(my->SPUTNIK_ACTIVEDIST == 0) my->SPUTNIK_ACTIVEDIST = 6000;
	my->SPUTNIK_ANIMSTATEATK = 100;
	my->HEALTH = HEALTH_SPUTNIK;
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 1.2);
	set(my, SHADOW);
	c_setminmax(me);
	my->min_z += SPUTNIK_FEET;
}

void SPUTNIK_GlobalInit()
{
	SPUTNIK_bmapSplatter[0] = bmap_create("splatter_purple_01.png");
	SPUTNIK_bmapSplatter[1] = bmap_create("splatter_purple_02.png");
	SPUTNIK_bmapSplatter[2] = bmap_create("splatter_purple_03.png");
	SPUTNIK_bmapSplatter[3] = bmap_create("splatter_purple_04.png");
	SPUTNIK_bmapSplatter[4] = bmap_create("splatter_purple_05.png");
	int i;
	for (i= 0; i < 5; i++)
	{
		bmap_preload(SPUTNIK_bmapSplatter[i]);
	}
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
			
			if (ptr->HEALTH > 0)
			{
				SPUTNIK__hitcheck(ptr);
			}
			
    		//DEBUG_VAR(ptr->DAMAGE_HIT, 240);
			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->event = NULL;
				ptr->SPUTNIK_SPLATTERTHRESHOLD = 7;
				//DEBUG_VAR(ptr->DAMAGE_HIT, 260);
				/* SPUTNIK has no hit sounds
				if (ptr->SPUTNIK_SOUNDHANDLE != 0)
				{
					switch(integer(random(2)))
					{
						case 0: ptr->SPUTNIK_SOUNDHANDLE = snd_play(sputnik_snd_hit1, 100, 0); break;
						case 1: ptr->SPUTNIK_SOUNDHANDLE = snd_play(sputnik_snd_hit2, 100, 0); break;
					}
				}
				*/
				
				ptr->push = -100;

				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				SPLATTER_explode(20, ptr, 500, SPUTNIK_bmapSplatter, 5);
				SPLATTER_splat(&ptr->x, vector(100.0/255.0, 67.0/255.0, 192.0/255.0));

				if (ptr->HEALTH <= 0)
				{
					ptr->SPUTNIK_STATE = SPUTNIK_STATE_DIE;
					ptr->SPUTNIK_ANIMSTATE = 0;
					snd_play(sputnik_snd_death, 100, 0);
				}
			}
			
			if (ptr->SPUTNIK_SOUNDHANDLE && (snd_playing(ptr->SPUTNIK_SOUNDHANDLE) == 0))
			{
				snd_remove(ptr->SPUTNIK_SOUNDHANDLE);
				ptr->SPUTNIK_SOUNDHANDLE = 0;
			}
			
			//DEBUG_VAR(ptr->SPUTNIK_STATE, 220);
			
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
					SPUTNIK__follow(ptr);
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
	
		//DEBUG_VAR(2, 280);
		if (ptr->SPUTNIK_STATE != SPUTNIK_STATE_DIE && ptr->SPUTNIK_STATE != SPUTNIK_STATE_DEAD)
		{
			VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 100);
			VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
			me = ptr;
			var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_BOX;
			ptr.min_x += 8;
			ptr.min_y += 8;
			ptr.max_x -= 8;
			ptr.max_y -= 8;
			c_trace(from, to, mode);
			ptr.min_x -= 8;
			ptr.min_y -= 8;
			ptr.max_x += 8;
			ptr.max_y += 8;
			if (HIT_TARGET)
			{
				var newZ = hit.z - ptr->min_z + SPUTNIK_FEET;
				if (ptr->z < newZ)
					ptr->z = minv(ptr->z + 17* time_step, newZ);
				else
					ptr->z = maxv(ptr->z - 17* time_step, newZ);
			}
		}
		
	}	
}

void SPUTNIK__hitcheck(ENTITY* ptr)
{
	if (ptr->SPUTNIK_SPLATTERTHRESHOLD > 0)
	{
		ptr->SPUTNIK_SPLATTERTHRESHOLD -= time_step;
		
		if (ptr->SPUTNIK_SPLATTERTHRESHOLD <= 0)
		{
			ptr->SPUTNIK_SPLATTERTHRESHOLD = 0;
			ptr->event = ENEMY_HIT_event;
			vec_zero(ptr->DAMAGE_VEC);
		}
	}
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
	if(ANG_turnToPlayer(ptr, ptr->SPUTNIK_TURNSPEED, 5) != 0)
	{
		ptr->SPUTNIK_RUNSPEEDCUR = 0;
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_FOLLOW;
		/* SPUTNIK has no spot sounds
		switch(integer(random(2)))
		{
			case 0: snd_play(sputnik_snd_spot1, 100, 0); break;
			case 1: snd_play(sputnik_snd_spot2, 100, 0); break;
		}
		*/
	}
	else if(!SCAN_IsPlayerNear(ptr, ptr->SPUTNIK_ACTIVEDIST + 100))
	{
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_INACTIVE;
	}
}

void SPUTNIK__follow(ENTITY* ptr)
{
	ptr->SPUTNIK_RUNSPEEDCUR = ptr->SPUTNIK_RUNSPEED; //minv(ptr->SPUTNIK_RUNSPEEDCUR + ptr->SPUTNIK_RUNSPEED*0.25*time_step, ptr->SPUTNIK_RUNSPEED);
	ANG_turnToPlayer(ptr, ptr->SPUTNIK_TURNSPEED, 5);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE;
	ptr.min_x -= 2;
	ptr.min_y -= 2;
	ptr.max_x += 2;
	ptr.max_y += 2;
	c_move(ptr, vector(ptr->SPUTNIK_RUNSPEEDCUR, 0, 0), nullvector, mode);
	ptr.min_x += 2;
	ptr.min_y += 2;
	ptr.max_x -= 2;
	ptr.max_y -= 2;
	
	// Player is near enough to attack
	if (SCAN_IsPlayerInSight(ptr, ptr->SPUTNIK_ATTACKRANGE, 360))
	{
		if ((ptr->SPUTNIK_ANIMSTATEATK == 0) || (ptr->SPUTNIK_ANIMSTATEATK >= 100))
		{
			ptr->SPUTNIK_ANIMSTATEATK = 0;
			switch(integer(random(2)))
			{
				case 0: snd_play(sputnik_snd_attack1, 100, 0); break;
				case 1: snd_play(sputnik_snd_attack2, 100, 0); break;
				case 2: snd_play(sputnik_snd_attack3, 100, 0); break;
			}
		}
		ptr->SPUTNIK_ANIMSTATEATK += ptr->SPUTNIK_ATTACKSPEED * time_step;
		ptr->SPUTNIK_ANIMSTATEATK = minv(ptr->SPUTNIK_ANIMSTATEATK, 100);
		ptr->SPUTNIK_ANIMSTATE = 0;
		ent_animate(ptr, SPUTNIK_ATTACKANIM, ptr->SPUTNIK_ANIMSTATEATK, 0);
		
		//DEBUG_VAR(ptr->SPUTNIK_ANIMSTATEATK,300);
		if (ptr->SPUTNIK_ANIMSTATEATK > 50)
		{
			if (ptr->SPUTNIK_DIDATTACK == 0)
			{
				playerAddHealth(-10-random(10));
			}
			ptr->SPUTNIK_DIDATTACK = 1;
		}
		else
		{
			ptr->SPUTNIK_DIDATTACK = 0;
		}
	}
	else if ( !SCAN_IsPlayerInSight(ptr, ptr->SPUTNIK_ATTACKRANGE, 75) && SCAN_IsPlayerBehind(ptr, 1200) ) // Player is to far away
	{
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_WAIT;
	}
	else // Still following
	{
		ptr->SPUTNIK_DIDATTACK = 0;
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->SPUTNIK_ANIMSTATE += ptr->SPUTNIK_RUNSPEED * time_step;
		ent_animate(ptr, SPUTNIK_WALKANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);		
	}

}

void SPUTNIK__die(ENTITY* ptr)
{
	ptr->SPUTNIK_ANIMSTATE += 5 * time_step;
	ent_animate(ptr, SPUTNIK_DIEANIM, ptr->SPUTNIK_ANIMSTATE, 0);
	/* transitions */
	if(ptr->SPUTNIK_ANIMSTATE >= 90)
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
		ENTITY* ptr = ent_create("sputnik.mdl", vector(5920 - 500 + random(1000), -6050 - 500 + random(1000), 250), Sputnik);
		wait(-10);
	}
	ENTITY* ptr = ent_create("sputnik.mdl", vector(6500,-6050,250), Sputnik);
	
}
#endif