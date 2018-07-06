#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"
#include "music_player.h"
#include "game.h"
#include "input.h"
#include "credits.h"
#include "hud.h"
#include "materials.h"
#include "meshFun.h"
#include "settings.h"
#include "bestiary.h"
#include "options.h"

#include <acknex.h>
#include <windows.h>

#define FRAMEWORK_ALPHA_BLENDSPEED  25

#define FRAMEWORK_STATE_SHUTDOWN    -1
#define FRAMEWORK_STATE_STARTUP      0
#define FRAMEWORK_STATE_MAINMENU     1
#define FRAMEWORK_STATE_CREDITS      2
#define FRAMEWORK_STATE_LOAD         3
#define FRAMEWORK_STATE_GAME         4
#define FRAMEWORK_STATE_SPLASHSCREEN 5
#define FRAMEWORK_STATE_UNLOAD       6
#define FRAMEWORK_STATE_BESTIARY     7

typedef struct
{
    int state;
    int nextState;
    int frameCounter;
    int loaderState;
} framework_t;

framework_t framework;

BMAP * framework_mouse_cursor = "cursor3.tga";

BMAP * framework_load_screen_bmap = "game_loadingscreen.png";

PANEL * framework_load_screen =
{
    bmap = framework_load_screen_bmap;
    size_x = 1280;
    size_y = 720;
    flags = TRANSLUCENT;
}

int framework_mousemode;

//! Initialisiert das Spiel und so
void framework_init()
{
    settings_init();
    settings_load();

    fps_min = 25; // overshoot vermeiden, v.a. wenn's ruckelt
    fps_max = settings.fps_limit;

    d3d_anisotropy = settings.anisotropy;
    d3d_triplebuffer = settings.vsync;

    particle_mode = 8;
    collision_mode = 2;
    preload_mode = 3; // preload a lot

    mouse_map = framework_mouse_cursor;

    video_set(
        settings.resolution_x,
        settings.resolution_y,
        0,
        2 - !!settings.fullscreen); // !! -> 0 oder 1
    
#ifndef FRAMEWORK_NO_POSTPROCESS
    SetupDefaultMaterials();
    SetupPostprocessing();
#endif

    on_frame = framework_update;
}

void framework_setup(ENTITY * ent, int subsystem)
{
    if(ent == NULL) error("framework: framework_setup mit NULL aufgerufen!");
    ent->SK_SUBSYSTEM = subsystem;
}

void framework_transfer(int state)
{
    framework.nextState = state;
}

void framework_cleanup()
{
    ENTITY * ent;
    ent = ent_next(NULL);
    while(ent)
    {
        you = ent;
        ent = ent_next(ent);
        if(you.SK_ENTITY_DEAD)
            ptr_remove(you);
    }
}

void framework_set_mousemode(int mode)
{
    switch(mode)
    {
    case MOUSEMODE_UI:
        // restore mouse cursor after game
        // mouse_mode 0 doesn't trigger mouse_ent and stuff
        mouse_map = framework_mouse_cursor;
        mouse_mode = 4;
        break;

    case MOUSEMODE_GAME:
        // remove mouse cursor in game
        // mouse_mode 0 doesn't trigger mouse_ent and stuff
        // so mouse_mode 1 is required
        mouse_map = NULL;
        mouse_mode = 1;
        mouse_range = 500;
        mouse_pointer = 0;

        vec_set(mouse_pos, screen_size);
        vec_scale(mouse_pos, 0.5);

        break;
    default:
        error("framework_set_mousemode: unknown mouse mode!");
    }
    framework_mousemode = mode;
}

void framework_capture_mouse()
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
}

//! Aktualisiert alles.
void framework_update()
{
#ifdef DEBUG
    if(key_alt && key_f4)
        framework_transfer(FRAMEWORK_STATE_SHUTDOWN);
#endif

#ifndef DEBUG_NO_CAPTURE
    if(window_focus && framework_mousemode == MOUSEMODE_GAME)
        framework_capture_mouse();
#endif

#ifdef DEBUG_NO_ESCAPE_QUIT
    on_esc = NULL;
#endif

    input_update();

    switch(framework.state)
    {
    case FRAMEWORK_STATE_STARTUP:
        if(framework.frameCounter == 1)
        {
            // spiel im ersten frame initialisieren
            input_init();

            splashscreen_init();
            mainmenu_init();
            music_init();
            game_init();
            credits_init();
            hud_init();
            bestiary_init();
            options_init();

            music_set_master_volume(settings.game_volume);

            // TODO: Fix relative link?
            music_start("Media/intro.mp3", 0.2, false);

            // Ladebildschirm passend skalieren
            framework_load_screen.scale_x = screen_size.x / framework_load_screen.size_x;
            framework_load_screen.scale_y = screen_size.y / framework_load_screen.size_y;

#ifdef DEBUG_FRAMEWORK_FASTSTART
            framework_transfer(FRAMEWORK_STATE_LOAD);
#else
            framework_transfer(FRAMEWORK_STATE_SPLASHSCREEN);
#endif
        }
        break;

    case FRAMEWORK_STATE_SPLASHSCREEN:
        splashscreen_update();
        if(splashscreen_is_done())
            framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    case FRAMEWORK_STATE_MAINMENU:
    {
        mainmenu_update();
        int response = mainmenu_get_response();
        if(response != MAINMENU_RESPONSE_STILLACTIVE)
        {
            switch(response)
            {
            case MAINMENU_RESPONSE_START:
                framework_transfer(FRAMEWORK_STATE_LOAD);
                break;
            case MAINMENU_RESPONSE_CREDITS:
                framework_transfer(FRAMEWORK_STATE_CREDITS);
                break;
            case MAINMENU_RESPONSE_EXIT:
                framework_transfer(FRAMEWORK_STATE_SHUTDOWN);
                break;
            case MAINMENU_RESPONSE_BESTIARY:
                framework_transfer(FRAMEWORK_STATE_BESTIARY);
                break;
            default:
                error("framework: unknown response from main menu.");
            }
        }
        break;
    }

    case FRAMEWORK_STATE_CREDITS:
        credits_update();
        if(credits_isdone())
            framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    case FRAMEWORK_STATE_LOAD:
        if(framework.loaderState == 3)
        {
#ifdef DEBUG_LEVEL
            level_load(DEBUG_LEVEL);
#else
            level_load(LEVEL_FILE);
#endif
			fog_color = 1;
			camera.fog_end = 6000.0;
			wait_for(level_load);
			meshFunDo();
			ENTITY *sky = ent_createlayer("sky_1+6.png", SHOW|CUBE|SKY, 100);
			bmap_to_cubemap(ent_getskin(sky, 0));
			
        }

        if(framework.loaderState == 6)
        {
            game_open();
        }
        
        if(framework.loaderState >= 9)
        {
            framework_load_screen->alpha -= FRAMEWORK_ALPHA_BLENDSPEED * time_step;
            if(framework_load_screen->alpha <= 0)
            {
                framework_load_screen->alpha = 0;
                framework_transfer(FRAMEWORK_STATE_GAME);
            }
        }
        framework.loaderState += 1;
        break;

    case FRAMEWORK_STATE_GAME:
        game_update();
        if(game_is_done())	
            framework_transfer(FRAMEWORK_STATE_UNLOAD);
        break;
    case FRAMEWORK_STATE_UNLOAD:
        level_load(NULL);
		music_start("Media/intro.mp3", 1.0, false);
        framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    case FRAMEWORK_STATE_UNLOAD:
        break;

    case FRAMEWORK_STATE_BESTIARY:
        bestiary_update();
        if(bestiary_is_done())
            framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    default:
        error(str_printf(NULL, "framework: unsupported state %d!", framework.state));

    }

    if(framework.state != framework.nextState)
    {
        diag(str_printf(NULL, "\nstate transition from %d to %d.", framework.state, framework.nextState));

        switch(framework.state)
        {
        case FRAMEWORK_STATE_SHUTDOWN:
            error("framework: shutdown state should never be left again!");
            break;

        case FRAMEWORK_STATE_STARTUP:
            break;

        case FRAMEWORK_STATE_SPLASHSCREEN:
            splashscreen_close();
            break;

        case FRAMEWORK_STATE_MAINMENU:
            mainmenu_close();
            break;

        case FRAMEWORK_STATE_CREDITS:
            credits_close();
            break;

        case FRAMEWORK_STATE_LOAD:
            reset(framework_load_screen, SHOW);
            break;

        case FRAMEWORK_STATE_GAME:
            game_close();
            break;

        case FRAMEWORK_STATE_UNLOAD:
            break;

        case FRAMEWORK_STATE_BESTIARY:
            bestiary_close();
            break;

        default:
            error(str_printf(NULL, "framework: unsupported state %d!", framework.state));
        }

        framework.state = framework.nextState;

        switch(framework.state)
        {
        case FRAMEWORK_STATE_SHUTDOWN:
            break;

        case FRAMEWORK_STATE_STARTUP:
            error("framework: startup state should never be entered again!");
            break;

        case FRAMEWORK_STATE_SPLASHSCREEN:
            splashscreen_open();
            break;

        case FRAMEWORK_STATE_MAINMENU:
            framework_set_mousemode(MOUSEMODE_UI);
            mainmenu_open();
            break;

        case FRAMEWORK_STATE_CREDITS:
            framework_set_mousemode(MOUSEMODE_GAME);
            credits_open();
            break;

        case FRAMEWORK_STATE_LOAD:
            framework_set_mousemode(MOUSEMODE_GAME);
            music_start("Media/background.mp3", 3, true);
            framework.loaderState = 0;
            set(framework_load_screen, SHOW);
            framework_load_screen->alpha = 100;


            break;

        case FRAMEWORK_STATE_GAME:
            // game was already openend
            // by LOAD state
            break;

        case FRAMEWORK_STATE_UNLOAD:
            break;

        case FRAMEWORK_STATE_BESTIARY:
            bestiary_open();
            break;

        default:
            error(str_printf(NULL, "framework: unsupported state %d!", framework.state));
        }
    }

    // Update music after updating the whole game state
    music_update();

    // Cleanup all dead entities
    framework_cleanup();

    if(framework.state == FRAMEWORK_STATE_SHUTDOWN)
    {
        on_frame = NULL;
        sys_exit(NULL);
    }

    framework.frameCounter += 1;
}
