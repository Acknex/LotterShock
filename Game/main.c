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
<<<<<<< HEAD
#include "enemy_hit.c"
#include "eselslerche.c"
=======
#include "doors.c"
>>>>>>> 8c798225195780f7198da81210ae66dd703f8631

void main()
{
	framework_init();
}
