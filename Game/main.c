#include <acknex.h>

#include "config.h"

#define PRAGMA_PATH "Source"
#define PRAGMA_PATH "Assets"
#define PRAGMA_PATH "Media"

#include "config.h"
#include "global.h"
#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"

#ifdef DEBUG
#include <default.c>
#endif

#include "framework.c"
#include "splashscreen.c"
#include "mainmenu.c"
#include "splatter.c"

void main()
{
	level_load("testlevel.wmb");
	wait(1);
    framework_init();
}
