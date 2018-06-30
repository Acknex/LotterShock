#include "dmgsys.h"
#include "enemy_hit.h"

void ENEMY_HIT_init(ENTITY* ent)
{
	ent->event = ENEMY_HIT_event;
	ent->emask |= ENABLE_SHOOT;	
    ent->emask |= ENABLE_SCAN;
}

void ENEMY_HIT_event()
{
	//todo: handle projectiles
    if(event_type == EVENT_SHOOT || event_type == EVENT_SCAN)
    {
		if (dmgsys_get_src() == DMGSYS_PLAYER)
		{
            my->DAMAGE_HIT += dmgsys_get_dmg();
			ENTITY* ent = dmgsys_get_ctx();
			//pushback only if player attacked
			//TODO: instead of player alone, also react on weapon type
			if (ent == player)
			{
				vec_set (my->DAMAGE_VEC, &my->x);
				vec_sub (my->DAMAGE_VEC, &ent->x);
				vec_normalize (my->DAMAGE_VEC, 10);
			}
		}
	}
}
