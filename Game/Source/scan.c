#include "scan.h"

var SCAN_IsPlayerNear(ENTITY* ent, var distance)
{
	if (player == NULL)
		return 0;
		
	if (vec_dist(&player->x, ent->x < distance))
		return 1;
	else
		return 0;
}

var SCAN_IsPlayerInSight(ENTITY* ent, var distance, var fov)
{
	if (player == NULL)
		return 0;
		
	if (fov == 0)
		fov = 360;	
		
	me = ent;
	you = NULL;
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS;
	var dist = c_trace(&ent->x, &player->x, mode);
	
	if ((dist >= 0) && (dist <= distance) && (you != NULL))
	{
		if (you == player)	
		{
			VECTOR* vecDir;
			vec_set(vecDir, &player->x);
			vec_sub(vecDir, &ent->x);
			
			ANGLE* vecAng;
			vec_to_angle(vecAng, vecDir);
			
			if (absv(vecAng->pan - ent->pan) <= fov)
				return 1;
		}
	}
	
	return 0;
}