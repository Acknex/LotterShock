#include "splatter.h"


void SPLATTER_explode(var count, ENTITY* ent, var distance, BMAP** decal, var bmapCount)
{
	var i;
	VECTOR targetPos;

	ENTITY* oldMe = me;
	me = ent;
	for (i = 0; i < count; i++)
	{
		vec_for_angle(&targetPos, vector(random(360), random(150)-60, 0));
		vec_scale(&targetPos, distance);
		vec_add(&targetPos, &ent->x);
		me = ent;
		c_ignore(GROUP_PLAYER,0);
		var mode = IGNORE_ME | IGNORE_WORLD | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | SCAN_TEXTURE | USE_POLYGON;
		var dist = c_trace(&ent->x, &targetPos, mode);
		if(HIT_TARGET)
		{
			var rnd = integer(random(bmapCount));
			PARTICLE* p = ent_decal(you, decal[rnd], minv((distance - dist) * 0.8, 370) * (random(0.3) + 1), random(360));
			if(p) // ent_decal can fail!!
			{
				p->material = matDecalBlood;
				p->lifespan = FX_WAITTIME;
				p->event = SPLATTER__fade_effect;
				p->flags |= TRANSLUCENT;
				p->alpha = 100;
			}
		}
		
	}
	me = oldMe;
}

void SPLATTER__fade_effect(PARTICLE* p)
{
	if (p->lifespan < 40)
	{
		p->alpha = maxv(0, p->alpha - 3* time_step);
	}	
}

action SPLATTER_splat_action()
{
	set(my, PASSABLE);
	my->material = matSplatBlood;
	VECTOR dir;
	VECTOR add;
	var sigX = 1;
	var sigY = 1;
	switch(integer(random(2)))
	{
		case 1: sigX = -1;
	}
	switch(integer(random(2)))
	{
		case 1: sigY = -1;
	}
	vec_set(dir, vector((random(80)+5)*sigX, (random(80)+5)*sigY, random(80)+20));
	vec_scale(dir, 0.7);
	
	my->skill41 = floatv(0.004);
	var x = 0;
	var dauer = 0.3;
	while(x < dauer)
	{
		
		my->skill41 = floatv(maxv(x/dauer, 0.004));
		vec_set(add, dir);
		vec_add(add, vector(0,0, -x*x*80));
		vec_scale(add, time_step);
		vec_add(my->x, add);
		x += time_step/16;
		wait(1);
	}
	ptr_remove(me);
}

void SPLATTER_splat(VECTOR* pos, VECTOR* color)
{
	VECTOR p2;
	vec_set(p2, pos);
	VECTOR c;
	vec_set(c, color);
	
	var i = 0;
	for(i = 0; i < 3; ++i)
	{	
		
		var j = 0;
		for(j = 0; j < 10; ++j)
		{
			ENTITY* p;
			switch(integer(random(3)))
			{
				case 0: p = ent_create("splat_t1.tga", p2, SPLATTER_splat_action); break;
				case 1: p = ent_create("splat_t2.tga", p2, SPLATTER_splat_action); break;
				case 2: p = ent_create("splat_t3.tga", p2, SPLATTER_splat_action); break;
				case 3: p = ent_create("splat_t4.tga", p2, SPLATTER_splat_action); break;          
			}
			p->skill45 = floatv(c.x);
			p->skill46 = floatv(c.y);
			p->skill47 = floatv(c.z);
			vec_scale(p->scale_x, 1 + random(2));
		}
		wait(-0.02);
	}
}

