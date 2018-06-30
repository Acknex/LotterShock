#ifndef ENEMY_HIT_H
#define ENEMY_HIT_H

void ENEMY_HIT_event()
{
	//todo: handle projectiles
	if(event_type == EVENT_SHOOT)
	{
		if (you == player)
		{
			//TODO: get weapon type from player
			//TODO: get damage from player
			var damage = 1000; //stub
			my->DAMAGE_HIT = damage;
		}
	}
}

#endif