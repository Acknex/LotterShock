action secretroom_water()
{
	c_setminmax(me);
	var anim;
	ENTITY* cbabe;
	while(1)
	{
		cbabe = ent_create("cababe.mdl", my->x, null);
		anim = (anim + time_step) % 100;
		ent_animate(cbabe, death, 40 + (anim/10), 0);
		wait(1);
	}
}