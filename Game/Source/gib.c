#include "global.h"
#include "framework.h"
#include "gib.h"
#include "materials.h"

#define PRAGMA_BIND "gib1.mdl"
#define PRAGMA_BIND "gib2.mdl"
#define PRAGMA_BIND "gib3.mdl"

#define GIB_WAITTIME skill20
#define GIB_STARPOS skill21
//#define GIB_STARPOS skill22
//#define GIB_STARPOS skill23
#define GIB_STATE skill24

#define GIB_FALL 0
#define GIB_FLOOR 1

STRING* GIB_strModels[3];

void GIB_Spawn(VECTOR* pos)
{
	var rnd = random(3);
	VECTOR entPos;
	vec_set(&entPos, pos);
	VECTOR* vecDist = vector(random(50) + 100, 0, 0);
	vec_rotate(vecDist, vector(random(360), 0, 0));
	vec_add(&entPos, vecDist);
	entPos.z += random(40) + 80;
	
   ENTITY* ent = ent_create(GIB_strModels[rnd], entPos, NULL);
	framework_setup(ent, SUBSYSTEM_ENEMY_GIB);	
   set(ent, PASSABLE);
   ent->pan = random(360);
   vec_scale(&ent->scale_x, random(40)+80);
   vec_set(&ent->GIB_STARPOS, &ent->x);
   ent->material = matFlesh;
}

void GIB_GlobalInit()
{
	GIB_strModels[0] = str_create("gib1.mdl");
	GIB_strModels[1] = str_create("gib2.mdl");
	GIB_strModels[2] = str_create("gib3.mdl");
}

void GIB_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_GIB)
   {
   	switch(ptr->GIB_STATE)
   	{
   		case GIB_FALL:
   		{
   			GIB__fall(ptr);
   			break;
   		}

   		case GIB_FLOOR:
   		{
   			GIB__floor(ptr);
   			break;
   		}

   		default:
   		{
   			break;
   		}
   	}
	}
}

void GIB__fall(ENTITY* ptr)
{
	ptr->roll += 5* time_step;
	VECTOR to;
	vec_set(&to, &ptr->GIB_STARPOS);
	to.z -= 1000;
	me = ptr;
	you = player;
	var mode = IGNORE_ME | IGNORE_YOU | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON;
	c_trace(&ptr->GIB_STARPOS, &to, mode);
	ptr->z -= 15* time_step;
	if(HIT_TARGET)
	{
		if (ptr->z <= hit.z - ptr->min_z)
		{
			ptr->GIB_STATE = GIB_FLOOR;
			ptr->z = hit.z - ptr->min_z;
			ptr->alpha = 100;
		}
	}
}

void GIB__floor(ENTITY* ptr)
{
	ptr->GIB_WAITTIME += time_step;
	if (ptr->GIB_WAITTIME > FX_WAITTIME - 40)
	{
		set(ptr, TRANSLUCENT);
		ptr->alpha = maxv(0, ptr->alpha - 3* time_step);
	}
	
	if (ptr->alpha <= 0)
	{
		set(ptr, INVISIBLE);
		ptr->SK_ENTITY_DEAD = 1;
	}
}
