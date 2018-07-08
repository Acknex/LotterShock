#include "flesh.h"
#include "enemy_hit.h"
#include "splatter.h"

#define FL_SPLATTERCNT skill21
#define FL_ORIGINALROT skill22
#define FL_ORIGINALROT skill23
#define FL_ORIGINALROT skill24

BMAP* FLESH_bmapSplatter[5];

action Flesh()
{
   framework_setup(my, SUBSYSTEM_FLESH);
	ENEMY_HIT_init(me);			
}

void FLESH_GlobalInit()
{
	FLESH_bmapSplatter[0] = bmap_create("splatter_red_01.png");
	FLESH_bmapSplatter[1] = bmap_create("splatter_red_02.png");
	FLESH_bmapSplatter[2] = bmap_create("splatter_red_03.png");
	FLESH_bmapSplatter[3] = bmap_create("splatter_red_04.png");
	FLESH_bmapSplatter[4] = bmap_create("splatter_red_05.png");
	int i;
	for (i= 0; i < 5; i++)
	{
		bmap_preload(FLESH_bmapSplatter[i]);
	}
}

void FLESH_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_FLESH)
	{
		ptr->FL_SPLATTERCNT = maxv(0, ptr->FL_SPLATTERCNT - time_step);
		
		//abuse enemy hit system
		if (ptr->DAMAGE_HIT && ptr->FL_SPLATTERCNT <= 0)
		{
			ptr->DAMAGE_HIT = 0;
			ptr->FL_SPLATTERCNT = 16;
			SPLATTER_explode(20, ptr, 800, FLESH_bmapSplatter, 5);
		}
		
	}
}
