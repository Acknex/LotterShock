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

void main()
{
	collision_mode = 2;
	framework_init();
}
