#include <acknex.h>

#define PRAGMA_PATH "Source"

#include "global.h"
#include "framework.h"

#ifdef DEBUG
#include <default.c>
#endif

#include "framework.c"

void main()
{
    framework_init();
    on_frame = framework_update;
}
