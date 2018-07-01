
VECTOR menuelevel_center;

action menuelevel_anchor()
{
	vec_set(&menuelevel_center, my->x);
	
	var camera_distance = vec_dist(my->x, camera->x);
	
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

