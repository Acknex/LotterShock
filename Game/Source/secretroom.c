BMAP* sr_foam = "foam.tga";

function p_foamevent(PARTICLE* p)
{
	if (p->lifespan > (19*16))
	{
		p->alpha = 100 - ((p->lifespan - 19*16) / (1*16)) * 100;
	}
	else if (p->lifespan < (10*16))
	{
		p->alpha = (p->lifespan / (10*16)) * 100;
	}
	//p->alpha=75;
}

function p_foam(PARTICLE* p)
{
//	set(p, MOVE);
	p->alpha = 0;
	p->size = 20 + random(20);
//	p->gravity = 0.01;
	p->bmap = sr_foam;
	p->lifespan = 20*16;
	p->event = p_foamevent;
}

action secretroom_water()
{
	c_setminmax(me);
	var anim;
	var bnim;
	var foamtreshold = 0;
	
	var posx = -234;//16985;
	var posy = 5600;//-800;
	var posz = 64;
	
	var scale = 3.4;
	ENTITY* cbabe;
	cbabe = ent_create("cbabe_n.mdl", _vec(posx, posy, posz), NULL);
	vec_fill(&(cbabe->scale_x), scale);
	cbabe->pan = 90;
	cbabe->tilt = -50;
	while(1)
	{
		anim = (anim + time_step) % 100;
		bnim = abs(-50 + anim);
		ent_animate(cbabe, "death", 40 + (bnim/20), 0);
		
#ifdef DEBUG_SECRETROOM
		cbabe->x = cbabe->x + (key_u - key_i) * time_step;
		cbabe->y = cbabe->y + (key_j - key_k) * time_step;
		cbabe->z = cbabe->z + (key_n - key_m) * time_step;
		
		cbabe->scale_x = cbabe->scale_x + (key_o - key_p) * time_step;
		vec_fill(&(cbabe->scale_x), cbabe->scale_x);
		
		DEBUG_VAR(cbabe->x, 300);
		DEBUG_VAR(cbabe->y, 320);
		DEBUG_VAR(cbabe->z, 340);
		DEBUG_VAR(cbabe->scale_x, 360);
		
		DEBUG_VAR(my->x + (my->max_x + my->min_x) / 2.0, 400);
		DEBUG_VAR(my->y + (my->max_y + my->min_y) / 2.0, 420);
		DEBUG_VAR(my->z + (my->max_z + my->min_z) / 2.0, 440);
#endif

		var l = my->max_x - my->min_x;
		var d = my->max_y - my->min_y;
 		
 		//I don't know what's wrong here, manual offsets applied
		var foam_x = my->x + my->min_x + random(l) -2300;
		var foam_y = my->y + my->min_y + random(d) + 50;
		var foam_z = my->z + my->min_z + random(20);
		
		foamtreshold += time_step / 2;
		
		var spawnfoam = integer(foamtreshold);
		foamtreshold -= spawnfoam;
		
		/*DEBUG_VAR(foam_x, 300);
		DEBUG_VAR(foam_y, 320);
		DEBUG_VAR(foam_z, 340);*/
		if (spawnfoam > 0)
		{
			effect(p_foam, spawnfoam, _vec(foam_x, foam_y, foam_z), nullvector);
		}
		
		wait(1);
	}
}