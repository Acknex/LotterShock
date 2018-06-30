#include "credits.h"
#include "wm3load.h"

var CREDITS_vDone = 0;

void credits_init()
{
	add_resource("media\\skaph.wrs");
}

void credits_open()
{
	CREDITS_vDone = 0;
	level_load(""); /* load empty level */
	wait(1);
	//WM3_load("media\\skaph.wm3");	
}

void credits_close()
{
	//WM3_unload();	
}

void credits_update()
{
	if(key_x)
	{
		CREDITS_vDone = 1;
	}
}

var credits_isdone()
{
	return CREDITS_vDone;	
}