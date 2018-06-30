#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"

// TODO:
// -
// -

#define FRAMEWORK_ALPHA_BLENDSPEED  25

#define FRAMEWORK_STATE_SHUTDOWN    -1
#define FRAMEWORK_STATE_STARTUP      0
#define FRAMEWORK_STATE_MAINMENU     1
#define FRAMEWORK_STATE_CREDITS      2
#define FRAMEWORK_STATE_LOAD         3
#define FRAMEWORK_STATE_GAME         4
#define FRAMEWORK_STATE_SPLASHSCREEN 5

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
    video_set(1280, 720, 0, 2); // 1280x720, Window

    framework_load_screen.size_x = screen_size.x / framework_load_screen.size_x;
    framework_load_screen.size_y = screen_size.y / framework_load_screen.size_y;

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

//! Aktualisiert alles.
void framework_update()
{
    if(key_esc)
        framework_transfer(FRAMEWORK_STATE_SHUTDOWN);

    switch(framework.state)
    {
    case FRAMEWORK_STATE_STARTUP:
        if(framework.frameCounter == 1)
        {
            // spiel im ersten frame initialisieren
            splashscreen_init();
            mainmenu_init();

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
        error("framework: credits not implemented yet.");
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
        else if(framework.loaderState > 3)
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
        error("framework: game not implemented yet.");
        break;
    }

    if(framework.state != framework.nextState)
    {
        switch(framework.state)
        {
        case FRAMEWORK_STATE_SPLASHSCREEN:
            splashscreen_close();
            break;

        case FRAMEWORK_STATE_MAINMENU:
            mainmenu_close();
            break;

        case FRAMEWORK_STATE_CREDITS:
            error("framework: credits not implemented yet.");
            break;

        case FRAMEWORK_STATE_LOAD:
            reset(framework_load_screen, SHOW);
            break;

        case FRAMEWORK_STATE_GAME:
            error("framework: credits not implemented yet.");
            break;
        }

        framework.state = framework.nextState;

        switch(framework.state)
        {
        case FRAMEWORK_STATE_SPLASHSCREEN:
            splashscreen_open();
            break;

        case FRAMEWORK_STATE_MAINMENU:
            mainmenu_open();
            break;

        case FRAMEWORK_STATE_CREDITS:
            error("framework: credits not implemented yet.");
            break;

        case FRAMEWORK_STATE_LOAD:
            framework.loaderState = 0;
            set(framework_load_screen, SHOW);
            framework_load_screen->alpha = 100;
            break;

        case FRAMEWORK_STATE_GAME:
            error("framework: game not implemented yet.");
            break;
        }
    }

    if(framework.state == FRAMEWORK_STATE_SHUTDOWN)
    {
        on_frame = NULL;
        sys_exit(NULL);
    }

    framework.frameCounter += 1;
}
