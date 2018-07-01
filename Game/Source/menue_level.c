
VECTOR menuelevel_center;


PANEL* HUD_mainmenue =
{
	bmap = "Menuetitle.png";
	flags = TRANSLUCENT | SHOW;
	layer = 2;
}

action menuelevel_anchor()
{
	vec_set(&menuelevel_center, my->x);
	
	var camera_distance = vec_dist(my->x, camera->x);
	
	HUD_mainmenue->pos_x = (screen_size.x-HUD_mainmenue.size_x) /2;
	HUD_mainmenue->pos_y = 40;
	
	while(1)
	{
		VECTOR diff;
		vec_set(&diff, &menuelevel_center);
		vec_sub(&diff, camera->x);
		vec_to_angle(camera->pan, &diff);
		
		camera->x = sin(total_ticks)*camera_distance;
		camera->y = cos(total_ticks)*camera_distance;
		
		wait(1);
	}
	
	
}

