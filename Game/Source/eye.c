#include "global.h"
#include "framework.h"
#include "eye.h"
#include "particle.h"
#include "scan.h"
#include "ang.h"
#include "enemy.h"
#include "splatter.h"
#include "gib.h"

#define EYE_PATHID skill1
#define EYE_PATHPROGRESS skill2
#define EYE_PATROLSPEED skill3
#define EYE_TURNSPEED skill4
#define EYE_ATTACKDIST skill5
#define EYE_ACTIVEDIST skill6
#define EYE_ANIMSPEED skill7
#define EYE_SHOTCOUNTER skill8

#define EYE_ANIMSTATE skill21
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
#define EYE_PATHENT skill36
#define EYE_BASESCALE skill37
//#define EYE_BASESCALE skill38
//#define EYE_BASESCALE skill39
#define EYE_LASTPAN skill40

#define EYE_STATE_INACTIVE 0
#define EYE_STATE_PATROL 1
#define EYE_STATE_RUN 2
#define EYE_STATE_ATTACK 3
#define EYE_STATE_DIE 4
#define EYE_STATE_DEAD 5
#define EYE_STATE_HIT 6

#define EYE_WALKANIM "stand"

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
	if(my->EYE_ANIMSPEED == 0) my->EYE_ANIMSPEED = 3;
	my->HEALTH = HEALTH_EYE;
	vec_scale(&my->scale_x, 10);
	vec_set(&my->EYE_BASESCALE, &my->scale_x);
	my->material = matObject;
	ENTITY* ent = ent_create(NULL, nullvector, NULL);
	if (path_set(ent, str_for_num(NULL, my->EYE_PATHID)) == 0)
	{
		path_set(ent, str_for_num(NULL, 0));
	}
	my->EYE_PATROLLEN = path_length(me);
	my->EYE_PATHPROGRESS = clamp(my->EYE_PATROLSPEED,0,100) / 100;
	path_spline (ent,&ent->x,my->EYE_PATROLLEN * my->EYE_PATHPROGRESS);
	vec_set(&my->x, ent->x);
	vec_set(&my->EYE_LASTPOS, &my->x);
	//set(my, PASSABLE);
	ENEMY_HIT_init(me);		
	my->EYE_PATHENT = handle(ent);
	my->group = GROUP_ENEMY;
	c_setminmax(me);
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

void EYE_Init()
{
	//DEBUG
		//ENTITY* ptr = ent_create("enemy_eye.mdl", vector(400,1600,200), Eye);
//		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(5900,-6050,250), Eye);		
		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(1200,6500,250), Eye);		
}

void EYE_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_EYE)
	{
		if (player)
		{
            /*if(vec_dist(ptr->x, player->x) < 1000)
            {
                VECTOR tmp;
                vec_set(tmp, ptr->x);
                if(vec_to_screen(tmp, camera))
                {
                    achievement_kill_beast(BEAST_EYE);
                }
            }*/

			ptr->EYE_ANIMSTATE += ptr->EYE_ANIMSPEED * time_step;
    		ptr->EYE_ZOFFSET = 0;

			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				ptr->event = NULL;
				ptr->EYE_STATE = EYE_STATE_HIT;
				SPLATTER_splat(&ptr->x, vector(1.0,0.0,0.0));
				ptr->EYE_COUNTER = 0;
				ptr->EYE_LASTPAN = ptr->pan;
			}

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

				case EYE_STATE_HIT:
				{
					EYE__hit(ptr);
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

action eye_shot()
{
	my->emask |= ENABLE_IMPACT | ENABLE_SHOOT | ENABLE_ENTITY;
	vec_scale(my->scale_x, 32);
	my->event = ENEMY__projectileEvent;
	my->skill20 = 50;
	my->skill21 = 16*10;
	set(my, LIGHT);
	framework_setup(my, SUBSYSTEM_PROJECTILE);
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
	ENTITY* ent = ptr_for_handle(ptr->EYE_PATHENT);
	path_spline (ent,&ent->x,ptr->EYE_PATROLDIST);

	/* transitions */
	if(SCAN_IsPlayerNear(ent, ptr->EYE_ACTIVEDIST))
	{
		ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);
		if (SCAN_IsPlayerInSight(ent, ptr->EYE_ACTIVEDIST, 360) || SCAN_IsPlayerNear(ent, ptr->EYE_ACTIVEDIST * 0.3))
		{
			ptr->EYE_STATE = EYE_STATE_PATROL;
			vec_set(&ptr->x, &ent->x);
		}
	}
}

void EYE__patrol(ENTITY* ptr)
{
	ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);
	ent_animate(ptr, EYE_WALKANIM, ptr->EYE_ANIMSTATE, ANM_CYCLE);

	ENTITY* ent = ptr_for_handle(ptr->EYE_PATHENT);
	//eye stuck detection. do not progress path until eye caught up
	if(vec_dist(&ptr->x, &ent->x) < 100)
	{
		ptr->EYE_PATROLDIST = cycle(ptr->EYE_PATROLDIST + ptr->EYE_PATROLSPEED*time_step,0,ptr->EYE_PATROLLEN);
		path_spline (ent,&ent->x,ptr->EYE_PATROLDIST);
	}

	VECTOR vdiff;
	vec_diff(&vdiff, &ent->x, &ptr->x);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | GLIDE;
	c_ignore(GROUP_ENEMY,0);
	c_move(ptr, nullvector, vdiff, mode);

	ANGLE vecAngle;
	VECTOR vecDir;
	vec_diff(&vecDir,&ptr->x,ptr->EYE_LASTPOS);
	vec_to_angle(&vecAngle,&vecDir);
	vec_set(ptr->EYE_LASTPOS,&ptr->x);
	ANG_turnToAngle(ptr, vecAngle.pan, ptr->EYE_TURNSPEED, 1);
	DEBUG_VAR(ptr->x, 50);
	DEBUG_VAR(ptr->y, 70);
	if (SCAN_IsPlayerInSight(ptr, ptr->EYE_ATTACKDIST, 30))
	{
		
		ptr->EYE_SHOTCOUNTER += time_step;
		if(ptr->EYE_SHOTCOUNTER > 16)
		{
			ptr->EYE_SHOTCOUNTER -= 16;
			//fix me.
			/*ENTITY* ent = ent_create("eye_shot.mdl", ptr->x, eye_shot);
			VECTOR v;
			vec_set(v, ptr->x);
			vec_sub(v, player->x);
			vec_normalize(v,1);
			vec_set(ent->skill1, v);
			vec_scale(v, -1);
			vec_to_angle(ent->pan, v);*/
		}
		
	}

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

void EYE__hit(ENTITY* ptr)
{
	ptr->EYE_COUNTER += 10 * time_step;
	ptr->pan = ptr->EYE_LASTPAN + 4*ptr->EYE_COUNTER;

	/* transitions */
	if (ptr->HEALTH <= 0)
	{
        achievement_kill_beast(BEAST_EYE);
		ptr->EYE_COUNTER = 0;
		ptr->EYE_STATE = EYE_STATE_DIE;
		set(ptr, PASSABLE);
	}
	else if (ptr->EYE_COUNTER >= 90)
	{
		ptr->EYE_STATE = EYE_STATE_PATROL;			
		ptr->event = ENEMY_HIT_event;
		ptr->EYE_COUNTER = 0;
		vec_zero(ptr->DAMAGE_VEC);
	}
	else if (ptr->EYE_COUNTER>= 50)
	{
		ptr->event = ENEMY_HIT_event;		
	}
	else
	{
	}
}

void EYE__die(ENTITY* ptr)
{
	ptr->EYE_COUNTER += time_step;
	vec_set(&ptr->scale_x, &ptr->EYE_BASESCALE);
	//factor 18 * 20 should make this end with cos(360deg) = 1 = max scale before poof
	vec_scale(&ptr->scale_x, 1 + (0.1*ptr->EYE_COUNTER * absv(cosv(2*ptr->EYE_COUNTER * 18))));
	VECTOR diff;
	vec_diff(&diff, &player->x, &ptr->x);
	ANGLE dir;
	vec_to_angle(&dir, &diff);
	ptr->pan = dir.pan;
	
	/* transitions */
	if(ptr->EYE_COUNTER >= 20)
	{
		var i;
		for ( i = 0; i < 3; i++)
		{
			GIB_Spawn(&ptr->x);
		}
		SPLATTER_explode(50, ptr, 400, EYE_bmapSplatter, 5);
		ptr->EYE_STATE = EYE_STATE_DEAD;
		ptr->SK_ENTITY_DEAD = 1;
	}

}
