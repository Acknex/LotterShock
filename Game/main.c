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

void main()
{
	framework_init();
}
