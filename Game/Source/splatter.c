#include "splatter.h"


void SPLATTER_explode(var count, VECTOR* pos, var distance, BMAP* decal)
{
	var i;
	VECTOR targetPos;

	for (i = 0; i < count; i++)
	{
		vec_for_angle(&targetPos, vector(random(360), random(360), 0));
		vec_scale(&targetPos, distance);
		vec_add(&targetPos, pos);
		var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | SCAN_TEXTURE | USE_POLYGON;
		var dist = c_trace(pos, &targetPos, mode);
		if(HIT_TARGET)
		{
	      PARTICLE* p = ent_decal(you, decal, minv((distance - dist) * 0.8, 270), random(360));
	      p->lifespan = 320;
	      p->event = SPLATTER__fade_effect;
	      p->flags |= TRANSLUCENT;
	      p->alpha = 100;
		}
		
	}
}

void SPLATTER__fade_effect(PARTICLE* p)
{
	if (p->lifespan < 40)
	{
		p->alpha = maxv(0, p->alpha - 3* time_step);
	}	
}

#ifdef DEBUG
BMAP* Splattermap = "blutlache.tga";
action Splattertest()
{
	while(1)
	{	
		if (key_space != 0)
			SPLATTER_explode(my->skill1, &my->x, my->skill2, Splattermap);
		
		wait(1);
	}
}
#endif