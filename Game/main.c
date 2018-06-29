#include <acknex.h>

#define PRAGMA_PATH "Source"
#define PRAGMA_PATH "Assets"
#define PRAGMA_PATH "Media"

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

void main()
{
    framework_init();
}
