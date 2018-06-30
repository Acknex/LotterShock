#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"
#include "music_player.h"
#include "game.h"
#include "input.h"
#include "credits.h"
#include "hud.h"
#include "materials.h"

#define FRAMEWORK_ALPHA_BLENDSPEED  25

#define FRAMEWORK_STATE_SHUTDOWN    -1
#define FRAMEWORK_STATE_STARTUP      0
#define FRAMEWORK_STATE_MAINMENU     1
#define FRAMEWORK_STATE_CREDITS      2
#define FRAMEWORK_STATE_LOAD         3
#define FRAMEWORK_STATE_GAME         4
#define FRAMEWORK_STATE_SPLASHSCREEN 5
#define FRAMEWORK_STATE_UNLOAD       6

typedef struct
{
    int state;
    int nextState;
    int frameCounter;
    int loaderState;
} framework_t;

framework_t framework;

BMAP * framework_load_screen_bmap = "game_loadingscreen.png";

PANEL * framework_load_screen =
{
    bmap = framework_load_screen_bmap;
    size_x = 1280;
    size_y = 720;
    flags = TRANSLUCENT;
}

//! Initialisiert das Spiel und so
void framework_init()
{
    fps_max = 61;
    video_set(1600, 900, 0, 2); // 1280x720, Window
    
    SetupDefaultMaterials();

    on_frame = framework_update;
#ifdef DEBUG
    on_esc = NULL;
#endif
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

//! Aktualisiert alles.
void framework_update()
{
#ifdef DEBUG
    if(key_alt && key_f4)
        framework_transfer(FRAMEWORK_STATE_SHUTDOWN);
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

            // TODO: Fix relative link?
            music_start("Media/intro.mp3", 1.0, false);

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
        }
        hud_show();
        if(framework.loaderState >= 6)
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
        framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    case FRAMEWORK_STATE_UNLOAD:
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
            mainmenu_open();
            break;

        case FRAMEWORK_STATE_CREDITS:
            credits_open();
            break;

        case FRAMEWORK_STATE_LOAD:
            framework.loaderState = 0;
            set(framework_load_screen, SHOW);
            framework_load_screen->alpha = 100;
            break;

        case FRAMEWORK_STATE_GAME:
            game_open();
            break;

        case FRAMEWORK_STATE_UNLOAD:
            break;

        default:
            error(str_printf(NULL, "framework: unsupported state %d!", framework.state));
        }
    }

    // Update music after updating the whole game state
    music_update();

    if(framework.state == FRAMEWORK_STATE_SHUTDOWN)
    {
        on_frame = NULL;
        sys_exit(NULL);
    }

    framework.frameCounter += 1;
}
