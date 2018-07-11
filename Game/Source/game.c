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
#include "environmentals.h"
#include "flesh.h"
#include "math.h"

#include <acknex.h>

bool game_done;

var game_ispaused;

bool game_hashud;

var game_iswon;

struct
{
    bool enabled;
    int stage;
    var progress;
    var speed;

    VECTOR pos_start, pos_end;
    ANGLE ang_start, ang_end;

} game_final_cutscene;

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
    FLESH_GlobalInit();

    game_hidehud = 0;
    game_hashud = 1;
}

void game_open()
{
#ifdef DEBUG_PEACEFUL
    for(you = ent_next(NULL); you != NULL; you = ent_next(you))
    {
        if(you->SK_SUBSYSTEM >= 1000)
            you->SK_ENTITY_DEAD = 1;
    }
#endif

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
    keys_reset();

    // reset story stuff
    story_enginesEnabled = 0;
    story_powercoreEnabled = 0;
    story_serverRoomState = 0;
    playerHasHazmat = 0;
    playerHasDoubleJump = 0;
    story_hasBattery = 0;

    game_final_cutscene.enabled = false;

    // activates cheats ingame
    input_cheats_enabled = 1;

    // disable the matrix and crt effect:
    materials_matrix_str = 0;
    materials_crt_str = 0;

    game_ispaused = 0;

    game_iswon = 0;

    game_hashud = true;
}

void game_set_complete()
{
    if(game_final_cutscene.enabled)
        return;

    game_final_cutscene.enabled = true;
    game_final_cutscene.stage = 0;
    game_final_cutscene.progress = 0;
    game_final_cutscene.speed = 5;

    game_hidehud = true;

    vec_set(game_final_cutscene.pos_start, camera->x);
    vec_set(game_final_cutscene.ang_start, camera->pan);

    vec_set(game_final_cutscene.pos_end, vector(11752, 3844, -176));
    vec_set(game_final_cutscene.ang_end, vector(283, -6, 0));

    set(player, INVISIBLE);
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
            journals_resume();
            framework_set_mousemode(MOUSEMODE_GAME);
            break;
        case PAUSEMENU_RESPONSE_QUIT:
            game_done = true;
            pausemenu_close();
            break;
        default:
            error("game_update: invalid response from pausemenu!");
        }
    }
    else
    {
        if(game_hashud != !game_hidehud)
        {
            game_hashud = !game_hidehud;
            if(game_hashud)
            {
                hud_show();
                weapons_open();
            }
            else
            {
                hud_hide();
                weapons_close();
            }
        }

#ifdef DEBUG
        if(key_k)
        {
            story_serverRoomState = 3;
            beep();
        }
#endif

        if(game_final_cutscene.enabled)
        {
            switch(game_final_cutscene.stage)
            {
            case 0: // Current camera pos to sit position
                vec_lerp(camera->x, game_final_cutscene.pos_start, game_final_cutscene.pos_end, game_final_cutscene.progress / 100.0);
                ang_lerp(camera->pan, game_final_cutscene.ang_start, game_final_cutscene.ang_end, game_final_cutscene.progress / 100.0);

                if(game_final_cutscene.progress == 100)
                {
                    game_final_cutscene.stage = 1;
                    game_final_cutscene.speed = 10;
                    game_final_cutscene.progress = 0;
                }
                break;

            case 1:
                if(game_final_cutscene.progress == 100)
                {
                    vec_set(game_final_cutscene.pos_start, game_final_cutscene.pos_end);
                    vec_set(game_final_cutscene.ang_start, game_final_cutscene.ang_end);
                    vec_set(game_final_cutscene.pos_end, vector(11842, 3939, -251));
                    vec_set(game_final_cutscene.ang_end, vector(220, 13, 0));
                    game_final_cutscene.stage = 2;
                    game_final_cutscene.speed = 5;
                    game_final_cutscene.progress = 0;
                }
                break;

            case 2: // Sit position to laydown

                vec_lerp(camera->x, game_final_cutscene.pos_start, game_final_cutscene.pos_end, game_final_cutscene.progress / 100.0);
                ang_lerp(camera->pan, game_final_cutscene.ang_start, game_final_cutscene.ang_end, game_final_cutscene.progress / 100.0);

                if(game_final_cutscene.progress == 100)
                {
                    game_final_cutscene.stage = 20;
                    game_final_cutscene.progress = 0;
                    game_final_cutscene.speed = 4;
                }
                break;

            case 20: // delay some bit after laydown (let the modem dial up)
                if(game_final_cutscene.progress == 100)
                {
                    game_final_cutscene.stage = 3;
                    game_final_cutscene.progress = 0;
                    game_final_cutscene.speed = 1;
                }
                break;

            case 3: // enter the matrix
                materials_matrix_str += 0.01 * time_step;
                if(materials_matrix_str >= 2.3)
                {
                    game_final_cutscene.stage = 4;
                    beep(); // TODO: Play laughing here!
                }
                break;

            case 4: // in the matrix, after laughing

                materials_matrix_str += 0.01 * time_step;

                if(materials_matrix_str >= 2.8)
                {
                    game_final_cutscene.stage = 5;
                    game_final_cutscene.progress = 0;
                    game_final_cutscene.speed = 2;
                }
                break;

            case 5: // lights-off-effect

                materials_crt_str = clamp(materials_crt_str + time_step, 0, 15);

                if(game_final_cutscene.progress == 100)
                {
                    game_final_cutscene.stage = 6;
                }

                break;

            case 6: // game done, roll credits
                game_iswon = true;
                game_done = true;
                break;
            }

            game_final_cutscene.progress = clamp(
                game_final_cutscene.progress + game_final_cutscene.speed * time_step,
                0,
                100);
        }
        else
        {
            movement_update();
            if(!weapons_disabled && game_hashud)
                weapons_update();
            projectiles_update();
            collectibles_update();
            keypad_update();
            doors_update();

            environmentals_update();

            ESELSLERCHE_Update();
            SPUTNIK_Update();
            SKULL_Update();
            EYE_Update();
            TURRET_Update();
            GIB_Update();
            ENEMY_UpdateProjectile();
            FLESH_Update();

            if(player && (story_serverRoomState == 0))
            {
                if(region_check(REGION_SERVERROOM, &player->x, &player->x))
                {
                    story_serverRoomState = 1;
                    journals_play(37, JOURNAL_LEVEL_STORY);
                }
            }


            journals_update();

            if(game_hashud)
                hud_update();

            map_update();
        }

        if(input_hit(INPUT_NAVBACK) || (window_focus == 0))
        {
            game_ispaused = true;
            journals_pause();
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

    if(game_hashud)
    {
        hud_hide();
        weapons_close();
    }
    map_close();
    journals_quit();
    pausemenu_close();
    environmentals_close();

    level_load(NULL); // unload the level so we can't see shit

    mouse_pointer = 1;
    materials_matrix_str = 0;
    materials_crt_str = 0;
    input_cheats_enabled = 0;
}

bool game_is_done()
{
    return game_done;
}

bool game_is_won()
{
    return game_iswon;
}
