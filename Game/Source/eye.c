#include "global.h"
#include "framework.h"
#include "eye.h"
#include "particle.h"

#include "splatter.h" //temp

#define EYE_TURNSPEED skill2

#define EYE_COUNTER skill22

BMAP* EYE_BmapDecal = "bulletHoleCool.tga";

void EYE__attack(ENTITY* ptr);

action Eye()
{
   framework_setup(my, SUBSYSTEM_ENEMY_EYE);

	if(my->EYE_TURNSPEED == 0) my->EYE_TURNSPEED = 10;
	vec_scale(me.scale_x, 10);
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
			EYE__turnToPlayer(ptr);
			EYE__attack(ptr);
		}
		
	}
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
	VECTOR dist;
	vec_set(&dist, vector(3000, 0, 0));
	vec_rotate(&dist, &dir);
	//vec_set(&to, &player->x);
	vec_set(&to, &dist);
	vec_add(&to, &from);
//draw_line3d(from, vector(255,255,255), 100);
//draw_line3d(to, vector(255,255,255), 100);
	if (ptr->EYE_COUNTER > 1)
	{
		ptr->EYE_COUNTER = cycle(ptr->EYE_COUNTER,0,1);
		PARTICLE_laser(from, dist);
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
