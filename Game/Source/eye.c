#include "global.h"
#include "framework.h"
#include "eye.h"

#include "splatter.h" //temp

#define EYE_COUNTER skill22

BMAP* EYE_BmapDecal = "bulletHoleCool.tga";

void EYE__attack(ENTITY* ptr);

action Eye()
{
   framework_setup(my, SUBSYSTEM_ENEMY_EYE);

	vec_scale(me.scale_x, 10);
}	


void EYE_GlobalInit()
{
}

void EYE_Init()
{
}

void EYE_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_EYE)
	{
		if (player)
		{
			EYE__attack(ptr);
		}
		
	}
}

void EYE__attack(ENTITY* ptr)
{
	ptr->EYE_COUNTER += 4*time_step;

	VECTOR from;
	vec_for_vertex(from, ptr, 195);
	VECTOR to;
	vec_set(&to, &player->x);
	vec_sub(&to, &ptr->x);
	ANGLE dir;
	vec_to_angle(&dir, &to);
	dir.pan -= 2;
	ptr->pan = dir.pan + 90;
	VECTOR dist;
	vec_set(&dist, vector(3000, 0, 0));
	vec_rotate(&dist, &dir);
	//vec_set(&to, &player->x);
	vec_set(&to, &dist);
	vec_add(&to, &from);
draw_line3d(from, vector(255,255,255), 100);
draw_line3d(to, vector(255,255,255), 100);
	if (ptr->EYE_COUNTER > 1)
	{
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

void spawneye_startup()
{
	
	wait(-5);
	//while(1)
	{
		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(5900,-6050,250), Eye);
		wait(-30);
	}
}