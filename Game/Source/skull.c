#include "global.h"
#include "framework.h"
#include "skull.h"
#include "scan.h"
#include "enemy_hit.h"

#define SKL_RUNSPEED skill1
#define SKL_TURNSPEED skill2
#define SKL_ATTACKDIST skill4
#define SKL_ACTIVEDIST skill5

#define SKL_COUNTER skill22
#define SKL_STATE skill23
#define SKL_RUNSPEEDCUR skill24
#define SKL_HITDIR skill30
#define SK_LASTPOS skill32
//#define SK_LASTPOS skill33
//#define SK_LASTPOS skill34
#define SKL_ZOFFSET skill35


#define SKL_STATE_INACTIVE 0
#define SKL_STATE_WAIT 1
#define SKL_STATE_RUN 2
#define SKL_STATE_ATTACK 3
#define SKL_STATE_DIE 4
#define SKL_STATE_DEAD 5
#define SKL_STATE_HIT 6
#define SKL_STATE_RETREAT 7

// uses: SKL_RUNSPEED, SKL_TURNSPEED, SKL_ATTACKDIST, SKL_ACTIVEDIST
action Skull()
{
   framework_setup(my, SUBSYSTEM_ENEMY_SKULL);
	//TODO: useful default values
	if(my->SKL_RUNSPEED == 0) my->SKL_RUNSPEED = 20;
	if(my->SKL_TURNSPEED == 0) my->SKL_TURNSPEED = 40;
	if(my->SKL_ATTACKDIST == 0) my->SKL_ATTACKDIST = 300;
	if(my->SKL_ACTIVEDIST == 0) my->SKL_ACTIVEDIST = 3000;
	my->HEALTH = 90;
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 1.5);
	set(my, SHADOW);
	SKULL__toFloor(me);
	c_setminmax(me);
}

void SKULL_GlobalInit()
{
}

void SKULL_Init()
{
}

void SKULL_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_SKULL)
	{
		if (player != NULL)
    	{
    		DEBUG_VAR(ptr->SKL_STATE, 50);
			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				ptr->event = NULL;
				ptr->SKL_STATE = SKL_STATE_HIT;
				SPLATTER_splat(&ptr->x, vector(0,0,0.8));
				set(ptr, TRANSLUCENT);
			}
			
			ptr->SKL_ZOFFSET = 0;
			
			switch(ptr->SKL_STATE)    	
			{
				case SKL_STATE_INACTIVE:
				{
					SKULL__inactive(ptr);
					break;
				}

				case SKL_STATE_WAIT:
				{
					SKULL__wait(ptr);
					break;
				}

				case SKL_STATE_RUN:
				{
					SKULL__run(ptr);
					break;
				}

				case SKL_STATE_ATTACK:
				{
					SKULL__attack(ptr);
					break;
				}

				case SKL_STATE_DIE:
				{
					SKULL__die(ptr);
					break;
				}

				case SKL_STATE_HIT:
				{
					SKULL__hit(ptr);
					break;
				}

				case SKL_STATE_RETREAT:
				{
					SKULL__retreat(ptr);
					break;
				}

				default:
				{
					break;
				}

			}	
			
			if (ptr->SKL_STATE != SKL_STATE_DIE && ptr->SKL_STATE != SKL_STATE_DEAD)
			{
				SKULL__toFloor(ptr);
			}
		}
	
	}	
}

var SKULL__toFloor(ENTITY* ptr)
{
	VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 10);
	VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
	me = ptr;
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON | USE_BOX;
	c_trace(from, to, mode);
	if(HIT_TARGET)
		ptr->z = hit.z + 150 + ptr->SKL_ZOFFSET;
}

var SKULL__turnToPlayer(ENTITY* ptr)
{
	ANGLE vecAngle;
	VECTOR vecTemp;
	vec_set(&vecTemp, &player->x);
	vec_sub(&vecTemp, &ptr->x);
	vec_to_angle(&vecAngle, &vecTemp);

	if (ang(ptr->pan) < vecAngle.pan - 5)
	{
		ptr->pan = minv(vecAngle.pan, ang(ptr->pan + ptr->SKL_TURNSPEED * time_step));
		return 0;
	}	
	if (ang(ptr->pan) > vecAngle.pan + 5)
	{
		ptr->pan = maxv(vecAngle.pan, ang(ptr->pan - ptr->SKL_TURNSPEED * time_step));
		return 0;
	}	
	return 1;
}

void SKULL__inactive(ENTITY* ptr)
{
	/* transitions */
	if(SCAN_IsPlayerNear(ptr, ptr->SKL_ACTIVEDIST))
	{
		ptr->SKL_ZOFFSET = 15 * sinv(total_ticks * 50);
		if (SCAN_IsPlayerInSight(ptr, ptr->SKL_ACTIVEDIST, 360) || SCAN_IsPlayerNear(ptr, ptr->SKL_ACTIVEDIST * 0.3))
		{
			ptr->SKL_STATE = SKL_STATE_WAIT;
		}
	}
}

void SKULL__wait(ENTITY* ptr)
{
	ptr->SKL_ZOFFSET = 30 * sinv(total_ticks * 20);

	/* transitions */
	if (SKULL__turnToPlayer(ptr) != 0)
	{
		ptr->SKL_RUNSPEEDCUR = 0;
		ptr->SKL_STATE = SKL_STATE_RUN;
	}
	else if(!SCAN_IsPlayerNear(ptr, ptr->SKL_ACTIVEDIST + 2000))
	{
		ptr->SKL_STATE = SKL_STATE_INACTIVE;
	}
	else
	{
	}
}

void SKULL__run(ENTITY* ptr)
{
	ptr->SKL_RUNSPEEDCUR = minv(ptr->SKL_RUNSPEEDCUR + 6*time_step, ptr->SKL_RUNSPEED);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, vector(ptr->SKL_RUNSPEEDCUR, 0, 0), nullvector, mode);

	/* transitions */
	if (SCAN_IsPlayerInSight(ptr, ptr->SKL_ATTACKDIST, 75))
	{
		ptr->SKL_STATE = SKL_STATE_ATTACK;
	}
	else if (!SCAN_IsPlayerInSight(ptr, ptr->SKL_ATTACKDIST, 75) && SCAN_IsPlayerBehind(ptr, 1200))
	{
		ptr->SKL_STATE = SKL_STATE_WAIT;
	}
	else
	{
		
	}

}

void SKULL__attack(ENTITY* ptr)
{
	ptr->roll = minv(ptr->roll + 60*time_step, 360);

	/* transitions */
	if (ptr->roll >= 360)
	{
		ptr->roll = 0;
		me = ptr;
		var dist = c_trace(&ptr->x, &player->x, IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | USE_POLYGON | SCAN_TEXTURE | ACTIVATE_SHOOT);

		ptr->SKL_STATE = SKL_STATE_RETREAT;	
	}
	
}

void SKULL__die(ENTITY* ptr)
{
	ptr->SKL_COUNTER = minv(ptr->SKL_COUNTER + 4*time_step, 100);
	var animState = (100 - ptr->SKL_COUNTER ) / 100;	
	vec_set(&ptr->scale_x, vector(animState, animState, animState));

	/* transitions */
	if(animState <= 0)
	{
		ptr->SKL_COUNTER = 0;
		ptr->SKL_STATE = SKL_STATE_DEAD;
		set(ptr, PASSABLE);
	}
}

void SKULL__hit(ENTITY* ptr)
{
	ptr->SKL_COUNTER = minv(ptr->SKL_COUNTER + 4*time_step, 40);
	
	VECTOR dir;
	vec_set(&dir, ptr->DAMAGE_VEC);
	vec_scale(&dir, 10*time_step);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, nullvector, dir, mode);

	/* transitions */
	if (ptr->HEALTH <= 0)
	{
		reset(ptr, TRANSLUCENT);
		ptr->SKL_STATE = SKL_STATE_DIE;
	}
	else if (ptr->SKL_COUNTER >= 40)
	{
		reset(ptr, TRANSLUCENT);
		ptr->SKL_STATE = SKL_STATE_WAIT;			
		ptr->event = ENEMY_HIT_event;
		ptr->DAMAGE_VEC = nullvector;
		ptr->SKL_COUNTER = 0;
	}
	else
	{
		
	}
}

void SKULL__retreat(ENTITY* ptr)
{
	ptr->SKL_COUNTER = minv(ptr->SKL_COUNTER + 4*time_step, 120);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, vector(ptr->SKL_RUNSPEED*-0.4, 0, 0), nullvector, mode);

	/* transitions */
	if (ptr->SKL_COUNTER > 20)
	{
		ptr->SKL_STATE = SKL_STATE_WAIT;	
		ptr->SKL_COUNTER = 0;
	}
}

void spawnskull_startup()
{
	
	wait(-5);
	//while(1)
	{
		ENTITY* ptr = ent_create("whiskas.mdl", vector(5800,-6050,250), Skull);
		wait(-30);
	}
}