#include "global.h"
#include "framework.h"
#include "eye.h"
#include "particle.h"
#include "scan.h"
#include "ang.h"

#include "splatter.h" //temp

#define EYE_PATHID skill1
#define EYE_PATHPROGRESS skill2
#define EYE_PATROLSPEED skill3
#define EYE_TURNSPEED skill4
#define EYE_ATTACKDIST skill5
#define EYE_ACTIVEDIST skill6

#define EYE_COUNTER skill22
#define EYE_STATE skill23
#define EYE_LASERDIST skill24
//#define EYE_LASERDIST skill25
//#define EYE_LASERDIST skill26
#define EYE_ACTIVEDIST skill27
#define EYE_LASTPOS skill28
//#define EYE_LASTPOS skill29
//#define EYE_LASTPOS skill30
#define EYE_PATROLLEN skill33
#define EYE_PATROLDIST skill34
#define EYE_ZOFFSET skill35

#define EYE_STATE_INACTIVE 0
#define EYE_STATE_PATROL 1
#define EYE_STATE_RUN 2
#define EYE_STATE_ATTACK 3
#define EYE_STATE_DIE 4
#define EYE_STATE_DEAD 5

BMAP* EYE_BmapDecal = "bulletHoleCool.tga";
BMAP* EYE_bmapSplatter[5];

void EYE__attack(ENTITY* ptr);

// uses: EYE_PATROLSPEED, EYE_TURNSPEED, EYE_ATTACKDIST, EYE_ACTIVEDIST, EYE_PATHID, EYE_PATHPROGRESS
action Eye()
{
   framework_setup(my, SUBSYSTEM_ENEMY_EYE);

	if(my->EYE_TURNSPEED == 0) my->EYE_TURNSPEED = 10;
	if(my->EYE_PATROLSPEED == 0) my->EYE_PATROLSPEED = 15;
	if(my->EYE_ACTIVEDIST == 0) my->EYE_ACTIVEDIST = 4000;
	if(my->EYE_ATTACKDIST == 0) my->EYE_ATTACKDIST = 2000;
	my->HEALTH = HEALTH_EYE;
	vec_scale(me.scale_x, 10);
//	my->EYE_PATHID = 2;//temp
	my->material = matObject;
	if (path_set(my, str_for_num(NULL, my->EYE_PATHID)) == 0)
	{
		path_set(my, str_for_num(NULL, 0));
	}
	my->EYE_PATROLLEN = path_length(me);
	my->EYE_PATHPROGRESS = clamp(my->EYE_PATROLSPEED,0,100) / 100;
	path_spline (my,&my->x,my->EYE_PATROLLEN * my->EYE_PATHPROGRESS);
	vec_set(&my->EYE_LASTPOS, &my->x);
	set(my, PASSABLE);
}	


void EYE_GlobalInit()
{
	EYE_bmapSplatter[0] = bmap_create("splatter_red_01.png");
	EYE_bmapSplatter[1] = bmap_create("splatter_red_02.png");
	EYE_bmapSplatter[2] = bmap_create("splatter_red_03.png");
	EYE_bmapSplatter[3] = bmap_create("splatter_red_04.png");
	EYE_bmapSplatter[4] = bmap_create("splatter_red_05.png");
	int i;
	for (i= 0; i < 5; i++)
	{
		bmap_preload(EYE_bmapSplatter[i]);
	}
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
            if(vec_dist(ptr->x, player->x) < 1000)
            {
                VECTOR tmp;
                vec_set(tmp, ptr->x);
                if(vec_to_screen(tmp, camera))
                {
                    achievement_kill_beast(BEAST_EYE);
                }
            }

    		/*DEBUG_VAR(ptr->EYE_STATE, 50);
    		DEBUG_VAR(ptr->x, 70);
    		DEBUG_VAR(ptr->y, 90);
    		DEBUG_VAR(ptr->z, 110);*/			
    		ptr->EYE_ZOFFSET = 0;

			/*it's broken and I don't know.*/
			switch(ptr->EYE_STATE)    	
			{
				case EYE_STATE_INACTIVE:
				{
					EYE__inactive(ptr);
					break;
				}

				case EYE_STATE_PATROL:
				{
					EYE__patrol(ptr);
					break;
				}

				case EYE_STATE_ATTACK:
				{
					EYE__attack(ptr);
					break;
				}

				case EYE_STATE_DIE:
				{
					EYE__die(ptr);
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

void EYE__inactive(ENTITY* ptr)
{
	//adjust eye position but do not do anything else
	ptr->EYE_PATROLDIST = cycle(ptr->EYE_PATROLDIST + ptr->EYE_PATROLSPEED*time_step,0,ptr->EYE_PATROLLEN);
	
	/* transitions */
	if(SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST))
	{
		ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);
		if (SCAN_IsPlayerInSight(ptr, ptr->EYE_ACTIVEDIST, 360) || SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST * 0.3))
		{
			ptr->EYE_STATE = EYE_STATE_PATROL;
		}
	}
}

void EYE__patrol(ENTITY* ptr)
{
	ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);
	ptr->EYE_PATROLDIST = cycle(ptr->EYE_PATROLDIST + ptr->EYE_PATROLSPEED*time_step,0,ptr->EYE_PATROLLEN);
	path_spline (ptr,&ptr->x,ptr->EYE_PATROLDIST );
	ANGLE vecAngle;
	VECTOR vecDir;
	vec_diff(&vecDir,&ptr->x,ptr->EYE_LASTPOS);
	vec_to_angle(&vecAngle,&vecDir);
	vec_set(ptr->EYE_LASTPOS,&ptr->x);
	ANG_turnToAngle(ptr, vecAngle.pan, ptr->EYE_TURNSPEED, 1);
	
	/* transitions */
	/*if (SCAN_IsPlayerInSight(ptr, ptr->EYE_ATTACKDIST, 75))
	{
		ptr->EYE_STATE = EYE_STATE_ATTACK;
	}
	else if (!SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST * 1.1))
	{
		ptr->EYE_STATE = EYE_STATE_INACTIVE;
	}
	else
	{
		
	}*/
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
		c_ignore(GROUP_PLAYER,0);
		c_trace(&from, &to, mode);
		if(HIT_TARGET)
		{
	     	PARTICLE* p = ent_decal(you, EYE_BmapDecal, 30, random(360));
			if(p) // ent_decal can fail!!
	     	{
	     		p->material = matDecalBlood;
	     	p->lifespan = 64;
	     	p->event = SPLATTER__fade_effect;
	     	p->flags |= TRANSLUCENT;
	     	p->alpha = 100;
     	}
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
		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(400,1600,200), Eye);
		//ENTITY* ptr = ent_create("enemy_eye.mdl", vector(5900,-6050,250), Eye);		
		wait(-30);
	}
}
