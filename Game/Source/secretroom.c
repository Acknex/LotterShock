action secretroom_water()
{
	c_setminmax(me);
	var anim;
	var bnim;
	
	var posx = 16985;
	var posy = -800;
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
#endif

		wait(1);
	}
}