#include "game.h"
#include "weapons.h"
#include "movement.h"
#include "input.h"
#include "eselslerche.h"
#include "gib.h"

#include <windows.h>

bool game_done;

void game_init()
{
    weapons_init();
}

void game_open()
{
    game_done = false;
    mouse_pointer = 0;
    weapons_open();
    ESELSLERCHE_Init();
    GIB_GlobalInit();
}

void game_capture_mouse()
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
}

void game_update()
{
#ifndef DEBUG_NO_CAPTURE
    if(window_focus)
        game_capture_mouse();
#endif

    movement_update();
    weapons_update();
	 hud_update();
	 ESELSLERCHE_Update();
	 GIB_Update();

    if(input_hit(INPUT_NAVBACK))
        game_done = true;
}

void game_close()
{
    weapons_close();
    GIB_Close();
    mouse_pointer = 1;
}

bool game_is_done()
{
    return game_done;
}
