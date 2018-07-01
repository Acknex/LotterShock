#include <acknex.h>

#include "config.h"

#define PRAGMA_PATH "Source"
#define PRAGMA_PATH "Assets"
#define PRAGMA_PATH "Media"

#include "global.h"
#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"
#include "input.h"
#include "doors.h"
#include "projectiles.h"
#include "materials.h"
#include "items.h"
#include "journals.h"

#ifdef DEBUG
#include <default.c>
#endif

#include "framework.c"
#include "splashscreen.c"
#include "mainmenu.c"
#include "music_player.c"
#include "game.c"
#include "weapons.c"
#include "movement.c"
#include "input.c"
#include "splatter.c"
#include "credits.c"
#include "math.c"
#include "scan.c"
#include "dmgsys.c"
#include "hud.c"
#include "enemy_hit.c"
#include "eselslerche.c"
#include "skull.c"
#include "doors.c"
#include "gib.c"
#include "items.c"
#include "particle.c"
#include "journals.c"
#include "eye.c"

#include "menue_level.c"

void main()
{
	level_load("menue.wmb");
}

action act_rotator()
{

	my->alpha = 100;
	//setSsaoSurface(my, SSAO_TYPE_BINARY);


	if(my->skill[0] == 0)

	my->skill[0] = 10;

	while(1)

	{

		my->pan -= my->skill[0]*time_step;

		wait(1);

	}

}

action act_lightflares()

{
	
	my->alpha = 100;

	my->flags |= (BRIGHT|TRANSLUCENT|NOFOG);
   set(my, FLAG1);
   
	my.emask &= ~DYNAMIC;
	//setSsaoSurface(my, SSAO_TYPE_BINARY);
}

action act_animation()

{



	if(my->string1 == NULL)

	{

		error("an animationobject has no animation set");

		return;

	}

	if(str_cmpi(my->string1, ""))

	{

		error("an animationobject has no animation set");

		return;

	}

	

	if(my->skill[0] == 0)

	my->skill[0] = 1;

	while(1)

	{

		my->skill[1] = (my->skill[1] +my->skill[0]*time_step) %100;

		ent_animate(my, my->string1, my->skill[1], ANM_CYCLE);

		wait(1);

	}

}