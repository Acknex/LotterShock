#include "global.h"
#include "framework.h"
#include "eye.h"
#include "particle.h"
#include "scan.h"

#include "splatter.h" //temp

#define EYE_TURNSPEED skill2

#define EYE_COUNTER skill22
#define EYE_STATE skill23
#define EYE_LASERDIST skill24
//#define EYE_LASERDIST skill25
//#define EYE_LASERDIST skill26
#define EYE_ACTIVEDIST skill27
#define EYE_ZOFFSET skill35

#define EYE_STATE_INACTIVE 0
#define EYE_STATE_WAIT 1
#define EYE_STATE_RUN 2
#define EYE_STATE_ATTACK 3

BMAP* EYE_BmapDecal = "bulletHoleCool.tga";

void EYE__attack(ENTITY* ptr);

action Eye()
{
   framework_setup(my, SUBSYSTEM_ENEMY_EYE);

	if(my->EYE_TURNSPEED == 0) my->EYE_TURNSPEED = 10;
	if(my->EYE_ACTIVEDIST == 0) my->EYE_ACTIVEDIST = 3000;
	vec_scale(me.scale_x, 10);
	
	my->material = matObject;
}	


void EYE_GlobalInit()
{
}

void spawneye();
void EYE_Init()
{
	spawneye();
}

void EYE_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_EYE)
	{
		if (player)
		{

    		DEBUG_VAR(ptr->EYE_STATE, 50);
			ptr->EYE_ZOFFSET = 0;

			/*it's broken and I don't know.*/
			switch(ptr->EYE_STATE)    	
			{
				case EYE_STATE_INACTIVE:
				{
					EYE__inactive(ptr);
					break;
				}

				case EYE_STATE_WAIT:
				{
					EYE__wait(ptr);
					break;
				}

				case EYE_STATE_ATTACK:
				{
					EYE__attack(ptr);
					break;
				}

				default:
				{
					break;
				}

			}
			//EYE__turnToPlayer(ptr);
			//EYE__attack(ptr);
		}
		
	}
}

var EYE__toFloor(ENTITY* ptr)
{
	VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 10);
	VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
	me = ptr;
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON;// | USE_BOX;
	c_trace(from, to, mode);
	if(HIT_TARGET)
		ptr->z = hit.z + 150 + ptr->EYE_ZOFFSET;
}

var EYE__turnToPlayer(ENTITY* ptr)
{
	ANGLE vecAngle;
	VECTOR vecTemp;
	vec_set(&vecTemp, &player->x);
	vec_sub(&vecTemp, &ptr->x);
	vec_to_angle(&vecAngle, &vecTemp);

	if (ang(ptr->pan) < vecAngle.pan - 1)
	{
		ptr->pan = minv(vecAngle.pan, ang(ptr->pan + ptr->EYE_TURNSPEED * time_step));
		return 0;
	}	
	if (ang(ptr->pan) > vecAngle.pan + 1)
	{
		ptr->pan = maxv(vecAngle.pan, ang(ptr->pan - ptr->EYE_TURNSPEED * time_step));
		return 0;
	}	
		return 1;
}

void EYE__inactive(ENTITY* ptr)
{
EYE__turnToPlayer(ptr);
		if (SCAN_IsPlayerInSight(ptr, ptr->EYE_ACTIVEDIST, 100) || SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST * 0.3))
		{
			ptr->EYE_STATE = EYE_STATE_ATTACK;
		}
}

void EYE__wait(ENTITY* ptr)
{
	ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);

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

void EYE__attack(ENTITY* ptr)
{
	ptr->EYE_COUNTER += 1*time_step;

	VECTOR from;
	vec_for_vertex(from, ptr, 195);
	VECTOR to;
	vec_set(&to, &player->x);
	vec_sub(&to, &ptr->x);
	ANGLE dir;
	vec_to_angle(&dir, &to);
	//ptr->pan = dir.pan;
	dir.pan = ptr->pan;
	vec_set(&ptr->EYE_LASERDIST, vector(3000, 0, 0));
	vec_rotate(&ptr->EYE_LASERDIST, &dir);
	//vec_set(&to, &player->x);
	vec_set(&to, &ptr->EYE_LASERDIST);
	vec_add(&to, &from);
//draw_line3d(from, vector(255,255,255), 100);
//draw_line3d(to, vector(255,255,255), 100);
	if (ptr->EYE_COUNTER > 1)
	{
		ptr->EYE_COUNTER = cycle(ptr->EYE_COUNTER,0,1);
		PARTICLE_laser(from, ptr);
		ptr->EYE_COUNTER -= 1;
		me = ptr;
		var mode = IGNORE_ME | IGNORE_WORLD | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | SCAN_TEXTURE | USE_POLYGON;
		c_trace(&from, &to, mode);
		if(HIT_TARGET)
		{
	     	PARTICLE* p = ent_decal(you, EYE_BmapDecal, 30, random(360));
	     	p->material = matDecalBlood;
	     	p->lifespan = 64;
	     	p->event = SPLATTER__fade_effect;
	     	p->flags |= TRANSLUCENT;
	     	p->alpha = 100;
		}
	}
	
		if (!SCAN_IsPlayerInSight(ptr, ptr->EYE_ACTIVEDIST, 100) && !SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST * 0.3))
		{
			ptr->EYE_STATE = EYE_STATE_INACTIVE;
		}
}

void spawneye()
{
	
	wait(-5);
	//while(1)
	{
		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(5900,-6050,250), Eye);
		wait(-30);
	}
}
