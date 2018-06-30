#include "global.h"
#include "framework.h"
#include "gib.h"

#define PRAGMA_BIND "gib1.mdl"
#define PRAGMA_BIND "gib2.mdl"
#define PRAGMA_BIND "gib3.mdl"

#define GIB_WAITTIME skill20

STRING* GIB_strModels[3];

void GIB_spawn(VECTOR* pos)
{
    framework_setup(my, SUBSYSTEM_ENEMY_GIB);	
}

void GIB_GlobalInit()
{
	GIB_strModels[0] = str_create();
}

void GIB_Close()
{
	var i;
	for (i = 0; i < 5; i++)
	{
		ptr_remove(GIB_strModels[i]);
	}
}

void GIB_Init()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_GIB)
   {
	}
}

void GIB_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_GIB)
   {
		VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 10);
		VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
		me = ptr;
		var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON;
		c_trace(from, to, mode);
		if(HIT_TARGET)
		{
			if (ptr->z <= hit.z - ptr->min_z)
			{
				ptr->z = hit.z - ptr->min_z;
			}
			else
			{
				c_
			}
		}
	}
}
