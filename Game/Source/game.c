#include "game.h"
#include "weapons.h"
#include "movement.h"
#include "input.h"
#include "eselslerche.h"
#include "sputnik.h"
#include "skull.h"
#include "eye.h"
#include "projectiles.h"
#include "gib.h"
#include "hud.h"
#include "map.h"
#include "turret.h"
#include "enemy.h"

#include <windows.h>

bool game_done;
var story_enginesEnabled = 0;

void game_init()
{
    weapons_init();
    GIB_GlobalInit();
    ESELSLERCHE_GlobalInit();
    SPUTNIK_GlobalInit();
    SKULL_GlobalInit();
    EYE_GlobalInit();
    map_init();
}

void game_open()
{
    game_done = false;
    weapons_open();
    ESELSLERCHE_Init();
    SPUTNIK_Init();
    SKULL_Init();
    EYE_Init();
    TURRET_Init();
    hud_show();
    player_initSpawn();
    map_open();

    // Setup mouse
    mouse_mode = 1;
    mouse_range = 500;
    mouse_pointer = 0;
    vec_set(mouse_pos, screen_size);
    vec_scale(mouse_pos, 0.5);

    // reset story stuff
    story_enginesEnabled = 0;
    playerHasHazmat = 0;
    playerHasDoubleJump = 0;

    // activates cheats ingame
    input_cheats_enabled = 1;
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
    if(!weapons_disabled)
        weapons_update();
    projectiles_update();
    collectibles_update();
    keypad_update();
    doors_update();
    journals_update();

	environmentals_update();
    
	ESELSLERCHE_Update();
    SPUTNIK_Update();
    SKULL_Update();
    EYE_Update();
    TURRET_Update();
    GIB_Update();
	ENEMY_UpdateProjectile();
	
    hud_update();

    map_update();
	
    if(input_hit(INPUT_NAVBACK))
        game_done = true;
}

void game_close()
{
	pp_desync(0);
    movement_close();
	projectiles_close();
    hud_hide();
    weapons_close();
    map_close();
    mouse_pointer = 1;
    input_cheats_enabled = 0;
}

bool game_is_done()
{
    return game_done;
}
