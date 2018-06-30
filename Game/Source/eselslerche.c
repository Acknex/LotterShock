#include "global.h"
#include "framework.h"
#include "eselslerche.h"
#include "splatter.h"
#include "scan.h"

#define EL_RUNSPEED skill1
#define EL_TURNSPEED skill2
#define EL_ANIMSPEED skill3
#define EL_EXPLODEDIST skill4
#define EL_ACTIVEDIST skill5
#define EL_ANIMSTATE skill21
#define EL_STATE skill22
#define EL_WALKANIM "walk"
#define EL_WAITANIM "stand"


#define EL_STATE_INACTIVE 0
#define EL_STATE_WAIT 1
#define EL_STATE_RUN 2
#define EL_STATE_EXPLODE 3

BMAP* EL_bmapSplatter = "blutlache.tga";

// uses: skill1, skill2
action Eselslerche()
{
    framework_setup(my, SUBSYSTEM_ENEMY_LERCHE);
}

void ESELSLERCHE_Init()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_LERCHE)
   {
   	if(ptr->EL_RUNSPEED == 0) ptr->EL_RUNSPEED = 0;
   	if(ptr->EL_TURNSPEED == 0) ptr->EL_TURNSPEED = 0;
   	if(ptr->EL_ANIMSPEED == 0) ptr->EL_ANIMSPEED = 0;
   	if(ptr->EL_EXPLODEDIST == 0) ptr->EL_EXPLODEDIST = 0;
   	if(ptr->EL_ACTIVEDIST == 0) ptr->EL_ACTIVEDIST = 0;
	}	
}

void ESELSLERCHE_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_LERCHE)
	{
		if (player != NULL)
    	{
			switch(ptr->EL_STATE)    	
			{
				case EL_STATE_INACTIVE:
				{
					break;
				}

				case EL_STATE_WAIT:
				{
					ESELSLERCHE__turnToPlayer(ptr);
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

				default:
				{
					break;
				}

			}	
		}
	}	
}

var ESELSLERCHE__turnToPlayer(ENTITY* ptr)
{
	var targetPan = ang(player->pan + 180);
	if (ang(ptr->pan) < targetPan)
	{
		ptr->pan = minv(targetPan, ang(ptr->pan + ptr->EL_TURNSPEED * time_step));
	}	
	if (ang(ptr->pan) > targetPan)
	{
		ptr->pan = maxv(targetPan, ang(ptr->pan - ptr->EL_TURNSPEED * time_step));
	}	
	
	if (ang(ptr->pan) == targetPan)
		return 1;
	else
		return 0;
}

void ESELSLERCHE__inactive(ENTITY* ptr)
{
	if(SCAN_IsPlayerNear(ptr, ptr->EL_ACTIVEDIST) > 0)
	{
		if (SCAN_IsPlayerInSight(ptr, ptr->EL_ACTIVEDIST, 60) > 0)
		{
			ptr->EL_STATE = EL_STATE_WAIT;
		}
	}
}

void ESELSLERCHE__wait(ENTITY* ptr)
{
	if (ESELSLERCHE__turnToPlayer(ptr) != 0)
	{
		ptr->EL_STATE = EL_STATE_RUN;
	}
}

void ESELSLERCHE__run(ENTITY* ptr)
{
	ESELSLERCHE__turnToPlayer(ptr);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE;
	c_move(ptr, vector(0, ptr->EL_RUNSPEED, 0), nullvector, mode);
	ptr->EL_ANIMSTATE = cycle(ptr->EL_ANIMSTATE + ptr->EL_ANIMSPEED * time_step, 0, 100);
	ent_animate(ptr, EL_WALKANIM, ptr->EL_ANIMSTATE, ANM_CYCLE);
	if (SCAN_IsPlayerInSight(ptr, ptr->EL_EXPLODEDIST, 360))
	{
		ptr->EL_STATE = EL_STATE_EXPLODE;
	}
}

void ESELSLERCHE__explode(ENTITY* ptr)
{
	SPLATTER_explode(50, &ptr->x, ptr->EL_EXPLODEDIST, EL_bmapSplatter);
}
