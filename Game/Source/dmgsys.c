#include "dmgsys.h"

typedef struct dmgsys_t
{
    int frame;
    int src;
    ENTITY * ctx;
    var dmg;
} dmgsys_t;

dmgsys_t dmgsys;

void dmgsys_set_src(int src, ENTITY * ctx, var dmg)
{
    dmgsys.frame = total_frames;
    dmgsys.src = src;
    dmgsys.ctx = ctx;
    dmgsys.dmg = dmg;
}

//! Fragt die Schadensquelle ab
int dmgsys_get_src()
{
    if(dmgsys.frame == total_frames)
        return dmgsys.src;
    else
        return DMGSYS_NONE;
}

//! Fragt die Herkunfts-Entity ab
ENTITY * dmgsys_get_ctx()
{
    if(dmgsys.frame == total_frames)
        return dmgsys.ctx;
    else
        return DMGSYS_NONE;
}

//! Fragt den Schaden ab
var dmgsys_get_dmg()
{
    if(dmgsys.frame == total_frames)
        return dmgsys.dmg;
    else
        return DMGSYS_NONE;
}
