#include <acknex.h>

#include "config.h"

#define PRAGMA_LEVEL "menue.wmp"
#define PRAGMA_LEVEL "bestiary.wmp"
#define PRAGMA_LEVEL "gamelevel.wmp"
#define PRAGMA_LEVEL "intro.wmp"

#define PRAGMA_PATH "Source"
#define PRAGMA_PATH "Shaders"
#define PRAGMA_PATH "Assets"
#define PRAGMA_PATH "Media"
#define PRAGMA_PATH "Screenshots"

#include "global.h"
#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"
#include "input.h"
#include "doors.h"
#include "environmentals.h"
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
#include "sputnik.c"
#include "skull.c"
#include "doors.c"
#include "environmentals.c"
#include "gib.c"
#include "items.c"
#include "particle.c"
#include "journals.c"
#include "eye.c"
#include "ini.c"
#include "settings.c"
#include "options.c"
#include "ang.c"
#include "map.c"
#include "bestiary.c"
#include "turret.c"
#include "enemy.c"
#include "pausemenu.c"
#include "ui.c"
#include "cheats.c"
#include "secretroom.c"
#include "flesh.c"
#include "intro.c"
#include "geist.c"

void main()
{
    framework_init();
}
