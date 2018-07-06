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
#include "journals.h"
#include "pausemenu.h"
#include "framework.h"

bool game_done;
var story_enginesEnabled = 0;

var game_ispaused;

void game_init()
{
    weapons_init();
    GIB_GlobalInit();
    ESELSLERCHE_GlobalInit();
    SPUTNIK_GlobalInit();
    SKULL_GlobalInit();
    EYE_GlobalInit();
    map_init();
    journals_init();
    pausemenu_init();
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

    // reset story stuff
    story_enginesEnabled = 0;
    playerHasHazmat = 0;
    playerHasDoubleJump = 0;

    // activates cheats ingame
    input_cheats_enabled = 1;

    game_ispaused = 0;
}

void game_update()
{
    if(game_ispaused)
    {
        pausemenu_update();

        switch(pausemenu_get_response())
        {
        case PAUSEMENU_RESPONSE_NONE: break;
        case PAUSEMENU_RESPONSE_CONTINUE:
            game_ispaused = false;
            pausemenu_close();
            framework_set_mousemode(MOUSEMODE_GAME);
            break;
        case PAUSEMENU_RESPONSE_QUIT:
            game_done = true;
            pausemenu_close();
            break;
        case PAUSEMENU_RESPONSE_OPTIONS:
            error("options not implemented yet!");
            break;
        }
    }
    else
    {
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
        {
            game_ispaused = true;
            pausemenu_open();
            framework_set_mousemode(MOUSEMODE_UI);
        }
    }
}

void game_close()
{
	pp_desync(0, 0);
    movement_close();
	projectiles_close();
    hud_hide();
    weapons_close();
    map_close();
    journals_quit();
    pausemenu_close();
    mouse_pointer = 1;
    input_cheats_enabled = 0;
}

bool game_is_done()
{
    return game_done;
}
