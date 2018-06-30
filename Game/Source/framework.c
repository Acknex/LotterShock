#include "framework.h"
#include "splashscreen.h"
#include "mainmenu.h"

// TODO:
// -
// -

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
} framework_t;

framework_t framework;

//! Initialisiert das Spiel und so
void framework_init()
{
    video_set(1280, 720, 0, 2); // 1280x720, Window

    on_frame = framework_update;

    splashscreen_init();
    mainmenu_init();
}

void framework_setup(ENTITY * ent, int subsystem)
{
    if(ent == NULL) error("framework_setup mit NULL aufgerufen!");
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
        //framework_transfer(FRAMEWORK_STATE_SPLASHSCREEN);
        break;

    case FRAMEWORK_STATE_SPLASHSCREEN:
        splashscreen_update();
        if(splashscreen_is_done())
            framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    case FRAMEWORK_STATE_MAINMENU:
        mainmenu_init();
        break;

    case FRAMEWORK_STATE_CREDITS:
        error("credits not implemented yet.");
        break;

    case FRAMEWORK_STATE_LOAD:
        error("game load not implemented yet.");
        break;

    case FRAMEWORK_STATE_GAME:
        error("game not implemented yet.");
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
            error("credits not implemented yet.");
            break;

        case FRAMEWORK_STATE_LOAD:
            error("credits not implemented yet.");
            break;

        case FRAMEWORK_STATE_GAME:
            error("credits not implemented yet.");
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
            error("credits not implemented yet.");
            break;

        case FRAMEWORK_STATE_LOAD:
            error("game load not implemented yet.");
            break;

        case FRAMEWORK_STATE_GAME:
            error("game not implemented yet.");
            break;
        }
    }

    if(framework.state == FRAMEWORK_STATE_SHUTDOWN)
    {
        on_frame = NULL;
        sys_exit(NULL);
    }
}
