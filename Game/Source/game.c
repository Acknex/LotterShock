#include "game.h"
#include "weapons.h"
#include "movement.h"
#include "input.h"
#include "eselslerche.h"
#include "skull.h"
#include "eye.h"
#include "projectiles.h"
#include "gib.h"

#include <windows.h>

bool game_done;

void game_init()
{
    weapons_init();
    GIB_GlobalInit();
    ESELSLERCHE_GlobalInit();
    SKULL_GlobalInit();
    EYE_GlobalInit();
}

void game_open()
{
    game_done = false;
    mouse_pointer = 0;
    weapons_open();
    ESELSLERCHE_Init();
    SKULL_Init();
    EYE_Init();
    hud_show();
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
    if(!weapons_disabled) weapons_update();
    projectiles_update();
    //keycard_update();
    medipack_update();
    keypad_update();
    medipack_update();
    doors_update();
    
    
	 hud_update();
	 ESELSLERCHE_Update();
	 SKULL_Update();
	 EYE_Update();
	 GIB_Update();

    if(input_hit(INPUT_NAVBACK))
        game_done = true;
}

void game_close()
{
movement_close();
projectiles_close();
    hud_hide();
    weapons_close();
    mouse_pointer = 1;
}

bool game_is_done()
{
    return game_done;
}
